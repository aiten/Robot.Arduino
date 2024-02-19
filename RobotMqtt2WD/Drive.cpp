#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <arduino.h>
#include <cstdint>

#include "Drive.h"
#include "MqttClient.h"

Motor motor1(D2, D1, D6);
Motor motor2(D4, D3, D7);

void Drive::Setup()
{
  pinMode(ONOFF_PIN, OUTPUT);
  motor1.Setup();
  motor2.Setup();
}

void Drive::Poll()
{
  motor1.Poll();
  motor2.Poll();
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
      DriveTo(head.dir, head.duration, head.speed);
      _driveBuffer.Dequeue();
    }
  }
}

bool Drive::Queue(uint16_t dir, uint32_t duration, uint8_t speed)
{
  if (_driveBuffer.IsFull())
  {
    return false;
  }

  Command &tail = _driveBuffer.NextTail();
  tail.speed = speed;
  tail.duration = duration;
  tail.dir = dir;

  _driveBuffer.Enqueue();

  if (IsIdle())
  {
    _until = 0;
  }

  return true;
}

bool Drive::Go(uint16_t dir, uint32_t duration, uint8_t speed)
{
  _driveBuffer.Clear();

  DriveTo(dir, duration, speed);

  return true;
}

void Drive::DriveTo(uint16_t dir, uint32_t duration, uint8_t speed)
{
  Serial.printf("DRIVE: driveto: dir=%u, duration=%u, speed=%u\n", (uint)dir, (uint)duration, (uint)speed);
  
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

  motor1.Start(speed_r*4);
  motor2.Start(speed_l*4);
  
  Serial.printf("DRIVE: right=%i, left=%i\n", (int)speed_r, (int)speed_l);

  JsonDocument doc;

  doc["direction"] = dir;
  doc["speed"] = speed;
  doc["speed_l"] = speed_l;
  doc["speed_r"] = speed_r;

  String output;
  serializeJson(doc, output);
  client.publish(MQTT_STAT + "/drive", output.c_str());

  return true;
}

void Drive::Stop()
{
  Serial.println("stop");
  motor1.Stop();
  motor2.Stop();
  digitalWrite(ONOFF_PIN, LOW);

  JsonDocument doc;
  doc["speed"] = 0;

  String output;
  serializeJson(doc, output);
  client.publish(MQTT_STAT + "/drive", output.c_str());
}
