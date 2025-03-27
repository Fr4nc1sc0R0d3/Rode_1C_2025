#include "cambiarEstadoGPIO.h"

void setEstadoGPIO(int8_t digitoBCD, gpioConf_t *vectorGPIO) {
    
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