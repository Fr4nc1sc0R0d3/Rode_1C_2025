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
#include <hc_sr04_multiple_control.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include <task.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
/*==================[macros and definitions]=================================*/

#define PERIODO_INTERRUPCION_US 500000 // cada 0,5 seg

/*==================[internal data definition]===============================*/

HCSR04 vectorSensores[4] = {{GPIO_20, GPIO_16},{GPIO_19, GPIO_17},{GPIO_18,GPIO_15},{GPIO_3,GPIO_13}};
TaskHandle_t handlerMedicion;

/*==================[internal functions declaration]=========================*/

void FuncTimer(void* param)
{
	vTaskNotifyGiveFromISR(handlerMedicion, pdFALSE);
}

static void medirTask(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		medicionesHCSRO4 mediciones = obtenerMediciones(vectorSensores);
	
		UartSendString(UART_PC, "Medicion sensor 1: " + mediciones.s1);
		UartSendString(UART_PC, "Medicion sensor 2: " + mediciones.s2);
		UartSendString(UART_PC, "Medicion sensor 3: " + mediciones.s3);
		UartSendString(UART_PC, "Medicion sensor 4: " + mediciones.s4);
		
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	serial_config_t puertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};

	timer_config_t timerMedicion = {
        .timer = TIMER_A,
        .period = PERIODO_INTERRUPCION_US,
        .func_p = FuncTimer,
        .param_p = NULL
    };
}
/*==================[end of file]============================================*/