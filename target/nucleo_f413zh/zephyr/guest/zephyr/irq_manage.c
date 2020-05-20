/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <arch/arm/cortex_m/cmsis.h>
#include "isotee_para.h"

static const uint32_t physical_irqn_to_virtual[CONFIG_NUM_IRQS] = {
    [I2C3_ER_IRQn] = ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER
};

void z_arch_irq_enable(unsigned int irq)
{
    //TODO: mask
	//NVIC_EnableIRQ((IRQn_Type)irq);
}

#if 0 // TODO: support this
void z_arch_irq_disable(unsigned int irq)
{
    //TODO: unmask
	//NVIC_DisableIRQ((IRQn_Type)irq);
}

int z_arch_irq_is_enabled(unsigned int irq)
{
    //TODO: check
	//return NVIC->ISER[REG_FROM_IRQ(irq)] & BIT(BIT_FROM_IRQ(irq));
}
#endif

void z_irq_priority_set(unsigned int irq, unsigned int prio, u32_t flags)
{
	ARG_UNUSED(irq);
	ARG_UNUSED(prio);
	ARG_UNUSED(flags);

    /* DO NOTHING for iSotEE guest */
}

void z_irq_spurious(void *unused)
{
	ARG_UNUSED(unused);
    // TODO: LOG_ERR
}

#if 0 // TODO: support this
void z_arch_isr_direct_header(void)
{
	z_sys_trace_isr_enter();
}
#endif
