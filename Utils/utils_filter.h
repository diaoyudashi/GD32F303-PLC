#ifndef __UTILS_FILTER_H
#define __UTILS_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float alpha;
    float output;
    uint8_t initialized;
} LPF_1st;

void LPF_Init(LPF_1st *f, float alpha);
float LPF_Update(LPF_1st *f, float input);
void LPF_Reset(LPF_1st *f);
uint32_t LPF_UpdateInt(LPF_1st *f, uint32_t input);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_FILTER_H */
