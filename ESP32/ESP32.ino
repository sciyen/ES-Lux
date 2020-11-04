#include "config.h"
#include "modes.h"
#include "communication.h"
#include "acc.h"

CRGB pixels[NUMPIXELS];
float BRIGHTNESS = 100;              // 0 is darkest, 100 is brightest
int SPEED = 100;                     // 1 is slowest, 100 is fastest
float COLOR[3] = {255, 255, 255};    //R, G, B
int R = 250, G = 0, B = 0;           //for rainbow mode use
bool R_rise = false, R_fall = false; //for rainbow mode use
bool G_rise = false, G_fall = false; //for rainbow mode use
bool B_rise = false, B_fall = false; //for rainbow mode use

int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; //MPU6050

int light_stat = 101;
int last_state;
int count = 0;

String data;
String url;
int httpCode;
String web_data;
HTTPClient http;

TaskHandle_t LED_UPDATE;
TaskHandle_t WIFI_HANDLE;

void setup()
{
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, PIN, GRB>(pixels, NUMPIXELS);
    fire_staff();
    wifi_connect();
    OTA();
    mpu_init();

    xTaskCreatePinnedToCore(
        WIFI_HANDLE_CODE,
        "wifi_handle",
        10000,
        NULL,
        1,
        &WIFI_HANDLE,
        0);
    delay(100);

    xTaskCreatePinnedToCore(
        LED_UPDATE_CODE, // Function to implement the task
        "led_update",    // Name of the task
        10000,           // Stack size in words
        NULL,            // Task input parameter
        1,               // Priority of the task
        &LED_UPDATE,     // Task handle.
        1);              // Core where the task should run
    delay(100);
}

void LED_UPDATE_CODE(void *pvParameters)
{
    Serial.print("Task0 running on core ");
    Serial.println(xPortGetCoreID());
    int mpu_count = 0;
    while (1)
    {
        FastLED.setBrightness(BRIGHTNESS);
        if (light_stat != last_state)
        {
            last_state = light_stat;
            FastLED.clear();
            FastLED.show();
            count = 0;
        }
        if (light_stat == 0)
        {
            digitalWrite(2, HIGH);
            FastLED.clear();
            FastLED.show();
        }
        else if (light_stat == 1)
        {
            digitalWrite(2, LOW);
            for (int i = 0; i < NUMPIXELS; i++)
            {
                pixels[i] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
                //pixels.setPixelColor(i, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
        }
        else if (light_stat == 101)
        {
            sickle();
        }
        else if (light_stat == 111)
        {
            reverse_sickle();
        }
        else if (light_stat == 102)
        {
            fan();
        }
        else if (light_stat == 103)
        {
            fire_staff();
        }
        else if (light_stat == 203)
        {
            star();
        }
        else if (light_stat == 104)
        {
            R > 250 ? R = 250 : 1;
            G > 250 ? G = 250 : 1;
            B > 250 ? B = 250 : 1;
            R < 0 ? R = 0 : 1;
            G < 0 ? G = 0 : 1;
            B < 0 ? B = 0 : 1;
            rainbow();
        }
        else if (light_stat == 201)
        {
            es1();
        }
        else if (light_stat == 211)
        {
            rev_es1();
        }
        else if (light_stat == 202)
        {
            es2();
        }
        else if (light_stat == 204)
        {
            lightning();
        }

        //MPU6050
        if (mpu_count % 10 == 0)
        {
            mpu_count = 0;
            mpu_read();
            //Serial.print("Accelerometer Values: ");
            /*Serial.print("AcX: ");
            Serial.print(AcX);
            Serial.print(" AcY: ");
            Serial.print(AcY);
            Serial.print(" AcZ: ");
            Serial.print(AcZ);
            Serial.print(" Temperature: ");
            Serial.print(Tmp);
            //Serial.print(" Gyroscope Values: \n");
            Serial.print(" GyX: ");
            Serial.print(GyX);
            Serial.print(" GyY: ");
            Serial.print(GyY);
            Serial.print(" GyZ: ");
            Serial.print(GyZ);
            Serial.print("\n");*/
        }
        mpu_count++;
        //delayMicroseconds(100);
        //delay(10);
        //Serial.println(light_stat);
        //Serial.println("test");
    }
}

//vTaskDelete(NULL);
void WIFI_HANDLE_CODE(void *pvParameters)
{
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    while (1)
    {
        /*if (Serial.available())
        {
            light_stat = Serial.parseInt();
        }
        delay(10);*/
        get_data();
        delay(10);
    }
}

void loop()
{
    ArduinoOTA.handle();
}
