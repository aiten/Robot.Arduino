#include <ArduinoJson.h>

#include "config.h"

uint32_t defaultSpeed = 255;
uint32_t defaultDuration = 250;

EspMQTTClient client;

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

  client.subscribe(MQTT_CMND + "/speed",
                   [](const String &payload)
                   {
                     defaultSpeed = std::strtoul(payload.c_str(), NULL, 0);
                     char buffer[128];
                     snprintf(buffer, sizeof(buffer), "ROBOT: setspeed: speed=%u", defaultSpeed);
                     Serial.println(buffer);
                     client.publish(MQTT_STAT + "/speed", std::to_string(defaultSpeed).c_str());
                   });

  client.subscribe(MQTT_CMND + "/duration",
                   [](const String &payload)
                   {
                     defaultDuration = std::strtoul(payload.c_str(), NULL, 0);
                     char buffer[128];
                     snprintf(buffer, sizeof(buffer), "ROBOT: setduration: defaultDuration=%u", defaultDuration);
                     Serial.println(buffer);
                     client.publish(MQTT_STAT + "/duration", std::to_string(defaultDuration).c_str());
                   });

  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  // client.subscribe("robot/" ROBOTNAME "/wildcardtest/#", [](const String &topic, const String &payload) { Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload); });

  client.publish(MQTT_DISCOVERY + "/config", WiFi.localIP().toString().c_str());

  // Execute delayed instructions
  // client.executeDelayed(5 * 1000, []() {
  //  client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  //});
}
