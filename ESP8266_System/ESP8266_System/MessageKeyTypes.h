#pragma once
#include "stdint.h"

enum DefaultKeys {
	SetMAC = 1,
	SetType,
	SetDeviceName,
	MessageCRC,
	MAC_Exist,
	MAC_Default
};

enum DeviceTypes {
	Server = 1,
	Thermometer,
	Camera,
	Doorbell,
	Thermostat
};

union TempsensorData {
	uint32_t rawData;
	struct {
		uint32_t temp : 16;
		uint32_t hum : 16;
	} datas;
};

enum TempsensorKeys {
	SetData = 2000,
	GetData,

};

enum ClientConnectState {
	NotConnected,
	Accepted,
	Denieded
};