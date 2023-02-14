#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#define DEBUGGER
#define DEBUGGER_TASK_REPORT

#define LUX_ID 1

/* Type Definition */
#define time_t unsigned long

#define QUEUE_SIZE 5

/* Pin Configuration */
#define PIN_LED 18       // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 32 // How many NeoPixels are attached to the Arduino?

/* Network configuration */ 
#define WIFI_SSID1 "Benson"
#define WIFI_PASS1 "0932209548"
#define WIFI_SSID2 "yen"
#define WIFI_PASS2 "0905026157"
#define WIFI_SSID3 "NCKUES[AUTO]"
#define WIFI_PASS3 "nckues_auto"

#define WIFI_REQUEST_URL "http://122.117.214.235:10240/get_effect"
#define WIFI_TIME_CHECK_URL "http://122.117.214.235:10240/esp_time"

#define WIFI_CONNECT_RETRY 20

#define START_TIME_CHECK_INTERVAL 100

#define FORCE_START_MODE true

/* Rotation Detector */
#define ROTATION_UPDATE_INTERVAL 1 //ms

#define META_PARAMETER_BUF_SIZE 4

#endif
