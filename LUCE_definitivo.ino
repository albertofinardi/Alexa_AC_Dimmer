#include <RBDdimmer.h>

/*
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProDimSwitch.h"

#define outputPin         12    //D6
#define zerocross         4     //D2   // for boards with CHANGEBLE input pins

      
const int buttonPin = 5; //???
int buttonState = 0;
int lastButtonState = 0;
bool universalState = true;

int initialBright = 0;

bool connectionLost = false;

dimmerLamp dimmer(outputPin, zerocross); 

#define WIFI_SSID         ""    
#define WIFI_PASS         ""
#define APP_KEY           "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"                                        // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define DIMSWITCH_ID      "xxxxxxxxxxxxxxxxxxxxxxxx"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need

// we use a struct to store all states and values for our dimmable switch
struct {
  bool powerState = false;
  int powerLevel = 0;
} device_state;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  device_state.powerState = state;
  universalState = device_state.powerState;
  dimmer.setPower(state?device_state.powerLevel:0);
  return true; // request handled properly
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  //int prePw = device_state.powerLevel;
  device_state.powerLevel = powerLevel;
  Serial.printf("Device %s power level changed to %d\r\n", deviceId.c_str(), device_state.powerLevel);
  //dimmering(prePw, device_state.powerLevel);
  dimmer.setPower(device_state.powerLevel);
  return true;
}

bool onAdjustPowerLevel(const String &deviceId, int levelDelta) {
  
  int PrePw = 0;
  if((device_state.powerLevel + levelDelta)<100){
    if((device_state.powerLevel + levelDelta)>1){
      //PrePw = device_state.powerLevel;
      device_state.powerLevel += levelDelta;
      dimmer.setPower(device_state.powerLevel);
      //dimmering(PrePw, device_state.powerLevel);
    }else{
      //PrePw = device_state.powerLevel;
      device_state.powerLevel = 1;
      //dimmering(PrePw, device_state.powerLevel);
      dimmer.setPower(device_state.powerLevel);
    }
  }else{
    //PrePw = device_state.powerLevel;
    device_state.powerLevel = 100;
    //dimmering(PrePw, device_state.powerLevel);
    dimmer.setPower(device_state.powerLevel);
  }
  Serial.printf("Device %s power level changed about %i to %d\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
  return true;
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];

  // set callback function to device
  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void syncFunc(){
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
  myDimSwitch.sendPowerLevelEvent(buttonState?100:0);
  dimmer.setPower(buttonState?100:0);
}

// main setup function
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  lastButtonState = buttonState;
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  
  /*initialBright = potValue;*/
  dimmer.begin(NORMAL_MODE, ON);
  
  setupWiFi();
  setupSinricPro();
  syncFunc();
  //Serial.print(initialBright);
}

void loop() {
  
  buttonState = digitalRead(buttonPin);
  button();

  if(WiFi.status() == WL_CONNECTED){
    if(connectionLost){
      SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
      myDimSwitch.sendPowerStateEvent(universalState);
      Serial.println("Data updated");
      connectionLost = false;
    }
    SinricPro.handle();
  }else{
    Serial.print("No connection");
    connectionLost = true;
  }
  
}

void button()
{
 if(buttonState !=lastButtonState )
 {
  if(WiFi.status() == WL_CONNECTED){
    SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
    universalState = !universalState;
    myDimSwitch.sendPowerStateEvent(universalState);
    Serial.printf("Device power turned %s manually (updated)\r\n", universalState?"on":"off");
    dimmer.setPower(universalState?100:0);
  }else{
    Serial.printf("Device power turned %s manually (no connection)\r\n", universalState?"on":"off");
    dimmer.setPower(universalState?100:0);
  }
 }
    lastButtonState = buttonState;  
}

unsigned long previousTime =  0;
#define millis_timer          50

void dimmering(int preValue, int newValue)                                                                            //dimmering
{

  dimmer.setPower(newValue);
  
}
  /*
void dimmering(int preValue, int newValue)                                                                            //dimmering
{
  //async delay

  unsigned long currentTime = millis();
  
  if (preValue < newValue){
    do {
      if(newValue <= 100){
        if (currentTime - previousTime >= millis_timer) {
            preValue++;
            Serial.printf("\n\tCurrent dimmer %d", preValue);
            dimmer.setPower(preValue);
            previousTime = currentTime;
        }
      }else{
        return;
      }
    }while(preValue < newValue);
  }
  else if (preValue > newValue){
    do {
      if(newValue >= 0){
        if (currentTime - previousTime >= millis_timer) {
            preValue--;
            Serial.printf("\n\tCurrent dimmer %d", preValue);
            dimmer.setPower(preValue);
            previousTime = currentTime;
        }
      }else{
        return;
      }
    }while(preValue > newValue);
  }
}*/
