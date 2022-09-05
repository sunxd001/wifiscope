/*
 * WiFi Osiolloscope program For ESP8266
 * Main File
 * 
 * Author: sunxd
*/

#include <time.h>
#include <Arduino.h>
#include "EEPROM.h"

#include "AC.h"

#ifdef UPDATE_IP
#include <ESP8266HTTPClient.h>
#endif

#include "Button2.h" //  https://github.com/LennartHennigs/Button2
#include "ESPRotary.h"
#define ROTARY_PIN1 D0
#define ROTARY_PIN2 D1
#define BUTTON_PIN  D2

#define CLICKS_PER_STEP 4   // this number depends on your rotary encoder
#define MIN_POS         1
#define MAX_POS         10
#define START_POS       0
#define INCREMENT       1   // this number is the counter increment on each step
ESPRotary r;
Button2 b;  
unsigned char rotaryStatus=0;
#define SCANSPEED 0
#define AMPLIFIER 1
#define TRIGERTYPE 2
#define TRIGERVALUE 3

#include <SPI.h>
#include <Adafruit_GFX.h>//图形库
#include <Adafruit_ST7735.h> // Hardware-specific library
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define TFT_CS     0  //connect CS pin to D8,hardware CS
#define TFT_RST    0 // connect this to the Arduino reset
#define TFT_DC     D3

Adafruit_ST7735 display(TFT_CS,  TFT_DC, TFT_RST);

unsigned char ucBuffer[1000];
uint16 tempBuffer[10000];
unsigned char oldBuffer[128];
unsigned int  ipTimer;
unsigned int  uiBuffer;

#ifdef UPDATE_IP
HTTPClient    http;    //Declare object of class HTTPClient
#endif

uint8 samplediv=1;


void setup()
{
  int i;

  ipTimer = 0;
  
  WiFi.mode(WIFI_OFF);
  DisableHotSpot = false;
  
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
#ifdef LOG_ENABLE
  Serial.println("ESP8266 WiFi AutoConnect");
#endif

  //0-161 is allocated for AutoConnect (162 bytes)
  //162-255 is free for the program (94 bytes)
  EEPROM.begin(256);
  
  delay(1000);
  
  init_AC();
  
  // Configuring Internal Pages
  if(secureLink[0] == 0)
  {
    Server.on("/", rootPage);
  }
  else
  {
    Server.on("/"+((String)secureLink)+"/", rootPage);
  }
  
  delay(1000);
  
  Server.begin();
#ifdef LOG_ENABLE
  Serial.println("HTTP server started");
#endif

  //Your Setup code
  uiBuffer = 0;

  b.setTapHandler(click);
  b.begin(BUTTON_PIN);//D2
  b.setLongClickHandler(LongPress);
  uint8 ch=1;  
  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, ch, INCREMENT);//D0,D1
  r.setChangedHandler(rotate);

  display.initR(INITR_144GREENTAB);
  display.fillScreen(ST7735_BLACK);
  
  
#ifdef LOG_ENABLE
  Serial.println("Setup Done!");
#endif

  delay(1000);
}

void Delay10Sec()
{
  unsigned int i, j;

  unsigned long mic;

  system_adc_read_fast(tempBuffer,  10000,  8);
  
  for(i = 0; i < LOOP_10_S; i++)
  {
    //Server.handleClient();
    
    r.loop();
    /*if(uiBuffer < 1000)
    {
      j = analogRead(A0) >> 2;

      if(j >= 256)
        j = 255;
      
      ucBuffer[uiBuffer] = j; //110us
      uiBuffer++;
    }*/
    if(uiBuffer < 1000)
    {
      ucBuffer[uiBuffer] = tempBuffer[uiBuffer/samplediv] >>2; //110us
      uiBuffer++;
    }
    else{
      Server.handleClient();
      b.loop();
      
    }   
  }
  b.loop();
  r.loop();
  draw();
}

void loop()
{
  Delay10Sec();

  handle_AC();
//没有定义UPDATE_IP，以下无用
#ifdef UPDATE_IP
  if(WiFi.status() == WL_CONNECTED)
  {
    if(ipTimer == 100)
    {
      http.begin("http://Your-Website");
      ipTimer = http.GET();
#ifdef LOG_ENABLE
      Serial.println("Updating IP");
      if(ipTimer>0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(ipTimer);
        Serial.println(http.getString());
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(ipTimer);
      }
#endif
      ipTimer = 0;
      http.end();  //Close connection
    }
    else
      ipTimer++;
  }
#endif
}

// on change
void rotate(ESPRotary& r) {
  samplediv = r.getPosition();
  Serial.println(samplediv);
  
}

// single click
void click(Button2& btn) {
  if(rotaryStatus==SCANSPEED)rotaryStatus=AMPLIFIER;
  else if(rotaryStatus==AMPLIFIER)rotaryStatus=TRIGERTYPE;
  else if(rotaryStatus==TRIGERTYPE)rotaryStatus=TRIGERVALUE;
  else if(rotaryStatus==TRIGERVALUE)rotaryStatus=SCANSPEED;
}

// long click
void LongPress(Button2& btn) { 
   Serial.println("LONG CLICK");
    
}

void draw(void) {
  
  ///////////////////////////////////////////
  display.drawRect(0,0,127,127,ST7735_YELLOW);
  
  display.drawLine(0,63,127,63,ST7735_YELLOW);
  display.drawLine(63,0,63,127,ST7735_YELLOW);
  uint16_t localcolor=display.Color565(50, 50, 50);
  for(int i=1;i<6;i++)
    for(int j=1;j<64;j++)
      display.drawPixel(i*25,j*2, localcolor);
  for(int i=1;i<6;i++)
    for(int j=1;j<64;j++)
      display.drawPixel(j*2,i*25, localcolor);

  for(int i=1;i<127;i++)
    display.drawLine(i-1,(254-oldBuffer[i-1])/2,i,(254-oldBuffer[i])/2,ST7735_BLACK);
  for(int i=1;i<127;i++)
    display.drawLine(i-1,(254-ucBuffer[i-1])/2,i,(254-ucBuffer[i])/2,ST7735_WHITE);
  for(int i=0;i<128;i++)
    oldBuffer[i]=ucBuffer[i];  

  if(rotaryStatus==SCANSPEED)
    display.fillRect(2, 5, 61, 10, ST7735_BLUE); 
  else     
    display.fillRect(2, 5, 61, 10, ST7735_BLACK);          
  display.setTextSize(1);
  display.setTextColor(ST7735_YELLOW);
  display.setCursor(2,5);
  display.print(samplediv*25);
  display.print("us/div");
  

  if(rotaryStatus==AMPLIFIER)
    display.fillRect(2, 15, 61, 10, ST7735_BLUE); 
  else     
    display.fillRect(2, 15, 61, 10, ST7735_BLACK);          
  display.setTextSize(1);
  display.setTextColor(ST7735_YELLOW);
  display.setCursor(2,15);
  display.print(samplediv*25);
  display.print("V/div");

  if(rotaryStatus==TRIGERTYPE)
    display.fillRect(2, 25, 61, 10, ST7735_BLUE); 
  else     
    display.fillRect(2, 25, 61, 10, ST7735_BLACK);          
  display.setTextSize(1);
  display.setTextColor(ST7735_YELLOW);
  display.setCursor(2,25);
  display.print(samplediv*25);
  display.print("UP");

  if(rotaryStatus==TRIGERVALUE)
    display.fillRect(2, 35, 61, 10, ST7735_BLUE); 
  else     
    display.fillRect(2, 35, 61, 10, ST7735_BLACK);          
  display.setTextSize(1);
  display.setTextColor(ST7735_YELLOW);
  display.setCursor(2,35);
  display.print(samplediv*25);
  display.print("V");
  
  
}
