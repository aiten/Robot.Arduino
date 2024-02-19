#pragma once

//////////////////////////////////////////

#include <SetupWiFi.h>

//////////////////////////////////////////

class SetupPage : public SetupWiFi
{
private:
  typedef SetupWiFi super;

public:
  SetupPage(const char *nameSID, EepromConfig &config, ESP8266WebServer &server) : super(nameSID, config, server)
  {
  }

protected:
  virtual String ConfigureAddValues() override;
  virtual bool StoreValues(String *config) override;
};
