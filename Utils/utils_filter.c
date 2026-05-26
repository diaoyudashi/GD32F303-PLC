#include "utils_filter.h"

void LPF_Init(LPF_1st *f, float alpha)
{
    f->alpha = alpha;
    f->output = 0.0f;
    f->initialized = 0;
}

float LPF_Update(LPF_1st *f, float input)
{
    if (!f->initialized) {
        f->output = input;
        f->initialized = 1;
    } else {
        f->output = f->alpha * input + (1.0f - f->alpha) * f->output;
    }
    return f->output;
}

void LPF_Reset(LPF_1st *f)
{
    f->output = 0.0f;
    f->initialized = 0;
}

uint32_t LPF_UpdateInt(LPF_1st *f, uint32_t input)
{
    return (uint32_t)LPF_Update(f, (float)input);
}
