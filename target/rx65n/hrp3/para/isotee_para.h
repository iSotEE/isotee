/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

typedef struct {
/* Readonly variables */
	void*    saved_pc;			/* MUST be first field */
	uint8_t  interrupt_enabled;
	uint32_t timer_cycles;
/* Writable variables */
	uint8_t* interrupt_suppressed;
} isotee_para_context_t;

#include "../../../isotee_para_common.h"

#define ISOTEE_CYCLES_PER_MS			(7500U)

#pragma inline isotee_timer_read_cycles
static uint32_t isotee_timer_read_cycles() {
	return isotee_para_context->timer_cycles;
}

#define SVC_PARA_INTERRUPT_ENABLE		3
#define SVC_PARA_INTERRUPT_DISABLE		4
//#define SVC_PARA_INTERRUPT_MASK			5
//#define SVC_PARA_INTERRUPT_UNMASK		6
#define SVC_PARA_INTERRUPT_POLL			7

/**
 * Virtual interrupts
 */
#define ISOTEE_VIRTUAL_INTERRUPT_R_ETHER	0
#define ISOTEE_VIRTUAL_INTERRUPT_NUMBER		1


#define TFN_ISOTEE_PARA_INITIALIZE	(30)

#define TFN_ISOTEE_DRIVER_R_ETHER_RX_INITIALIZE			(31)
#define TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2				(32)
#define TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2_BufRelease	(33)
#define TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_GetBuf		(34)
#define TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_SetBuf		(35)
#define TFN_ISOTEE_DRIVER_R_ETHER_LinkProcess			(36)
#define TFN_ISOTEE_DRIVER_R_ETHER_CheckWrite			(37)


static inline isotee_irq_t isotee_para_interrupt_poll() {	/* Use function instead of macro for calling conventions */
	__int_exception(SVC_PARA_INTERRUPT_POLL);
}
