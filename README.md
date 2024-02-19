Robot.Arduino

Robot project using Mqtt

What you need:

* A WLAN  
  I use a simple WiFi Router for this project.
* A Mqtt-broker.  
  You can use a minimal broker running on a esp8266 - see Sketch RobotMqttBroker
* Devices, like a esp8266 LOLIN(WEMOS) D1
* To send Mqtt command to the devices you can use  
  * "MQTT-Explorer-0.4.0-beta1.exe" or
  * see https://github.com/aiten/Robot
  

To run (and compile) all the sketches, please:

* Set Sketchbook location (Arduino Menu: File:Preferences...) to this folder
* Install additional libraries:
  * ArduinoJson (by Benoit Blanchon)
  * EspMQTTClient (by Patrick Lapointe)
  * PicoMQTT (by Michal Lesniewski)
  * PubSubClient (by Nick O'Leary) - included by EspMQTTClient
  