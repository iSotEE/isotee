/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "isotee_para.h"
#include "isotee_syscall.h"

const volatile isotee_para_context_t* isotee_para_context;

void isotee_para_initialize(void *interrupt_handler) {
	isotee_para_context = _svc_service_call_1(interrupt_handler, TFN_ISOTEE_PARA_INITIALIZE);
}
