#include "IncomingMessageHandler.h"

void handleConnectRequest(EspMessage* p)
{   // [0] - type, [1] - old mac, [2] - key len, [3] - def mac
    PRINTLN("handleConnectRequest");
    EspMessageHandler mess;
    mess.setSenderMac(handler.getServerMAC());
    mess.setReceiverMac(p->SenderMAC);
    uint8_t mac = 0;
    PRINTF("Sender mac: %x:%x:%x:%x:%x:%x\r\n", p->SenderMAC[0], p->SenderMAC[1], p->SenderMAC[2], p->SenderMAC[3], p->SenderMAC[4], p->SenderMAC[5]);
    if (p->MessageLength >= 4) {
        mac = convertDataTo32Bit(p->Dictionarys[1].Value);
        PRINTLN("MAC exist");
    }
    if (clients.isElement(p->SenderMAC, &mac) || clients.getActiveClientsNumber() < MAX_NUMBER_OF_CLIENTS) {
        PRINTLN("elemenet");
        mess.setMessageType(EspMessageType::ConnectAccepted);
        mess.setKeyValue(DefaultKeys::SetMAC, mac);
        clients.addClient(mac);
        EspClient* cl = clients.getClient(mac);
        cl->updateAction();
        if (p->MessageLength >= 4) {
            uint8_t defMAC[6];
            memcpy(defMAC, p->Dictionarys[3].Key, 6);
            cl->setDefMAC(p->Dictionarys[3].Key);
            PRINTF("Sender mac: %x:%x:%x:%x:%x:%x\r\n", defMAC[0], defMAC[1], defMAC[2], defMAC[3], defMAC[4], defMAC[5]);
        }
        else {
            cl->setDefMAC(p->SenderMAC);
        }
        if (p->MessageLength >= 1) {
            cl->setType(convertDataTo32Bit(p->Dictionarys[0].Value));
        }
        StaticJsonDocument<256> doc;
        doc["deviceId"] = mac;
        doc["action"] = "connect";
        doc["deviceType"] = cl->getType();
        String str;
        serializeJson(doc, str);
        webSocket.broadcastTXT(str);
        WifiEspNow.send(p->SenderMAC, mess.getMessage(), mess.getMessageLength());
    }
    else {
        PRINTLN("No more slote");
        mess.setMessageType(EspMessageType::ConnectDenied);
        WifiEspNow.send(p->SenderMAC, mess.getMessage(), mess.getMessageLength());
    }
}

void handleConnectAccepted(EspMessage* p)
{
    PRINTLN("handleConnectAccepted");
    if (p->MessageLength > 0 && convertDataTo32Bit(p->Dictionarys[0].Key) == DefaultKeys::SetMAC) {
        handler.setMAC((uint8_t)convertDataTo32Bit(p->Dictionarys[0].Value));
        handler.clientModeSet(ClientConnectState::Accepted);
        cron.enableAction(TEST_THERMO_ID);
    }
}

void handleConnectDenied(EspMessage* p)
{
    PRINTLN("handleConnectDenied");
    handler.clientModeSet(ClientConnectState::Denieded);
}

void handleConnectRequired(EspMessage* p)
{
    PRINTLN("handleConnectRequired");
    handler.clientModeSet(ClientConnectState::NotConnected);
    //handler.resetMAC();
    cron.addSingleRunAction(FUNCTIONS_ID::tryConnect);
    cron.disableAction(TEST_THERMO_ID);
}

void handleTempSensor(EspMessage* p)
{
    PRINTLN("handleTempSensor");
    EspClient* client = clients.isConnectedClient(p->SenderMAC);
    if (client != nullptr) {
        PRINTLN("Connected client");
        client->updateAction();
        TempsensorData tsd;
        StaticJsonDocument<256> doc; 
        String str;
        for (uint8_t i = 0; i < p->MessageLength; i++) {
            switch (convertDataTo32Bit(p->Dictionarys[i].Key))
            {
            case TempsensorKeys::SetData:
                tsd.rawData = convertDataTo32Bit(p->Dictionarys[i].Value);
                PRINTF("Temp: %d, Hum: %d\r\n", tsd.datas.temp, tsd.datas.hum);
                doc["deviceId"] = p->SenderMAC[5];
                doc["action"] = "setData";
                doc["data"]["temp"] = tsd.datas.temp;
                doc["data"]["hum"] = tsd.datas.hum;
                serializeJson(doc, str);
                webSocket.broadcastTXT(str);
                client->SaveData(tsd.rawData, cron.getTime());
                break;
            default:
                break;
            }
        }
       /* uint8_t validData = 0;
        auto datas = client->GetDatas(&validData);
        PRINTF("Valid datas: %d", validData);
        for (uint8_t i = 0; i < validData; i++) {
            tsd.rawData = datas[i].Data;
            PRINTF("Index:%d Temp: %d C\r\n", i, tsd.datas.temp);
        }*/
        if (p->Type.bits.needConfirmation) {
            EspMessageHandler mess;
            mess.setSenderMac(handler.getServerMAC());
            mess.setReceiverMac(p->SenderMAC);
            mess.setMessageType(EspMessageType::MessageOK);
            mess.setKeyValue(DefaultKeys::MessageCRC, convertDataTo32Bit(p->CRC32));
            WifiEspNow.send(p->SenderMAC, mess.getMessage(), mess.getMessageLength());
        }
    }
    else {
        PRINTLN("Not connected client");
        EspMessageHandler mess;
        mess.setSenderMac(handler.getServerMAC());
        mess.setReceiverMac(p->SenderMAC);
        mess.setMessageType(EspMessageType::ConnectRequired);
        WifiEspNow.send(p->SenderMAC, mess.getMessage(), mess.getMessageLength());
    }
}
