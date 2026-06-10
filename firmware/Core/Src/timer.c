/*
 * timer.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdint.h>
#include "bool.h"
#include "timer.h"



bool timer_on = false;
uint8_t timer_int = 0;
uint32_t timer_sec = 0;


void TimerInit(void) {
	TimerStart();
	return;
}

void TimerIncrement(void) {
	if (timer_on == true) {
		timer_int++;
		if (timer_int >= 10 ) {
			timer_int = 0;
			timer_sec+=60;
		}
	}
}

void TimerStart(void) {
	timer_int = 0;
	timer_sec = 0;
	timer_on  = true;
}

void TimerPause(void) {
	timer_on = false;
}

void TimerResume(void) {
	timer_on = true;
}

void TimerStop(void) { // TODO
	timer_on = false;
}

void TimerCountdown(void) { // TODO
	return;
}

uint32_t TimerGetSeconds(void) {
	return timer_sec;
}

