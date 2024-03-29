#pragma once

//////////////////////////////////////////

#define SINGLE_LN298
//#define TWO_LN298

//////////////////////////////////////////

enum EConfigEEpromIdx
{
  SSIDIdx=0,
  PwdIdx,
  DeviceNameIdx,
  MqttBrokerIdx,
  MqttUserIdx,
  MqttPwdIdx,

  SizeIdx          // to calculate max   
};

//////////////////////////////////////////

#include <EepromConfig.h>
#include <EspMQTTClient.h>
#include <StatusLed.h>
#include "SetupPage.h"
#include "Drive.h"

//////////////////////////////////////////

extern EepromConfig eepromConfig;
extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern EspMQTTClient client;
extern Drive drive;
extern StatusLed statusLed;

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;
extern void MqttClientloop(void);

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

//////////////////////////////////////////

