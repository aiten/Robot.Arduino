#include <EspMQTTClient.h>

#include "SetupPage.h"
#include "Ampel.h"
#include "Config.h"

extern ESP8266WebServer server;
extern SetupPage setupWiFi;
extern String DeviceName;
extern Ampel ampel[];

#define MQTT_DISCOVERY "robot/discovery/" + DeviceName
#define MQTT_CMND "robot/cmnd/" + DeviceName
#define MQTT_STAT "robot/stat/" + DeviceName


extern EspMQTTClient client;
