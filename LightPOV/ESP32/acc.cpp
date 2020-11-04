#include "acc.h"

RotationDetector::RotationDetector(){

}

RotationDetector::init(){
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    mpu.initialize();
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

    // Setup Digital High/Low Pass Filter
    mpu.setDLPFMode(MPU6050_DLPF_BW_5);
    mpu.setDHPFMode(MPU6050_DHPF_0P63);
}

RotationDetector::update(){
    static time_t last_update_time = millis();
    static int16_t last_acc=0, delta[2]={0}, diff=0, last_diff=0;
    static int8_t idx = 0;
    time_t now_time = millis();
    if (now_time - last_update_time > ROTATION_UPDATE_INTERVAL){
        acc_x = mpu.getAccelerationX();

        delta[1] = delta[0];                // Forward move
        delta[0] = acc_x - last_acc;
        diff = (3 * delta[0] - delta[1] )/2;
        if (last_diff < 0 && diff > 0 ){
            interval = now_time - last_low_time;
            last_low_time = now_time;
        }
        last_diff = diff;
        last_update_time = now_time;
    }
}