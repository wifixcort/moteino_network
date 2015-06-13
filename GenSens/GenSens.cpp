/*
Easy handler for RFM69

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

#include "GenSens.h"

GenSens::GenSens(uint8_t &node_Id, uint8_t frequency, const char *encryptK, uint8_t netw_id, boolean mote_type){
	node_id = node_Id;
	radio.initialize(frequency, node_id, netw_id);
	if(mote_type){//is this a high power moteino?
		radio.setHighPower();
	}//end if
	radio.encrypt(encryptK);//(const char*)
}

boolean GenSens::moteino_receive(String &msg){
  msg = "";//Delete old string
  node_id_receive = radio.SENDERID;//Save node id
  if (radio.receiveDone()){
	//msg += radio.SENDERID;//is the same uint8_t than DEC
	//msg += "<";//Close ID node identifier
	for(uint8_t i = 0; i < radio.DATALEN; i++){
	  msg += (char)radio.DATA[i];//This is the data
	}//end for
	//msg += ">";
	if (radio.ACKRequested()){
	  radio.sendACK();
	  //msg += " - ACK sent";
	}//end if
	alert(3);
	return true;
  }//end if
  return false;//
}//end moteino_receive

boolean GenSens:: moteino_send(uint8_t &gateway, const char *buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime){//retrines default 2, retriesWait default 40

  if (radio.sendWithRetry(gateway, buffer, bufferSize, retries, retryWaitTime)){
	alert(3);
	return true;//ok!
  }else{
	return false;//nothing!
  }//end if
}//end moteino_send

uint8_t GenSens::moteino_id_receive(){
  return node_id_receive;
}

void GenSens::mote_sleep(){
  Serial.flush();
  radio.sleep();
}//end mote_sleep


void GenSens::alert(uint8_t t_delay){
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  delay(t_delay);
  digitalWrite(LED,LOW);
}//end alert_incomming

int GenSens::moteino_rssi(){
  return radio.RSSI;
}//end moteino_rssi
