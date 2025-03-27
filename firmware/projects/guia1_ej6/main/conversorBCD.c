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
#include <gpio_mcu.h>
#include <cambiarEstadoGPIO.H>
#include <funcionBCD.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

typedef struct {
    gpio_t pin;
    io_t dir;
} gpioConf_t;


void encenderLED(int32_t digitoBCD, int8_t cantidadDigitos, gpioConf_t *vectorConfiguracionLED, gpioConf_t *vectorSeleccionLED) {
    //setEstadoGPIO(digitoBCD, vectorConfiguracionLED);

    // EN PROCESO.
}




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
            setEstadoGPIO(bcd_result[i], gpioConfiguracion);
        }

    } else {
        printf("Error en la conversión\n");
    }


}
/*==================[end of file]============================================*/