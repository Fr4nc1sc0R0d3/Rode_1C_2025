#include "funcionBCD.h"
#include <stdio.h>
#include <stdint.h>

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