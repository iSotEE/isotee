/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include "isotee_para.h"

LOG_MODULE_REGISTER(isotee_guest);

void isotee_para_guest_interrupt_handler() {
    LOG_ERR("iSotEE guest IRQ handler entry.");
}

void _isotee_guest_startup() {
    LOG_ERR("iSotEE guest thread entry.");
    isotee_para_initialize();
    *isotee_para_context_interrupt_suppressed = 0;
    for (volatile uint32_t i = 0;; i++) {
        if ((i % 10000000U) == 0) {
            LOG_ERR("iSotEE guest thread %lu.", i);
            k_yield();
        }
    }
#if 0
    for (int i = 0;; i++) {
//        k_sleep(100U);
        k_busy_wait(100U * 1000U /*000U * 1000U*/);
//        LOG_ERR("iSotEE guest thread %d.", i);
    }
#endif
}

