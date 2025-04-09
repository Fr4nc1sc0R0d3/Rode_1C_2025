/*!
 * @mainpage Ejercicio 3 - Guia 1
 *
 * @section descripcion_codigo Descripción general del código
 *
 * El objetivo de esta actividad es construir una estructura de datos, que contenga
 * los siguientes datos:
 * - uint8_t mode → ON, OFF, TOGGLE
 * - uint8_t n_led → indica el número de LED a controlar
 * - uint8_t n_ciclos → indica la cantidad de ciclos de encendido/apagado
 * - uint16_t periodo → indica el tiempo de cada ciclo
 *
 * La estructura debe servir como configuración para el control de un LED en particular. 
 * Además, como último punto se debe subir una captura de la señal temporal que se genera al presionar
 * el pulsador.
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
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

/**
 * @def CONFIG_BLINK_PERIOD
 * @brief Tiempo de encendido/apagado en milisegundos usado como base para el retardo.
 */
#define CONFIG_BLINK_PERIOD 100

/*==================[internal data definition]===============================*/

/**
 * @enum LedMode
 * @brief Modos posibles de operación para un LED.
 */
enum {ON, OFF, TOGGLE};

/**
 * @struct LED
 * @brief Estructura que representa la configuración de un LED.
 *
 * Contiene los parámetros necesarios para controlar un LED, incluyendo el modo de funcionamiento,
 * el número de LED, la cantidad de ciclos de encendido/apagado y el periodo de cada ciclo.
 */
typedef struct
{
    uint8_t mode;       /**< Modo de operación: ON, OFF, TOGGLE */
    uint8_t n_led;      /**< Número de LED a controlar */
    uint8_t n_ciclos;   /**< Cantidad de ciclos de encendido/apagado (solo en modo TOGGLE) */
    uint16_t periodo;   /**< Duración del ciclo en milisegundos */
} LED;

/*==================[internal functions declaration]=========================*/

/**
 * @brief Controla un LED según la configuración dada.
 *
 * Esta función activa el LED en modo ON, OFF o TOGGLE según los valores establecidos
 * en la estructura `LED`.
 *
 * @param led Puntero a una estructura LED con la configuración deseada.
 */
void actuadorLeds(LED *led) {
    if (led == NULL) return;

    int8_t retardo = led->periodo / CONFIG_BLINK_PERIOD; // Cálculo de retardo

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
                for (int j = 0; j < retardo; j++) {
                    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
                }
            }
            break;
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal de la aplicación.
 *
 * Inicializa los LEDs y switches. Luego configura un LED para que realice un parpadeo
 * (modo TOGGLE) durante una cantidad determinada de ciclos y periodo.
 */
void app_main(void) {
    LedsInit();
    SwitchesInit();

    LED led1;
    led1.mode = TOGGLE;
    led1.n_ciclos = 10;
    led1.n_led = LED_1;
    led1.periodo = 1000;

    actuadorLeds(&led1);
}

/*==================[end of file]============================================*/
