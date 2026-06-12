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



static ButtonArray_t btn_arr;
static GPIO_PinState btn_state[COUNT_BUTTON_PINS];
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
static void buttonPress(Button_Pin btn_pin);
static void buttonRelease(Button_Pin btn_pin);



void ButtonInit(ButtonArray_t *p_btn_arr) {
	if (p_btn_arr == NULL)
		return;

	for (uint8_t i=0; i<COUNT_BUTTON_PINS; i++) {
		ButtonRegisterCbClick(i, p_btn_arr[i]->cb_click);
		ButtonRegisterCbHoldStart(i, p_btn_arr[i]->cb_hold_start);
		ButtonRegisterCbHoldStop(i, p_btn_arr[i]->cb_hold_stop);
	}
}

void ButtonSetState(Button_Pin btn_pin, GPIO_PinState state) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;
	btn_state[btn_pin] = state;
}

void ButtonRegisterCbClick(Button_Pin btn_pin, ButtonCb_t btn_cb_click) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;
	btn_arr[btn_pin].cb_click = btn_cb_click;
}
void ButtonRegisterCbHoldStart(Button_Pin btn_pin, ButtonCb_t btn_cb_hold_start) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;
	btn_arr[btn_pin].cb_hold_start = btn_cb_hold_start;
}
void ButtonRegisterCbHoldStop(Button_Pin btn_pin, ButtonCb_t btn_cb_hold_stop) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;
	btn_arr[btn_pin].cb_hold_stop = btn_cb_hold_stop;
}

void ButtonAction(Button_Pin btn_pin) {
	if (btn_state[btn_pin] == GPIO_PIN_SET)
		buttonPress(btn_pin);
	else
		buttonRelease(btn_pin);
}

void ButtonHoldTimerExpire(Button_Pin btn_pin) {
	if (is_holding == true)
		return;

	is_holding = true;

	for (uint8_t i=0; i<COUNT_BUTTON_PINS; i++) {
		if (btn_pin != i)
			stopDetectionHold(btn_pin);
	}

	if (btn_arr[btn_pin].cb_hold_start != NULL)
		btn_arr[btn_pin].cb_hold_start();  // blocking function

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


static void buttonPress(Button_Pin btn_pin) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;

	if (btn_arr[btn_pin].cb_click != NULL)
		btn_arr[btn_pin].cb_click();

	startDetectionHold(btn_pin);
}

static void buttonRelease(Button_Pin btn_pin) {
	if (btn_pin >= COUNT_BUTTON_PINS)
		return;

	if (btn_arr[btn_pin].cb_hold_stop != NULL)
		btn_arr[btn_pin].cb_hold_stop();

	stopDetectionHold(btn_pin);
}
