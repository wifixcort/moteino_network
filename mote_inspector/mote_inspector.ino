/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//-----------------
#include <GenSens.h>
#include <RFM69.h>
#include <LowPower.h>   //https://github.com/rocketscream/Low-Power

#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   115200 

GenSens *mio;

uint8_t node_id = 1;   //This node id, default for OLED proyect
uint8_t gateway = 215; //Gateway

//String pck = "";//Packet to send
String msg = "";//Received packets

volatile uint8_t up_down_buttons = 0;//node up or down
volatile unsigned long deb_interval, last_time, last;
uint8_t temp_button = 0;

int node_rssi = 0;

int display_conf(uint8_t font_size, uint8_t x_cursor, uint8_t y_cursor);

void setup()   {                
  Serial.begin(SERIAL_BAUD);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // draw many lines
  /*testdrawline();
  display.display();
  delay(2000);
  display.clearDisplay();*/

  // Clear the buffer.
  display.clearDisplay();

  //mio  = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, gateway, true);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  char buff[30];
    sprintf(buff, "\nTransmitting at: %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  // text display tests
  display_conf(1, 0, 15);
  display.println(buff);
  display.display();
  delay(2000);
  display.clearDisplay();
  //display.setTextColor(BLACK, WHITE); // 'inverted' text
  //display.println(3.141592);
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.print("0x"); 
  //display.println(0xDEADBEEF, HEX);
  
  
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  
  
 InitialiseInterrupt();
 interrupts();
  // miniature bitmap display
  /*display.clearDisplay();
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();*/

  // invert the display
  /*display.invertDisplay(true);
  delay(1000); 
  display.invertDisplay(false);
  delay(1000); */

  // draw a bitmap icon and 'animate' movement
  //testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop() {
  if(temp_button != up_down_buttons){
    temp_button = up_down_buttons;
    delete(mio);
    node_id = up_down_buttons;
    mio = new GenSens(node_id, FREQUENCY, ENCRYPTKEY, gateway, true);//node#, freq, encryptKey, gateway, LowPower/HighPower(false/true)
  }
  mio->moteino_receive(msg);  
  if(msg != ""){
    node_rssi = mio->moteino_rssi();
    Serial.println(msg);
  }
    
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Contador de Moteino!!");  
  //delay(2000);
  //display.clearDisplay();
  display.setTextColor(WHITE);
  display.println("");
  //----node id---
  display.setTextColor(WHITE);
    display.setCursor(50,30);
  display.println(mio->id);
  //----node rssi---incomming pack
  display.setTextColor(WHITE);
  display.setCursor(0,50);
  display.print(F("RSSI = "));
  //-----
  display.setTextColor(WHITE);
  display.setCursor(50,50);
  display.println(node_rssi);
//  display_conf(1, 0, 0);
    display.display();
  //delay(2000);
  display.clearDisplay();
  
  /*testscrolltext();
  display.display();
  delay(2000);
  display.clearDisplay();  */
}//end loop

void InitialiseInterrupt(){
  cli();
  PCICR |= (1<<PCIE2);
  PCMSK2 |= (1<<PCINT21);
  PCMSK2 |= (1<<PCINT22);
  MCUCR = (1<<ISC01) | (1<<ISC00);
  sei();
}//end InitialiseInterrupt
 
ISR(PCINT2_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
            // will generate an interrupt: but this will always be the same interrupt routine
  last_time = millis();
  deb_interval = last_time-last;
  if(deb_interval > 10){
    if (digitalRead(5)==0){
      if(up_down_buttons < 255){//Bock nodes out of range
        up_down_buttons++;
      }//end if
    }//end if
    if(digitalRead(6)==0){
      if(up_down_buttons != 0){//Block negative nodes
        up_down_buttons--;
      }//end if
    }//end if
  }
  last = last_time;


}

int display_conf(uint8_t font_size, uint8_t x_cursor, uint8_t y_cursor){
  display.setTextSize(font_size);
  display.setTextColor(WHITE);
  display.setCursor(x_cursor,y_cursor);
}//end display_conf




/*void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
	icons[f][XPOS] = random(display.width());
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}*/


/*void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
}*/

/*void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
}*/

/*void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
}*/

/*void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
}*/

/*void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}*/

/*void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}*/

/*void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}*/
   
/*void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}*/

/*void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(250);
}*/

void testscrolltext(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(50,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
 
/*  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);*/    
  //display.startscrolldiagright(0x00, 0x07);
  //delay(2000);
  display.startscrolldiagleft(0x00, 0x00);
  delay(2000);
  display.stopscroll();
}
