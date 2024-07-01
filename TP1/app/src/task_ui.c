/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_button.h"
#include "task_ui.h"
/********************** macros and definitions *******************************/
/* Specify size of message queue from task_button to ao_ui */
#define UI_QUEUE_LENGTH 1
#define UI_QUEUE_ITEM_SIZE sizeof(ao_ui_handle_t)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern SemaphoreHandle_t hsem_button;
extern SemaphoreHandle_t hsem_led;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

/* active object handler function */
void task_ui(void *argument)
{
	ao_ui_handle_t * hao_ui = (ao_ui_handle_t *) argument; // handle with queue >> incoming messages from task button

	ao_ui_message_t msg;

	while (true)
	{
		// block while waiting for queue message
		if (pdPASS == xQueueReceive(hao_ui->hqueue, &msg, portMAX_DELAY))
		{
			LOGGER_INFO("UI >> message from task_button received: %d", (int) msg.body);
		}
		// than take action and send message to corresponding led
		// keep semaphore ??
		/*
		if(pdTRUE == xSemaphoreTake(hsem_button, portMAX_DELAY))
		{

		  LOGGER_INFO("ui led activate");
		  xSemaphoreGive(hsem_led);
		}
		*/



	}
}

void ao_ui_send(ao_ui_handle_t * hao_ui, ao_ui_message_t msg){
	xQueueSend(hao_ui->hqueue, (void*)&msg, 0);
}


/* active object function for queue and task creation */
void ao_ui_init( ao_ui_handle_t * hao_ui)
{
	/* Create message queue */
	hao_ui->hqueue = xQueueCreate(UI_QUEUE_LENGTH, UI_QUEUE_ITEM_SIZE); // receives task button messages
	while(NULL == hao_ui->hqueue)
	{
		// error
	}

	/* Create task with handler function passing the active object handle as a parameter */
	BaseType_t status = xTaskCreate(task_ui, "task_ui", 128, (void *) hao_ui, tskIDLE_PRIORITY, NULL);
	while (pdPASS != status)
	{
	// error
	}
}

/********************** end of file ******************************************/
