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

#include <SoftwareSerial.h>

#include "Adafruit_FONA.h"


//#include <Wire.h> //I2C needed for sensors
//#include "MPL3115A2.h" //Pressure sensor
//#include "HTU21D.h" //Humidity sensor


//SoftwareSerial debug(3, 4);//ARDUINO(RX, TX)->ESP8266(TX, RX)
#define _serial Serial
//#define _dbg  debug

//---------FONA network-------------
//
#define FONA_RX 3
#define FONA_TX 4
#define FONA_RST 5
#define FONA_KEY 7
#define FONA_PS 8

//------------------------------------------------------------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
#define DEBUG //uncoment for debuging
//#define DEBUG1 //uncoment for debuging
//#define FREERAM

GenSens *mio;

//------------Network identifiers-----------------
uint8_t node_id = 3;   //This node id
uint8_t gw_id = 1;    //gatewayId
uint8_t netword_id = 215; //Gateway


//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

//---Paquetes to route----
String pck = "";//Packet to send
String msg = "";//Received packets
String temp ="";//Temporal buffer to receive from RX/TX

//=======FONA things======
// this is a large buffer for replies
char replybuffer[255];
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);




void setup() {
  Serial.begin(SERIAL_BAUD);
  //while (!Serial);///?????
  // make it slow so its easy to read!
  fona_on();
  fonaSS.begin(4800); // if you're using software serial
    // See if the FONA is responding
  check_fona();
  
  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }//end if
  
  //APN configuration
  //fona.setGPRSNetworkSettings(F("kolbi3g"), F(""), F(""));
  
  gprs_disable();
  gprs_enable();
  
  //>>>>>>>>>>>>>>>>
  #if defined(DEBUG)
    digitalWrite(9, HIGH);
  #endif

  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, netword_id);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  
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
    check_fona();
  }//end if
   
  #if defined(FREERAM)
    Serial.print("Free 2 = ");
    Serial.println(freeRam());
  #endif

  //---------All nodes---------------
  mio->moteino_receive(msg);
  #if defined(DEBUG1)
    if(msg != ""){
      Serial.println(F("Paquete de un nodo"));
      Serial.println(msg);
    }
  #endif
  
  temp = serial_read(1000);//Get messages from UNO

  if(temp != ""){//Check empty temp
    //Send messages received from UNO and send to GW
    pck = String(node_id);//add node ID
    pck += ' ';
    pck += temp;
    temp = "";
    send_url(pck);//Send URL through FONA to emoncms cloud
    String kk1 = "13 34 65 67 23";
    send_url(kk1);
    String kk2 = "14 34 65 67 23";
    send_url(kk2);
    String kk3 = "15 34 65 67 23";
    send_url(kk3);
    String kk4 = "16 34 65 67 23";
    send_url(kk4);
    String kk5 = "17 34 65 67 23";
    send_url(kk5);
    String kk6 = "18 34 65 67 23";
    send_url(kk6);
    String kk7 = "19 34 65 67 23";
    send_url(kk7);
    if(mio->moteino_send(gw_id, pck.c_str(), pck.length(), 2, 100)){//Wheater Station
      #if defined(DEBUG)
        Serial.println(F("Interno ok"));
        Serial.println(pck);
      #endif
    }else{
      #if defined(DEBUG)
        Serial.println(F("Interno not send"));
        Serial.println(pck);       
      #endif
    }//end if
  }//end if
  #if defined(FREERAM)
    Serial.print("Free 5 = ");
    Serial.println(freeRam());
  #endif
   if(msg != ""){//Check empty msg
    //Adding RSSI
    msg += ' ';
    msg += mio->moteino_rssi();
    send_url(msg);//Send URL through FONA to emoncms cloud
    if(mio->moteino_send(gw_id, msg.c_str(), msg.length(), 2, 100)){//Wheater Station
      #if defined(DEBUG)
        Serial.println(F("Node ok"));
        Serial.println(msg);
      #endif
    }else{
      #if defined(DEBUG)
        Serial.println(F("Node not send"));
        Serial.println(msg);       
      #endif
    }//end if
  }//end if 
   
  
  Serial.flush();
}//end loop

String json_split(String &message, String &node_id){
  String brak = "";// "{";
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
      brak += String(data_num) + ":" + number + ",";
      data_num++;
      //Serial.println(brak);
      number = "";
    }else{
      number += message[j];
    }//end if
    if(message.length() == j+1){
      brak += String(data_num) + ":" + number;
    }
  }//end for
  //brak += "}";
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
  String url = "166.78.62.254/input/post.json?node="+node+"&apikey=d1699ac02ed979dd0c4af09b84a3c9f5&json=\{";
  char data[json.length()];
  json.toCharArray(data, json.length());
  char clos[] = "}";

//166.78.62.254/input/post.json?node=3&json={1:3,2:482,3:588}&apikey=d1699ac02ed979dd0c4af09b84a3c9f5
  
  char c_url[url.length()];
  sprintf(c_url, "%s%s%s", url.c_str(),data,clos);
  flushSerial();
  
  #if defined(DEBUG)  
    Serial.println(c_url);
    Serial.println(F("****"));
  #endif
       
  if (!fona.HTTP_GET_start(c_url, &statuscode, (uint16_t *)&length)) {
    Serial.println("Failed!");
    return -1;
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


