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
#include <dht11.h>
#include <analog_io_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_mcu.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/

#define CANAL_INGRESO_SENAL CH1
#define PERIODO_MUESTREO_SENSOR_TyH 1000000
#define PERIODO_MUESTREO_SENSOR_RADIACION 5000000

/*==================[internal data definition]===============================*/

TaskHandle_t handlerTareaTyH;

TaskHandle_t handlerTareaRadiacion;

bool encendido;

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaTemperatura, pdFALSE);
}

void FuncTimerB(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaRadiacion, pdFALSE);
}

static void tareaMedirRadiacion(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		// aca va codigo tarea
	}
}

static void tareaMedirTyH(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		// aca va codigo tarea
	}
}

void encenderDispositivo() {
	encendido = true;
}

void apagarDispositivo() {
	encendido = false;
}

/*==================[external functions definition]==========================*/
void app_main(void){

	analog_input_config_t configAnalog = {
		.input = CANAL_INGRESO_SENAL,
		.mode = ADC_SINGLE,
		.func_p = NULL, 
		.param_p = NULL
	};

	timer_config_t timerMuestreoTyH = {
        .timer = TIMER_A,
        .period = PERIODO_MUESTREO_SENSOR_TEMPERATURA,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	timer_config_t timerMuestreoRadiacion = {
        .timer = TIMER_B,
        .period = PERIODO_MUESTREO_SENSOR_RADIACION,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

	serial_config_t configPuertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};
	
	TimerInit(&timerMuestreoRadiacion);
	TimerInit(&timerMuestreoTyH);
	AnalogInputInit(&configAnalog);
	UartInit(&configPuertoSerie);
	LedsInit();
	SwitchesInit();
	SwitchActivInt(SWITCH_1, encenderDispositivo, 0);
    SwitchActivInt(SWITCH_2, apagarDispositivo, 0);
	xTaskCreate(&tareaMedirRadiacion, "tareaMedirRadiacion", 2048, NULL, 5, &handlerTareaRadiacion);
	xTaskCreate(&tareaMedirTyH, "tareaMedirTyH", 2048, NULL, 5, &handlerTareaTyH);
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
	

}
/*==================[end of file]============================================*/