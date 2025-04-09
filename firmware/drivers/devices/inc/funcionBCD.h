#ifndef FUNCION_BCD_H
#define FUNCION_BCD_H

#include <stdint.h>

/**
 * @brief Convierte un número entero en su representación BCD y lo guarda en un arreglo.
 *
 * Esta función convierte un número entero sin signo (`uint32_t`) a un arreglo de dígitos
 * individuales en formato BCD (Decimal Codificado en Binario). El número es descompuesto 
 * dígito por dígito y guardado en el array desde el menos significativo al más significativo.
 *
 * @param numero Número a convertir.
 * @param digitos Cantidad de dígitos a extraer (máx 10 para `uint32_t`).
 * @param bcd_array Puntero al arreglo donde se almacenarán los dígitos en BCD.
 * @return int8_t Devuelve 0 si la conversión fue exitosa, -1 si hubo error (por ejemplo, si `digitos` es inválido).
 */
int8_t convertToBcdArray(uint32_t numero, uint8_t digitos, uint8_t *bcd_array);

#endif // FUNCION_BCD_H
