/*
 * utils.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "utils.h"



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
