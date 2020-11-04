#ifndef MODES_H
#define MODES_H
#include "config.h"
#include "acc.h"

class Effects{
private:
    RotationDetector detector;
    CHSV pixels[NUMPIXELS];

public:
    void sickle();
    void square();
    void update();
}
void sickle();
void reverse_sickle();
void fan();
void es1();
void es2();
void rev_es1();
void fire_staff();
void star();
void rainbow();
void lightning();

#endif