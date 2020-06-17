#pragma once

#include <stdint.h>

#define MAX_DATA 100

class DB_Handler
{
private:
	uint8_t nextIndex = 0;
	String fileName;
	uint8_t fileIsValid = 0;
public:
	DB_Handler(String& filename);
	void saveData(String& data, uint32_t unixTime);
	void loadData(String& data);
};

