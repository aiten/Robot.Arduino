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

#include <EepromConfig.h>
#include <EspMQTTClient.h>
#include "SetupPage.h"

//////////////////////////////////////////

extern EepromConfig eepromConfig;
extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern EspMQTTClient client;

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;
extern String SendTo;

extern void MqttClientloop(void);

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

#define MQTT_SENDTO_CMND "robot/cmnd/" + SendTo
#define MQTT_SENDTO_STAT "robot/stat/" + SendTo

//////////////////////////////////////////

