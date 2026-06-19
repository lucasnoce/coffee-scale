/*
 * timer.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bool.h"
#include "timer.h"


static TIM_HandleTypeDef *p_htim;
static bool timer_on = false;
static uint32_t timer_cyccnt = 0;
static uint32_t timer_msec = 0;


void TimerInit(TIM_HandleTypeDef *htim) {
	p_htim = htim;
	TimerStart();
	return;
}

void TimerIncrement(void) {
	if (timer_on == true) {
		uint32_t inc_us = (DWT->CYCCNT - timer_cyccnt) / 100;
		timer_cyccnt = DWT->CYCCNT;
		timer_msec += inc_us / 1000;
	}
}

void TimerStart(void) {
	timer_msec = 0;
	timer_on  = true;
	HAL_TIM_Base_Start_IT(p_htim);
}

void TimerPause(void) {
	timer_on = false;
	HAL_TIM_Base_Stop_IT(p_htim);
}

void TimerResume(void) {
	timer_on = true;
	HAL_TIM_Base_Start_IT(p_htim);
}

void TimerStop(void) { // TODO
	timer_on = false;
	HAL_TIM_Base_Stop_IT(p_htim);
}

void TimerCountdown(void) { // TODO
	return;
}

uint32_t TimerGetSeconds(void) {
	return (timer_msec / 1000);
}

