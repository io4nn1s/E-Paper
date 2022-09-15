#ifndef __EPaper_H
#define __EPaper_H

#include "io.h"
#include "graphics.h"
#include "background.h"

#define Part_Init() Part_Init_SSD1680()

void Clear_Screen(unsigned char pixels);
void Display_Image(const unsigned char *point, uint8_t inv, uint8_t RAM);
void battery_indicator(int8_t lvl);
void digital_clock(uint16_t Y, char init_time[4], uint16_t time_frame);
#endif
