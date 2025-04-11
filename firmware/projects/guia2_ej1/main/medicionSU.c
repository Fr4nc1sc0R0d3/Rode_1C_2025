/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1, LED_2 and LED_3 blink at different rates, using FreeRTOS tasks.
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include <hc_sr04.h>

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1 1000
#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500
/*==================[internal data definition]===============================*/
TaskHandle_t visualizarTaskHandle = NULL;
TaskHandle_t medirTaskHandle = NULL;
TaskHandle_t teclasTaskHandle = NULL;
extern bool medicionActivada = false; 
extern bool visualizacionActivada = false;
extern uint16_t medicionSensor;
typedef struct {
    gpio_t pin;
    io_t dir;
} gpioConf_t;
extern gpioConf_t gpioConfiguracion[4]= {{GPIO_20, GPIO_OUTPUT},{GPIO_21, GPIO_OUTPUT},{GPIO_22, GPIO_OUTPUT},{GPIO_23, GPIO_OUTPUT}};
extern gpioConf_t gpioSeleccion[3] = {{GPIO_19, GPIO_OUTPUT},{GPIO_18, GPIO_OUTPUT}, {GPIO_9, GPIO_OUTPUT}};

/*==================[internal functions declaration]=========================*/

int8_t getSwitchPresionada() {
    teclas  = SwitchesRead();
    switch(teclas){
        case SWITCH_1:
            return 1
        break;
        case SWITCH_2:
            return 2;
        break;
    }
}

int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {

	// Retorna 0, en caso de exito, y -1 en caso contrario.

    // Verificar que el puntero no sea nulo
    if (bcd_number == NULL) {
        return -1; // Error: puntero nulo
    }

    // Verificar que el número de dígitos sea válido
    if (digits == 0 || digits > 10) {
        return -1; // Error: número de dígitos inválido, ya que un numero de 32 bits se representa con 10 digitos, y la cantidad de digitos no puede ser 0, ya que no se estaria extrayendo ningun digito.
    }

    for (int i=0; i<digits; i++) {
		bcd_number[i]=data%10;  				
		data/=10;                               
	}
	
    return 0; // Éxito
}

void BCDToGPIO(int8_t digitoBCD, gpioConf_t *vectorGPIO) {
    
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

void encenderLedsSegunMedicion(uint16_t medicion) {
    if (medicion < 10) {
        LedOff(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    } 
    if (medicion > 10 && medicion < 20) {
        LedOn(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    }
    if (medicion > 20 && medicion < 30) {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOff(LED_3);
    }

    if (medicion > 30) {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOn(LED_3);
    }
}

void encenderLED(uint16_t medicion) {
    uint8_t BCDArray[3];

    convertToBcdArray(medicion, 3, BCDArray);
    for(int8_t i = 0; i < 3; i ++) {
        BCDToGPIO(BCDArray[i],gpioConfiguracion);
        GPIOOn(gpioSeleccion[i].pin);
        vTaskDelay(pdMS_TO_TICKS(5));
        GPIOOff(gpioSeleccion[i].pin);
    }

}



static void medirTask(void *pvParameter){
    while(true){
        medicionSensor = HcSr04ReadDistanceInCentimeters();
        encenderLedsSegunMedicion(medicionSensor)
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

static void visualizarTask(void *pvParameter){
    while(true){
        if (medicionActivada) {
            encenderLED(medicionSensor);
        }
    }
}

static void teclasTask(void *pvParameter){
    while(true){
        int8_t numeroSwitch = getSwitchPresionada();
        if (numeroSwitch == 1) {
            medicionActivada = !medicionActivada;
            visualizacionActivada = medicionActivada;
        } if (numeroSwitch == 2) {
            medicionActivada = !medicionActivada;
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    LedsInit();
    
 
    for(int8_t i = 0; i < 4; i ++) {
        GPIOInit(gpioConfiguracion[i].pin,gpioConfiguracion[i].dir);
    }
    for(int8_t i = 0; i < 3; i ++) {
        GPIOInit(gpioSeleccion[i].pin, gpioSeleccion[i].dir);
    }



    if (!HcSr04Init(GPIO_3, GPIO_2)) {
        printf("Error al inicializar sensor ultrasonico.");
        return;
    }
    xTaskCreate(&visualizarTask, "visualizarTask", 512, NULL, 5, &visualizarTaskHandle);
    xTaskCreate(&medirTask, "medirTask", 512, NULL, 5, &medirTaskHandle);
    xTaskCreate(&teclasTask, "teclasTask", 512, NULL, 5, &teclasTaskHandle);
}
