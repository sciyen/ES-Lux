#include "queue.h"


Queue::Queue(){
    head = tail = 0;
}

void Queue::addHead(Mode* input){
    head = nextIndex(head);
    queue[head] = *input;
}

int Queue::getCapcity(){
    if(tail - head >= 0)
       return tail - head;
    else 
       return tail + QUEUE_SIZE - head;
}

int Queue::nextIndex(int index){
    if(index <= 0)
        return QUEUE_SIZE-1;  //make the index in the range of buffer
    else
        return index-1;
}

bool Queue::is_full(){
    return nextIndex(head) == tail;
}

bool Queue::is_empty(){
    return getCapcity() <= 0;
}

bool Queue::enqueue(Mode* input){
    if( is_full() )
        return false;
    addHead(input);
    return true;
}

bool Queue::dequeue(Mode* output){
    if( is_empty() )   // Empty
        return false;
    *output = queue[tail];
    tail = nextIndex(tail);
    return true;
}

void Queue::print(){
    for (int i=0; i<QUEUE_SIZE; i++){
        Serial.print(queue[i].mode);
        Serial.print(", ");
    }
    Serial.print(head);
    Serial.print(", ");
    Serial.print(tail);
    Serial.println(", ");
}