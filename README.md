# Alexa AC Dimmer

[![Maintenance](https://img.shields.io/badge/Maintained%3F-no-red.svg)](https://bitbucket.org/lbesson/ansi-colors)

A project based on a NodeMCU (ESP8266) board to create a physically and with Alexa controllable AC Dimmer Switch

## Part List
- NodeMCU (ESP8266) board;
- [Arduino AC Dimmer](https://www.amazon.com/RobotDyn-controller-control-Arduino-Raspberry/dp/B072K9P7KH/ref=sr_1_2?keywords=arduino+ac+dimmer&qid=1583418112&sr=8-2 "Arduino AC Dimmer");
- Jumper wire;
- Switch;

## Wiring
- Connect Arduino AC Dimmer PWM to D5 and Z-C to D6;
- Connect one side of the switch to Ground and the other one to D1;

## Code
### Software
To code this project you need to install [Arduino IDE](https://www.arduino.cc/en/main/OldSoftwareReleases "Arduino IDE"), adding the ESP8266 board library following these steps:
- Go to files and click on the preference in the Arduino IDE;
- Copy the below code in the Additional boards Manager:
        http://arduino.esp8266.com/stable/package_esp8266com_index.json;
- Click OK to close the preference Tab;
- Go to Tools and board, and then select board Manager;
- Navigate to esp8266 by esp8266 community and install the software for Arduino. **INSTALL 2.5.0 VERSION**.
    
### Libraries
In order to make this project you need to install the following libraries:
- [RDBDimmer](https://github.com/RobotDynOfficial/RBDDimmer "RDBDimmer");
- [Sinric Pro](https://github.com/sinricpro/esp8266-esp32-sdk#arduinoide "Sinric Pro");

## Make your account
Make an account on [Sinric Pro](https://sinric.pro "Sinric Pro") and install [Sinric Pro Skill](https://www.amazon.com/HOME-Sinric-Pro/dp/B07ZT5VDT8 "Sinric Pro Skill") on your Alexa. Once you've done that, login in your Sinric account and, from the website, add a new "Light with dimmer" device. 

## Ready
You're ready to modify the code with your:
- WiFi SSID and password; 
- App Key and App Secret (you can find them in your account data on [Sinric Pro](https://sinric.pro "Sinric Pro"));
- Device ID (you can find it in your devices menu on [Sinric Pro](https://sinric.pro "Sinric Pro"));

and upload it on your board.
