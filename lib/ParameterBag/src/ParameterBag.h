#ifndef _ParameterBag_h
#define _ParameterBag_h

#include "PositionGps.h"
#include "PositionStorage.h"

class ParameterBag
{
    PositionGps* m_pPositionGps;
    PositionStorage* m_pPositionStorage;
    long m_lastWaypointLong;
    long m_lastWaypointLat;

public:
    ParameterBag()
    : m_pPositionGps(0)
    , m_pPositionStorage(0)
    , m_lastWaypointLong(0)
    , m_lastWaypointLat(0)
    {}
    ~ParameterBag() {}

    void setPositionGps(PositionGps* pPositionGps) { m_pPositionGps = pPositionGps; }
    void setPositionStorage(PositionStorage* pPositionStorage) { m_pPositionStorage = pPositionStorage; }
    long getLastLong() { return m_pPositionGps->getLastPositionLong(); }
    long getLastLat() { return m_pPositionGps->getLastPositionLat(); }
    String getLastPositionDateTime() { return m_pPositionGps->getLastPositionDateTime(); }

    long getLastWaypointLong() { return m_lastWaypointLong; }
    long getLastWaypointLat() { return m_lastWaypointLat; }
    void setLastWaypointLong(long lastWaypointLong) { m_lastWaypointLong = lastWaypointLong; }
    void setLastWaypointLat(long lastWaypointLat) { m_lastWaypointLat = lastWaypointLat; }
    String getLastWaypointDateTime() { return m_pPositionGps->getLastWaypointDateTime(); }

    String getFilesList() { return m_pPositionStorage->getFilesAsHtmlTable(); }
    void deleteFile(String filename) { m_pPositionStorage->deleteFile(filename); }
    File getFileHandle(String filename) { return m_pPositionStorage->getFileHandle(filename); }
};
#endif