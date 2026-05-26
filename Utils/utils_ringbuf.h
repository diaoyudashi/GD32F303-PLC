#ifndef __UTILS_RINGBUF_H
#define __UTILS_RINGBUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t head;
    volatile uint16_t tail;
} RingBuf;

void RingBuf_Init(RingBuf *rb, uint8_t *buf, uint16_t size);
bool RingBuf_Put(RingBuf *rb, uint8_t data);
bool RingBuf_Get(RingBuf *rb, uint8_t *data);
uint16_t RingBuf_Available(const RingBuf *rb);
bool RingBuf_IsEmpty(const RingBuf *rb);
bool RingBuf_IsFull(const RingBuf *rb);
void RingBuf_Flush(RingBuf *rb);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_RINGBUF_H */
