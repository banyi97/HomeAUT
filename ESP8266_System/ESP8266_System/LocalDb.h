#include "Db.h"
#include "FS.h"
#pragma once
#define MAX_LINE_OF_DATA 100

class LocalDb : public Db {
public:
	virtual void Save(uint8_t, String&);
	virtual void Save(uint8_t, String&, uint32_t);
	virtual bool GetAll(uint8_t, String&);
};

