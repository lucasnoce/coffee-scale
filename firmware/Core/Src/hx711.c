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

#ifdef HX711_LOOP_BACK_TEST
#define samplingDelay(x) HAL_Delay(x)
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

static float reading_buf[BUF_SIZE] = {0};
static float reading_avrg = 0.0;
static float reading_rate = 0.0;
static bool reading_buf_full = false;
static uint8_t buf_pos = BUF_SIZE - 1;

bool sample_ready = false;

#ifdef HX711_LOOP_BACK_TEST
const static uint32_t dout_seq[] = {
	//111001000000000000000000000000
	0b000000000000000000000000,	0b000001001111011101000110,	0b000001110000010111011010,	0b000001111111101111101111,	0b000010001001100111101010,
	0b000010010000110100001100,	0b000010010110011100001000,	0b000010011011000010010001,	0b000010011110111010001101,	0b000010100010001111111110,
	0b000010100101001011100010,	0b000010100111110010011001,	0b000010101010001000011111,	0b000010101100010000110000,	0b000010101110001101011011,
	0b000010110000000000010001,	0b000010110001101010101000,	0b000010110011001101101001,	0b000010110100101010001111,	0b000010110110000001001000,
	0b000010110111010010111110,	0b000010111000100000010011,	0b000010111001101001100011,	0b000010111010101111001001,	0b000010111011110001011000,
	0b000010111100110000100101,	0b000010111101101100111111,	0b000010111110100110110101,	0b000010111111011110010011,	0b000011000000010011100111,
	0b000011000001000110111000,	0b000011000001111000010001,	0b000011000010100111111010,	0b000011000011010101111001,	0b000011000100000010010111,
	0b000011000100101101011000,	0b000011000101010111000011,	0b000011000101111111011101,	0b000011000110100110101001,	0b000011000111001100101101,
	0b000011000111110001101100,	0b000011001000010101101011,	0b000011001000111000101011,	0b000011001001011010110001,	0b000011001001111011111111,
	0b000011001010011100011001,	0b000011001010111011111111,	0b000011001011011010110110,	0b000011001011111000111110,	0b000011001100010110011010,
	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100,
	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100,	0b000011001100110011001100
};
const static uint8_t dout_seq_size = (sizeof(dout_seq) / sizeof(dout_seq[0]));
static uint8_t dout_seq_pos = 0;
static uint8_t dout_seq_bit = 24;
static bool lb_test_running = false;
#endif



static float calculateAvrg(float last);
static float calculateRate(void);
static void readSample(void);



void HX711Init(GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dout_gpio, uint16_t dout_pin) {
	if (hx711_init == true || sck_gpio == NULL || dout_gpio == NULL) {
		return;
	}

	hx711.sck_gpio  = sck_gpio;
	hx711.sck_pin   = sck_pin;
	hx711.dout_gpio = dout_gpio;
	hx711.dout_pin  = dout_pin;
	hx711.slope     = 0.0001192093038;
	hx711.offset    = 0.0;

#ifdef HX711_LOOP_BACK_TEST
	stopInterrupts();
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_SET);
	resumeInterrupts();
	HX711LoopBackTestStart();
#else
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
#endif

	uint32_t startTime = HAL_GetTick();
	while (sample_ready == false) {
		HAL_Delay(1);
		if (HAL_GetTick() - startTime > 150) return;
	}

	sample_ready = false;
	readSample();

	HX711Tare();
	reading_buf[buf_pos] = 0.0;
	reading_avrg = 0.0;
	reading_rate = 0.0;

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

void HX711SetSampleReady(void) {
	#ifdef HX711_LOOP_BACK_TEST
	if (lb_test_running == false)
	#endif
	sample_ready = true;
}

void HX711Tare(void) {
	hx711.offset = -reading_avrg;
}

float HX711GetReadingAverage(void) {
	return reading_avrg;
}

float HX711GetReadingRate(void) {
	return reading_rate;
}

#ifdef HX711_LOOP_BACK_TEST
void HX711LoopBackTestStart(void) {
	// DOUT low informs that a new sample is ready
	if (lb_test_running == false) {
		dout_seq_bit = 24;
		HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_RESET);
	}
}

static void HX711LoopBackTestStop(void) {
	// DOUT high informs that HX711 is busy
	HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_SET);
	lb_test_running = false;
}

void HX711LoopBackTestSendData(void) {
	lb_test_running = true;

	if (dout_seq_bit > 0) dout_seq_bit--;
	else {
		HX711LoopBackTestStop();
		return;
	}

	if (((dout_seq[dout_seq_pos] >> dout_seq_bit) & 0x01) == 0x01)
		HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(HX711_DOUT_LoopBack_GPIO_Port, HX711_DOUT_LoopBack_Pin, GPIO_PIN_RESET);

	if (dout_seq_bit == 0) {
		dout_seq_pos = (dout_seq_pos + 1) % dout_seq_size;
		HX711LoopBackTestStop();
	}
}
#endif

void HX711Loop(void) {
	if (hx711_init == false) return;

	if (sample_ready == true) {
		sample_ready = false;
		readSample();
	}
}



static float calculateAvrg(float last) {
	float avrg = 0.0;

	if (reading_buf_full == true) {
		avrg = (reading_avrg + ((reading_buf[buf_pos] - last) / BUF_SIZE));
	}
	else {
		for(uint8_t i = 0; i <= buf_pos; i++) avrg += reading_buf[i];
		avrg = (avrg / (buf_pos + 1));
	}

	return avrg;
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
		samplingDelay(1);
		HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
		samplingDelay(1);

		data = data << 1;

		if (HAL_GPIO_ReadPin(hx711.dout_gpio, hx711.dout_pin) == GPIO_PIN_SET)
		  data |= 0x01;
	}

	resumeInterrupts();

#ifndef HX711_LOOP_BACK_TEST
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_SET);
	DelayUs(1);
	HAL_GPIO_WritePin(hx711.sck_gpio, hx711.sck_pin, GPIO_PIN_RESET);
	DelayUs(1);
#endif

	buf_pos = (buf_pos + 1) % BUF_SIZE;
	if (buf_pos >= (BUF_SIZE - 1)) reading_buf_full = true;

	float last = reading_buf[buf_pos];
	reading_buf[buf_pos] = (((float)((int32_t)(data ^ 0x800000))) * hx711.slope) + hx711.offset;
	reading_avrg = calculateAvrg(last);
	reading_rate = calculateRate();
}
