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
String lastPosition = "";
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
String dateTimeStr = "";
int filenameNr = 0;
#define error(msg) {sd.errorPrint(&Serial, F(msg));fatalBlink();}

// waypoint 
#define WAYPOINT_LED_PIN 2
#define WAYPOINT_BTN_PIN 27
EasyButton waypointButton(WAYPOINT_BTN_PIN);

/**
 * after a fatal error this function is called which never ends.
 * button led is toggled.
 */
void fatalBlink() {
    while (true) {
        SysCall::yield();
        digitalWrite(WAYPOINT_LED_PIN, HIGH);
        delay(200);
        digitalWrite(WAYPOINT_LED_PIN, LOW);
        delay(200);
    }
}

/**
 * blink a couple of times
 */
void trippleBlink() {
    for (int i = 0; i < 6; i++) {
        digitalWrite(WAYPOINT_LED_PIN, HIGH);
        delay(100);
        digitalWrite(WAYPOINT_LED_PIN, LOW);
        delay(100);
    }
}

/**
 * toggles button led
 */
void toggleWaypointLed()
{
    digitalWrite(WAYPOINT_LED_PIN, HIGH);
    delay(200);
    digitalWrite(WAYPOINT_LED_PIN, LOW);
}

void turnOnWaypointLed()
{
    digitalWrite(WAYPOINT_LED_PIN, HIGH);
}

void turnOffWaypointLed()
{
    digitalWrite(WAYPOINT_LED_PIN, LOW);
}

/**
 * initialize the SD card
 */
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

/**
 * append the given position to the given file. the file is
 * created when it does not exist yet.
 * system goes into panic mode if append fails.
 **/
void appendPositionToFile(String filename, String position)
{
    if (sdcardInitialized) {
        // save position
        if (logFile.open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
            logFile.println(position);
            logFile.flush();
            logFile.close();
            toggleWaypointLed();
            Serial.println(position);
        } else {
            logFile.sync();
            if (logFile.getWriteError()) {
                error(String("*** error file.open >" + filename + "<").c_str());
            }
        }
    }
}

/**
 * flush previously logged data
 **/
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

/**
 * callback function when the waypoint button is pressed.
 * checks if lastPosition is set and stores it in a separate file
 **/
void onButtonPressed() {
    Serial.println("Waypoint button has been pressed!");
    if (lastPosition.length() > 0) {
        Serial.println("saving waypoint...");
        appendPositionToFile(
            String("p" + dateTimeStr + ".txt"), 
            lastPosition
        );
        trippleBlink();
    }
}

/**
 * setup routines for gps, sd card, button class
 */
void setup() {
    Serial.begin(BAUD);
    Serial.println("MOSI: " + String(MOSI)); // system defined constants
    Serial.println("MISO: " + String(MISO));
    Serial.println("SCK: " +  String(SCK));
    Serial.println("SS: " + String(SS));
    Serial.println("GPS Txd2: " + String(TXD2));

    pinMode(WAYPOINT_LED_PIN, OUTPUT);
    turnOnWaypointLed();

    // sd card
    initSD();

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

    Serial.println("waybutton setup");
    waypointButton.begin();
    waypointButton.onPressed(onButtonPressed);

    Serial.println("setup() completed");
}

/**
 * main loop. reads gps data if available and stores it in the
 * attached sd card. only every MAX_LOOP_WAIT gps data is stored.
 */
void loop() {
    Serial.flush();
    GPSModule.flush();

    // update internal button state
    waypointButton.read();

    while (GPSModule.available() > 0) {
        char c = GPSModule.read();
        
        if (nmea.process(c) && nmea.isValid()) {
            turnOffWaypointLed();
            lastPosition = String(nmea.getSentence());
            if (nmea.getYear() >= YEAR_2020) {
                if (currLoopWait % MAX_LOOP_WAIT == 0) {
                    dateTimeStr = String(nmea.getYear()) + String(nmea.getMonth()) + String(nmea.getDay());
                    appendPositionToFile(
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