#include "MQTToverSerial.h"

void Test(char* topic, byte* payload, unsigned int length)
{
  Serial.println("ODEBRANO");
}

MQTToverSerial::MQTToverSerial(WiFiClient& _wifiClient, HardwareSerial& _serial, const char* _serverIp, int _serverPort, const char* _MQTTlogin, const char* _MQTTpass, const char* _MQTTid) : wifiClient(_wifiClient), serial(_serial), serverIp(_serverIp), serverPort(_serverPort), MQTTlogin(_MQTTlogin), MQTTpass(_MQTTpass), MQTTid(_MQTTid)
{
  pubSubClient = new PubSubClient(wifiClient);
  pubSubClient->setServer(serverIp, serverPort);
  pubSubClient->setCallback( [&](char* topic, byte* payload, unsigned int length) 
  {
      this->serial.print("MSSG");
      this->serial.print(specialCharacter);
      this->serial.print(topic);
      this->serial.print(specialCharacter);
      for (unsigned int j = 0; j < length; ++j)
      {
        this->serial.print((char) payload[j]);
      }
      this->serial.println("");
    });
  //pubSubClient->setCallback(Test);
  pubSubClient->connect(MQTTid, MQTTlogin, MQTTpass);

  if (pubSubClient->connected())
  {
    connectedToBroker = true;
  }
}

MQTToverSerial::~MQTToverSerial()
{

}

bool MQTToverSerial::Publish(const char* topic, const char* message)
{
  if (pubSubClient == NULL)
  {
    serial.print("ERROR");
    serial.print(specialCharacter);
    serial.println("Publish");
    Reconnect();
  }

  pubSubClient->publish(topic, message);

  return true;
}

bool MQTToverSerial::SubscribeTopic(const char* topic)
{
  if (!pubSubClient->connected())
  {
    serial.print("ERROR");
    serial.print(specialCharacter);
    serial.println("SubscribeTopic");
    Reconnect();
  }

  pubSubClient->subscribe(topic);

  return true;
}

void MQTToverSerial::Reconnect()
{
  // Loop until we're reconnected
  while (!pubSubClient->connected()) 
  {
    // Attempt to connect
    if (!pubSubClient->connect(MQTTid, MQTTlogin, MQTTpass)) 
    {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

#define BUFFER_SIZE 2000

void MQTToverSerial::ReadSerial()
{
  char serialBuffer[BUFFER_SIZE + 1];
  memset(serialBuffer, 0, BUFFER_SIZE + 1);
  while(serial.readBytesUntil('\n', serialBuffer, BUFFER_SIZE))
  {
    char command[5];
    strncpy(command, serialBuffer, 4);
    command[4] = '\0';
    
    if (strcmp(command, "SEND") == 0)
    {
      size_t i = 5;

      char topic[BUFFER_SIZE + 1];
      size_t i_t = 0;
      memset(topic, 0, BUFFER_SIZE + 1);
      char message[BUFFER_SIZE + 1];
      size_t i_m = 0;
      memset(message, 0, BUFFER_SIZE + 1);
      
      while (i < BUFFER_SIZE + 1 && serialBuffer[i] != specialCharacter)
      {
        topic[i_t] = serialBuffer[i];
        ++i;
        ++i_t;
      }
      ++i;
      while (i < BUFFER_SIZE + 1 && serialBuffer[i] != '\n')
      {
        message[i_m] = serialBuffer[i];
        ++i;
        ++i_m;
      }

      if(!pubSubClient->connected())
        Reconnect();
      Publish(topic, message);
    }
    else if (strcmp(command, "SUBS") == 0)
    {
      size_t i = 5;

      char topic[BUFFER_SIZE + 1];
      size_t i_t = 0;
      memset(topic, 0, BUFFER_SIZE + 1);
      
      while (i < BUFFER_SIZE + 1 && serialBuffer[i] != '\n')
      {
        topic[i_t] = serialBuffer[i];
        ++i;
        ++i_t;
      }

      if(!pubSubClient->connected())
        Reconnect();
      SubscribeTopic(topic);
    }
    
    memset(serialBuffer, 0, BUFFER_SIZE + 1);
  }
}

bool MQTToverSerial::Loop()
{
  if (!pubSubClient->connected()) {
    Reconnect();
  }
  return pubSubClient->loop();
}
