/*
 * hx711.h
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#ifndef INC_HX711_H_
#define INC_HX711_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bool.h"
#include "main.h"

#if defined(HX711_SCK_LoopBack_Pin) && defined(HX711_DOUT_LoopBack_Pin)
#define HX711_LOOP_BACK_TEST
#endif



void HX711Init(GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dout_gpio, uint16_t dout_pin);
void HX711Loop(void);

float HX711GetSlope(void);
float HX711GetOffset(void);

void HX711SetSlope(float slope);
void HX711SetOffset(float offset);
void HX711SetSampleReady(void);

void HX711Tare(void);
float HX711GetReadingAverage(void);
float HX711GetReadingRate(void);

#ifdef HX711_LOOP_BACK_TEST
void HX711LoopBackTestStart(void);
void HX711LoopBackTestSendData(void);
#endif


#endif /* INC_HX711_H_ */
