#ifndef ACC_H
#define ACC_H
#include <Wire.h>
#define MPU_addr 0x68  // I2C address of the MPU-6050
void mpu_init();
void mpu_read();

#endif