#include <GpsBle.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define DEVICE_NAME "Dev344"
#define SERVICE_UUID        "25ad886b-c283-4af9-8c94-12ab113e2cca"
#define CHARACTERISTIC_UUID "9c04f59a-c7f4-4af6-9b3c-757ad36fc21b"

void GpsBle::init()
{
#ifdef USE_BLE
    /**
     * WARNING: it's not possible to run Serial.println
     * from an ISR
     */
    BLEDevice::init(DEVICE_NAME);
    m_pServer = BLEDevice::createServer();
    m_pService = m_pServer->createService(SERVICE_UUID);
    m_pCharacteristic = m_pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ);

    m_pCharacteristic->setValue("idle.");
    m_pService->start();

    // m_pAdvertising = BLEDevice::getAdvertising();
    m_pAdvertising = m_pServer->getAdvertising();
    m_pAdvertising->addServiceUUID(SERVICE_UUID);
    m_pAdvertising->setScanResponse(true);
    m_pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    m_pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
#endif
}

// ----------------------------------------------------------------------

void GpsBle::setValue(String value)
{
#ifdef USE_BLE
    m_pCharacteristic->setValue(value.c_str());
#endif
}
