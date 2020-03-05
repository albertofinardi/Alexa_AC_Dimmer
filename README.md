# Alexa-AC-Dimmer
A project based on a NodeMCU (ESP8266) board to create a physically and Alexa controllable AC Dimmer Switch

### Part List
- NodeMCU (ESP8266) board;
- Arduino AC Dimmer;

### Connections

### Code
- #### Software
To code this project you need to install [Arduino IDE](https://www.arduino.cc/en/main/OldSoftwareReleases "Arduino IDE"), adding the ESP8266 board library following these steps:
	1. Go to files and click on the preference in the Arduino IDE;
	2. Copy the below code in the Additional boards Manager:
		http://arduino.esp8266.com/stable/package_esp8266com_index.json;
	3. Click OK to close the preference Tab;
	4. Go to Tools and board, and then select board Manager;
	5. Navigate to esp8266 by esp8266 community and install the software for Arduino. **INSTALL 2.5.0 VERSION**.
- #### Libraries
In order to make this proget you need to install the following libraries:
	1. [RDBDimmer](https://github.com/RobotDynOfficial/RBDDimmer "RDBDimmer");
	2. [Sinric Pro](https://github.com/sinricpro/esp8266-esp32-sdk "Sinric Pro");
