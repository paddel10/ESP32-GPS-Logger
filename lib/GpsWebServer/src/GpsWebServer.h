#ifndef _GpsWebServer_h
#define _GpsWebServer_h

// FS_NO_GLOBALS is used to avoid name collision with sdfat
#define FS_NO_GLOBALS 
#include <ESPAsyncWebServer.h>
#include <ParameterBag.h>

#define WEBSERVER_PORT 80
#define SSID_PREFIX "GPSLogger-AP-"

class GpsWebServer
{
private:
    AsyncWebServer* m_pServer;
    const char* m_ssidPassword = "gpslogger123"; // yes, password is hardcoded
    String m_ssid;

public:
    static ParameterBag* s_pParameterBag;

private:
    String setupAccessPoint();
    void setupServer();
    static String httpRequestProcessor(const String &var);

public:
    GpsWebServer(ParameterBag* pParameterBag)
    : m_ssid("")
    {
        m_pServer = new AsyncWebServer(WEBSERVER_PORT);
        s_pParameterBag = pParameterBag;
    }
    ~GpsWebServer() {}
    void init();
    String getSsid() { return m_ssid; }
};

#endif