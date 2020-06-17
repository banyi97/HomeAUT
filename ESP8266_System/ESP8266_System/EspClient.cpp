#include "EspClient.h"

void EspClient::SetMAC(uint8_t mac)
{
	this->MAC = mac;
}

void EspClient::SaveData(uint32_t data, uint32_t time)
{
	this->db.Save(data, time);
}

InMemoryData* EspClient::GetData(uint8_t index)
{
	return this->db.GetData(index);
}

InMemoryData* EspClient::GetDatas(uint8_t* p)
{
	return this->db.GetDatas(p);
}

void EspClient::Reset()
{
	this->db.Reset();
	this->isInUse = false;
}

uint8_t EspClient::GetMAC()
{
	return this->MAC;
}

bool EspClient::IsInUse()
{
	return this->isInUse;
}

void EspClient::SetClientName(String& name)
{
	this->Name = name;
}

void EspClient::setType(uint8_t typeID)
{
	this->Type = typeID;
}

uint8_t EspClient::getType()
{
	return this->Type;
}

void EspClient::setDefMAC(uint8_t* mac)
{
	memcpy(this->defMAC, mac, 6);
}

uint8_t* EspClient::getDefMAC()
{
	return this->defMAC;
}

bool EspClient::checkDefMAC(uint8_t* mac)
{
	for (uint8_t i = 0; i < 6; i++)
	{
		if (this->defMAC[i] != mac[i])
			return false;
	}
	return true;
}

void EspClient::setClientUseState(bool state)
{
	this->isInUse = state;
}

uint32_t EspClient::lastActionTime()
{
	return this->latestIncominingReq;
}

void EspClient::updateAction()
{
	this->latestIncominingReq = cron.getTime();
}

