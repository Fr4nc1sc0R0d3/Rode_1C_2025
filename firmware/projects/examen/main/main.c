/*! @mainpage Examen parcial EP: "estación de monitoreo"
 *
 * @section genDesc Descripcion general
 *
 * Luego de la misteriosa nevada mortal que cubrió Buenos Aires, un pequeño grupo 
 * de sobrevivientes intenta construir una estación de monitoreo ambiental 
 * para detectar indicios de una nueva nevada tóxica y la presencia de radiación.
 * El sistema está compuesto por un sensor de humedad y temperatura DHT11 
 * y un  sensor analógico de radiación.
 * Por un lado se debe detectar el riesgo de nevada, la cual se da si la húmedad 
 * se encuentra por encima del 85%  y la temperatura entre 0 y 2ºC. 
 * Para esto se deben tomar muestras cada 1 segundo y se envían por UART 
 * con el siguiente formato:
 * “Temperatura: 10ºC - Húmedad: 70%”
 * Si se da la condición de riesgo de nevada se debe indicar el estado encendiendo 
 * el led Rojo de la placa, además del envío de un mensaje por la UART:
 * “Temperatura: 1ºC - Húmedad: 90% - RIESGO DE NEVADA”
 * Además se debe monitorizar la radiación ambiente, para ello se cuenta 
 * con un sensor analógico que da una salida de 0V para 0mR/h y 3.3V 
 * para una salida de 100 mR/h. Se deben tomar muestras de radiación cada 5 segundos, 
 * enviando el mensaje por UART:
 * “Radiación 30mR/h”
 * Si se sobrepasan los 40mR/h se debe informar del riesgo por Radiación, 
 * encendiendo el led Amarillo de la placa, y enviando en el mensaje:
 * “Radiación 50mR/h - RADIACIÓN ELEVADA”
 * Si no hay riesgo de nevada ni radiación excesiva, se indicará con el led Verde esta situación.
 * El botón 1 se utilizará para encender el dispositivo, 
 * comenzando el muestreo de los sensores y el envío de información. 
 * El botón 2 apaga el dispositivo, deteniendo el proceso de muestreo 
 * y apagando todas las notificaciones.
 *
 *
 * @section hardConn Conexion de hardware
 *
 * |   	DHT11		|   ESP-EDU 	|
 * |:--------------:|:--------------|
 * | 	VCC     	|	3V3     	|
 * | 	DATA	 	| 	GPIO_12		|
 * | 	GND		 	| 	GND 		|
 * 
 * |   	Sensor de radiacion		|   ESP-EDU 	|
 * |:--------------------------:|:--------------|
 * | 	VCC     	            |	3V3         |
 * | 	DATA	 	            | 	GPIO_1(CH1) |
 * | 	GND		 	            | 	GND 		|                      |
 *
 * @author Francisco Rode (francisco.rode@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <dht11.h>
#include <analog_io_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_mcu.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/

#define CANAL_INGRESO_SENAL CH1
#define PERIODO_MUESTREO_SENSOR_TyH 1000000
#define PERIODO_MUESTREO_SENSOR_RADIACION 5000000
#define GPIO_SENSOR_TyH GPIO_12

/*==================[internal data definition]===============================*/

/**
 * @brief Handler de tarea asociada a medicion de temperatura y humedad.
*/
TaskHandle_t handlerTareaTyH;

/**
 * @brief Handler de tarea asociada a medicion de radiacion.
*/
TaskHandle_t handlerTareaRadiacion;

/**
 * @brief Bandera que controla el estado del sistema (true indica encendido y false apagado).
*/
bool encendido;

/**
 * @brief Variable que almacena la humedad mediada por parte del sensor dht11.
*/
float humedad;

/**
 * @brief Variable que almacena la temperatura mediada por parte del sensor dht11.
*/
float temperatura;

/**
 * @brief Variable que almacena la tension sensada por el conversor analogico digital (CAD).
*/
uint16_t voltaje = 0;

/**
 * @brief Variable que almacena la radiacion mediada por parte del sensor dht11.
*/
float radiacionMedida;

/**
 * @brief Bandera que controla si la radiacion es alta (true indica que es alta y false que es normal).
*/
bool radiacionAlta;

/**
 * @brief Bandera que controla si hay riesgo de nevada (true indica que si hay riesgo, y false que no hay riesgo).
*/
bool riesgoNevada;



/*==================[internal functions declaration]=========================*/

/**
 * @brief Convierte la señal del sensor a radiación en mR/h.
 * 
 * @param voltajeSensor Valor leído del CAD.
 * @return Radiación equivalente en mR/h.
 */
float devolverRadiacionSensada(uint16_t voltajeSensor) {
	return (100/3.3)*voltajeSensor;
}

/**
 * @brief Función de callback para el temporizador de temperatura y humedad.
 */
void FuncTimerA(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaTyH, pdFALSE);
}

/**
 * @brief Función de callback para el temporizador de radiación.
 */
void FuncTimerB(void* param)
{
	vTaskNotifyGiveFromISR(handlerTareaRadiacion, pdFALSE);
}

/**
 * @brief Controla el LED verde si no hay riesgos presentes.
 */
void FuncTimerC(void *param) {
	if (!radiacionAlta && !riesgoNevada) {
		LedOff(LED_3);
		LedOff(LED_2);
		LedOn(LED_1);
	}
}

/**
 * @brief Tarea que mide y evalúa la radiación ambiental, y envia datos por UART.
 */
static void tareaMedirRadiacion(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (encendido) {
			char bufferR[15];
			AnalogInputReadSingle(CANAL_INGRESO_SENAL, &voltaje);
			sprintf(bufferR, "%.2f", (float)voltaje);
			UartSendString(UART_PC, "Radiación: ");
			UartSendString(UART_PC, bufferR);
			UartSendString(UART_PC, "mR/h");
			if (devolverRadiacionSensada(voltaje) > 40) {
				radiacionAlta = true;
				LedOn(LED_2);
				UartSendString(UART_PC, " - RADIACIÓN ELEVADA");
			}
			radiacionAlta = false;
			UartSendString(UART_PC, " \r\n");
		}
		
	}
}

/**
 * @brief Tarea que mide temperatura y humedad, detecta riesgo de nevada, y envia datos por UART.
 */
static void tareaMedirTyH(void *pvParameter){

	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (encendido) {
			if(dht11Read(&humedad, &temperatura)) {
				char bufferH[10];
				char bufferT[10];
				sprintf(bufferT, "%.2f", temperatura);
				sprintf(bufferH, "%.2f", temperatura);
				UartSendString(UART_PC, "Temperatura: ");
				UartSendString(UART_PC, bufferT);
				UartSendString(UART_PC, "°C - ");
				UartSendString(UART_PC, "Húmedad: ");
				UartSendString(UART_PC, bufferH);
				UartSendString(UART_PC, "%");
				if (humedad > 85) {
					if (temperatura > 0 && temperatura < 2) {
						LedOn(LED_3);
						UartSendString(UART_PC, " - RIESGO DE NEVADA");
						UartSendString(UART_PC, " \r\n");
						riesgoNevada = true;

					} else {
						UartSendString(UART_PC, " \r\n");
						riesgoNevada = false;
					}
				} else {
					riesgoNevada = false;
					UartSendString(UART_PC, " \r\n");
				}
				
			}
		}
		

	}
}

/**
 * @brief Enciende el dispositivo (habilita muestreo y notificaciones).
 */
void encenderDispositivo() {
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
	TimerStart(TIMER_C);
	encendido = true;
	
}

/**
 * @brief Apaga el dispositivo y todas las señales visuales tanto como los timers.
 */
void apagarDispositivo() {
	LedOff(LED_1);
	LedOff(LED_2);
	LedOff(LED_3);
	TimerStop(TIMER_A);
	TimerStop(TIMER_B);
	TimerStop(TIMER_C);
	encendido = false;
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal del programa.
 */
void app_main(void){

	analog_input_config_t configAnalog = {
		.input = CANAL_INGRESO_SENAL,
		.mode = ADC_SINGLE,
		.func_p = NULL, 
		.param_p = NULL
	};

	timer_config_t timerMuestreoTyH = {
        .timer = TIMER_A,
        .period = PERIODO_MUESTREO_SENSOR_TyH,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	timer_config_t timerControlLeds = {
		.timer = TIMER_C,
		.period = PERIODO_MUESTREO_SENSOR_TyH/2,
		.func_p = FuncTimerB,
		.param_p = NULL
	};

	timer_config_t timerMuestreoRadiacion = {
        .timer = TIMER_B,
        .period = PERIODO_MUESTREO_SENSOR_RADIACION,
        .func_p = FuncTimerC,
        .param_p = NULL
    };

	serial_config_t configPuertoSerie = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL
	};
	
	TimerInit(&timerMuestreoRadiacion);
	TimerInit(&timerMuestreoTyH);
	TimerInit(&timerControlLeds);
	AnalogInputInit(&configAnalog);
	UartInit(&configPuertoSerie);
	LedsInit();
	SwitchesInit();
	dht11Init(GPIO_SENSOR_TyH);
	SwitchActivInt(SWITCH_1, encenderDispositivo, 0);
    SwitchActivInt(SWITCH_2, apagarDispositivo, 0);
	xTaskCreate(&tareaMedirRadiacion, "tareaMedirRadiacion", 2048, NULL, 5, &handlerTareaRadiacion);
	xTaskCreate(&tareaMedirTyH, "tareaMedirTyH", 2048, NULL, 5, &handlerTareaTyH);
	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
	

}
/*==================[end of file]============================================*/