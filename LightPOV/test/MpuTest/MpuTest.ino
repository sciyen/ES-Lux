#include <BluetoothSerial.h>
#include <Wire.h>
//#include <ArduinoOTA.h>
BluetoothSerial BT;     //宣告藍芽物件，名稱為BT
#include <MPU6050.h>

/* Network configuration */ 
#define WIFI_SSID1 "Benson"
#define WIFI_PASS1 "0932209548"
#define WIFI_SSID2 "2.4G"
#define WIFI_PASS2 "12345qwert"
#define WIFI_SSID3 "NCKUES[AUTO]"
#define WIFI_PASS3 "nckues_auto"

#define WIFI_CONNECT_RETRY 20
/*
void connect(){
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i){
        if (WiFi.SSID(i) == WIFI_SSID1){
            WiFi.begin(WIFI_SSID1, WIFI_PASS1); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID1);
            break;
        }
        if (WiFi.SSID(i) == WIFI_SSID2){
            WiFi.begin(WIFI_SSID2, WIFI_PASS2); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID2);
            break;
        }
        if (WiFi.SSID(i) == WIFI_SSID3){
            WiFi.begin(WIFI_SSID3, WIFI_PASS3); //trying to connect the modem
            Serial.println();
            Serial.print("Connected to ");
            Serial.println(WIFI_SSID3);
            break;
        }
    }

    Serial.print("Waiting for connection");
    // Wait for connection
    int connection_times = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        connection_times++;
        if (connection_times >= WIFI_CONNECT_RETRY)
        {
            Serial.println();
            Serial.println("Connection Failed! Rebooting...");
            delay(1000);
            ESP.restart();
        }
    }

    Serial.println("");
    /*Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void OTA()
{
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            Serial.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            Serial.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            Serial.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}
*/
uint8_t buffer[14];
int16_t  AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
MPU6050 accelgyro;
void mpu_init()
{
    /*
    Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);    // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);*/
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    accelgyro.initialize();
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
    accelgyro.setDLPFMode(MPU6050_DLPF_BW_5);
    accelgyro.setDHPFMode(MPU6050_DHPF_0P63);
}
void mpu_read()
{
    accelgyro.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);
    /*
    Wire.beginTransmission(0x68);
    Wire.write(0x3B); //starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 14, true); //request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read(); //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();//0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX = Wire.read() << 8 | Wire.read(); //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    */
    /*
    I2Cdev::readBytes(0x68, 0x3B, 14, buffer);
    AcX = (((int16_t)buffer[0]) << 8) | buffer[1];
    AcY = (((int16_t)buffer[2]) << 8) | buffer[3];
    AcZ = (((int16_t)buffer[4]) << 8) | buffer[5];
    GyX = (((int16_t)buffer[8]) << 8) | buffer[9];
    GyY = (((int16_t)buffer[10]) << 8) | buffer[11];
    GyZ = (((int16_t)buffer[12]) << 8) | buffer[13];*/
}

void setup() {
    Serial.begin(115200);
    BT.begin("ESP32");   //請改名例如英文+生日，以免與其他同學名稱重複
    mpu_init();
    //OTA();
}
 
void loop() {
    //ArduinoOTA.handle();
    mpu_read();
    float amp = 1+sqrt((float)AcX*AcX + (float)AcY*AcY + (float)AcZ*AcZ );
    BT.print(AcX);
    BT.print(",");
    BT.print(AcY);
    BT.print(",");
    BT.print(AcZ);/*
    BT.print(",");
    amp = 1;//+sqrt((float)GyX*GyX + (float)GyY*GyY + (float)GyZ*GyZ );
    BT.print(GyX/amp);
    BT.print(",");
    BT.print(GyY/amp);
    BT.print(",");
    BT.print(GyZ/amp);*/
    BT.println("");
    delay(10);
}
