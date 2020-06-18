#include "PositionStorage.h"
#include <WebPageHelper.h>

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
            // set date time callback function
            // SdFile::dateTimeCallback(dateTime);
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

// ----------------------------------------------------------------------

String PositionStorage::getFilesAsHtmlTable()
{
    // Directory file.
    SdFile root;
    // Use for file creation in folders.
    SdFile file;
    char filename[13];
    String htmlTable = "";

    if (!root.open("/")) {
        Serial.println("*** open root failed");
        return "";
    }
    while (file.openNext(&root, O_RDONLY)) {
        if (!file.isHidden() && file.isFile() && file.getName(&filename[0], 13 * sizeof(char))) {
            htmlTable += getTableRow(String(&filename[0]), String(file.fileSize() / 1024) + String(" kB"));
        }
        file.close();
    }
    return String("<table>") + htmlTable + String("</table>");
}

// ----------------------------------------------------------------------

String PositionStorage::getCardSizeAsString()
{
    // TODO: not working
    char buffer[100];
    buffer[99] = '\0';
    float cardSize = m_pSd->card()->cardSize() * 0.000512;
    float used = cardSize - m_pSd->vol()->freeClusterCount() * m_pSd->vol()->blocksPerCluster() * 0.000512;
    sprintf(&buffer[0], "Size: %d.%02d MB (Used: %d.%02d MB)",
        (unsigned int)cardSize,
        (unsigned int)(cardSize * 100.0) % 100,
        (unsigned int)used,
        (unsigned int)(used * 100.0) % 100);
    Serial.println(&buffer[0]);
    return String(&buffer[0]);
}
/*
void PositionStorage::dateTime(uint16_t* date, uint16_t* time) 
{
  // User gets date and time from GPS or real-time
  // clock in real callback function

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(year, month, day);

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(hour, minute, second);
}
*/
