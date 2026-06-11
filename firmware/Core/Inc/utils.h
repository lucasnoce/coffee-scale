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

#define stopInterrupts() __disable_irq()
#define resumeInterrupts() __enable_irq()

void UtilsInit(void);
void DelayUs(uint32_t delay_us);

#endif /* INC_UTILS_H_ */
