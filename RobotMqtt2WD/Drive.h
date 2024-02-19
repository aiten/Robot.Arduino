#pragma once

//////////////////////////////////////////

#include <Motor.h>
#include "RingBuffer.h"
#include <EspMQTTClient.h>

//////////////////////////////////////////

extern EspMQTTClient client;

//////////////////////////////////////////

const int ONOFF_PIN = D0;
const uint8_t offValue = 0;

class Drive
{
private:

  const uint32_t IdleTime = 0xffffffff;

  uint32_t _until = IdleTime;

  bool Start(uint16_t dir, uint8_t speed);
  void Stop();

  struct Command
  {
    uint16_t dir;
    uint32_t duration;
    uint8_t speed;
  };

  CRingBufferQueue<Command,128> _driveBuffer;

  void DriveTo(uint16_t dir, uint32_t duration, uint8_t speed);

public:

  void Setup();

  void Poll();

  bool Queue(uint16_t dir, uint32_t duration, uint8_t speed);
  bool Go(uint16_t dir, uint32_t duration, uint8_t speed);      // instante, without queue (empty queue)

  bool IsIdle() 
  {
    return _until == IdleTime;
  }
};