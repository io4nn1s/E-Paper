/****************************************************************
 * File name:   operations.cpp                                  *
 * Author:      IA                                              *
 * Date:        07/09/2020                                      *
 * Description: Used for E-Paper operations.                    *
 *                                                              *
 * Copyright (C) 2020-2022  Midas Displays                      *
 *                                                              *
 ****************************************************************/
#include "io.h"
#include "graphics.h"

void Update(void)
{   
//   Write_Command(0x22); 
//   Write_Data(0xF7);   
   Write_Command(0x20); 
   READBUSY();   
}
void Epaper_Sleep(void)
{     
   Write_Command(0x10);  //enter deep sleep
   Write_Data(0x01); 
   delay(100);
}
void Part_Update(void)
{
   Write_Command(0x22); 
   Write_Data(0x0F);   
   Write_Command(0x20); 
   READBUSY();         
}
void Part_off(void)
{  
   Write_Command(0x22); 
   Write_Data(0x03);    //disable analog and clock signal
   Write_Command(0x20); 

   Write_Command(0x10); //enter deep sleep
   Write_Data(0x01); 
}

// ******** Display Partial Graphics ****************************************************************
void Display_Part(unsigned int x_start, unsigned int y_start, unsigned char *image, unsigned int SIZE_X, unsigned int SIZE_Y, unsigned char SET_RAM)
{
   unsigned int i;  
   unsigned int x_end, y_start_H, y_start_L, y_end_H, y_end_L;
   unsigned char buff;
   x_start = (x_start/8) + 1;
   x_end = x_start + (SIZE_X/8) - 1;
   
   y_start_H=0;
   y_start_L=y_start;
   if(y_start>=256)
   {
      y_start_H=y_start_L/256;
      y_start_L=y_start_L%256;
   }
   y_end_H=0;
   y_end_L=y_start+SIZE_Y-1;
   if(y_end_L>=256)
   {
      y_end_H=y_end_L/256;
      y_end_L=y_end_L%256;      
   }     
   
   Write_Command(0x44);       // set RAM x address start/end
   Write_Data(x_start);       // RAM x address start 
   Write_Data(x_end);         // RAM x address end  
   Write_Command(0x45);       // set RAM y address start/end
   Write_Data(y_start_L);     // RAM y address start (lower 8 bits)
   Write_Data(y_start_H);     // RAM y address start (upper 8 bits)
   Write_Data(y_end_L);       // RAM y address end (lower 8 bits)
   Write_Data(y_end_H);       // RAM y address end (upper 8 bits)

   Write_Command(0x4E);       // set RAM x address count
   Write_Data(x_start); 
   Write_Command(0x4F);       // set RAM y address count    
   Write_Data(y_start_L);
   Write_Data(y_start_H);

   if(SET_RAM==0x2)
      Write_Command(0x26);   //Write Black and White image to RAM 2
   else
      Write_Command(0x24);   //Write Black and White image to RAM 1
   for(i=0;i<SIZE_Y*SIZE_X/8;i++)
   {        
      buff=pgm_read_byte_near(image);                   
      Write_Data(~buff);
      image++;
   } 
}
void Display_Static_Part(unsigned int X, unsigned int Y, unsigned char *graphic, unsigned int SIZE_X, unsigned int SIZE_Y)
{
   Display_Part(X, Y, graphic, SIZE_X, SIZE_Y, RAM1);
   Display_Part(X, Y, graphic, SIZE_X, SIZE_Y, RAM2);
}
// ******** Clear Partial Screen ****************************************************************
void Part_Clear(unsigned int x_start, unsigned int y_start, unsigned char colour, unsigned int SIZE_X, unsigned int SIZE_Y, unsigned char SET_RAM)
{
   unsigned int i;
   unsigned int x_end, y_start_H, y_start_L, y_end_H, y_end_L;
   x_start = (x_start/8) + 1;
   x_end = x_start + (SIZE_X/8) - 1;

   y_start_H = 0;
   y_start_L = y_start;
   if (y_start >= 256)
   {
     y_start_H = y_start_L / 256;
     y_start_L = y_start_L % 256;
   }
   y_end_H = 0;
   y_end_L = y_start + SIZE_Y - 1;
   if (y_end_L >= 256)
   {
     y_end_H = y_end_L / 256;
     y_end_L = y_end_L % 256;
   }

   Write_Command(0x44);       // set RAM x address start/end
   Write_Data(x_start);       // RAM x address start 
   Write_Data(x_end);         // RAM x address end  
   Write_Command(0x45);       // set RAM y address start/end
   Write_Data(y_start_L);     // RAM y address start (lower 8 bits)
   Write_Data(y_start_H);     // RAM y address start (upper 8 bits)
   Write_Data(y_end_L);       // RAM y address end (lower 8 bits)
   Write_Data(y_end_H);       // RAM y address end (upper 8 bits)

   Write_Command(0x4E);       // set RAM x address count
   Write_Data(x_start); 
   Write_Command(0x4F);       // set RAM y address count    
   Write_Data(y_start_L);
   Write_Data(y_start_H);

   if(SET_RAM==0x2)
       Write_Command(0x26);   //Clear part of RAM 2
    else
       Write_Command(0x24);   //Clear part of RAM 1
   for (i = 0; i < SIZE_Y * SIZE_X / 8; i++)
   {
      Write_Data(colour);
   }
   //Part_Update();
}
