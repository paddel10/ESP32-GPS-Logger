#ifndef _ParameterBag_h
#define _ParameterBag_h

class ParameterBag
{
    long m_lastLong;
    long m_lastLat;
    long m_lastWaypointLong;
    long m_lastWaypointLat;
    String m_ssid;


public:
    ParameterBag()
    : m_lastLong(0)
    , m_lastLat(0)
    , m_lastWaypointLong(0)
    , m_lastWaypointLat(0)
    , m_ssid("")
    {}
    ~ParameterBag() {}
    long getLastLong() { return m_lastLong; }
    long getLastLat() { return m_lastLat; }
    long getLastWaypointLong() { return m_lastWaypointLong; }
    long getLastWaypointLat() { return m_lastWaypointLat; }

    void setLastLong(long lastLong) { m_lastLong = lastLong; }
    void setLastLat(long lastLat) { m_lastLat = lastLat; }
    void setLastWaypointLong(long lastWaypointLong) { m_lastWaypointLong = lastWaypointLong; }
    void setLastWaypointLat(long lastWaypointLat) { m_lastWaypointLat = lastWaypointLat; }

    String getSsid() { return m_ssid; }
    void setSsid(String ssid) { m_ssid = ssid; }
};
#endif