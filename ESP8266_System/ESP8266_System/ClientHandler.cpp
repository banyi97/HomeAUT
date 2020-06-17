#include "ClientHandler.h"

ClientHandler::ClientHandler()
{
	uint8_t mac = 0x01;
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
		this->clients[i].SetMAC(mac);
		mac++;
	}
}

void ClientHandler::addClient(uint8_t mac){
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
		if (this->clients[i].GetMAC() == mac && !this->clients[i].IsInUse()) {
			this->activeClients++;
			this->clients[i].setClientUseState(true);
			return;
		}
	}
}

void ClientHandler::removeClient(uint8_t mac){
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
		if (this->clients[i].GetMAC() == mac && this->clients[i].IsInUse()) {
			this->activeClients--;
			this->clients[i].Reset();
			return;
		}
	}
}

uint8_t ClientHandler::getFreeMAC()
{
	uint32_t unixTime = cron.getTime();
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
		if (!this->clients[i].IsInUse() || this->clients[i].lastActionTime() + MAX_MAC_LIFETIME <= unixTime) {
			this->clients[i].Reset();
			return this->clients[i].GetMAC();
		}
	}
	return 0;
}

uint8_t ClientHandler::getActiveClientsNumber()
{
	return this->activeClients;
}

bool ClientHandler::isElement(uint8_t* mac, uint8_t* oldMAC)
{
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
	{
		if (this->clients[i].checkDefMAC(mac)) {
			*oldMAC = this->clients[i].GetMAC();
			return true;
		}
	}
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
	{
		if (this->clients[i].GetMAC() == *oldMAC && !this->clients[i].IsInUse()) {
			return true;		
		}
	}
	*oldMAC = this->getFreeMAC();
	return false;
}

EspClient* ClientHandler::isConnectedClient(uint8_t* mac)
{
	uint8_t* serverMAC = handler.getServerMAC();
	if (mac[0] != serverMAC[0] || mac[1] != serverMAC[1] || mac[2] != serverMAC[2] || mac[3] != serverMAC[3] || mac[4] != serverMAC[4]) {
		return nullptr;
	}
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
	{	
		if (this->clients[i].GetMAC() == mac[5] && this->clients[i].IsInUse()) {
			return &this->clients[i];
		}
	}
	return nullptr;
}

EspClient* ClientHandler::getClient(uint8_t mac)
{
	for (uint8_t i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
		if (this->clients[i].GetMAC() == mac)
			return &this->clients[i];
	}
	return nullptr;
}
