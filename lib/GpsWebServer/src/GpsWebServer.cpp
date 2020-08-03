#include "GpsWebServer.h"
#include <WebPageHelper.h>

String GpsWebServer::m_sROOT_URI = "/";
String GpsWebServer::m_sFILES_URI = "/files";
String GpsWebServer::m_sWIFI = "/wifi";
String GpsWebServer::m_sDOWNLOAD_FILE = "/download";
String GpsWebServer::m_sDELETE_FILE = "/delete";

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
#ifdef USE_ASYNC
    m_pServer->on(m_sROOT_URI.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on / received.");
        request->send(200, "text/html", getDefaultPage());
    });
    m_pServer->on(m_sFILES_URI.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on /files received.");
        request->send(200, "text/html", getFilesPage());
    });
    m_pServer->on(m_sDELETE_FILE.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on /delete received.");
        if (request->hasParam("file")) {
            AsyncWebParameter* p = request->getParam("file");
            String filename = p->value();
            Serial.println("parameter: " + filename);
            deleteFile(filename);
        }
        request->redirect(GpsWebServer::m_sFILES_URI);
    });
    m_pServer->on(m_sDOWNLOAD_FILE.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("on /download received.");
        //send 128 bytes as plain text
        if (request->hasParam("file")) {
            AsyncWebParameter* p = request->getParam("file");
            String filename = p->value();
            Serial.println("parameter: " + filename);
            //request->send("text/plain", 128, [this](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
                // Write up to "maxLen" bytes into "buffer" and return the amount written.
                // index equals the amount of bytes that have been already sent
                // You will not be asked for more bytes once the content length has been reached.
                // Keep in mind that you can not delay or yield waiting for more data!
                // Send what you currently have and you will be asked for more again
            //    return mySource.read(buffer, maxLen);
            //});
        }
        request->send(200, "text/html", getFilesPage());
    });
    m_pServer->onNotFound([](AsyncWebServerRequest *request) {
        Serial.println("onNotFound() called");
        request->send(404);
    });
#else
    m_pServer->on(m_sROOT_URI.c_str(), [this]() {
        Serial.println("on / received.");
        sendHTML_Header();
        m_pServer->sendContent(getDefaultPage());
        sendHTML_Stop();
    });
    m_pServer->on(m_sFILES_URI.c_str(), [this]() {
        Serial.println("on /files received.");
        sendHTML_Header();
        m_pServer->sendContent(getFilesPage());
        sendHTML_Stop();
    });
    m_pServer->on(m_sDELETE_FILE.c_str(), HTTP_GET, [this]() {
        Serial.println("on /delete received.");
        if (m_pServer->hasArg("file")) {
            String filename = m_pServer->arg("file");
            Serial.println("parameter: " + filename);
            deleteFile(filename);
        }
        m_pServer->sendHeader("Location", m_sFILES_URI);
        m_pServer->send(303);
    });
    m_pServer->on(m_sDOWNLOAD_FILE.c_str(), [this]() {
        Serial.println("on /download received.");
        if (m_pServer->hasArg("file")) {
            String filename = m_pServer->arg("file");
            Serial.println("parameter: " + filename);
            downloadFile(filename);
        } else {
            m_pServer->sendHeader("Location", m_sFILES_URI);
            m_pServer->send(303);
        }
    });
#endif
    m_pServer->begin();
}

// ----------------------------------------------------------------------

String GpsWebServer::getDefaultPage()
{
    String content = String("<table>");
    content += getTableRow("SSID", m_ssid);
    content += getTableRow("Last Position Latitude (deg)", String(m_pParameterBag->getLastLat() / 1000000.));
    content += getTableRow("Last Position Longitude (deg)", String(m_pParameterBag->getLastLong() / 1000000.));
    content += getTableRow("Last Position date/time", m_pParameterBag->getLastPositionDateTime());
    content += getTableRow("Last Waypoint Latitude (deg)", String(m_pParameterBag->getLastWaypointLat() / 1000000.));
    content += getTableRow("Last Waypoint Longitude (deg)", String(m_pParameterBag->getLastWaypointLong() / 1000000.));
    content += getTableRow("Last Waypoint date/time", m_pParameterBag->getLastWaypointDateTime());
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
// ----------------------------------------------------------------------

void GpsWebServer::downloadFile(String filename)
{
    File file = m_pParameterBag->getFileHandle(filename);
    if (file) {
        uint32_t fileSize = file.size();
        Serial.println("downloading file! " + String(fileSize));
        m_pServer->sendHeader("Content-Type", "application/octet-stream");
        m_pServer->sendHeader("Content-Disposition", "attachment; filename=" + filename);
        m_pServer->sendHeader("Connection", "close");
        if (m_pServer->streamFile(file, "application/octet-stream") != fileSize) {
            Serial.println("sent less data than expected! " + String(fileSize));
        }
        file.close();
    } else {
        Serial.println("could not download file >" + filename + "<");
    }
}

// ----------------------------------------------------------------------

void GpsWebServer::deleteFile(String filename)
{
    m_pParameterBag->deleteFile(filename);
}

// ----------------------------------------------------------------------

void GpsWebServer::serveClient()
{
#ifndef USE_ASYNC
    m_pServer->handleClient();
#endif
}

// ----------------------------------------------------------------------

void GpsWebServer::sendHTML_Header()
{
#ifndef USE_ASYNC
    m_pServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
    m_pServer->sendHeader("Pragma", "no-cache"); 
    m_pServer->sendHeader("Expires", "-1"); 
    m_pServer->setContentLength(CONTENT_LENGTH_UNKNOWN); 
    m_pServer->send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves. 
#endif
}

// ----------------------------------------------------------------------

void GpsWebServer::sendHTML_Stop()
{
#ifndef USE_ASYNC
  m_pServer->sendContent("");
  m_pServer->client().stop(); // Stop is needed because no content length was sent
#endif
}

