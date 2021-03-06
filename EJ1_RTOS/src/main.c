/*  Copyright 2020 - FreeRTOS Kernel V10.0.1

 *  Ejecutar dos tareas sincronizadas
 *  Tarea1 debe ejecutar de manera continua un busy waiting de 1 seg, indicando este estado por el puerto serie
 *  Tarea2 debe estar serializada, bloqueada hasta que la Tarea1 complete el loop, una vez que pase sto, debe cambiar el estado del led Azul
 *  Justificar herramienta de sincronizacion usada
 *  - xTaskCreate()
 *  - vTaskStartScheduler()
 *  - vTaskDelay(ticks)
 *
 *  */
 /*==================[inclusions]=============================================*/

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*==================[macros and definitions]=================================*/

#define TAM_PILA 150

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

xSemaphoreHandle sem;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void vTarea1(void *pvParameters){
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for(;;){
    xSemaphoreTake(sem, 10000 / portTICK_RATE_MS);
    printf("\r\nBusy Waiting Tarea1.\r\n" );
    vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_RATE_MS);
    printf("Fin Busy Waiting\r\n");
    xSemaphoreGive(sem);
    vTaskDelay(1);
  }
}

static void vTarea2( void *pvParameters){
  for(;;){
    xSemaphoreTake(sem, 10000 / portTICK_RATE_MS);
    Board_LED_Toggle(LED_BLUE);
    printf("Estado del LED azul: %d\r\n", Board_LED_Test(LED_BLUE));
    xSemaphoreGive(sem);
    vTaskDelay(1);
  }
}
/*==================[external functions definition]==========================*/

int main(void)
{

  vSemaphoreCreateBinary(sem);

	xTaskCreate(vTarea1, (const char *)"Tarea1", TAM_PILA, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate(vTarea2, (const char *)"Tarea2", TAM_PILA, NULL, tskIDLE_PRIORITY+1, NULL );

	vTaskStartScheduler(); /* y por ??ltimo se arranca el planificador . */
    //Nunca llegara a ese lazo  .... espero
     for( ;; );
     return 0;
}

/*==================[end of file]============================================*/
