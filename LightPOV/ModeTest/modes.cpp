#include "modes.h"

#define LIMIT_OUTPUT(x) ((x)>0xff)?0xff:((x)<0?0:(x))

/* Create ramp-like function 
 *   /     /     upper
 *  /     /
 * +     +       lower
 * |range|
 */ 
uint8_t ColorScheduler::calc_ramp(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow){
    uint8_t mod = idx % range;
    int16_t output = lower + (uint16_t)mod * (upper - lower) / range;
    return (!overflow)?LIMIT_OUTPUT(output):output;
    //return (!overflow && output > 0xff) ? 255 : output;
}

/* Create triangular-like function 
 *   / \   /    upper
 *  /   \ /
 * +     +      lower
 * |range|
 */ 
uint8_t ColorScheduler::calc_tri(uint16_t idx, uint8_t range, uint8_t lower, uint8_t upper, bool overflow){
    uint8_t delta = upper - lower;
    uint8_t half = range / 2;
    int16_t mod = idx % range;
    uint16_t output = lower + abs((int16_t)mod - half) * delta / half;
    return (!overflow)?LIMIT_OUTPUT(output):output;
    //return (!overflow && output > 0xff) ? 255 : output;
}

/* Create pulse-like function 
 *  |\        upper
 *  | \  
 *  |   --    lower
 * @Param:
 *      top: length of peak time
 * # TODO: the `lower` parameter is useless now
 */ 
uint8_t ColorScheduler::calc_pulse(uint16_t idx, uint8_t range, uint8_t lower, uint8_t top){
    uint8_t mod = idx - (idx/range) * range;
    if (mod < top) return 0xff;
    uint16_t decay = (uint16_t)(idx - top) * 8 / range;
    return 0xff >> decay;
}

ColorScheduler::ColorScheduler(time_t _effect_entry_time, uint8_t defaultV){
    effect_entry_time = _effect_entry_time;
    defaultValue = defaultV;
}

void ColorScheduler::SetXHsvParam(ValueParam H, ValueParam S, ValueParam V){
    XHp = H; XSp = S; XVp = V;
}

void ColorScheduler::SetYHsvParam(ValueParam H, ValueParam S, ValueParam V){
    YHp = H; YSp = S; YVp = V;
}

uint8_t ColorScheduler::getFuncValue(ValueParam* vp, uint16_t idx, bool overflow){
    switch (vp->func){
        case FuncConst:
            return vp->p1;
        case FuncRamp:
            return calc_ramp(idx, vp->range, vp->lower, vp->p1, true);
        case FuncTri:
            return calc_tri(idx, vp->range, vp->lower, vp->p1);
        case FuncPulse:
            return calc_pulse(idx, vp->range, vp->lower, vp->p1);
        default:
            return defaultValue;
    }
}

void ColorScheduler::updateHeading(time_t now_time, time_t last_low_time, bool restart){
    static time_t delta=0;
    if (restart)    delta = now_time - last_low_time;
    else            delta = now_time - effect_entry_time;

    headingColor.h = getFuncValue(&XHp, delta);
    headingColor.s = getFuncValue(&XSp, delta);
    headingColor.v = getFuncValue(&XVp, delta);
}

CHSV inline ColorScheduler::getPixelColor(uint8_t y){/*
    return CHSV(headingColor.h,
                headingColor.s,
                headingColor.v);*/
    return CHSV(headingColor.h + getFuncValue(&YHp, y),
                headingColor.s + getFuncValue(&YSp, y),
                headingColor.v + getFuncValue(&YVp, y));
}

void ColorScheduler::getPixelColor(CHSV* pixels, uint32_t bitmap, CHSV replace){
    for( uint8_t i=0; i<NUMPIXELS; i++){
        if (bitmap & 1)
            pixels[i] = getPixelColor(i);
        else pixels[i] = replace;
        bitmap = bitmap >> 1;
    }
}

void ColorScheduler::getPixelColor(CHSV* pixels){
    for( uint8_t i=0; i<NUMPIXELS; i++){
        pixels[i] = getPixelColor(i);
    }
}

Effects::Effects(){
    FastLED.addLeds<WS2812B, PIN_LED, GRB>(rgb_pixels, NUMPIXELS);
}

void inline Effects::showLED(){
    hsv2rgb_rainbow(pixels, rgb_pixels, NUMPIXELS);
    FastLED.show();
}

uint16_t Effects::get_idx(time_t now_time, time_t start_time, time_t interval){
    return (now_time - start_time) * 0xff / 500;
}

void Effects::update(){
    detector.update();
}

/* Block until pass through start position */
void Effects::blockUntilStart(time_t start, uint16_t timeout){
    time_t start_time = millis();
    while( detector.read_flag() != Rotation_SET && (millis()-start_time < timeout)) 
        ;
    detector.clear_flag();
}

bool inline Effects::detectPassStart(time_t start){
    if ( detector.read_flag() ){
        detector.clear_flag();
        return true;
    }
    return false;
}

/*
 * param[0]: position_fix
 * param[1]: times
 */
void Effects::square(Mode* m){
    uint8_t position_fix = m->param[0];
    uint8_t times = m->param[1];
    time_t now_time;
    uint32_t bitmap = 0b11001100110011001100110011001100;
    ColorScheduler sch = ColorScheduler( millis() );
    sch.SetXHsvParam(m->XH, m->XS, m->XV);
    sch.SetYHsvParam(m->YH, m->YS, m->YV);
    for (int i=0; i<times; i++){
        for (int b=0; b<20; b++){
            now_time = millis();
            uint16_t idx = get_idx(now_time, sch.effect_entry_time, detector.interval);
            sch.updateHeading(idx, detector.last_low_time, position_fix);
            sch.getPixelColor(pixels);
            
            /*
            Serial.print(sch.headingColor.h);
            Serial.print(",");
            Serial.print(sch.headingColor.s);
            Serial.print(",");
            Serial.print(sch.headingColor.v);
            Serial.println(",");
            for (int a=0; a<NUMPIXELS; a++){
                Serial.print(rgb_pixels[a].r);
                Serial.print("|");
                Serial.print(rgb_pixels[a].g);
                Serial.print("|");
                Serial.print(rgb_pixels[a].b);
                Serial.print(",");
            }
                Serial.println(",");*/
/*
            if ( (now_time-sch.effect_entry_time) & 0x10 )
                sch.getPixelColor(pixels, bitmap, CHSV(0, 0, 0));
            else
                sch.getPixelColor(pixels, bitmap>>2, CHSV(0, 0, 0));*/
            showLED();
        }
        if (position_fix)
            blockUntilStart(0, 1000);
    }
}
/*
void Effects::sickle() //鐮刀 101
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
*/
