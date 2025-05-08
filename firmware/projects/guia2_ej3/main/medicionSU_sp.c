/*! @mainpage Procesamiento de señal de salida de sensor ultrasonico HCSR04 implementando interrupciones
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
 * Este enfoque facilita el diseño y mejora la eficiencia mediante el uso de interrupciones, principal diferencia con el proyecto anterior. 
 * Además, permite al usuario interactuar con el sistema para pausar la visualización o desactivar la medición del sensor según se requiera, con las respectivas interrupciones.
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
#include "timer_mcu.h"
#include "uart_mcu.h"

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
 * @def PROFUNDIDAD_STACK_TASK_VISUALIZACION
 * @brief Tamaño del stack para la tarea de visualización.
 */
#define PROFUNDIDAD_STACK_TASK_VISUALIZACION 2048

/**
 * @def PRIORIDAD_TASK_VISUALIZACION
 * @brief Prioridad de la tarea de visualización.
 */
#define PRIORIDAD_TASK_VISUALIZACION 5

/**
 * @def PERIODO_MS_VISUALIZAR
 * @brief Período en ms del timer asociado a la visualización.
 */
#define PERIODO_MS_VISUALIZAR 1000000

/*==================[internal data definition]===============================*/
/**
 * @brief Handle de la tarea de visualización.
 */
TaskHandle_t visualizarTaskHandle = NULL;

/**
 * @brief Flag para habilitar o deshabilitar la medición.
 */
bool medicionActivada = false; 

/**
 * @brief Flag para congelar el valor mostrado en el display.
 */
bool holdMedicion = false;

/**
 * @brief Última medición obtenida del sensor.
 */
uint16_t medicionSensor;

/**
 * @brief Valor retenido para visualización congelada.
 */
uint16_t medicionHold;

/*==================[internal functions declaration]=========================*/

/**
 * @brief Enciende los LEDs en función del valor de distancia medido.
 *
 * @param medicion Distancia en centímetros obtenida del sensor.
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
 * @brief Tarea encargada de realizar la medición y actualizar la visualización.
 *
 * Esta tarea es notificada por un timer (usando interrupciones).
 * Si la medición está activada, lee del sensor HC-SR04, enciende los LEDs
 * según el valor leído, y escribe la medición (o valor retenido) en el display LCD.
 *
 * @param pvParameter Parámetro no utilizado.
 */
static void visualizarTask(void *pvParameter){
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        if (medicionActivada) {
            medicionSensor = HcSr04ReadDistanceInCentimeters();
            encenderLedsSegunMedicion(medicionSensor);
            if (holdMedicion) {
                LcdItsE0803Write(medicionHold);
				UartSendString(UART_PC, (char*)UartItoa(medicionHold, 10));
				UartSendString(UART_PC, " cm\r\n");
            } else {
                LcdItsE0803Write(medicionSensor);  
				UartSendString(UART_PC, (char*)UartItoa(medicionSensor, 10));
				UartSendString(UART_PC, " cm\r\n");
            }    
        } else {
            LcdItsE0803Off();
        }
    }
}

/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(visualizarTaskHandle, pdFALSE);    
}

/**
 * @brief Función invocada por interrupción del SWITCH_1.
 *
 * Activa o desactiva la medición de distancia.
 */
void FuncTecla1() {
    medicionActivada = !medicionActivada;
}

/**
 * @brief Función invocada por interrupción del SWITCH_2.
 *
 * Congela o libera la visualización del valor actual en el display.
 */
void FuncTecla2() {
    holdMedicion = !holdMedicion;
    if (holdMedicion == true) {
        medicionHold = medicionSensor;
    }
}

void callbackUART(void *param) {
    uint8_t recibido;
    if (UartReadByte(UART_PC, &recibido)) {
        if (recibido == 'o') {
            FuncTecla1();
        } else if (recibido == 'h') {
            FuncTecla2(); 
        }
    }
}


/*==================[external functions definition]==========================*/
/**
 * @brief Función principal del programa.
 *
 * Inicializa periféricos, configura interrupciones de teclas y timer,
 * crea la tarea de visualización y lanza el timer que la activa periódicamente.
 */
void app_main(void){
    // Inicializacion de perifericos
    LedsInit();
    SwitchesInit();
    SwitchActivInt(SWITCH_1, FuncTecla1, 0);
    SwitchActivInt(SWITCH_2, FuncTecla2, 0);
    HcSr04Init(GPIO_3,GPIO_2);
    LcdItsE0803Init();

	serial_config_t uart_config = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = callbackUART,
		.param_p = NULL
	};
	UartInit(&uart_config);
	
	
	timer_config_t timerVisualizar = {
        .timer = TIMER_A,
        .period = PERIODO_MS_VISUALIZAR,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timerVisualizar);
    xTaskCreate(&visualizarTask, "visualizarTask", PROFUNDIDAD_STACK_TASK_VISUALIZACION, NULL, PRIORIDAD_TASK_VISUALIZACION, &visualizarTaskHandle);
    TimerStart(timerVisualizar.timer);
     
}
