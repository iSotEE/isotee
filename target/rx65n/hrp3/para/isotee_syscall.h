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

/**
 * Function code for service calls
 */
#define TFN_ACT_TSK (-5)
#define TFN_GET_TST (-6)
#define TFN_CAN_ACT (-7)
#define TFN_EXT_TSK (-8)
#define TFN_TER_TSK (-9)
#define TFN_CHG_PRI (-10)
#define TFN_GET_PRI (-11)
#define TFN_GET_INF (-12)
#define TFN_SLP_TSK (-13)
#define TFN_TSLP_TSK (-14)
#define TFN_WUP_TSK (-15)
#define TFN_CAN_WUP (-17)
#define TFN_REL_WAI (-18)
#define TFN_SUS_TSK (-25)
#define TFN_RSM_TSK (-26)
#define TFN_DLY_TSK (-27)
#define TFN_RAS_TER (-29)
#define TFN_DIS_TER (-31)
#define TFN_ENA_TER (-32)
#define TFN_SNS_TER (-33)
#define TFN_SIG_SEM (-37)
#define TFN_WAI_SEM (-39)
#define TFN_POL_SEM (-40)
#define TFN_TWAI_SEM (-41)
#define TFN_SET_FLG (-45)
#define TFN_CLR_FLG (-47)
#define TFN_WAI_FLG (-48)
#define TFN_POL_FLG (-49)
#define TFN_TWAI_FLG (-50)
#define TFN_SND_DTQ (-53)
#define TFN_PSND_DTQ (-54)
#define TFN_TSND_DTQ (-56)
#define TFN_FSND_DTQ (-57)
#define TFN_RCV_DTQ (-59)
#define TFN_PRCV_DTQ (-60)
#define TFN_TRCV_DTQ (-61)
#define TFN_SND_PDQ (-65)
#define TFN_PSND_PDQ (-66)
#define TFN_TSND_PDQ (-68)
#define TFN_RCV_PDQ (-69)
#define TFN_PRCV_PDQ (-70)
#define TFN_TRCV_PDQ (-71)
#define TFN_SET_TIM (-73)
#define TFN_GET_TIM (-74)
#define TFN_ADJ_TIM (-75)
#define TFN_SET_DFT (-76)
#define TFN_LOC_MTX (-77)
#define TFN_PLOC_MTX (-78)
#define TFN_TLOC_MTX (-79)
#define TFN_UNL_MTX (-80)
#define TFN_SND_MBF (-81)
#define TFN_PSND_MBF (-82)
#define TFN_TSND_MBF (-83)
#define TFN_RCV_MBF (-84)
#define TFN_PRCV_MBF (-85)
#define TFN_TRCV_MBF (-86)
#define TFN_GET_MPF (-89)
#define TFN_PGET_MPF (-90)
#define TFN_TGET_MPF (-91)
#define TFN_REL_MPF (-92)
#define TFN_FCH_HRT (-94)
#define TFN_REF_OVR (-96)
#define TFN_STA_CYC (-97)
#define TFN_STP_CYC (-98)
#define TFN_STA_ALM (-101)
#define TFN_STP_ALM (-103)
#define TFN_STA_OVR (-105)
#define TFN_STP_OVR (-107)
#define TFN_SAC_SYS (-109)
#define TFN_REF_SYS (-110)
#define TFN_ROT_RDQ (-111)
#define TFN_GET_LOD (-112)
#define TFN_GET_DID (-113)
#define TFN_GET_TID (-115)
#define TFN_GET_NTH (-116)
#define TFN_LOC_CPU (-117)
#define TFN_UNL_CPU (-119)
#define TFN_DIS_DSP (-121)
#define TFN_ENA_DSP (-122)
#define TFN_SNS_CTX (-123)
#define TFN_SNS_LOC (-124)
#define TFN_SNS_DSP (-125)
#define TFN_SNS_DPN (-126)
#define TFN_SNS_KER (-127)
#define TFN_EXT_KER (-128)
#define TFN_ATT_MEM (-129)
#define TFN_DET_MEM (-130)
#define TFN_SAC_MEM (-131)
#define TFN_PRB_MEM (-132)
#define TFN_REF_MEM (-133)
#define TFN_ATT_PMA (-135)
#define TFN_CFG_INT (-137)
#define TFN_DIS_INT (-138)
#define TFN_ENA_INT (-139)
#define TFN_REF_INT (-140)
#define TFN_CHG_IPM (-141)
#define TFN_GET_IPM (-142)
#define TFN_XSNS_DPN (-145)
#define TFN_CLR_INT (-146)
#define TFN_RAS_INT (-147)
#define TFN_PRB_INT (-148)
#define TFN_REF_CFG (-149)
#define TFN_REF_VER (-150)
#define TFN_SAC_DOM (-153)
#define TFN_SLT_DOM (-154)
#define TFN_CHG_SOM (-157)
#define TFN_GET_SOM (-159)
#define TFN_INI_SEM (-162)
#define TFN_INI_FLG (-163)
#define TFN_INI_DTQ (-164)
#define TFN_INI_PDQ (-165)
#define TFN_INI_MTX (-167)
#define TFN_INI_MBF (-168)
#define TFN_INI_MPF (-169)
#define TFN_REF_TSK (-177)
#define TFN_REF_SEM (-178)
#define TFN_REF_FLG (-179)
#define TFN_REF_DTQ (-180)
#define TFN_REF_PDQ (-181)
#define TFN_REF_MTX (-183)
#define TFN_REF_MBF (-184)
#define TFN_REF_MPF (-185)
#define TFN_REF_CYC (-186)
#define TFN_REF_ALM (-187)
#define TFN_REF_ISR (-188)
#define TFN_REF_SPN (-189)
#define TFN_ACRE_TSK (-193)
#define TFN_ACRE_SEM (-194)
#define TFN_ACRE_FLG (-195)
#define TFN_ACRE_DTQ (-196)
#define TFN_ACRE_PDQ (-197)
#define TFN_ACRE_MTX (-199)
#define TFN_ACRE_MBF (-200)
#define TFN_ACRE_MPF (-201)
#define TFN_ACRE_CYC (-202)
#define TFN_ACRE_ALM (-203)
#define TFN_ACRE_ISR (-204)
#define TFN_ACRE_SPN (-205)
#define TFN_DEL_TSK (-209)
#define TFN_DEL_SEM (-210)
#define TFN_DEL_FLG (-211)
#define TFN_DEL_DTQ (-212)
#define TFN_DEL_PDQ (-213)
#define TFN_DEL_MTX (-215)
#define TFN_DEL_MBF (-216)
#define TFN_DEL_MPF (-217)
#define TFN_DEL_CYC (-218)
#define TFN_DEL_ALM (-219)
#define TFN_DEL_ISR (-220)
#define TFN_DEL_SPN (-221)
#define TFN_SAC_TSK (-225)
#define TFN_SAC_SEM (-226)
#define TFN_SAC_FLG (-227)
#define TFN_SAC_DTQ (-228)
#define TFN_SAC_PDQ (-229)
#define TFN_SAC_MTX (-231)
#define TFN_SAC_MBF (-232)
#define TFN_SAC_MPF (-233)
#define TFN_SAC_CYC (-234)
#define TFN_SAC_ALM (-235)
#define TFN_SAC_ISR (-236)
#define TFN_SAC_SPN (-237)
#define TFN_DEF_OVR (-242)
#define TFN_DEF_INH (-243)
#define TFN_DEF_EXC (-244)
#define TFN_DEF_SVC (-245)
#define TFN_MACT_TSK (-257)
#define TFN_MIG_TSK (-259)
#define TFN_CHG_SPR (-260)
#define TFN_MSTA_CYC (-261)
#define TFN_MSTA_ALM (-263)
#define TFN_MROT_RDQ (-265)
#define TFN_MGET_LOD (-266)
#define TFN_GET_PID (-267)
#define TFN_MGET_NTH (-268)
#define TFN_LOC_SPN (-273)
#define TFN_TRY_SPN (-275)
#define TFN_UNL_SPN (-277)

uint8_t isotee_driver_r_ether_rx_initialize(const uint8_t mac_addr[]);
