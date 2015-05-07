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

GenSens *mio;

uint8_t node_id = 8;  //This node id
uint8_t gw_id = 1;    //gatewayId
uint8_t gateway = 300;//Gateway
uint8_t t_wait = 1;   //Wait T_WAIT*8 [8 because you sleep 8s]
uint8_t n_times = 0;  //Time to wait before send the packets 

//#include "apikey.h" 
//use tabs to add apikey.h file or uncomment this line
//#define APIKEY "xxxxxxxxxxxxxxxxx" //emoncms Account--> "Write API Key"

String pck = "34.4;2.6;3;4";//Packet to send
String msg = "";//Received packets

void setup() {
  // put your setup code here, to run once:
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, gateway);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
    Serial.begin(SERIAL_BAUD);
  //#if defined(DEBUG)
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff); 
  //#endif
   
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
    if(mio->moteino_send(gw_id , pck.c_str(), pck.length(), 2, 100)){
      #if defined(DEBUG)
        Serial.print(F("ok!"));
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
