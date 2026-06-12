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
	GPIO_TypeDef *gpio;
	uint16_t pin;
} ButtonInfo_t;

typedef enum {
	BUTTON_PIN_OK = 0,
	BUTTON_PIN_R,
	BUTTON_PIN_L,
	BUTTON_PIN_BACK,
	COUNT_BUTTON_PINS
} Button_Pin;

typedef struct {
    Button_Pin pin;
    GPIO_PinState state;
} ButtonEventData_t;

typedef ButtonInfo_t ButtonArray_t[COUNT_BUTTON_PINS];

void ButtonInit(ButtonArray_t *btn_arr);
void ButtonAction(ButtonEventData_t *btn_event_data);
void ButtonPress(Button_Pin btn_pin);
void ButtonRelease(Button_Pin btn_pin);
void ButtonHoldTimerExpire(Button_Pin btn_pin);

#endif /* INC_BUTTONS_H_ */
