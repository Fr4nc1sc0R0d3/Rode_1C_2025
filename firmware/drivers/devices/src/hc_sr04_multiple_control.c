#include "hc_sr04_multiple_control.h"


medicionesHCSRO4 obtenerMediciones(HCSR04 *vectorGPIOs) {
    medicionesHCSRO4 mediciones = {0};

    // Lo dejo en i=1 para testear solo el primero.
    for (uint8_t i = 0; i <= 4; i++) {
        HcSr04Init(vectorGPIOs[i].echo, vectorGPIOs[i].trigger);
        uint16_t distancia = HcSr04ReadDistanceInCentimeters();
        switch (i) {
            case 0: mediciones.s1 = distancia; break;
            case 1: mediciones.s2 = distancia; break;
            case 2: mediciones.s3 = distancia; break;
            case 3: mediciones.s4 = distancia; break;
            case 4: mediciones.s5 = distancia; break;
        }
    }

    return mediciones;
}

