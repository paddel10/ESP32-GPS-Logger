#include "BlinkLed.h"

void BlinkLed::init()
{
    pinMode(m_ledPin, OUTPUT);
}

// ----------------------------------------------------------------------

void BlinkLed::fatalBlink()
{
    while (true) {
        // TODO: from SDfat - SysCall::yield();
        digitalWrite(m_ledPin, HIGH);
        delay(200);
        digitalWrite(m_ledPin, LOW);
        delay(200);
    }
}

// ----------------------------------------------------------------------

void BlinkLed::trippleBlink()
{
    for (int i = 0; i < 6; i++) {
        digitalWrite(m_ledPin, HIGH);
        delay(100);
        digitalWrite(m_ledPin, LOW);
        delay(100);
    }
}

// ----------------------------------------------------------------------

void BlinkLed::toggleLed()
{
    digitalWrite(m_ledPin, HIGH);
    delay(200);
    digitalWrite(m_ledPin, LOW);
}

// ----------------------------------------------------------------------

void BlinkLed::turnOnLed()
{
    digitalWrite(m_ledPin, HIGH);
}

// ----------------------------------------------------------------------

void BlinkLed::turnOffLed()
{
    digitalWrite(m_ledPin, LOW);
}

// ----------------------------------------------------------------------
