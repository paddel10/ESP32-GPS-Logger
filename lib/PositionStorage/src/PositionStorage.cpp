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
    m_totalFileSize = 0;

    if (!root.open("/")) {
        Serial.println("*** open root failed");
        return "";
    }
    while (file.openNext(&root, O_RDONLY)) {
        if (!file.isHidden() && file.isFile() && file.getName(&filename[0], 13 * sizeof(char))) {
            String name = String(&filename[0]);
            String deleteLink = String("[<a href=\"delete?file=" + name + "\" onclick=\"return confirm('Delete file?');\">delete</a>] ");
            String downloadLink = String("<a href=\"download?file=" + name + "\">") + name + String("</a>");
            htmlTable += getTableRow(deleteLink + downloadLink, String(file.fileSize() / 1024) + String(" kB"), false);
            m_totalFileSize += file.fileSize();
        }
        file.close();
    }
    return String("<table>") + htmlTable + String("</table>") + String("Total size: ") + getTotalFileSizeInKb();
}

// ----------------------------------------------------------------------

void PositionStorage::deleteFile(String filename)
{
    bool bRet = m_pSd->remove(filename.c_str());
    String status = "NOT deleted";
    if (bRet) {
        status = "deleted";
    }
    Serial.println(">" + filename + "< " + status);
}

// ----------------------------------------------------------------------

File PositionStorage::getFileHandle(String filename)
{
    return m_pSd->open(filename.c_str(), O_RDONLY);
}
