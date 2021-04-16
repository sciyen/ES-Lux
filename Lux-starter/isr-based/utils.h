#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"
#include "bitmap.h"

#define NUM_LEDS 16

/* Calculate the length of the given string */
int get_len(char *s);

/* Set all brightness of each led to the given value
 * @Params:
 *      value: ranging from 0~7
 */
void show_leds(uint8_t value);

/* Show strings
 * @Params:
 *      str: char array
 *      clear_len: distance between each string (ms)
 */
void show_str(char *str,
              int16_t color = -1,
              int8_t dc = 30,
              int clear_len = 10);

void show_hsv_demo();

void show_bitmap_fire();

float hue_to_rgb(float v1, float v2, float vH);

/* Convert the color space from HSV to RGB
 * @Params:
 *      H: Hue, 0~360
 *      S: Saturation, 0~1
 *      V: Value, 0~1, (Set to 1 for white, 0 for black)
 *      br:
 */
void set_hsl(int H, float S, float L, uint8_t *br, uint8_t *bg, uint8_t *bb);

void ISR_enable();

void ISR_disable();

#endif