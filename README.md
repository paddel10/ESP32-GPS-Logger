Logging GPS data to a SD card. Additional button press stores current location in a separate file as a point of interest (POI). The GPS data stored is a NMEA sentence.

| Component | Purpose |
| -------- |-------------|
| ESP32 - Wroom32 | Main controller. Reads GPS data from UART and writes the position to a SD card. Handles button pushes to store additional POIs. |
| GPS NEO-7M-C | GPS device |
| SD card read/write SPI module | Writes position to a SD card. |
| Momentary button with LED | Button when pressed initiates saving the current position to the SD card. Button contains a LED ring which is used to display the status of the system. |

TBD: Fritzing

## Thirdparty Libraries
| Name | Purpose | URL |
| -------- |-------------| -------------|
| SdFat | Arduino FAT16/FAT32 Library | https://github.com/greiman/SdFat |
| EasyButton | Arduino library for debouncing momentary contact switches | https://github.com/evert-arias/EasyButton |
| MicroNMEA | A compact Arduino library to parse NMEA sentences | https://github.com/stevemarple/MicroNMEA |