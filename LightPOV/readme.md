# Light POV

## Important Features
- Using `Effects` to manage the displaying time of an effect.
- Using `ColorScheduler` to set colors with pre-defined functions.
- Using OTA to upload program through Wifi.
- Using a web server to send commands to POVs for synchronization.
- Displaying pre-loaded bitmaps to display complex patterns.

## Project file structure
```
├── ESP32                       : Main program project
│   ├── ESP32.ino               : Main program code
│   ├── acc.cpp                 : 
│   ├── acc.h                   : Accelerometer module
│   ├── bitmaps.h               : bitmaps for displaying patterns
│   ├── communication.cpp       : Wifi connection and OTA
│   ├── communication.h         : Wifi connection and OTA
│   ├── config.h                : Configuration for Wifi password, host address, etc.
│   ├── core.h                  : Definitions of pattern functions
│   ├── modes.cpp
│   └── modes.h                 : Implementations of patterns, 
│                               : The scheduling of effects
├── hardware
│   ├── buttomCap.SLDPRT
│   ├── buttomCap.STL
│   ├── combine.SLDASM
│   ├── core.SLDPRT
│   ├── core.STL
│   ├── core_front.SLDPRT
│   └── core_front.STL
├── readme.md
└── test                        : Some module test code
```

## The hardware 
The schematic was not recorded.
- ESP32
- WS2812 
- 18650 battery

## Welcome for Contribution
