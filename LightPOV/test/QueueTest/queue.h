#ifndef QUEUE_H
#define QUEUE_H
#include <Arduino.h>
#define QUEUE_SIZE 10
enum MODES{MODES_EMPTY=0};

typedef struct _mode{
    MODES mode;
    uint32_t start_time;
    uint32_t duration;
}Mode;

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
