/*
 * buttons.h
 *
 *  Created on: Jun 11, 2026
 *      Author: lucas
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bool.h"

#define BTN_STATE_PRESSED true
#define BTN_STATE_RELEASED false

typedef void (*ButtonCb_t)(void);

typedef struct {
	ButtonCb_t cb_click;
	ButtonCb_t cb_hold_start;
	ButtonCb_t cb_hold_stop;
} ButtonCallback_t;

typedef enum {
	BUTTON_PIN_OK = 0,
	BUTTON_PIN_R,
	BUTTON_PIN_L,
	BUTTON_PIN_BACK,
	COUNT_BUTTON_PINS
} Button_Pin;

typedef ButtonCallback_t ButtonArray_t[COUNT_BUTTON_PINS];

void ButtonInit(ButtonArray_t *btn_arr);
void ButtonSetState(Button_Pin btn_pin, GPIO_PinState state);
void ButtonRegisterCbClick(Button_Pin btn_pin, ButtonCb_t btn_cb_click);
void ButtonRegisterCbHoldStart(Button_Pin btn_pin, ButtonCb_t btn_cb_hold_start);
void ButtonRegisterCbHoldStop(Button_Pin btn_pin, ButtonCb_t btn_cb_hold_stop);
void ButtonAction(Button_Pin btn_pin);
void ButtonHoldTimerExpire(Button_Pin btn_pin);

#endif /* INC_BUTTONS_H_ */
