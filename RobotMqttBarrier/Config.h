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

  SizeIdx          // to calculate max   
};

//////////////////////////////////////////

#include <EepromConfig.h>
#include <EspMQTTClient.h>
#include <StatusLed.h>
#include "SetupPage.h"
#include "PushButton.h"
#include "Ampel.h"

//////////////////////////////////////////

extern EepromConfig eepromConfig;
extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern EspMQTTClient client;
extern StatusLed statusLed;

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;
extern void MqttClientloop(void);

extern Ampel ampel[];

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

//////////////////////////////////////////

