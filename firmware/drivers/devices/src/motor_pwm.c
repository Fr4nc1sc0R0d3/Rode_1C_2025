#include "motor_pwm.h"

#define UMBRAL 40


// Motores van en numero en una vista superior de izquierda a derecha (sentido rotacion) en sentido creciente.
// 1     3
//    2
void inicializarMotores(gpio_t pinMotor1, gpio_t pinMotor2, gpio_t pinMotor3) {
    PWMInit(PWM_0, pinMotor1, 2000);
    PWMInit(PWM_1, pinMotor2, 2000);
    PWMInit(PWM_2, pinMotor3, 2000);
}

void setPWM(uint8_t ct, uint8_t numeroMotor) {

    switch(numeroMotor) {
        case 1:
            PWMSetDutyCycle(PWM_0,ct);
            PWMOn(PWM_0);
            break;
        case 2:
            PWMSetDutyCycle(PWM_1,ct);
            PWMOn(PWM_1);
            break;
        case 3:
            PWMSetDutyCycle(PWM_2,ct);
            PWMOn(PWM_2);
            break;
    }

}

void unsetPWM(uint8_t numeroMotor) {

    switch(numeroMotor) {
        case 1:
            PWMSetDutyCycle(PWM_0,0);
            PWMOff(PWM_0);
            break;
        case 2:
            PWMSetDutyCycle(PWM_1,0);
            PWMOff(PWM_1);
            break;
        case 3:
            PWMSetDutyCycle(PWM_2,0);
            PWMOff(PWM_2);
            break;
    }
}

uint8_t calcularCT(uint16_t ct) {
    return (-100/UMBRAL)*(ct) + 100;
}

void controlMotoresSegunMedicion(medicionesHCSRO4 mediciones) {

    if (mediciones.s1 <= UMBRAL) {
        setPWM(calcularCT(mediciones.s1),1);
    } else {
        unsetPWM(1);
    }

    if (mediciones.s2 <= UMBRAL || mediciones.s3 <= UMBRAL || mediciones.s4 <= UMBRAL) {
        uint16_t medicionMasPequena = UMBRAL;
        if (mediciones.s2 <= UMBRAL && mediciones.s2 < medicionMasPequena)
            medicionMasPequena = mediciones.s2;
        if (mediciones.s3 <= UMBRAL && mediciones.s3 < medicionMasPequena)
            medicionMasPequena = mediciones.s3;
        if (mediciones.s4 <= UMBRAL && mediciones.s4 < medicionMasPequena)
            medicionMasPequena = mediciones.s4;
        setPWM(calcularCT(medicionMasPequena), 2);
    } else {
        unsetPWM(2);
    }

    if (mediciones.s5 <= UMBRAL) {
        setPWM(calcularCT(mediciones.s5),3);
    } else {
        unsetPWM(3);
    }
}