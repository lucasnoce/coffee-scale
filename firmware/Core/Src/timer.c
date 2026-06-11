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
uint32_t timer_sec = 0;


void TimerInit(void) {
	TimerStart();
	return;
}

void TimerIncrement(void) {
	if (timer_on == true) {
		timer_sec++;
	}
}

void TimerStart(void) {
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

