#include "Essentials.h"
#include "Constants.h"
volatile uint8 led_buffer[3];
int buffer_excute_counter;
uint24 buffer_start_time;

void led_init(){
    pinMode(BTN_PIN, INPUT_PULLUP);
    pinMode(RED_LED_PIN,    OUTPUT);
    pinMode(GREEN_LED_PIN,  OUTPUT);
    pinMode(BLUE_LED_PIN,   OUTPUT);
    digitalWrite(RED_LED_PIN,   LED_CLOSE);
    digitalWrite(GREEN_LED_PIN, LED_CLOSE);
    digitalWrite(BLUE_LED_PIN,  LED_CLOSE);
}

void buffer_init(){
    led_buffer[LED_R_INDEX] = LED_CLOSE;
    led_buffer[LED_G_INDEX] = LED_CLOSE;
    led_buffer[LED_B_INDEX] = LED_CLOSE;
    buffer_excute_counter = 0;
    buffer_start_time = millis();
}

void inline led_update(){
    analogWrite(RED_LED_PIN,    led_buffer[LED_R_INDEX]);
    analogWrite(GREEN_LED_PIN,  led_buffer[LED_G_INDEX]);
    analogWrite(BLUE_LED_PIN,   led_buffer[LED_B_INDEX]);
}

uint24 inline get_buffer_start_time(){
    return (millis() - buffer_start_time);
}
/*The start time parameter is 4 bytes.*/
uint24 inline get_cmd_start_time(int index){
    return (uint24)cmd_buffer[index][BUFFER_START_TIME0_BIT] << 24 
         | (uint24)cmd_buffer[index][BUFFER_START_TIME1_BIT] << 16
         | (uint24)cmd_buffer[index][BUFFER_START_TIME2_BIT] << 8
         | (uint24)cmd_buffer[index][BUFFER_START_TIME3_BIT];
}
/*The duration parameter is 2 bytes.*/
uint16 inline get_cmd_duration(int index){
    return (uint16)cmd_buffer[index][BUFFER_DURATION0_BIT] << 8
         | (uint16)cmd_buffer[index][BUFFER_DURATION1_BIT];
}

/*The 3 leading parameter is 1 byte and the following 2 parameter is 2 bytes.*/
uint8 inline get_cmd_para_1(int index, int para){
    return (uint8)cmd_buffer[index][BUFFER_P1_BIT + para];
}
uint16 inline get_cmd_para_2(int index, int para){
    return (uint16)cmd_buffer[index][BUFFER_P1_BIT + para] << 8
         | (uint16)cmd_buffer[index][BUFFER_P1_BIT + para + 1];
}
uint24 inline get_cmd_para_3(int index, int para){
    return (uint24)cmd_buffer[index][BUFFER_P1_BIT + para] << 16
         | (uint24)cmd_buffer[index][BUFFER_P1_BIT + para + 1] << 8
         | (uint24)cmd_buffer[index][BUFFER_P1_BIT + para + 2];
}
void buffer_update(){
    /* if the task work done, move to next task. */
    if( get_buffer_start_time() > 
        get_cmd_start_time(buffer_excute_counter) + get_cmd_duration(buffer_excute_counter) ){
        buffer_excute_counter += 1;
        set_rgb(LED_CLOSE, LED_CLOSE, LED_CLOSE);
    }
    
    /* if command buffer is a empty task, reset the counter and wait for next call. */
    if( cmd_buffer[buffer_excute_counter][BUFFER_MODE_BIT] == LM_EMPTY ){
        buffer_init();
        return;
    }
    
    /* if the task time is about to start */
    if( get_buffer_start_time() > get_cmd_start_time(buffer_excute_counter) ){
        switch(cmd_buffer[buffer_excute_counter][BUFFER_MODE_BIT]){
            case LM_EMPTY:
            case LM_SET_CLOSE: set_rgb(LED_CLOSE, LED_CLOSE, LED_CLOSE); break;
            case LM_SET_RGB: set_rgb(
                get_cmd_para_1(buffer_excute_counter, 0), 
                get_cmd_para_1(buffer_excute_counter, 1), 
                get_cmd_para_1(buffer_excute_counter, 2)); break;
            case LM_SET_HSL: set_hsl(
                get_cmd_para_1(buffer_excute_counter, 0), 
                (float)get_cmd_para_1(buffer_excute_counter, 1)/255.0, 
                (float)get_cmd_para_1(buffer_excute_counter, 2)/255.0); break;
            case LM_SET_HSL_PROGRESSIVE: set_hsl_progressive(
                (get_buffer_start_time()-get_cmd_start_time(buffer_excute_counter)),
                get_cmd_duration(buffer_excute_counter),
                get_cmd_para_1(buffer_excute_counter, 0), //H
                get_cmd_para_1(buffer_excute_counter, 1), //S
                get_cmd_para_1(buffer_excute_counter, 2), //L
                get_cmd_para_1(buffer_excute_counter, 3), //colorTrans
                get_cmd_para_1(buffer_excute_counter, 4));//brightTrans                 
                break;
            case LM_SET_HSL_SPARK_ASYNC: set_hsl_spark_async(
                (get_buffer_start_time()-get_cmd_start_time(buffer_excute_counter)), 
                get_cmd_duration(buffer_excute_counter),
                get_cmd_para_1(buffer_excute_counter, 0), //H
                get_cmd_para_1(buffer_excute_counter, 1), //S
                get_cmd_para_1(buffer_excute_counter, 2), //L
                get_cmd_para_1(buffer_excute_counter, 3), //colorTrans
                get_cmd_para_1(buffer_excute_counter, 4), //brightTrans
                get_cmd_para_1(buffer_excute_counter, 5), //count
                get_cmd_para_1(buffer_excute_counter, 6));//duty
                break;
            case LM_SET_HSL_SPARK_SYNC: set_hsl_spark_sync(
                (get_buffer_start_time()-get_cmd_start_time(buffer_excute_counter)), 
                get_cmd_duration(buffer_excute_counter),
                get_cmd_para_1(buffer_excute_counter, 0), //H
                get_cmd_para_1(buffer_excute_counter, 1), //S
                get_cmd_para_1(buffer_excute_counter, 2), //L
                get_cmd_para_1(buffer_excute_counter, 3), //colorTrans
                get_cmd_para_1(buffer_excute_counter, 4), //brightTrans
                get_cmd_para_1(buffer_excute_counter, 5), //count
                get_cmd_para_1(buffer_excute_counter, 6));//duty
                break;
            case LM_SET_HSL_METEOR_ASYNC: set_hsl_meteor_async(
                (get_buffer_start_time()-get_cmd_start_time(buffer_excute_counter)), 
                get_cmd_duration(buffer_excute_counter),
                get_cmd_para_1(buffer_excute_counter, 0), //H
                get_cmd_para_1(buffer_excute_counter, 1), //S
                get_cmd_para_1(buffer_excute_counter, 2), //L
                get_cmd_para_1(buffer_excute_counter, 3), //colorTrans
                get_cmd_para_1(buffer_excute_counter, 4), //meteorTrans
                get_cmd_para_1(buffer_excute_counter, 5), //count
                get_cmd_para_1(buffer_excute_counter, 6));//duty
                break;
            case LM_SET_HSL_METEOR_SYNC: set_hsl_meteor_sync(
                (get_buffer_start_time()-get_cmd_start_time(buffer_excute_counter)), 
                get_cmd_duration(buffer_excute_counter),
                get_cmd_para_1(buffer_excute_counter, 0), //H
                get_cmd_para_1(buffer_excute_counter, 1), //S
                get_cmd_para_1(buffer_excute_counter, 2), //L
                get_cmd_para_1(buffer_excute_counter, 3), //colorTrans
                get_cmd_para_1(buffer_excute_counter, 4), //meteorTrans
                get_cmd_para_1(buffer_excute_counter, 5), //count
                get_cmd_para_1(buffer_excute_counter, 6));//duty
                break;        
        }
    }
}
/*
void hsv_test(){
    set_hsv(0, 255, 255);
    set_hsv(30, 255, 255);
    set_hsv(60, 255, 255);
    set_hsv(90, 255, 255);
    set_hsv(120, 255, 255);
    set_hsv(180, 255, 255);
    set_hsv(210, 255, 255);
    set_hsv(240, 255, 255);
    set_hsv(270, 255, 255);
    set_hsv(300, 255, 255);
    set_hsv(330, 255, 255);
    set_hsv(360, 255, 255);
}*/

void ISR_enable()
{
  TCCR2A = 0;
  TCCR2B = 0; 
  TCCR2B |= (1<<WGM22);  // CTC mode; Clear Timer on Compare
  TCCR2B |= (1<<CS22) | (1<<CS20);  // Prescaler == 8 ||(1<<CS30)
  TIMSK2 |= (1 << OCIE2A);  // enable CTC for TIMER1_COMPA_vect
  TCNT2=0;  // counter 歸零 
  OCR2A = 1000;
}

void ISR_disable()
{
  TCCR2A = 0;
  TCCR2B = 0; 
}


ISR(TIMER2_COMPA_vect)
{
    led_update();
}
