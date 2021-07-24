/*  Copyright 2020 - FreeRTOS Kernel V10.0.1

2 - Cree un programa usando FreeRtos que cumpla con las siguientes consignas:

* Poseer dos tareas, Tarea1 y Tarea2, una cola de mansajes entre ellas y una rutina de atención
  de interrupción de timer RIT.
* La Tarea1 debe enviar elementos a la cola de mensajes cada 1500ms.
* La Tarea2 debe esperar de la interrupción del timer RIT (que será de 3 segundos) .
* Una vez que expira el timer, consumir un elemento, indicándolo por puerto serie el elemento consumido
y cambiar el estado del led Amarillo.
 *
 *  */
 /*==================[inclusions]=============================================*/

#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "supporting_functions.h"

/*==================[macros and definitions]=================================*/

#define TAM_PILA 150
#define TAM_COLA 100
#define TAM_MSG 16
#define MSG "Mensaje Enviado"
static xQueueHandle cola;
SemaphoreHandle_t sem;

/*-------------------------------------------------------------------------*/
static void initHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();
}

static void InitTimer(void)
{
    Chip_RIT_Init(LPC_RITIMER);
    Chip_RIT_SetTimerInterval(LPC_RITIMER,3000);
}

void RIT_IRQHandler( void ){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);

  if(xHigherPriorityTaskWoken == pdTRUE){
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }
  Chip_RIT_ClearInt(LPC_RITIMER); //Limpia el flag de interrupcion ¿?
}
/*----------------------------------TAREAS----------------------------------*/

static void vTarea1(void *pvParameters){
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for(;;){
    vTaskDelayUntil(1500 / pdMS_TO_TICKS, &xLastWakeTime);
    printf("Escribiendo en la cola...\r\n");
    xQueueSend(cola, MSG, (portTickType) 1000 );
  }
}

static void vTarea2( void *pvParameters){
  char *buff;
  for(;;){
    xSemaphoreTake(sem, (portTickType) 1000);
    xQueueReceive(cola, buff, (portTickType) 1000 );
    printf("Mensaje Leido: %s", buff);
    Board_LED_Toggle(3);
  }
}
/*==================[external functions definition]==========================*/

int main(void)
{
  initHardware();
  InitTimer();

  cola = xQueueCreate(TAM_COLA, TAM_MSG);
  sem = xSemaphoreCreateBinary();

	xTaskCreate(vTarea1, (const char *)"Tarea1", TAM_PILA, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate(vTarea2, (const char *)"Tarea2", TAM_PILA, NULL, tskIDLE_PRIORITY+1, NULL );

  NVIC_EnableIRQ(RITIMER_IRQn);
  NVIC_SetPriority(RITIMER_IRQn, (1<<__NVIC_PRIO_BITS) -1);

  vTaskStartScheduler(); /* y por último se arranca el planificador . */
    //Nunca llegara a ese lazo  .... espero
     for( ;; );
     return 0;
}

/*==================[end of file]============================================*/
