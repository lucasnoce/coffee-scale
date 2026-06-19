/*
 * utils.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "utils.h"



extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;



void UtilsInit(void) {
	/* Configure delay parameters */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DelayUs(uint32_t delay_us) {
	uint32_t start = DWT->CYCCNT;
	uint32_t ticks = delay_us * (SystemCoreClock / 1000000);
	while ((DWT->CYCCNT - start) < ticks);
}

void UtilsStopInterruptsGPIO(uint16_t gpio_pins) {
	EXTI->IMR &= ~gpio_pins;
}

void UtilsResumeInterruptsGPIO(uint16_t gpio_pins) {
	EXTI->PR = gpio_pins;
	EXTI->IMR |= gpio_pins;
}

void UtilsStopInterruptsTIM(uint8_t tim_instances) {
	if (tim_instances & (UTILS_TIM_DISPLAY_UPDATE | UTILS_TIM_TIMER_INCREMENT | UTILS_TIM_BTN_HOLD)) {
		HAL_TIM_Base_Stop_IT(&htim2);
	}
	if (tim_instances & (UTILS_TIM_HX711_LB_TEST_START)) {
		HAL_TIM_Base_Stop_IT(&htim4);
	}
}

void UtilsResumeInterruptsTIM(uint8_t tim_instances) {
	if (tim_instances & (UTILS_TIM_DISPLAY_UPDATE | UTILS_TIM_TIMER_INCREMENT | UTILS_TIM_BTN_HOLD)) {
		HAL_TIM_Base_Start_IT(&htim2);
	}
	if (tim_instances & (UTILS_TIM_HX711_LB_TEST_START)) {
		HAL_TIM_Base_Start_IT(&htim4);
	}
}

