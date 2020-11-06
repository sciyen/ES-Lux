#ifndef MODES_H
#define MODES_H
#include <stdint.h>
#include <FastLED.h>
#include <cppQueue.h>
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
    uint16_t calc_ramp(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow=false);
    uint16_t calc_tri(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow=false);
    uint16_t calc_pulse(uint16_t idx, uint8_t range, uint8_t lower, uint8_t top);

    uint16_t getFuncValue(ValueParam* vp, uint16_t idx, bool overflow=false);

public:
    CHSV headingColor;
    /* ------------------
       |   *   *   *
     entry s   s   s
    */
    time_t effect_entry_time;
    uint16_t effect_start_idx;
    ColorScheduler(uint16_t _effect_entry_time, uint8_t defaultV=0);

    void SetXHsvParam(ValueParam H, ValueParam S, ValueParam V);

    void SetYHsvParam(ValueParam H, ValueParam S, ValueParam V);

    /* Call this function before updating one line */
    void updateHeading(uint16_t now_time, bool restart=false);

    /* Get single pixel value with y index specified */
    CRGB inline getPixelColor(uint8_t y);

    /* Get one line pixel array with bitmap specified */
    void getPixelColor(CRGB* pixels, uint32_t bitmap, CHSV replace);

    /* Get one line pixel array */
    void getPixelColor(CRGB* pixels);
};

class Effects{
private:
    time_t current_music_time;
    time_t last_music_update_time;
    uint16_t time_idx;      // Effect X direction index
    ColorScheduler sch;

    RotationDetector detector;
    CRGB pixels[NUMPIXELS];

    time_t effect_entry_time;   // The time 

    void blockUntilStart(time_t start, uint16_t timeout);
    bool detectPassStart(time_t start);

    void showLED();

public:
    /******************************
     *     Sequential control     *
     ******************************/
    uint8_t effect_id;

    void setMusicTime(time_t t);
    time_t getMusicTime();

    cppQueue buffer;

    Effects();

    /* Return next effect id.
     * If the buffer is full, it will return a negative value.
     */
    int16_t checkBufferAvailable();

    void feedNewEffect(Mode* m);

    /* Perform LED show if there has any task available */
    void perform();

    /* Update acceleration sensor */
    void update();

    /* Helper function */
    void setEffectStart(Mode* m);
    void setEffectBlockStart();
    uint16_t getIdx();
    bool checkDuration(Mode* m);

    /* Effect list */
    void clear();

    void plain(Mode* m);

    void square(Mode* m);

    void sickle(Mode* m);

    void fan(Mode* m);

    void boxes(Mode* m);

    void bitmap(Mode* m, uint32_t map, uint8_t length, bool reverse=false);

    void colormap(Mode* m, uint32_t* map, uint8_t length);
};

#endif
