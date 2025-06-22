/**
 * @file hc_sr04_multiple_control.h
 * @brief Control de múltiples sensores ultrasónicos HC-SR04
 * 
 * Este archivo define la interfaz para el control de múltiples sensores
 * ultrasónicos HC-SR04 utilizados en el proyecto de vincha ultrasónica
 * para personas con discapacidad visual.
 * 
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 * @date 12/09/2023
 */

#ifndef HC_SR04_MULTIPLE_CONTROL_H
#define HC_SR04_MULTIPLE_CONTROL_H

#include "gpio_mcu.h"
#include "hc_sr04.h"

/*==================[macros and definitions]=================================*/

/**
 * @struct HCSR04
 * @brief Estructura para configuración de un sensor HC-SR04
 * 
 * Esta estructura define los pines GPIO necesarios para el control
 * de un sensor ultrasónico HC-SR04, incluyendo los pines de ECHO y TRIGGER.
 */
typedef struct {
    gpio_t echo;    /**< Pin GPIO conectado al pin ECHO del sensor */
    gpio_t trigger; /**< Pin GPIO conectado al pin TRIGGER del sensor */
} HCSR04;

/*==================[external functions declaration]=========================*/

/**
 * @brief Obtiene las mediciones de todos los sensores HC-SR04 configurados
 * 
 * Esta función realiza la medición de distancia de todos los sensores
 * ultrasónicos HC-SR04 configurados en el sistema y almacena los resultados
 * en el vector proporcionado.
 * 
 * Los sensores están configurados de la siguiente manera:
 * - Sensor 0: Frontal (GPIO 2-TRIGGER, GPIO 3-ECHO)
 * - Sensor 1: Lateral izquierdo (GPIO 22-TRIGGER, GPIO 21-ECHO)  
 * - Sensor 2: Lateral derecho (GPIO 23-TRIGGER, GPIO 19-ECHO)
 * 
 * @param[out] vectorMediciones Puntero a un array de uint16_t donde se almacenarán
 *                             las mediciones en centímetros. El array debe tener
 *                             al menos 3 elementos para almacenar las mediciones
 *                             de los 3 sensores.
 * 
 * @note Las mediciones se realizan de forma secuencial para evitar interferencias
 *       entre sensores.
 * @note Si un sensor no puede realizar la medición, se almacena el valor 0.
 * @note La función es bloqueante durante el tiempo de medición de cada sensor.
 * 
 * @warning El puntero vectorMediciones debe apuntar a un array válido de al menos
 *          3 elementos uint16_t, de lo contrario se producirá un comportamiento
 *          indefinido.
 * 
 * @see HCSR04
 */
void obtenerMediciones(uint16_t *vectorMediciones);

#endif /* HC_SR04_MULTIPLE_CONTROL_H */