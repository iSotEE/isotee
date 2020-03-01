/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ISOTEE_GUEST_MIN_PRI	(TMAX_INTPRI)
#define ISOTEE_GUEST_MAX_IPL	IPM_TO_IPL(ISOTEE_GUEST_MIN_PRI)

/*
 *  プロセッサ依存部（RX65N用）
 */
#define OMIT_CORE_SET_GET_IPM
#include "prc_kernel_impl.h"

#if TIPM_LOCK != TIPM_LOCK_ALL
#error Unsupported TIPM_LOCK
#endif

Inline void
t_set_ipm( PRI intpri )
{
	uint32_t ipl = IPM_TO_IPL( intpri );
	set_ipl((ipl > ISOTEE_GUEST_MAX_IPL) ? ipl : ISOTEE_GUEST_MAX_IPL);
}

Inline PRI
t_get_ipm( void )
{
	uint32_t ipl = current_ipl();
	if (ipl <= ISOTEE_GUEST_MAX_IPL) {
		return TIPM_ENAALL;
	}
	return IPL_TO_IPM( ipl );
}

extern void isotee_guest_inject_interrupt(uint32_t virtual_interrupt);

#define EXTSVC_PROTOTYPE(func) extern ER_UINT func(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid)


EXTSVC_PROTOTYPE(extsvc_isotee_para_initialize);

extern void isotee_guest_tick(intptr_t exinf);

#include "isotee_driver_r_ether_rx.h"
