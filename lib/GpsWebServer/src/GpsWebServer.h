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
    ParameterBag* m_pParameterBag;
    String m_ssid;
    AwsTemplateProcessor* m_processor;

private:
    String setupAccessPoint();
    void setupServer();

public:

    GpsWebServer(ParameterBag* pParameterBag)
    : m_pParameterBag(pParameterBag)
    , m_ssid("")
    {
        m_pServer = new AsyncWebServer(WEBSERVER_PORT);
    }
    ~GpsWebServer() {}
    void init();
    void setProcessor(AwsTemplateProcessor processor) { m_processor = &processor; }
    String getSsid() { return m_ssid; }
};

#endif