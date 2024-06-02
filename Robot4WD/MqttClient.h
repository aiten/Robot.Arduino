#pragma once

#include "Config.h"
#include "Drive.h"

#include <ArduinoJson.h>

class MqttClient
{
private:
  PicoMQTT::Client &_client;
  Drive &_drive;

public:
  MqttClient(PicoMQTT::Client &client, Drive &drive) : _client(client), _drive(drive)
  {
  }

  uint32_t defaultSpeed = 255;
  uint32_t defaultDuration = 250;

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

  bool GoOrDrive(const char *msg, const String &payload, uint16_t &direction, uint32_t &duration, uint8_t &speed, uint &id, float &time)
  {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }

    direction = doc["direction"] | 0;
    duration = doc["duration"] | defaultDuration;
    speed = doc["speed"] | defaultSpeed;
    id = doc["id"] | 0;
    time = doc["time"] | 0.0;

    // char buffer[128];
    // snprintf(buffer, sizeof(buffer), "ROBOT: %s: direction=%u, speed=%u, duration=%u", msg, (uint)direction, speed, duration);

    return true;
  }

  void onConnectionEstablished()
  {
    _client.subscribe(MQTT_CMND + "/reset", [](const String &) {
      // setupWiFi.LeaveNetwork();
      Serial.println("Reset WiFi connection: please reboot");
    });

    _client.subscribe(MQTT_CMND + "/drive", [&](const String &payload) {
      uint16_t direction;
      uint32_t duration;
      uint8_t speed;
      uint id;
      float time;

      if (this->GoOrDrive("drive", payload, direction, duration, speed, id, time))
      {
        this->_drive.Queue(direction, duration, speed, id, time);
      }
    });

    _client.subscribe(MQTT_CMND + "/go", [&](const String &payload) {
      uint16_t direction;
      uint32_t duration;
      uint8_t speed;
      uint id;
      float time;

      if (this->GoOrDrive("drive", payload, direction, duration, speed, id, time))
      {
        this->_drive.Go(direction, duration, speed, id, time);
      }
    });

    _client.subscribe(MQTT_CMND + "/stop", [&](const String &payload) { this->_drive.Go(0, 0, 0, 0, 0.0); });

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