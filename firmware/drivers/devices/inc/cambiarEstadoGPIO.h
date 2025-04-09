/**
 * @file cambiarEstadoGPIO.h
 * @brief Declaración de la función para mapear un dígito BCD a pines GPIO.
 *
 * Este archivo contiene la declaración de la función `BCDToGPIO`, que toma un dígito BCD
 * (de 0 a 9) e interpreta cada bit para cambiar el estado de 4 pines GPIO, en función
 * de su valor binario. Los pines deben haber sido previamente inicializados como salidas.
 *
 * El mapeo de bits es el siguiente:
 * - b0 → GPIO_20  
 * - b1 → GPIO_21  
 * - b2 → GPIO_22  
 * - b3 → GPIO_23  
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 */

 #ifndef cambiarEstadoGPIO  
 #define cambiarEstadoGPIO
 
 #include <stdint.h>
 #include <gpio_mcu.h>
 
 /**
  * @brief Estructura para configurar un GPIO.
  */
 typedef struct {
     gpio_t pin; /**< Número de pin GPIO. */
     io_t dir;   /**< Dirección del pin: 0 (IN) o 1 (OUT). */
 } gpioConf_t;
 
 /**
  * @brief Cambia el estado de 4 pines GPIO según un dígito BCD.
  *
  * Esta función interpreta un valor BCD (0 a 9) y activa o desactiva 4 pines GPIO
  * de acuerdo al valor de cada bit. El arreglo de estructuras `vectorGPIO` debe contener
  * la configuración de los 4 pines, previamente inicializados como salidas.
  *
  * @param digitoBCD Dígito en formato BCD (de 0 a 9).
  * @param vectorGPIO Puntero al arreglo de estructuras gpioConf_t que representa los pines.
  */
 void BCDToGPIO(int8_t digitoBCD, gpioConf_t *vectorGPIO);
 
 #endif // cambiarEstadoGPIO
 