#include <EspMQTTClient.h>

#include "SetupPage.h"
#include "Drive.h"
#include "Config.h"

extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern Drive drive;
extern String DeviceName;

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName


extern EspMQTTClient client;
