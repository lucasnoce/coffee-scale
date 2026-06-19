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

#define UTILS_GPIO_ALL_BTN_PINS          (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)
#define UTILS_GPIO_ALL_INT_PINS          (UTILS_GPIO_ALL_BTN_PINS | GPIO_PIN_11)
#define UTILS_TIM_DISPLAY_UPDATE         ((uint8_t)0x01)
#define UTILS_TIM_TIMER_INCREMENT        ((uint8_t)0x02)
#define UTILS_TIM_BTN_HOLD               ((uint8_t)0x04)
#define UTILS_TIM_HX711_LB_TEST_START    ((uint8_t)0x08)
#define UTILS_TIM_ALL                    ((uint8_t)0x0F)

#define UtilsDisableInterrupts() __disable_irq()
#define UtilsEnableInterrupts() __enable_irq()

void UtilsInit(void);

void UtilsStopInterruptsGPIO(uint16_t gpio_pins);
void UtilsResumeInterruptsGPIO(uint16_t gpio_pins);
void UtilsStopInterruptsTIM(uint8_t tim_instances);
void UtilsResumeInterruptsTIM(uint8_t tim_instances);

void DelayUs(uint32_t delay_us);

#endif /* INC_UTILS_H_ */
