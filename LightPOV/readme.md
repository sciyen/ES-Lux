# Light POV (光蛇)

## Important Features
- Using `Effects` to manage the displaying time of an effect.
- Using `ColorScheduler` to set colors with pre-defined functions.
- Using OTA to upload program through Wifi.
- Using a web server to send commands to POVs for synchronization.
- Displaying pre-loaded bitmaps to display complex patterns.

## Project file structure
```
├── ControlPanel                : Web server for controlling the POV
│   ├── package-lock.json
│   ├── package.json
│   ├── public                  : Web front-end
│   ├── readme.md
│   └── server.js               : Web main server
├── ESP32                       : Program for ESP32
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
The schematic is an unrecorded artifact. The hardware is a simple design with an ESP32, a WS2812 LED strip, and a 18650 battery. 
Maybe schematics and PCB layouts will be added in the future.

## Welcome for Contribution
If you are interested in this project, feel free to contribute to this project.
If you have any questions, please open an issue or contact @sciyen via sciyen.ycc@gmail.com .