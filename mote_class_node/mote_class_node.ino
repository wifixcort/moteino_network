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
//------------------------------------------------------------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
#define DEBUG

uint8_t node_id = 8;  //This node id
uint8_t gw_id = 1;    //gatewayId
uint8_t netword_id = 199;//Gateway
uint8_t t_wait = 1;   //Wait T_WAIT*8 [8 because you sleep 8s]
uint8_t n_times = 0;  //Time to wait before send the packets

GenSens *mio;

String pck = "";//Packet to send
String msg = "";//Received packets

void setup() {
  // put your setup code here, to run once:
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, netword_id);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
    Serial.begin(SERIAL_BAUD);
  //#if defined(DEBUG)
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff); 
  //#endif
  //Add node_id to pck, sometimes radio.SENDERID is 0 -.- no id node #
  pck +=node_id;//Ad node ID
  pck +=";3;2;5;4;8;6;9";
   
}

void loop() {
  // put your main code here, to run repeatedly:
  if(n_times >= t_wait){
    mio->moteino_receive(msg);
    #if defined(DEBUG)
      if(pck != ""){
        Serial.println(msg);
      }
    #endif
    //pck += ;
    if(mio->moteino_send(gw_id, pck.c_str(), pck.length(), 2, 200)){
      #if defined(DEBUG)
        Serial.print(F("ok!"));
        Serial.println(pck);
      #endif
    }else{
      #if defined(DEBUG)
        Serial.print(F("nothing"));
      #endif
    }//end if
    n_times = 0;//Back to start
  }else{
    n_times++;//wait more
  }  
  mio->mote_sleep();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
