/*! @mainpage Proyecto final electronica programable - Vincha ultrasonica para personas con discapacidad visual
 *
 * @section genDesc Descripcion general
 *
 * Este proyecto implementa una vincha ultrasónica asistiva para personas con discapacidad visual,
 * diseñada para detectar obstáculos en el entorno y proporcionar retroalimentación táctil mediante
 * motores vibradores. El sistema utiliza tres sensores ultrasónicos HC-SR04 distribuidos 
 * estratégicamente para cubrir diferentes zonas de detección: un sensor frontal con mayor 
 * sensibilidad (umbral de 60cm) y dos sensores ubicados inferior al anterior, con alcance extendido (umbral de 115cm).
 * 
 * Cuando se detectan obstáculos dentro de los umbrales establecidos, el sistema activa motores
 * vibradores específicos ubicados en diferentes posiciones de la vincha, permitiendo al usuario
 * identificar la dirección y proximidad de los obstáculos. El dispositivo opera de forma 
 * autónoma utilizando FreeRTOS para la gestión de tareas concurrentes, realizando mediciones
 * cada 500ms y actualizando los actuadores cada 250ms para una respuesta rápida y eficiente.
 *
 * <a href="https://drive.google.com/file/d/1H7Z_4weVHPXkyW7RTqY9Dbl9-jrLwNHd/view?usp=sharing">Ver demostracion practica</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    HC-SR04 (1)   |   ESP32   	|
 * |:----------------:|:------------|
 * | 	VCC	 	      | 	5V		|
 * | 	GND	 	      | 	GND		|
 * | 	ECHO	 	  | 	GPIO 3	|
 * | 	TRIGGER	 	  | 	GPIO 2	|
 * 
 * |    HC-SR04 (2)   |   ESP32   	|
 * |:----------------:|:------------|
 * | 	VCC	 	      | 	5V		|
 * | 	GND	 	      | 	GND		|
 * | 	ECHO	 	  | 	GPIO 21	|
 * | 	TRIGGER	 	  | 	GPIO 22	|
 * 
 * |    HC-SR04 (3)   |   ESP32   	|
 * |:----------------:|:------------|
 * | 	VCC	 	      | 	5V		|
 * | 	GND	 	      | 	GND		|
 * | 	ECHO	 	  | 	GPIO 19	|
 * | 	TRIGGER	 	  | 	GPIO 23	|	
 * 
 * |    BUZZER        |   ESP32   	|
 * |:----------------:|:------------|
 * | 	VCC	 	      | 	GPIO 18	|
 * | 	GND	 	      | 	GND		|
 * 
 * |    MOTOR VIBRADOR (1)        |   ESP32   	|
 * |:----------------------------:|:------------|
 * | 	VCC	 	                  | 	3,3 V	|
 * | 	GND	 	                  | 	GND		|        
 * | 	IN	 	                  | 	GPIO_20	|     
 * 
 * |    MOTOR VIBRADOR (2)        |   ESP32   	|
 * |:----------------------------:|:------------|
 * | 	VCC	 	                  | 	3,3 V	|
 * | 	GND	 	                  | 	GND		|        
 * | 	IN	 	                  | 	GPIO_12	|     
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <hc_sr04_multiple_control.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "buzzer.h"
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/**
 * @brief Array que almacena las mediciones de los tres sensores ultrasónicos
 * 
 * Índice 0: Sensor frontal superior
 * Índice 1: Sensor frontal inferior izquierdo  
 * Índice 2: Sensor frontal inferior derecho
 */
uint16_t medicionesSensores[3] = {0};

/**
 * @brief Handle de la tarea de medición y control de actuadores
 */
TaskHandle_t handlerMedicionYControl;

/**
 * @brief Handle de la tarea de debug por puerto serie
 */
TaskHandle_t handlerDebug;

/**
 * @brief Umbral superior de detección para el sensor frontal (en cm)
 * 
 * Cuando el sensor frontal detecta un obstáculo a esta distancia o menor,
 * se activa la retroalimentación correspondiente.
 */
uint16_t umbralSuperior = 60;

/**
 * @brief Umbral inferior de detección para sensores laterales (en cm)
 * 
 * Cuando los sensores laterales detectan un obstáculo a esta distancia o menor,
 * se activa la retroalimentación correspondiente.
 */
uint16_t umbralInferior = 115;

/*==================[internal functions declaration]=========================*/

/**
 * @brief Función de callback del Timer A
 * 
 * Esta función se ejecuta cada vez que vence el Timer A y notifica
 * a la tarea de medición y control para que ejecute una nueva iteración.
 * 
 * @param param Parámetro no utilizado
 */
void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerMedicionYControl, pdFALSE);
}

/**
 * @brief Función de callback del Timer B
 * 
 * Esta función se ejecuta cada vez que vence el Timer B y notifica
 * a la tarea de debug para que envíe información por puerto serie.
 * 
 * @param param Parámetro no utilizado
 */
void FuncTimerB(void* param)
{
	vTaskNotifyGiveFromISR(handlerDebug, pdFALSE);
}

/**
 * @brief Envía información de debug por puerto serie
 * 
 * Esta función envía las mediciones actuales de los tres sensores
 * ultrasónicos a través del puerto serie para propósitos de debug
 * y monitoreo del sistema.
 */
void debugPorPuertoSerie() {
    for (int i = 0; i < 3; i ++) {
        switch (i) {
            case 0: UartSendString(UART_PC, "Medición sensor 1: "); break;
            case 1: UartSendString(UART_PC, "Medición sensor 2: "); break;
            case 2: UartSendString(UART_PC, "Medición sensor 3: "); break;
        }
        UartSendString(UART_PC, (char*)UartItoa(medicionesSensores[i],10));
        UartSendString(UART_PC, " cm\r\n");
    }  
}

/**
 * @brief Controla el buzzer y motores vibradores según las mediciones
 * 
 * Esta función analiza las mediciones de los sensores ultrasónicos
 * y activa los actuadores correspondientes (buzzer y motores vibradores)
 * cuando se detectan obstáculos dentro de los umbrales establecidos.
 * 
 * Lógica de control:
 * - Sensor 0 (frontal): umbral de 60cm, activa motor vibrador en GPIO_20 y buzzer
 * - Sensores 1-2 (laterales): umbral de 115cm, activa motor vibrador en GPIO_12 y buzzer
 * - El buzzer (GPIO_18) se activa junto con cualquier motor vibrador
 */
void controlarBuzzerYMotores() {
    bool seSuperoUmbralSuperior = false;
    bool seSuperoUmbralInferior = false;

    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            if (medicionesSensores[i] <= umbralSuperior) {
                seSuperoUmbralInferior = true;
                break;
            }
        } else {
            if (medicionesSensores[i] <= umbralInferior) {
                seSuperoUmbralSuperior = true;
                break;
            }
        }
    }
    
    if (seSuperoUmbralSuperior) {
        GPIOOn(GPIO_12);
        GPIOOn(GPIO_18);
    } else {
        GPIOOff(GPIO_12);
    }

    if (seSuperoUmbralInferior) {
        GPIOOn(GPIO_20);
        GPIOOn(GPIO_18);
    } else {
        GPIOOff(GPIO_20);
    }

    if (!seSuperoUmbralInferior && !seSuperoUmbralSuperior) {
        GPIOOff(GPIO_18);
        GPIOOff(GPIO_12);
        GPIOOff(GPIO_20);
    }
}

/**
 * @brief Tarea principal de medición y control
 * 
 * Esta tarea se ejecuta periódicamente (cada 500ms) y realiza las siguientes acciones:
 * 1. Obtiene las mediciones de los tres sensores ultrasónicos
 * 2. Ejecuta la lógica de control de actuadores
 * 
 * La tarea permanece bloqueada hasta recibir una notificación del Timer A.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado)
 */
static void medicionYControlTask(void *pvParameter) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        obtenerMediciones(medicionesSensores);
        controlarBuzzerYMotores();
    }
}

/**
 * @brief Tarea de debug por puerto serie
 * 
 * Esta tarea se ejecuta periódicamente (cada 250ms) y envía información
 * de debug con las mediciones actuales de los sensores a través del puerto serie.
 * 
 * La tarea permanece bloqueada hasta recibir una notificación del Timer B.
 * 
 * @param pvParameter Parámetro de la tarea (no utilizado)
 */
static void debugTask(void *pvParameter) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        debugPorPuertoSerie();
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal de la aplicación
 * 
 * Esta función inicializa todos los componentes del sistema:
 * - Configuración del puerto serie para debug
 * - Configuración de timers para tareas periódicas
 * - Inicialización de GPIOs para actuadores
 * - Creación de tareas de FreeRTOS
 * - Inicio de timers
 * 
 * Configuración de timers:
 * - Timer A: 500ms para medición y control
 * - Timer B: 250ms para debug
 * 
 * GPIOs utilizados:
 * - GPIO_18: Buzzer
 * - GPIO_20: Motor vibrador 1
 * - GPIO_12: Motor vibrador 2
 */
void app_main(void){

	serial_config_t puertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};

	timer_config_t timerMedicion = {
        .timer = TIMER_A,
        .period = 500000,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    timer_config_t timerBuzzerYMotores = {
        .timer = TIMER_B,
        .period = 250000,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

    GPIOInit(GPIO_18, GPIO_OUTPUT);
    GPIOInit(GPIO_20, GPIO_OUTPUT);
    GPIOInit(GPIO_12, GPIO_OUTPUT);
	UartInit(&puertoSerie);
	TimerInit(&timerMedicion);
    TimerInit(&timerBuzzerYMotores);

    xTaskCreate(&medicionYControlTask, "MedirDistanciasYControl", 8192, NULL, 5, &handlerMedicionYControl);
    xTaskCreate(&debugTask, "debugPuertoSerie", 8192, NULL, 5, &handlerDebug);

    TimerStart(TIMER_A);
    TimerStart(TIMER_B);
}

/*==================[end of file]============================================*/