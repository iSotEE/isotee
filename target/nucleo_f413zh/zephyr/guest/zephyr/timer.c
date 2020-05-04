/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "drivers/system_timer.h"
#include "isotee_para.h"

u32_t z_clock_elapsed(void) {
    /* Stub implementations. See "legacy_api.h". */
    return 0;
}

u32_t z_timer_cycle_get_32(void) {
    /* Return the current HW cycle counter */
    return isotee_para_context->timer_cycles;
}
