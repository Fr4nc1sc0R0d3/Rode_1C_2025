/*! @mainpage Procesamiento de señal de salida de sensor ultrasonico HCSR04 implementando tareas
 *
 * @section descripcionGeneral Descripcion general
 *
 * Este proyecto utiliza el sensor ultrasónico HC-SR04 para medir distancias.
 * Las mediciones se procesan y se visualizan en un display LCD, mientras que tres LEDs
 * indican visualmente el rango de distancia detectado. El sistema cuenta con tres tareas:
 * 
 * - Una tarea para realizar la medición con el sensor ultrasónico.
 * - Una tarea para mostrar los resultados en el display LCD.
 * - Una tarea para la lectura de teclas que permite activar/desactivar la medición y congelar el valor mostrado.
 *
 * Este enfoque esta basado en el uso de tareas concurrentes, las cuales responden segun el tiempo asignado que tienen. 
 * Además, permite al usuario interactuar con el sistema para pausar la visualización o desactivar la medición del sensor según se requiera.
 * 
 * 
 * @author Francisco Rode (francisco.rode@ingenieria.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpio_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include <hc_sr04.h>
#include "lcditse0803.h"

/*==================[macros and definitions]=================================*/
/**
 * @def MEDICION_INFERIOR
 * @brief Umbral inferior de distancia en centímetros para el encendido de LEDs.
 */
#define MEDICION_INFERIOR 10

/**
 * @def MEDICION_INTERMEDIA
 * @brief Umbral intermedio de distancia en centímetros para el encendido de LEDs.
 */
#define MEDICION_INTERMEDIA 20

/**
 * @def MEDICION_SUPERIOR
 * @brief Umbral superior de distancia en centímetros para el encendido de LEDs.
 */
#define MEDICION_SUPERIOR 30

/**
 * @def DELAY_MS_TASK_MEDICION
 * @brief Retardo en milisegundos entre cada medición del sensor.
 */
#define DELAY_MS_TASK_MEDICION 1000

/**
 * @def DELAY_MS_TASK_VISUALIZACION
 * @brief Retardo en milisegundos entre cada actualización del display.
 */
#define DELAY_MS_TASK_VISUALIZACION 1000

/**
 * @def DELAY_MS_TASK_TECLAS
 * @brief Retardo en milisegundos entre cada lectura de teclas.
 */
#define DELAY_MS_TASK_TECLAS 100

/**
 * @def PROFUNDIDAD_STACK_TASK_MEDICION
 * @brief Tamaño del stack para la tarea de medición.
 */
#define PROFUNDIDAD_STACK_TASK_MEDICION 2048

/**
 * @def PROFUNDIDAD_STACK_TASK_VISUALIZACION
 * @brief Tamaño del stack para la tarea de visualización.
 */
#define PROFUNDIDAD_STACK_TASK_VISUALIZACION 2048

/**
 * @def PROFUNDIDAD_STACK_TASK_TECLAS
 * @brief Tamaño del stack para la tarea de teclas.
 */
#define PROFUNDIDAD_STACK_TASK_TECLAS 2048

/**
 * @def PRIORIDAD_TASK_MEDICION
 * @brief Prioridad de la tarea de medición.
 */
#define PRIORIDAD_TASK_MEDICION 5

/**
 * @def PRIORIDAD_TASK_VISUALIZACION
 * @brief Prioridad de la tarea de visualización.
 */
#define PRIORIDAD_TASK_VISUALIZACION 5

/**
 * @def PRIORIDAD_TASK_TECLAS
 * @brief Prioridad de la tarea de lectura de teclas.
 */
#define PRIORIDAD_TASK_TECLAS 5

/*==================[internal data definition]===============================*/
/** @brief Handle de la tarea de visualización. */
TaskHandle_t visualizarTaskHandle = NULL;

/** @brief Handle de la tarea de medición. */
TaskHandle_t medirTaskHandle = NULL;

/** @brief Handle de la tarea de lectura de teclas. */
TaskHandle_t teclasTaskHandle = NULL;

/** @brief Flag para habilitar o deshabilitar la medición. */
bool medicionActivada = false; 

/** @brief Flag para habilitar la visualización retenida. */
bool visualizacionActivada = false;

/** @brief Flag para congelar el valor mostrado. */
bool holdMedicion = false;

/** @brief Última medición obtenida del sensor. */
uint16_t medicionSensor;

/** @brief Valor retenido de la medición para visualización congelada. */
uint16_t medicionHold;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Controla el encendido de los LEDs en función del valor de medición.
 *
 * @param medicion Valor de distancia en centímetros leída del sensor HC-SR04.
 */
void encenderLedsSegunMedicion(uint16_t medicion) {
    if (medicion < MEDICION_INFERIOR) {
        LedOff(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    } 
    else if (medicion > MEDICION_INFERIOR && medicion < MEDICION_INTERMEDIA) {
        LedOn(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    }
    else if (medicion > MEDICION_INTERMEDIA && medicion < MEDICION_SUPERIOR) {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOff(LED_3);
    }

    else {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOn(LED_3);
    }
}

/**
 * @brief Tarea que realiza la medición de distancia usando el sensor HC-SR04.
 *
 * La tarea se ejecuta cada 1 segundo si la medición está activada.
 *
 * @param pvParameter Parámetro no utilizado.
 */
static void medirTask(void *pvParameter){
    while(true){
        if (medicionActivada) {
            medicionSensor = HcSr04ReadDistanceInCentimeters();
            encenderLedsSegunMedicion(medicionSensor);
        }
        vTaskDelay(DELAY_MS_TASK_MEDICION/portTICK_PERIOD_MS);
        
    }
}

/**
 * @brief Tarea que visualiza el valor de distancia en el display LCD.
 *
 * Si la opción de retención está activa, se muestra el valor retenido.
 * De lo contrario, se muestra la última medición. Si la medición está
 * desactivada, el display se apaga.
 *
 * @param pvParameter Parámetro no utilizado.
 */
static void visualizarTask(void *pvParameter){
    while(true){
        if (medicionActivada) {
            if (holdMedicion) {
                LcdItsE0803Write(medicionHold);
            } else {
                LcdItsE0803Write(medicionSensor);  
            }    
        } else {
            LcdItsE0803Off();
        }
        vTaskDelay(DELAY_MS_TASK_VISUALIZACION/portTICK_PERIOD_MS);
    }
}

/**
 * @brief Tarea que gestiona la lectura de las teclas.
 *
 * - SWITCH_1: Activa o desactiva la medición.
 * - SWITCH_2: Congela o libera la visualización del valor actual.
 *
 * @param pvParameter Parámetro no utilizado.
 */
static void teclasTask(void *pvParameter){
    while(true){
        switch(SwitchesRead()) {
            case SWITCH_1:
                medicionActivada = !medicionActivada;
                break;
            case SWITCH_2:
                holdMedicion = !holdMedicion;
                if (holdMedicion == true) {
                    medicionHold = medicionSensor;
                }
                break;
        }
        vTaskDelay(DELAY_MS_TASK_TECLAS/portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
/**
 * @brief Función principal de la aplicación.
 *
 * Inicializa los periféricos y crea las tareas para medición, visualización y manejo de teclas.
 */
void app_main(void){

    // Inicializacion de perifericos
    LedsInit();
    SwitchesInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3,GPIO_2);

    // Creacion de tareas.
    xTaskCreate(&visualizarTask, "visualizarTask", PROFUNDIDAD_STACK_TASK_VISUALIZACION, NULL, PRIORIDAD_TASK_VISUALIZACION, &visualizarTaskHandle);
    xTaskCreate(&medirTask, "medirTask", PROFUNDIDAD_STACK_TASK_MEDICION, NULL, PRIORIDAD_TASK_MEDICION, &medirTaskHandle);
    xTaskCreate(&teclasTask, "teclasTask", PROFUNDIDAD_STACK_TASK_TECLAS, NULL, PRIORIDAD_TASK_TECLAS, &teclasTaskHandle);
}
