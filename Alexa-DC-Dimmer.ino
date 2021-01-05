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

 /*   |**************************************IMPORTANT******************************************|
  *   |                                                                                         |               
  *   |                             LIBRARY NODEMCU VERSION 2.5.0                               |
  *   |                                                                                         |
  *   |*****************************************************************************************|
  * 
  */


//----------------------------------------------------------CHANGABLE-------------------------------------------------------------------------------------------
#define outputPin         14      //D5 on NodeMCU
#define zerocross         12      //D6 on NodeMCU  // for boards with CHANGEBLE input pins
#define buttonPin         5       //D1 on NodeMCU

const int maxBrightOffline = 60;       // Max % brightness if offline
const int maxBright = 60;               // Max % brightness
const int threshold = 15;               // Min % brightness 
const int brightOn = 1;
unsigned long debounceDelay = 2000;     // To prevent double signal


#define WIFI_SSID         "xxxxxxxxx"    
#define WIFI_PASS         "xxxxxxxxx"
#define APP_KEY           "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"                                        // Get an APP KEY on Sinric.com
#define APP_SECRET        "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"   // Get an APP SECRET on Sinric.com
#define DIMSWITCH_ID      "XXXXXXXXXXxxxxxxxxxxxxxx"                                                    // Get a DEVICE ID on Sinric.com
#define BAUD_RATE         9600                                                                          // Change baudrate to your need
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

      
int buttonState = 0;
int lastButtonState = 0;
bool universalState = true;
bool connectionLost = true;
dimmerLamp dimmer(outputPin, zerocross); 

struct {
  bool powerState = false;
  int powerLevel = 0;
} device_state;

bool onPowerState(const String &deviceId, bool &state) {
  device_state.powerState = state;
  universalState = bool(state);
  int val = int(map(device_state.powerLevel,0,100,threshold, maxBright));
  //dimmer.setPower(state?val:threshold);
  dimmer.setPower(state?val:0);
  
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  return true; // request handled properly
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  device_state.powerLevel = powerLevel;
  //device_state.powerState = true;
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
  int val = int(map(device_state.powerLevel,0,100,threshold, maxBright));
  
  dimmer.setPower(val);
  Serial.printf("Device %s power level changed to %d\r\n", deviceId.c_str(), device_state.powerLevel);
  return true;
}


bool onAdjustPowerLevel(const String &deviceId, int &levelDelta) {
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];  
  int value = device_state.powerLevel + levelDelta;
  if(value < 100 && value > 0){
    device_state.powerLevel += levelDelta;
    int val = int(map(value,0,100,threshold, maxBright));
    dimmer.setPower(val);
    Serial.printf("Device %s power level changed about %i to %d\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
    levelDelta = device_state.powerLevel;
    return true;
  }else{
    if(value >= 100){
      device_state.powerLevel = 100;
      dimmer.setPower(maxBright);
      Serial.printf("Device %s power level changed about %i to %d, raising max\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
      levelDelta = 100;
      return true;
    }
    else if (value <= 0){
      device_state.powerLevel= brightOn;
      dimmer.setPower(0);
      Serial.printf("Device %s power level changed about %i to %d, turning off\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
      levelDelta = brightOn;
      return true;
    }
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setAutoConnect (true);
  WiFi.setAutoReconnect (true);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);
  SinricPro.begin(APP_KEY, APP_SECRET); 
  SinricPro.onConnected([](){ 
    Serial.printf("Connected to SinricPro\r\n"); 
    connectionLost = false;
    SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
    myDimSwitch.sendPowerStateEvent(universalState);
    Serial.println("Data updated");
  }); 
  SinricPro.onDisconnected([](){ 
    connectionLost = true;
    Serial.printf("Disconnected from SinricPro\r\n"); 
  });
}

void syncFunc(){
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
  int value = (buttonState ? 100 : 0);
  int val = int(map(value,0,100,threshold, maxBright));
  myDimSwitch.sendPowerLevelEvent(maxBright);
  myDimSwitch.sendPowerStateEvent(buttonState);
  dimmer.setPower(val);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  buttonState = digitalRead(buttonPin);
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  dimmer.begin(NORMAL_MODE, ON);
  dimmer.setPower(0);
  
  setupWiFi();
  setupSinricPro();
  syncFunc();
}

void loop() {
  SinricPro.handle();
  buttonState = digitalRead(buttonPin);
  button();
}

unsigned long lastDebounceTime = 0;
bool updated = true;

void button()
{
 uploadData();
 if(buttonState != lastButtonState )
 {
  if(!connectionLost){
    universalState = !universalState;
    Serial.printf("Device power turned %s manually\r\n", universalState?"on":"off");
    int val = int(map(device_state.powerLevel,0,100,threshold, maxBright));
    //dimmer.setPower(universalState?val:threshold);
    dimmer.setPower(universalState?val:0);
    lastDebounceTime = millis();
    updated = false;
  }else{
    universalState = !universalState;
    Serial.printf("Device power turned %s manually (no connection)\r\n", universalState?"on":"off");
    int val = int(map(maxBrightOffline,0,100,threshold, maxBright));
    //dimmer.setPower(universalState? val : threshold);
    dimmer.setPower(universalState? val : 0);
    WiFi.reconnect();
  }
 }
    lastButtonState = buttonState;  
}

void uploadData(){
  if (((millis() - lastDebounceTime) > debounceDelay) && !updated && !connectionLost) {
      SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];
      myDimSwitch.sendPowerStateEvent(universalState);
      Serial.printf("Device power updated\r\n");
      updated = true;
    }
}
