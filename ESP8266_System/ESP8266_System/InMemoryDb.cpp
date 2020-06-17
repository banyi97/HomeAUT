#include "InMemoryDb.h"

void InMemoryDb::Save(uint32_t data, uint32_t time){
	uint8_t index = this->index++;
	if (this->index >= MAX_SAVED_INMEMORY_DATA) {
		this->index = 0;
	}
	if (this->numbersOfValidData < MAX_SAVED_INMEMORY_DATA) {
		this->numbersOfValidData++;
	}
	this->datas[index].Data = data;
	this->datas[index].Time = time;
}

InMemoryData* InMemoryDb::GetData(uint8_t index){
	if (index >= MAX_SAVED_INMEMORY_DATA) {
		return nullptr;
	}
	return &this->datas[index];
}

InMemoryData* InMemoryDb::GetDatas(uint8_t* validDataNum){
	*validDataNum = this->numbersOfValidData;
	return this->datas;
}

void InMemoryDb::Reset()
{
	this->index = 0;
	this->numbersOfValidData = 0;
}
