#pragma once

////////////////////////////////////////////////////////

#include "EepromConfig.h"

////////////////////////////////////////////////////////

class SetupWiFi
{
private:

	const char* _nameSID;

	EepromConfig& _config;
	ESP8266WebServer& _server;

	uint _foundWLans = 0;

protected:

	EepromConfig& GetConfig() const { return _config; }
	ESP8266WebServer& GetServer() const { return _server; }

public:
	SetupWiFi(const char* nameSID, EepromConfig& config, ESP8266WebServer& server) : _nameSID(nameSID), _config(config), _server(server)
	{
	}

public:
	void Setup();

protected:
	virtual void OnMainPage();
	virtual String ConfigureAddValues();

	virtual void OnConfig();
	virtual bool StoreValues(String* config);
	virtual void OnScan();

private:
	bool TestWifi();
	void SetupAP();
	void LaunchWebServer();
	void ConfigureWebServer();

	void PrintNetworks();
	uint ScanNetworks();

	String GetHtmlNetworks();
};
