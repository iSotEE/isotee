/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "kernel/kernel_impl.h"
#include "kernel/task.h"
#include "kernel_cfg.h"
#include "../para/isotee_para.h"

typedef struct {
	void     *pc;
	uint32_t  psw;
} itf_t;

#if ISOTEE_CYCLES_PER_MS != HRTCNT_TO_CMWCNT(1000U)
#error wrong ISOTEE_CYCLES_PER_MS
#endif

#pragma section B B_ISOTEE_SHARED_RO
isotee_para_context_t isotee_guest_context;
#pragma section


#pragma section B B_ISOTEE_SHARED_RW
uint8_t isotee_guest_interrupt_suppressed;
#pragma section

uint32_t isotee_guest_ipl = ISOTEE_GUEST_MAX_IPL;
uint8_t  isotee_guest_running = 0;
itf_t*   isotee_guest_itfp = NULL; /* Interruptible Trap Frame Pointer */
void*    isotee_guest_interrupt_handler = NULL;
uint8_t  isotee_guest_interrupt_status[ISOTEE_VIRTUAL_INTERRUPT_NUMBER];

TCB* const isotee_guest_tcb = get_tcb(ISOTEE_GUEST_TASK);

ER_UINT
extsvc_isotee_para_initialize(intptr_t interrupt_handler, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid)
{
	isotee_guest_interrupt_handler = (void*)interrupt_handler;
	isotee_guest_interrupt_suppressed = 1;
	isotee_guest_context.interrupt_suppressed = &isotee_guest_interrupt_suppressed;
	memset(isotee_guest_interrupt_status, 0, sizeof(isotee_guest_interrupt_status));
	isotee_guest_tick(0);
	sta_cyc(ISOTEE_GUEST_TICK_CYC);
	return((ER_UINT) &isotee_guest_context);
}

static inline
void modify_trap_frame() {
	SIL_PRE_LOC;
	SIL_LOC_INT();
	if (isotee_guest_itfp != NULL) {
		if (!isotee_guest_interrupt_suppressed && isotee_guest_ipl == 0 && (isotee_guest_itfp->psw & PSW_PM_MASK) != 0) {
			isotee_guest_context.saved_pc = isotee_guest_itfp->pc;
			isotee_guest_itfp->pc = isotee_guest_interrupt_handler;
			isotee_guest_interrupt_suppressed = 1;
		}
		isotee_guest_itfp = NULL;
	}
	SIL_UNL_INT();
}

void isotee_guest_tick(intptr_t exinf) {
	modify_trap_frame();
	isotee_guest_context.timer_cycles = sil_rew_mem((void*)CMTW0_CMWCNT_ADDR);
}

void isotee_guest_inject_interrupt(uint32_t virtual_interrupt) {
	if (isotee_guest_ipl != 0) { // For debug
		syslog(LOG_NOTICE, "isotee_guest_ipl != 0");
		while(1);
	}

	isotee_guest_interrupt_status[virtual_interrupt] = 1;
	modify_trap_frame();
}

/*
void isotee_guest_tick(intptr_t exinf) {
	if (isotee_guest_itfp != NULL) {
		if (!isotee_guest_interrupt_suppressed && isotee_guest_ipl == 0) {
//			syslog(LOG_NOTICE, "timer modify itfp");
			isotee_guest_context.saved_pc = isotee_guest_itfp->pc;
			isotee_guest_itfp->pc = isotee_guest_interrupt_handler;
			isotee_guest_interrupt_suppressed = 1;
		}
		isotee_guest_itfp = NULL;
	}
	isotee_guest_context.timer_cycles = sil_rew_mem((void*)CMTW0_CMWCNT_ADDR);
	sta_alm(ISOTEE_GUEST_TICK, 100U * 1000U);
}


void isotee_guest_inject_interrupt(uint32_t virtual_interrupt) {
	isotee_guest_interrupt_status[virtual_interrupt] = 1;
	SIL_PRE_LOC;
	SIL_LOC_INT();
	if (isotee_guest_itfp != NULL) {
		if (!isotee_guest_interrupt_suppressed) {
//			syslog(LOG_NOTICE, "virtual int modify itfp");
			if (isotee_guest_ipl != 0) {
				syslog(LOG_NOTICE, "isotee_guest_ipl != 0");
			}
			isotee_guest_context.saved_pc = isotee_guest_itfp->pc;
			isotee_guest_itfp->pc = isotee_guest_interrupt_handler;
			isotee_guest_interrupt_suppressed = 1;
		}
		isotee_guest_itfp = NULL;
	}
	SIL_UNL_INT();

}
*/
