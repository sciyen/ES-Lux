#include "acc.h"

RotationDetector::RotationDetector(){

}

uint8_t RotationDetector::setMpuBit(uint8_t b, uint8_t bitStart, uint8_t length, uint8_t data){
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    b &= ~(mask); // zero all important bits in existing byte
    b |= data; // combine data with existing byte
    return b;
}

void RotationDetector::init(){
#ifdef USE_WIRE_LIB
    Wire.begin();

    /* Setting up DLPF */
    Wire.beginTransmission(MPU6050_ADDRESS_AD0_LOW);
    // Read from config bytes
    Wire.write(MPU6050_RA_CONFIG); 
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDRESS_AD0_LOW, 1, true); 
    uint8_t config = Wire.read(); 
    Wire.endTransmission(true);

    // Edit config
    config = setMpuBit(config, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, MPU6050_DLPF_BW_5);

    // Write into config reg
    Wire.beginTransmission(MPU6050_ADDRESS_AD0_LOW);
    Wire.write((uint8_t) MPU6050_RA_CONFIG); // send address
    Wire.write((uint8_t) config); // send data
    Wire.endTransmission(true);
    delay(100);
    Wire.beginTransmission(MPU6050_ADDRESS_AD0_LOW);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    
#else
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
#endif
}

void RotationDetector::clear_flag(){
    flag = Rotation_RESET;
}

RotationFlag RotationDetector::read_flag(){
    return flag;
}

void RotationDetector::update(){

    static time_t last_update_time = millis();
    static int16_t last_acc=0, delta[2]={0}, diff=0, last_diff=0;
    static int8_t idx = 0;
    time_t now_time = millis();
    if (now_time - last_update_time > ROTATION_UPDATE_INTERVAL){
    #ifdef USE_WIRE_LIB      
        Wire.beginTransmission(MPU6050_ADDRESS_AD0_LOW);
        //starting with register 0x3B (ACCEL_XOUT_H)
        Wire.write(MPU6050_RA_ACCEL_XOUT_H); 
        Wire.endTransmission(false);
        //request a total of 2 registers
        Wire.requestFrom(MPU6050_ADDRESS_AD0_LOW, 2, true); 
        //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
        int32_t raw = Wire.read() << 8 | Wire.read(); 
        acc_x = (raw != -1) ? raw : acc_x;
        //acc_x = raw;
    #else
        acc_x = mpu.getAccelerationX();
    #endif

        delta[1] = delta[0];                // Forward move
        delta[0] = acc_x - last_acc;
        diff = (3 * delta[0] - delta[1] )/2;
        diff = (diff + last_diff) / 2;
        if (last_diff < 0 && diff > 0 ){
            interval = now_time - last_low_time;
            last_low_time = now_time;
            flag = Rotation_SET;
            Serial.println("Break");
        }
        last_acc = acc_x;
        last_diff = diff;
        last_update_time = now_time;
        /*
        Serial.print(delta[0]);
        Serial.print(",");
        Serial.println(delta[1]);*/
        //Serial.println(acc_x);
    }
}
