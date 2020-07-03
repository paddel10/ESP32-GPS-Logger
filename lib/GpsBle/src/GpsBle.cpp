#include <GpsBle.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define DEVICE_NAME "Dev344"
#define SERVICE_UUID        "25ad886b-c283-4af9-8c94-12ab113e2cca"
#define CHARACTERISTIC_UUID_TX "9c04f59a-c7f4-4af6-9b3c-757ad36fc21b"

bool deviceConnected = false;

#ifdef USE_BLE
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
#endif

void GpsBle::init()
{
#ifdef USE_BLE
    BLEDevice::init(DEVICE_NAME);
    // Create the BLE Server
    m_pServer = BLEDevice::createServer();
    m_pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    m_pService = m_pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    m_pCharacteristic = m_pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
                
    m_pCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    m_pService->start();

    // Start advertising
    m_pServer->getAdvertising()->start();
#endif
}

// ----------------------------------------------------------------------

void GpsBle::setValue(String value)
{
#ifdef USE_BLE
   if (deviceConnected) {
       Serial.println("notify BLE devices: " + value);
        m_pCharacteristic->setValue(value.c_str());
        m_pCharacteristic->notify();
   }
#endif
}
