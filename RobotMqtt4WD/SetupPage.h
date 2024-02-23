#pragma once

//////////////////////////////////////////

#include <SetupWiFi.h>

//////////////////////////////////////////

class SetupPage : public SetupWiFi
{
private:
  typedef SetupWiFi super;

public:
  SetupPage(const char *nameSID, EepromConfig &config, ESP8266WebServer &server, uint8_t statusPin) : super(nameSID, config, server, statusPin)
  {
  }

protected:
  virtual String ConfigureAddValues() override
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

  virtual bool StoreValues(String *configString) override
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
};
