/*
 * processes.c
 *
 *  Created on: Sep 2, 2022
 *      Author: michciopc
 */

#include <buffer.h>
#include "processes.h"
#include "SSD1306.h"

void combine_buffers();
// zmienne globalne - warstwy stworzone na stałe w programie
T_PROCESS current_proc, voltage_proc, battery_proc;

static T_PROCESS *process[MAX_PROCESSES]; //tablica wskaźników na procesy typu T_PROCESS
static uint8_t x_move = 0;
static uint8_t tempo = 0;

volatile uint8_t ms10_cnt;	// zmienna na potrzeby timerów programowych
volatile uint8_t s1_cnt;

static const unsigned char battery_pattern1[] = { 0, 0, 0, 0, 0xFE, 0xFF, 0xFF,
		0xFF, 0xF8, 0xFC, 0xFC, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x3F, 0x3F,
		0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0x7F, 0xFF, 0xFF, 0xFF };

//******************** WARSTWA 1 ********************************
void current_event(void) {

	uint16_t col = 0;
	uint8_t step = 0;
	uint8_t color = 1;
	uint8_t dir = 1;  //kierunek animacji 0-1

	active_buf = current_proc.buf;

	if (++tempo > 5) {

		buf_cls();

		while ((col + x_move) < 512) {
			if ((x_move > 7) && (col < (x_move - 8))) {	//dorysowanie pierwszych ośmiu bajtów
				if (dir) {
					if (color)
						current_proc.buf[(511 - col)] = 0xFF;
				} else {
					if (color)
						current_proc.buf[(col)] = 0xFF;
				}
			}

			if (dir) {								//wypełnienie reszty bajtów
				if (color)
					current_proc.buf[511 - (col + x_move)] = 0xFF;
			} else {
				if (color)
					current_proc.buf[(col + x_move)] = 0xFF;
			}

			++col;

			if (++step == 8) {
				step = 0;
				color = (color) ? 0 : 1; //zamiana color 0 na 1 i na odwrót
			}
		}
		x_move++;
		if (x_move == 16)
			x_move = 0;
		tempo = 0;
	}
}

//******************** WARSTWA 2 ********************************

void voltage_event(void) {
	active_buf = voltage_proc.buf;

	uint8_t v_percent = 30; //procent napięcia baterii
	uint16_t level = (113 * v_percent) / 100;
	uint8_t x = 121 - level; //współrzędna poziomu naładowania baterii

	for (uint16_t i = 0; i < 128; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			voltage_proc.buf[i + j * 128] = (i < x) ? 0x00 : 0xFF;
			if (i > (x + 2))
				voltage_proc.buf[i + j * 128] = 0xAA;
		}
	}
}

//******************** WARSTWA 3 ********************************
void battery_event(void) {
	active_buf = battery_proc.buf;


}





void PROCESSES_EVENT(uint8_t all) {
	uint8_t idx = 0;
	static uint8_t lms10 = 255;

	if (lms10 != ms10_cnt) { // co 10 ms

		if (all) { 				// jeśli aktywna obsługa warstw oraz ich zdarzeń
			while (idx < MAX_PROCESSES) {
				if (process[idx]) {	//jeżeli występuje taki proces w tablicy procesów
					if (process[idx]->EVENT)
						process[idx]->EVENT();//jeżeli jest przydzielone jakieś zdarzenie w danym procesie
											  //to ma się wykonać
					//if (process[idx]->visible)
					//	GLCD_CopyAllBuffer(process[idx]->buf); //jeśli warstwa ma być widoczna to ją pokaż

				}
				idx++; 	//zwiększ indeks procesu
			}
			combine_buffers();
			GLCD_CopyAllBuffer(hd_buf);
			GLCD_Render();
		}

		//wyświetl na ekranie LCD zawartość bufora RAM
		if (!(ms10_cnt % (LCD_REFRESH_MS / 10))) {
			//GLCD_Render();
			//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}
		lms10 = ms10_cnt;
	}
}

int8_t create_obj(T_PROCESS *pr, uint8_t visible, void (*event)(void)) {
	uint8_t i;
	for (i = 0; i < MAX_PROCESSES; i++) {
		if (!process[i]) {

			pr->EVENT = event;
			pr->visible = visible;
			process[i] = pr;
			active_buf = pr->buf;
			buf_cls();
			break;
		}
	}
	if (MAX_PROCESSES == i)
		i = -1;
	return i;
}

void free_obj(uint8_t handle) {
	process[handle] = 0;
}

void combine_buffers() {
	for (uint16_t i = 0; i < 512; i++) {
		hd_buf[i] = current_proc.buf[i];
		hd_buf[i] =
				(voltage_proc.buf[i] == 0xAA) ?
						current_proc.buf[i] : voltage_proc.buf[i];
	}

	//Rysujemy baterię

	//styk dodatni
	for (uint16_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			hd_buf[i + j * 128] = battery_pattern1[i + j * 8];
		}
	}

	//styk ujemny
	for (uint8_t i = 124; i < 128; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			hd_buf[i + j * 128] = 0xFF;
		}
	}

	//jeden bok
	for (uint8_t i = 8; i < 124; i++) {

		hd_buf[i] = hd_buf[i] | 0x0F;
	}

	//drugi bok
	for (uint16_t i = 8+3*128; i < 124+3*128; i++) {

		hd_buf[i] = hd_buf[i] | 0xF0;
	}
}
