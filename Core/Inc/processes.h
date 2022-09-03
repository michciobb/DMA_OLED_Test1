/*
 * processes.h
 *
 *  Created on: Sep 2, 2022
 *      Author: michciopc
 */

#ifndef INC_PROCESSES_H_
#define INC_PROCESSES_H_

#include <inttypes.h>
#include <stdint.h>
#include "gpio.h"

#include "buffer.h"

#define MAX_PROCESSES 3
#define LCD_REFRESH_MS	20

typedef struct {
	uint8_t visible;
	uint8_t buf[ROWS*COLS];
	void (*EVENT)(void);
}T_PROCESS;

extern T_PROCESS current_proc, voltage_proc, battery_proc;

extern volatile uint8_t ms10_cnt;
extern volatile uint8_t s1_cnt;

void PROCESSES_EVENT(uint8_t all);
void current_event(void);
void voltage_event(void);
void battery_event(void);
int8_t create_obj(T_PROCESS * pr,  uint8_t visible, void (*event)(void));

#endif /* INC_PROCESSES_H_ */
