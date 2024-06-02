#pragma once

//////////////////////////////////////////

enum EConfigEEpromIdx {
  SSIDIdx = 0,
  PwdIdx,
  DeviceNameIdx,
  MqttBrokerIdx,
  MqttUserIdx,
  MqttPwdIdx,
  SendToIdx,

  SizeIdx // to calculate max
};

//////////////////////////////////////////

#if defined(ARDUINO_ESP8266_ESP01)

// use Board "Generic ESP8285" to get ARDUINO_ESP8266_ESP01 defined

#define STATUS_LED_PIN 1 // 0xff
#define PUSHBUTTON_PIN 3 // 0xff
#define PUSHBUTTON_ISON true
#define DONOTUSESERIAL

#define GYROCONVERT(x, y) -y, x

#define SDA_PIN 0
#define SCL_PIN 2

#else

// define for e.g. LOLIN(WEMOS) D1

#define STATUS_LED_PIN LED_BUILTIN
#define PUSHBUTTON_PIN D3
#define PUSHBUTTON_ISON false

#define GYROCONVERT(x, y) x, y

#endif

//////////////////////////////////////////

#include <EepromConfig.h>
#include <PicoMQTT.h>
#include <StatusLed.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

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
