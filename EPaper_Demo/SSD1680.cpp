/****************************************************************
 * File name:   SSD1680.cpp                                     *
 * Author:      IA                                              *
 * Date:        02/09/2020                                      *
 * Description: Initialization routines for the SSD1680 IC      *
 *              controller used for AMEPD (E-Paper) displays.   *
 *                                                              *
 * Copyright (C) 2020-2022  Midas Displays                      *
 *                                                              *
 ****************************************************************/

#include "io.h"
#include "LUT.h"

void Load_LUT_from_MCU(unsigned char *waveform_data)
{
   uint8_t count;
   Write_Command(0x32);              //Load LUT
   for(count=0;count<153;count++) Write_Data(waveform_data[count]);
      READBUSY();

   Write_Command(0x3F);              //End Option for LUT
   Write_Data(*(waveform_data+153));
   //Write_Data(0x07);
   
   Write_Command(0x03);              //Gate Voltage (VGH)
   Write_Data(*(waveform_data+154));

   Write_Command(0x04);              //Source Voltage (VSH/VSL)
   Write_Data(*(waveform_data+155));
   Write_Data(*(waveform_data+156));
   Write_Data(*(waveform_data+157));

   Write_Command(0x2C);            //VCOM Voltage
   Write_Data(*(WF_FULL+158));     // 0x45 --> -1.75V
   //Serial.print("VCOM = "); Serial.println(*(WF_FULL+158),HEX);
}

void init_SSD1680(void)
{
   Reset();                      //hardware reset
   READBUSY();
   Write_Command(0x12);          //software reset
   READBUSY();

   Write_Command(0x01);          //Driver output control      
   Write_Data(Y_Addr_Start_L);   //MUX Gate lines = 250-1 (0xF9)
   Write_Data(Y_Addr_Start_H);
   Write_Data(0x00);

   Write_Command(0x18);      //Select temperature sensor (optional)
   Write_Data(0x80);         // 48h = external, 80h = Internal

//--- ORIENTATION  ------------------------------------------------
   Write_Command(0x11);        //set data entry mode       
   Write_Data(0x03);           // Y decrement X increment (def: 0x03)

   Write_Command(0x44);        //set RAM-X address start/end position   
   Write_Data(X_Addr_Start);   // Start X (def: 0x00 +1)
   Write_Data(X_Addr_End);     // End  X (def: 0x0F +1) --> (128/8)=16
   Write_Command(0x45);        //set RAM-Y address start/end position       
   Write_Data(Y_Addr_End_L);   // Start Y_L (def: 250-1=249 (0xF9)
   Write_Data(Y_Addr_End_H);   // Start Y_H (def: 0x00)
   Write_Data(Y_Addr_Start_L);   // End Y_L (def: 0x00)
   Write_Data(Y_Addr_Start_H);   // End Y_H (def: 0x00)
   
   Write_Command(0x4E);        //set RAM X address counter to 
   Write_Data(X_Addr_Start);   // 1 (0x01)
   Write_Command(0x4F);        //set RAM Y address counter to 
   Write_Data(Y_Addr_Start_L); // 250-1=249 (0xF9)
   Write_Data(Y_Addr_Start_H); // (0x00)
//-----------------------------------------------------------------
 /*
   Write_Command(0x3C);   //Border Wavefrom Control
   Write_Data(0x05);      //LUT1, Follow LUT, VBD=VSS as GS Transition

   //Load_LUT_from_MCU((uint8_t*)WF_FULL);   
   
   Write_Command(0x22);      //Display Update Control Sequence
   Write_Data(0xB1);         // 0xB1: Load temperature, load LUT Mode1
   Write_Command(0x20); 
 */
   READBUSY();
}

//================================================================
//   Partial Update
//================================================================
void Part_Init_SSD1680(void)
{
   Reset();                  //hardware reset - ALWAYS before Init
   
   READBUSY();  
   Load_LUT_from_MCU(WF_PARTIAL);
   Write_Command(0x37);      //Setup Display Mode
   Write_Data(0x00);         // OTP Selection: 0=default, 1=spare
   Write_Data(0x00);         // Set Display Mode
   Write_Data(0x00);         // .. 0 = Mode1, 1 = Mode2
   Write_Data(0x00);         // ..
   Write_Data(0x00);         // ..
   Write_Data(0x40);         // Ping-Pong for Display Mode2
   Write_Data(0x00);         // module ID / waveform version
   Write_Data(0x00);         // ..
   Write_Data(0x00);         // 
   Write_Data(0x00);         // (no remarks)

   Write_Command(0x3C);      //Border Wavefrom (see datasheet)
   Write_Data(0x80);         // LUT0, VBD=VSS, VBD as VCOM

   Write_Command(0x22);      //Display Update Sequence Option
   Write_Data(0xC0);         //Enable clock and  Enable analog
   Write_Command(0x20);      //Activate Display Update Sequence
   READBUSY(); 
}
