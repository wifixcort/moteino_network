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

//#include <Wire.h> //I2C needed for sensors
//#include "MPL3115A2.h" //Pressure sensor
//#include "HTU21D.h" //Humidity sensor



SoftwareSerial debug(3, 4);//ARDUINO(RX, TX)->ESP8266(TX, RX)

#define _serial Serial
#define _dbg  debug

//------------------------------------------------------------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
#define DEBUG //uncoment for debuging
//#define DEBUG1 //uncoment for debuging
//#define FREERAM


GenSens *mio;

//------------Network identifiers-----------------
uint8_t node_id = 2;   //This node id
uint8_t gw_id = 1;    //gatewayId
uint8_t netword_id = 215; //Gateway

//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

String pck = "";//Packet to send
String msg = "";//Received packets
String temp ="";


uint8_t count = 0;//Debug purpose

void setup() {
  #if defined(DEBUG)
    digitalWrite(9, HIGH);
  #endif

  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, netword_id);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  Serial.begin(SERIAL_BAUD);
   _dbg.begin(SERIAL_BAUD);
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
  
  #if defined(DEBUG)
   // Serial.println(count);
  #endif
   
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
  #if defined(FREERAM)
    Serial.print("Free 4 = ");
    Serial.println(freeRam());
  #endif
  //Time!!!
  
  temp = serial_read(1000);//Get messages from UNO

  if(temp != ""){//Check empty temp
    //Send messages received from UNO and send to GW
    pck = String(node_id);//add node ID
    pck += ' ';
    pck += temp;
    temp = "";
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

