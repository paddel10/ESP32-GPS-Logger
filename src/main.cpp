#include <Arduino.h>
#include <SPI.h>
#include <EasyButton.h>
#include <BlinkLed.h>
#include <PositionStorage.h>
#include <PositionGps.h>

#define BAUD 9600

// gps module
// U2UXD is unused and can be used for the project - reserved PINS TX (=1), RX (=3)
#define RXD2 16
#define TXD2 17
PositionGps positionGps(RXD2, TXD2);

// sdcard
#define SD_CS 5
PositionStorage positionStorage(SD_CS);

// waypoint button
#define WAYPOINT_BTN_PIN 27
EasyButton waypointButton(WAYPOINT_BTN_PIN);

// led
#define WAYPOINT_LED_PIN 2
BlinkLed blinkLed(WAYPOINT_LED_PIN);

void handleAppendPositionRet(int ret)
{
    switch (ret) {
        case APPEND_POSITION_SUCCESS:
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
    String lastPosition = positionGps.getLastPosition();
    if (lastPosition.length() > 0) {
        Serial.println("saving waypoint...");
        // lastWaypoint = lastPosition;
        int ret = positionStorage.appendPosition(
            positionGps.getLastWaypointFilename(), 
            positionGps.getLastPosition()
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

    blinkLed.init();
    blinkLed.turnOnLed();

    // sd card
    positionStorage.init();

    // prepare gps
    positionGps.init();

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

    // update internal button state
    waypointButton.read();

    int ret = positionGps.readPosition();

    if (ret == POSITION_GPS_VALID) {
        ret = positionStorage.appendPosition(
            positionGps.getLastPositionFilename(),
            positionGps.getLastPosition()
        );
        switch (ret) {
            case APPEND_POSITION_SUCCESS:
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