#ifndef ACC_H
#define ACC_H

#include "config.h"
#include "core.h"

#define USE_WIRE_LIB 
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
// address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_LOW     0x68 
#define MPU6050_RA_CONFIG           0x1A

#define MPU6050_DLPF_BW_5           0x06
#define MPU6050_DLPF_BW_10          0x05
#define MPU6050_CFG_DLPF_CFG_BIT    2
#define MPU6050_CFG_DLPF_CFG_LENGTH 3


#ifdef USE_WIRE_LIB
    #include <Wire.h>
#else
    #include <MPU6050.h>
#endif 

enum RotationFlag{Rotation_RESET, Rotation_SET};

class RotationDetector{
private:
#ifndef USE_WIRE_LIB
    MPU6050 mpu;
#endif
    int16_t acc_x;
    RotationFlag flag;
    uint8_t setMpuBit(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

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
