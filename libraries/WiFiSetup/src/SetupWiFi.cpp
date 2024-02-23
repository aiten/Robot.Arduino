#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "SetupWiFi.h"
#include "StatusLed.h"

void SetupWiFi::Setup()
{
	Serial.println("Disconnecting previously connected WiFi");

	WiFi.disconnect();

	delay(10);
	Serial.println("Startup");

	//---------------------------------------- Read EEPROM for SSID and pass
	Serial.println("Reading EEPROM");

	auto configString = _config.ReadStrings();
	String ssid = configString[0];
	String pwd = configString[1];

	Serial.print("SSID: ");
	Serial.println(ssid);

	Serial.print("PASS: ");
	Serial.println("*******");
	// Serial.println(epass);

	WiFi.begin(ssid.c_str(), pwd.c_str());

	if (TestWifi())
	{
		Serial.println("Succesfully Connected!!!");
		Serial.print("IP address:  ");
		Serial.println(WiFi.localIP());
		Serial.printf("Gataway IP:  %s\n", WiFi.gatewayIP().toString().c_str());
		Serial.print("Subnet mask: ");
		Serial.println(WiFi.subnetMask());
		LaunchWebServer();
	}
	else
	{
		StatusLed statusLed(_statusPin, 75);

		Serial.println("Turning the HotSpot On");
		SetupAP();
		LaunchWebServer();

		Serial.println("Waiting.");

		unsigned long lastCh = 0;
		while ((WiFi.status() != WL_CONNECTED))
		{
			if (lastCh + 1000 < millis())
			{
				Serial.print(".");
				lastCh = millis();
			}
			_server.handleClient();
			statusLed.Loop();
		}
	}
}

bool SetupWiFi::TestWifi()
{
	// test 10 sec

	unsigned long testUntil = millis() + 10000;
	unsigned long testNextUntil = 0;

	StatusLed statusLed(_statusPin, 200);

	Serial.println("Waiting for Wifi to connect");

	while (testUntil > millis())
	{
		if (WiFi.status() == WL_CONNECTED)
		{
			Serial.println();
			return true;
		}

		if (testNextUntil < millis())
		{
			testNextUntil = millis() + 500;
			Serial.print("*");
		}

		delay(25);
		statusLed.Loop();
	}

	Serial.println();
	Serial.println("Connect timed out, opening AP");
	return false;
}

void SetupWiFi::SetupAP(void)
{
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	_foundWLans = ScanNetworks();
	PrintNetworks();

	String appName = _nameSID;
	appName += '-';
	auto mac = WiFi.macAddress();

	for (uint i = mac.length() / 2; i < mac.length(); i++)
	{
		if (mac[i] != ':')
		{
			appName += mac[i];
		}
	}

	WiFi.softAP(appName, "");
}

uint SetupWiFi::ScanNetworks()
{
	Serial.println("Scan networks");
	auto foundWLans = WiFi.scanNetworks();
	Serial.println("scan done");
	return foundWLans;
}

void SetupWiFi::PrintNetworks()
{
	if (_foundWLans == 0)
	{
		Serial.println("no networks found");
	}
	else
	{
		Serial.print(_foundWLans);
		Serial.println(" networks found");
		for (uint i = 0; i < _foundWLans; ++i)
		{
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
			delay(10);
		}
	}
}

void SetupWiFi::LaunchWebServer()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println("WiFi connected");
	}
	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP());
	Serial.print("SoftAP IP: ");
	Serial.println(WiFi.softAPIP());

	ConfigureWebServer();
	// Start the server
	_server.begin();
	Serial.println("Server started");
}

void SetupWiFi::OnMainPage()
{
	auto configString = _config.ReadStrings();
	String ssid = configString[0];
	String pwd = configString[1];

	IPAddress ip = WiFi.softAPIP();
	String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
	String content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at "
					 "<style> label { width:100px; display: inline-block; } </style>"
					 "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
	content += ipStr;
	content += "<p>";
	content += GetHtmlNetworks();
	content += "</p><br />"
			   "<form method='get' action='setting'>"
			   "<label>SSID: </label><input name='ssid' value=\"" +
			   ssid + "\" length=32><input type=\"password\" name='pass' value=\"" + pwd + "\" length=64><br />";
	content += ConfigureAddValues();
	content += "<br /><input type='submit'></form>"
			   "</html>";
	_server.send(200, "text/html", content);
}

String SetupWiFi::ConfigureAddValues()
{
	return "";
}

void SetupWiFi::OnConfig()
{
	int statusCode;
	String content;

	auto configString = _config.ReadStrings();

	if (StoreValues(configString))
	{
		_config.WriteStrings(configString);

		ESP.reset();
	}
	else
	{
		content = "{\"Error\":\"404 not found\"}";
		statusCode = 404;
		Serial.println("Sending 404");
	}

	_server.sendHeader("Access-Control-Allow-Origin", "*");
	_server.send(statusCode, "application/json", content);
}

bool SetupWiFi::StoreValues(String *configString)
{
	configString[0] = _server.arg("ssid");
	configString[1] = _server.arg("pass");

	return (configString[0].length() > 0 && configString[1].length() > 0);
}

void SetupWiFi::OnScan()
{
	// setupAP();
	IPAddress ip = WiFi.softAPIP();
	String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

	String content = "<!DOCTYPE HTML>\r\n<html>go back";
	content += "<p>";
	content += GetHtmlNetworks();

	_server.send(200, "text/html", content);
}

void SetupWiFi::ConfigureWebServer()
{
	_server.on("/", [this]()
			   { this->OnMainPage(); });
	_server.on("/scan", [this]()
			   { this->OnScan(); });

	_server.on("/setting", [this]()
			   { this->OnConfig(); });
}

String SetupWiFi::GetHtmlNetworks()
{
	String st = "<ol>";
	for (int i = 0; i < (int)_foundWLans; ++i)
	{
		// Print SSID and RSSI for each network found
		st += "<li>";
		st += WiFi.SSID(i);
		st += " (";
		st += WiFi.RSSI(i);

		st += ")";
		st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
		st += "</li>";
	}
	st += "</ol>";

	return st;
}
