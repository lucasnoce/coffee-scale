/*
 * hx711.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "hx711.h"
#include "bool.h"
#include "utils.h"

#define BUF_SIZE 10



typedef struct {
	GPIO_TypeDef  *sck_gpio;
	GPIO_TypeDef  *dout_gpio;
	float         slope;
	int32_t       offset;
	uint16_t      sck_pin;
	uint16_t      dout_pin;
} HX711_t;



static HX711_t hx711;
static bool hx711_init = false;

static float reading_buf[BUF_SIZE] = {0};
static float reading_avrg = 0.0;
static float reading_rate = 0.0;
static uint8_t buf_pos = BUF_SIZE - 1;

bool sample_ready = false;



static void readSample(void);
static float calculateRate(void);



void HX711Init(GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dout_gpio, uint16_t dout_pin) {
	if (hx711_init == true || sck_gpio == NULL || dout_gpio == NULL) {
		return;
	}

	hx711.sck_gpio  = sck_gpio;
	hx711.sck_pin   = sck_pin;
	hx711.dout_gpio = dout_gpio;
	hx711.dout_pin  = dout_pin;
	hx711.slope     = 1;
	hx711.offset    = 0;

	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	HAL_Delay(10);

	hx711_init = true;
}

float HX711GetSlope(void) {
	return hx711_init == false ? 0.0 : hx711.slope;
}

int32_t HX711GetOffset(void) {
	return hx711_init == false ? 0 : hx711.offset;
}

void HX711SetSlope(float slope) {
	if (hx711_init == false) return;
	hx711.slope = slope;
}

void HX711SetOffset(int32_t offset) {
	if (hx711_init == false) return;
	hx711.offset = offset;
}

void HX711SetSampleReady(void) {
	sample_ready = true;
}

void HX711Tare(void) {
	if (hx711_init == false) return;
	hx711.offset = (int32_t) (reading_avrg / BUF_SIZE);
}

float HX711GetReadingAverage(void) {
	return (reading_avrg - (float) hx711.offset) / hx711.slope;
}

float HX711GetReadingRate(void) {
	return reading_rate;
}

void HX711LoopBackTest(void) {
	// TODO
}

void HX711Loop(void) {
	if (hx711_init == false) return;

	if (sample_ready == true) {
		sample_ready = false;
		readSample();
	}
}



static float calculateRate(void) {
    const float dt = 0.1f;

    float sum_t = 0.0f;
    float sum_w = 0.0f;
    float sum_tw = 0.0f;
    float sum_tt = 0.0f;

    for(uint8_t i = 0; i < BUF_SIZE; i++) {
        float t = i * dt;
        uint8_t pos = (buf_pos + i) % BUF_SIZE;

        sum_t += t;
        sum_w += reading_buf[pos];
        sum_tw += t * reading_buf[pos];
        sum_tt += t * t;
    }

    float N = (float) BUF_SIZE;

    return (N * sum_tw - sum_t * sum_w) / (N * sum_tt - sum_t * sum_t);
}

static void readSample(void) {
	uint32_t data = 0;
//	uint32_t startTime = HAL_GetTick();
//
//	while (HAL_GPIO_ReadPin(hx711.dout_gpio, hx711.dout_pin) == GPIO_PIN_SET) {
//		HAL_Delay(1);
//		if(HAL_GetTick() - startTime > 150)
//		  return;
//	}

	stopInterrupts();

	for (uint8_t i=0; i<24 ; i++) {
		HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
		DelayUs(1);
		HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
		DelayUs(1);

		data = data << 1;

		if(HAL_GPIO_ReadPin(hx711.dout_gpio, hx711.dout_pin) == GPIO_PIN_SET)
		  data ++;
	}

	resumeInterrupts();

	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	DelayUs(1);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	DelayUs(1);

	buf_pos = (buf_pos + 1) % BUF_SIZE;

	float last = reading_buf[buf_pos];
	reading_buf[buf_pos] = data ^ 0x800000;
	reading_avrg += ((reading_buf[buf_pos] - last) / BUF_SIZE);
	reading_rate = calculateRate();
}
