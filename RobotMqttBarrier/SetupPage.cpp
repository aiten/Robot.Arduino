#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "Config.h"
#include "SetupPage.h"

String SetupPage::ConfigureAddValues()
{
  auto configString = GetConfig().ReadStrings();
  String deviceName = configString[EConfigEEpromIdx::DeviceNameIdx];
  String mqttBroker = configString[EConfigEEpromIdx::MqttBrokerIdx];
  String mqttUser = configString[EConfigEEpromIdx::MqttUserIdx];
  String mqttPwd = configString[EConfigEEpromIdx::MqttPwdIdx];

  return "<label>Name:</label><input name='name'  value=\"" + deviceName +
         "\" length=32><br />"
         "<label>Mqtt-Broker:</label><input name='mqttBroker' value=\"" +
         mqttBroker +
         "\" length=32><br />"
         "<label>Mqtt-User:</label><input name='mqttUser' value=\"" +
         mqttUser +
         "\" length=32><br />"
         "<label>Mqtt-Pwd:</label><input type=\"password\" name='mqttPwd' value=\"" +
         mqttPwd + "\" length=32><br />";
}

bool SetupPage::StoreValues(String *configString)
{
  if (!super::StoreValues(configString))
  {
    return false;
  }

  configString[EConfigEEpromIdx::DeviceNameIdx] = GetServer().arg("name");
  configString[EConfigEEpromIdx::MqttBrokerIdx] = GetServer().arg("mqttBroker");
  configString[EConfigEEpromIdx::MqttUserIdx] = GetServer().arg("mqttUser");
  configString[EConfigEEpromIdx::MqttPwdIdx] = GetServer().arg("mqttPwd");
  return true;
}