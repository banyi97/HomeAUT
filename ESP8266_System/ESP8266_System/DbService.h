#pragma once

#include "InMemoryDb.h"
#include "FS.h"
#include <list>

#define MAX_CLIENT 20

class Client {
public:
	bool Has = false;
	uint8_t id;
	uint8_t MAC;
	String clientName;
	InMemoryDb db;
};

class DbService{
private:
	uint32_t id = 1;
	//std::list<Client*> clients;
public:
	DbService();
	void setNewClient(uint8_t);
	void removeClient(uint8_t);
	void setData(uint8_t, String&);
	void setData(uint8_t, String&, uint32_t);
	bool getDatas(uint8_t, String&);
};
