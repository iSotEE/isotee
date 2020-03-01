/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * Driver 'r_ether_rx'
 */

EXTSVC_PROTOTYPE(extsvc_isotee_driver_r_ether_rx_initialize);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_Read_ZC2);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_Read_ZC2_BufRelease);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_Write_ZC2_GetBuf);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_Write_ZC2_SetBuf);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_LinkProcess);
EXTSVC_PROTOTYPE(extsvc_R_ETHER_CheckWrite);

#define INTNO_R_ETHER	INT_GROUPAL1
#define INHNO_R_ETHER	INTNO_R_ETHER
extern void isotee_driver_r_ether_isr();
