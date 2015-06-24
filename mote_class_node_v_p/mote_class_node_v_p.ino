/*
Standart Node Trasmition Network v.2

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
#include <Wire.h>
//------------------------------------------------------------

//---------
#include <SHT1x.h>
#include "Adafruit_SI1145.h"
#include "DHT.h"
//---------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
//#define DEBUG
#define DHTTYPE DHT11   // DHT 11 

uint8_t node_id = 8;  //This node id
uint8_t gw_id = 2;    //gatewayId
uint8_t netword_id = 215;//Gateway
uint8_t t_wait = 3;   //Wait T_WAIT*8 [8 because you sleep 8s]
uint8_t n_times = 0;  //Time to wait before send the packets
//-------------------------
uint8_t data_pin = 6; //SHT10 Data Pin
uint8_t clock_pin = 7; //SHT10 Clock Pin
uint8_t dht11_pin = 5; //DHT11 pin
//SI145
//A4 SDA
//A5 SCL

GenSens *mio;
SHT1x sht1x(data_pin, clock_pin);//Soil
Adafruit_SI1145 uv = Adafruit_SI1145();//UV Light

DHT dht(dht11_pin, DHTTYPE);// Initialize DHT sensor for normal 16mhz Arduino


String pck = "";//Packet to send
String msg = "";//Received packets

void setup() {
  dht.begin();
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, netword_id);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  Serial.begin(SERIAL_BAUD);
  #if defined(DEBUG)
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff); 
  #endif
  if (! uv.begin()) {
    #if defined(DEBUG)
    Serial.println("Didn't find Si1145");
    #endif
  }else{
    #if defined(DEBUG)
      Serial.println("Find Si1145");
    #endif
  }//end if    
   
}

void loop() {
  pck +=node_id;//Ad node ID
  pck +=";";
  if(n_times >= t_wait){
    //float temp_c_sht10;
    //float humidity_sht10;
    if(mio->moteino_receive(msg)){
      #if defined(DEBUG)
        if(pck != ""){
          Serial.println(msg);
        }//end if
      #endif
    }//end if
    //temp_c_sht10 = sht1x.readTemperatureC();
    //humidity_sht10 = sht1x.readHumidity();
    //Read SHT10 Sensor Soil Temp and Humidity
    pck += sht1x.readTemperatureC();//temp_c_sht10;
    pck += ";";
    pck += sht1x.readHumidity();//humidity_sht10;
    pck += ";";//Uncomment for more data to send
    
    //Read SI1145 UV Light sensor
    pck += uv.readVisible();
    pck += ";";
    pck += uv.readIR();
    pck += ";";
    float UVindex = uv.readUV();
    UVindex /= 100.0; 
    pck += UVindex;//uv.readUV();
    pck += ";";
  
    float dht11_humidity = dht.readHumidity();
    float dht11_temp = dht.readTemperature();
    if (isnan(dht11_humidity) || isnan(dht11_temp)) {// || isnan(f)
      #if defined(DEBUG)
        Serial.println("Failed to read from DHT sensor!");
      #endif
      dht11_humidity = -1;//error
      dht11_temp = -1;//error
    }
    //float dth11_heat_index = dht.computeHeatIndex(f, h);?
    pck += dht11_humidity;
    pck += ";";
    pck += dht11_temp;
    //pck += ";";

      #if defined(DEBUG)
        Serial.println(F("To Send = "));
        Serial.println(pck);
      #endif
    
    
    if(mio->moteino_send(gw_id, pck.c_str(), pck.length(), 2, 200)){
      #if defined(DEBUG)
        Serial.println(F("ok!"));
        Serial.println(pck);
      #endif
    }else{
      #if defined(DEBUG)
        Serial.println(F("nothing"));
      #endif
    }//end if
    n_times = 0;//Back to start
  }else{
    n_times++;//wait more
  }
  pck = ""; 
  mio->mote_sleep();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
