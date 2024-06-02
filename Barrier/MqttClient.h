#pragma once

#include "Ampel.h"
#include "Config.h"

#include <ArduinoJson.h>

class MqttClient
{
private:
  PicoMQTT::Client &_client;
  class Ampel *_ampel;

public:
  MqttClient(PicoMQTT::Client &client, class Ampel *ampel) : _client(client), _ampel(ampel)
  {
  }

  PicoMQTT::Client &Client()
  {
    return _client;
  }

  void PublishDiscovery()
  {
    JsonDocument doc;

    doc["ip"] = WiFi.localIP().toString();
    doc["upTime"] = millis() / 1000;
    doc["deviceName"] = DeviceName;
    doc["mqttBroker"] = MqttBroker;
    doc["mqttUser"] = MqttUser;

    String output;
    serializeJson(doc, output);

    _client.publish(MQTT_DISCOVERY + "/config", output.c_str());
  }

  bool SetAmpel(const char *msg, const String &payload, uint8_t &idx, bool &toRed, bool &toGreen, uint32_t &delayTime)
  {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }

    idx = doc["idx"] | 0;
    toRed = doc["toRed"] | false;
    toGreen = doc["toGreen"] | false;
    delayTime = doc["delay"] | false;

    return true;
  }

  void onConnectionEstablished()
  {
    _client.subscribe(MQTT_CMND + "/reset", [](const String &) {
      // setupWiFi.LeaveNetwork();
      Serial.println("Reset WiFi connection: please reboot");
    });

    _client.subscribe(MQTT_CMND + "/set", [&](const String &payload) {
      uint8_t idx;
      bool toRed;
      bool toGreen;
      uint32_t delay;

      if (SetAmpel("set", payload, idx, toRed, toGreen, delay))
      {
        if (toRed)
          _ampel[idx].ToRed(delay);
        else if (toGreen)
          _ampel[idx].ToGreen(delay);
      }
    });

    PublishDiscovery();
  }

  void MqttClientloop(void)
  {
    static uint32_t until = 0;

    _client.loop();

    if (until <= millis())
    {
      until = millis() + 60000;
      Serial.println("wait ...");
      PublishDiscovery();
    }
  }
};
