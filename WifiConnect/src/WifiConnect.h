#ifndef _WifiConnect_h
#define _WifiConnect_h

#include <Arduino.h>
#include <WiFi.h>

#define MAX_RETRY_COUNT 10

void connectToWifi()
{
    String ssid = "AndroidAP";
    String password = "qtdd8869";

    Serial.print("Trying to connect to WiFi " + ssid + " ");

    WiFi.begin(ssid.c_str(), password.c_str());

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < MAX_RETRY_COUNT) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }
    if (retryCount == MAX_RETRY_COUNT) {
        Serial.println("*** Connecting to WiFi failed.");
    } else {
        Serial.println("connected!");
    }
}
#endif