#ifndef _BlinkLed_h
#define _BlinkLed_h

#include <Arduino.h>

class BlinkLed 
{
    int m_ledPin;

public:
    BlinkLed(int ledPin): m_ledPin(ledPin) {}
    ~BlinkLed() {}

    void init();

    /**
     * after a fatal error this function is called which never ends.
     * button led is toggled.
     */
    void fatalBlink();

    /**
     * blink a couple of times
     */
    void trippleBlink();

    /**
     * toggles led
     */
    void toggleLed();
    void turnOnLed();
    void turnOffLed();

};

#endif
