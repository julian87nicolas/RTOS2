/*  Copyright 2020 - FreeRTOS Kernel V10.0.1

3 - Cree un programa usando FreeRtos que cumpla con las siguientes consignas:

* Posea tres tareas, la Tarea1, Tarea2 y Tarea3.
* La Tarea1, debe tener la mayor prioridad, la Tarea2 media prioridad y Tarea3 baja prioridad.
* La Tarea1 y la Tarea3 comparten el puerto serie, enviando un mensaje cada una periodicamente, cada 800 milisegundos.
* La Tarea2 es una tarea periódica, que ejecuta de manera contínua un bucle de busy waiting de 1 segundo aprox , luego cambia el led Rojo cada y genera un delay de 200milisegundos.
* Plantee su solución para evitar inversión de prioridad*/
/*
    LED_1 = amarillo
    LED_2 = rojo
    LED_3 = verde
*/
 /*==================[inclusions]=============================================*/

 #include "board.h"
 #include "FreeRTOS.h"
 #include "FreeRTOSConfig.h"
 #include "task.h"
 #include "semphr.h"
 #include "queue.h"
 #include "chip.h"
 #include "string.h"
 #include "stdlib.h"

/*==================[macros and definitions]=================================*/

#define TAM_PILA 150

SemaphoreHandle_t sem, semT;

/*-------------------------------------------------------------------------*/
static void initHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();
}

static void InitTimer(void)
{
    Chip_RIT_Init(LPC_RITIMER);
    Chip_RIT_SetTimerInterval(LPC_RITIMER,1000);
}

void RIT_IRQHandler( void ){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
  printf("Inicio INTERRUPCION\r\n");
  if(xHigherPriorityTaskWoken == pdTRUE){
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }
  printf("Fin INTERRUPCION\r\n");
  Chip_RIT_ClearInt(LPC_RITIMER); //Limpia el flag de interrupcion ¿?
}
/*----------------------------------TAREAS----------------------------------*/

static void vTarea1(void *pvParameters){
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for(;;){
    //xSemaphoreTake(semT, 1000 / portTICK_RATE_MS);
    printf("Mensaje en puerto serie - %s -.\r\n", (void*) pvParameters);
    vTaskDelayUntil(&xLastWakeTime, 800 / portTICK_RATE_MS);
    //xSemaphoreGive(semT);
  }
}

static void vTarea2( void *pvParameters){
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for(;;){
    if(xSemaphoreTake(sem, (portTickType) 1000) == pdTRUE){
      vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_RATE_MS);
      Board_LED_Toggle(LED_2);
      vTaskDelay(200 / portTICK_RATE_MS);
    }
  }
}
/*==================[external functions definition]==========================*/

int main(void)
{
  initHardware();
  InitTimer();

  sem = xSemaphoreCreateBinary();
  semT = xSemaphoreCreateBinary();

	xTaskCreate(vTarea1, (const char *)"Tarea1", TAM_PILA, (void *)"Tarea1", tskIDLE_PRIORITY+3, NULL );
	xTaskCreate(vTarea2, (const char *)"Tarea2", TAM_PILA, NULL, tskIDLE_PRIORITY+2, NULL );
  xTaskCreate(vTarea1, (const char *)"Tarea3", TAM_PILA, (void *)"Tarea3", tskIDLE_PRIORITY+1, NULL );

  NVIC_EnableIRQ(RITIMER_IRQn);
  NVIC_SetPriority(RITIMER_IRQn, (1<<__NVIC_PRIO_BITS) -1);

  vTaskStartScheduler();
     for( ;; );
     return 0;
}

/*==================[end of file]============================================*/
