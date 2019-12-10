/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "isotee_para.h"
#include "isotee_syscall.h"

static uint8_t init_suppressed = 0;
static uint8_t init_pending = 0;

const volatile isotee_para_context_t* isotee_para_context;
volatile uint8_t* isotee_para_context_interrupt_suppressed = &init_suppressed;
volatile uint8_t* isotee_para_context_interrupt_pending = &init_pending;

void isotee_para_initialize() {
	isotee_para_context = _svc_service_call_1(isotee_para_guest_interrupt_handler, TFN_ISOTEE_PARA_INITIALIZE);
	isotee_para_context_interrupt_suppressed = isotee_para_context->interrupt_suppressed;
	isotee_para_context_interrupt_pending = isotee_para_context->interrupt_pending;

	if (isotee_para_initialize == (void*)0x1 && isotee_para_initialize != (void*)0x1) {
		/* Symbol references to make compiler happy */
		isotee_para_interrupt_pending_handler();
	}
}

#pragma inline_asm isotee_para_interrupt_pending_check
void isotee_para_interrupt_pending_check() {
	mov.l	#_isotee_para_context_interrupt_pending, r1
	mov.l	[r1], r1
	mov.b	[r1], r1
	mov.l	#_isotee_para_context_interrupt_suppressed, r2
	mov.l	[r2], r2
	cmp		#0, r1
	bz		no_pending_interrupt	/* Return if no pending interrupt */
	mov.b	#1, [r2]				/* Enter interrupt-suppressed critical section */
	pushm	r1-r5					/* Space for r1 - r5 */
	bra		_isotee_para_interrupt_pending_handler
no_pending_interrupt:
}
