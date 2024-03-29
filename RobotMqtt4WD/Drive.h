#pragma once

//////////////////////////////////////////

#include <RingBuffer.h>
#include <EspMQTTClient.h>

//////////////////////////////////////////

extern EspMQTTClient client;

//////////////////////////////////////////

const int ONOFF_PIN = D0;

class Drive
{
private:

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