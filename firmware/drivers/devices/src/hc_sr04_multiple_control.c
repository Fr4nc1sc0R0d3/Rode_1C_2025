/**
 * @file hc_sr04_multiple_control.c
 * @brief Implementación del control de múltiples sensores ultrasónicos HC-SR04
 * 
 * Este archivo contiene la implementación de las funciones para el control
 * de múltiples sensores ultrasónicos HC-SR04 utilizados en el proyecto de
 * vincha ultrasónica para personas con discapacidad visual.
 * 
 * Los sensores están distribuidos estratégicamente para cubrir diferentes
 * zonas de detección alrededor del usuario.
 * 
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 * @date 12/09/2023
 */

/*==================[inclusions]=============================================*/
#include "hc_sr04_multiple_control.h"
#include <stdint.h>

/*==================[internal data definition]===============================*/

/**
 * @brief Array de configuración de los sensores HC-SR04
 * 
 * Este array define la configuración de pines GPIO para cada uno de los
 * tres sensores ultrasónicos utilizados en el sistema:
 * 
 * - Índice 0: Sensor frontal lateral izquierdo
 *   - ECHO: GPIO_21, TRIGGER: GPIO_22
 * - Índice 1: Sensor frontal superior
 *   - ECHO: GPIO_3, TRIGGER: GPIO_2
 * - Índice 2: Sensor frontal lateral derecho
 *   - ECHO: GPIO_19, TRIGGER: GPIO_23
 * 
 * @note El orden de inicialización es importante para mantener la correspondencia
 *       con el array de mediciones en el código principal
 */
HCSR04 vectorSensores[3] = {
    {GPIO_21, GPIO_22},  // Sensor frontal inferior izquierdo
    {GPIO_3, GPIO_2},    // Sensor frontal superior
    {GPIO_19, GPIO_23}   // Sensor frontal inferior derecho
};

/*==================[external functions definition]==========================*/

/**
 * @brief Obtiene las mediciones de todos los sensores HC-SR04 configurados
 * 
 * Esta función inicializa secuencialmente cada sensor HC-SR04 y obtiene
 * su medición de distancia. Las mediciones se realizan de forma secuencial
 * para evitar interferencias ultrasónicas entre sensores.
 * 
 * Proceso de medición:
 * 1. Para cada sensor en el array vectorSensores:
 *    - Inicializa el sensor con sus pines específicos
 *    - Realiza la medición de distancia
 *    - Almacena el resultado en el vector de mediciones
 * 
 * @param[out] vectorMediciones Puntero a un array de uint16_t donde se almacenarán
 *                             las mediciones en centímetros. Debe tener al menos
 *                             3 elementos:
 *                             - [0]: Medición sensor lateral izquierdo
 *                             - [1]: Medición sensor frontal
 *                             - [2]: Medición sensor lateral derecho
 * 
 * @note La función realiza una inicialización completa de cada sensor en cada
 *       llamada para garantizar mediciones precisas
 * @note El tiempo total de ejecución depende del tiempo de respuesta de cada sensor
 * @note Si un sensor falla, la función continúa con los siguientes sensores
 * 
 * @warning Esta función es bloqueante durante el tiempo de medición
 * @warning El parámetro vectorMediciones debe apuntar a memoria válida
 * 
 * @see HCSR04
 * @see vectorSensores
 */
void obtenerMediciones(uint16_t *vectorMediciones) {
    for (uint8_t i = 0; i < 3; i++) {
        // Inicializar el sensor actual con sus pines específicos
        HcSr04Init(vectorSensores[i].echo, vectorSensores[i].trigger);
        
        // Obtener la medición de distancia en centímetros
        vectorMediciones[i] = HcSr04ReadDistanceInCentimeters();
    }
}