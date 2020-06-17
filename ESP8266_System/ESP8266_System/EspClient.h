#pragma once

#include "InMemoryDb.h"
#include "WString.h"

#include "CronHandler.h"

extern CronHandler cron;

class EspClient
{
private:
	bool isInUse = false;
	uint8_t MAC;
	uint8_t Type;
	uint8_t defMAC[6];
	uint32_t latestIncominingReq;
	String Name;
	InMemoryDb db;
public:
	void SetMAC(uint8_t);
	void SaveData(uint32_t data, uint32_t time);
	InMemoryData* GetData(uint8_t);
	InMemoryData* GetDatas(uint8_t*);
	void Reset();
	uint8_t GetMAC();
	bool IsInUse();
	void SetClientName(String&);
	void setType(uint8_t typeID);
	uint8_t getType();
	void setDefMAC(uint8_t* mac);
	uint8_t* getDefMAC();
	bool checkDefMAC(uint8_t* mac);
	void setClientUseState(bool state);
	uint32_t lastActionTime();
	void updateAction();
};

