#include <ArduinoJson.h>

#include "Drive.h"

void Drive::Setup()
{
  pinMode(ONOFF_PIN, OUTPUT);
  Stop();
}

void Drive::Poll()
{
  if (millis() >= _until)
  {
    if (_driveBuffer.IsEmpty())
    {
      Stop();
      _until = IdleTime;
    }
    else
    {
      Command &head = _driveBuffer.Head();
      DriveTo(head.dir, head.duration, head.speed, head.id, head.time);
      _driveBuffer.Dequeue();
    }
  }
}

bool Drive::Queue(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time)
{
  if (_driveBuffer.IsFull())
  {
    return false;
  }

  Command &tail = _driveBuffer.NextTail();
  tail.speed = speed;
  tail.duration = duration;
  tail.dir = dir;
  tail.id = id;
  tail.time = time;

  _driveBuffer.Enqueue();

  if (IsIdle())
  {
    _until = 0;
  }

  return true;
}

bool Drive::Go(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time)
{
  _driveBuffer.Clear();

  DriveTo(dir, duration, speed, id, time);

  return true;
}

void Drive::DriveTo(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time)
{
  _statusLed.ToggleNow(100);

  auto myTime=millis();
  Serial.printf("DRIVE: time=%u.%03u, driveto: dir=%u, duration=%u, speed=%u, id=%u, time=%0.3f\n", (uint)(myTime/1000),(uint)(myTime%1000), (uint)dir, (uint)duration, (uint)speed, id, time);

  if (Start(dir, speed))
  {
    digitalWrite(ONOFF_PIN, HIGH);
  }

  _until = millis() + duration;
}

bool Drive::Start(uint16_t dir, uint8_t speed)
{
  if (speed == 0)
  {
    Stop();
    return false;
  }

  int16_t speed_r;
  int16_t speed_l;

  if (dir < 90)
  {
    speed_l = speed;
    speed_r = map(dir, 0, 90, speed, -speed);
  }
  else if (dir < 180)
  {
    speed_l = map(dir, 90, 180, speed, -speed);
    speed_r = -speed;
  }
  else if (dir < 270)
  {
    speed_l = -speed;
    speed_r = map(dir, 180, 270, -speed, speed);
  }
  else
  {
    speed_l = map(dir, 270, 360, -speed, speed);
    speed_r = speed;
  }

#if defined SINGLE_LN298
  SetPins(speed_l, D5, D6); // left
  SetPins(speed_r, D7, D8); // right
#elif defined TWO_LN298
  SetPins(speed_l, D1, D2); // front left
  SetPins(speed_l, D3, D4); // rear right
  SetPins(speed_r, D6, D5); // front right
  SetPins(speed_r, D8, D7); // rear right
#endif

//  Serial.printf("DRIVE: time=%lu, right=%i, left=%i\n", millis(), (int)speed_r, (int)speed_l);

  JsonDocument doc;

  doc["direction"] = dir;
  doc["speed"] = speed;
  doc["speed_l"] = speed_l;
  doc["speed_r"] = speed_r;

  String output;
  serializeJson(doc, output);
  _client.publish(MQTT_STAT + "/drive", output.c_str());

  return true;
}

void Drive::SetPins(int16_t speed, uint8_t pinFW, uint8_t pinBW)
{
  analogWrite(pinFW, speed >= 0 ? speed : 0);
  analogWrite(pinBW, speed <= 0 ? -speed : 0);
}

void Drive::Stop()
{
  auto myTime=millis();
  Serial.printf("DRIVE: time=%u.%03u, stop\n", (uint)(myTime/1000),(uint)(myTime%1000));
#if defined SINGLE_LN298
  SetPins(0, D5, D6); // left
  SetPins(0, D7, D8); // right
#elif defined TWO_LN298
  SetPins(0, D1, D2);       // front left
  SetPins(0, D3, D4);       // rear right
  SetPins(0, D6, D5);       // front right
  SetPins(0, D8, D7);       // rear right
#endif
  digitalWrite(ONOFF_PIN, LOW);

  JsonDocument doc;
  doc["speed"] = 0;

  String output;
  serializeJson(doc, output);
  _client.publish(MQTT_STAT + "/drive", output.c_str());
}
