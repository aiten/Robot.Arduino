#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
// #include <ESP8266mDNS.h>

#include <EepromConfig.h>
#include <PicoMQTT.h>
#include <SetupWiFi.h>
#include <WiFiClient.h>

#if __has_include("config.h")
#include "config.h"
#endif

#define MQTT_USER "mqttDevice"
#define MQTT_PASSWORD "mqttDevice"

String eepromStringBuffer[6];

EepromConfig eepromConfig(2, 0, eepromStringBuffer);

ESP8266WebServer server(80);
SetupWiFi setupWiFi("MqttBroker", eepromConfig, server);

class MQTT : public PicoMQTT::Server
{
protected:
  PicoMQTT::ConnectReturnCode auth(const char *client_id, const char *username, const char *password) override
  {
    // only accept client IDs which are 3 chars or longer
    if (String(client_id).length() < 3)
    { // client_id is never NULL
      return PicoMQTT::CRC_IDENTIFIER_REJECTED;
    }

    // only accept connections if username and password are provided
    if (!username || !password)
    { // username and password can be NULL
      // no username or password supplied
      return PicoMQTT::CRC_NOT_AUTHORIZED;
    }

    // accept two user/password combinations
    if (((String(username) == MQTT_USER) && (String(password) == MQTT_PASSWORD)))
    {
      return PicoMQTT::CRC_ACCEPTED;
    }

    // reject all other credentials
    return PicoMQTT::CRC_BAD_USERNAME_OR_PASSWORD;
  }
} mqtt;

void setup()
{
  Serial.begin(115200); // Initialising if(DEBUG)Serial Monitor
  // Setup serial

  EEPROM.begin(512);

  setupWiFi.Setup();

  mqtt.begin();

  pinMode(LED_BUILTIN, OUTPUT);
}

long until = 0;
bool ledOnOff = false;

void loop()
{
  server.handleClient();
  // MDNS.update();
  mqtt.loop();

  if (millis() > until)
  {

    digitalWrite(LED_BUILTIN, ledOnOff);
    ledOnOff = !ledOnOff;
    until = millis() + 1000;
  }
}
