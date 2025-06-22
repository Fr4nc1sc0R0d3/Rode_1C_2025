#ifndef CONTROL_SWITCHES
#define CONTROL_SWITCHES

#include <gpio_mcu.h>

//    2
// 1     3
void inicializarSwitches(gpio_t gpio1, gpio_t gpio2, gpio_t gpio3);
void asociarInterrupciones(gpio_t gpioSwitch, void *ptrIntFunc);

#endif