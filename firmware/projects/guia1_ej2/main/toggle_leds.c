/*! @mainpage Ejercicio 2 Guia 1 
 *
 * @section descripcion_codigo Descripcion general del codigo
 *
 * El presente codigo genera busca resolver 2 problemas, tomando como base de codigo provisto por la catedra presente en el proyecto "1_blinking_switch":
 * - Hacer titilar el LED 1 y el LED 2 mientras se presionan las teclas 1 y 2 respectivamente.
 * - Hacer titilar el LED 3 mientras se mantengan presionadas las teclas 1 y 2.
 * 
 * @author Francisco Rode (francisco.rode@ingenieria/uner.edu.ar).
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

/** @def CONFIG_BLINK_PERIOD
 * @brief Periodo de encendido del led.
*/
#define CONFIG_BLINK_PERIOD 100

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/** @fn void app_main(void)
 * @brief Funcion principal que resolve los problemas anteriormente mencionados. 
 * En principio, inicializa los pines de los LEDs y de las teclas (switches).
 * Luego, realiza un ciclo while infinito, en donde en cada iteracion almacena en la variable "teclas"
 * el binario asociado a las teclas presionadas:
 * - teclas = 0000 --> ninguna tecla presionada
 * - teclas = 0001 --> primer tecla presionada
 * - teclas = 0010 --> segunda tecla presionada
 * - teclas = 0011 --> las dos teclas presionadas al mismo tiempo
 * En base a esto, se hacen operaciones logicas AND entre lo que esta almacenado en teclas y el valor binario 
 * en SWITCH_1, SWITCH_2 o SWITCH_3 respectivamente, llegando a:
 * - teclas & SWITCH_1 = 1 --> detecta que tecla almacena SWITCH_1
 * - teclas & SWITCH_2 = 1 --> detecta que tecla almacena SWITCH_2
 * - teclas & SWITCH_3 = 1 --> detecta que tecla almacena SWITCH_3
 * - teclas & SWITCH_1 = 1 --> detecta que tecla almacena SWITCH_1
 * - (teclas & SWITCH_1) && (teclas & SWITCH_2) = 1 --> detecta que tecla almacena 0011 (las dos teclas presionadas)
 * En base a esto se hace el control de que LED encender.
 */
void app_main(void){
    
    uint8_t teclas;
    
    LedsInit();
    SwitchesInit();

    while (1) {
        teclas = SwitchesRead();

        if ((teclas & SWITCH_1) && (teclas & SWITCH_2)) {
            LedOn(LED_3);
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
            LedOff(LED_3);
        }
        else if (teclas & SWITCH_1) {
            LedOn(LED_1);
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
            LedOff(LED_1);
        }
        else if (teclas & SWITCH_2) {
            LedOn(LED_2);
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
            LedOff(LED_2);
        }

        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
