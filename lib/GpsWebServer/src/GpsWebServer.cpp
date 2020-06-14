#include "GpsWebServer.h"

#include <WiFi.h>
#include <SPIFFS.h>

// static decleration
ParameterBag* GpsWebServer::s_pParameterBag;

String GpsWebServer::httpRequestProcessor(const String &var)
{
    Serial.println("httpRequestProcessor");
    if (var == "SSID" && GpsWebServer::s_pParameterBag) {
        return GpsWebServer::s_pParameterBag->getSsid();
    }
    return String();
}

// ----------------------------------------------------------------------

void GpsWebServer::init()
{
    // mount file system
    Serial.println("Mounting file system ...");
    if (!SPIFFS.begin(true)) {
        Serial.println("*** An Error has occurred while mounting SPIFFS");
        return;
    }

    Serial.println("Setting up access point ...");
    String apIpAddress = setupAccessPoint();
    Serial.println("AP IP address: " + apIpAddress);

    Serial.println("Setting up server ...");
    setupServer();

    Serial.println("MAC address: " + WiFi.macAddress());
}

// ----------------------------------------------------------------------

String GpsWebServer::setupAccessPoint()
{
    // Remove the password parameter, if you want the AP (Access Point) to be open
    String macAddress = WiFi.macAddress();
    macAddress.replace(":", "");
    m_ssid = SSID_PREFIX + macAddress.substring(macAddress.length() - 6);

    Serial.println("ssid: " + m_ssid);
    WiFi.softAP(m_ssid.c_str(), m_ssidPassword);
    // return the IP address
    return WiFi.softAPIP().toString(); 
}

// ----------------------------------------------------------------------

void GpsWebServer::setupServer()
{
    m_pServer->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("request received.");
        request->send(SPIFFS, "/index.html", String(), false, this->httpRequestProcessor);
        // request->send(SPIFFS, "/index.html");
        // request->send(200, "text/plain", "Hello World");
    });
  
    m_pServer->onNotFound([](AsyncWebServerRequest *request) {
        Serial.println("onNotFound() called");
        request->send(404);
    });

    (m_pServer->serveStatic("/", SPIFFS, "/")).setCacheControl("max-age=600");
    m_pServer->begin();
}