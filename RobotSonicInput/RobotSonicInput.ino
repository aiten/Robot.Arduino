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
#include "DistanceSensor.h"

String eepromStringBuffer[EConfigEEpromIdx::SizeIdx];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;
String SendTo;

EepromConfig eepromConfig(EConfigEEpromIdx::SizeIdx, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("RobotSonicInput", eepromConfig, server, STATUS_LED_PIN);

StatusLed statusLed(STATUS_LED_PIN,500);

HCSr04 sensor1(D1, D2);
HCSr04 sensor2(D5, D6);

void setup(void)
{
  Serial.begin(115200); // Initialising if(DEBUG)Serial Monitor

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
  setupDistanceSensor();
}

void loop(void)
{
  MqttClientloop();
  server.handleClient();
  loopDistanceSensor();
  statusLed.Loop();
}
