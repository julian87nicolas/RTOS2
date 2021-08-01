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

xSemaphoreHandle sema, semb;

int a = 1, b = 2;
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void vTarea1(void *pvParameters){
  for(;;){
    xSemaphoreTake(sema, 1000 / portTICK_RATE_MS);
    xSemaphoreTake(semb, 1000 / portTICK_RATE_MS);
    a = b;
    printf("Dentro de la tarea1\r\n");
    xSemaphoreGive(semb);
    xSemaphoreGive(sema);
  }
}

static void vTarea2( void *pvParameters){
  for(;;){
    xSemaphoreTake(semb, 1000 / portTICK_RATE_MS);
    xSemaphoreTake(sema, 1000 / portTICK_RATE_MS);
    b = a;
    printf("Dentro de la tarea2\r\n");
    xSemaphoreGive(sema);
    xSemaphoreGive(semb);
  }
}
void vApplicationIdleHook( void ){
  printf("\r\nABRAZO MORTAL!\r\n");
}

/*==================[external functions definition]==========================*/

int main(void)
{

  vSemaphoreCreateBinary(sema);
  vSemaphoreCreateBinary(semb);
	xTaskCreate(vTarea1, (const char *)"Tarea1", TAM_PILA, NULL, tskIDLE_PRIORITY+1 , NULL );
	xTaskCreate(vTarea2, (const char *)"Tarea2", TAM_PILA, NULL, tskIDLE_PRIORITY+1, NULL );

	vTaskStartScheduler(); /* y por último se arranca el planificador . */
    //Nunca llegara a ese lazo  .... espero
     for( ;; );
     return 0;
}

/*==================[end of file]============================================*/
