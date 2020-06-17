#pragma once

#include "EspClient.h"
#include "CronHandler.h"
#include "WifiHandler.h"

extern CronHandler cron;
extern WifiHandler handler;

#define MAX_NUMBER_OF_CLIENTS 20
#define MAX_MAC_LIFETIME 300 // 5 sec

class ClientHandler
{
	uint8_t activeClients = 0;
	EspClient clients[MAX_NUMBER_OF_CLIENTS];
public:
	ClientHandler();
	void addClient(uint8_t);
	void removeClient(uint8_t);
	uint8_t getFreeMAC();
	uint8_t getActiveClientsNumber();
	bool isElement(uint8_t* mac, uint8_t* oldMAC);
	EspClient* isConnectedClient(uint8_t* mac);
	EspClient* getClient(uint8_t mac);
};

