/*
 * events.h
 *
 *  Created on: Jun 11, 2026
 *      Author: lucas
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_

typedef enum {
    EVENT_ID_NONE = 0,
    EVENT_ID_DISPLAY_UPDATE,
    EVENT_ID_HX711_SAMPLE_READY,
    EVENT_ID_BUTTON_ACTION,
    EVENT_ID_BUTTON_HOLD,
    EVENT_ID_TIMEOUT
} EventId_t;

void EventInit(void);
void EventLoop(void);
void EventQueue(EventId_t evt_id, uintptr_t evt_data);

#endif /* INC_EVENTS_H_ */
