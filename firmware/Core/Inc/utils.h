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
#include "hx711.h"

#ifdef HX711_LOOP_BACK_TEST
void stopInterrupts(void);
void resumeInterrupts(void);
#else
#define stopInterrupts() __disable_irq()
#define resumeInterrupts() __enable_irq()
#endif

void UtilsInit(void);
void DelayUs(uint32_t delay_us);

#endif /* INC_UTILS_H_ */
