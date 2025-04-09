/*!
 * @mainpage Ejercicio 5 - Guia 1
 *
 * @section descripcion_codigo Descripción general del código
 *
 * El objetivo de este programa es convertir un número entero (por ejemplo, 1234) en su 
 * representación BCD (Decimal Codificado en Binario), y luego mostrar cada dígito individual 
 * a través de pines GPIO, utilizando una función que activa los pines de salida según los bits 
 * correspondientes al dígito BCD.
 *
 * Para lograr esto, el programa realiza los siguientes pasos:
 * - Inicializa un arreglo de estructuras `gpioConf_t` para configurar los GPIO 20 a 23 como salidas.
 * - Convierte el número entero a un arreglo de dígitos BCD utilizando la función `convertToBcdArray`.
 * - Itera por cada dígito BCD y lo envía a la función `BCDToGPIO`, la cual activa los GPIO en función 
 *   del valor binario del dígito.
 *
 * El mapeo de bits se realiza de la siguiente forma:
 * - b0 → GPIO_20  
 * - b1 → GPIO_21  
 * - b2 → GPIO_22  
 * - b3 → GPIO_23  
 *
 * De esta manera, cada dígito decimal se convierte en una señal binaria.
 *
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
#include <funcionBCD.h>
#include <cambiarEstadoGPIO.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/



/*==================[external functions definition]==========================*/
void app_main(void){
	
    // Inicializacion de pines.
    gpioConf_t gpioConfiguracion[4]= {{GPIO_20, GPIO_OUTPUT},{GPIO_21, GPIO_OUTPUT},{GPIO_22, GPIO_OUTPUT},{GPIO_23, GPIO_OUTPUT}};
    for(int8_t i = 0; i < 4; i ++) {
        GPIOInit(gpioConfiguracion[i].pin,gpioConfiguracion[i].dir);
    }


    uint32_t numero = 1234;       // Número a convertir
    uint8_t digitos = 4;           // Cantidad de dígitos a convertir
    uint8_t bcd_result[10];        // Array para almacenar los dígitos BCD (máximo 10 dígitos para uint32_t)
    int8_t resultado = convertToBcdArray(numero, digitos, bcd_result);
    
    // Verificar el resultado
    if(resultado == 0){
        printf("Conversión exitosa.");
        printf("\n");
        printf("Dígitos BCD (en orden inverso): ");
        for(int i = 0; i < digitos; i++){
            printf("%d ", bcd_result[i]);
        }

        for(int8_t i = 0; i<digitos; i++) {
            printf("Digito BCD: %d\n", bcd_result[i]);
            printf("/n");
            BCDToGPIO(bcd_result[i], gpioConfiguracion);
        }

    } else {
        printf("Error en la conversión\n");
    }


}
/*==================[end of file]============================================*/