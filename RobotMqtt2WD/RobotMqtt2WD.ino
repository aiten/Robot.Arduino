#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <EepromConfig.h>
#include <Motor.h>
#include <SetupWiFi.h>
#include <WiFiClient.h>

// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
// https://github.com/plapointe6/EspMQTTClient

#include "Config.h"
#include "Drive.h"
#include "SetupPage.h"

String eepromStringBuffer[6];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;

EepromConfig eepromConfig(6, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("Robot2WD", eepromConfig, server);

Drive drive;

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

  client.setMqttServer(MqttBroker.c_str(), MqttUser.c_str(), MqttPwd.c_str());
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
}

uint32_t until = 0;

void loop(void)
{
  client.loop();
  server.handleClient();
  drive.Poll();
  MDNS.update();

  if (until <= millis())
  {
    until = millis() + 60000;
    Serial.println("wait ...");
  }
}
