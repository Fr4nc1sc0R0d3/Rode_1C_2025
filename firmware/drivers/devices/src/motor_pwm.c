#include "motor_pwm.h"




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

float calcularCT(uint16_t ct, float umbral) {
    return (-100/umbral)*(ct) + 100;
}

void controlMotoresSegunMedicion(uint16_t *medicionesSensores, float umbral) {

    if (medicionesSensores[0] <= umbral) {
        setPWM(calcularCT(medicionesSensores[0], umbral),1);
    } else {
        unsetPWM(1);
    }

    if (medicionesSensores[1] <= umbral || medicionesSensores[2] <= umbral || medicionesSensores[3] <= umbral) {
        uint16_t medicionMasPequena = umbral;
        if (medicionesSensores[1] <= umbral && medicionesSensores[1] < medicionMasPequena)
            medicionMasPequena = medicionesSensores[1];
        if (medicionesSensores[2] <= umbral && medicionesSensores[2] < medicionMasPequena)
            medicionMasPequena = medicionesSensores[2];
        if (medicionesSensores[3] <= umbral && medicionesSensores[3] < medicionMasPequena)
            medicionMasPequena = medicionesSensores[3];
        setPWM(calcularCT(medicionMasPequena, umbral), 2);
    } else {
        unsetPWM(2);
    }

    if (medicionesSensores[4] <= umbral) {
        setPWM(calcularCT(medicionesSensores[4], umbral),3);
    } else {
        unsetPWM(3);
    }
}