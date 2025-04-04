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
#include "funcionBCD.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/



/*==================[external functions definition]==========================*/
void app_main(void){
    
    // Ejemplo de uso de convertToBcdArray
    uint32_t numero = 1234;       // Número a convertir
    uint8_t digitos = 4;           // Cantidad de dígitos a convertir
    uint8_t bcd_result[10];        // Array para almacenar los dígitos BCD (máximo 10 dígitos para uint32_t)
    
    // Llamar a la función
    int8_t resultado = convertToBcdArray(numero, digitos, bcd_result);
    
    // Verificar el resultado
    if(resultado == 0){
        printf("Conversión exitosa. Dígitos BCD (en orden inverso): ");
        for(int i = 0; i < digitos; i++){
            printf("%d ", bcd_result[i]);
        }
        printf("\n");
    } else {
        printf("Error en la conversión\n");
    }
}
/*==================[end of file]============================================*/