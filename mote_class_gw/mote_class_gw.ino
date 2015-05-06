#include <GenSens.h>
//-----Need to be declared for correct GenSens functioning----
#include <RFM69.h>
#include <SPI.h>
#include <LowPower.h>   //https://github.com/rocketscream/Low-Power
//------------------------------------------------------------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
//#define DEBUG //uncoment for debuging

GenSens *mio;

uint8_t node_id = 1;   //This node id
uint8_t gateway = 300; //Gateway
//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

//#include "apikey.h" 
//use tabs to add apikey.h file or uncomment this line
//#define APIKEY "xxxxxxxxxxxxxxxxx" //emoncms Account--> "Write API Key"

String pck = "";//Packet to send
String msg = "";//Received packets

void setup() {
  // put your setup code here, to run once:
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, gateway);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  Serial.begin(SERIAL_BAUD);
  #if defined(DEBUG) 
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
  #endif
}//end setup

void loop(){
  //if(n_times >= T_WAIT){
    mio->moteino_receive(msg);
    
    if(msg != ""){//Check if msg is empty
      Serial.println(mio->moteino_id_receive());//Send Id to RPI
      String sensor_c = "";//
      for(uint8_t i = 0; i < msg.length(); i++){
        if((msg[i] != ';')){
          #if defined(DEBUG)
            Serial.print(msg[i]);
          #endif
          sensor_c += msg[i];//Sensor packet
        }else{
          Serial.print(sensor_c.c_str());//Send sensor_c to RPI just print (toFloat())
          Serial.print(F(" "));
          sensor_c = "";//Clean senor_c for next sensor value
          #if defined(DEBUG)
            Serial.println(F("end ond and start new sensor"));
          #endif
        }//end if
      }//end for
      Serial.println();//end transmition to RPI
    }//end if
    
    //n_times = 0;//Back to start
  //}else{
   // n_times++;//wait more
  //}  
  Serial.flush();
  //mio->mote_sleep();
  //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
