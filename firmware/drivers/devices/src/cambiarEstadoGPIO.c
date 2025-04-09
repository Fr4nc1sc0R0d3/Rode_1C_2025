/**
 * @file cambiarEstadoGPIO.c
 * @brief Implementación de la función que cambia el estado de pines GPIO según un dígito BCD.
 *
 * Esta función recibe un valor BCD (de 0 a 9) y un vector de configuraciones GPIO.
 * Interpreta cada bit del dígito BCD y enciende o apaga el pin correspondiente.
 */

 #include "cambiarEstadoGPIO.h"
 #include <stdio.h>  // Para printf
 
 /**
  * @brief Cambia el estado de 4 pines GPIO según un dígito BCD.
  *
  * Recorre los 4 bits menos significativos del dígito BCD, evaluando si están en 1 o 0,
  * y enciende o apaga el pin correspondiente del arreglo `vectorGPIO` usando `GPIOOn` o `GPIOOff`.
  *
  * El bit menos significativo (b0) se corresponde con el primer pin del vector (GPIO_20).
  * Luego, se avanza bit a bit desplazando el número hacia la derecha.
  *
  * @param digitoBCD Dígito codificado en BCD (de 0 a 9).
  * @param vectorGPIO Puntero a un vector de estructuras gpioConf_t con la configuración de los pines GPIO.
  */
 void BCDToGPIO(int8_t digitoBCD, gpioConf_t *vectorGPIO) {
     
     for(int8_t i = 0; i < 4; i++) {
 
         if (digitoBCD & 1) {
             printf("Pongo en alto el pin: %d", vectorGPIO[i].pin);
             GPIOOn(vectorGPIO[i].pin);
         } else {
             printf("Pongo en bajo el pin: %d", vectorGPIO[i].pin);
             GPIOOff(vectorGPIO[i].pin);
         }
 
         printf("\n");
         digitoBCD = digitoBCD >> 1; // Avanza al próximo bit.
     }
 }
 