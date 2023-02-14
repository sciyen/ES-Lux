#include "queue.h"

Queue q;
void setup(){
    for(int i=0; i<8; i++){
        Mode m;
        m.mode = i;
        m.start_time = i;
        q.enqueue(&m);
    }
    Serial.begin(9600);
}

void loop(){
    /*for (int i=0; i<10; i++){
        Mode m;
        q.dequeue(&m);
        Serial.print(m.mode);
        Serial.print(", ");
    }*/
    for (int i=0; i<5; i++){
        q.print();
        Mode m;
        if (q.dequeue(&m) ){
            Serial.println(m.mode);
            Serial.println(m.start_time);
        }
        else 
        Serial.println("empty");
        delay(1000);
    }
    
    for (int i=0; i<4; i++){
        q.print();
        Mode m;
        m.mode = 10+i;
        m.start_time = 10+i;
        q.enqueue(&m);
    }
}
