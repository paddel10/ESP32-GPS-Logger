#ifndef _GpsWebServer_h
#define _GpsWebServer_h

#ifdef USE_ASYNC
    // FS_NO_GLOBALS is used to avoid name collision with sdfat
    #define FS_NO_GLOBALS
    #include <ESPAsyncWebServer.h>
#else
    #include <ESP32WebServer.h>
#endif
#include <WiFi.h>
#include <ParameterBag.h>

#define WEBSERVER_PORT 80
#define SSID_PREFIX "GPSLogger-AP-"

class GpsWebServer
{
private:
#ifdef USE_ASYNC
    AsyncWebServer* m_pServer;
#else
    ESP32WebServer* m_pServer;
#endif
    const char* m_ssidPassword = "gpslogger123"; // yes, password is hardcoded
    ParameterBag* m_pParameterBag;
    String m_ssid;

    static String m_sROOT_URI;
    static String m_sFILES_URI;
    static String m_sWIFI;
    static String m_sDOWNLOAD_FILE;
    static String m_sDELETE_FILE;

private:
    String setupAccessPoint();
    void setupServer();
    String getDefaultPage();
    String getFilesPage();
    void downloadFile(String filename);
    void deleteFile(String filename);
    void sendHTML_Header();
    void sendHTML_Stop();

public:
    GpsWebServer(ParameterBag* pParameterBag)
    : m_pParameterBag(pParameterBag)
    , m_ssid("")
    {
        #ifdef USE_ASYNC
            m_pServer = new AsyncWebServer(WEBSERVER_PORT);
        #else
            m_pServer = new ESP32WebServer(WEBSERVER_PORT);
        #endif
    }
    ~GpsWebServer() {
        // cleanup
        if (m_pServer) delete m_pServer;
    }
    void init();
    String getSsid() { return m_ssid; }
    void serveClient();
};

#endif