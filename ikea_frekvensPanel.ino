#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <FrekvensPanel.h>
#include "FS.h"
#include "config.h"
#include "web.h"
#include "wifi.h"

// see readme.md for wiring instructions

#include "button.h"

extern struct ConfigSettingsStruct ConfigSettings;
extern struct ConfigPanel cfgPanel;

bool configOK=false;
String modeWiFi="STA";

int positionX=16;
int positionY=1;

String text ;
int sizeText =2;
int scrollText =1;
int xText=1;
int yText=1;
int light= 1020;
 
#define p_ena 14 
#define p_data 12
#define p_clock 2
#define p_latch 0

#define p_btn1 13 // A0 (Adafruit Feather M0) - RED button (black wire)
#define p_btn2 5 // A1 (Adafruit Feather M0) - YELLOW button (white wire)

FrekvensPanel panel(p_latch, p_clock, p_data);

char activeProgram = 0;
char activeBrightMode = 1;

// the red wire (COM) for the buttons is connected to GND, so the button signal is inverted
SimpleButton button1(1,p_btn1,1,20);
SimpleButton button2(2,p_btn2,1,20);

void setBrightMode(int brightMode)
{

  activeBrightMode = brightMode % 4;
  switch(activeBrightMode)
  {
    case 0: { pinMode(p_ena, OUTPUT); digitalWrite(p_ena, 1); } break; // off
    case 1: analogWrite(p_ena, 1020); break; // very dim
    case 2: analogWrite(p_ena, 600); break; // dim
    case 3: { pinMode(p_ena, OUTPUT); digitalWrite(p_ena, 0); } break; // on 100%
  }
}

void setProgram(int program)
{
  activeProgram = (program % 3);
}

void buttonHandler(int id, int state)
{
  if (state) // button pressed
  {
    switch(id)
    {
      case 1: setBrightMode(activeBrightMode+1); break;
      case 2: setProgram(activeProgram+1); break;
    }
  }  
}

void program0()
{
  //int s = sizeText.toInt();
  panel.fillScreen(0);
  if (cfgPanel.scrollText.toInt())
  {
    panel.setCursor(positionX,positionY);
  }else{
    positionX = cfgPanel.xText.toInt();
    positionY = cfgPanel.yText.toInt();
    panel.setCursor(positionX,positionY);
  }
  panel.setTextWrap(false);
  panel.setTextSize(cfgPanel.sizeText.toInt());
  analogWrite(p_ena, cfgPanel.light.toInt());
  panel.print(cfgPanel.text);

}

int p1_i = 0;
void program1()
{
 /* float pi23 = PI*2/3;
  float p = 0.02 * p1_i++;
  int x0 = 7.98 + 7.98 * cos(p);
  int y0 = 7.98 + 7.98 * sin(p);
  p += pi23;
  int x1 = 7.98 + 7.98 * cos(p);
  int y1 = 7.98 + 7.98 * sin(p);
  p += pi23;
  int x2 = 7.98 + 7.98 * cos(p);
  int y2 = 7.98 + 7.98 * sin(p);
  panel.clear();
  panel.drawLine(x0,y0,x1,y1,1);  
  panel.drawLine(x1,y1,x2,y2,1);  
  panel.drawLine(x2,y2,x0,y0,1);  */
}


void program2()
{
  
  /*int x = random(16);
  int y = random(16);
  panel.drawRect(x,y,random(16-x),random(16-y),random(6)==0);*/
}

void setup() 
{
  Serial.begin(9600);
  if (!SPIFFS.begin()) {
    return;
  }

  loadConfigPanel();
  
  if (!loadConfig()) {  
  } else {
    configOK=true;
   
  }

  
  
  if (configOK)
  {
    if (!setupSTAWifi())
    {
      setupWifiAP();
      modeWiFi="AP";
    }
       
  }else{
    setupWifiAP();
    modeWiFi="AP";
  }
  initWebServer();
  
  
  pinMode(p_btn1, INPUT_PULLUP); 
  pinMode(p_btn2, INPUT_PULLUP); 

  panel.clear();
  pinMode(p_ena, OUTPUT); 
  analogWrite(p_ena, 500); // full brightness
  button1.attachHandler(buttonHandler);
  button2.attachHandler(buttonHandler);
  panel.scan();

  
  noInterrupts();                                // Switch off interrupts whilst they are set up
  timer0_isr_init();                             // Initialise Timer0
  timer0_attachInterrupt(ISR);                   // Goto the ISR function below when an interrupt occurs
  timer0_write(ESP.getCycleCount() + 800000L); // Pre-load timer with a time value (1-second)
  interrupts();                                  // Switch interrupts back on

}

ICACHE_RAM_ATTR void ISR (void) {                                // Timer reached zero, now re-load it to repeat
  timer0_write(ESP.getCycleCount() + 3200000L); // Reset the timer, do this first for timing accuracy
  int count = text.length();
  int l;
  
  switch(sizeText)
  {
    case 1: l=8; positionY=4; break;
    case 2: l= 12; positionY=1;  break;
    default: l=12; positionY=1; break;
  }

  if (scrollText==1)
  {
    if (positionX < (((count *l)*-1))-16)
    {
     positionX=16;
    }else{
     positionX--;
    }     
  }else{
    positionX=0;      
  }
 
}

void loop() 
{

  webServerHandleClient();
  
  if (modeWiFi=="STA")
  {
    if(WiFi.status() != WL_CONNECTED) {
      // we've lost the connection, so we need to reconnect
      setupSTAWifi();
    }
  }


    switch(activeProgram)
    {
      case 0: program0(); break;
      case 1: program1(); break;
      case 2: program2(); break;
    }
    panel.scan();
    button1.scan();
    button2.scan();

    
    delay(10);
  
}
