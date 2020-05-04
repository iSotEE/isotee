/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include "isotee_para.h"

LOG_MODULE_REGISTER(isotee_guest);

#define CYC_PER_TICK (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC	\
		      / CONFIG_SYS_CLOCK_TICKS_PER_SEC)

static u32_t announced_cycles;

static void clock_isr() {
    u32_t current_cycles = isotee_para_context->timer_cycles;
	u32_t dticks = (current_cycles - announced_cycles) / CYC_PER_TICK;;
	announced_cycles += dticks * CYC_PER_TICK;

//    LOG_ERR("announce %d ticks.", dticks);
	z_clock_announce(dticks);
}

void z_interrupt_handler() {
//    LOG_ERR("iSotEE guest IRQ handler entry.");
    clock_isr();
    *isotee_para_context_interrupt_suppressed = 0;
}

void main() {
    LOG_ERR("iSotEE guest thread entry.");
    isotee_para_initialize();
    *isotee_para_context_interrupt_suppressed = 0;
    announced_cycles = isotee_para_context->timer_cycles;
    for (uint32_t i = 0;; i++) {
        LOG_ERR("iSotEE guest thread %lu.", i);
        k_sleep(1000);
    }
}
