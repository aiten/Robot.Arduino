#pragma once

#include <Wire.h>

// see: https://microcontrollerslab.com/mpu-6050-esp8266-nodemcu-oled/
// GPIO5 (I2C SCL) => D1
// GPIO4 (I2C SDA) => D2

#include "MqttClient.h"
#include "PushButton.h"

class DistanceSensor
{
public:
  DistanceSensor(MqttClient &mqttClient, StatusLed &statusLed) : _mqttClient(mqttClient), _statusLed(statusLed), sensor1(SENSOR1_TRGPIN, SENSOR1_ECHOPIN), sensor2(SENSOR2_TRGPIN, SENSOR2_ECHOPIN)
  {
  }

private:
  MqttClient &_mqttClient;
  StatusLed &_statusLed;

  HCSr04 sensor1;
  HCSr04 sensor2;

  const long SEND_INTERVAL = 150;
  const long SEND_INTERVAL_ADD = 150; // add time to go
  const int MIN_SPEED = 60;

  void LimitPublishGo(uint direction, uint speed, uint duration)
  {
    static bool _speed0Sent = false;
    if (speed < MIN_SPEED)
    {
      speed = 0;
    }

    if (speed != 0 || _speed0Sent == false)
    {
      _mqttClient.PublishGo(direction, speed, duration);
      _speed0Sent = speed == 0;
    }
  }

public:
  void setupDistanceSensor()
  {
  }

  void loopDistanceSensor()
  {
    static long until = 0;
    static bool isOn = PUSHBUTTON_ISON;
    static CPushButton pushButton(PUSHBUTTON_PIN, LOW);

    if (pushButton.IsOn())
    {
      // toggle state
      isOn = !isOn;
      // statusLed.SetOnOffTime(isOn ? 1000: 100);
      _statusLed.SetLed(isOn, 100000000);
    }

    if (millis() < until || !isOn)
    {
      return;
    }

    until = millis() + SEND_INTERVAL;

    uint mm1, mm2;
    // convert the time into a distance
    mm1 = sensor1.ReadMM();
    mm2 = sensor2.ReadMM();

    mm1 = min(mm1, (uint)400);
    mm2 = min(mm2, (uint)400);

    /*
      Serial.print(mm1);
      Serial.print("mm, ");
      Serial.print(mm2);
      Serial.print("mm, ");
      Serial.println();
    */

    // auto direction = (uint)map((int) mm1, 400,20,0,360);
    auto direction = (uint)(map((int)mm1, 400, 20, 0, 360) + 180) % 360;
    auto speed = (uint)map((int)mm2, 400, 20, 0, 255);

    LimitPublishGo(direction, speed, SEND_INTERVAL + SEND_INTERVAL_ADD);
  }
};