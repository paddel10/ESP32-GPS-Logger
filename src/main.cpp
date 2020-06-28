#include <Arduino.h>
#include <EasyButton.h>
#include <BlinkLed.h>
#include <PositionStorage.h>
#include <PositionGps.h>
#include <GpsWebServer.h>
#include <ParameterBag.h>
#include <GpsBle.h>
#include <ESP.h>
#include <esp_spi_flash.h>

#define BAUD 9600

// common set of parameters
ParameterBag* pParameterBag;

// gps module
// U2UXD is unused and can be used for the project - reserved PINS TX (=1), RX (=3)
#define RXD2 16
#define TXD2 17
PositionGps* pPositionGps;

// sdcard
#define SD_CS 5
PositionStorage* pPositionStorage;

// waypoint button
#define WAYPOINT_BTN_PIN 27
EasyButton waypointButton(WAYPOINT_BTN_PIN);

// led
#define WAYPOINT_LED_PIN 2
BlinkLed blinkLed(WAYPOINT_LED_PIN);

// webserver
GpsWebServer* pGpsWebServer;

// bluetooth low energy
GpsBle* pGpsBle;

void handleAppendPositionRet(int ret)
{
    switch (ret) {
        case APPEND_POSITION_SUCCESS:
            pParameterBag->setLastWaypointLat(pPositionGps->getLastPositionLat());
            pParameterBag->setLastWaypointLong(pPositionGps->getLastPositionLong());
            blinkLed.trippleBlink();
            break;
        case APPEND_POSITION_ERROR:
            blinkLed.fatalBlink();
            break;
        case APPEND_POSITION_IDLE:
        default:
            break;
    }
}

/**
 * callback function when the waypoint button is pressed.
 * checks if lastPosition is set and stores it in a separate file
 **/
void onButtonPressed() {
    Serial.println("Waypoint button has been pressed!");
    String lastPosition = pPositionGps->getLastPosition();
    if (lastPosition.length() > 0) {
        Serial.println("saving waypoint...");
        pPositionGps->setLastWaypointDateTime();
        // lastWaypoint = lastPosition;
        int ret = pPositionStorage->appendPosition(
            pPositionGps->getLastWaypointFilename(), 
            pPositionGps->getLastPosition()
        );
        handleAppendPositionRet(ret);
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

    pParameterBag = new ParameterBag();

    blinkLed.init();
    blinkLed.turnOnLed();

    // prepare bluetooth
    Serial.println("bluetooth low energy (BLE) setup");
    pGpsBle = new GpsBle();
    pGpsBle->init();
    #ifdef USE_BLE
        Serial.println("BLE initilaized");
    #else
        Serial.println("BLE not available");
    #endif

    // webserver
    pGpsWebServer = new GpsWebServer(pParameterBag);
    pGpsWebServer->init();

    // sd card
    pPositionStorage = new PositionStorage(SD_CS);
    pPositionStorage->init();

    // prepare gps
    pPositionGps = new PositionGps(RXD2, TXD2);
    pPositionGps->init();

    Serial.println("waybutton setup");
    waypointButton.begin();
    waypointButton.onPressed(onButtonPressed);

    pParameterBag->setPositionGps(pPositionGps);
    pParameterBag->setPositionStorage(pPositionStorage);

    // chip info
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    Serial.println("Hardware info");
    Serial.printf("%d cores Wifi %s%s\n", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    Serial.printf("Silicon revision: %d\n", chip_info.revision);
    Serial.printf("%dMB %s flash\n", spi_flash_get_chip_size()/(1024*1024),
    (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embeded" : "external");
    
    //get chip id
    String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
    chipId.toUpperCase();
    
    Serial.printf("Chip id: %s\n", chipId.c_str());
    Serial.printf("Free heap: %d\n", esp_get_free_heap_size());
    Serial.println("setup() completed");
}

/**
 * main loop. reads gps data if available and stores it in the
 * attached sd card. only every MAX_LOOP_WAIT gps data is stored.
 */
void loop() {
    Serial.flush();

    // update internal button state
    waypointButton.read();

    int ret = pPositionGps->readPosition();

    if (ret == POSITION_GPS_VALID) {
        String value;
        ret = pPositionStorage->appendPosition(
            pPositionGps->getLastPositionFilename(),
            pPositionGps->getLastPosition()
        );
        switch (ret) {
            case APPEND_POSITION_SUCCESS:
                value = String(pPositionGps->getLastPositionLat()) + String("/") + String(pPositionGps->getLastPositionLong());
                pGpsBle->setValue(value);
                blinkLed.toggleLed();
                break;
            case APPEND_POSITION_ERROR:
                blinkLed.fatalBlink();
                break;
            default:
                break;
        }
    }
}