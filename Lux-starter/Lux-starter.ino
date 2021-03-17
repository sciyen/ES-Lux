#define PIN_ENABLE 10
#define PIN_CLOCK 13
#define PIN_DATA 11

#define NUM_LEDS 16

volatile uint16_t clock_pulse = 0;
volatile uint8_t led_map[NUM_LEDS] = {0};
void setup()
{
    //DDRC = B00111111;
    DDRD = B11111100;
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_DATA, OUTPUT);

    digitalWrite(PIN_ENABLE, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_DATA, LOW);
    ISR_enable();
}

void loop()
{
    for (int u = 0; u < 4; u++)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            led_map[i] = (u << 4) + (u << 2) + u;
        }
        delay(1000);
    }
}

void ISR_enable()
{
    ISR_disable();
    //1k = 16M / 64 / 250
    OCR2A = 60;

    TCCR2A |= (1 << WGM21);
    // Set to CTC Mode

    TIMSK2 |= (1 << OCIE2A);
    //Set interrupt on compare match

    TCCR2B |= (1 << CS21) | (1 << CS20);
    // set prescaler to 64 and starts PWM

    sei();
}

void ISR_disable()
{
    TCCR2A = 0;
    TCCR2B = 0;
}

ISR(TIMER2_COMPA_vect)
{
    //sei();
    digitalWrite(PIN_ENABLE, LOW);

    if (clock_pulse >= 16)
    {
        // create pulse
        digitalWrite(PIN_DATA, LOW);
        PORTB |= B00100000;
        PORTB ^= B00100000;
        //digitalWrite(PIN_CLOCK, HIGH);
        //digitalWrite(PIN_CLOCK, LOW);
        digitalWrite(PIN_DATA, HIGH);
        clock_pulse = 0;
    }
    else
    {
        digitalWrite(PIN_CLOCK, HIGH);
        digitalWrite(PIN_CLOCK, LOW);
    }

    // set led output
    //PORTC = B00111111 & led_map[clock_pulse];
    PORTD = B11111100 & led_map[clock_pulse] << 2;
    clock_pulse++;

    digitalWrite(PIN_ENABLE, HIGH);
    //cli();
}