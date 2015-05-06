#include "GenSens.h"

GenSens::GenSens(uint8_t &node_Id, uint8_t frequency, const char *encryptK, uint8_t netw_id, boolean mote_type){
	node_id = node_Id;
	radio.initialize(frequency, node_id, netw_id);
	if(mote_type){//is this a high power moteino?
		radio.setHighPower();
	}//end if
	radio.encrypt(encryptK);//(const char*)
	//	  char buff[50];
	//sprintf(buff, "\nTransmitting at %d Mhz...", frequency==RF69_433MHZ ? 433 : frequency==RF69_868MHZ ? 868 : 915);
	//Serial.println(buff);
}

//int GenSens::moteino_init(){//

//}//end moteino_init

boolean GenSens::moteino_receive(String &msg){
  msg = "";//Delete old string
  node_id_receive = radio.SENDERID;
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
	alert_incomming(3);
	return true;
  }//end if
  return false;//
}//end moteino_receive

boolean GenSens:: moteino_send(uint8_t &gateway, const char *buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime){//retrines default 2, retriesWait default 40

  if (radio.sendWithRetry(gateway, buffer, bufferSize, retries, retryWaitTime)){
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


void GenSens::alert_incomming(uint8_t t_delay){
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  delay(t_delay);
  digitalWrite(LED,LOW);
}//end alert_incomming
