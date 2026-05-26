#include <stddef.h>
#include "app_system.h"
#include "app_config.h"
#include "app_led.h"

typedef struct {
    uint32_t          interval;
    uint32_t          last_tick;
    uint8_t           repeat;
    uint8_t           running;
    SoftTimerCallback callback;
} SoftTimer;

static SystemState sys_state = SYS_INIT;
static volatile uint32_t sys_tick = 0;
static SoftTimer soft_timers[SOFTTIMER_MAX] = {0};

void APP_System_Init(void)
{
    sys_state = SYS_INIT;
    sys_tick = 0;
    for (uint8_t i = 0; i < SOFTTIMER_MAX; i++) {
        soft_timers[i].callback = NULL;
    }
}

void APP_System_Run(void)
{
    uint32_t now = sys_tick;

    /* Process soft timers */
    for (uint8_t i = 0; i < SOFTTIMER_MAX; i++) {
        if (soft_timers[i].callback && soft_timers[i].running) {
            if (now - soft_timers[i].last_tick >= soft_timers[i].interval) {
                soft_timers[i].last_tick = now;
                soft_timers[i].callback();
                if (!soft_timers[i].repeat) {
                    soft_timers[i].running = 0;
                }
            }
        }
    }
}

/* Called from SysTick_Handler */
void APP_System_TickInc(void)
{
    sys_tick++;
}

SystemState APP_System_GetState(void)        { return sys_state; }
void     APP_System_SetState(SystemState s)  { sys_state = s; }
uint32_t APP_System_GetTick(void)            { return sys_tick; }

int8_t APP_System_TimerCreate(uint32_t interval_ms, uint8_t repeat, SoftTimerCallback cb)
{
    for (uint8_t i = 0; i < SOFTTIMER_MAX; i++) {
        if (soft_timers[i].callback == NULL) {
            soft_timers[i].interval = interval_ms;
            soft_timers[i].repeat = repeat;
            soft_timers[i].running = 0;
            soft_timers[i].last_tick = 0;
            soft_timers[i].callback = cb;
            return (int8_t)i;
        }
    }
    return -1;
}

void APP_System_TimerStart(int8_t id)
{
    if (id >= 0 && id < SOFTTIMER_MAX && soft_timers[id].callback) {
        soft_timers[id].last_tick = sys_tick;
        soft_timers[id].running = 1;
    }
}

void APP_System_TimerStop(int8_t id)
{
    if (id >= 0 && id < SOFTTIMER_MAX) {
        soft_timers[id].running = 0;
    }
}

void APP_System_TimerDelete(int8_t id)
{
    if (id >= 0 && id < SOFTTIMER_MAX) {
        soft_timers[id].callback = NULL;
        soft_timers[id].running = 0;
    }
}
