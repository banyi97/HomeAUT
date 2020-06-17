#include "EspMessage.h"

uint32_t convertDataTo32Bit(uint8_t* p) {
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

void convert32BitToTransfer(uint32_t p, uint8_t* place) {
	place[3] = (uint8_t)(p);
	place[2] = (uint8_t)(p >> 8);
	place[1] = (uint8_t)(p >> 16);
	place[0] = (uint8_t)(p >> 24);
}

uint8_t EspMessageHandler::getDictionaryLength()
{
	return this->message.MessageLength * 8; // key 4 byte + value 4 byte
}

void EspMessageHandler::calcCRC() {
	CRC32 crc;
	crc.update(this->message.SenderMAC);
	crc.update(this->message.ReceiverMAC);
	crc.update(this->message.MessageLength);
	crc.update(this->message.Type.type);
	crc.update((uint8_t*)this->message.Dictionarys, this->getDictionaryLength());
	convert32BitToTransfer(crc.finalize(), this->message.CRC32);
}

EspMessageHandler::EspMessageHandler() {
	this->reset();
}

void EspMessageHandler::setMessageType(EspMessageType type, bool needConfirm)
{
	this->message.Type.bits.type = type;
	this->message.Type.bits.needConfirmation = needConfirm;
}

bool EspMessageHandler::setKeyValue(uint32_t key, uint32_t value)
{
	if (this->message.MessageLength + 1 >= MAX_VALUE_LENGTH) {
		return false;
	}
	convert32BitToTransfer(key, this->message.Dictionarys[this->message.MessageLength].Key);
	convert32BitToTransfer(value, this->message.Dictionarys[this->message.MessageLength].Value);
	this->message.MessageLength++;
	return  true;
}

bool EspMessageHandler::setCustomValue(uint32_t key, uint32_t length, void* p)
{
	uint8_t len = length / 8; 
	if (length % 8 != 0) {
		len++;
	}
	if (this->message.MessageLength + len + 1>= MAX_VALUE_LENGTH) {
		return false;
	}
	this->setKeyValue(key, length);
	uint8_t* place = this->message.Dictionarys[this->message.MessageLength].Key;
	memcpy(place, p, length);
	this->message.MessageLength += len; 
	return true;
}

void EspMessageHandler::setSenderMac(uint8_t* mac, uint8_t* macID) {
	for(uint8_t i = 0; i < 6; i++)
		this->message.SenderMAC[i] = mac[i];
	if (macID != nullptr) {
		this->message.SenderMAC[5] = *macID;
	}
}

void EspMessageHandler::setReceiverMac(uint8_t* mac, uint8_t* macID) {
	for (uint8_t i = 0; i < 6; i++)
		this->message.ReceiverMAC[i] = mac[i];
	if (macID != nullptr) {
		this->message.ReceiverMAC[5] = *macID;
	}
}

void EspMessageHandler::reset() {
	this->message.MessageLength = 0;
}

const uint8_t* EspMessageHandler::getMessage(bool encrpyt)
{
	this->calcCRC();
	MessType type;
	type.type = this->message.Type.type;
	uint32_t crc = convertDataTo32Bit(this->message.CRC32);
	if (encrpyt) {
		EspMessageHandler::encryptMessage(&this->message);
	}
	if (type.bits.needConfirmation) {
		EspMessageHandler::saveToOutmess(&this->message, crc);
	}
	return (uint8_t*)&this->message;
}

uint8_t EspMessageHandler::getMessageLength()
{
	uint8_t len = MESSAGE_MIN_SIZE;
	return len + this->getDictionaryLength();
}

bool EspMessageHandler::saveToOutmess(EspMessage* messp, uint32_t crc)
{
	for (uint8_t i = 0; i < OUTGOING_ESP_BUFFER_LENGTH; i++) {
		if (!outMess[i].IsActive) {
			outMess[i].IsActive = true;
			outMess[i].crc32 = crc;
			outMess[i].sendNum = 0;
			outMess[i].nextSendTime = cron.getTime() + OUTGOING_NEXT_SEND_TIME;
			memcpy(&outMess[i].message, messp, EspMessageHandler::getMessageLen(messp));
			return true;
		}
	}
	return false;
}

uint32_t EspMessageHandler::calcCRC32(EspMessage* messp)
{
	CRC32 crc;
	crc.update(messp->SenderMAC);
	crc.update(messp->ReceiverMAC);
	crc.update(messp->MessageLength);
	crc.update(messp->Type);
	crc.update((uint8_t*)messp->Dictionarys, messp->MessageLength * 8);
	return crc.finalize();
}

void EspMessageHandler::encryptMessage(EspMessage* messp)
{
	ChaCha chacha;
	uint8_t cha_key[]{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint8_t cha_counter[]{ 109, 110, 111, 112, 113, 114, 115, 116 };
	// add iv key
	//uint8_t cha_iv[]{ 101,102,103,104,105,106,107,108 }; // static key
	for (uint8_t i = 0; i < 8; i++) {
		messp->Cryp[i] = random(255);
	}
	chacha.setKey(cha_key, sizeof(cha_key));
	chacha.setCounter(cha_counter, sizeof(cha_counter));
	chacha.setIV(messp->Cryp, sizeof(messp->Cryp));
	chacha.encrypt(messp->CRC32, messp->CRC32, 5 + 8 * messp->MessageLength);
}

void EspMessageHandler::decryptMessage(EspMessage* messp)
{
	ChaCha chacha;
	uint8_t cha_key[]{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint8_t cha_counter[]{ 109, 110, 111, 112, 113, 114, 115, 116 };
	chacha.setKey(cha_key, sizeof(cha_key));
	chacha.setCounter(cha_counter, sizeof(cha_counter));
	chacha.setIV(messp->Cryp, sizeof(messp->Cryp));
	chacha.decrypt(messp->CRC32, messp->CRC32, 5 + 8 * messp->MessageLength);
}

uint8_t EspMessageHandler::getMessageLen(EspMessage* messp)
{
	return MESSAGE_MIN_SIZE + messp->MessageLength * 8;
}
