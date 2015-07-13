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

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   115200 
#define DEBUG //uncoment for debuging

GenSens *mio;

uint8_t node_id = 1;   //This node id
uint8_t network = 199; //Gateway
//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

String pck = "";//Packet to send
String msg = "";//Received packets

void setup() {
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, network, false);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  Serial.begin(SERIAL_BAUD);
  #if defined(DEBUG)
    Serial.println(F("This is your gateway")); 
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
  #endif
}//end setup

void loop(){
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
    }//end if
    
    //n_times = 0;//Back to start
  //}else{
   // n_times++;//wait more
  //}  
  Serial.flush();
  //mio->mote_sleep();
  //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
