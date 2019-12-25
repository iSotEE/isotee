/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

typedef struct {
/* Readonly variables */
	void*    saved_pc;			/* MUST be first field */
	uint32_t timer_cycles;
/* Writable variables */
	uint8_t* interrupt_suppressed;
	uint8_t* interrupt_pending;
} isotee_para_context_t;

#include "../../../isotee_para_common.h"

#define ISOTEE_CYCLES_PER_MS			(7500U)

extern volatile uint8_t* isotee_para_context_interrupt_suppressed;
extern volatile uint8_t* isotee_para_context_interrupt_pending;

#define SVC_PARA_INTERRUPT_ENABLE		3
#define SVC_PARA_INTERRUPT_DISABLE		4
#define SVC_PARA_INTERRUPT_MASK			5
#define SVC_PARA_INTERRUPT_UNMASK		6
#define SVC_PARA_INTERRUPT_POLL			7

/**
 * Virtual interrupts
 */
#define ISOTEE_VIRTUAL_INTERRUPT_R_ETHER	0
#define ISOTEE_VIRTUAL_INTERRUPT_TEST_SWINT	1
#define ISOTEE_VIRTUAL_INTERRUPT_NUMBER		2


#define TFN_ISOTEE_PARA_INITIALIZE	(30)

#define TFN_ISOTEE_DRIVER_R_ETHER_RX_INITIALIZE			(31)
#define TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2				(32)
#define TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2_BufRelease	(33)
#define TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_GetBuf		(34)
#define TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_SetBuf		(35)
#define TFN_ISOTEE_DRIVER_R_ETHER_LinkProcess			(36)
#define TFN_ISOTEE_DRIVER_R_ETHER_CheckWrite			(37)

#pragma inline_asm isotee_para_interrupt_enable
static void isotee_para_interrupt_enable() { /* Use inline_asm instead of macro for calling conventions */
	int #SVC_PARA_INTERRUPT_ENABLE
}

#pragma inline_asm isotee_para_interrupt_disable
static void isotee_para_interrupt_disable() { /* Use inline_asm instead of macro for calling conventions */
	int #SVC_PARA_INTERRUPT_DISABLE
}

#pragma inline isotee_para_interrupt_suppress_on
static void isotee_para_interrupt_suppress_on() {
	*isotee_para_context_interrupt_suppressed = 1U;
}

#pragma inline isotee_para_interrupt_suppress_off
static void isotee_para_interrupt_suppress_off() {
	*isotee_para_context_interrupt_suppressed = 0U;
}

#pragma inline isotee_para_interrupt_pending_clear
static void isotee_para_interrupt_pending_clear() {
	*isotee_para_context_interrupt_pending = 0U;
}

void isotee_para_interrupt_pending_handler();

#pragma inline_asm isotee_para_interrupt_mask
static void isotee_para_interrupt_mask(isotee_irq_t irq) { /* Use inline_asm instead of macro for calling conventions */
	int #SVC_PARA_INTERRUPT_MASK
}

#pragma inline_asm isotee_para_interrupt_unmask
static void isotee_para_interrupt_unmask(isotee_irq_t irq) { /* Use inline_asm instead of macro for calling conventions */
	int #SVC_PARA_INTERRUPT_UNMASK
}

#pragma inline_asm isotee_para_interrupt_poll
static isotee_irq_t isotee_para_interrupt_poll() { /* Use inline_asm instead of macro for calling conventions */
	int #SVC_PARA_INTERRUPT_POLL
}

#pragma inline isotee_para_timer_read
static uint32_t isotee_para_timer_read() {
	return isotee_para_context->timer_cycles;
}

