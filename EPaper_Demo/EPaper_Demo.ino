/**************************************************************************************
                                    MDE0213A122250BW

   Size/Type:       2.13" E-Paper
   Resolution:      122x250
   IC:              SSD1680
   MCU:             Adafruit Pro Trinket 3V (ATmega328p) 12MHz
   Description:     Black & White E-Paper Demo with Partial Refresh. The demo emulates
                    an application with date, time and battery "live" indication. 

   Company:         Midas Components Ltd
   Authors:         I. Antonakis
   Date:            02/03/2022
   Release:         v 1.0

 *************************************************************************************/

#include "EPaper.h"

/**************************************************************************************
                                    SET-UP FUNCTION
 *************************************************************************************/
void setup()
{
  //Enable Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("Initializing DEMO");

  //Setup pin directions
  pinMode(DC, OUTPUT);
  pinMode(nCS, OUTPUT);
  pinMode(BUSY, INPUT);
  pinMode(RESET, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);

  //Initialization routine -- Full Update settings
  init_SSD1680();
  delay(20);
  
  Clear_Screen(WHITE); //clear any previous image

} // END of setup()

/**************************************************************************************
                                       MAIN LOOP
 *************************************************************************************/
void loop()
{
   int x,y;
   uint8_t temp[2];
   uint8_t* ptr;
   int temperature;

   //read temperature
   ptr = Read_Data(0x1B, temp, 2);
   temperature = (ptr[0]<<4) | (ptr[1]>>4);

   Serial.print("Temp. HEX = 0x");
   Serial.print(temperature,HEX);Serial.print("  "); Serial.println(temperature,BIN);
//   Serial.print("Temp. DEC = ");
//   Serial.println(temperature,DEC); 
   Serial.print("Temp. Deg = ");
   Serial.println((temperature/16),DEC);
   //delay(1000);
   
   Display_Image(background, 0xFF, RAM1);        //  write full image to RAM1 and RAM2
   Display_Image(background, 0xFF, RAM2);        // (static images need to fill both RAM's)
   Update();                                     //  perform full refresh

   //Display partial images as static when they do not change frequently.
   //For this we use FUll initialization and full refresh
   Display_Static_Part( 0, 50, &NUM[1][0], 16, 16);       // 100%
   Display_Static_Part( 16, 50, &NUM[0][0], 16, 16);
   Display_Static_Part( 32, 50, &NUM[0][0], 16, 16);
   Display_Static_Part( 48, 50, percent, 16, 16);
   Display_Static_Part( 80, 46, &battery[5][0], 40, 24);  // full battery 
   
   Display_Static_Part( 30, 90, &NUM[2][0], 16, 16);      // Time - set 
   Display_Static_Part( 46, 90, &NUM[2][0], 16, 16);
   Display_Static_Part( 62, 90, dots, 16, 16);
   Display_Static_Part( 78, 90, &NUM[5][0], 16, 16);
   Display_Static_Part( 94, 90, &NUM[0][0], 16, 16);
   
   Display_Static_Part( 0, 128, date, 128, 16);
   Update();                                       //  perform full refresh
   Epaper_Sleep();                                 // turn off display controller
   delay(1000);

   Serial.println("Clock");
   digital_clock(90, "2251", 1000);

  //RESET .....
  Serial.println("RESET .....");
  Part_off();   

  init_SSD1680();
  Clear_Screen(WHITE);
  delay(20);

} // End of loop()

/**************************************************************************************
                                    DISPLAY ROUTINES
 *************************************************************************************/
void Clear_Screen(unsigned char pixels)
{
   unsigned int i;
   Write_Command(0x24);   //write to RAM1: 0x00 = BLACK, 0xFF = WHITE
   for (i = 0; i < FULLSCREEN_BYTES; i++)
      Write_Data(pixels);
   Write_Command(0x26);   //write to RAM2: 0x00 = BLACK, 0xFF = WHITE
   for (i = 0; i < FULLSCREEN_BYTES; i++)
      Write_Data(0x00);
   Update();
}

void Display_Image(const unsigned char *point, uint8_t inv, uint8_t RAM)
{
   unsigned int i;
   unsigned char buff;

   if(RAM==0x2)
      Write_Command(0x26);   //write RAM2
   else
      Write_Command(0x24);   //write RAM1  
  
   for (i = 0; i < FULLSCREEN_BYTES; i++)
   {
      buff = pgm_read_byte_near(point);
      Write_Data(buff ^ inv);
      point++;
   }
   //Update();
}


/**************************************************************************************
                                    OTHER  FUNCTIONS
 *************************************************************************************/
void battery_indicator(int8_t lvl)
{
   Part_Init();
   Part_Clear( 0, 50, WHITE, Disp_width, 100, RAM1);
   Part_Clear( 0, 50, WHITE, Disp_width, 100, RAM2);
   Part_Update();
   Serial.print("Partial clear before battry display");
   delay(2000);
   init_SSD1680();
   if(lvl>=5) {
      Display_Static_Part( 0, 50, &NUM[1][0], 16, 16);
      Display_Static_Part( 16, 50, &NUM[0][0], 16, 16);
   }else {
      Display_Static_Part( 16, 50, &NUM[lvl*2][0], 16, 16);
   }
   Display_Static_Part( 32, 50, &NUM[0][0], 16, 16);
   Display_Static_Part( 48, 50, percent, 16, 16);
   Display_Static_Part( 80, 46, &battery[lvl][0], 40, 24);
   Display_Static_Part( 0, 128, date, 128, 16);
   Update();
   Part_off(); 
   Serial.print("Battery status: "); Serial.println(lvl);

}
 
void digital_clock(uint16_t Y, char init_time[4], uint16_t time_frame)
{  // eg. init_time = "2359"  for 23:59 (max time)

   uint8_t m1, m0;            //minutes
   uint8_t h1, h0;            //hours
   uint8_t t1,t2,t3,t4;       //time counters
   int8_t charge;             //battery level
   uint8_t refresh_counter;

   h1 = init_time[0]-0x30;    //store ASCII char ('0'-'9') and convert to numbers (0-9)
   h0 = init_time[1]-0x30; 
   m1 = init_time[2]-0x30; 
   m0 = init_time[3]-0x30; 

   if(h1>2 || h0>9 || m1>5 || m0>9) //error handling
   {
      //ERROR: Max time is "2359" or 23:59
      h1=0; h0=0; m1=0; m0=0;      //reset time to 00:00
   }

   //SET Initial Time
   Part_Init();
   Display_Part( 30, Y, &NUM[h1][0], 16, 16, RAM1);
   Display_Part( 46, Y, &NUM[h0][0], 16, 16, RAM1);
   Display_Part( 62, Y, dots, 16, 16, RAM1);
   Display_Part( 78, Y, &NUM[m1][0], 16, 16, RAM1);
   Display_Part( 94, Y, &NUM[m0][0], 16, 16, RAM1);
   Part_Update();
   Part_off();  
   
   //START Clock
   //for(t1=h1; t1<3; t1++)
   t1=h1;
   charge=4;
   refresh_counter=0; 
   while(t1<3)
   {
      for(t2=h0; t2<10; t2++)
      {
         for(t3=m1; t3<6; t3++)
         {
            for(t4=m0; t4<10; t4++)
            {
               Part_Init();
             
               Display_Part( 30, Y, &NUM[t1][0], 16, 16, RAM1);
               Display_Part( 46, Y, &NUM[t2][0], 16, 16, RAM1);
               Display_Part( 62, Y, dots, 16, 16, RAM1);
               Display_Part( 78, Y, &NUM[t3][0], 16, 16, RAM1);
               Display_Part( 94, Y, &NUM[t4][0], 16, 16, RAM1);
               Part_Update();
               Part_off(); 
               refresh_counter+=1;
               delay(time_frame);
               if(t4>=9)
               {
                  m0=0;
                  break;
               }
            } //end t4

            if(refresh_counter>=20)
            {
               refresh_counter=0;

               battery_indicator(charge);
               charge--;
               if(charge<0) charge=5;   //reset charge level
//               init_SSD1680();
//               Display_Image(background, 0xFF, RAM1);
//               Display_Image(background, 0xFF, RAM2);
//               Update();

               Serial.println("Full refresh after 20 Part Updates");
            }
            
            if(t3>=5)
            {
               m1=0;
               break;
            }
            
            
         } //t3
          
         
         if((t1<2 && t2>=9) || (t1>=2 && t2>=3))
         {
            h0=0;
            break;
         }
      }
      t1++;
      if(t1>2)
      {
         h1=0;
         t1=0;
//         init_SSD1680();
//         Clear_Screen(WHITE);
//         Epaper_Sleep(); 
         break;
      }
   }
} //*/
