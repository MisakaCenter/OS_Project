#include "buffer.h"

buffer_item buffer[max_buf + 1];
int head, tail;

int insert_item(buffer_item item) {
    if (head == (tail + 1) % (max_buf + 1)) return -1; // buffer is full
    tail = (tail + 1) % (max_buf + 1);
    buffer[tail] = item;
    return 0;
}

int remove_item(buffer_item *item) {
    if (head == tail) return -1;
    head = (head + 1) % (max_buf + 1);
    *item = buffer[head];
    return 0;
}

void initial_buffer() {
    head = 0;
    tail = 0;
}