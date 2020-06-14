#ifndef _PositionStorage_h
#define _PositionStorage_h

#include <Arduino.h>
#include <SdFat.h>

#define APPEND_POSITION_ERROR -1
#define APPEND_POSITION_IDLE 0
#define APPEND_POSITION_SUCCESS 1
#define APPEND_POSITION_UNKNOWN -2

#define INIT_POSITION_STORAGE_ERROR -1
#define INIT_POSITION_STORAGE_IDLE 0
#define INIT_POSITION_STORAGE_SUCCESS 1

class PositionStorage 
{
    int m_sdcardInitialized = 0;
    int m_sdCsPin;
    SdFat* m_pSd;
    SdFile* m_pLogFile;;

public:
    PositionStorage(int sdCsPin)
    : m_sdCsPin(sdCsPin)
    {
        m_pSd = new SdFat();
        m_pLogFile = new SdFile();
    }
    ~PositionStorage() {}

    /**
     * initialize storage
     * 
     * @return INIT_POSITION_STORAGE_XXX
     **/
    int init();

    /**
     * append the given position to the given file. the file is
     * created when it does not exist yet.
     * system goes into panic mode if append fails.
     * 
     * @param filename the filname
     * @param position nmea string
     * @return APPEND_POSITION_XXX
     **/
    int appendPosition(String filename, String position);
};

#endif