/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef _ASMLANGUAGE

typedef struct {
/* Readonly variables */
	void*    saved_pc;			/* MUST be first field */
	uint32_t timer_cycles;
/* Pointers to writable variables */
	uint8_t* interrupt_suppressed;
	uint8_t* interrupt_pending;
    uint32_t*interrupt_status_bitmap;
} isotee_para_context_t;

#define isotee_para_interrupt_enable        DEFAULT_isotee_para_interrupt_enable
#define isotee_para_interrupt_disable       DEFAULT_isotee_para_interrupt_disable
#define isotee_para_interrupt_suppress_on   DEFAULT_isotee_para_interrupt_suppress_on
#define isotee_para_interrupt_suppress_off  DEFAULT_isotee_para_interrupt_suppress_off
#define isotee_para_interrupt_mask          DEFAULT_isotee_para_interrupt_mask
#define isotee_para_interrupt_unmask        DEFAULT_isotee_para_interrupt_unmask
#define isotee_para_interrupt_poll          DEFAULT_isotee_para_interrupt_poll
#include "../../../isotee_para_common.h"
#undef isotee_para_interrupt_enable
#undef isotee_para_interrupt_disable
#undef isotee_para_interrupt_suppress_on
#undef isotee_para_interrupt_suppress_off
#undef isotee_para_interrupt_mask
#undef isotee_para_interrupt_unmask
#undef isotee_para_interrupt_poll

#if CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC != 96000000
#error "The host and guest must have same value."
#endif

extern volatile uint8_t* isotee_para_context_interrupt_suppressed;
extern volatile uint8_t* isotee_para_context_interrupt_pending;
extern volatile uint32_t*isotee_para_context_interrupt_status_bitmap;

#endif /* _ASMLANGUAGE */

#define BARE_METAL_HYPERCALL_INTERRUPT_ENABLE   (4)
#define BARE_METAL_HYPERCALL_INTERRUPT_DISABLE  (5)
#define BARE_METAL_HYPERCALL_INTERRUPT_MASK     (6)
#define BARE_METAL_HYPERCALL_INTERRUPT_UNMASK   (7)
#define BARE_METAL_HYPERCALL_INTERRUPT_POLL     (8)

#define HOST_AWARE_HYPERCALL_DRIVER_USB_DC_STM32_CLOCK_ENABLE   (56)
#define HOST_AWARE_HYPERCALL_PARA_INITIALIZE                    (57)

/**
 * Virtual interrupts
 */
#define ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER    0
#define ISOTEE_VIRTUAL_INTERRUPT_OTG_FS     1
#define ISOTEE_VIRTUAL_INTERRUPT_NUMBER     2

#ifndef _ASMLANGUAGE

static inline void isotee_para_interrupt_enable() {
	__asm__ volatile("svc %[svid]\n": /* OUT */ : /* IN */ [svid] "i" (BARE_METAL_HYPERCALL_INTERRUPT_ENABLE) : "memory");
}

static inline uint32_t isotee_para_interrupt_disable() { /* Use inline_asm instead of macro for calling conventions */
	register uint32_t ret __asm__("r0");
	__asm__ volatile("svc %[svid]\n": /* OUT */ "=r"(ret) : /* IN */ [svid] "i" (BARE_METAL_HYPERCALL_INTERRUPT_DISABLE) : "memory");
    __asm__ volatile("": : :"memory");
    return ret;
}

static inline void isotee_para_interrupt_suppress_on() {
	*isotee_para_context_interrupt_suppressed = 1U;
}

static inline void isotee_para_interrupt_suppress_off() {
	*isotee_para_context_interrupt_suppressed = 0U;
}

static inline void isotee_para_interrupt_mask(isotee_irq_t irq) {
	register isotee_irq_t ret __asm__("r0") = irq;

	__asm__ volatile("svc %[svid]\n"
			 : "=r"(ret)
			 : [svid] "i" (BARE_METAL_HYPERCALL_INTERRUPT_MASK),
			   "r" (ret)
			 : "memory", "r1", "r2", "r3");
}

static inline void isotee_para_interrupt_unmask(isotee_irq_t irq) {
	register isotee_irq_t ret __asm__("r0") = irq;

	__asm__ volatile("svc %[svid]\n"
			 : "=r"(ret)
			 : [svid] "i" (BARE_METAL_HYPERCALL_INTERRUPT_UNMASK),
			   "r" (ret)
			 : "memory", "r1", "r2", "r3");
}

static inline isotee_irq_t isotee_para_interrupt_poll() {
    uint32_t pending_bitmap = *isotee_para_context_interrupt_status_bitmap;
    isotee_irq_t ret = __builtin_clz(pending_bitmap);
    *isotee_para_context_interrupt_status_bitmap = (pending_bitmap << (ret + 1)) >> (ret + 1);
    return ret;
}

#endif /* _ASMLANGUAGE */
