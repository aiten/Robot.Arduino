#pragma once

//////////////////////////////////////////

#include <EepromConfig.h>

//////////////////////////////////////////

enum EConfigEEpromIdx
{
  SSIDIdx=0,
  PwdIdx,
  DeviceNameIdx,
  MqttBrokerIdx,
  MqttUserIdx,
  MqttPwdIdx,
};

extern EepromConfig eepromConfig;
