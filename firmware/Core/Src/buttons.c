/*
 * buttons.c
 *
 *  Created on: Jun 11, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "buttons.h"
#include "bool.h"



static ButtonArray_t *p_btn_arr;
static bool is_holding = false;

static uint32_t tim_channel[COUNT_BUTTON_PINS] = {
	TIM_CHANNEL_1,
	TIM_CHANNEL_2,
	TIM_CHANNEL_3,
	TIM_CHANNEL_4
};

extern TIM_HandleTypeDef htim2;



static void startDetectionHold(Button_Pin btn_pin);
static void stopDetectionHold(Button_Pin btn_pin);



void ButtonInit(ButtonArray_t *btn_arr) {
	if (btn_arr == NULL)
		return;
	p_btn_arr = btn_arr;
}

void ButtonAction(ButtonEventData_t *btn_event_data) {
	if (btn_event_data->state == GPIO_PIN_SET)
		ButtonPress(btn_event_data->pin);
	else
		ButtonRelease(btn_event_data->pin);
}

void ButtonPress(Button_Pin btn_pin) {
	if (btn_pin >= COUNT_BUTTON_PINS || p_btn_arr[btn_pin]->cb_click == NULL)
		return;
	p_btn_arr[btn_pin]->cb_click();
	startDetectionHold(btn_pin);
}

void ButtonRelease(Button_Pin btn_pin) {
	if (btn_pin >= COUNT_BUTTON_PINS || p_btn_arr[btn_pin]->cb_hold_stop == NULL)
		return;
	p_btn_arr[btn_pin]->cb_hold_stop();
	stopDetectionHold(btn_pin);
}

void ButtonHoldTimerExpire(Button_Pin btn_pin) {
	if (is_holding == true || p_btn_arr[btn_pin]->cb_hold_start == NULL)
		return;

	is_holding = true;

	for (uint8_t i=0; i<COUNT_BUTTON_PINS; i++) {
		if (btn_pin != i)
			stopDetectionHold(btn_pin);
	}

	p_btn_arr[btn_pin]->cb_hold_start();  // blocking function

	stopDetectionHold(btn_pin);

	is_holding = false;
}



static void startDetectionHold(Button_Pin btn_pin) {
	uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SET_COMPARE(&htim2, tim_channel[btn_pin], now + 10);
	HAL_TIM_OC_Start_IT(&htim2, tim_channel[btn_pin]);
}

static void stopDetectionHold(Button_Pin btn_pin) {
	HAL_TIM_OC_Stop_IT(&htim2, tim_channel[btn_pin]);
}
