

#include "utils.h"

#define PIN_ENABLE 10
#define PIN_CLOCK 13
#define PIN_DATA 11

extern volatile uint8_t led_map[NUM_LEDS][3];
char str[] = "NCKU ES";

void setup()
{
    Serial.begin(115200);

    // setup led RGB output pin
    DDRD = B01100000;
    DDRB = B00000010;

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_DATA, OUTPUT);

    digitalWrite(PIN_ENABLE, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_DATA, LOW);
    ISR_enable();
}

int16_t color = 0;
void loop()
{
    for (int u = 7; u >= 0; u--) {
        for (int i = 0; i < NUM_LEDS; i++) {
            for (int k = 0; k < 3; k++) {
                led_map[i][k] = u;
            }
        }
        delay(200);
    }

    show_hsv_demo();

    for (int i = 0; i < 20; i++) {
        color = (color + get_len(str) * 4) % 360;
        show_str(str, color, 1, 10);
    }
}
