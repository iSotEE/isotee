/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
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
volatile uint32_t*isotee_para_context_interrupt_status_bitmap = NULL;

void isotee_para_initialize() {
    isotee_para_interrupt_disable();
	isotee_para_context = (void*) isotee_host_syscall_invoke1((intptr_t)isotee_para_guest_interrupt_handler, HOST_AWARE_HYPERCALL_PARA_INITIALIZE);
	isotee_para_context_interrupt_suppressed = isotee_para_context->interrupt_suppressed;
	isotee_para_context_interrupt_pending = isotee_para_context->interrupt_pending;
	isotee_para_context_interrupt_status_bitmap = isotee_para_context->interrupt_status_bitmap;
    *isotee_para_context_interrupt_suppressed = 1U;
}
