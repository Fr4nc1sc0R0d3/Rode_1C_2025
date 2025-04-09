/*!
 * @mainpage Ejercicio 4 - Guia 1
 *
 * @section descripcion_codigo Descripción general del código
 *
 * El objetivo de este programa es demostrar el uso de una función que convierte un número entero 
 * (tipo `uint32_t`) a un arreglo de dígitos en formato BCD (Decimal Codificado en Binario).
 * 
 * La conversión se realiza con la función `convertToBcdArray`, que toma como entrada:
 * - un número entero,
 * - la cantidad de dígitos a extraer,
 * - y un puntero a un arreglo donde se almacenarán los dígitos BCD.
 * 
 * En este ejemplo, el número 1234 se convierte en un arreglo de 4 dígitos y se imprime por consola.
 * 
 * Esta implementación puede utilizarse como base para mostrar valores en displays de 7 segmentos,
 * LCDs, o para comunicaciones donde se requiera manipulación de cada dígito decimal por separado.
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <funcionBCD.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
    
    // Ejemplo de uso de convertToBcdArray
    uint32_t numero = 1234;        // Número a convertir
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
