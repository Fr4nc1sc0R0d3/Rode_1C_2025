
#ifndef MOTOR_PWM
#define MOTOR_PWM

#include "pwm_mcu.h"
#include <gpio_mcu.h>
#include "hc_sr04_multiple_control.h"


void inicializarMotores(gpio_t pinMotor1, gpio_t pinMotor2, gpio_t pinMotor3);

void setPWM(uint8_t ct, uint8_t numeroMotor);

void unsetPWM(uint8_t numeroMotor);

void controlMotoresSegunMedicion(medicionesHCSRO4 mediciones);

uint8_t calcularCT(uint16_t ct);


#endif