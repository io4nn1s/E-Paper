/****************************************************************
 * File name:   io.h                                            *
 * Author:      IA                                              *
 * Date:        07/09/2020                                      *
 * Description: Contains all port declerations, some macros and *
 *              macros.                                         *
 *                                                              *
 * Copyright (C) 2020-2022  Midas Displays                      *
 *                                                              *
 ****************************************************************/
#ifndef __IO_H
#define __IO_H

#include <Arduino.h>

//PINS:  MDIB-EP1    Trinket board
#define  BUSY        3
#define  RESET       9
#define  DC          8
#define  nCS         6
#define  SCL         5
#define  SDA         4

//E-Paper rows and collumns
#define Disp_width       122
#define Disp_height      250
#define FULLSCREEN_BYTES 4000

#define  X_Addr_Start    0x01
#define  X_Addr_End      0x10 
#define  Y_Addr_Start_H  0x00  
#define  Y_Addr_Start_L  0xF9
#define  Y_Addr_End_H    0x00
#define  Y_Addr_End_L    0x00

#define BLACK            0x00
#define WHITE            0xFF
#define RAM1             0x1
#define RAM2             0x2

//Function declarations
void Reset(void);
void READBUSY(void);
void Write_Command(unsigned char cmd);
void Write_Data(unsigned char data);
uint8_t * Read_Data(uint8_t command, uint8_t *DataBuff, uint8_t nbytes);

void init_SSD1680(void);
void Part_Init_SSD1680(void);
void Update(void);
void Epaper_Sleep(void);
void Part_Update(void);
void Part_off(void);

void Display_Part(unsigned int x_start, unsigned int y_start, unsigned char *image, unsigned int SIZE_X, unsigned int SIZE_Y, unsigned char SET_RAM);
void Display_Static_Part(unsigned int X, unsigned int Y, unsigned char *graphic, unsigned int SIZE_X, unsigned int SIZE_Y);
void Part_Clear(unsigned int x_start, unsigned int y_start, unsigned char colour, unsigned int SIZE_X, unsigned int SIZE_Y, unsigned char SET_RAM);
void digital_clock(uint16_t Y, char init_time[4], uint16_t time_frame);

#endif
