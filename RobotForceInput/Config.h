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

//////////////////////////////////////////

#include <EepromConfig.h>
#include <EspMQTTClient.h>
#include <StatusLed.h>

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

