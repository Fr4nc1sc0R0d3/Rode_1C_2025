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

/** @brief Frecuencia de muestreo de señal analógica */
#define FRECUENCIA_MUESTREO 500

/*==================[internal data definition]===============================*/

/** @brief Handler que maneja la tarea asociada al muestreo */
TaskHandle_t handlerMuestreo;

/** @brief Voltaje medido */
uint16_t voltaje = 0;


/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que maneja la interrupción del timer A
 */
void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerMuestreo, pdFALSE); 
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
		AnalogInputReadSingle(CH1, &voltaje);
		UartSendString(UART_PC, ">tension:");
		UartSendString(UART_PC, (const char*)UartItoa(voltaje,10));
		UartSendString(UART_PC, " \r\n");
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){

	timer_config_t TimerA = {
        .timer = TIMER_A,
        .period = (1/FRECUENCIA_MUESTREO),
        .func_p = FuncTimerA,
        .param_p = NULL
    };
	

	analog_input_config_t configAnalog = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL, 
		.param_p = NULL,
		.sample_frec =NULL
	};
	

	serial_config_t puertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};
	


	AnalogInputInit(&configAnalog);
	//AnalogOutputInit();
	UartInit(&puertoSerie);
	TimerInit(&TimerA);
	xTaskCreate(&muestrearTask, "taskMuestrear", 2048, NULL, 5, &handlerMuestreo);
	TimerStart(TimerA.timer);

}
/*==================[end of file]============================================*/