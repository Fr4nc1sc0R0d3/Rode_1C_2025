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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "motor_pwm.h"
/*==================[macros and definitions]=================================*/

#define PERIODO_INTERRUPCION_US 100000 

/*==================[internal data definition]===============================*/

HCSR04 vectorSensores[5] = {{GPIO_3,GPIO_2},{GPIO_9, GPIO_18},{GPIO_21,GPIO_22},{GPIO_19, GPIO_20},{GPIO_15,GPIO_17}}; 
TaskHandle_t handlerMedicion;

/*==================[internal functions declaration]=========================*/

void FuncTimer(void* param)
{
	vTaskNotifyGiveFromISR(handlerMedicion, pdFALSE);
}

static void medirTask(void *pvParameter) {
    char buffer[32];  // Mucho más chico

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        UartSendString(UART_PC, "Iniciando medición...\r\n");

        medicionesHCSRO4 mediciones = obtenerMediciones(vectorSensores);

        // Sensor 1
        itoa(mediciones.s1, buffer, 10);
        UartSendString(UART_PC, "Medición sensor 1: ");
        UartSendString(UART_PC, buffer);
        UartSendString(UART_PC, " cm\r\n");

		if (mediciones.s1 >= 60) {
			setPWM(0);
		} else {
            printf("Prendiendo motor!");
			uint16_t ct = 100-((5/3))*mediciones.s1;
            
			setPWM(ct);
		}

		

        // Sensor 2
        itoa(mediciones.s2, buffer, 10);
        UartSendString(UART_PC, "Medición sensor 2: ");
        UartSendString(UART_PC, buffer);
        UartSendString(UART_PC, " cm\r\n");

        // Sensor 3
        itoa(mediciones.s3, buffer, 10);
        UartSendString(UART_PC, "Medición sensor 3: ");
        UartSendString(UART_PC, buffer);
        UartSendString(UART_PC, " cm\r\n");

        // Sensor 4
        itoa(mediciones.s4, buffer, 10);
        UartSendString(UART_PC, "Medición sensor 4: ");
        UartSendString(UART_PC, buffer);
        UartSendString(UART_PC, " cm\r\n");
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

	UartInit(&puertoSerie);
	inicializarMotor(GPIO_23);
	TimerInit(&timerMedicion);
	TimerStart(TIMER_A);

	xTaskCreate(&medirTask, "MedirDistancias", 8192, NULL, 5, &handlerMedicion);
}
/*==================[end of file]============================================*/