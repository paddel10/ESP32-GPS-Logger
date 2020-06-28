#include "GpsWebServer.h"
#include <WebPageHelper.h>

String GpsWebServer::m_sROOT_URI = "/";
String GpsWebServer::m_sFILES_URI = "/files";
String GpsWebServer::m_sWIFI = "/wifi";

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

    m_pServer->onNotFound([](AsyncWebServerRequest *request) {
        Serial.println("onNotFound() called");
        request->send(404);
    });
#endif
    m_pServer->begin();
}

// ----------------------------------------------------------------------

void GpsWebServer::serveClient()
{
    #ifdef USE_ASNYC
    #else
        WiFiClient client;// = m_pServer->available();   // Listen for incoming clients

        if (client) {
            String header; // Variable to store the HTTP request
            String currentLine = "";                // make a String to hold incoming data from the client
            while (client.connected()) {            // loop while the client's connected
                if (client.available()) {             // if there's bytes to read from the client,
                    char c = client.read();             // read a byte, then
                    Serial.write(c);                    // print it out the serial monitor
                    header += c;
                    if (c == '\n') {                    // if the byte is a newline character
                        // if the current line is blank, you got two newline characters in a row.
                        // that's the end of the client HTTP request, so send a response:
                        if (currentLine.length() == 0) {
                            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                            // and a content-type so the client knows what's coming, then a blank line:
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type:text/html");
                            client.println("Connection: close");
                            client.println();
                            if (header.indexOf("GET /files") >= 0) {
                                
                            } else if (header.indexOf("GET /") >= 0) {

                            }
                        }  else { // if you got a newline, then clear currentLine
                            currentLine = "";
                        }
                    } else if (c != '\r') {  // if you got anything else but a carriage return character,
                        currentLine += c;      // add it to the end of the currentLine
                    }
                }
            }
            // Close the connection
            client.stop();
        }
    #endif
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

