#ifndef ACC_H
#define ACC_H

#include "config.h"
#include "core.h"
#include <MPU6050.h>

class RotationDetector{
private:
    MPU6050 mpu;
    int16_t acc_x;
    time_t last_low_time;
    time_t interval;

public:
    RotationDetector();
    void init();
    void update();
}

#endif