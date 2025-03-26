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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
int matrizNumerosBCD[10][4] = {
	{0,0,0,0},
	{0,0,0,1},
	{0,0,1,0},
	{0,0,1,1},
	{0,1,0,0},
	{0,1,0,1},
	{0,1,1,0},
	{0,1,1,1},
	{1,0,0,0},
	{1,0,0,1}
};
/*==================[internal functions declaration]=========================*/

int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {

    // Verificar que el puntero no sea nulo
    if (bcd_number == NULL) {
        return -1; // Error: puntero nulo
    }

    // Verificar que el número de dígitos sea válido
    if (digits == 0 || digits > 10) {
        return -1; // Error: número de dígitos inválido, ya que un numero de 32 bits se representa con 10 digitos, y la cantidad de digitos no puede ser 0, ya que no se estaria extrayendo ningun digito.
    }

	// Reservar memoria dinámicamente para los dígitos
    int *digitos = (int *)malloc(digits * sizeof(int));
    if (digitos == NULL) {
        return -1; // Error: No se pudo asignar memoria
    }
	

    // Extraer cada dígito y almacenarlo en el arreglo
	for (int i = 0; i < digits; i++) {
		int8_t digito = data % 10; // Obtener el último dígito
		digitos[i] = digito;
        data /= 10; // Eliminar el último dígito
	}

	for (int i = 0; i < digits; i ++) {
		int *array_bcd_asociado = matrizNumerosBCD[digitos[i]];
		for (int j = 0; j < 4; j++) {
			bcd_number[i * 4 + j] = array_bcd_asociado[j];
		}
	}

	free(digitos); // liberar memoria por utilizacion de memoria por arreglo dinamico.
    return 0; // Éxito
}

/*==================[external functions definition]==========================*/
void app_main(void){
	printf("Hello world!\n");
}
/*==================[end of file]============================================*/