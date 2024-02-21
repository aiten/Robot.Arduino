#include <ArduinoJson.h>

#include "config.h"

EspMQTTClient client;

void PublishDiscovery()
{
  JsonDocument doc;

  doc["ip"] = WiFi.localIP().toString();
  doc["upTime"] = millis() / 1000;
  doc["deviceName"] = DeviceName;
  doc["mqttBroker"] = MqttBroker;
  doc["mqttUser"] = MqttUser;
  doc["sendTo"] = SendTo;

  String output;
  serializeJson(doc, output);

  client.publish(MQTT_DISCOVERY + "/config", output.c_str());
}

void PublishPing()
{
  JsonDocument doc;

  doc["ip"] = WiFi.localIP().toString();
  doc["deviceName"] = DeviceName;

  String output;
  serializeJson(doc, output);

  client.publish(MQTT_SENDTO_CMND + "/ping", output.c_str());
}

void PublishGo(uint direction, uint speed, uint duration)
{
  JsonDocument doc;

  doc["direction"] = direction;
  doc["speed"] = speed;
  doc["duration"] = duration;

  String output;
  serializeJson(doc, output);

  client.publish(MQTT_SENDTO_CMND + "/go", output.c_str());
}

void onConnectionEstablished()
{
  client.subscribe(MQTT_CMND + "/reset", [](const String &) {
    // setupWiFi.LeaveNetwork();
    Serial.println("Reset WiFi connection: please reboot");
  });

  client.subscribe(MQTT_CMND + "/set", [](const String &payload) {
    /*
        uint8_t idx;
        bool toRed;
        bool toGreen;
        uint32_t delay;

        if (SetAmpel("set", payload, idx, toRed, toGreen,delay))
        {
          if (toRed) ampel[idx].ToRed(delay);
          else if (toGreen) ampel[idx].ToGreen(delay);
        }
    */
  });

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
    PublishPing();
  }
}
