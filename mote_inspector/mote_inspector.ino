/*
Moteino Inspector Trasmition Network

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


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

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

int display_conf(uint8_t font_size, uint8_t x_cursor, uint8_t y_cursor);

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

void setup()   {                
  Serial.begin(SERIAL_BAUD);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(500);

  // draw many lines
  /*testdrawline();
  display.display();
  delay(2000);
  display.clearDisplay();*/

  // Clear the buffer.
  display.clearDisplay();

  mio  = new Moteino(node_id, FREQUENCY, ENCRYPTKEY, network, false);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  char buff[30];
    sprintf(buff, "\nTransmitting at: %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  // text display tests
  display_conf(1, 0, 15);
  display.println(buff);
  display.display();
  delay(500);
  display.clearDisplay();
  
  mip = new Inspector_string();
  
  pinMode(5, INPUT_PULLUP);//Interrupt UP
  pinMode(6, INPUT_PULLUP);//Interrupt DOWN
  pinMode(7, INPUT_PULLUP);//Interrupt CHANGE
  
 InitialiseInterrupt();
 interrupts();

}


void loop() {
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
      Serial.print(msg);//Print sensor values
      Serial.println();//end structure transmition to Emoncms
      Serial.flush();
      idrssi += id_msg;
      idrssi +="   ";
      idrssi +=node_rssi;
      if(idrssi != ""){
        mip->insp_set_last(idrssi);
      }
      idrssi = "";
    }//end if
    //#endif
   /* boolean flag_1 = true;
    boolean flag_2 = false;
    for(uint8_t i = 0; i < msg.length(); i++){
      if(((msg[i] == ';')||(msg[i]==' '))&&(flag_1)){
        inspector_pack += "_";
        flag_1 = false;
      }//end if
      if(flag_1){
        inspector_pack += msg[i];//Node id
      }//if      
      if(msg[i]=='-'){
        flag_2 = true;
      }
      if(flag_2){
        if(!(msg[i] == ';')||!(msg[i]==' ')){
          inspector_pack += msg[i]; 
        }else{

          inspector_pack += "_";
          flag_2 = false;                
        }
      }
    }//end for*/
  /*  Serial.println(inspector_pack);
      for(unsigned int i = 0; i< 5; i++){
    Serial.print(mip->insp_string[i]);
    Serial.print(" ");
      Serial.print("|");
  }
  Serial.println("");
  Serial.println("------");*/
   // mip->insp_set_last(inspector_pack);

  if(!scroll){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println(F("Contador de Moteino!!"));  
    //delay(2000);
    //display.clearDisplay();
    display.setTextColor(WHITE);
    display.println("");
    //---------node id---------
    display.setTextColor(WHITE);
    display.setCursor(0,20);
    display.print(F("This node ID:"));
    
    display.setTextColor(WHITE);
      display.setCursor(80,20);
    display.println(mio->moteino_id());//This node ID
    //----node rssi---incomming pack
    display.setTextColor(WHITE);
    display.setCursor(0,40);
    display.print(F("Node id= "));
    
    //---received node id------
    display.setTextColor(WHITE);
    display.setCursor(0,40);
    display.println(id_msg);//------ID message received

    //----RSSI packet receive-----
    display.setTextColor(WHITE);
    display.setCursor(0,50);
    display.print(F("RSSI= "));
    
    display.setTextColor(WHITE);
    display.setCursor(50,50);
    display.println(node_rssi);//----RSSI
    
    //---------------------------
      display.display();
    //delay(2000);
    display.clearDisplay();    
  }else{
    //testscrolltext();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(F("#    ID"));
    
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println(mip->insp_string[0]);

    display.setTextColor(WHITE);
    display.setCursor(0,20);
    display.println(mip->insp_string[1]); 
    Serial.print(mip->insp_string[1]);
    Serial.print(F("+"));
    
    display.setTextColor(WHITE);
    display.setCursor(0,30);
    display.println(mip->insp_string[2]);
    Serial.print(mip->insp_string[2]);
    Serial.print(F("+"));
      
    display.setTextColor(WHITE);
    display.setCursor(0,40);
    display.println(mip->insp_string[3]);    
    Serial.print(mip->insp_string[3]);
    Serial.print(F("+"));

    display.setTextColor(WHITE);
    display.setCursor(0,50);
    display.println(mip->insp_string[4]);   
    Serial.print(mip->insp_string[4]);
    Serial.println(F("|"));
    
    #if defined(DEBUG)
      Serial.println(F("Nodo"));
      for(uint8_t i = 0; i < 5; i++){
        Serial.print(mip->insp_string[i]);
        Serial.print(F("+"));
      }
    #endif
    
    display.display();
    delay(1000);
    display.clearDisplay();
       
  }//end if
  
}//end loop

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
  if(deb_interval > 10){
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

int display_conf(uint8_t font_size, uint8_t x_cursor, uint8_t y_cursor){
  display.setTextSize(font_size);
  display.setTextColor(WHITE);
  display.setCursor(x_cursor,y_cursor);
}//end display_conf

