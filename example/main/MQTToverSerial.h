#ifndef MQTTOVERSERIAL
#define MQTTOVERSERIAL

#include <WiFi.h>
#include "PubSubClient.h"

class MQTToverSerial
{
public:
    MQTToverSerial(WiFiClient& _wifiClient, HardwareSerial &_serial, const char* _serverIp, int _serverPort, const char* _MQTTlogin, const char* _MQTTpass, const char* _MQTTid, HardwareSerial* _debugSerial = NULL);
    ~MQTToverSerial();
    bool SubscribeTopic(const char* topic);
    bool UnsubscribeTopic(const char* topic);
    bool Publish(const char* topic, const char* message);
    bool Loop();

    void SetSpecialCharacter(char _specialCharacter) { specialCharacter = _specialCharacter; };
    char GetSpecialCharacter() { return specialCharacter; };

    void ReadSerial();
private:
    PubSubClient* pubSubClient = NULL;
    HardwareSerial &serial;
    HardwareSerial* debugSerial;
    WiFiClient &wifiClient;
    const char* &serverIp;
    int &serverPort;
    
    const char* &MQTTlogin;
    const char* &MQTTpass;
    const char* &MQTTid;

    char specialCharacter = '$';

    void Reconnect();

    void Error(const char* buff);
};

#endif
