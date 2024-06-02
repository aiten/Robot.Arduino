#pragma once

#include "HX711.h"

#include "MqttClient.h"
#include "PushButton.h"

class ForceSensor
{
public:
  ForceSensor(MqttClient &mqttClient, StatusLed &statusLed) : _mqttClient(mqttClient), _statusLed(statusLed)
  {
  }

private:
  MqttClient &_mqttClient;
  StatusLed &_statusLed;

  const long SEND_INTERVAL = 100;
  const long SEND_INTERVAL_ADD = 250; // add time to go
  const int MIN_SPEED = 60;

  const uint8_t _dataPin[2] = {D5, D6};
  const uint8_t _clockPin = D7;

#define REMEMBERFORCECOUNT 4

  const float MINREVERSE = 20.0f;
  const float MAXREVERSE = 20.0f;
  const long REVERSETIME = SEND_INTERVAL * 3 + 10;

  HX711 _scales[2];

  const float _calib[2] = {1000.0, 900.0};

  long _lastForceTimeR = 0;
  long _lastForceTimeL = 0;

  float _forceDirectionL = 1.0f;
  float _forceDirectionR = 1.0f;

public:
  inline bool LimitPublishGo(uint direction, uint speed, uint duration)
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

    return speed > 0;
  }

  inline void setupForceSensor()
  {
    for (int i = 0; i < sizeof(_scales) / sizeof(_scales[0]); i++)
    {
      _scales[i].begin(_dataPin[i], _clockPin, true);
      _scales[i].set_scale(_calib[i]);
      // reset the scale to zero = 0
      _scales[i].tare();
    }
    /*
        pinMode(DIRL_ON_LED, OUTPUT);
        pinMode(DIRL_OFF_LED, OUTPUT);
        pinMode(DIRR_ON_LED, OUTPUT);
        pinMode(DIRR_OFF_LED, OUTPUT);
    */
  }

  inline void CheckReverse(float force, float &forceDirection, long &lastForceTime, bool left)
  {
    if (force < MINREVERSE)
    {
      if (lastForceTime != 0 && (millis() - lastForceTime) < REVERSETIME)
      {
        forceDirection *= -1.0f;
        /*
                Serial.print(forceDirection);
                Serial.println((left) ? "Left" : "Right");
        */
      }
      lastForceTime = 0;
    }
    else if (lastForceTime == 0)
    {
      lastForceTime = millis();
    }
  }

  inline void loopForceSensor()
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

    float valueL = _scales[0].get_units();
    float valueR = _scales[1].get_units();

    float forceR = max(min(valueR, 1000.0f), 0.0f);
    float forceL = max(min(valueL, 1000.0f), 0.0f);

    CheckReverse(forceR, _forceDirectionR, _lastForceTimeR, false);
    CheckReverse(forceL, _forceDirectionL, _lastForceTimeL, true);

    analogWrite(DIRL_ON_LED, _forceDirectionL > 0.0 ? 255 : map(forceL, 0, 1000, 0, 255));
    analogWrite(DIRL_OFF_LED, _forceDirectionL < 0.0 ? 255 : map(forceL, 0, 1000, 0, 255));
    analogWrite(DIRR_ON_LED, _forceDirectionR > 0.0 ? 255 : map(forceR, 0, 1000, 0, 255));
    analogWrite(DIRR_OFF_LED, _forceDirectionR < 0.0 ? 255 : map(forceR, 0, 1000, 0, 255));

    /*
            Serial.print(forceR);
            Serial.print("R, ");
            Serial.print(forceL);
            Serial.print("L, ");
            Serial.print(valueR);
            Serial.print("R, ");
            Serial.print(valueL);
            Serial.print("L, ");

            Serial.print(direction);
            Serial.print("Dir, ");

            Serial.println();
            return;
    */
    uint direction = 0;
    if (forceL >= forceR)
    {
      auto value = (forceR / forceL) * 45.0f;
      if (_forceDirectionL > 0.0)
      {
        if (_forceDirectionR > 0.0)
          direction = 45.0 - value;
        else
          direction = 45.0 + value;
      }
      else
      {
        if (_forceDirectionR > 0.0)
          direction = 225.0f + value;
        else
          direction = 135.0f + value;
      }
    }
    else
    {
      auto value = (forceL / forceR) * 45.0f;
      if (_forceDirectionL > 0.0)
      {
        if (_forceDirectionR > 0.0)
          direction = 305.0f + value;
        else
          direction = 135.0f - value;
      }
      else
      {
        if (_forceDirectionR > 0.0)
          direction = 305.0f - value;
        else
          direction = 225.0f - value;
      }
    }

    // mirror => break mode
    direction = 360 - direction;

    uint speed = map(max((uint)abs(forceL), (uint)abs(forceR)), 0, 1000, 0, 255);

    LimitPublishGo(direction, speed, SEND_INTERVAL + SEND_INTERVAL_ADD);
  }
};