/*
 * buffer.h
 *
 *  Created on: Sep 2, 2022
 *      Author: michciopc
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#define ROWS 4
#define COLS 128

#include <inttypes.h>
#include <stdint.h>

#include "processes.h"
#include "SSD1306.h"

extern uint8_t hd_buf[ROWS*COLS]; //główny bufor
extern uint8_t * active_buf; //wskaźnik aktywnego bufora

void buf_cls(void);
//void copy_buf(T_PROCESS * w);

#endif /* INC_BUFFER_H_ */
