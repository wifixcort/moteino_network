/*
Standart Gateway Trasmition Network v.2

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


#include <GenSens.h>
//-----Need to be declared for correct GenSens functioning----
#include <RFM69.h>
#include <SPI.h>
#include <LowPower.h>   //https://github.com/rocketscream/Low-Power
//------------------------------------------------------------
#include <SoftwareSerial.h>

#include "Adafruit_FONA.h"
#define _serial Serial

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   115200 
#define DEBUG //uncoment for debuging

#define FONA_RX 3
#define FONA_TX 4
#define FONA_RST 5
#define FONA_KEY 7
#define FONA_PS 8

//=======FONA things======
// this is a large buffer for replies
char replybuffer[255];
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);



GenSens *mio;

uint8_t node_id = 1;   //This node id
uint8_t gateway = 215; //Gateway
//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

String pck = "";//Packet to send
String msg = "";//Received packets

String apikey = "yourApikey";

void setup() {
  Serial.begin(SERIAL_BAUD);
  //while (!Serial);///?????
  // make it slow so its easy to read!
  //fona_on();
  fonaSS.begin(4800); // if you're using software serial
    // See if the FONA is responding
  check_fona();
  
  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    #if defined(DEBUG)
      Serial.print("SIM card IMEI: "); Serial.println(imei);
    #endif
  }//end if
  
  //APN configuration
  //fona.setGPRSNetworkSettings(F("kolbi3g"), F(""), F(""));
  
  gprs_disable();
  gprs_enable();
  
  //>>>>>>>>>>>>>>>>
  #if defined(DEBUG)
    digitalWrite(9, HIGH);
  #endif

  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, gateway, true);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  
   //_dbg.begin(SERIAL_BAUD);
  #if defined(DEBUG)
    Serial.println(F("This is your Proxy")); 
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
  #endif
  #if defined(FREERAM)
    Serial.print("Free 1 = ");
    Serial.println(freeRam());
  #endif
}//end setup

void loop(){
  if(check_fona() == -1){
    fona_off();
    fona_on();
    gprs_disable();
    gprs_enable();
  }//end if
  //if(n_times >= T_WAIT){
    mio->moteino_receive(msg);
    
    if(msg != ""){//Check if msg is empty
      #if defined(DEBUG)
        Serial.print(F("Node ID = "));
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
      msg += mio->moteino_rssi();
      Serial.print(msg);//Print sensor values
      Serial.println();//end structure transmition to Emoncms
      send_url(msg);
      msg = "";
    }//end if
    
    //n_times = 0;//Back to start
  //}else{
   // n_times++;//wait more
  //}  
  Serial.flush();
  //mio->mote_sleep();
  //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}



String json_split(String &message, String &node_id){
  String brak = "\{";
  String number = "";
  uint16_t j = 0;
  uint8_t data_num = 1;
  node_id = "";
  for(uint8_t i = 0; i < message.length(); i++){
    if((message[i] == ';')||(message[i] == ' ')){
      j = i+1;
      break;
    }else{
      node_id += message[i];
    }//end if
  }//end for

  for(j; j < message.length(); j++){
    if((message[j] == ';')||(message[j] == ' ')){
      brak += String(data_num) + "\:" + number + "\,";
      data_num++;
      //Serial.println(brak);
      number = "";
    }else{
      if(message[j]!= '\r'){
        number += message[j];
      }
    }//end if
    
  }//end for
  brak += String(data_num) + "\:" + number+"\}";
  return brak;
  
}//end json_pck


String serial_read(unsigned long timeOut){
  String mjs = "";
  unsigned long start = millis();
  while(millis()-start < timeOut){   
    if(_serial.available()> 0){   
      char c = _serial.read(); // read the next character.
      if(c == '\n'){break;}
      mjs +=c;
    }//end if
  }//end while
  return mjs;
}//end serial_read


int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}//end freeRam

   // Post data to website
int send_url(String &raw_paq){
  uint16_t statuscode;
  int16_t length;
  String node = "";//Store node id
  String json = json_split(raw_paq, node);//split packet into json format and store node id througth reference
  String url = "166.78.62.254/input/post.json?node="+node+"&apikey="+apikey+"&json=";

  
  
  int data_len = json.length()+1;
  char data[data_len];
  json.toCharArray(data, data_len);
  #if defined(DEBUG)
    Serial.println(json);
  #endif

  int l_url = url.length()+json.length();//strlen(data)
  char c_url[l_url];
  sprintf(c_url, "%s%s", url.c_str(),json.c_str());
  flushSerial();
  
  #if defined(DEBUG)  
    Serial.println(c_url);
    Serial.println(F("****"));
  #endif
       
  if (!fona.HTTP_GET_start(c_url, &statuscode, (uint16_t *)&length)) {
    Serial.println("GPRS send failed!");
    return -1;
  }else{
    Serial.println("GPRS send ok");
  }
  while (length > 0) {
    while (fona.available()) {
      char c = fona.read();     
           // Serial.write is too slow, we'll write directly to Serial register!
      #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
           loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
           UDR0 = c;
      #else
           Serial.write(c);
      #endif
      length--;
      if (! length) break;
    }//end while
  }//end while
  #if defined(DEBUG)
    Serial.println(F("\n****"));
  #endif
  fona.HTTP_GET_end();
}//end send_url
/*
int get_int_len (int value){
  int l=1;
  while(value>9){ l++; value/=10; }
    return l;
}//end get_int_len
*/
int gprs_enable(){
  // turn GPRS on
  if (!fona.enableGPRS(true)){
    #if defined(DEBUG)
      Serial.println(F("Failed to turn on GPRS"));
    #endif
    gprs_enable();
  }else{
    #if defined(DEBUG)
      Serial.println(F("GPRS ON"));
    #endif
  }
}//end gprs_enable

int gprs_disable(){
       // turn GPRS off
       if (!fona.enableGPRS(false)){
         #if defined(DEBUG)
           Serial.println(F("Failed to turn off"));
         #endif
       }
}//end gprs_disable


void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}//end flushSerial


int check_fona(){
  // See if the FONA is responding
  if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1)
    #if defined(DEBUG)
      Serial.println(F("Couldn't find FONA"));
    #endif  
    return -1;
  }
  #if defined(DEBUG)
    Serial.println(F("FONA is OK"));
  #endif
  //configure a GPRS APN
  fona.setGPRSNetworkSettings(F("kolbi3g"), F(""), F(""));
  return 0;
}//end check_fona

void fona_off(){
  #if defined(DEBUG)
    Serial.println("Turning off Fona: ");
  #endif
  while(digitalRead(FONA_PS)==LOW){
    digitalWrite(FONA_KEY, LOW);
  }
  digitalWrite(FONA_KEY, HIGH);
  delay(4000);
}

void fona_on(){
  #if defined(DEBUG)
    Serial.println("Turning on Fona: ");
  #endif
  while(digitalRead(FONA_PS)==HIGH){
    digitalWrite(FONA_KEY, LOW);
  }
  digitalWrite(FONA_KEY, HIGH);
  delay(4000);
}


