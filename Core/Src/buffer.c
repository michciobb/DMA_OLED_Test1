/*
 * buffer.c
 *
 *  Created on: Sep 2, 2022
 *      Author: michciopc
 */
#include <buffer.h>



uint8_t hd_buf[ROWS*COLS];		// główny bufor LCD

uint8_t * active_buf = hd_buf;	// wskaźnik aktywnego bufora




// wyczyszczenie aktywnego bufora
void buf_cls(void) {
	memset(active_buf, 0x00, ROWS*COLS);
}

