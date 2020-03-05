# Alexa-AC-Dimmer

A project based on a NodeMCU (ESP8266) board to create a physically and Alexa controllable AC Dimmer Switch

## Part List
- NodeMCU (ESP8266) board;
- Arduino AC Dimmer;

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
- [Sinric Pro](https://github.com/sinricpro/esp8266-esp32-sdk "Sinric Pro");

## Make your account
Make an account on [Sinric Pro](https://sinric.pro "Sinric Pro") and install [Sinric Pro Skill](https://www.amazon.com/HOME-Sinric-Pro/dp/B07ZT5VDT8 "Sinric Pro Skill") on your Alexa . Once you've done that, login in your account and, from the website, add a new "Light with dimmer" device. 
