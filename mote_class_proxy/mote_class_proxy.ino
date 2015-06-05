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

#include <Wire.h> //I2C needed for sensors
#include "MPL3115A2.h" //Pressure sensor
#include "HTU21D.h" //Humidity sensor

#include <PinChangeInt.h>


//------------------------------------------------------------

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   9600
#define DEBUG //uncoment for debuging
//#define DEBUG1 //uncoment for debuging

GenSens *mio;
MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//-------------------Hardware pin definitions--------------------------------
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte WSPEED = 3;
//const byte RAIN = 4;//Original pin 2. Moteino use this pin
const byte RAIN = 5;
const byte STAT1 = 7;
const byte STAT2 = 8;//not needed
//Do not use pin 8 if moteino have flash memory

// analog I/O pins
const byte WDIR = A0;
const byte LIGHT = A1;
const byte BATT = A2;
const byte REFERENCE_3V3 = A3;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;


byte windspdavg[60]; //120 bytes to keep track of 2 minute average
int winddiravg[60]; //120 ints to keep track of 2 minute average
float windgust_10m[10]; //10 floats to keep track of 10 minute max
int windgustdirection_10m[10]; //10 ints to keep track of 10 minute max

volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain

//These are all the weather values that wunderground expects:
//--------
int winddir = 0; // [0-360 instantaneous wind direction]
//--------
float windspeedmph = 0; // [mph instantaneous wind speed]
float windgustmph = 0; // [mph current wind gust, using software specific time period]
int windgustdir = 0; // [0-360 using software specific time period]
float windspdmph_avg2m = 0; // [mph 2 minute average wind speed mph]
int winddir_avg2m = 0; // [0-360 2 minute average wind direction]
//float windgustmph_10m = 0; // [mph past 10 minutes wind gust mph ]
//int windgustdir_10m = 0; // [0-360 past 10 minutes wind gust direction]
float humidity = 0; // [%]
float tempf = 0; // [temperature F]
float temp_c = 0;// [temperature C]
float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
volatile float dailyrainin = 0; // [rain inches so far today in local time]
//float baromin = 30.03;// [barom in] - It's hard to calculate baromin locally, do this in the agent
float pressure = 0;
//float dewptf; // [dewpoint F] - It's hard to calculate dewpoint locally, do this in the agent

float batt_lvl = 5; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain;


uint8_t node_id = 2;   //This node id
uint8_t gw_id = 1;    //gatewayId
uint8_t netword_id = 300; //Gateway
//uint8_t t_wait = 1;  //Wait T_WAIT*8 [8 because you sleep 8s] just for simple nodes, not for gateway

String pck = "";//Packet to send
String msg = "";//Received packets
unsigned long prev_time = 0;
unsigned long time_to_send = 1000;//1000*Ns

//For speed and windir 1 minute average
unsigned long cero_time = 0;
unsigned long time_to_read = 1000;//1000*Ns
unsigned int minute = 0;

/*My interrupts modif*/
unsigned int count = 0;


void setup() {
  #if defined(DEBUG)
    digitalWrite(9, HIGH);
  #endif
  pinMode(STAT1, OUTPUT); //Status LED Blue
  pinMode(STAT2, OUTPUT); //Status LED Green
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor
  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);
  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 
  
  //Configure the humidity sensor
  myHumidity.begin();

  seconds = 0;
  lastSecond = millis();
  time_to_send *= 60;//Upload time data of Weather Station

  // attach external interrupt pins to IRQ functions
  //attachInterrupt(0, rainIRQ, FALLING);
  attachInterrupt(1, wspeedIRQ, FALLING);

  digitalWrite(RAIN, HIGH);  
  PCintPort::attachInterrupt(RAIN, rainISR, FALLING);//button_ISR
  //rainISR
  // turn on interrupts
  interrupts();
  
  
  mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, netword_id);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  Serial.begin(SERIAL_BAUD);
  #if defined(DEBUG)
    Serial.println(F("This is your gateway")); 
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
  #endif
}//end setup

void loop(){
  #if defined(DEBUG)
    Serial.println(F("node up"));
  #endif
    //Serial.println(count);
  if(millis() - lastSecond >= 1000){  
    digitalWrite(STAT1, HIGH); //Blink stat LED
    lastSecond += 1000;
    
    //Calc the wind speed and direction every second for 120 second to get 2 minute average
    int currentDirection = get_wind_direction();
    float currentSpeed = get_wind_speed();
    unsigned long now_time = millis();
    if(now_time - cero_time > time_to_read) {
      
      windspdavg[minute] = (int)currentSpeed;
      winddiravg[minute] = currentDirection;
      // save the last time
      cero_time = now_time;
      minute++;
      if(minute > 60){
        minute =0;
      }
    }//if /speed reads
    //float currentSpeed = random(5); //For testing


/*    //Check to see if this is a gust for the minute
    if(currentSpeed > windgust_10m[minutes_10m])
    {
      windgust_10m[minutes_10m] = currentSpeed;
      windgustdirection_10m[minutes_10m] = currentDirection;
    }
*/
    //Check to see if this is a gust for the day
    if(currentSpeed > windgustmph)
    {
      windgustmph = currentSpeed;
      windgustdir = currentDirection;
    }
    if(++seconds > 59)
    {
      seconds = 0;

      if(++minutes > 59) minutes = 0;
      //if(++minutes_10m > 9) minutes_10m = 0;

      rainHour[minutes] = 0; //Zero out this minute's rainfall amount
      //windgust_10m[minutes_10m] = 0; //Zero out this minute's gust
    } 
  }
  
  
  
  
  //if(n_times >= T_WAIT){
   //-----------This Station--------------
  calcWeather(); //Go calc all the various sensors

  //temp_c = (tempf-32)*0.555556;
  pck = "";//Clean packet
  pck += node_id;//This node
  pck += " ";
  pck += winddir_avg2m; /*winddir;*/ pck += " ";
  pck += windspdmph_avg2m;/*windspeedmph;*/ pck += " ";
  pck += humidity; pck += " ";
  pck += tempf; pck += " ";
  pck += rainin; pck += " ";
  pck += dailyrainin; pck += " ";
  pck += pressure; pck += " ";
  pck += batt_lvl; pck += " ";
  pck += light_lvl;
  
  //-------------------------------------
    
  //---------All nodes---------------
  mio->moteino_receive(msg);
  #if defined(DEBUG1)
    if(msg != ""){
      Serial.println(F("Paquete de un nodo"));
      Serial.println(msg);
    }
  #endif
    // Not need for now just receive from nodes and send it to gateway
    /*if(msg != ""){//Check if msg is empty
      #if defined(DEBUG)
        Serial.print(F("Node ID = "));
      #endif
      Serial.print((unsigned int)mio->moteino_id_receive());//Send Id to RPI/Emoncms
      #if defined(DEBUG)
        Serial.print(F("Packet received from this node = "));
      #endif      
      Serial.print(' ');
      //while(msg[i] != '\0'){
      for(uint8_t i = 0; i < msg.length(); i++){
        if((msg[i] == ';')){
          msg[i] = ' ';
        }//end if
      }//end for
      Serial.print(msg);//Print sensor values
      Serial.println();//end structure transmition to Emoncms
    }//end if  */
    
  //Time!!!
  unsigned long current_time = millis();
  if(current_time - prev_time > time_to_send) {
  // save the last time
  prev_time = current_time;
    if(pck != ""){//Check empty pck
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
  }//end if   

  if(msg != ""){//Check empty msg
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

  //---Send wheater sensors and pilars sensor to gateway------

    
  //msg = "";//Start again
    //---------------------------------------
    //n_times = 0;//Back to start
  //}else{
   // n_times++;//wait more
  //}  
  Serial.flush();
  //mio->mote_sleep();
  //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}//end loop


//Interrupt without debounce
void button_ISR() {
     count++;
  //last_interrupt_time=millis();
  // Serial.println ("Awake");
}//end button_ISR

//Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void rainISR()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
  raintime = millis(); // grab current time
  raininterval = raintime - rainlast; // calculate interval between this and last event
  
  if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
  {
    dailyrainin += 0.011; //Each dump is 0.011" of water
    rainHour[minutes] += 0.011; //Increase this minute's amount of rain
  
    rainlast = raintime; // set up for next event
  }//end if
}//end rainISR

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis(); //Grab the current time
    windClicks++; //There is 1.492MPH for each click per second.
  }
}

//Calculates each of the variables that wunderground is expecting
void calcWeather()
{
  //Calc winddir
  winddir = get_wind_direction();

  //Calc windspeed
  windspeedmph = get_wind_speed();

  //Calc humidity
  humidity = myHumidity.readHumidity();
  //float temp_h = myHumidity.readTemperature();
  //Serial.print(" TempH:");
  //Serial.print(temp_h, 2);

  //Calc tempf from pressure sensor
  tempf = myPressure.readTemp();
//  myPressure.readTempF();
  //Serial.print(" TempP:");
  //Serial.print(tempf, 2);

  pressure = myPressure.readPressure();

  //Calc light level
  light_lvl = get_light_level();

  //Calc battery level
  batt_lvl = get_battery_level();  
  
  //Calc windspdmph_avg2m
  float temp = 0;
  for(int i = 0 ; i < 60; i++)
    temp += windspdavg[i];
  temp /= 160.0;
  windspdmph_avg2m = temp;

  //Calc winddir_avg2m
  temp = 0; //Can't use winddir_avg2m because it's an int
  for(int i = 0 ; i < 60 ; i++)
    temp += winddiravg[i];
  temp /= 60;
  winddir_avg2m = temp;  
  
  
  //Total rainfall for the day is calculated within the interrupt
  //Calculate amount of rainfall for the last 60 minutes
  rainin = 0;  
  for(int i = 0 ; i < 60 ; i++){
    rainin += rainHour[i];
  }//end for
}//end calcWeather


//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);
  //Serial.print("Light sensor = ");
  //Serial.println(lightSensor);
  
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  
  lightSensor = operatingVoltage * lightSensor;
  
  return(lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);
  
  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V
  
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  
  return(rawVoltage);
}

//Returns the instataneous wind speed
float get_wind_speed()
{
  float deltaTime = millis() - lastWindCheck; //750ms

  deltaTime /= 1000.0; //Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();

  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

   //Serial.println();
   //Serial.print("Windspeed:");
   //Serial.println(windSpeed);

  return(windSpeed);
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction() 
{
  unsigned int adc;

  adc = analogRead(WDIR); // get the current reading from the sensor

  if (adc < 380) return (113);
  if (adc < 393) return (68);
  if (adc < 414) return (90);
  if (adc < 456) return (158);
  if (adc < 508) return (135);
  if (adc < 551) return (203);
  if (adc < 615) return (180);
  if (adc < 680) return (23);
  if (adc < 746) return (45);
  if (adc < 801) return (248);
  if (adc < 833) return (225);
  if (adc < 878) return (338);
  if (adc < 913) return (0);
  if (adc < 940) return (293);
  if (adc < 967) return (315);
  if (adc < 990) return (270);
  return (0); // error, disconnected?
}


