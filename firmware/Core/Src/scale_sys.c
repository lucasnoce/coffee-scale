/*
 * scale_sys.c
 *
 *  Created on: Jun 18, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "scale_sys.h"
#include "bool.h"
#include "events.h"
#include "utils.h"
#include "display.h"
#include "hx711.h"
#include "timer.h"
#include "buttons.h"



extern TIM_HandleTypeDef htim2;  // used for delay (utils) and display
extern TIM_HandleTypeDef htim5;  // used for timer feature
extern TIM_HandleTypeDef htim4;  // used for loop back test



void ScaleSysInit(void){
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim5);
	HAL_TIM_Base_Stop_IT(&htim4);

	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_2);
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_3);
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_4);

	UtilsInit();
	EventInit();
	DisplayInit();
	HX711Init(HX711_SCK_GPIO_Port, HX711_SCK_Pin, HX711_DOUT_GPIO_Port, HX711_DOUT_Pin);
	TimerInit(&htim5);

//	ButtonArray_t btn_arr = {0};
//	btn_arr[BUTTON_PIN_OK].cb_click = buttonCbTest;
//	btn_arr[BUTTON_PIN_R].cb_hold_start = buttonCbTest;
//	ButtonInit(&btn_arr);

	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim4);
}

void ScaleSysLoop(void){
	EventLoop();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == TIM2) {  // (T = 200ms) display refresh rate @5Hz
		TimerIncrement();
		EventQueue(EVENT_ID_DISPLAY_UPDATE, 0);
	}
	else if(htim->Instance == TIM5) {  // (T = 1s) timer
	}
	else if(htim->Instance == TIM4) {  // (T = 200ms) loop back test
	#ifdef HX711_LOOP_BACK_TEST_EN
		EventQueue(EVENT_ID_HX711_LB_TEST_START, 0);
	#endif
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM2) {
    	HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_1);
    	HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_2);
    	HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_3);
    	HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_4);
    	EventQueue(EVENT_ID_BUTTON_HOLD, (uintptr_t)(Button_Pin) (htim->Channel >> 1));
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_Pin);  // assumes all EXTI GPIOs are in Port A

	switch (GPIO_Pin) {
		#ifdef HX711_LOOP_BACK_TEST_EN
		case HX711_SCK_LoopBack_Pin:
			HX711LoopBackTestSendData();
			break;
		#endif

		case HX711_DOUT_Pin:
			EventQueue(EVENT_ID_HX711_SAMPLE_READY, 0);
			break;

//		case BTN_OK_Pin:
//			ButtonSetState(BUTTON_PIN_OK, state);
//			EventQueue(EVENT_ID_BUTTON_ACTION, (uintptr_t) BUTTON_PIN_OK);
//			break;
//
//		case BTN_R_Pin:
//			ButtonSetState(BUTTON_PIN_R, state);
//			EventQueue(EVENT_ID_BUTTON_ACTION, (uintptr_t) BUTTON_PIN_R);
//			break;
//
//		case BTN_L_Pin:
//			ButtonSetState(BUTTON_PIN_L, state);
//			EventQueue(EVENT_ID_BUTTON_ACTION, (uintptr_t) BUTTON_PIN_L);
//			break;
//
//		case BTN_BACK_Pin:
//			ButtonSetState(BUTTON_PIN_BACK, state);
//			EventQueue(EVENT_ID_BUTTON_ACTION, (uintptr_t) BUTTON_PIN_BACK);
//			break;

		default:
			break;
	}
}
