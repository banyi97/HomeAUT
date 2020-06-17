#pragma once
#include <WifiEspNow.h>
#include "EspMessage.h"
#include "ClientHandler.h"
#include "WifiHandler.h"
#include "stdint.h"
#include "CronHandler.h"
#include "DebugPrint.h"
#include "MessageKeyTypes.h"
#include <WebSocketsServer.h>

extern ClientHandler clients;
extern WifiHandler handler;
extern CronHandler cron;
extern WebSocketsServer webSocket;

void handleConnectRequest(EspMessage* p);
void handleConnectAccepted(EspMessage* p);
void handleConnectDenied(EspMessage* p);
void handleConnectRequired(EspMessage* p);
void handleTempSensor(EspMessage* p);
