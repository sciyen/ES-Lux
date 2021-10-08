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

#include "utils.h"

#define PIN_ENABLE 10
#define PIN_CLOCK 13
#define PIN_DATA 11

extern volatile uint8_t led_map[NUM_LEDS][3];

void setup()
{
    Serial.begin(115200);

    // setup led RGB output pin
    DDRD = B01100000;
    DDRB = B00000010;

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_DATA, OUTPUT);

    digitalWrite(PIN_ENABLE, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_DATA, LOW);
    ISR_enable();
}

int16_t color = 0;
void loop()
{
    show_hsv_demo();

    for (int x = 0; x < 200; x++)
        show_bitmap_logo();

    uint8_t R, G, B;
    for (int x = 0; x < 1000; x++) {
        set_hsl(x % 360, 1, 0.5, &R, &G, &B);
        for (int y = 0; y < NUM_LEDS; y++) {
            if ((y - (12 + 3 * sin(2 * 3.14 * x / 16)) <= 0) &&
                (y - (3 + 3 * sin(2 * 3.14 * x / 16)) >= 0)) {
                led_map[y][0] = R;
                led_map[y][1] = G;
                led_map[y][2] = B;
            } else {
                led_map[y][0] = 8;
                led_map[y][1] = 8;
                led_map[y][2] = 8;
            }
        }
        delay(1);
    }

    for (int i = 0; i < 20; i++) {
        char str[] = "NCKU ES!";
        color = (color + get_len(str) * 8) % 360;
        show_str(str, color, 1, 10);
    }
}
