#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EepromConfig.h>
#include <ArduinoOTA.h>
#include <SetupWiFi.h>
#include <WiFiClient.h>
#include <StatusLed.h>

// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
// https://github.com/plapointe6/EspMQTTClient

#include "Config.h"
#include "SetupPage.h"
#include "MPU6050.h"

String eepromStringBuffer[EConfigEEpromIdx::SizeIdx];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;
String SendTo;

EepromConfig eepromConfig(EConfigEEpromIdx::SizeIdx, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("RobotGyroInput", eepromConfig, server, STATUS_LED_PIN);

StatusLed statusLed(STATUS_LED_PIN,500);

Adafruit_MPU6050 mpu;

void setup(void)
{
#if !defined(DONOTUSESERIAL)    // use RX,TX on esp01
  Serial.begin(115200); // Initialising if(DEBUG)Serial Monitor
#endif

  EEPROM.begin(512);

  setupWiFi.Setup();

  auto configString = eepromConfig.ReadStrings();

  DeviceName = configString[EConfigEEpromIdx::DeviceNameIdx];
  MqttBroker = configString[EConfigEEpromIdx::MqttBrokerIdx];
  MqttUser = configString[EConfigEEpromIdx::MqttUserIdx];
  MqttPwd = configString[EConfigEEpromIdx::MqttPwdIdx];
  SendTo = configString[EConfigEEpromIdx::SendToIdx];

  ArduinoOTA.setHostname(DeviceName.c_str());
  client.enableOTA("Robot");  
  client.setMqttServer(MqttBroker.c_str(), MqttUser.c_str(), MqttPwd.c_str());
  client.setMqttClientName(DeviceName.c_str());
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  setupMPU6050();
}

void loop(void)
{
  MqttClientloop();
  server.handleClient();
  loopMPU6050();
  statusLed.Loop();
}
