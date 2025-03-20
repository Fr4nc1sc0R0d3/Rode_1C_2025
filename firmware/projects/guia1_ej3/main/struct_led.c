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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
typedef struct
{
	uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} LED;

enum {ON, OFF, TOGGLE};

void actuadorLeds(LED *led) {
    if (led == NULL) return;
    int8_t retardo = led->periodo/CONFIG_BLINK_PERIOD;

    switch (led->mode) {
        case ON:
            LedOn(led->n_led);    
        break;

        case OFF: 
            LedOff(led->n_led);
        break;

        case TOGGLE:
            for (uint8_t i = 0; i < led->n_ciclos; i++) {
                LedToggle(led->n_led);
                for(int j = 0; j < retardo; j++) {
                    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
                }
            }
        break;

    }
}

/*==================[external functions definition]==========================*/
void app_main(void) {
    LedsInit();
    SwitchesInit();
    LED led1, led2, led3;
    led1.mode = TOGGLE;
    led1.n_ciclos = 10;
    led1.n_led = LED_1;
    led1.periodo = 1000;
    actuadorLeds(&led1);
    /*
    printf("Ingrese los datos para LED 1:\n");
    printf("Modo (0: OFF, 1: ON, 2: TOGGLE): ");
    scanf("%hhu", &led1.mode);
    printf("Número de LED: ");
    scanf("%hhu", &led1.n_led);
    printf("Cantidad de ciclos de encendido/apagado: ");
    scanf("%hhu", &led1.n_ciclos);
    printf("Periodo de cada ciclo (ms): ");
    scanf("%hu", &led1.periodo);

    printf("\nIngrese los datos para LED 2:\n");
    printf("Modo (0: OFF, 1: ON, 2: TOGGLE): ");
    scanf("%hhu", &led2.mode);
    printf("Número de LED: ");
    scanf("%hhu", &led2.n_led);
    printf("Cantidad de ciclos de encendido/apagado: ");
    scanf("%hhu", &led2.n_ciclos);
    printf("Periodo de cada ciclo (ms): ");
    scanf("%hu", &led2.periodo);

    printf("\nIngrese los datos para LED 3:\n");
    printf("Modo (0: OFF, 1: ON, 2: TOGGLE): ");
    scanf("%hhu", &led3.mode);
    printf("Número de LED: ");
    scanf("%hhu", &led3.n_led);
    printf("Cantidad de ciclos de encendido/apagado: ");
    scanf("%hhu", &led3.n_ciclos);
    printf("Periodo de cada ciclo (ms): ");
    scanf("%hu", &led3.periodo);

    printf("\nDatos ingresados:\n");
    printf("LED 1 - Modo: %d, Número: %d, Ciclos: %d, Periodo: %d ms\n", led1.mode, led1.n_led, led1.n_ciclos, led1.periodo);
    printf("LED 2 - Modo: %d, Número: %d, Ciclos: %d, Periodo: %d ms\n", led2.mode, led2.n_led, led2.n_ciclos, led2.periodo);
    printf("LED 3 - Modo: %d, Número: %d, Ciclos: %d, Periodo: %d ms\n", led3.mode, led3.n_led, led3.n_ciclos, led3.periodo);
    */


    


}
/*==================[end of file]============================================*/