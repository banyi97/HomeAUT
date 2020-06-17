#pragma once

#include "DebugPrint.h"
#include "RTClib.h"
#include <stdint.h>
#include <FS.h>

#define LENGTH_OF_VECTOR 20
#define SINGLE_LIST_LENGTH 10

#define SEC 1
#define MIN 60
#define HOUR 3600
#define DAY 86400

#define TEST_THERMO_ID 200

extern void (*func_ptr[])(void* param);

enum FUNCTIONS_ID {
	test = 0,
	test2 = 1,
	WSsendRandom = 2,
	EspNowSender = 3,
	restart = 4,
	tryConnect = 5,
	testThermo = 6
};

union CronExpressionMeta {
	uint8_t header;
	struct {
		uint8_t isActive : 1;
		uint8_t isSetted : 1;
		uint8_t isSaved : 1;
		uint8_t : 5;
	} bits;
	CronExpressionMeta():header(0) {}
};
struct CronExpression {
	uint8_t id;
	CronExpressionMeta meta;
	uint64_t sec;
	uint64_t min;
	uint32_t hour;
	uint32_t day;
	uint8_t dayOfWeek;
	uint16_t month;
	uint32_t nextActionTime;
	uint8_t funID;
	void* param;
	CronExpression(): meta(), id(0), nextActionTime(0){}
};
struct CronSingle {
	CronExpressionMeta meta;
	FUNCTIONS_ID funID;
	uint32_t executeTime;
	void* param;
};

struct CronSaveLoad {
	uint64_t sec;
	uint64_t min;
	uint32_t hour;
	uint32_t day;
	uint8_t dayOfWeek;
	uint16_t month;
	uint8_t id;
	uint8_t funID;
	uint8_t meta;
};

class CronHandler
{
private:
	RTC_Millis rtc;
	CronExpression list[LENGTH_OF_VECTOR];
	CronSingle listSingle[SINGLE_LIST_LENGTH];
	void setExpressionFields(CronExpression*, String&);
	uint8_t checkSubType(const String&);
	bool setField(uint64_t&, const String&, const uint8_t);
	bool setField(uint32_t&, const String&, const uint8_t);
	bool setField(uint16_t&, const String&, const uint8_t);
	bool setField(uint8_t&,  const String&, const uint8_t);
	size_t numberOfCharInString(const String&, const char);
	void calcNextActionTime(CronExpression*, uint32_t);
	bool setNewId(uint8_t, uint8_t);
	uint8_t numberOfSavedExpression();
public:
	CronHandler();
	CronHandler(const char*, const char*);
	void handle();
	uint8_t addAction(String& expression, FUNCTIONS_ID funID, void* param = nullptr, bool save = false, bool executeAfterInit = false, bool enable = true);
	uint8_t addAction(String& expression, FUNCTIONS_ID funID, uint8_t id, void* param = nullptr, bool save = false, bool executeAfterInit = false, bool enable = true);
	bool addSingleRunAction(FUNCTIONS_ID funID, uint32_t time = 0, void* param = nullptr);
	bool removeAction(uint8_t id);
	bool enableAction(uint8_t id);
	bool disableAction(uint8_t id);
	CronExpression* getCron(uint8_t id);
	CronExpression* getAllCron();
	void loadCron();
	void saveCron();
	void removeAllSaved();
	void adjustRTC(DateTime&);
	uint32_t getTime();
};

