/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

typedef unsigned int u32_t;

#define _SVC_CALL_SYSTEM_CALL		3

static inline u32_t isotee_host_syscall_invoke1(u32_t arg1, u32_t call_id)
{
	register u32_t ret __asm__("r0") = arg1;
	register u32_t r6 __asm__("r6") = call_id;

	__asm__ volatile("svc %[svid]\n"
			 : "=r"(ret)
			 : [svid] "i" (_SVC_CALL_SYSTEM_CALL),
			   "r" (ret), "r" (r6)
			 : "r8", "memory", "r1", "r2", "r3");
	return ret;
}

static inline u32_t isotee_host_syscall_invoke0(u32_t call_id)
{
	register u32_t ret __asm__("r0");
	register u32_t r6 __asm__("r6") = call_id;

	__asm__ volatile("svc %[svid]\n"
			 : "=r"(ret)
			 : [svid] "i" (_SVC_CALL_SYSTEM_CALL),
			   "r" (ret), "r" (r6)
			 : "r8", "memory", "r1", "r2", "r3");

	return ret;
}
