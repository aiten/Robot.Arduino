#pragma once

//////////////////////////////////////////

enum EConfigEEpromIdx
{
  SSIDIdx=0,
  PwdIdx,
  DeviceNameIdx,
  MqttBrokerIdx,
  MqttUserIdx,
  MqttPwdIdx,
  SendToIdx,

  SizeIdx          // to calculate max   
};

//////////////////////////////////////////

// define for e.g. LOLIN(WEMOS) D1

#define STATUS_LED_PIN LED_BUILTIN
#define PUSHBUTTON_PIN 0xff
#define PUSHBUTTON_ISON true

#define SENSOR1_TRGPIN D1
#define SENSOR1_ECHOPIN D2

#define SENSOR2_TRGPIN D5
#define SENSOR2_ECHOPIN D6

//////////////////////////////////////////

#include <EepromConfig.h>
#include <PicoMQTT.h>
#include <StatusLed.h>
#include "SetupPage.h"
#include "HCSr04.h"

//////////////////////////////////////////

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;
extern String SendTo;

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

#define MQTT_SENDTO_CMND "robot/cmnd/" + SendTo
#define MQTT_SENDTO_STAT "robot/stat/" + SendTo

//////////////////////////////////////////
