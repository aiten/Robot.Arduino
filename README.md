Robot.Arduino

Robot project using Mqtt

What you need:

* A WLAN  
  I use a simple WiFi Router for this project.
* A Mqtt-broker.  
  You can use a minimal broker running on a esp8266 - see Sketch RobotMqttBroker
* Devices, like a esp8266 LOLIN(WEMOS) D1
* To send Mqtt command to the devices, you can use  
  * "MQTT-Explorer-0.4.0-beta1.exe" or
  * see https://github.com/aiten/Robot
  

To run (and compile) all the sketches, please:

* Set Sketchbook location (Arduino Menu: File:Preferences...) to this folder
* Install additional libraries:
  * ArduinoJson (by Benoit Blanchon)
  * EspMQTTClient (by Patrick Lapointe)
  * PicoMQTT (by Michal Lesniewski)
  * PubSubClient (by Nick O'Leary) - included by EspMQTTClient
  * HX711 (by Rob Tillaart) - use for InputForce

Install 8266 Arduino environment

* see e.g. https://arduino-esp8266.readthedocs.io/en/latest/installing.html#
* enter the following text in "Additional board manager URLs:" (Arduino Menu: File:Preferences...):  http://arduino.esp8266.com/stable/package_esp8266com_index.json
* Install ESP8266 Boards

Device configuration
* An AP is opened, if the device cannot connect to the specified WLAN.  
  The name of the AP is e.g. Robot4WD_xxxxxx (xxxxxx is a unique number)
* To configure the device, please goto page 192.168.4.1  
