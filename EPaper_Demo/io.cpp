/****************************************************************
 * File name:   io.cpp                                          *
 * Author:      IA                                              *
 * Date:        27/08/2020                                      *
 * Description: Used for main I/O operations.                   *
 *                                                              *
 * Copyright (C) 2020-2022  Midas Displays                      *
 *                                                              *
 ****************************************************************/

#include "io.h"

//Toggle reset and enable display.
void Reset(void)
{
   digitalWrite(RESET,HIGH);
   delay(10);
   digitalWrite(RESET,LOW);
   delay(40);
   digitalWrite(RESET,HIGH);
   delay(50);
}

//Read BUSY pin from E-Paper   
void READBUSY(void)
{
   //long Start_time, current_time;
   //Start_time = millis();        //read time elapsed until now

   while(1)
   {
      //current_time = millis();   //set current time
      if(digitalRead(BUSY)==LOW)
      {
         //Serial.print("Busy time = ");
         //Serial.println((float)(current_time - Start_time)/1000,3);
         break;
      }
      delayMicroseconds(1);
   }
   delayMicroseconds(10);
}

//################################################################
//   Send Command/Data Functions
//################################################################
void SPI4W_WriteByte(unsigned char serial_data)
{
   unsigned char TempData;
   unsigned char bitShift;
   TempData=serial_data;

   digitalWrite(SCL,LOW);
   for(bitShift=0;bitShift<8;bitShift++)
   {
      if(TempData&0x80)
         digitalWrite(SDA,HIGH);
      else
         digitalWrite(SDA,LOW);
      digitalWrite(SCL,HIGH);
      digitalWrite(SCL,LOW);      
      TempData<<=1;
   }
}
void Write_Command(unsigned char cmd)
{
   digitalWrite(nCS,LOW);   //select chip
   digitalWrite(DC,LOW);    //set command
   SPI4W_WriteByte(cmd);    //send
   digitalWrite(nCS,HIGH);  //unselect chip
}
void Write_Data(unsigned char data)
{
   digitalWrite(nCS,LOW);   //select chip
   digitalWrite(DC,HIGH);   //set data
   SPI4W_WriteByte(data);   //send
   digitalWrite(nCS,HIGH);  //unselect chip
}

//################################################################
//   Read Data Functions
//################################################################
uint8_t SPI4W_ReadByte(void)
{
   unsigned char TempData;
   unsigned char bitShift;

   digitalWrite(SCL,LOW);
   for(bitShift=0;bitShift<8;bitShift++)
   {
      TempData<<=1;                  // shift for next bit
      TempData |= digitalRead(SDA);  // read SDA pin (HIGH/LOW)
      digitalWrite(SCL,HIGH);
      digitalWrite(SCL,LOW);      
      
   }
   return TempData;
}

uint8_t * Read_Data(uint8_t command, uint8_t *DataBuff, uint8_t nbytes)
{
   uint8_t n;
   digitalWrite(nCS,LOW);    //select chip
   digitalWrite(DC,LOW);     //LOW=command, HIGH=data
   SPI4W_WriteByte(command); //send command
   Serial.println("------------------------------------");
   Serial.println("Read Data:");
   pinMode(SDA, INPUT);      //Prepare for Reading
   digitalWrite(DC,HIGH);    //LOW=command, HIGH=data
   for(n=0; n<nbytes; n++)
   {
      DataBuff[n] = SPI4W_ReadByte();
      Serial.print(n); Serial.print(") "); Serial.println(DataBuff[n],HEX);
   }
   digitalWrite(nCS,HIGH);   //unselect chip
   pinMode(SDA, OUTPUT);     //Set pin back to output
   Serial.println("------------------------------------");
   return DataBuff;
}
