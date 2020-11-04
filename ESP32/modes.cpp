#include "modes.h"

extern int count;
extern CRGB pixels[NUMPIXELS];
extern float BRIGHTNESS;    // 0 is darkest, 100 is brightest
extern int SPEED;           // 1 is slowest, 100 is fastest
extern float COLOR[3];      //R, G, B
extern int R, G, B;         //for rainbow mode use
extern bool R_rise, R_fall; //for rainbow mode use
extern bool G_rise, G_fall; //for rainbow mode use
extern bool B_rise, B_fall; //for rainbow mode use

extern int light_stat;
extern int last_state;
extern int count;

int sickle_temp = 0;
int sickle_i = NUMPIXELS / 2;
int reverse_sickle_i = 0;
int fan_j = 0;
int es1_i = 0;
int es2_i = 31;
int rev_es1_i = 0;
int star_i = 0;
int lightning_i = 0;

void sickle() //鐮刀 101
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        pixels[sickle_i - 1] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
        //pixels.setPixelColor(sickle_i - 1, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
        FastLED.show();
        pixels[sickle_i + sickle_temp] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
        //pixels.setPixelColor(sickle_i + sickle_temp, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
        FastLED.show();
        sickle_temp += 2;
        if (sickle_i == 0)
        {
            count = -SPEED;
            sickle_i = NUMPIXELS / 2 + 1;
            sickle_temp = 0;
        }
        sickle_i--;
    }
    count++;
}

void reverse_sickle() //反向鐮刀 111
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        pixels[reverse_sickle_i] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
        //pixels.setPixelColor(reverse_sickle_i, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
        FastLED.show();
        pixels[NUMPIXELS - 1 - reverse_sickle_i] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
        //pixels.setPixelColor(NUMPIXELS - 1 - reverse_sickle_i, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
        FastLED.show();
        if (reverse_sickle_i == NUMPIXELS / 2 - 1)
        {
            count = -SPEED;
            reverse_sickle_i = -1;
        }
        reverse_sickle_i++;
    }
    count++;
}

void fan() //風扇 102
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        FastLED.clear();
        if (fan_j < 8)
        {
            for (int i = fan_j; i < NUMPIXELS; i += 8)
            {
                pixels[i] = CRGB(COLOR[0], COLOR[1], COLOR[2]);
                //pixels.setPixelColor(i, pixels.Color(COLOR[0] * BRIGHTNESS / 100.0, COLOR[1] * BRIGHTNESS / 100.0, COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            fan_j++;
        }
        else
        {
            count = -SPEED;
            fan_j = 0;
        }
    }
    count++;
}

//橫版工科
void es1() //工科 201
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        FastLED.clear();
        if (es1_i < 34)
        {
            for (int j = NUMPIXELS - 16; j < NUMPIXELS; j++)
            {
                float temp;
                ES1[33 - es1_i][NUMPIXELS - 1 - j] ? (temp = 1.0) : (temp = 0.0);
                pixels[j] = CRGB(temp * COLOR[0], temp * COLOR[1], temp * COLOR[2]);
                //pixels.setPixelColor(j, pixels.Color(temp * COLOR[0] * BRIGHTNESS / 100.0, temp * COLOR[1] * BRIGHTNESS / 100.0, temp * COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            es1_i++;
        }
        else
        {
            count = -SPEED;
            es1_i = 0;
        }
    }
    count++;
}

void es2() //ES 202
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        if (es2_i < 19)
        {
            FastLED.clear();
            for (int j = 14; j < NUMPIXELS; j++)
            {
                float temp;
                ES2[18 - es2_i][NUMPIXELS - 1 - j] ? (temp = 1.0) : (temp = 0.0);
                pixels[j] = CRGB(temp * COLOR[0], temp * COLOR[1], temp * COLOR[2]);
                //pixels.setPixelColor(j, pixels.Color(temp * COLOR[0] * BRIGHTNESS / 100.0, temp * COLOR[1] * BRIGHTNESS / 100.0, temp * COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            es2_i++;
        }
        else
        {
            count = -SPEED;
            es2_i = 0;
        }
    }
    count++;
}

void rev_es1() //工科填滿 211
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        if (rev_es1_i < 34)
        {
            FastLED.clear();
            for (int j = 16; j < NUMPIXELS; j++)
            {
                float temp;
                ES1[33 - rev_es1_i][NUMPIXELS - 1 - j] ? (temp = 0.0) : (temp = 1.0);
                pixels[j] = CRGB(temp * COLOR[0], temp * COLOR[1], temp * COLOR[2]);
                //pixels.setPixelColor(j, pixels.Color(temp * COLOR[0] * BRIGHTNESS / 100.0, temp * COLOR[1] * BRIGHTNESS / 100.0, temp * COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            rev_es1_i++;
        }
        else
        {
            count = -SPEED;
            rev_es1_i = 0;
        }
    }
    count++;
}

void fire_staff() //火棍 103
{
    //pixels.clear();
    for (int i = NUMPIXELS - 1; i >= NUMPIXELS - 7; i--)
    {
        pixels[i] = CRGB(255, 0, 0);
        //pixels.setPixelColor(i, pixels.Color(255 * BRIGHTNESS / 100.0, 0, 0));
    }
    FastLED.show();
    //delay(SPEED);
}

void star() //星星 203
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        if (star_i < 16)
        {
            FastLED.clear();
            for (int j = 0; j < NUMPIXELS; j++)
            {
                float temp;
                STAR[15 - star_i][NUMPIXELS - 1 - j] ? (temp = 1.0) : (temp = 0.0);
                pixels[j] = CRGB(temp * COLOR[0], temp * COLOR[1], temp * COLOR[2]);
                //pixels.setPixelColor(j, pixels.Color(temp * COLOR[0] * BRIGHTNESS / 100.0, temp * COLOR[1] * BRIGHTNESS / 100.0, temp * COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            star_i++;
        }
        else
        {
            count = -SPEED;
            star_i = 0;
        }
    }
    count++;
}

void rainbow() //彩虹 104
{
    //RGB 255,0,0 255,128,0 255,255,0 0,255,0 0,255,255 0,0,255 128,0,255 2上1下3上2下1上3下
    //RGB 簡化 250,0,0 255,125,0 250,250,0 0,250,0 0,250,250 0,0,250 125,0,250 2上1下3上2下1上3下
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        FastLED.clear();
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels[i] = CRGB(R, G, B);
            //pixels.setPixelColor(i, pixels.Color((float)R * BRIGHTNESS / 100.0, (float)G * BRIGHTNESS / 100.0, (float)B * BRIGHTNESS / 100.0));
            if (R >= 250)
                R_rise = false;
            if (G >= 250)
                G_rise = false;
            if (B >= 250)
                B_rise = false;
            if (R <= 0)
                R_fall = false;
            if (G <= 0)
                G_fall = false;
            if (B <= 0)
                B_fall = false;
            if (R >= 250 && G <= 0 && B <= 0)
                G_rise = true; //2上
            if (R >= 250 && G >= 250 && B <= 0)
                R_fall = true; //1下
            if (R <= 0 && G >= 250 && B <= 0)
                B_rise = true; //3上
            if (R <= 0 && G >= 250 && B >= 250)
                G_fall = true; //2下
            if (R <= 0 && G <= 0 && B >= 250)
                R_rise = true; //1上
            if (R >= 250 && G <= 0 && B >= 250)
                B_fall = true; //3下

            if (R_rise)
                R += 50;
            if (G_rise)
                G += 50;
            if (B_rise)
                B += 50;
            if (R_fall)
                R -= 50;
            if (G_fall)
                G -= 50;
            if (B_fall)
                B -= 50;
        }
        FastLED.show();
    }
    count++;
}

void lightning() //閃電 204
{
    if (count == 0)
    {
        FastLED.clear();
    }
    if (count % SPEED == 0)
    {
        FastLED.clear();
        if (lightning_i < 14)
        {
            for (int j = 5; j < NUMPIXELS; j++)
            {
                float temp;
                LIGHTNING[13 - lightning_i][NUMPIXELS - 1 - j] ? (temp = 1.0) : (temp = 0.0);
                pixels[j] = CRGB(temp * COLOR[0], temp * COLOR[1], temp * COLOR[2]);
                //pixels.setPixelColor(j, pixels.Color(temp * COLOR[0] * BRIGHTNESS / 100.0, temp * COLOR[1] * BRIGHTNESS / 100.0, temp * COLOR[2] * BRIGHTNESS / 100.0));
            }
            FastLED.show();
            lightning_i++;
        }
        else
        {
            count = -SPEED;
            lightning_i = 0;
        }
    }
    count++;
}
