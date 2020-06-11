#include "PositionStorage.h"

int PositionStorage::init()
{
    Serial.println("PositionStorage::init() called ...");
    if (!m_sdcardInitialized) {
        // sd card
        if (!m_pSd->begin(m_sdCsPin, SD_SCK_MHZ(4))) {
            m_pSd->initErrorHalt();
            Serial.println("PositionStorage::init() ... error.");
            return INIT_POSITION_STORAGE_ERROR;
        } else {
            m_sdcardInitialized = 1;
            Serial.println("PositionStorage::init() ... success.");
            return INIT_POSITION_STORAGE_SUCCESS;
        }
    }
    Serial.println("PositionStorage::init() ... idle.");
    return INIT_POSITION_STORAGE_IDLE;
}

// ----------------------------------------------------------------------

int PositionStorage::appendPosition(String filename, String position)
{
    Serial.println("PositionStorage::appendPosition() called ... sdcardInitialized: " + String(m_sdcardInitialized) + ", filename >" + filename + "<");
    if (m_sdcardInitialized) {
        // save position
        if (m_pLogFile->open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
            m_pLogFile->println(position);
            m_pLogFile->flush();
            m_pLogFile->close();
            Serial.println("PositionStorage::appendPosition() ... success.");
            return APPEND_POSITION_SUCCESS;
        } else {
            m_pLogFile->sync();
            if (m_pLogFile->getWriteError()) {
                Serial.println("PositionStorage::appendPosition() ... error file open >" + filename + "<");
                return APPEND_POSITION_ERROR;
            } else {
                Serial.println("PositionStorage::appendPosition() ... unknown.");
                return APPEND_POSITION_UNKNOWN;
            }
        }
    }
    Serial.println("PositionStorage::appendPosition() ... idle.");
    return APPEND_POSITION_IDLE;
}