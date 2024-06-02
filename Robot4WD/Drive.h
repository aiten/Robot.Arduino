#pragma once

//////////////////////////////////////////

#include "Config.h"
#include <RingBuffer.h>

//////////////////////////////////////////

const int ONOFF_PIN = D0;

class Drive
{
public:
  Drive(PicoMQTT::Client &client, StatusLed &statusLed) : _client(client), _statusLed(statusLed)
  {
  }

private:
  PicoMQTT::Client &_client;
  StatusLed &_statusLed;

  const uint32_t IdleTime = 0xffffffff;

  uint32_t _until = IdleTime;

  bool Start(uint16_t dir, uint8_t speed);
  void Stop();

  void SetPins(int16_t speed, uint8_t pinFW, uint8_t pinBW);

  struct Command
  {
    uint16_t dir;
    uint32_t duration;
    uint8_t speed;
    uint16_t id;
    float time;
  };

  CRingBufferQueue<Command, 128> _driveBuffer;

  void DriveTo(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time);

public:
  void Setup();

  void Poll();

  bool Queue(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time);
  bool Go(uint16_t dir, uint32_t duration, uint8_t speed, uint id, float time); // instante, without queue (empty queue)

  bool IsIdle()
  {
    return _until == IdleTime;
  }
};