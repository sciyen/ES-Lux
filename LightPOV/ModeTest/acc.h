#ifndef ACC_H
#define ACC_H

#include "config.h"
#include "core.h"
#include <MPU6050.h>

enum RotationFlag{Rotation_RESET, Rotation_SET};

class RotationDetector{
private:
    MPU6050 mpu;
    int16_t acc_x;
    RotationFlag flag;

public:
    time_t interval;
    time_t last_low_time;
    RotationDetector();
    void init();
    void clear_flag();
    RotationFlag read_flag();
    void update();
};

#endif
