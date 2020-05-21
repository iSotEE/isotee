/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include <sw_isr_table.h>
#include <arch/arm/cortex_m/cmsis.h>
#include "isotee_para.h"

LOG_MODULE_REGISTER(isotee_guest);

#define CYC_PER_TICK (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC	\
		      / CONFIG_SYS_CLOCK_TICKS_PER_SEC)

static u32_t announced_cycles;

static void clock_isr() {
    u32_t current_cycles = isotee_para_context->timer_cycles;
	u32_t dticks = (current_cycles - announced_cycles) / CYC_PER_TICK;;
	announced_cycles += dticks * CYC_PER_TICK;

// TODO:   LOG_ERR("announce %d ticks.", dticks);
	z_clock_announce(dticks);
}

typedef const struct _isr_table_entry *isr_t;

static const isr_t virtual_irqn_to_isr[ISOTEE_VIRTUAL_INTERRUPT_NUMBER] = {
    [ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER] = &_sw_isr_table[I2C3_ER_IRQn]
};

/**
 * Body of interrupt handling
 *
 * Preconditions:
 * 1) In interrupt-suppressed critical section
 *
 * Postconditions:
 * 1) In task critical section
 */
void z_interrupt_handler() {
// TODO:   LOG_ERR("iSotEE guest IRQ handler entry.");
//
    /* Lock interrupts to access status_bitmap exclusively */
    isotee_para_interrupt_disable();

    /* TODO: impl isotee_para_interrupt_poll using bitmap */
	isotee_irq_t irq;
	while ((irq = isotee_para_interrupt_poll()) < ISOTEE_VIRTUAL_INTERRUPT_NUMBER) {
// TODO:        LOG_ERR("iSotEE guest IRQ handler virtual irq %d.", irq);
        isr_t isr_entry = virtual_irqn_to_isr[irq];
        isr_entry->isr(isr_entry->arg);
		isotee_para_interrupt_unmask(irq);
	}

    clock_isr();

#if defined(CONFIG_ISOTEE_GUEST_USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
    isotee_para_interrupt_enable();
#else
	isotee_para_interrupt_suppress_off();
#endif
}

void main() {
    LOG_ERR("iSotEE guest thread entry.");
    isotee_para_initialize();
    isotee_para_interrupt_suppress_off();
    announced_cycles = isotee_para_context->timer_cycles;
    for (uint32_t i = 0;; i++) {
        LOG_ERR("iSotEE guest thread %lu.", i);
        k_sleep(10000U);
    }
}
