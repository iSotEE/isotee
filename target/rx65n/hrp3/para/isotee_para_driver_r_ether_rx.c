/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "isotee_para.h"
#include "isotee_syscall.h"
#include "r_ether_rx_if.h"

uint8_t isotee_driver_r_ether_rx_initialize(const uint8_t mac_addr[]) {
	CAL_SVC_1M(uint8_t, TFN_ISOTEE_DRIVER_R_ETHER_RX_INITIALIZE, uint8_t[], mac_addr);
}

int32_t R_ETHER_Read_ZC2 (uint32_t channel, void **pbuf) {
	CAL_SVC_2M(int32_t, TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2, uint32_t, channel, void**, pbuf);
}

int32_t R_ETHER_Read_ZC2_BufRelease (uint32_t channel) {
	CAL_SVC_1M(int32_t, TFN_ISOTEE_DRIVER_R_ETHER_Read_ZC2_BufRelease, uint32_t, channel);
}

ether_return_t R_ETHER_Write_ZC2_GetBuf (uint32_t channel, void **pbuf, uint16_t *pbuf_size) {
	CAL_SVC_3M(ether_return_t, TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_GetBuf, uint32_t, channel, void**, pbuf, uint16_t*, pbuf_size);
}

ether_return_t R_ETHER_Write_ZC2_SetBuf (uint32_t channel, const uint32_t len) {
	CAL_SVC_2M(ether_return_t, TFN_ISOTEE_DRIVER_R_ETHER_Write_ZC2_SetBuf, uint32_t, channel, uint32_t, len);
}

void R_ETHER_LinkProcess (uint32_t channel) {
	_svc_service_call_1((intptr_t)(channel), TFN_ISOTEE_DRIVER_R_ETHER_LinkProcess);
}

ether_return_t R_ETHER_CheckWrite (uint32_t channel) {
	CAL_SVC_1M(ether_return_t, TFN_ISOTEE_DRIVER_R_ETHER_CheckWrite, uint32_t, channel);
}
