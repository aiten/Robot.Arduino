#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EepromConfig.h>
#include <PushButton.h>
#include <SetupWiFi.h>
#include <StatusLed.h>
#include <WiFiClient.h>

// https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
// https://github.com/plapointe6/EspMQTTClient

#include "Config.h"
#include "MqttClient.h"
#include "SetupPage.h"

String eepromStringBuffer[EConfigEEpromIdx::SizeIdx];

String DeviceName;
String MqttBroker;
String MqttUser;
String MqttPwd;

EepromConfig eepromConfig(EConfigEEpromIdx::SizeIdx, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupPage setupWiFi("Barrier", eepromConfig, server, STATUS_LED_PIN);

StatusLed statusLed(STATUS_LED_PIN, 500);
CPushButton pushButton(D5, LOW);

PicoMQTT::Client espMQTTClient;

#include "Ampel.h"

extern MqttClient mqttClient;

Ampel ampel[] = {
    Ampel(mqttClient, D1, D2, D3, 0),
    Ampel(mqttClient, D6, D7, D8, 1),
};

MqttClient mqttClient(espMQTTClient, ampel);

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

  ArduinoOTA.setHostname(DeviceName.c_str());
  ArduinoOTA.setPassword("Robot");
  ArduinoOTA.begin();

  espMQTTClient.host = MqttBroker.c_str();
  espMQTTClient.username = MqttUser.c_str();
  espMQTTClient.password = MqttPwd.c_str();
  espMQTTClient.client_id = DeviceName.c_str();
  mqttClient.onConnectionEstablished();

  espMQTTClient.begin();
  // espMQTTClient.setMqttServer(MqttBroker.c_str(), MqttUser.c_str(), MqttPwd.c_str());
  // espMQTTClient.setMqttClientName(DeviceName.c_str());
  // espMQTTClient.enableDebuggingMessages(); // Enable debugging messages sent to serial output

  for (uint8_t i = 0; i < sizeof(ampel) / sizeof(Ampel); i++)
  {
    ampel[i].Init();
  }
  ampel[0].SetPhase(Ampel::Green);
}

void loop(void)
{
  mqttClient.MqttClientloop();
  server.handleClient();
  statusLed.Loop();
  ArduinoOTA.handle();

  for (uint8_t i = 0; i < sizeof(ampel) / sizeof(Ampel); i++)
  {
    ampel[i].Poll();

    /*
        if (ampel[i].IsRed() && ampel[i].PhaseSinceTime() > 20000)
        {
          Serial.println("ToGreen");
          ampel[i].ToGreen();
        }
        else
    */
    if (ampel[i].IsGreen() && ampel[i].PhaseSinceTime() > 15000)
    {
      Serial.println("ToReed");
      ampel[i].ToRed();
    }
  }

  if (pushButton.IsOn())
  {
    static uint32_t lasttime = 0;
    if (millis() - lasttime > 200)
    {
      // prevent: prellen
      lasttime = millis();
      static int idx = 0;
      ampel[idx].ToGreen(2000);
      idx = (idx + 1) % (sizeof(ampel) / sizeof(Ampel));
      Serial.println("Button pressed");
    }
  }
}
