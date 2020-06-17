#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WifiEspNow.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoJson.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#include "DebugPrint.h"

#include "WifiHandler.h"
#include "EspMessage.h"
#include "CronHandler.h"
#include "ClientHandler.h"
#include "IncomingMessageHandler.h"
#include "MessageKeyTypes.h"

#define IS_CLIENT 0
#define IS_SERVER 1

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WifiHandler handler;
CronHandler cron;
IncomingEspMessage messages[INCOMING_ESP_BUFFER_LENGTH];
OutgoingEspMessage outMess[OUTGOING_ESP_BUFFER_LENGTH];
ClientHandler clients;

void testFunc(void* param) {
    PRINTLN("periodic message");
}
void testFunc2(void* param) {
    PRINTLN("periodic message2");
}
void sendRandomDatasWithWs(void* param) {
    StaticJsonDocument<256> doc;
    doc["deviceId"] = "01:23:45:67:89:01";
    doc["data"] = random(100);
    doc["labelData"] = random(100);
    String str;
    serializeJson(doc, str);
    webSocket.broadcastTXT(str);
}
void restartAction(void* param) {
    ESP.restart();
}
void sendEspMess(void* param) {
    PRINTLN("Send esp message");
    EspMessageHandler mess;
    mess.setReceiverMac(handler.getServerMAC());
    mess.setSenderMac(handler.getMAC());
    mess.setKeyValue(19957, 1294634);
    mess.setMessageType(EspMessageType::ServerMessage);
    WifiEspNow.send(handler.getServerMAC(), mess.getMessage(), mess.getMessageLength());
}
void tryConnectToServer(void* param) {
    PRINTLN("Try connect to server");
    NetworkMode mode;
    mode.mode = handler.getMode();
    if (mode.bits.operationMode == SERVER_MODE || handler.getClientMode() == ClientConnectState::Accepted)
        return;
    EspMessageHandler mess;
    mess.setReceiverMac(handler.getServerMAC());
    mess.setMessageType(EspMessageType::ConnectRequest);
    mess.setKeyValue(DefaultKeys::SetType, DeviceTypes::Thermometer);
    uint8_t* mac = handler.getMAC();
    if (mac != nullptr && *mac != 0) {
        PRINTF("MAC: %d\r\n", *mac);
        mess.setSenderMac(handler.getServerMAC(), mac);
        mess.setKeyValue(DefaultKeys::MAC_Exist, *mac);
        mess.setCustomValue(DefaultKeys::MAC_Default, 6, handler.getDefMAC());
    }
    else {
        PRINTLN("MAC null");
        mess.setSenderMac(handler.getDefMAC());
    }
    WifiEspNow.send(handler.getServerMAC(), mess.getMessage(), mess.getMessageLength());
    switch (handler.getClientMode())
    {
    case ClientConnectState::NotConnected:

        cron.addSingleRunAction(FUNCTIONS_ID::tryConnect, 10); // 10s
        break;
    case ClientConnectState::Denieded:
        cron.addSingleRunAction(FUNCTIONS_ID::tryConnect, 300); // 5min
        break;
    default:
        break;
    }
}
void testThermometer(void* param) {
    PRINTLN("Test thermometer");
    NetworkMode mode;
    mode.mode = handler.getMode();
    if (mode.bits.operationMode == SERVER_MODE || handler.getClientMode() != ClientConnectState::Accepted)
        return;
    DateTime time = cron.getTime();
    //PRINTF("Time: %d:%d:%d\r\n", time.hour(), time.minute(), time.second());
    TempsensorData tsd;
    tsd.datas.temp = random(255);
    tsd.datas.hum = random(100);
    EspMessageHandler mess;
    mess.setSenderMac(handler.getServerMAC(), handler.getMAC());
    mess.setReceiverMac(handler.getServerMAC());
    mess.setMessageType(EspMessageType::TempSensor, true);
    mess.setKeyValue(TempsensorKeys::SetData, tsd.rawData);
    WifiEspNow.send(handler.getServerMAC(), mess.getMessage(), mess.getMessageLength());
}

void (*func_ptr[])(void* param) = {
    testFunc,
    testFunc2,
    sendRandomDatasWithWs,
    sendEspMess,
    restartAction,
    tryConnectToServer,
    testThermometer,
};

void socketHandler(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

    switch (type) {
    case WStype_DISCONNECTED:
        PRINTF("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(num);
        PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
        PRINTF("[%u] get Text: %s\n", num, payload);

        // send message to client
        webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
        break;
    case WStype_BIN:
        PRINTF("[%u] get binary length: %u\n", num, length);
        hexdump(payload, length);

        // send message to client
        webSocket.sendBIN(num, payload, length);
        break;
    }
}

void setup(void) {
    //SPIFFS.format();
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(true);
    SPIFFS.begin();
    if (!handler.isNetworkConfigFileAvailable()) {
        handler.createNetworkConfigFile();
    }

    //handler.removeNetworkConfigFile();
    //handler.createNetworkConfigFile();
    //handler.setOpMode(CLIENT_MODE);
    //handler.setWifiMode(AP_MODE); 
    //handler.setSTA("ASUS", "rv4xkydf_BME", 0);

    handler.begin();
    //cron.loadCron();
    NetworkMode mode;
    mode.mode = handler.getMode();
    PRINTLN("mode");
    PRINTLN(mode.mode);
    if (mode.bits.operationMode == SERVER_MODE) {
        PRINTLN("SERVERMODE");
        if (MDNS.begin(handler.getDeviceName()))
            PRINTLN(handler.getDeviceName());
        PRINTLN("MDNS responder started");
    }
    else if (mode.bits.operationMode == CLIENT_MODE) {
        PRINTLN("CLIENTMODE");
        cron.addSingleRunAction(FUNCTIONS_ID::tryConnect);
        //String exp2 = "*/2 * * * * *";
        //cron.addAction(exp2, FUNCTIONS_ID::EspNowSender);
    }

    server.on("/api/heap", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
        });

    server.on("/api/netconf", HTTP_GET, [](AsyncWebServerRequest* request) {
        File file = SPIFFS.open(F("/netconfig.json"), "r");
        request->send(200, "application/json", file.readString());
        file.close();
        });

    server.on("/api/setMode", HTTP_POST, [](AsyncWebServerRequest* request) {
        StaticJsonDocument<MAX_STATIC_LENGTH> doc;
        String body = request->arg("body");
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            request->send(400, "text/plain", "parse fail");
            PRINTLN("Parse error");
            return;
        }
        if (doc.containsKey("mode")) {
            uint8_t mode = doc["mode"].as<uint8_t>();
            PRINTLN(mode);
            if (mode == SERVER_MODE) {
                handler.setOpMode(SERVER_MODE);

                request->send(200, "text/plain", String("ok server mode"));
                return;
            }
            else if (mode == CLIENT_MODE) {
                handler.setOpMode(CLIENT_MODE);

                request->send(200, "text/plain", String("ok client mode"));
                return;
            }
            else {
                request->send(400, "text/plain", String("not supported mode"));
                return;
            }
        }
        request->send(400, "text/plain", String("not contains mode"));
        });

    server.on("/api/setWifi", HTTP_POST, [](AsyncWebServerRequest* request) {
        StaticJsonDocument<MAX_STATIC_LENGTH> doc;

        String body = request->arg("body");
        PRINTLN(body);
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            request->send(400, "text/plain", "parse fail");
            PRINTLN("Parse error");
            return;
        }
        if (doc.containsKey("wifiMode")) {
            uint8_t mode = doc["wifiMode"].as<uint8_t>();
            StaticJsonDocument<MAX_STATIC_LENGTH> file;
            handler.loadNetworkConfigFile(file);
            if (mode == 1) { // STA
                NetworkMode netMode;
                netMode.mode = doc["m"].as<uint8_t>();
                netMode.bits.wifi = mode;
                file["m"] = netMode.mode;
                for (uint8_t i = 0; i < 4; i++) {
                    file["STA"][i]["s"] = doc["param"][i]["ssid"];
                    file["STA"][i]["p"] = doc["param"][i]["password"];
                }
                handler.saveNetworkConfigFile(file);
                request->send(200, "text/plain", String("ok sta mode"));
                return;
            }
            else if (mode == 2) { // AP
                NetworkMode netMode;
                netMode.mode = doc["m"].as<uint8_t>();
                netMode.bits.wifi = mode;
                file["m"] = netMode.mode;
                file["AP"]["s"] = doc["param"]["ssid"];
                file["AP"]["p"] = doc["param"]["password"];
                handler.saveNetworkConfigFile(file);
                request->send(200, "text/plain", String("ok ap mode"));
                return;
            }
            else {
                request->send(400, "text/plain", String("not supported wifimode"));
                return;
            }
        }
        request->send(400, "text/plain", String("not contains wifimode"));
        });

    server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest* request) {
        cron.addSingleRunAction(FUNCTIONS_ID::restart);
        request->send(200, "text/plain", String("restarted"));
        });

    server.onNotFound([](AsyncWebServerRequest* request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        }
        else {
            request->send(404);
        }
        });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*"); // CORS
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS, PUT, DELETE");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Content-Length, Accept-Encoding, X-CSRF-Token, Authorization");
    //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");;
    String exp1 = "* * * * * *";
    String exp2 = "*/5 * * * * *";
    //cron.removeAllSaved();
    //cron.addAction(exp1, FUNCTIONS_ID::restart, nullptr, false, false, false);
    //cron.addAction(exp1, FUNCTIONS_ID::test, nullptr, true);
    //cron.addAction(exp2, FUNCTIONS_ID::EspNowSender);
    cron.addAction(exp2, FUNCTIONS_ID::testThermo, TEST_THERMO_ID, nullptr, false, false, false);
    server.begin();
    webSocket.begin();
    webSocket.onEvent(socketHandler);

    uint8_t* p = handler.getDefSTA_MAC();
    PRINTF("DEF STA: %x:%x:%x:%x:%x:%x\r\n", p[0], p[1], p[02], p[3], p[4], p[5]);
    p = handler.getDefAP_MAC();
    PRINTF("DEF  AP: %x:%x:%x:%x:%x:%x\r\n", p[0], p[1], p[02], p[3], p[4], p[5]);
    p = handler.getServerMAC();
    PRINTF("DEF  SERVER: %x:%x:%x:%x:%x:%x\r\n", p[0], p[1], p[02], p[3], p[4], p[5]);
    PRINTF("STA: ");
    PRINTLN(WiFi.macAddress());
    PRINTF("AP: ");
    PRINTLN(WiFi.softAPmacAddress());

    if (!WifiEspNow.begin()) {
        PRINTLN("WifiEspNow.begin() failed");
        ESP.restart();
    }
    WifiEspNow.onReceive(printReceivedMessage, nullptr);
    PRINTLN("ESP NOW RUN");

    PRINTLN("START");
}

void loop(void) {
    MDNS.update();
    webSocket.loop();
    cron.handle();

    // handle incoming message
    EspMessage* p;
    for (uint8_t i = 0; i < INCOMING_ESP_BUFFER_LENGTH; i++) {
        if (messages[i].IsActive) {
            p = &(messages[i].message);
            // handle
            switch (p->Type.bits.type)
            {
            case EspMessageType::ConnectRequest:
                handleConnectRequest(p);
                break;
            case EspMessageType::ConnectAccepted:
                handleConnectAccepted(p);
                break;
            case EspMessageType::ConnectDenied:
                handleConnectDenied(p);
                break;
            case EspMessageType::TempSensor:
                handleTempSensor(p);
                break;
            case EspMessageType::ConnectRequired:
                handleConnectRequired(p);
                break;
            case EspMessageType::MessageOK:
                PRINTLN("Confirm arraived");
                for (uint8_t j = 0; j < OUTGOING_ESP_BUFFER_LENGTH; j++) {
                    if (outMess[j].IsActive && convertDataTo32Bit(p->Dictionarys[0].Key) == DefaultKeys::MessageCRC && outMess[j].crc32 == convertDataTo32Bit(p->Dictionarys[0].Value)) {
                        outMess[j].IsActive = false;
                        break;
                    }
                }
                break;
            default:
                break;
            }
            messages[i].IsActive = false;
        }
    }
    uint32_t time = cron.getTime();
    for (uint8_t i = 0; i < OUTGOING_ESP_BUFFER_LENGTH; i++) {
        if (outMess[i].IsActive && outMess[i].nextSendTime <= time) {
            outMess[i].nextSendTime += OUTGOING_NEXT_SEND_TIME;
            if (outMess[i].sendNum++ > OUTGOING_MAX_RESEND) {
                outMess[i].IsActive = false;
            }
            else {
                PRINTLN("Send again");
                WifiEspNow.send(outMess[i].message.ReceiverMAC, outMess[i].message.Cryp, EspMessageHandler::getMessageLen(&outMess[i].message));
            }
        }
    }
}

void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
    PRINTF("Message from %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    EspMessage* message;
    message = (EspMessage*)buf;
    EspMessageHandler::decryptMessage(message);
    if (convertDataTo32Bit(message->CRC32) == EspMessageHandler::calcCRC32(message)) {
        for (uint8_t i = 0; i < INCOMING_ESP_BUFFER_LENGTH; i++) {
            if (!messages[i].IsActive) {
                messages[i].IsActive = true;
                memcpy(&messages[i].message, message, EspMessageHandler::getMessageLen(message));
                return;
            }
        }
    }
}