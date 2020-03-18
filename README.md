# MQTToverSerial

Simple library based on [PubSubClient](https://github.com/knolleary/pubsubclient) (with all limitations) for Arduino (tested on ESP32) to communicate with MQTT broker over HardwareSerial connection (tested with UART)

# Table of contents

* [General info](#general-info)
* [Public methods](#public-methods)
* [Commands](#commands)
* [Example](#example)

# General info

Library allows you to make Serial (e.g. UART) <--> MQTT Broker connections with commands sended via UART.

# Public methods

In class contructor you have to set connected wifiClient object to communicate via WiFi, HardwareSerial object to communicate via Serial, MQTT broker server IP, MQTT broker server port, MQTT broker server login, MQTT broker server password and your MQTT id.

## bool SubscribeTopic(const char* topic)

Method to subscribe topic. Returns true on success.

## bool UnsubscribeTopic(const char* topic)

Method to unsubscribe topic. Returns true on success.

## bool Publish(const char* topic, const char* message)

Method to publish message in specified topic. Returns true on success.

## bool Loop()

Method to call in loop to maintain connection to the server. Returns true on success.

## void SetSpecialCharacter(char _specialCharacter)

Set character as new commands delimiter.

## char GetSpecialCharacter()

Get actual delimiter.

## void ReadSerial()

Analyse serial buffer to find sended commands.

# Commands

After program Arduino device (ESP32 in my case) you just send following commands via Serial. Default delimiter in commands is '$' (dollar sign). You can change it with _SetSpecialCharacter_ method. All commands have to end with new line character (_\n_).

## SUBS

Subscribe topic: SUBS$_topic\_name_

To subscribe _demo_ topic send via Serial:

`SUBS$demo\n`

## UNSB

Unsubscribe topic: UNSB$_topic\_name_

To unsubscribe _demo_ topic send via Serial:

`UNSB$demo\n`

## SEND

Send message to specified topic: SEND$_topic\_name$message_

To send message _MQTT broker test_ to _demo_ topic send via Serial:

`SEND$demo$MQTT broker test\n`

## MSSG

Received message from subscribed topic: MSSG$_topic\_name$message_

When you get _MQTT broker test_ message from _demo_ topic you will get from Serial:

`MSSG$demo$MQTT broker test\n`

## ERR_

Error message with description:

`ERR_$No connection with MQTT broker!\n`

# Example

Example project could be found in the /example/ directory. Was tested on ESP32 Dev Module and with Arduino Serial Monitor.
