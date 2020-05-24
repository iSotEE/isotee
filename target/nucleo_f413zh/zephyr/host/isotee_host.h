/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

__syscall void isotee_para_initialize(void *guest_interrupt_handler);
__syscall int isotee_driver_usb_dc_stm32_clock_enable(void);
