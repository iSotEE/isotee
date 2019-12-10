/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * Type for (virtual) interrupt number (< ISOTEE_VIRTUAL_INTERRUPT_NUMBER)
 */
typedef uint32_t isotee_irq_t;

/**
 * Pointer to the shared READ-ONLY paravirtualization context.
 */
extern const volatile isotee_para_context_t* isotee_para_context;

/**
 * Initialize paravirtualization service, such as:
 * 1. Obtain and set isotee_para_context
 * 2. Register interrupt handler
 *
 * @param interrupt_handler function address of the common interrupt handler
 */
void isotee_para_initialize();

/**
 * Enable the virtual interrupt controller.
 * (i.e. Enter interrupt-disabled section)
 */
void isotee_para_interrupt_enable();

/**
 * Disable the virtual interrupt controller.
 * (i.e. Exit interrupt-disabled section)
 */
void isotee_para_interrupt_disable();

/**
 * Enter interrupt-suppressed critical section.
 * Suppress incoming interrupt requests.
 */
void isotee_para_interrupt_suppress_on();

/**
 * Exit interrupt-suppressed critical section.
 */
void isotee_para_interrupt_suppress_off();

/**
 * Switch to interrupt handler if pending interrupts exist.
 * Must be called outside critical section.
 */
void isotee_para_interrupt_pending_check();

/**
 * Clear interrupt pending status.
 */
void isotee_para_interrupt_pending_clear();

/**
 * Mask an interrupt.
 * @param irq interrupt number
 */
void isotee_para_interrupt_mask(isotee_irq_t irq);

/**
 * Unmask an interrupt.
 * @param irq interrupt number
 */
void isotee_para_interrupt_unmask(isotee_irq_t irq);

/**
 * Non-blocking receive an interrupt.
 * @return received interrupt number (value >= ISOTEE_VIRTUAL_INTERRUPT_NUMBER means not any)
 */
isotee_irq_t isotee_para_interrupt_poll();

/**
 * Read the monotonic virtual timer
 * @return count value in cycles
 */
uint32_t isotee_para_timer_read();

/**
 * Interrupt handler should be provided by guest OS.
 */
void isotee_para_guest_interrupt_handler();
