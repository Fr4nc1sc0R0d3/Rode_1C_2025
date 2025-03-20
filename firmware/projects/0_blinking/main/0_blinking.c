/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 blink.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 10000
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void) {
    LedsInit();

    while (true) {
        TickType_t delay = 300 / portTICK_PERIOD_MS;  // Tiempo base de espera

        /* Efecto "Carrera" */
        for (int i = 0; i < 3; i++) {
            LedOn(LED_1); vTaskDelay(delay);
            LedOff(LED_1);
            LedOn(LED_2); vTaskDelay(delay);
            LedOff(LED_2);
            LedOn(LED_3); vTaskDelay(delay);
            LedOff(LED_3);
        }

        /* Efecto "Carrera inversa" */
        for (int i = 0; i < 3; i++) {
            LedOn(LED_3); vTaskDelay(delay);
            LedOff(LED_3);
            LedOn(LED_2); vTaskDelay(delay);
            LedOff(LED_2);
            LedOn(LED_1); vTaskDelay(delay);
            LedOff(LED_1);
        }

        /* Efecto "Pulso" */
        for (int i = 0; i < 3; i++) {
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
            vTaskDelay(delay * 2);
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
            vTaskDelay(delay);
        }

        /* Efecto "Onda" */
        for (int i = 0; i < 3; i++) {
            LedOn(LED_1); vTaskDelay(delay);
            LedOn(LED_2); vTaskDelay(delay);
            LedOn(LED_3); vTaskDelay(delay * 2);
            LedOff(LED_1); vTaskDelay(delay);
            LedOff(LED_2); vTaskDelay(delay);
            LedOff(LED_3); vTaskDelay(delay * 2);
        }

        /* Efecto "Explosión" */
        for (int i = 0; i < 3; i++) {
            LedOn(LED_1);
            LedOn(LED_3);
            vTaskDelay(delay);
            LedOn(LED_2);
            vTaskDelay(delay);
            LedOff(LED_1);
            LedOff(LED_3);
            vTaskDelay(delay);
            LedOff(LED_2);
            vTaskDelay(delay);
        }

        /* Pequeña pausa antes de repetir */
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
/*==================[end of file]============================================*/
