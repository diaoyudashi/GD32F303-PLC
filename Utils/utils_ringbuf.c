#include "utils_ringbuf.h"

void RingBuf_Init(RingBuf *rb, uint8_t *buf, uint16_t size)
{
    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

bool RingBuf_Put(RingBuf *rb, uint8_t data)
{
    uint16_t next = (rb->head + 1) % rb->size;
    if (next == rb->tail) {
        return false;
    }
    rb->buffer[rb->head] = data;
    rb->head = next;
    return true;
}

bool RingBuf_Get(RingBuf *rb, uint8_t *data)
{
    if (rb->head == rb->tail) {
        return false;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

uint16_t RingBuf_Available(const RingBuf *rb)
{
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    }
    return rb->size - rb->tail + rb->head;
}

bool RingBuf_IsEmpty(const RingBuf *rb) { return rb->head == rb->tail; }
bool RingBuf_IsFull(const RingBuf *rb)  { return ((rb->head + 1) % rb->size) == rb->tail; }

void RingBuf_Flush(RingBuf *rb)
{
    rb->head = 0;
    rb->tail = 0;
}
