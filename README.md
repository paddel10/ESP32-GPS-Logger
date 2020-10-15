Logging GPS data to a SD card. Additional button press stores current location in a separate file as a point of interest (POI). GPS data is stored as a NMEA sentence.

| Component | Purpose |
| -------- |-------------|
| ESP32 - Wroom32 | Main controller. Reads GPS data from UART and writes the position to a SD card. Handles button pushes to store additional POIs. |
| GPS NEO-7M-C | GPS device |
| SD card read/write SPI module | Writes position to a SD card. |
| Momentary button with LED | Button when pressed initiates saving the current position to the SD card. Button contains a LED ring which is used to display the status of the system. |

TBD: Fritzing

## Pins used on the ESP32 - Wroom32
name, pin on board, pin in main.cpp

## Thirdparty Libraries
| Name | Purpose | URL |
| -------- |-------------| -------------|
| SdFat | Arduino FAT16/FAT32 Library | https://github.com/greiman/SdFat |
| EasyButton | Arduino library for debouncing momentary contact switches | https://github.com/evert-arias/EasyButton |
| MicroNMEA | A compact Arduino library to parse NMEA sentences | https://github.com/stevemarple/MicroNMEA |

## Decoding stack trace after crash
https://github.com/littleyoda/EspStackTraceDecoder
```
java -jar EspStackTraceDecoder.jar C:\Users\<USER>\Documents\Arduino\hardware\espressif\esp32\tools\xtensa-esp32-elf\bin\xtensa-esp32-elf-addr2line.exe C:\Users\<USER>\Documents\GitHub\ESP32-GPS-Logger\.pio\build\nodemcu-32s\firmware.elf dump3.txt
```

&copy; 2020