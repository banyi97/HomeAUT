#include "WifiHandler.h"

#include <FS.h>
#include <string.h>

bool WifiHandler::isNetworkConfigFileAvailable() {
	return SPIFFS.exists(F("/netconfig.json"));
}

void WifiHandler::createNetworkConfigFile() {
	File f = SPIFFS.open(F("/netconfig.json"), "w");
	f.println(F("{\"m\":6,\"d\":\"esp8266\",\"AP\":{\"s\":\"espAP\",\"p\":\"12345678\"},\"STA\":[{\"s\":\"\",\"p\":\"\"},{\"s\":\"\",\"p\":\"\"},{\"s\":\"\",\"p\":\"\"},{\"s\":\"\",\"p\":\"\"}]}"));
	f.close();
}

bool WifiHandler::loadNetworkConfigFile(JsonDocument& doc) {
	File file = SPIFFS.open(F("/netconfig.json"), "r");
	DeserializationError error = deserializeJson(doc, file);
	if (error) {
		PRINTLN(F("Failed to read file, using default configuration"));
		return false;
	}
	return true;
}

bool WifiHandler::saveNetworkConfigFile(JsonDocument& doc) {
	File file = SPIFFS.open(F("/netconfig.json"), "w");
	if (serializeJson(doc, file) == 0) {
		PRINTLN(F("Failed to write to file"));
	}
	file.close();
}

uint8_t* WifiHandler::getDefMAC()
{
	if (this->mode.bits.wifi == 1) { // sta
		return this->defSTA_MAC;
	}
	else if (this->mode.bits.wifi == 2) { // ap
		return this->defAP_MAC;
	}
	else
		return nullptr;
}

uint8_t* WifiHandler::getDefSTA_MAC()
{
	return this->defSTA_MAC;
}

uint8_t* WifiHandler::getDefAP_MAC()
{
	return this->defAP_MAC;
}

uint8_t* WifiHandler::getServerMAC()
{
	return this->serverMAC;
}

void WifiHandler::setMAC(uint8_t mac)
{
	uint8_t clientMAC[6];
	for (uint8_t i = 0; i < 5; i++) {
		clientMAC[i] = serverMAC[i];
	}
	PRINTF("before  mac: %x:%x:%x:%x:%x:%x\r\n", clientMAC[0], clientMAC[1], clientMAC[02], clientMAC[3], clientMAC[4], clientMAC[5]);
	this->clientMode.MAC = mac;;
	clientMAC[5] = mac;
	PRINTF("after  mac: %x:%x:%x:%x:%x:%x\r\n", clientMAC[0], clientMAC[1], clientMAC[02], clientMAC[3], clientMAC[4], clientMAC[5]);
	if (this->mode.bits.wifi == 1) { // sta
		wifi_set_macaddr(STATION_IF, clientMAC);
		PRINTLN("STA mac");
		PRINTLN(WiFi.macAddress());
	}
	else if (this->mode.bits.wifi == 2) { // ap
		wifi_set_macaddr(SOFTAP_IF, clientMAC);
		PRINTLN("Soft mac");
		PRINTLN(WiFi.softAPmacAddress());
	}
}

uint8_t* WifiHandler::getMAC()
{
	if (this->mode.bits.operationMode == SERVER_MODE) {
		return nullptr;
	}
	else {
		return &this->clientMode.MAC;
	}
}

bool WifiHandler::clientModeIsConnected()
{
	return this->clientMode.connectState == ClientConnectState::Accepted;
}

void WifiHandler::clientModeSet(uint8_t state)
{
	this->clientMode.connectState = state;
}

uint8_t WifiHandler::getClientMode()
{
	return this->clientMode.connectState;
}

void WifiHandler::resetMAC()
{
	if (this->mode.bits.wifi == 1) { // sta
		wifi_set_macaddr(STATION_IF, this->defSTA_MAC);
	}
	else if (this->mode.bits.wifi == 2) { // ap
		wifi_set_macaddr(SOFTAP_IF, this->defAP_MAC);
	}
}

void WifiHandler::removeNetworkConfigFile() {
	if (SPIFFS.exists(F("/netconfig.json"))) {
		SPIFFS.remove(F("/netconfig.json"));
	}
}

WifiHandler::WifiHandler()
{
	WiFi.macAddress(defSTA_MAC);
	WiFi.softAPmacAddress(defAP_MAC);
}

void WifiHandler::begin() {
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	if (!this->loadNetworkConfigFile(doc)) {
		this->removeNetworkConfigFile();
		ESP.restart();
	}
	NetworkMode mode;
	mode.mode = (uint8_t)doc["m"];
	this->mode.mode = mode.mode;
	const char* deviceName = doc["d"];
	this->deviceName = deviceName;
	PRINTLN("Mode");
	PRINTLN(mode.mode);
	if ((mode.bits.wifi) == STA_MODE) {
		WiFi.mode(WIFI_STA);
		PRINTLN("STA mode");
		for (uint8_t i = 0; i < MAX_NETWORK_NUMBER; ++i) {
			const char* ssid = doc["STA"][i]["s"];
			const char* pass = doc["STA"][i]["p"];
			this->wifiMulti.addAP(ssid, pass);
		}
		uint8_t num = 0;
		while (wifiMulti.run() != WL_CONNECTED) {
			PRINTLN("Connecting...");
			delay(1000);
			++num;
			if (num == MAX_SEC)
				break;
		}
		if (wifiMulti.run() == WL_CONNECTED) {
			PRINTLN(WiFi.localIP());
			PRINTLN(WiFi.macAddress());
			if (mode.bits.operationMode == SERVER_MODE) {
				wifi_set_macaddr(STATION_IF, serverMAC);
				PRINTLN("STA app");
				PRINTLN(WiFi.macAddress());
			}
			return;
		}
	}
	PRINTLN("AP mode");
	WiFi.mode(WIFI_AP);
	const char* ssid = doc["AP"]["s"];
	const char* pass = doc["AP"]["p"];
	PRINTLN(ssid);
	PRINTLN(pass);
	WiFi.softAP(ssid, pass, 6);
	WiFi.enableAP(true);
	PRINTLN("MAC address: ");
	PRINTLN(WiFi.softAPmacAddress());
	if (mode.bits.operationMode == SERVER_MODE) {
		wifi_set_macaddr(SOFTAP_IF, serverMAC);
		PRINTLN("Soft ap:");
		PRINTLN(WiFi.softAPmacAddress());
	}
}

String& WifiHandler::getDeviceName()
{
	return this->deviceName;
}

void WifiHandler::reload(){
	switch (WiFi.getMode())
	{
	case WIFI_STA:
		WiFi.disconnect(true);
		break;
	case WIFI_AP:
		WiFi.softAPdisconnect(true);
		break;
	default:
		break;
	}
	this->begin();
}

uint8_t WifiHandler::getMode(){
	return this->mode.mode;
}

void WifiHandler::setOpMode(uint8_t netMode){
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	this->loadNetworkConfigFile(doc);
	NetworkMode mode;
	mode.mode = (uint8_t)doc["m"];
	mode.bits.operationMode = netMode;
	doc["m"] = mode.mode;
	this->saveNetworkConfigFile(doc);
}

void WifiHandler::setWifiMode(uint8_t wifiMode){
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	this->loadNetworkConfigFile(doc);
	NetworkMode mode;
	mode.mode = (uint8_t)doc["m"];
	mode.bits.wifi = wifiMode;
	doc["m"] = mode.mode;
	this->saveNetworkConfigFile(doc);
}

void WifiHandler::setMode(uint8_t mode) {
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	this->loadNetworkConfigFile(doc);
	doc["m"] = mode;
	this->saveNetworkConfigFile(doc);
}

void WifiHandler::setAP(const String& ssid, const String& pass) {
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	this->loadNetworkConfigFile(doc);
	doc["AP"]["s"] = ssid;
	doc["AP"]["p"] = pass;
	this->saveNetworkConfigFile(doc);
}

void WifiHandler::setSTA(const String& ssid, const String& pass, uint8_t index) {
	StaticJsonDocument<MAX_STATIC_LENGTH> doc;
	this->loadNetworkConfigFile(doc);
	doc["STA"][index]["s"] = ssid;
	doc["STA"][index]["p"] = pass;
	this->saveNetworkConfigFile(doc);
}
