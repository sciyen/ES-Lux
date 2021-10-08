/***************************************************************************
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, sciyen(Yen Cheng Chu),
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

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

void show_bitmap_logo();

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