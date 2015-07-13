/*

  HelloWorld.pde
  
  "Hello World!" example code.
  
  >>> Before compiling: Please remove comment from the constructor of the 
  >>> connected graphics display (see below).
  
  Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: http://code.google.com/p/u8glib/wiki/device
//U8GLIB_NHD27OLED_BW u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD27OLED_2X_BW u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD27OLED_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD27OLED_2X_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD31OLED_BW u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD31OLED_2X_BW u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD31OLED_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_NHD31OLED_2X_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGS102 u8g(13, 11, 10, 9, 8);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGM132 u8g(13, 11, 10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGM128 u8g(13, 11, 10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGM128_2X u8g(13, 11, 10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_ST7920_128X64_1X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_128X64_4X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_128X64_1X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_ST7920_128X64_4X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_ST7920_192X32_1X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_192X32_4X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_192X32_1X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_ST7920_192X32_4X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_ST7920_192X32_1X u8g(13, 11, 10);	// SPI Com: SCK = en = 13, MOSI = rw = 11, CS = di = 10
//U8GLIB_ST7920_192X32_4X u8g(10);		// SPI Com: SCK = en = 13, MOSI = rw = 11, CS = di = 10, HW SPI
//U8GLIB_ST7920_202X32_1X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_202X32_4X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, di=17,rw=16
//U8GLIB_ST7920_202X32_1X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_ST7920_202X32_4X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
//U8GLIB_LM6059 u8g(13, 11, 10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_LM6063 u8g(13, 11, 10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGXL160_BW u8g(10, 9);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGXL160_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGXL160_2X_BW u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_DOGXL160_2X_GR u8g(13, 11, 10, 9);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Reset = 8
//U8GLIB_PCF8812 u8g(13, 11, 10, 9, 8);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Reset = 8
//U8GLIB_KS0108_128 u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 14, 15, 17, 16); 		// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs1=14, cs2=15,di=17,rw=16
//U8GLIB_LC7981_160X80 u8g(8, 9, 10, 11, 4, 5, 6, 7,  18, 14, 15, 17, 16); 	// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs=14 ,di=15,rw=17, reset = 16
//U8GLIB_LC7981_240X64 u8g(8, 9, 10, 11, 4, 5, 6, 7,  18, 14, 15, 17, 16); 	// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs=14 ,di=15,rw=17, reset = 16
//U8GLIB_LC7981_240X128 u8g(8, 9, 10, 11, 4, 5, 6, 7,  18, 14, 15, 17, 16); 	// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs=14 ,di=15,rw=17, reset = 16
//U8GLIB_ILI9325D_320x240 u8g(18,17,19,U8G_PIN_NONE,16 );  			// 8Bit Com: D0..D7: 0,1,2,3,4,5,6,7 en=wr=18, cs=17, rs=19, rd=U8G_PIN_NONE, reset = 16
//U8GLIB_SBN1661_122X32 u8g(8,9,10,11,4,5,6,7,14,15, 17, U8G_PIN_NONE, 16); 	// 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 cs1=14, cs2=15,di=17,rw=16,reset = 16
//U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9);	// SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7);	// SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7 (new white HalTec OLED)
//U8GLIB_SSD1306_128X64 u8g(10, 9);		// HW SPI Com: CS = 10, A0 = 9 (Hardware Pins are  SCK = 13 and MOSI = 11)
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI 
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC

#include <SPI.h>
#include <Wire.h>
#define OLED_RESET 4

//-----------------
#include <Moteino.h>
#include <RFM69.h>
#include <LowPower.h>   //https://github.com/rocketscream/Low-Power

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
#define DEBUG

Moteino *mio;


uint8_t node_id = 1;   //This node id, default for OLED proyect
uint8_t network = 199; //Gateway

String inspector_pack = "";//node_id + RSSI_1 + RSSI_2 + ... + RSSI_n
String msg = "";//Received packets

volatile uint8_t up_down_buttons = 0;//node up or down
volatile unsigned long deb_interval, last_time, last;
uint8_t temp_button = 0;

int node_rssi = 0;
uint8_t id_msg = 0;
volatile boolean scroll = false;
String idrssi = "";


void draw(String to_print, int x, int y) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_profont12);//u8g_font_unifont
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( x, y, to_print.c_str());
}

class Inspector_string{
  private:
    unsigned int s_length;
  
  public:
    String insp_string[5] ={"*","*","*","*","*"};
    
    Inspector_string(){
      s_length = 5;
      //insp_string[s_length];//Matrix length
     /* for(unsigned int i = 0; i < s_length; i++){
       // insp_string[i] = "4";//
      }*/
    }//end constructor
    
    String insp_get_last(void){
      return insp_string[s_length-1];
    }//end insp_pop

    int insp_set_last(String last){//Set last node 
      for(unsigned int i = 1; i< s_length; i++){//move all forward a single step
          insp_string[i-1] = insp_string[i];
      }//end for
      insp_string[s_length-1] = last;
      //Serial.println(insp_string[s_length-1]);
      #if defined(DEBUG)
        for(unsigned int i = 1; i< s_length; i++){//move all forward a single step
          Serial.println(insp_string[i]);
        }//end for
      #endif
      return 1;
      
    }//emd insp_push
        
};

Inspector_string *mip;

void setup(void) {
  Serial.begin(SERIAL_BAUD);
  node_id = 1; 
    pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
   mip = new Inspector_string();
    pinMode(5, INPUT_PULLUP);//Interrupt UP
  pinMode(6, INPUT_PULLUP);//Interrupt DOWN
  pinMode(7, INPUT_PULLUP);//Interrupt CHANGE
  
 InitialiseInterrupt();
 interrupts();
}

/*
Renglones
      draw("Hello", 0, 11);
      draw("World", 0, 23);
      draw("World", 0, 35);
      draw("World", 0, 47);
      draw("World", 0, 59);    
*/

void loop(void) {
    if(temp_button != up_down_buttons){
    temp_button = up_down_buttons;
    delete(mio);
    node_id = up_down_buttons;
    mio = new Moteino(node_id, FREQUENCY, ENCRYPTKEY, network, false);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
    #if defined(DEBUG)
      Serial.println(F("Changing Node id"));  
      Serial.println(node_id);
    #endif
}
 
  mio->moteino_receive(msg);
  
    if(msg != ""){//Check if msg is empty
      node_rssi = mio->moteino_rssi();
      id_msg = mio->moteino_id_receive();
      #if defined(DEBUG)
        Serial.print(F("Node ID = "));
        Serial.println();
      #endif
      //Serial.print((unsigned int)mio->moteino_id_receive());//Send Id to RPI/Emoncms
      #if defined(DEBUG)
        Serial.print(F("Packet received from this node = "));
      #endif      
      //Serial.print(' ');
      for(uint8_t i = 0; i < msg.length(); i++){
        if((msg[i] == ';')){
          msg[i] = ' ';
        }//end if
      }//end for
      msg += ' ';
      msg += node_rssi;
      //Serial.print(msg);//Print sensor values
     // Serial.println();//end structure transmition to Emoncms
      //Serial.flush();
      idrssi += id_msg;
      idrssi +="   ";
      idrssi +=node_rssi;
      if(idrssi != ""){
        mip->insp_set_last(idrssi);
      }
      idrssi = "";
    }//end if  */
  // picture loop
  u8g.firstPage();  
  do {
    if(!scroll){
      draw("My ID:", 0, 11);
      draw(String(mio->moteino_id()), 50, 11);
      draw("Signal strength", 0, 33);
      u8g.drawHLine(0, 1+14, 60);
      draw("Node ID= ", 0, 47);
      draw(String(id_msg), 55, 47); 
      draw("RSSI= ", 0, 59);       
      draw(String(node_rssi), 50, 59);             
    }else{
       draw("#    ID", 0, 11);
       u8g.drawHLine(0, 1+11, 60);
       u8g.drawStr( 0, 20, mip->insp_string[0].c_str());
       u8g.drawStr( 0, 30, mip->insp_string[1].c_str());
       u8g.drawStr( 0, 40, mip->insp_string[2].c_str());
       u8g.drawStr( 0, 50, mip->insp_string[3].c_str());
       u8g.drawStr( 0, 60, mip->insp_string[4].c_str());
       #if defined(DEBUG)
         Serial.println(mip->insp_string[0].c_str());
         Serial.println(mip->insp_string[1].c_str());
         Serial.println(mip->insp_string[2].c_str());
         Serial.println(mip->insp_string[3].c_str());
         Serial.println(mip->insp_string[4].c_str());
         Serial.println("--------------------");
       #endif
       
//       draw(mip->insp_string[0], 0, 23);
//       draw(mip->insp_string[1], 0, 35);
//       draw(mip->insp_string[2], 0, 47);
//       draw(mip->insp_string[3], 0, 59);    
    }
    

  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}

void InitialiseInterrupt(){
  cli();
  PCICR |= (1<<PCIE2);
  PCMSK2 |= (1<<PCINT21);
  PCMSK2 |= (1<<PCINT22);
  PCMSK2 |= (1<<PCINT23);
  MCUCR = (1<<ISC01) | (1<<ISC00);
  sei();
}//end InitialiseInterrupt
 
ISR(PCINT2_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
            // will generate an interrupt: but this will always be the same interrupt routine
  last_time = millis();
  deb_interval = last_time-last;
  if(deb_interval > 20){
    if (digitalRead(5)==0){
      if(up_down_buttons < 255){//Bock nodes out of range
        up_down_buttons++;
      }//end if
    }//end if
    if(digitalRead(6)==0){
      if(up_down_buttons != 0){//Block negative nodes
        up_down_buttons--;
      }//end if
    }//end if
    if(digitalRead(7)==0){
      if(scroll){
        scroll = false;
      }else{
        scroll = true;
      }//end if
      //Crear boolean para listar los nodos
     }//end if
  }
  last = last_time;
}

