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
};

//////////////////////////////////////////

#include <EepromConfig.h>
#include <EspMQTTClient.h>
#include "SetupPage.h"
#include "Ampel.h"

//////////////////////////////////////////

extern EepromConfig eepromConfig;
extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern EspMQTTClient client;

extern String DeviceName;
extern String MqttBroker;
extern String MqttUser;
extern String MqttPwd;

extern Ampel ampel[];

//////////////////////////////////////////

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName

//////////////////////////////////////////

