#ifndef ESSENTIAL_H
#define ESSENTIAL_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Tasks.h"
#include "Modes.h"

#define uint8  byte
#define uint16 unsigned int
#define uint24 unsigned long

/* LED */
#define   BTN_PIN   10

#define   RED_LED_PIN   5
#define GREEN_LED_PIN   9
#define  BLUE_LED_PIN   6
#define LED_OPEN  1
#define LED_CLOSE 0
#define LED_R_INDEX 0
#define LED_G_INDEX 1
#define LED_B_INDEX 2
extern volatile uint8 led_buffer[3];

/* LED BUFFER */
#define          BUFFER_MODE_BIT 0
#define   BUFFER_START_TIME0_BIT 1
#define   BUFFER_START_TIME1_BIT 2
#define   BUFFER_START_TIME2_BIT 3
#define   BUFFER_START_TIME3_BIT 4
#define     BUFFER_DURATION0_BIT 5
#define     BUFFER_DURATION1_BIT 6
#define            BUFFER_P1_BIT 7
#define            BUFFER_P2_BIT 8
#define            BUFFER_P3_BIT 9
#define            BUFFER_P4_BIT 10
#define            BUFFER_P5_BIT 11
#define            BUFFER_P6_BIT 12
#define            BUFFER_P7_BIT 13

extern int buffer_counter;
extern int buffer_excute_counter;
extern uint24 buffer_start_time;

void led_init();
void buffer_init();
void led_update();
uint24 inline get_buffer_start_time();
uint24 inline get_cmd_start_time(int index);
uint16 inline get_cmd_duration(int index);
uint8 inline get_cmd_para_1(int index, int para);
uint16 inline get_cmd_para_2(int index, int para);
uint24 inline get_cmd_para_3(int index, int para);
void buffer_update();
//void hsv_test();
void ISR_enable();
void ISR_disable();

#endif
