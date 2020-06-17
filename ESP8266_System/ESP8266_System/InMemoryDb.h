#pragma once

#include <stdint.h>

#define MAX_SAVED_INMEMORY_DATA 50

struct InMemoryData {
	uint32_t Time;
	uint32_t Data;
};

class InMemoryDb
{
private:
	uint8_t index = 0;
	uint8_t numbersOfValidData = 0;
	InMemoryData datas[MAX_SAVED_INMEMORY_DATA];
public:
	void Save(uint32_t, uint32_t);
	InMemoryData* GetData(uint8_t);
	InMemoryData* GetDatas(uint8_t*);
	void Reset();
};

