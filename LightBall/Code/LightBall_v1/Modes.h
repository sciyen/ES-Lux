#ifndef MODES_H
#define MODES_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define LIMITED_MIN 0
#define LIMITED_MAX 255
/* HSV */
#define DEFAULT_HSV_S 255
#define HSV_V_MAX 100
#define HSV_PROGRESSIVE_UPDATE_CYCLE 180
/* RGB */
#define SPARK_PROGRESSIVE_TIME_LIMIT 20
#include "Essentials.h"
#define uint8  byte
#define uint16 unsigned int
#define uint24 unsigned long

char get_limited_value(int v);
void set_rgb(uint8 r, uint8 g, uint8 b);
float HueToRGB(float v1, float v2, float vH);
void set_hsl(int H, float S, float L);
void set_hsl_progressive(uint24 index, uint16 duration, 
                         uint8 h, uint8 s, uint8 l, 
                         uint8 colorTrans, uint8 brightTrans);
void set_hsl_spark_async(uint24 index, uint16 duration, 
                         uint8 h, uint8 s, uint8 l, 
                         uint8 colorTrans, uint8 brightTrans, 
                         uint8 count, uint8 duty);
void set_hsl_spark_sync(uint24 index, uint16 duration, 
                         uint8 h, uint8 s, uint8 l, 
                         uint8 colorTrans, uint8 brightTrans, 
                         uint8 count, uint8 duty);
void set_hsl_meteor_async(uint24 index, uint16 duration, 
                         uint8 h, uint8 s, uint8 l, 
                         uint8 colorTrans, uint8 meteorTrans, 
                         uint8 count, uint8 duty);
void set_hsl_meteor_sync(uint24 index, uint16 duration, 
                         uint8 h, uint8 s, uint8 l, 
                         uint8 colorTrans, uint8 meteorTrans, 
                         uint8 count, uint8 duty);
#endif
