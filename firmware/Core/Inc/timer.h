/*
 * timer.h
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>

void TimerInit(void);
void TimerIncrement(void);
void TimerStart(void);
void TimerPause(void);
void TimerResume(void);
void TimerStop(void);
void TimerCountdown(void);
uint32_t TimerGetSeconds(void);

#endif /* INC_TIMER_H_ */
