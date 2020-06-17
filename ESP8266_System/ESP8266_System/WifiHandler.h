#pragma once

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "DebugPrint.h"
#include "MessageKeyTypes.h"

#define MAX_NETWORK_NUMBER 4
#define MAX_SEC 15
#define MAX_STATIC_LENGTH 1024

#define SERVER_MODE 1
#define CLIENT_MODE 0
#define STA_MODE 1
#define AP_MODE 2

#define CLIENT_CONNECTED 1
#define CLIENT_NOT_CONNTECTED 0

struct ClientModeState {
	uint8_t MAC = 0;
	uint8_t connectState;
	uint8_t reconnectNum = 0;
};
union NetworkMode {
	uint8_t mode;
	struct {
		uint8_t wifi : 2;
		uint8_t operationMode : 1;
		uint8_t : 5;
	} bits;
	NetworkMode() :mode(0) {}
};
struct Network {
	char* ssid;
	char* pass;
};
class WifiHandler
{
	uint8_t serverMAC[6] = { 0x32, 0xAE, 0xA4, 0x07, 0x01, 0x00 };
	uint8_t defSTA_MAC[6];
	uint8_t defAP_MAC[6];
	NetworkMode mode;
	ESP8266WiFiMulti wifiMulti;
	String deviceName;
	ClientModeState clientMode;
public:
	WifiHandler();
	void begin();
	String& getDeviceName();
	void reload();
	void createNetworkConfigFile();
	bool isNetworkConfigFileAvailable();
	void removeNetworkConfigFile();
	uint8_t getMode();
	void setMode(uint8_t);
	void setOpMode(uint8_t);
	void setWifiMode(uint8_t);
	void setAP(const String& ssid, const String& pass);
	void setSTA(const String& ssid, const String& pass, uint8_t index);
	bool loadNetworkConfigFile(JsonDocument& doc);
	bool saveNetworkConfigFile(JsonDocument& doc);
	uint8_t* getDefMAC();
	uint8_t* getDefSTA_MAC();
	uint8_t* getDefAP_MAC();
	uint8_t* getServerMAC();
	void setMAC(uint8_t mac);
	uint8_t* getMAC();
	bool clientModeIsConnected();
	void clientModeSet(uint8_t state);
	uint8_t getClientMode();
	void resetMAC();
};

