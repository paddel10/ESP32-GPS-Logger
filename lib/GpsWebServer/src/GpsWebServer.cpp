#include "GpsWebServer.h"

#include <WiFi.h>
#include <WebPageHelper.h>

String GpsWebServer::m_sROOT_URI = "/";
String GpsWebServer::m_sFILES_URI = "/files";

void GpsWebServer::init()
{
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
    m_pServer->on(m_sROOT_URI.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on / received.");
        request->send(200, "text/html", getDefaultPage());
    });
    m_pServer->on(m_sFILES_URI.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on /files received.");
        request->send(200, "text/html", getFilesPage());
    });
    m_pServer->onNotFound([](AsyncWebServerRequest *request) {
        Serial.println("onNotFound() called");
        request->send(404);
    });

    m_pServer->begin();
}

// ----------------------------------------------------------------------

String GpsWebServer::getDefaultPage()
{
    String content = String("<table>");
    content += getTableRow("SSID", m_ssid);
    content += getTableRow("Last Position (Lat/Long)", String(m_pParameterBag->getLastLat()) + "/" + String(m_pParameterBag->getLastLong()));
    content += getTableRow("Last Waypoint (Lat/Long)", String(m_pParameterBag->getLastWaypointLat()) + "/" + String(m_pParameterBag->getLastWaypointLong()));
    content += String("</table>");

    String body = getPageBody("ESP32 - GPS Logger", content);

    return getPageHeader("Files", m_sFILES_URI) + body + getPageFooter();
}

// ----------------------------------------------------------------------

String GpsWebServer::getFilesPage()
{
    String content = m_pParameterBag->getFilesList();
    String body = getPageBody("ESP32 - GPS Logger", content);
    return getPageHeader("Home", m_sROOT_URI) + body + getPageFooter();
}
