/*
 * scale_sys.h
 *
 *  Created on: Jun 18, 2026
 *      Author: lucas
 */

#ifndef INC_SCALE_SYS_H_
#define INC_SCALE_SYS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

void ScaleSysInit(void);
void ScaleSysLoop(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* INC_SCALE_SYS_H_ */
