#pragma once

#include <stdint.h>

#include <Crypto.h>
#include <ChaCha.h>

#include <CRC32.h>
#include "CronHandler.h"

#define INCOMING_ESP_BUFFER_LENGTH 10
#define OUTGOING_ESP_BUFFER_LENGTH 5
#define MAX_VALUE_LENGTH 28
#define MESSAGE_MIN_SIZE 26 // 2*6 MAC, 1 message type, 8 cript, 4 crc, 1 length
#define OUTGOING_NEXT_SEND_TIME 1
#define OUTGOING_MAX_RESEND 1

extern CronHandler cron;

union MessType {
	uint8_t type;
	struct {
		uint8_t type : 7;
		uint8_t needConfirmation : 1;
	} bits;
};
enum EspMessageType {
	ServerMessage = 1,
	ConnectRequest,
	ConnectAccepted,
	ConnectDenied,
	ConnectRequired,
	MessageOK,
	TempSensor

	//max 127
};
typedef struct {
	uint8_t Key[4];
	uint8_t Value[4];
} EspDictionary;
typedef struct {
	uint8_t Cryp[8];
	uint8_t SenderMAC[6];
	uint8_t ReceiverMAC[6];
	uint8_t MessageLength;
	uint8_t CRC32[4];
	MessType Type;
	EspDictionary Dictionarys[MAX_VALUE_LENGTH];
} EspMessage;

typedef struct {
	bool IsActive = false;
	EspMessage message;
} IncomingEspMessage;
typedef struct {
	bool IsActive = false;
	EspMessage message;
	uint8_t sendNum = 0;
	uint32_t nextSendTime;
	uint32_t crc32;
} OutgoingEspMessage;

extern OutgoingEspMessage outMess[OUTGOING_ESP_BUFFER_LENGTH];

uint32_t convertDataTo32Bit(uint8_t* p);
void convert32BitToTransfer(uint32_t p, uint8_t* place);

class EspMessageHandler {
	EspMessage message;
	uint8_t getDictionaryLength();
	void calcCRC();
public:
	EspMessageHandler();
	void setMessageType(EspMessageType type, bool needConfirm = false);
	bool setKeyValue(uint32_t key, uint32_t value);
	bool setCustomValue(uint32_t key, uint32_t length, void* p);
	void setSenderMac(uint8_t* mac, uint8_t* macID = nullptr);
	void setReceiverMac(uint8_t* mac, uint8_t* macID = nullptr);
	void reset();
	const uint8_t* getMessage(bool encrpyt = true);
	uint8_t getMessageLength();

	static bool saveToOutmess(EspMessage* messp, uint32_t crc);
	static uint32_t calcCRC32(EspMessage* messp);
	static void encryptMessage(EspMessage* messp);
	static void decryptMessage(EspMessage* messp);
	static uint8_t getMessageLen(EspMessage* messp);
};