//#include "DB_Handler.h"
//
//DB_Handler::DB_Handler(String& filename)
//{
//	this->fileName = filename;
//	File file;
//	if (SPIFFS.exists("/db/" + filename)) {
//		file = SPIFFS.open(this->fileName, "r");
//		this->fileIsValid = 1;
//		String next = file.readStringUntil('/n');
//		this->nextIndex = next.toInt();
//		if (this->nextIndex >= MAX_DATA) {
//			this->nextIndex = 0;
//		}
//	}
//	else {
//		file = SPIFFS.open("/db/" + this->fileName, "w");
//		file.println(this->nextIndex);		
//	}
//	file.close();
//}
//
//void DB_Handler::saveData(String& data, uint32_t unixTime)
//{
//	if (this->fileIsValid) {
//		File file = SPIFFS.open("/db/"+this->fileName, "r");
//		if (file) {		
//			uint8_t actual = this->nextIndex++;
//			if (this->nextIndex >= MAX_DATA)
//				this->nextIndex = 0;
//			file.println(this->nextIndex);
//			for (uint8_t i = 0; i < actual; i++) {
//				file.readStringUntil('/n');
//			}
//			file.println(String(unixTime) + ":" + data);
//			file.close();
//		}
//		else {
//			this->fileIsValid = 0;
//		}
//	}
//}
//
//void DB_Handler::loadData(String& data)
//{
//	if (this->fileIsValid) {
//		File file = SPIFFS.open("/db/"+this->fileName, "r");
//		if (file) {
//			data = file.readString();
//		}
//		else {
//			this->fileIsValid = 0;
//			data = "";
//		}
//		file.close();
//	}
//}
