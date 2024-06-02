#pragma once

//////////////////////////////////////////

#define SINGLE_LN298
//#define TWO_LN298

//////////////////////////////////////////

enum EConfigEEpromIdx {
  SSIDIdx = 0,
  PwdIdx,
  DeviceNameIdx,
  MqttBrokerIdx,
  MqttUserIdx,
  MqttPwdIdx,

  SizeIdx // to calculate max
};

//////////////////////////////////////////

// define for e.g. LOLIN(WEMOS) D1

#define STATUS_LED_PIN LED_BUILTIN

//////////////////////////////////////////

#include <EepromConfig.h>
#include <PicoMQTT.h>
#include <StatusLed.h>

//////////////////////////////////////////

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

//////////////////////////////////////////
