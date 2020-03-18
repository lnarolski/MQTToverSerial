#include "MQTToverSerial.h"

MQTToverSerial::MQTToverSerial(WiFiClient& _wifiClient, HardwareSerial &_serial, const char* _serverIp, int _serverPort, const char* _MQTTlogin, const char* _MQTTpass, const char* _MQTTid, HardwareSerial* _debugSerial) : wifiClient(_wifiClient), serial(_serial), serverIp(_serverIp), serverPort(_serverPort), MQTTlogin(_MQTTlogin), MQTTpass(_MQTTpass), MQTTid(_MQTTid), debugSerial(_debugSerial)
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

      if (this->debugSerial != NULL)
      {
          this->debugSerial->print("MSSG");
          this->debugSerial->print(specialCharacter);
          this->debugSerial->print(topic);
          this->debugSerial->print(specialCharacter);
          for (unsigned int j = 0; j < length; ++j)
          {
            this->debugSerial->print((char) payload[j]);
          }
          this->debugSerial->println("");
      }
  });
  pubSubClient->connect(MQTTid, MQTTlogin, MQTTpass);
}

MQTToverSerial::~MQTToverSerial()
{
  if (pubSubClient != NULL)
    delete pubSubClient;
}

bool MQTToverSerial::Publish(const char* topic, const char* message)
{
  pubSubClient->publish(topic, message);

  return true;
}

bool MQTToverSerial::SubscribeTopic(const char* topic)
{
  if (!pubSubClient->connected())
  {
    Reconnect();
  }

  return pubSubClient->subscribe(topic);
}

bool MQTToverSerial::UnsubscribeTopic(const char* topic)
{
  if (!pubSubClient->connected())
  {
    Reconnect();
  }

  return pubSubClient->unsubscribe(topic);
}

void MQTToverSerial::Reconnect()
{
  // Loop until we're reconnected
  while (!pubSubClient->connected()) 
  {
    // Attempt to connect
    if (!pubSubClient->connect(MQTTid, MQTTlogin, MQTTpass)) 
    {
      serial.print("ERR_");
      serial.print(specialCharacter);
      serial.println("No connection with MQTT broker!");
      if (debugSerial != NULL)
      {
          debugSerial->print("ERR_");
          debugSerial->print(specialCharacter);
          debugSerial->println("No connection with MQTT broker!");
      }
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

#define BUFFER_SIZE 2000 //UART RX buffer size

void MQTToverSerial::ReadSerial()
{
  char serialBuffer[BUFFER_SIZE + 1];
  memset(serialBuffer, 0, BUFFER_SIZE + 1);
  while(serial.readBytesUntil('\n', serialBuffer, BUFFER_SIZE))
  {
    if (debugSerial != NULL)
    {
      debugSerial->println(serialBuffer);
    }
    
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

      if (strlen(topic) == 0)
      {
        Error(serialBuffer);
        
        memset(serialBuffer, 0, BUFFER_SIZE + 1);
        continue;
      }
      
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

      if (strlen(topic) == 0)
      {
        Error(serialBuffer);
        
        memset(serialBuffer, 0, BUFFER_SIZE + 1);
        continue;
      }

      if(!pubSubClient->connected())
        Reconnect();
      SubscribeTopic(topic);
    }
    else if (strcmp(command, "UNSB") == 0)
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

      if (strlen(topic) == 0)
      {
        Error(serialBuffer);
        
        memset(serialBuffer, 0, BUFFER_SIZE + 1);
        continue;
      }

      if(!pubSubClient->connected())
        Reconnect();
      UnsubscribeTopic(topic);
    }
    else
    {
      Error(serialBuffer);      
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

void MQTToverSerial::Error(const char* buff)
{
    serial.print("ERR_");
    serial.print(specialCharacter);
    serial.println(buff);
    
    if (debugSerial != NULL)
    {   
      debugSerial->print("ERR_");
      debugSerial->print(specialCharacter);
      debugSerial->println(buff);
    }
}
