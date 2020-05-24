/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <sw_isr_table.h>
#include <arch/arm/cortex_m/cmsis.h>
#include <init.h>
#include <drivers/system_timer.h>
#include "isotee_para.h"

#define CYC_PER_TICK (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC	\
		      / CONFIG_SYS_CLOCK_TICKS_PER_SEC)

bool arch_is_in_isr;

static u32_t announced_cycles;

static void clock_isr() {
    u32_t current_cycles = isotee_para_context->timer_cycles;
	u32_t dticks = (current_cycles - announced_cycles) / CYC_PER_TICK;;
	announced_cycles += dticks * CYC_PER_TICK;

	z_clock_announce(dticks);
}

typedef const struct _isr_table_entry *isr_t;

static const isr_t virtual_irqn_to_isr[ISOTEE_VIRTUAL_INTERRUPT_NUMBER] = {
    [ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER] = &_sw_isr_table[I2C3_ER_IRQn],
    [ISOTEE_VIRTUAL_INTERRUPT_OTG_FS] = &_sw_isr_table[OTG_FS_IRQn]
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
    /* Lock interrupts to access status_bitmap exclusively */
    isotee_para_interrupt_disable();

    arch_is_in_isr = true;

	isotee_irq_t irq;
	while ((irq = isotee_para_interrupt_poll()) < ISOTEE_VIRTUAL_INTERRUPT_NUMBER) {
        isr_t isr_entry = virtual_irqn_to_isr[irq];
        isr_entry->isr(isr_entry->arg);
		isotee_para_interrupt_unmask(irq);
	}

    clock_isr();

    arch_is_in_isr = false;

#if defined(CONFIG_ISOTEE_GUEST_USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
    isotee_para_interrupt_enable();
#else
	isotee_para_interrupt_suppress_off();
#endif
}

static int isotee_guest_init(struct device *unused) {
    isotee_para_initialize();
    announced_cycles = isotee_para_context->timer_cycles;
    isotee_para_interrupt_suppress_off();
    isotee_para_interrupt_enable();
    return 0;
}

SYS_INIT(isotee_guest_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

void k_cpu_idle() {
#if defined(CONFIG_ISOTEE_GUEST_USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
	isotee_para_interrupt_suppress_off();
#else
    isotee_para_interrupt_enable();
#endif
    while(1);
}
