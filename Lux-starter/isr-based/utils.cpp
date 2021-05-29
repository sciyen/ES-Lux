#include "utils.h"

volatile uint8_t led_map[NUM_LEDS][3] = {0};

int get_len(char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

void show_leds(uint8_t value)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        for (int k = 0; k < 3; k++) {
            led_map[i][k] = value;
        }
    }
}

void show_str(char *str, int16_t color, int8_t dc, int clear_len)
{
    uint8_t C[3] = {8, 8, 8};
    // int16_t dc = 360 / (get_len(str) * 8);
    for (int idx = get_len(str) - 1; idx >= 0; idx--) {
        for (int w = 7; w >= 0; w--) {
            if (color != -1) {
                set_hsl(color, 1, 0.5, &(C[0]), &(C[1]), &(C[2]));
                color = (color + dc) % 360;
            }
            for (int h = 0; h < 8; h++) {
                bool set = ((font8x8_basic[str[idx]][7 - h] >> w) % 2);
                for (int k = 0; k < 3; k++) {
                    if (set) {
                        led_map[2 * h][k] = C[k];
                        led_map[2 * h + 1][k] = C[k];
                    } else {
                        led_map[2 * h][k] = 8;
                        led_map[2 * h + 1][k] = 8;
                    }
                }
            }
            delay(1);
        }
    }

    show_leds(8);
    delay(clear_len);
}

void show_hsv_demo()
{
    for (uint16_t color = 0; color < 360; color++) {
        uint8_t R, G, B;
        set_hsl(color, 1, 0.5, &R, &G, &B);
        for (int i = 0; i < NUM_LEDS; i++) {
            led_map[i][0] = R;
            led_map[i][1] = G;
            led_map[i][2] = B;
        }
        delay(10);
    }
}

void show_bitmap_logo()
{
    for (int i = 0; i < 8; i++) {
        uint8_t R, G, B;
        for (int h = 0; h < NUM_LEDS; h++) {
            R = ((LOGO[i][h] >> 6) & B111);
            G = ((LOGO[i][h] >> 3) & B111);
            B = ((LOGO[i][h]) & B111);
            led_map[h][0] = 8 - R;
            led_map[h][1] = 8 - G;
            led_map[h][2] = 8 - B;
        }
        delay(2);
    }
}

void ISR_enable()
{
    // atmega 328p timer register setting
    // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328

    ISR_disable();
    // 10k = 16M / 64 / 50
    OCR2A = 5;  // 50

    TCCR2A |= (1 << WGM21);
    // Set to CTC Mode

    TIMSK2 |= (1 << OCIE2A);
    // Set interrupt on compare match

    TCCR2B |= (1 << CS21) | (1 << CS20);
    // set prescaler to 32 and starts PWM

    sei();
}

void ISR_disable()
{
    TCCR2A = 0;
    TCCR2B = 0;
}

// atmega 328p port manipulation
// https://www.arduino.cc/en/Reference/PortManipulation
// Port registers B (digital pin 8 to 13)
// Port registers C (analog input pins)
// Port registers D (digital pins 0 to 7)

ISR(TIMER2_COMPA_vect)
{
    static volatile uint8_t clock_pulse = 0;
    // counter for the simulated pwm
    uint8_t current_led = clock_pulse >> 3;
    uint8_t remainder = clock_pulse & B0111;

    if (remainder == 0) {
        PORTD |= B01100000;  //&= B10011111;  //
        PORTB |= B00000010;  //&= B11111101;  //
        // update to next led
        PORTB &= B11111011;  // reset enable
        if (current_led >= NUM_LEDS) {
            // create pulse
            PORTB &= B11110111;  // reset data
            PORTB |= B00100000;  // set   clock
            PORTB ^= B00100000;  // reset clock
            PORTB |= B00001000;  // set   data
            clock_pulse = 0;

            // recalculate parameters
            current_led = clock_pulse >> 3;
            remainder = clock_pulse & B0111;
        } else {
            PORTB |= B00100000;  // set   clock
            PORTB ^= B00100000;  // reset clock
        }
    }

    // set led output
    PORTD &= ~((led_map[current_led][0] <= remainder) << 5);
    PORTD &= ~((led_map[current_led][2] <= remainder) << 6);
    PORTB &= ~((led_map[current_led][1] <= remainder) << 1);

    PORTB |= B00000100;  // set enable
    clock_pulse++;
}

float hue_to_rgb(float v1, float v2, float vH)
{
    if (vH < 0)
        vH += 1;
    if (vH > 1)
        vH -= 1;
    if ((6 * vH) < 1)
        return (v1 + (v2 - v1) * 6 * vH);
    if ((2 * vH) < 1)
        return v2;
    if ((3 * vH) < 2)
        return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);
    return v1;
};

void set_hsl(int H, float S, float L, uint8_t *br, uint8_t *bg, uint8_t *bb)
{
    uint8_t R, G, B;

    if (S == 0) {
        R = G = B = (unsigned char) (L * 255);
    } else {
        float v1, v2;
        float hue = (float) H / 360;

        v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
        v1 = 2 * L - v2;

        R = (unsigned char) (8 * hue_to_rgb(v1, v2, hue + (1.0f / 3)));
        G = (unsigned char) (8 * hue_to_rgb(v1, v2, hue));
        B = (unsigned char) (8 * hue_to_rgb(v1, v2, hue - (1.0f / 3)));
    }
    *br = 8 - R;
    *bg = 8 - G;
    *bb = 8 - B;
};