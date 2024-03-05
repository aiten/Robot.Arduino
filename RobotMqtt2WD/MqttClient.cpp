#include <ArduinoJson.h>

#include "config.h"

uint32_t defaultSpeed = 255;
uint32_t defaultDuration = 250;

EspMQTTClient client;

void PublishDiscovery()
{
  JsonDocument doc;

  doc["ip"] = WiFi.localIP().toString();
  doc["upTime"] = millis()/1000;
  doc["deviceName"] = DeviceName;
  doc["mqttBroker"] = MqttBroker;
  doc["mqttUser"] = MqttUser;

  String output;
  serializeJson(doc, output);

  client.publish(MQTT_DISCOVERY + "/config", output.c_str());
}

bool GoOrDrive(const char *msg, const String &payload, uint16_t &direction, uint32_t &duration, uint8_t &speed)
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

  char buffer[128];
  snprintf(buffer, sizeof(buffer), "ROBOT: %s: direction=%u, speed=%u, duration=%u", msg, (uint)direction, speed, duration);

  return true;
}

void onConnectionEstablished()
{
  client.subscribe(MQTT_CMND + "/reset",
                   [](const String &)
                   {
                     //setupWiFi.LeaveNetwork();
                     Serial.println("Reset WiFi connection: please reboot");
                   });

  client.subscribe(MQTT_CMND + "/drive",
                   [](const String &payload)
                   {
                     uint16_t direction;
                     uint32_t duration;
                     uint8_t speed;

                     if (GoOrDrive("drive", payload, direction, duration, speed))
                     {
                       drive.Queue(direction, duration, speed);
                     }
                   });

  client.subscribe(MQTT_CMND + "/go",
                   [](const String &payload)
                   {
                     uint16_t direction;
                     uint32_t duration;
                     uint8_t speed;

                     if (GoOrDrive("drive", payload, direction, duration, speed))
                     {
                       drive.Go(direction, duration, speed);
                     }
                   });

  client.subscribe(MQTT_CMND + "/stop", [](const String &payload) { drive.Go(0, 0, 0); });

  PublishDiscovery();
}

void MqttClientloop(void)
{
  static uint32_t until = 0;
  
  client.loop();

  if (until <= millis())
  {
    until = millis() + 60000;
    Serial.println("wait ...");
    PublishDiscovery();
  }
}
