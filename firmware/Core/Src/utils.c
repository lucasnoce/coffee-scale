/*
 * utils.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "utils.h"



#ifdef HX711_LOOP_BACK_TEST
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
#endif



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

#ifdef HX711_LOOP_BACK_TEST
void stopInterrupts(void) {
	EXTI->IMR &= ~EXTI_IMR_IM11;
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim5);
}

void resumeInterrupts(void) {
	EXTI->PR = EXTI_PR_PR11;
	EXTI->IMR |= EXTI_IMR_IM11;
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim5);
}
#endif
