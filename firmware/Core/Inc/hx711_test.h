/*
 * hx711_test.h
 *
 *  Created on: Jun 15, 2026
 *      Author: lucas
 */

#ifndef INC_HX711_TEST_H_
#define INC_HX711_TEST_H_

#include "main.h"
#include "bool.h"

#if defined(HX711_SCK_LoopBack_Pin) && defined(HX711_DOUT_LoopBack_Pin)
#define HX711_LOOP_BACK_TEST
#endif



#ifdef HX711_LOOP_BACK_TEST
bool HX711LoopBackIsTestRunning(void);
void HX711LoopBackTestStart(void);
void HX711LoopBackTestSendData(void);
#endif



#endif /* INC_HX711_TEST_H_ */
