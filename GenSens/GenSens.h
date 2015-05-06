#ifndef __GENSENS_H__
#define __GENSENS_H__
#include <Arduino.h>

#define ONE_WIRE_BUS 7
// Simplified Moteino Node Test

#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define LED           9 // Moteinos have LEDs on D9
  #define FLASH_SS      8 // and FLASH SS on D8
#endif


class GenSens{
  private:
    uint8_t node_id;
	uint8_t node_id_receive;
	//const char *moteino_type;
	//unsigned long uptime;
	RFM69 radio;
	void alert_incomming(uint8_t t_delay);
  public:
    GenSens(){};//Empty constructor
	~GenSens();
    GenSens(uint8_t &node_Id, uint8_t frequency, const char *encryptK, uint8_t netw_id, boolean mote_type=false);
    //int moteino_init();//
	boolean moteino_receive(String &msg);
	boolean moteino_send(uint8_t &gateway, const char *buffer, uint8_t bufferSize, uint8_t retries=2, uint8_t retryWaitTime=40);// const void* buffer
	void mote_sleep();
   uint8_t moteino_id_receive();
};
#endif /* #ifndef __GENSENS_H__ */
