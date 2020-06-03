#include <Arduino.h>
#include <SoftwareSerial.h>
#include <MicroNMEA.h>
#include <SPI.h>
#include <SdFat.h>
#include <EasyButton.h>

#define YEAR_2020 2020
#define MAX_LOOP_WAIT 5
// U2UXD is unused and can be used for the project - reserved PINS TX (=1), RX (=3)
#define RXD2 16
#define TXD2 17
#define BAUD 9600
#define GPS_BAUD 9600

// gps module
SoftwareSerial GPSModule(RXD2, TXD2);
String lastPosition;
int currLoopWait = 0;

// nmea parser
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

// sdcard
#define SD_CS 5
int sdcardInitialized = 0;
int nmeaFileExists = 0;
SdFat sd;
SdFile logFile;
String currentFilename;
int filenameNr = 0;
#define error(msg) {sd.errorPrint(&Serial, F(msg));fatalBlink();}

// waypoint 
#define WAYPOINT_LED_PIN 2
#define WAYPOINT_BTN_PIN 26
EasyButton waypointButton(WAYPOINT_BTN_PIN);

void fatalBlink() {
  while (true) {
    SysCall::yield();
    digitalWrite(WAYPOINT_LED_PIN, HIGH);
    delay(200);
    digitalWrite(WAYPOINT_LED_PIN, LOW);
    delay(200);
  }
}

void toggleWaypointLed()
{
    // digitalWrite(WAYPOINT_LED_PIN, !digitalRead(WAYPOINT_LED_PIN));
    digitalWrite(WAYPOINT_LED_PIN, HIGH);
    delay(200);
    digitalWrite(WAYPOINT_LED_PIN, LOW);
}
// Write to the SD card
/*void createFile(fs::FS &fs, const char * path) {
    if (!nmeaFileExists) {
        Serial.printf("Creating file: %s\n", path);
        File file = fs.open(path, FILE_WRITE);
        if(!file) {
            Serial.println("Failed to open file for writing");
            return;
        }
        file.close();
        nmeaFileExists = 1;
    }
}*/
// Append data to the SD card
/*void appendFile(fs::FS &fs, const char* path, const char* message) {
    Serial.printf("Appending to file: %s\n", path);

    createFile(fs, path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message) && file.print("\r\n")) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}*/

/*void initSD() 
{
    SD.begin(SD_CS);  
    if(!SD.begin(SD_CS)) {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    Serial.println("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
}*/
void initSD()
{
    if (!sdcardInitialized) {
        // sd card
        if (!sd.begin(SD_CS, SD_SCK_MHZ(4))) {
            sd.initErrorHalt();
            error("Card failed, or not present");
        } else {
            Serial.println("Card initialized");
            sdcardInitialized = 1;
        }
    }
}

void appendFile(String filename, String sentence)
{
    if (sdcardInitialized) {
        // save position
        if (logFile.open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
            logFile.println(sentence);
            logFile.flush();
            logFile.close();
            toggleWaypointLed();
            Serial.println(sentence);
        } else {
            logFile.sync();
            if (logFile.getWriteError()) {
                error(String("*** error file.open >" + filename + "<").c_str());
            }
            
            /*Serial.println("could not open file for write");
            currentFilename = filename + String("_") + String(filenameNr);
            filenameNr++;*/
        }
    
        // save poi
        /*String value = button.getValue();
        String poiFilename = String("p") + filename;
        if (value.length() > 0) {
            if (logFile.open(poiFilename.c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
                logFile.println(value);
                logFile.flush();
                logFile.close();
                Serial.println("POI saved: " + value);
                button.resetValue();
            } else {
                Serial.println("error opening " + poiFilename);
            }
        }*/
    }
}

void gpsHardwareReset()
{
    Serial.println("Resetting GPS module ...");
    // Empty input buffer
    while (GPSModule.available()) {
      GPSModule.read();
    }

    // Reset is complete when the first valid message is received
    while (1) {
        while (GPSModule.available()) {
            char c = GPSModule.read();
            if (nmea.process(c)) {
                nmea.clear();
                Serial.println("... done");
                return;
            }
        }
    }
}

void setup() {
    Serial.begin(BAUD);
    Serial.println("MOSI: " + String(MOSI)); // system defined constants
    Serial.println("MISO: " + String(MISO));
    Serial.println("SCK: " +  String(SCK));
    Serial.println("SS: " + String(SS));
    Serial.println("GPS Txd2: " + String(TXD2));

    // sd card
    initSD();
    pinMode(WAYPOINT_LED_PIN, OUTPUT);

    // prepare gps
    GPSModule.begin(GPS_BAUD);
    gpsHardwareReset();
    
    // Clear the list of messages which are sent.
    MicroNMEA::sendSentence(GPSModule, "$PORZB");

    // Send only RMC and GGA messages.
      MicroNMEA::sendSentence(GPSModule, "$PORZB,RMC,1,GGA,1");

    // Disable compatability mode (NV08C-CSM proprietary message) and
    // adjust precision of time and position fields
    MicroNMEA::sendSentence(GPSModule, "$PNVGNME,2,9,1");

    Serial.println("setup() completed");
}

void loop() {
    Serial.flush();
    GPSModule.flush();

    while (GPSModule.available() > 0) {
        char c = GPSModule.read();
        
        if (nmea.process(c) && nmea.isValid()) {
            lastPosition = String(nmea.getSentence());
            if (nmea.getYear() >= YEAR_2020) {
                if (currLoopWait % MAX_LOOP_WAIT == 0) {
                    // String dateTimeStr = String(nmea.getYear()) + String(nmea.getMonth()) + String(nmea.getDay()) + String(nmea.getHour()) + String(nmea.getMinute()) + String(nmea.getSecond());
                    String dateTimeStr = String(nmea.getYear()) + String(nmea.getMonth()) + String(nmea.getDay());
                    /*appendFile(
                        SD, 
                        String("/" + filename + ".txt").c_str(), 
                        lastPosition.c_str()
                    );*/
                    appendFile(
                        String(dateTimeStr + ".txt"), 
                        lastPosition
                    );
                }
                currLoopWait++;
            }
            nmea.clear();
        }
    }
}