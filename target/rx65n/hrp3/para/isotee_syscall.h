/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define	SVC_SERVICE_CALL	1

typedef int32_t ER_UINT;
typedef int32_t FN;
#define Inline static inline

#pragma inline_asm _svc_service_call_0
static ER_UINT
_svc_service_call_0(FN fncd) {
	MOV.L	R1, R5
	INT		#SVC_SERVICE_CALL
}

#pragma inline_asm _svc_service_call_1
static ER_UINT
_svc_service_call_1(intptr_t par1, FN fncd) {
	MOV.L	R2, R5
	INT		#SVC_SERVICE_CALL
}

#pragma inline_asm _svc_service_call_2
static ER_UINT
_svc_service_call_2(intptr_t par1, intptr_t par2, FN fncd) {
	MOV.L	R3, R5
	INT		#SVC_SERVICE_CALL
}

#pragma inline_asm _svc_service_call_3
static ER_UINT
_svc_service_call_3(intptr_t par1, intptr_t par2, intptr_t par3, FN fncd) {
	MOV.L	R4, R5
	INT		#SVC_SERVICE_CALL
}

#pragma inline_asm _svc_service_call_4
static ER_UINT
_svc_service_call_4(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, FN fncd) {
	MOV.L	0[SP], R5
	INT		#SVC_SERVICE_CALL
}

#pragma inline_asm _svc_service_call_5
static ER_UINT
_svc_service_call_5(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, FN fncd) {
	MOV.L	0[SP], R14
	MOV.L	4[SP], R5
	INT		#SVC_SERVICE_CALL
}

#define CAL_SVC_0M(TYPE, FNCD) \
		return ((TYPE)_svc_service_call_0(FNCD))

#define CAL_SVC_1M(TYPE, FNCD, TYPE1, PAR1) \
		return ((TYPE)_svc_service_call_1((intptr_t)(PAR1), FNCD))

#define CAL_SVC_2M(TYPE, FNCD, TYPE1, PAR1, TYPE2, PAR2) \
		return ((TYPE)_svc_service_call_2((intptr_t)(PAR1), (intptr_t)(PAR2), FNCD))

#define CAL_SVC_3M(TYPE, FNCD, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3) \
		return ((TYPE)_svc_service_call_3((intptr_t)(PAR1), (intptr_t)(PAR2), (intptr_t)(PAR3), FNCD))

#define CAL_SVC_4M(TYPE, FNCD, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3, TYPE4, PAR4) \
		return ((TYPE)_svc_service_call_4((intptr_t)(PAR1), (intptr_t)(PAR2), (intptr_t)(PAR3), (intptr_t)(PAR4), FNCD))

#define CAL_SVC_5M(TYPE, FNCD, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3, TYPE4, PAR4, TYPE5, PAR5) \
		return ((TYPE)_svc_service_call_5((intptr_t)(PAR1), (intptr_t)(PAR2), (intptr_t)(PAR3), (intptr_t)(PAR4), (intptr_t)(PAR5), FNCD))

#define CAL_SVC_1M_SYSTIM(TYPE, FNCD, TYPE1, PAR1) \
		return ((TYPE)_svc_service_call_1_systim((TYPE1)(PAR1), FNCD))

uint8_t isotee_driver_r_ether_rx_initialize(const uint8_t mac_addr[]);
