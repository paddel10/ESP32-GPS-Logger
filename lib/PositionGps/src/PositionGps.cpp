#include "PositionGps.h"

#define GPS_BAUD 9600
#define YEAR_2020 2020

void PositionGps::init()
{
    Serial.println("PositionGps::init() called ...");
    // prepare gps
    m_pGpsModule->begin(GPS_BAUD);
    this->gpsHardwareReset();

    // Clear the list of messages which are sent.
    MicroNMEA::sendSentence(*m_pGpsModule, "$PORZB");

    // Send only RMC and GGA messages.
    MicroNMEA::sendSentence(*m_pGpsModule, "$PORZB,RMC,1,GGA,1");

    // Disable compatability mode (NV08C-CSM proprietary message) and
    // adjust precision of time and position fields
    MicroNMEA::sendSentence(*m_pGpsModule, "$PNVGNME,2,9,1");

    Serial.println("PositionGps::init() ... completed.");
}

// ----------------------------------------------------------------------

int PositionGps::readPosition()
{
    int ret = POSITION_GPS_IDLE;

    m_pGpsModule->flush();

    while (m_pGpsModule->available() > 0) {
        char c = m_pGpsModule->read();
        
        if (m_pNmea->process(c) && m_pNmea->isValid()) {
            m_lastPosition = String(m_pNmea->getSentence());
            Serial.println(m_lastPosition);
            if (m_pNmea->getYear() >= YEAR_2020) {
                if (m_currLoopWait % MAX_LOOP_WAIT == 0) {
                    m_lastPositionFilename = String(m_pNmea->getYear()) + String(m_pNmea->getMonth()) + String(m_pNmea->getDay()) + ".txt";
                    m_lastWaypointFilename = String("p" + m_lastPositionFilename);
                    ret = POSITION_GPS_VALID;
                }
                m_currLoopWait++;
            }
            m_pNmea->clear();
        }
    }

    return ret;
}

// ----------------------------------------------------------------------

void PositionGps::gpsHardwareReset()
{
    Serial.println("PositionGps::gpsHardwareReset() Resetting GPS module ...");
    // Empty input buffer
    while (m_pGpsModule->available()) {
      m_pGpsModule->read();
    }

    // Reset is complete when the first valid message is received
    while (1) {
        while (m_pGpsModule->available()) {
            char c = m_pGpsModule->read();
            if (m_pNmea->process(c)) {
                m_pNmea->clear();
                Serial.println("PositionGps::gpsHardwareReset() ... done.");
                return;
            }
        }
    }
}