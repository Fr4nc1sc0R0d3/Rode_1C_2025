#ifndef funcionBCD  
#define funcionBCD
#include <stdint.h>
#include <gpio_mcu.h>

typedef struct {
    gpio_t pin;
    io_t dir;
} gpioConf_t;

void setEstadoGPIO(int8_t digitoBCD, gpioConf_t *vectorGPIO);

#endif