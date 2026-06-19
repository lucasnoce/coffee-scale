/*
 * events.c
 *
 *  Created on: Jun 11, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "events.h"
#include "hx711.h"
#include "buttons.h"
#include "display.h"
#include "utils.h"



#define EVENT_QUEUE_SIZE 16



static volatile EventId_t event_queue[EVENT_QUEUE_SIZE];
static volatile uintptr_t event_data[EVENT_QUEUE_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;



void EventInit(void) {
	for (uint8_t i=0; i<EVENT_QUEUE_SIZE; i++) {
		event_queue[i] = EVENT_ID_NONE;
	}
}

//static volatile uint32_t display_time = 0;
//static volatile uint32_t display_freq = 0;
void EventQueue(EventId_t evt_id, uintptr_t evt_data) {
	event_queue[head] = evt_id;
	event_data[head]  = evt_data;
	head = (head + 1) % EVENT_QUEUE_SIZE;

//	if (evt_id == EVENT_ID_DISPLAY_UPDATE) {
//		display_freq = (DWT->CYCCNT - display_time)/100;
//		display_time = DWT->CYCCNT;
//	}
}

void EventLoop(void) {
	uint32_t start = DWT->CYCCNT;
	uint8_t evt_dif = (head > tail) ? (head - tail) : (EVENT_QUEUE_SIZE - tail + head);
    if (tail != head) {
    	EventId_t event = event_queue[tail];
    	uintptr_t data  = event_data[tail];

        event_queue[tail] = EVENT_ID_NONE;
        event_data[tail]  = 0;

        tail = (tail + 1) % EVENT_QUEUE_SIZE;

        switch (event) {
            case EVENT_ID_DISPLAY_UPDATE:
            	DisplayUpdate();
                break;

            case EVENT_ID_HX711_SAMPLE_READY:
            	HX711Loop();
                break;

            case EVENT_ID_HX711_LB_TEST_START:
            	HX711LoopBackTestStart();
                break;

//            case EVENT_ID_BUTTON_ACTION:
//            	ButtonAction((Button_Pin) data);
//                break;
//
//            case EVENT_ID_BUTTON_HOLD:
//            	ButtonHoldTimerExpire((Button_Pin) data);
//                break;

            default:
                break;
        }
    }
    else {
    	HAL_Delay(1);
    }
	uint32_t end = DWT->CYCCNT;
	uint32_t dif = (DWT->CYCCNT - start)/100;
}

