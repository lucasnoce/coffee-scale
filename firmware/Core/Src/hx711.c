/*
 * hx711.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "hx711.h"
#include "hx711_test.h"
#include "bool.h"
#include "utils.h"



#define BUF_RAW_SIZE 3
#define BUF_FLT_SIZE 10



#ifdef HX711_LOOP_BACK_TEST_EN
#define samplingDelay(x) DelayUs(10)
#else
#define samplingDelay(x) DelayUs(x)
#endif



typedef struct {
	GPIO_TypeDef *sck_gpio;
	GPIO_TypeDef *dout_gpio;
	float        slope;
	float        offset;
	uint16_t     sck_pin;
	uint16_t     dout_pin;
} HX711_t;



static HX711_t hx711;
static bool hx711_init = false;

static int32_t buf_raw[BUF_RAW_SIZE] = {0};  // stores raw values, used to calculate median
static float buf_flt[BUF_FLT_SIZE] = {0};    // stores converted median values (from buf_raw)
static float reading_avrg = 0.0;
static float reading_rate = 0.0;
static bool buf_flt_full = false;
static uint8_t buf_flt_pos = BUF_FLT_SIZE - 1;
static uint8_t buf_raw_pos = BUF_RAW_SIZE - 1;

/* Linear Regression constant values (possible thanks to fixed sampling rate) */
const static float lr_N = (float) BUF_FLT_SIZE;
const static float lr_t[BUF_FLT_SIZE] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
const static float lr_sum_t  = 4.5;
const static float lr_sum_t2 = 2.85;
const static float lr_den = (lr_N * lr_sum_t2) - (lr_sum_t * lr_sum_t);


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

static void stopInterrupts(void);
static void resumeInterrupts(void);
static int32_t calculateMedian3(void);
static float calculateAvrgForced(void);
static float calculateAvrgFast(float last);
static float calculateRate(void);
static void readSample(void);
static void readSamplePoll(void);



void HX711Init(GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dout_gpio, uint16_t dout_pin) {
	if (hx711_init == true || sck_gpio == NULL || dout_gpio == NULL) {
		return;
	}

	hx711.sck_gpio  = sck_gpio;
	hx711.sck_pin   = sck_pin;
	hx711.dout_gpio = dout_gpio;
	hx711.dout_pin  = dout_pin;
	hx711.slope     = 0.0001192093038 / 1.1;
	hx711.offset    = 0.0;

#ifdef HX711_LOOP_BACK_TEST_EN
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_SET);
#else
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
#endif

	for (uint8_t i = 0; i < (BUF_FLT_SIZE + 1); i++) { // need +1 because of median overhead
		#ifdef HX711_LOOP_BACK_TEST_EN
		HX711LoopBackTestStart();
		#endif
		readSamplePoll();
	}

	while ()

	reading_avrg = calculateAvrgForced();
	reading_rate = calculateRate();
	HX711Tare();

	hx711_init = true;
}

float HX711GetSlope(void) {
	return hx711_init == false ? 0.0 : hx711.slope;
}

float HX711GetOffset(void) {
	return hx711_init == false ? 0.0 : hx711.offset;
}

void HX711SetSlope(float slope) {
	if (hx711_init == false) return;
	hx711.slope = slope;
}

void HX711SetOffset(float offset) {
	if (hx711_init == false) return;
	hx711.offset = offset;
}

void HX711Tare(void) {
	hx711.offset = -reading_avrg;

	for (uint8_t i = 0; i < BUF_FLT_SIZE; i++)
		buf_flt[i] -= reading_avrg;

	reading_avrg = 0;
	reading_rate = calculateRate();
}

float HX711GetReadingAverage(void) {
	return reading_avrg;
}

float HX711GetReadingRate(void) {
	return reading_rate;
}

void HX711Loop(void) {
	if (hx711_init == false)
		return;
	readSample();
}


static void stopInterrupts(void) {
//	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim4);

	// stop buttons
	EXTI->IMR &= ~(EXTI_IMR_IM3 |
	               EXTI_IMR_IM4 |
	               EXTI_IMR_IM5 |
				   EXTI_IMR_IM6 |
				   EXTI_IMR_IM11);
}

static void resumeInterrupts(void) {
//	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);

	EXTI->PR = GPIO_PIN_3 |
			   GPIO_PIN_4 |
			   GPIO_PIN_5 |
			   GPIO_PIN_6 |
			   GPIO_PIN_11;

	EXTI->IMR |= (EXTI_IMR_IM3 |
				  EXTI_IMR_IM4 |
				  EXTI_IMR_IM5 |
				  EXTI_IMR_IM6 |
				  EXTI_IMR_IM11);
}

static int32_t calculateMedian3(void) {
	if (buf_raw[0] > buf_raw[1]) {
		int32_t t = buf_raw[0];
		buf_raw[0] = buf_raw[1];
		buf_raw[1] = t;
	}

	if (buf_raw[1] > buf_raw[2]) {
		int32_t t = buf_raw[1];
		buf_raw[1] = buf_raw[2];
		buf_raw[2] = t;
	}

	if (buf_raw[0] > buf_raw[1]) {
		buf_raw[1] = buf_raw[0];
	}

	return buf_raw[1];
}

static float calculateAvrgForced(void) {
	float avrg = 0.0;
	for (uint8_t i = 0; i < BUF_FLT_SIZE; i++)
		avrg += buf_flt[i];
	return (avrg / BUF_FLT_SIZE);
}

static float calculateAvrgFast(float last) {
//	float avrg = 0.0;

//	avrg = (reading_avrg + ((buf_flt[buf_flt_pos] - last) / BUF_FLT_SIZE));
//	if (buf_flt_full == true) {
//		avrg = (reading_avrg + ((buf_flt[buf_flt_pos] - last) / BUF_FLT_SIZE));
//	}
//	else {
//		for(uint8_t i = 0; i <= buf_flt_pos; i++) avrg += buf_flt[i];
//		avrg = (avrg / (buf_flt_pos + 1));
//	}

	return (reading_avrg + ((buf_flt[buf_flt_pos] - last) / BUF_FLT_SIZE));
}

static float calculateRate(void) {
    float _lr_sum_w = (float) BUF_FLT_SIZE * reading_avrg;
    float _lr_sum_tw = 0.0;

    for (uint8_t i = 0; i < BUF_FLT_SIZE; i++) {
        uint8_t pos = (buf_flt_pos + i) % BUF_FLT_SIZE;
        _lr_sum_tw += (lr_t[i] * buf_flt[pos]);
    }

    return (lr_N * _lr_sum_tw - lr_sum_t * _lr_sum_w) / lr_den;
}

static void readSample(void) {
	uint32_t data = 0;

	stopInterrupts();

	for (uint8_t i=0; i<24 ; i++) {
		HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
		samplingDelay(1);
		HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
		samplingDelay(1);

		data = data << 1;

		if (HAL_GPIO_ReadPin(hx711.dout_gpio, hx711.dout_pin) == GPIO_PIN_SET)
		  data |= 0x01;
	}

	resumeInterrupts();

	#ifndef HX711_LOOP_BACK_TEST_EN
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	DelayUs(1);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	DelayUs(1);
	#endif

	buf_raw_pos = (buf_raw_pos + 1) % BUF_RAW_SIZE;
	buf_flt_pos = (buf_flt_pos + 1) % BUF_FLT_SIZE;
	if (buf_flt_pos >= (BUF_FLT_SIZE - 1)) buf_flt_full = true;

	buf_raw[buf_raw_pos] = (data ^ 0x800000);

	int32_t median3 = calculateMedian3();
	float last = buf_flt[buf_flt_pos];

	buf_flt[buf_flt_pos] = (((float) median3) * hx711.slope) + hx711.offset;
	reading_avrg = calculateAvrgFast(last);
	reading_rate = calculateRate();
}

static void readSamplePoll(void) {
	uint32_t maxDelay = 200;
	uint32_t startTime = HAL_GetTick();

	while (HAL_GPIO_ReadPin(hx711.dout_gpio, hx711.dout_pin) == GPIO_PIN_SET) {
		HAL_Delay(1);
		if (HAL_GetTick() - startTime > maxDelay)
			return;
	}

	readSample();
}
