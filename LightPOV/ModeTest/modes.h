#ifndef MODES_H
#define MODES_H
#include <stdint.h>
#include <FastLED.h>
#include "config.h"
#include "core.h"
#include "acc.h"


class ColorScheduler{
private:
    uint8_t defaultValue;

    /* X-dir color scheduler */
    ValueParam XHp, XSp, XVp;
    /* Y-dir color scheduler */
    ValueParam YHp, YSp, YVp;

    // now_time(millis()), restart whether restart while pass throught start
    uint8_t calc_ramp(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow=false);
    uint8_t calc_tri(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow=false);
    uint8_t calc_pulse(uint16_t idx, uint8_t range, uint8_t lower, uint8_t top);

    uint8_t getFuncValue(ValueParam* vp, uint16_t idx, bool overflow=false);

public:
    CHSV headingColor;
    time_t effect_entry_time;
    ColorScheduler(time_t _effect_entry_time, uint8_t defaultV=0);

    void SetXHsvParam(ValueParam H, ValueParam S, ValueParam V);

    void SetYHsvParam(ValueParam H, ValueParam S, ValueParam V);

    /* Call this function before updating one line */
    void updateHeading(time_t now_time, time_t last_low_time, bool restart);

    /* Get single pixel value with y index specified */
    CHSV inline getPixelColor(uint8_t y);

    /* Get one line pixel array with bitmap specified */
    void getPixelColor(CHSV* pixels, uint32_t bitmap, CHSV replace);

    /* Get one line pixel array */
    void getPixelColor(CHSV* pixels);
};

class Effects{
private:
    RotationDetector detector;
    CHSV pixels[NUMPIXELS];
    CRGB rgb_pixels[NUMPIXELS];
    time_t effect_entry_time;

    inline uint16_t get_idx(time_t now_time, time_t start_time, time_t interval);

    void blockUntilStart(time_t start, uint16_t timeout);
    bool detectPassStart(time_t start);

public:
    Effects();
    void square(Mode* m);
    void update();
    void showLED();
};
/*
void sickle();
void reverse_sickle();
void fan();
void es1();
void es2();
void rev_es1();
void fire_staff();
void star();
void rainbow();
void lightning();*/

#endif
