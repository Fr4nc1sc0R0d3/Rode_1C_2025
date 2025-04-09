/*! @mainpage Ejercicio 5 - Guia 1
 *
 * @section descripcion_codigo Descripción general del código
 *
 * El objetivo de este ejercicio es mostrar un número entero en una pantalla LCD compuesta
 * por tres displays de 7 segmentos, utilizando conversores BCD a 7 segmentos (CD4543).
 * 
 * Para lograr esto, el código realiza las siguientes tareas:
 * - Convierte un número entero de 32 bits a un arreglo de dígitos BCD (con la función `convertToBcdArray`)
 * - Configura los pines GPIO para representar cada bit del número BCD (usando `BCDToGPIO`)
 * - Utiliza multiplexado para encender secuencialmente cada uno de los tres dígitos del display,
 *   activando el pin correspondiente de selección (SEL_1, SEL_2, SEL_3) durante un pequeño retardo.
 *
 * Cada ciclo de multiplexado presenta un dígito a la vez mediante la combinación de:
 * - Pines de datos (D1 a D4): GPIO_20 a GPIO_23
 * - Pines de selección (SEL_1 a SEL_3): GPIO_19, GPIO_18 y GPIO_9
 *
 * Esta solución reutiliza el código de los puntos anteriores:
 * - Punto 4: conversión de número entero a BCD (`convertToBcdArray`)
 * - Punto 5: salida de un dígito BCD en pines GPIO (`BCDToGPIO`)
 *
 * El resultado es que el número completo aparece visualizado correctamente en los displays,
 * utilizando multiplexado con un pulso típico de activación de 50 ms para "latchear" el valor
 * en los Flip-Flops internos del CD4543.
 *
 * @section hardConn Conexión del Hardware
 *
 * | Señal EDU-ESP | Función        | Display (CD4543) |
 * |:-------------:|:--------------:|:----------------:|
 * | GPIO_20       | D1 (b0)        | Entrada BCD      |
 * | GPIO_21       | D2 (b1)        | Entrada BCD      |
 * | GPIO_22       | D3 (b2)        | Entrada BCD      |
 * | GPIO_23       | D4 (b3)        | Entrada BCD      |
 * | GPIO_19       | SEL_1          | Selección dígito 1 |
 * | GPIO_18       | SEL_2          | Selección dígito 2 |
 * | GPIO_9        | SEL_3          | Selección dígito 3 |
 * | +5V           | Alimentación   | CD4543           |
 * | GND           | Tierra         | CD4543           |
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
//#include <cambiarEstadoGPIO.h>
//#include <funcionBCD.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

 /**
  * @brief Estructura para configurar un GPIO.
  */
typedef struct {
    gpio_t pin;
    io_t dir;
} gpioConf_t;


/*==================[internal functions declaration]=========================*/
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
    
    for(int8_t i = 0; i < 4; i ++) {
        
        if (digitoBCD & 1) {
            printf("Pongo el alto el pin: %d", vectorGPIO[i].pin);
            GPIOOn(vectorGPIO[i].pin);
        } else {
            printf("Pongo el bajo el pin: %d", vectorGPIO[i].pin);
            GPIOOff(vectorGPIO[i].pin);
        }

        printf("\n");

        digitoBCD = digitoBCD >> 1; // Avanza al proximo bit.
    }
}

/**
 * @brief Convierte un número a un arreglo BCD (Decimal Codificado en Binario).
 *
 * @param numero Número a convertir.
 * @param digitos Cantidad de dígitos que se desean convertir.
 * @param bcd_array Puntero a un arreglo donde se guardarán los dígitos.
 * @return int8_t 0 si fue exitoso, -1 si ocurrió un error.
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {

	// Retorna 0, en caso de exito, y -1 en caso contrario.

    // Verificar que el puntero no sea nulo
    if (bcd_number == NULL) {
        return -1; // Error: puntero nulo
    }

    // Verificar que el número de dígitos sea válido
    if (digits == 0 || digits > 10) {
        return -1; // Error: número de dígitos inválido, ya que un numero de 32 bits se representa con 10 digitos, y la cantidad de digitos no puede ser 0, ya que no se estaria extrayendo ningun digito.
    }

    for (int i=0; i<digits; i++) {
		bcd_number[i]=data%10;  				
		data/=10;                               
	}
	
    return 0; // Éxito
}



/**
 * @brief Muestra un número de hasta 3 dígitos en un display de 7 segmentos multiplexado.
 *
 * Esta función convierte el número recibido a BCD, y luego lo muestra en
 * los 3 displays mediante multiplexado. Cada display se activa uno a la vez,
 * con un retardo corto para “latchear” los datos en el CD4543.
 *
 * @param digito32Bits Número entero a mostrar (hasta 3 dígitos).
 * @param cantidadDigitos Cantidad de dígitos que se desea mostrar (por ejemplo, 3).
 * @param vectorConfiguracionLED Vector con los pines D1-D4 conectados al BCD.
 * @param vectorSeleccionLED Vector con los pines SEL_1 a SEL_3 de selección del display.
 */
void encenderLED(int32_t digito32Bits, int8_t cantidadDigitos, gpioConf_t *vectorConfiguracionLED, gpioConf_t *vectorSeleccionLED) {
    uint8_t BCDArray[3];
    convertToBcdArray(digito32Bits, cantidadDigitos, BCDArray);
    for(int8_t i = 0; i < 3; i ++) {
        BCDToGPIO(BCDArray[i],vectorConfiguracionLED);
        printf("%d", BCDArray[i]);
		printf("\n");
        GPIOOn(vectorSeleccionLED[i].pin);
        vTaskDelay(pdMS_TO_TICKS(5));
        GPIOOff(vectorSeleccionLED[i].pin);
    }

}

/*==================[external functions definition]==========================*/
void app_main(void){
	
    // Inicializacion de pines.
    gpioConf_t gpioConfiguracion[4]= {{GPIO_20, GPIO_OUTPUT},{GPIO_21, GPIO_OUTPUT},{GPIO_22, GPIO_OUTPUT},{GPIO_23, GPIO_OUTPUT}};
    gpioConf_t gpioSeleccion[3] = {{GPIO_19, GPIO_OUTPUT},{GPIO_18, GPIO_OUTPUT}, {GPIO_9, GPIO_OUTPUT}};

    for(int8_t i = 0; i < 4; i ++) {
        GPIOInit(gpioConfiguracion[i].pin,gpioConfiguracion[i].dir);
    }
    for(int8_t i = 0; i < 3; i ++) {
        GPIOInit(gpioSeleccion[i].pin, gpioSeleccion[i].dir);
    }

    uint32_t numero = 123;
    uint8_t digitos = 3;

    encenderLED(numero,digitos,gpioConfiguracion,gpioSeleccion);

}
/*==================[end of file]============================================*/