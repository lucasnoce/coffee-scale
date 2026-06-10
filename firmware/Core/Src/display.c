/*
 * display.c
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#include <stdio.h>
#include <stdint.h>
#include "bool.h"
#include "display.h"
#include "load_cell.h"
#include "timer.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"



const uint8_t text_x_padding = 2;
const uint8_t text_y_padding = 2;
const uint8_t text_first_row_h = 8;
const uint8_t text_row_title_h = 8;
const uint8_t text_row_value_h = 18;

uint8_t text_row[] = {
	0,
	text_first_row_h + text_row_title_h + text_y_padding,
	text_first_row_h + text_row_value_h + 2*text_row_title_h + 2*text_y_padding
};

uint8_t text_col[] = {
	0,
	(SSD1306_WIDTH - 1)/2 + text_x_padding
};

bool show_mass  = true;
bool show_flow  = true;
bool show_timer = true;



static void resetDisplay(void);
static void updateDisplayMass(void);
static void updateDisplayFlow(void);
static void updateDisplayTimer(void);
//static void updateDisplayGrind(void);



void DisplayInit(void) {
	ssd1306_Init();
}

void DisplayUpdate(void) {
	resetDisplay();
	updateDisplayMass();
	updateDisplayFlow();
	updateDisplayTimer();
	ssd1306_UpdateScreen();
}



static void resetDisplay(void) {
	ssd1306_Fill(Black);
	ssd1306_Line((text_col[1] - text_x_padding), 1, (text_col[1] - text_x_padding), (SSD1306_HEIGHT - 1), White);  // vertical
//	ssd1306_Line(1, (text_row[1] - text_row_title_h - text_y_padding), (SSD1306_WIDTH - 1), (text_row[1] - text_row_title_h - text_y_padding), White);  // horizontal 1
	ssd1306_Line(1, (text_row[2] - text_row_title_h - text_y_padding), (SSD1306_WIDTH - 1), (text_row[2] - text_row_title_h - text_y_padding), White);  // horizontal 2

	ssd1306_SetCursor(text_col[0], text_row[0]);
	ssd1306_WriteString("Coffee Scale      [g]", Font_6x8, White);

	ssd1306_SetCursor(text_col[0], (text_row[1] - text_row_title_h));
	ssd1306_WriteString("Grind", Font_6x8, White);
	ssd1306_SetCursor(text_col[0], (text_row[2] - text_row_title_h));
	ssd1306_WriteString("Timer", Font_6x8, White);
	ssd1306_SetCursor(text_col[1], (text_row[1] - text_row_title_h));
	ssd1306_WriteString("Flow", Font_6x8, White);
	ssd1306_SetCursor(text_col[1], (text_row[2] - text_row_title_h));
	ssd1306_WriteString("Mass", Font_6x8, White);
}

static void updateDisplayMass(void) {
	char text[16] = "";

	ssd1306_SetCursor(text_col[1], text_row[2]);

	if (show_mass == true) {
		float mass = LoadCellGetLastReading();

		if (mass < 1000.0 && mass > -100.0) {
			snprintf(text, sizeof(text), "%.1f", mass);
			ssd1306_WriteString(text, Font_11x18, White);
		}
		else if ((mass < -100.0 && mass > -1000.0) || (mass < 10000.0 && mass > 1000.0)) {
			snprintf(text, sizeof(text), "%.1f", mass);
			ssd1306_SetCursor(text_col[1], text_row[2]+4);
			ssd1306_WriteString(text, Font_7x10, White);
		}
		else {
			snprintf(text, sizeof(text), "%.0f", mass);
			ssd1306_SetCursor(text_col[1], text_row[2]+4);
			ssd1306_WriteString(text, Font_7x10, White);
		}
	}
	else {
		snprintf(text, sizeof(text), "--");
		ssd1306_WriteString(text, Font_11x18, White);
	}
}

static void updateDisplayFlow(void) {
	char text[16] = "";

	ssd1306_SetCursor(text_col[1], text_row[1]);

	if (show_flow == true) {
		float flow = LoadCellGetReadingRate();

		if (flow < 1000.0 && flow > -100.0) {
			snprintf(text, sizeof(text), "%.1f", flow);
			ssd1306_WriteString(text, Font_11x18, White);
		}
		else if ((flow < -100.0 && flow > -1000.0) || (flow < 10000.0 && flow > 1000.0)) {
			snprintf(text, sizeof(text), "%.1f", flow);
			ssd1306_SetCursor(text_col[1], text_row[1]+4);
			ssd1306_WriteString(text, Font_7x10, White);
		}
		else {
			snprintf(text, sizeof(text), "%.0f", flow);
			ssd1306_SetCursor(text_col[1], text_row[1]+4);
			ssd1306_WriteString(text, Font_7x10, White);
		}
	}
	else {
		snprintf(text, sizeof(text), "--");
		ssd1306_WriteString(text, Font_11x18, White);
	}
}

static void updateDisplayTimer(void) {
	char text[16] = "";

	ssd1306_SetCursor(text_col[0], text_row[2]);

	if (show_timer == true) {
		uint32_t seconds = TimerGetSeconds();
		if (seconds < 60){
			snprintf(text, sizeof(text), "%u", (unsigned int) seconds);
			ssd1306_WriteString(text, Font_11x18, White);
		}
		else if (seconds < 3600){
			snprintf(text, sizeof(text), "%u:%02u", (unsigned int)(seconds/60), (unsigned int)(seconds%60));
			ssd1306_WriteString(text, Font_11x18, White);
		}
		else {
			snprintf(text, sizeof(text), "%u:%02u:%02u", (unsigned int)(seconds/3600), (unsigned int)((seconds%3600)/60), (unsigned int)(seconds%60));
			ssd1306_SetCursor(text_col[0], text_row[2]+4);
			ssd1306_WriteString(text, Font_7x10, White);
		}
	}
	else {
		snprintf(text, sizeof(text), "--");
		ssd1306_WriteString(text, Font_11x18, White);
	}
}
