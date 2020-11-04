#ifndef QUEUE_H
#define QUEUE_H
#include "modes.h"
#include <Arduino.h>

#define QUEUE_SIZE 10

class Queue{
private:
    Mode queue[QUEUE_SIZE];
    int head;
    int tail;

    void addHead(Mode* input);
    int getCapcity();
    int nextIndex(int index);
public:
    Queue();
    bool is_empty();
    bool is_full();
    bool enqueue(Mode* input);
    bool dequeue(Mode* output);
    void print();
};

#endif