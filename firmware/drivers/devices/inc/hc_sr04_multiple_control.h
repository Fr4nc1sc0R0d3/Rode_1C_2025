#ifndef HC_SR04_MULTIPLE_CONTROL_H
#define HC_SR04_MULTIPLE_CONTROL_H

#include "gpio_mcu.h"
#include "hc_sr04.h"

typedef struct {
    gpio_t echo;
    gpio_t trigger;
} HCSR04;

typedef struct {
    uint16_t s1;
    uint16_t s2;
    uint16_t s3;
    uint16_t s4;
    uint16_t s5;
} medicionesHCSRO4;


medicionesHCSRO4 obtenerMediciones(HCSR04 *vectorGPIOs);

#endif
