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
#define GPIO_SENSOR_TyH GPIO_12

/*==================[internal data definition]===============================*/

TaskHandle_t handlerTareaTyH;

TaskHandle_t handlerTareaRadiacion;

bool encendido;

float humedad;

float temperatura;

uint16_t voltaje = 0;

float radiacionMedida;

bool radiacionAlta;

bool riesgoNevada;



/*==================[internal functions declaration]=========================*/

float devolverRadiacionSensada(uint16_t voltajeSensor) {
	return (100/3.3)*voltajeSensor;
}

void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaTyH, pdFALSE);
}

void FuncTimerB(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaRadiacion, pdFALSE);
}

void FuncTimerC(void *param) {
	if (!radiacionAlta && !riesgoNevada) {
		LedOff(LED_3);
		LedOff(LED_2);
		LedOn(LED_1);
	}
}

static void tareaMedirRadiacion(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (encendido) {
			char bufferR[15];
			AnalogInputReadSingle(CANAL_INGRESO_SENAL, &voltaje);
			sprintf(bufferR, "%.2f", (float)voltaje);
			UartSendString(UART_PC, "Radiación: ");
			UartSendString(UART_PC, bufferR);
			UartSendString(UART_PC, "mR/h");
			if (devolverRadiacionSensada(voltaje) > 40) {
				radiacionAlta = true;
				LedOn(LED_2);
				UartSendString(UART_PC, " - RADIACIÓN ELEVADA");
			}
			radiacionAlta = false;
			UartSendString(UART_PC, " \r\n");
		}
		
	}
}

static void tareaMedirTyH(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (encendido) {
			if(dht11Read(&humedad, &temperatura)) {
				char bufferH[10];
				char bufferT[10];
				sprintf(bufferT, "%.2f", temperatura);
				sprintf(bufferH, "%.2f", temperatura);
				UartSendString(UART_PC, "Temperatura: ");
				UartSendString(UART_PC, bufferT);
				UartSendString(UART_PC, "°C - ");
				UartSendString(UART_PC, "Húmedad: ");
				UartSendString(UART_PC, bufferH);
				UartSendString(UART_PC, "%");
				if (humedad > 85) {
					if (temperatura > 0 && temperatura < 2) {
						LedOn(LED_3);
						UartSendString(UART_PC, " - RIESGO DE NEVADA");
						UartSendString(UART_PC, " \r\n");
						riesgoNevada = true;

					} else {
						UartSendString(UART_PC, " \r\n");
						riesgoNevada = false;
					}
				} else {
					riesgoNevada = false;
					UartSendString(UART_PC, " \r\n");
				}
				
			}
		}
		

	}
}

void encenderDispositivo() {
	encendido = true;
}

void apagarDispositivo() {
	encendido = false;
	LedOff(LED_1);
	LedOff(LED_2);
	LedOff(LED_3);
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
        .period = PERIODO_MUESTREO_SENSOR_TyH,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	timer_config_t timerControlLeds = {
		.timer = TIMER_C,
		.period = PERIODO_MUESTREO_SENSOR_TyH/2,
		.func_p = FuncTimerB,
		.param_p = NULL
	};

	timer_config_t timerMuestreoRadiacion = {
        .timer = TIMER_B,
        .period = PERIODO_MUESTREO_SENSOR_RADIACION,
        .func_p = FuncTimerC,
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
	TimerInit(&timerControlLeds);
	AnalogInputInit(&configAnalog);
	UartInit(&configPuertoSerie);
	LedsInit();
	SwitchesInit();
	dht11Init(GPIO_SENSOR_TyH);
	SwitchActivInt(SWITCH_1, encenderDispositivo, 0);
    SwitchActivInt(SWITCH_2, apagarDispositivo, 0);
	xTaskCreate(&tareaMedirRadiacion, "tareaMedirRadiacion", 2048, NULL, 5, &handlerTareaRadiacion);
	xTaskCreate(&tareaMedirTyH, "tareaMedirTyH", 2048, NULL, 5, &handlerTareaTyH);
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
	

}
/*==================[end of file]============================================*/