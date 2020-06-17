#pragma once
#include <stdint.h>
#include <WString.h>

class Db {
public:
	virtual void Save(uint8_t, String&) = 0;
	virtual void Save(uint8_t, String&, uint32_t) = 0;
	virtual bool GetAll(uint8_t, String&) = 0;
};

