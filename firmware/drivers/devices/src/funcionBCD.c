#include "funcionBCD.h"

/**
 * @brief Convierte un número a un arreglo BCD (Decimal Codificado en Binario).
 *
 * @param numero Número a convertir.
 * @param digitos Cantidad de dígitos que se desean convertir.
 * @param bcd_array Puntero a un arreglo donde se guardarán los dígitos.
 * @return int8_t 0 si fue exitoso, -1 si ocurrió un error.
 */
int8_t convertToBcdArray(uint32_t numero, uint8_t digitos, uint8_t *bcd_array) {
    if(digitos == 0 || digitos > 10) {
        return -1;  // Error por parámetro inválido
    }

    for(uint8_t i = 0; i < digitos; i++) {
        bcd_array[i] = numero % 10;
        numero /= 10;
    }

    return 0;
}
