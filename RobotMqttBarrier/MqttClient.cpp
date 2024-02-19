#include <ArduinoJson.h>
#include <arduino.h>
#include <cstdint>

#include "MqttClient.h"

EspMQTTClient client;

bool SetAmpel(const char *msg, const String &payload, uint8_t &idx, bool &toRed,
              bool &toGreen, uint32_t delayTime)
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
  client.subscribe(MQTT_CMND + "/reset", [](const String &) {
    // setupWiFi.LeaveNetwork();
    Serial.println("Reset WiFi connection: please reboot");
  });

  client.subscribe(MQTT_CMND + "/set", [](const String &payload) {
    uint8_t idx;
    bool toRed;
    bool toGreen;
    uint32_t delay;

    if (SetAmpel("set", payload, idx, toRed, toGreen,delay))
    {
      if (toRed) ampel[idx].ToRed(delay);
      else if (toGreen) ampel[idx].ToGreen(delay);
    }
  });

  client.publish(MQTT_DISCOVERY + "/config", WiFi.localIP().toString().c_str());
}
