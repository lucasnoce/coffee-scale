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
#include "hx711_test.h"
#include "bool.h"



typedef struct {
	GPIO_TypeDef *sck_gpio;
	GPIO_TypeDef *dout_gpio;
	uint16_t     sck_pin;
	uint16_t     dout_pin;
} HX711Config_t;



bool HX711Init(HX711Config_t *cfg);
void HX711Loop(void);

float HX711GetSlope(void);
float HX711GetOffset(void);

void HX711SetSlope(float slope);
void HX711SetOffset(float offset);

void HX711Tare(void);
float HX711GetReadingAverage(void);
float HX711GetReadingRate(void);



#endif /* INC_HX711_H_ */
