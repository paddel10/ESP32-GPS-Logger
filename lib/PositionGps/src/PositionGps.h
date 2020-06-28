#ifndef _PositionGps_h
#define _PositionGps_h
#include <Arduino.h>
#include <MicroNMEA.h>
#include <SoftwareSerial.h>

#define MAX_LOOP_WAIT 5

#define POSITION_GPS_IDLE 0
#define POSITION_GPS_VALID 1

class PositionGps
{
    String m_lastPosition;
    String m_lastPositionDateTime;
    String m_lastPositionFilename;
    String m_lastWaypointFilename;
    String m_lastWaypointDateTime;
    long m_lastPositionLat;
    long m_lastPositionLong;
    int m_currLoopWait;
    int m_maxLoopWait;
    // gps
    SoftwareSerial* m_pGpsModule;

    // nmea parser
    char m_nmeaBuffer[100];
    MicroNMEA* m_pNmea;

private:
    /**
     * flush previously logged data
     **/
    void gpsHardwareReset();

    void setLastPositionDateTime();

public:
    PositionGps(int rxPin, int txPin)
    : m_lastPosition("")
    , m_lastPositionDateTime("")
    , m_lastPositionFilename("")
    , m_lastWaypointFilename("")
    , m_lastWaypointDateTime("")
    , m_lastPositionLat(0)
    , m_lastPositionLong(0)
    , m_currLoopWait(0)
    , m_maxLoopWait(MAX_LOOP_WAIT)
    {
        m_pNmea = new MicroNMEA(m_nmeaBuffer, sizeof(m_nmeaBuffer));
        m_pGpsModule = new SoftwareSerial(rxPin, txPin);
    }
    ~PositionGps() {
        // cleanup
        if (m_pNmea) delete m_pNmea;
        if (m_pGpsModule) delete m_pGpsModule;
    }

    void init();

    int readPosition();
    
    String getLastPosition() { return m_lastPosition; }

    long getLastPositionLat() { return m_lastPositionLat; }
    long getLastPositionLong() { return m_lastPositionLong; }

    String getLastPositionDateTime() { return m_lastPositionDateTime; }
    String getLastWaypointDateTime() { return m_lastWaypointDateTime; }
    void setLastWaypointDateTime();

    String getLastPositionFilename() { return m_lastPositionFilename; }

    String getLastWaypointFilename() { return m_lastWaypointFilename; }
};

#endif
