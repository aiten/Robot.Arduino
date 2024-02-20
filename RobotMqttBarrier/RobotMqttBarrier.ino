#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EepromConfig.h>
#include <SetupWiFi.h>
#include <WiFiClient.h>

// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
// https://github.com/plapointe6/EspMQTTClient

#include "Config.h"
#include "SetupPage.h"

String eepromStringBuffer[EConfigEEpromIdx::SizeIdx];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;

EepromConfig eepromConfig(EConfigEEpromIdx::SizeIdx, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("RobotBarrier", eepromConfig, server);

#include "Ampel.h"

Ampel ampel[] = {
    Ampel(D1, D2, D3, 0),
    Ampel(D4, D5, D6, 1),
};

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

  client.setMqttServer(MqttBroker.c_str(), MqttUser.c_str(), MqttPwd.c_str());
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output

  for (uint8_t i = 0; i < sizeof(ampel) / sizeof(Ampel); i++)
  {
    ampel[i].Init();
  }
  ampel[0].SetPhase(Ampel::Green);
}

void loop(void)
{
  MqttClientloop();
  server.handleClient();
  MDNS.update();

  for (uint8_t i = 0; i < sizeof(ampel) / sizeof(Ampel); i++)
  {
    ampel[i].Poll();
    /*
        if (ampel[i].IsRed() && ampel[i].PhaseSinceTime() > 20000)
        {
          Serial.println("ToGreen");
          ampel[i].ToGreen();
        }
        else if (ampel[i].IsGreen() && ampel[i].PhaseSinceTime() > 30000)
        {
          Serial.println("ToRead");
          ampel[i].ToRed();
        }
        */
  }
}
