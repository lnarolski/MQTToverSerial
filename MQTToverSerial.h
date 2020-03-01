#ifndef MQTTOVERSERIAL
#define MQTTOVERSERIAL

#include <WiFi.h>
#include <PubSubClient.h>

// In this project I am using "The Standard Template Library (STL) for AVR with C++ streams" (https://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/) so you also have to download and install this
// Remeber to include in your project:
// #include <WiFi.h>

class MQTToverSerial
{
public:
    MQTToverSerial(WiFiClient& _wifiClient, HardwareSerial &_serial, const char* _serverIp, int _serverPort, const char* _MQTTlogin, const char* _MQTTpass, const char* _MQQTid);
    ~MQTToverSerial();
    bool SubscribeTopic(const char* topic);
    bool UnsubscribeTopic(const char* topic);
    bool Publish(const char* topic, const char* message);
    bool IsConnected();
    bool Loop();

    void SetSpecialCharacter(char _specialCharacter) { specialCharacter = _specialCharacter; };
    char GetSpecialCharacter() {return specialCharacter;};

    void ReadSerial();
private:
    PubSubClient* pubSubClient;
    HardwareSerial &serial;
    WiFiClient &wifiClient;
    const char* &serverIp;
    int &serverPort;
    
    const char* &MQTTlogin;
    const char* &MQTTpass;
    const char* &MQTTid;

    bool connectedToBroker = false;

    char specialCharacter = '$';

    void Reconnect();
};

#endif
