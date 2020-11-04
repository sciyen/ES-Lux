#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//#include <ESP8266WiFi.h>  //for https
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void wifi_connect();
void get_data();
String receivelightmessage();
void OTA();

#endif