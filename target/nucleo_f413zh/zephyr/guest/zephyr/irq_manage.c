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
    [I2C3_ER_IRQn] = ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER,
    [OTG_FS_IRQn] = ISOTEE_VIRTUAL_INTERRUPT_OTG_FS
};

void z_arch_irq_enable(unsigned int irq)
{
    isotee_para_interrupt_unmask(physical_irqn_to_virtual[irq]);
}

void z_arch_irq_disable(unsigned int irq)
{
    isotee_para_interrupt_mask(physical_irqn_to_virtual[irq]);
}

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
}
