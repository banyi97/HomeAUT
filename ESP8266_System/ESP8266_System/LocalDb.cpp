#include "LocalDb.h"

void LocalDb::Save(uint8_t id, String& data){
	this->Save(id, data, 0);
}

void LocalDb::Save(uint8_t id, String& data, uint32_t unixTime){
	File f;
	String filepath = "/db/" + String(id);
	if (SPIFFS.exists(filepath)) {
		f = SPIFFS.open(filepath, "r+");
		uint8_t index = f.readStringUntil('\n').toInt();
		uint8_t nextIndex = index++;
		if (nextIndex >= MAX_LINE_OF_DATA)
			nextIndex = 0;
		f.seek(0);
		f.println(nextIndex);
		for (uint8_t i = 0; i < index; i++) {
			f.readStringUntil('\n');
		}
	}
	else {
		f = SPIFFS.open(filepath, "w");
		f.println(1);
	}
	f.println(String(unixTime) + ":" + data);
	f.close();
}

bool LocalDb::GetAll(uint8_t id, String& place){
	String filepath = "/db/" + String(id);
	if (SPIFFS.exists(filepath)) {
		File f = SPIFFS.open(filepath, "r");
		place = f.readString();
		f.close();
		return true;
	}
	place = "";
	return false;
}
