#include <RBDdimmer.h>
#include "SinricPro.h"
#include "SinricProDimSwitch.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>

//YOUR INFO
#define WIFI_SSID         "Vodafone-34119905"    
#define WIFI_PASS         "xdkbz5pjbm747w4"
#define APP_KEY           "5a1d7e94-c5eb-448e-bb24-ff191f171e66"//"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define APP_SECRET        "07cc83d4-89f2-4c90-a30d-a8171453ce75-d08a07a2-4246-4a11-9555-bfd92e5c5e04"//"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define DIMSWITCH_ID      "5e5d0bab13b0d2499e309c57"//"xxxxxxxxxxxxxxxxxxxxxxxx"
#define BAUD_RATE         9600

//PINS
const int outputPin =     12;
const int zerocross =     4;
const int buttonPin =     5;

//INTERNAL VARIABLES
int   buttonState =         0;
int   lastButtonState =     0;
bool  universalState =      true;
int   initialBright =       0;
bool  connectionLost =      false;

//Dimmer lamp
dimmerLamp dimmer(outputPin, zerocross); 

struct {
  bool powerState = false;
  int powerLevel = 0;
} device_state;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device power turned %s (WebRequest)\r\n", state?"on":"off");
  device_state.powerState = state;
  universalState = device_state.powerState;
  if(device_state.powerLevel != 0){
    dimmer.setPower(state?device_state.powerLevel:0);
  }else{
    if (state){ 
      device_state.powerLevel = 50; 
      Serial.println("Power to 50");
      }
    dimmer.setPower(state?50:0);
  }
  return true;
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  //int pre = device_state.powerLevel;
  device_state.powerLevel = powerLevel;
  Serial.printf("Device power level changed to %d (WebRequest)\r\n", device_state.powerLevel);
  dimmer.setPower(device_state.powerLevel);
  //dimmering(pre, device_state.powerLevel);
  return true;
}

bool onAdjustPowerLevel(const String &deviceId, int levelDelta) {
  if((device_state.powerLevel + levelDelta)<100){
    if((device_state.powerLevel + levelDelta)>1){
      device_state.powerLevel += levelDelta;
      dimmer.setPower(device_state.powerLevel);
    }else{
      device_state.powerLevel = 1;
      dimmer.setPower(device_state.powerLevel);
    }
  }else{
    device_state.powerLevel = 100;
    dimmer.setPower(device_state.powerLevel);
  }
  Serial.printf("Device power level changed about %i to %d (WebRequest)\r\n", levelDelta, device_state.powerLevel);
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
  myDimSwitch.sendPowerStateEvent(buttonState?true:false);
  dimmer.setPower(buttonState?100:0);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  
  pinMode(buttonPin, INPUT_PULLUP);
  dimmer.begin(NORMAL_MODE, ON);
  
  setupWiFi();
  setupSinricPro();
  syncFunc();
  
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
  }else{
    Serial.print("No connection");
    connectionLost = true;
  }
  SinricPro.handle();
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

/*
unsigned long previousTime =  0;
#define millis_timer          50
void dimmering(int preValue, int newValue)                                                                            //dimmering
{
  //async delay

  unsigned long currentTime = millis();
  
  if (preValue < newValue){
    while(preValue < newValue){
      if(newValue <= 100){
        if (currentTime - previousTime >= millis_timer) {
            preValue++;
            Serial.printf("\n\tCurrent dimmer %d", preValue);
            dimmer.setPower(preValue);
            previousTime = currentTime;
            yield();
        }
      }else{
        return;
      }
    }
  }
  else if (preValue > newValue){
    while(preValue > newValue){
      if(newValue >= 0){
        if (currentTime - previousTime >= millis_timer) {
            preValue--;
            Serial.printf("\n\tCurrent dimmer %d", preValue);
            dimmer.setPower(preValue);
            previousTime = currentTime;
            yield();
        }
      }else{
        return;
      }
    }
  }
}
*/
