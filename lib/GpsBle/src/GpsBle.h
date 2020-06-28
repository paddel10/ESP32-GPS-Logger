#ifndef _GpsBle_h
#define _GpsBle_h

#include <Arduino.h>
#ifdef USE_BLE
    #include <BLEDevice.h>
    #include <BLEUtils.h>
    #include <BLEServer.h>
#endif

class GpsBle
{
#ifdef USE_BLE
    BLEServer* m_pServer;
    BLEService* m_pService;
    BLECharacteristic* m_pCharacteristic;
    BLEAdvertising* m_pAdvertising;
#endif
public:
    GpsBle() {}
    ~GpsBle() {}

    void init();
    void setValue(String value);
};
#endif
