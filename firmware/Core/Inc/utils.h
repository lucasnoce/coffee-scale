/*
 * utils.h
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "hx711_test.h"

#ifdef HX711_LOOP_BACK_TEST
void UtilsStopInterruptsAll(void);
void UtilsResumeInterruptsAll(void);
#else
#define UtilsStopInterruptsAll() __disable_irq()
#define UtilsResumeInterruptsAll() __enable_irq()
#endif

void UtilsInit(void);
void DelayUs(uint32_t delay_us);

#endif /* INC_UTILS_H_ */
