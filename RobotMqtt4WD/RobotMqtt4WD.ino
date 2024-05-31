#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EepromConfig.h>
#include <SetupWiFi.h>
#include <StatusLed.h>
#include <WiFiClient.h>

// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
// https://github.com/plapointe6/EspMQTTClient

#include "Config.h"
#include "Drive.h"
#include "MqttClient.h"
#include "SetupPage.h"

String eepromStringBuffer[EConfigEEpromIdx::SizeIdx];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;

EepromConfig eepromConfig(EConfigEEpromIdx::SizeIdx, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("Robot4WD", eepromConfig, server, STATUS_LED_PIN);

StatusLed statusLed(STATUS_LED_PIN, 500);

PicoMQTT::Client espMQTTClient;
Drive drive(espMQTTClient, statusLed);

MqttClient mqttClient(espMQTTClient, drive);

void onConnectionEstablished()
{
  mqttClient.onConnectionEstablished();
}

void setup(void)
{
  Serial.begin(115200); // Initialising if(DEBUG)Serial Monitor

  EEPROM.begin(512);

  setupWiFi.Setup();

  drive.Setup();

  auto configString = eepromConfig.ReadStrings();

  DeviceName = configString[EConfigEEpromIdx::DeviceNameIdx];
  MqttBroker = configString[EConfigEEpromIdx::MqttBrokerIdx];
  MqttUser = configString[EConfigEEpromIdx::MqttUserIdx];
  MqttPwd = configString[EConfigEEpromIdx::MqttPwdIdx];

  ArduinoOTA.setHostname(DeviceName.c_str());
  espMQTTClient.host = MqttBroker.c_str();
  espMQTTClient.username = MqttUser.c_str();
  espMQTTClient.password = MqttPwd.c_str();
  espMQTTClient.client_id = DeviceName.c_str();
  onConnectionEstablished();
  espMQTTClient.begin();
//  espMQTTClient.setMaxPacketSize(512);
//  espMQTTClient.enableOTA("Robot");
//  espMQTTClient.setMqttServer(MqttBroker.c_str(), MqttUser.c_str(), MqttPwd.c_str());
//  espMQTTClient.setMqttClientName(DeviceName.c_str());
  //  espMQTTClient.enableDebuggingMessages(); // Enable debugging messages sent to serial output
}

void loop(void)
{
  mqttClient.MqttClientloop();
  server.handleClient();
  drive.Poll();
  statusLed.Loop();
}
