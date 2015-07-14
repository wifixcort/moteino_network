/*
Moteino Inspector Trasmition Network
SSD1306 U8glib library version

Ricardo Mena C
ricardo@crcibernetica.com
http://crcibernetica.com

 License
 **********************************************************************************
 This program is free software; you can redistribute it 
 and/or modify it under the terms of the GNU General    
 Public License as published by the Free Software       
 Foundation; either version 3 of the License, or        
 (at your option) any later version.                    
                                                        
 This program is distributed in the hope that it will   
 be useful, but WITHOUT ANY WARRANTY; without even the  
 implied warranty of MERCHANTABILITY or FITNESS FOR A   
 PARTICULAR PURPOSE. See the GNU General Public        
 License for more details.                              
                                                        
 You should have received a copy of the GNU General    
 Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
                                                        
 Licence can be viewed at                               
 http://www.gnu.org/licenses/gpl-3.0.txt

 Please maintain this license information along with authorship
 and copyright notices in any redistribution of this code
 **********************************************************************************
*/

#include "U8glib.h"
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
      mio = new Moteino(node_id, FREQUENCY, ENCRYPTKEY, network, false);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
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
  digitalWrite(5 ,HIGH);
  digitalWrite(6 ,HIGH);
  digitalWrite(7 ,HIGH);
  
 //InitialiseInterrupt();
 //interrupts();
}

void loop(void) {
  buttons();
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
      #if defined(DEBUG)
        Serial.print(msg);//Print sensor values
        Serial.println();//end structure transmition to Emoncms
        Serial.flush();
      #endif
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
       //u8g.drawHLine(0, 1+11, 60);
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

int buttons(){
  last_time = millis();
  deb_interval = last_time-last;
  if(deb_interval > 50){
    if (digitalRead(6)==0){
      if(up_down_buttons < 255){//Bock nodes out of range
        up_down_buttons++;
      }//end if
    }//end if
    if(digitalRead(5)==0){
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
}//end buttons
/*

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
*/
