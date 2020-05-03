/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Driver for the timer model of the POSIX native_posix board
 * It provides the interfaces required by the kernel and the sanity testcases
 * It also provides a custom k_busy_wait() which can be used with the
 * POSIX arch and InfClock SOC
 */

#include "zephyr/types.h"
#include "irq.h"
#include "device.h"
#include "drivers/system_timer.h"
#include "sys_clock.h"
#include "soc.h"

/* Stub implementations. See "legacy_api.h". */
void z_clock_set_timeout(s32_t ticks, bool idle) {}
u32_t z_clock_elapsed(void) { return 0; }

static u64_t tick_period; /* System tick period in number of hw cycles */
static s64_t silent_ticks;
static s32_t _sys_idle_elapsed_ticks = 1;

/**
 * Return the current HW cycle counter
 * (number of microseconds since boot in 32bits)
 */
u32_t z_timer_cycle_get_32(void)
{
#if 0 // TODO: support
	return hwm_get_time();
#endif
}

/**
 * Interrupt handler for the timer interrupt
 * Announce to the kernel that a tick has passed
 */
static void sp_timer_isr(void *arg)
{
	ARG_UNUSED(arg);
	_sys_idle_elapsed_ticks = silent_ticks + 1;
	silent_ticks = 0;
	z_clock_announce(_sys_idle_elapsed_ticks);
}

/*
 * Enable the hw timer, setting its tick period, and setup its interrupt
 */
int z_clock_driver_init(struct device *device)
{
#if 0 // TODO: support
	ARG_UNUSED(device);

	tick_period = 1000000ul / CONFIG_SYS_CLOCK_TICKS_PER_SEC;

	hwtimer_enable(tick_period);

	IRQ_CONNECT(TIMER_TICK_IRQ, 1, sp_timer_isr, 0, 0);
	irq_enable(TIMER_TICK_IRQ);
#endif

	return 0;
}
