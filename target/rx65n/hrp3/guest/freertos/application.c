/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "../../para/isotee_para.h"

extern void EINT_Trig_isr();

void* const isotee_isr_table[ISOTEE_VIRTUAL_INTERRUPT_NUMBER] = {
	[ISOTEE_VIRTUAL_INTERRUPT_R_ETHER] = EINT_Trig_isr
};

long write(long fileno, const unsigned char *buf, long count) {
	/* NOT SUPPORTED BY DEFAULT */
	/* unused */ ((void)fileno);
	/* unused */ ((void)buf);
	/* unused */ ((void)count);
    vAssertCalled();
	return -1;
}
