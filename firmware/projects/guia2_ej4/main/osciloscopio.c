/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/

/** @brief Tamano de buffer ECG */
#define BUFFER_SIZE 231

/** @brief Frecuencia de muestreo de señal analógica */
#define FRECUENCIA_MUESTREO 500

/*==================[internal data definition]===============================*/

/** @brief Handler que maneja la tarea asociada al muestreo */
TaskHandle_t handlerMuestreo;

/** @brief Handler que maneja la tarea asociada al muestreo */
TaskHandle_t handlerEnvioECG;

/** @brief Periodo de interrupcion del timer de muestreo */
uint16_t periodoTimerMuestreo = 2000; // T=(1/500Hz)*1000000

/** @brief Periodo de interrupcion del timer de envio*/
uint16_t periodoTimerEnvio = 4000; // Mitad de perido de muestreo

/** @brief Muestras de ECG */
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/** @brief Indice que intera sobre las muestras del ecg */
uint16_t indiceECG = 0;


/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que maneja la interrupción del timer A
 */
void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerMuestreo, pdFALSE); 
}
/**
 * @brief Función que maneja la interrupción del timer B
 */
void FuncTimerB(void* param)
{
	vTaskNotifyGiveFromISR(handlerEnvioECG, pdFALSE);
}
/**
 * @brief Tarea que muestrea la señal que ingresa en el canal 1
 *
 * @param pvParameter parámetro utilizado por freeRTOS
 */

static void muestrearTask(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		uint16_t voltaje = 0;
		AnalogInputReadSingle(CH1, &voltaje);
		UartSendString(UART_PC, ">tension:");
		UartSendString(UART_PC, (const char*)UartItoa(voltaje,10));
		UartSendString(UART_PC, " \r\n");
	}
}
/**
 * @brief Tarea que envia la conversion analogica al canal 0.
 *
 * @param pvParameter parámetro utilizado por freeRTOS
 */

static void enviarTask(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(ecg[indiceECG]);
		indiceECG++;
		if(indiceECG >= BUFFER_SIZE){
            indiceECG = 0;
        }
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){

	timer_config_t timerMuestreo = {
        .timer = TIMER_A,
        .period = periodoTimerMuestreo,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
	timer_config_t timerEnvio = {
        .timer = TIMER_B,
        .period = periodoTimerMuestreo,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
	
	analog_input_config_t configAnalog = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL, 
		.param_p = NULL
	};
	

	serial_config_t puertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};
	
    UartInit(&puertoSerie);
	TimerInit(&timerMuestreo); 
	TimerInit(&timerEnvio);
	
	AnalogInputInit(&configAnalog);
	AnalogOutputInit();
	xTaskCreate(&muestrearTask, "taskMuestrear", 2048, NULL, 5, &handlerMuestreo);
	xTaskCreate(&enviarTask, "taskEnvioECG", 2048, NULL, 5, &handlerEnvioECG);
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
	
	

}
/*==================[end of file]============================================*/