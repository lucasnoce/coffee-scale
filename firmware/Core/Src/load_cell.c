/*
 * load_cell.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include <math.h>
#include "load_cell.h"



#define BUF_SIZE 10



static float reading_buf[BUF_SIZE];
static float reading_last = 0.0;
static float reading_rate = 0.0;
static uint8_t buf_pos = 0;

static float test_read = 0.0;


void LoadCellInit(void) {
	for (uint8_t i=0; i<BUF_SIZE-1; i++) {
		reading_buf[i] = 0.0;
	}
	return;
}

void LoadCellRead(void) {
	test_read += 1.0;
	buf_pos = (buf_pos + 1) % BUF_SIZE;
	reading_buf[buf_pos] = (float) (10000.0/2) * (sin(test_read) + 1);
	reading_last = reading_buf[buf_pos];
	reading_rate = reading_buf[buf_pos] - reading_buf[(buf_pos + 1) % BUF_SIZE];
	return;
}

float LoadCellGetLastReading(void) {
	return reading_last;
}

float LoadCellGetReadingRate(void) {
	return reading_rate;
}
