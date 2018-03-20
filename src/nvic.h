#pragma once

#include <stddef.h>

enum {
    IRQ_OFFSET = -16,
    IRQ_NMI = -14,
    IRQ_HARD_FAULT = -13,
    IRQ_MEMMANG_FAULT = -12,
    IRQ_BUS_FAULT = -11,
    IRQ_USAGE_FAULT = -10,
    IRQ_SVCALL = -5,
    IRQ_PENDSV = -2,
    IRQ_SYSTICK = -1,
    IRQ_IRQ0 = 0,
};

typedef void (*irq_handler_func_t)(void);

#ifdef __cplusplus
extern "C" {
#endif

int nvic_set_handler(int irqn, irq_handler_func_t handler_func);
int nvic_dispatch(int irqn);

#ifdef __cplusplus
}  /* extern "C" */
#endif
