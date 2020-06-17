#include "CronHandler.h"

void CronHandler::setExpressionFields(CronExpression* p, String& expression)
{
	size_t lastIndex = 0, index = 0;
	for (uint8_t i = 0; i < 6; i++) {
		index = expression.indexOf(' ', lastIndex);
		String sub = expression.substring(lastIndex, index);
		lastIndex = index + 1;
		switch (i)
		{
		case 0:
			this->setField(p->sec, sub, 60);
			break;
		case 1:
			this->setField(p->min, sub, 60);
			break;
		case 2:
			this->setField(p->hour, sub, 24);
			break;
		case 3:
			this->setField(p->day, sub, 31);
			break;
		case 4:
			this->setField(p->month, sub, 12);
			break;
		case 5:
			this->setField(p->dayOfWeek, sub, 7);
			break;
		default:
			break;
		}
	}
}

uint8_t CronHandler::checkSubType(const String& sub)
{
	if (sub.startsWith("*/")) {
		return 1;
	}
	else if (sub.startsWith("-")) {
		return 2;
	}
	else if (sub.startsWith(",")) {
		return 3;
	}
	else if (sub.startsWith("*")) {
		return 4;
	}
	return 0;
}

bool CronHandler::setField(uint64_t& field, const String& sub, const uint8_t fieldConst)
{
	int32_t convert = -1, rangeRight = -1;
	uint8_t index = 0, lastIndex = 0;
	field = 0;
	uint32_t type = this->checkSubType(sub);
	uint32_t* p = (uint32_t*)&field;
	switch (type)
	{
	case 1:
		sscanf(sub.c_str(), "*/%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			for (uint8_t i = 0; i < fieldConst; i += convert)
			{
				if (i < 32) {
					p[0] |= 1 << i;
				}
				else {
					p[1] |= 1 << i;
				}
			}
			return true;
		}
		break;
	case 2:
		sscanf(sub.c_str(), "%d-%d", &convert, &rangeRight);
		if (convert >= 0 && rangeRight >= 0 && convert < fieldConst && rangeRight < fieldConst && convert <= rangeRight) {
			for (uint8_t i = convert; i <= rangeRight; i++) {
				if (i < 32) {
					p[0] |= 1 << i;
				}
				else {
					p[1] |= 1 << i;
				}
			}
			return true;
		}
		break;
	case 3:
		for (uint8_t i = 0; i <= this->numberOfCharInString(sub, ','); i++)
		{
			index = sub.indexOf(' ', lastIndex);
			String subc = sub.substring(lastIndex, index);
			lastIndex = index + 1;
			sscanf(subc.c_str(), "%d", &convert);
			if (convert >= 0 && convert < fieldConst) {
				if (convert < 32) {
					p[0] |= 1 << convert;
				}
				else {
					p[1] |= 1 << convert;
				}
			}
		}
		return true;
		break;
	case 4:
		for (uint8_t i = 0; i < fieldConst; i++)
		{
			if (i < 32) {
				p[0] |= 1 << i;
			}
			else {
				p[1] |= 1 << i;
			}
		}
		return true;
		break;
	default:
		sscanf(sub.c_str(), "%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			if (convert < 32) {
				p[0] |= 1 << convert;
			}
			else {
				p[1] |= 1 << convert;
			}
			return true;
		}
		break;
	}
	return false;
}

bool CronHandler::setField(uint32_t& field, const String& sub, const uint8_t fieldConst)
{
	int32_t convert = -1, rangeRight = -1;
	uint8_t index = 0, lastIndex = 0;
	field = 0;
	uint32_t type = this->checkSubType(sub);
	switch (type)
	{
	case 1:
		sscanf(sub.c_str(), "*/%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			for (uint8_t i = 0; i < fieldConst; i += convert)
			{
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 2:
		sscanf(sub.c_str(), "%d-%d", &convert, &rangeRight);
		if (convert >= 0 && rangeRight >= 0 && convert < fieldConst && rangeRight < fieldConst && convert <= rangeRight) {
			for (uint8_t i = convert; i <= rangeRight; i++) {
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 3:
		for (uint8_t i = 0; i <= this->numberOfCharInString(sub, ','); i++)
		{
			index = sub.indexOf(' ', lastIndex);
			String subc = sub.substring(lastIndex, index);
			lastIndex = index + 1;
			sscanf(subc.c_str(), "%d", &convert);
			if (convert >= 0 && convert < fieldConst) {
				field |= 1 << convert;
			}
		}
		return true;
		break;
	case 4:
		for (uint8_t i = 0; i < fieldConst; i++)
		{
			field |= 1 << i;
		}
		return true;
		break;
	default:
		sscanf(sub.c_str(), "%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			field |= 1 << convert;
			return true;
		}
		break;
	}
	return false;
}

bool CronHandler::setField(uint16_t& field, const String& sub, uint8_t fieldConst)
{
	int32_t convert = -1, rangeRight = -1;
	uint8_t index = 0, lastIndex = 0;
	field = 0;
	uint32_t type = this->checkSubType(sub);
	switch (type)
	{
	case 1:
		sscanf(sub.c_str(), "*/%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			for (uint8_t i = 0; i < fieldConst; i += convert)
			{
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 2:
		sscanf(sub.c_str(), "%d-%d", &convert, &rangeRight);
		if (convert >= 0 && rangeRight >= 0 && convert < fieldConst && rangeRight < fieldConst && convert <= rangeRight) {
			for (uint8_t i = convert; i <= rangeRight; i++) {
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 3:
		for (uint8_t i = 0; i <= this->numberOfCharInString(sub, ','); i++)
		{
			index = sub.indexOf(' ', lastIndex);
			String subc = sub.substring(lastIndex, index);
			lastIndex = index + 1;
			sscanf(subc.c_str(), "%d", &convert);
			if (convert >= 0 && convert < fieldConst) {
				field |= 1 << convert;
			}
		}
		return true;
		break;
	case 4:
		for (uint8_t i = 0; i < fieldConst; i++)
		{
			field |= 1 << i;
		}
		return true;
		break;
	default:
		sscanf(sub.c_str(), "%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			field |= 1 << convert;
			return true;
		}
		break;
	}
	return false;
}

bool CronHandler::setField(uint8_t& field, const String& sub, uint8_t fieldConst)
{
	int32_t convert = -1, rangeRight = -1;
	uint8_t index = 0, lastIndex = 0;
	field = 0;
	uint32_t type = this->checkSubType(sub);
	switch (type)
	{
	case 1:
		sscanf(sub.c_str(), "*/%d", &convert);
		if (convert >= 0 && convert < fieldConst) {			
			for (uint8_t i = 0; i < fieldConst; i += convert)
			{
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 2:
		sscanf(sub.c_str(), "%d-%d", &convert, &rangeRight);
		if (convert >= 0 && rangeRight >= 0 && convert < fieldConst && rangeRight < fieldConst && convert <= rangeRight) {
			for (uint8_t i = convert; i <= rangeRight; i++) {
				field |= 1 << i;
			}
			return true;
		}
		break;
	case 3:
		for (uint8_t i = 0; i <= this->numberOfCharInString(sub, ','); i++)
		{
			index = sub.indexOf(' ', lastIndex);
			String subc = sub.substring(lastIndex, index);
			lastIndex = index + 1;
			sscanf(subc.c_str(), "%d", &convert);
			if (convert >= 0 && convert < fieldConst) {
				field |= 1 << convert;
			}
		}
		return true;
		break;
	case 4:
		for (uint8_t i = 0; i < fieldConst; i++)
		{
			field |= 1 << i;
		}
		return true;
		break;
	default:
		sscanf(sub.c_str(), "%d", &convert);
		if (convert >= 0 && convert < fieldConst) {
			field |= 1 << convert;
			return true;
		}
		break;
	}
	return false;
}

size_t CronHandler::numberOfCharInString(const String& str, const char c)
{
	size_t index = 0, lastIndex = 0, number = 0;
	for (auto it = str.begin(); it != str.end(); it++)
	{
		if (*it == c) {
			number++;
		}
	}
	return number;
}

void CronHandler::calcNextActionTime(CronExpression* p, uint32_t unixTime)
{
	auto date = DateTime(unixTime + 1); // actual time + 1 sec
	uint32_t* p_min = (uint32_t*)&p->min;
	uint32_t* p_sec = (uint32_t*)&p->sec;
	while (true) {
		if (!(p->month & (1 << date.month()))) { // actual time + month days max - actual day + 1
			switch (date.month())
			{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				date = date + ((32 - date.day()) * DAY);
				continue;
			case 4:
			case 6:
			case 9:
			case 11:
				date = date + ((31 - date.day()) * DAY);
				continue;
			case 2:
				if (date.year() % 4 == 0) {
					date = date + ((30 - date.day()) * DAY);
				}
				else {
					date = date + ((29 - date.day()) * DAY);
				}
				continue;
			default:
				p->nextActionTime = 0;
				break;
			}
		}
		if (!((p->dayOfWeek & (1 << date.dayOfTheWeek())) && (p->day & (1 << date.day())))) {
			date = date + DAY; // actual time + 1 day
			continue;
		}
		if (!(p->hour & (1 << date.hour()))) {
			date = date + HOUR; // actual time + 1 hour
			continue;
		}
		//if (!(p->min & (1 << date.minute()))) {
		//	date = date + MIN; // actual time + 1 min
		//	continue;
		//}
		if (!((date.minute() < 32 && (p_min[0] & (1 << date.minute()))) || (date.minute() >= 32 && (p_min[1] & (1 << date.minute()))))) {
			date = date + MIN; // actual time + 1 min
			continue;
		}
		if (!((date.second() < 32 && (p_sec[0] & (1 << date.second()))) || (date.second() >= 32 && (p_sec[1] & (1 << date.second()))))) {
			date = date + SEC; // actual time + 1 sec
			continue;
		}
		//if (!(p->sec & (1 << date.second()))) {
		//	date = date + SEC; // actual time + 1 sec
		//	continue;
		//}
		break;
	}
	p->nextActionTime = date.unixtime();
}

bool CronHandler::setNewId(uint8_t oldId, uint8_t newId)
{
	for (uint8_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		if (this->list[i].id == oldId) {
			this->list[i].id = newId;
			return true;
		}
	}
	return false;
}

uint8_t CronHandler::numberOfSavedExpression()
{
	uint8_t num = 0;
	for (uint8_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		if (this->list[i].meta.bits.isSaved)
			num++;
	}
	return num;
}

void CronHandler::saveCron()
{
	File f = SPIFFS.open(F("/cron.txt"), "w");
	CronSaveLoad buff;
	CronExpression* p;
	f.write((uint8_t)(this->numberOfSavedExpression()));
	for (uint8_t i = 0; i < LENGTH_OF_VECTOR; i++)
	{
		p = &this->list[i];
		if (p->meta.bits.isSaved) {
			buff.day = p->day;
			buff.dayOfWeek = p->dayOfWeek;
			buff.funID = p->funID;
			buff.hour = p->hour;
			buff.id = p->id;
			buff.min = p->min;
			buff.month = p->month;
			buff.sec = p->sec;
			buff.meta = p->meta.header;
			f.write((uint8_t*)&buff, sizeof(buff));
		}
	}
	f.close();
}

CronHandler::CronHandler()
{
	this->rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
	uint8_t id = 1;
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		p->id = id++;
	}
}

CronHandler::CronHandler(const char* date, const char* time)
{
	this->rtc.begin(DateTime(date, time));
	uint8_t id = 1;
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		p->id = id++;
	}
}

void CronHandler::handle()
{
	auto unixTime = this->rtc.now().unixtime();
	CronExpression* p;
	size_t i;
	for (i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		if (p->meta.bits.isActive != 0 && p->meta.bits.isSetted != 0 && p->nextActionTime <= unixTime) {
			this->calcNextActionTime(p, unixTime);
			func_ptr[p->funID](p->param);
		}
	}
	CronSingle* p2;
	for (i = 0; i < SINGLE_LIST_LENGTH; i++) {
		p2 = &this->listSingle[i];
		if (p2->meta.bits.isActive && p2->executeTime <= unixTime) {
			func_ptr[p2->funID](p2->param);
			p2->meta.bits.isActive = 0;
		}
	}
}

uint8_t CronHandler::addAction(String& expression, FUNCTIONS_ID funID, void* param, bool save, bool executeAfterInit, bool enable)
{
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		if (!(p->meta.bits.isSetted)) {
			this->setExpressionFields(p, expression);
			p->funID = funID;
			p->param = param;
			p->meta.bits.isSetted = 1;
			p->meta.bits.isSaved = save;
			if (enable) {
				p->meta.bits.isActive = 1;
			}
			this->calcNextActionTime(p, this->rtc.now().unixtime());
			if (executeAfterInit) {
				func_ptr[p->id](p->param);
			}
			if (save) {
				this->saveCron();
			}
			return p->id;
		}
	}
	return 0;
}

uint8_t CronHandler::addAction(String& expression, FUNCTIONS_ID funID, uint8_t id, void* param, bool save, bool executeAfterInit, bool enable)
{
	uint8_t _id = this->addAction(expression, funID, param, false, executeAfterInit, enable);
	if (_id > 0) {
		this->setNewId(_id, id);
		return id;
	}
	else {
		return 0;
	}
	if (save) {
		this->saveCron();
	}
	return _id;
}

bool CronHandler::addSingleRunAction(FUNCTIONS_ID funID, uint32_t time, void* param)
{
	CronSingle* p;
	for (uint8_t i = 0; i < SINGLE_LIST_LENGTH; i++) {
		p = &this->listSingle[i];
		if (!p->meta.bits.isActive) {
			p->meta.bits.isActive = 1;
			p->funID = funID;
			p->param = param;
			p->executeTime = this->rtc.now().unixtime() + time;
			return true;
		}
	}
	return false;
}

bool CronHandler::removeAction(uint8_t id)
{
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		if (p->id == id) {
			uint8_t isSave = p->meta.bits.isSaved;
			p->meta.header = 0;
			if (isSave) {
				this->saveCron();
			}
			return true;
		}
	}
	return false;
}

bool CronHandler::enableAction(uint8_t id)
{
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		if (p->id == id) {
			p->meta.bits.isActive = 1;
			return true;
		}
	}
	return false;
}

bool CronHandler::disableAction(uint8_t id)
{
	CronExpression* p;
	for (size_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		p = &this->list[i];
		if (p->id == id) {
			p->meta.bits.isActive = 0;
			return true;
		}
	}
	return false;
}

CronExpression* CronHandler::getCron(uint8_t id)
{
	for (uint8_t i = 0; i < LENGTH_OF_VECTOR; i++) {
		if (this->list[i].id == id) {
			return &this->list[i];
		}
	}
	return nullptr;
}

CronExpression* CronHandler::getAllCron()
{
	return this->list;
}

void CronHandler::loadCron()
{
	File f;
	if (SPIFFS.exists(F("/cron.txt"))) {
		f = SPIFFS.open(F("/cron.txt"), "r");
		CronSaveLoad buff;
		CronExpression* p;
		uint8_t len;
		f.read(&len, 1);
		for (uint8_t i = 0; i < len; i++)
		{
			p = &this->list[i];
			f.read((uint8_t*)&buff, sizeof(buff));
			p->day = buff.day;
			p->dayOfWeek = buff.dayOfWeek;
			p->funID = buff.funID;
			p->hour = buff.hour;
			p->id = buff.id;
			p->min = buff.min;
			p->month = buff.month;
			p->sec = buff.sec;
			p->meta.header = buff.meta;
		}
	}
	else {
		f = SPIFFS.open(F("/cron.txt"), "w");
		f.write((uint8_t)0);
	}
	f.close();
}

void CronHandler::removeAllSaved()
{
	if (SPIFFS.exists(F("/cron.txt"))) {
		SPIFFS.remove(F("/cron.txt"));
	}
}

void CronHandler::adjustRTC(DateTime& dt)
{
	this->rtc.adjust(dt);
}

uint32_t CronHandler::getTime()
{
	return this->rtc.now().unixtime();
}
