#include "configs.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(NUM_PIXEL, PIN_LED_COMMU, NEO_GRB + NEO_KHZ800);

/* Timer for pixel update */
hw_timer_t *pixel_timer = NULL;
void IRAM_ATTR onPixelTimer() {
    //pixels.clear();
    for(int i=0; i<NUM_PIXEL; i++) {
        //pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    }
    //pixels.show();
}

void setup(){
    Serial.begin(115200);

    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.clear(); // Set all pixel colors to 'off'

    /* Pixel timer attach */
    // Use timer 0, Timer counts up(True), counts down(false)
    pixel_timer = timerBegin(1, PIXEL_PRESCALER, true);
    timerAttachInterrupt(pixel_timer, &onPixelTimer, true);
    // Counter will reload while reaching OCR(true)
    timerAlarmWrite(pixel_timer, PIXEL_OCR, true);
    timerAlarmEnable(pixel_timer);
    Serial.println("System ready");
}

void loop(){

}
