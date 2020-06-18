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
    String m_lastPositionFilename;
    String m_lastWaypointFilename;
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

public:
    PositionGps(int rxPin, int txPin)
    : m_lastPosition("")
    , m_lastPositionFilename("")
    , m_lastWaypointFilename("")
    , m_lastPositionLat(0)
    , m_lastPositionLong(0)
    , m_currLoopWait(0)
    , m_maxLoopWait(MAX_LOOP_WAIT)
    {
        m_pNmea = new MicroNMEA(m_nmeaBuffer, sizeof(m_nmeaBuffer));
        m_pGpsModule = new SoftwareSerial(rxPin, txPin);
    }
    ~PositionGps() {}

    void init();

    int readPosition();
    
    String getLastPosition() { return m_lastPosition; }

    long getLastPositionLat() { return m_lastPositionLat; }
    long getLastPositionLong() { return m_lastPositionLong; }

    String getLastPositionFilename() { return m_lastPositionFilename; }

    String getLastWaypointFilename() { return m_lastWaypointFilename; }
};

#endif
