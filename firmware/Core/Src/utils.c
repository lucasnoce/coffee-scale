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



#ifdef HX711_LOOP_BACK_TEST_EN

static void stopInterruptsTimers(void) {
//	EXTI->IMR &= ~EXTI_IMR_IM11;
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim5);
}

static void resumeInterruptsTimers(void) {
//	EXTI->PR = EXTI_PR_PR11;
//	EXTI->IMR |= EXTI_IMR_IM11;
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim5);
}

static void stopInterruptsGPIO(void) {
	EXTI->IMR &= ~(EXTI_IMR_IM3  |
	               EXTI_IMR_IM4  |
	               EXTI_IMR_IM5  |
	               EXTI_IMR_IM6  |
	               EXTI_IMR_IM11);
}

static void resumeInterruptsGPIO(void) {
	// Clear pending interrupts
	EXTI->PR = GPIO_PIN_3 |
	           GPIO_PIN_4 |
	           GPIO_PIN_5 |
	           GPIO_PIN_6 |
	           GPIO_PIN_11;

	EXTI->IMR |= (EXTI_IMR_IM3  |
	              EXTI_IMR_IM4  |
	              EXTI_IMR_IM5  |
	              EXTI_IMR_IM6  |
	              EXTI_IMR_IM11);
}

void UtilsStopInterruptsAll(void) {
	stopInterruptsTimers();
	stopInterruptsGPIO();
}

void UtilsResumeInterruptsAll(void) {
	resumeInterruptsTimers();
	resumeInterruptsGPIO();
}

#endif
