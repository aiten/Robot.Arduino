#pragma once

////////////////////////////////////////////////////////

#include "EepromConfig.h"

////////////////////////////////////////////////////////

class SetupWiFi
{
private:
	const char *_nameSID;

	EepromConfig &_config;
	ESP8266WebServer &_server;

	uint _foundWLans = 0;
	uint8_t _statusPin = 0;

protected:
	EepromConfig &GetConfig() const { return _config; }
	ESP8266WebServer &GetServer() const { return _server; }

public:
	SetupWiFi(const char *nameSID, EepromConfig &config, ESP8266WebServer &server, uint8_t statusPin = 0xff) : 
		_nameSID(nameSID),
		_config(config),
		_server(server),
		_statusPin(statusPin)
	{
	}

public:
	void Setup();

protected:
	virtual void OnMainPage();
	virtual String ConfigureAddValues();

	virtual void OnConfig();
	virtual bool StoreValues(String *config);
	virtual void OnScan();

private:
	bool TestWifi();
	void SetupAP();
	void LaunchWebServer();
	void ConfigureWebServer();

	void PrintNetworks();
	uint ScanNetworks();

	String GetHtmlNetworks();

	bool HasStatusPin() const { return _statusPin != 0xff; }
};
