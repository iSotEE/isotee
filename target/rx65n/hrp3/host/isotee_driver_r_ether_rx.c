#include <t_syslog.h>
#include "kernel/kernel_impl.h"
#include "../para/isotee_para.h"
#include "r_ether_rx_if.h"

#if defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
#if ETHER_CFG_MODE_SEL == 0
#define R_ETHER_PinSet_CHANNEL_0()  R_ETHER_PinSet_ETHERC0_MII()
#elif ETHER_CFG_MODE_SEL == 1
#define R_ETHER_PinSet_CHANNEL_0()  R_ETHER_PinSet_ETHERC0_RMII()
#endif
#elif defined(BSP_MCU_RX63N)
#if ETHER_CFG_MODE_SEL == 0
#define R_ETHER_PinSet_CHANNEL_0()  R_ETHER_PinSet_ETHERC_MII()
#elif ETHER_CFG_MODE_SEL == 1
#define R_ETHER_PinSet_CHANNEL_0()  R_ETHER_PinSet_ETHERC_RMII()
#endif
#endif

#if (ETHER_CFG_EINT_INT_PRIORITY != 1) || (ETHER_CFG_AL1_INT_PRIORTY != 1)
#error Illegal interrupt priority for R_ETHER
#endif

static void EINT_handler(void *ectrl) {
	uint32_t eesr = ((ether_cb_arg_t*)ectrl)->status_eesr;
    if (eesr & 0x00040000) { // EDMAC FR (Frame Receive Event) interrupt
    	isotee_guest_inject_interrupt(ISOTEE_VIRTUAL_INTERRUPT_R_ETHER);
    }
#if 0 // TODO: for debug
    eesr &=~0x00040000;
    if (eesr & (1 << 16)) { // EDMAC RFOF
    	syslog(LOG_NOTICE, "EDMAC RFOF");
    }
    eesr &=~(1<<16);
    if (eesr & (1 << 24)) { // EDMAC RFCOF
    	syslog(LOG_NOTICE, "EDMAC RFCOF");
    }
    eesr &=~(1<<24);
    if (eesr != 0) {
    	syslog(LOG_NOTICE, "eesr=0x%x",eesr);
    }
#endif
}

ER_UINT
extsvc_isotee_driver_r_ether_rx_initialize(intptr_t myethaddr, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	if (!EXTSVC_PROBE_MEM_READ_SIZE(myethaddr, 6)) {
		return 0;
	}

	ether_return_t eth_ret;
    ether_param_t  param;
    ether_cb_t     cb_func;

    R_ETHER_PinSet_CHANNEL_0();
    R_ETHER_Initial();

    /* Set the callback function (Ether interrupt event) */
    cb_func.pcb_int_hnd  = &EINT_handler;
    param.ether_callback = cb_func;
    eth_ret = R_ETHER_Control(CONTROL_SET_INT_HANDLER, param);
    if (ETHER_SUCCESS != eth_ret)
    {
        return 0; // pdFALSE;
    }

    param.channel = ETHER_CHANNEL_0;
    eth_ret = R_ETHER_Control(CONTROL_POWER_ON, param);        // PHY mode settings, module stop cancellation

    if (ETHER_SUCCESS != eth_ret)
    {
        return 0; // pdFALSE;
    }

    eth_ret = R_ETHER_Open_ZC2(ETHER_CHANNEL_0, myethaddr, ETHER_FLAG_OFF);

    if (ETHER_SUCCESS != eth_ret)
    {
        return 0; // pdFALSE;
    }

    do //TODO allow for timeout
    {
        eth_ret = R_ETHER_CheckLink_ZC(0);
    }
    while(ETHER_SUCCESS != eth_ret);

    return 1;
}

ER_UINT
extsvc_R_ETHER_Read_ZC2(intptr_t channel, intptr_t pbuf, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	if (!EXTSVC_PROBE_MEM_WRITE(pbuf, void*)) {
		return ETHER_ERR_INVALID_PTR;
	}
	return((ER_UINT) R_ETHER_Read_ZC2((uint32_t)channel, (void**)pbuf));
}

ER_UINT
extsvc_R_ETHER_Read_ZC2_BufRelease(intptr_t channel, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	return((ER_UINT) R_ETHER_Read_ZC2_BufRelease((uint32_t)channel));
}

ER_UINT
extsvc_R_ETHER_Write_ZC2_GetBuf(intptr_t channel, intptr_t pbuf, intptr_t pbuf_size, intptr_t par4, intptr_t par5, ID cdmid) {
	if (!EXTSVC_PROBE_MEM_WRITE(pbuf, void*) || !EXTSVC_PROBE_MEM_WRITE(pbuf_size, uint16_t)) {
		return ETHER_ERR_INVALID_PTR;
	}
	return((ER_UINT) R_ETHER_Write_ZC2_GetBuf((uint32_t)channel, (void**)pbuf, (uint16_t*)pbuf_size));
}

ER_UINT
extsvc_R_ETHER_Write_ZC2_SetBuf(intptr_t channel, intptr_t len, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	return((ER_UINT) R_ETHER_Write_ZC2_SetBuf((uint32_t)channel, (uint32_t)len));
}

ER_UINT
extsvc_R_ETHER_LinkProcess(intptr_t channel, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	R_ETHER_LinkProcess((uint32_t)channel);
	return(0);
}

ER_UINT
extsvc_R_ETHER_CheckWrite(intptr_t channel, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid) {
	return((ER_UINT) R_ETHER_CheckWrite((uint32_t)channel));
}

void isotee_driver_r_ether_isr() {
	R_ATTRIB_INTERRUPT void group_al1_handler_isr (void);
	group_al1_handler_isr();
}
