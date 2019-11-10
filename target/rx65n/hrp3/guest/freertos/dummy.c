#include "FreeRTOS.h"

#define R_ATTRIB_INTERRUPT


// TODO: should be called for undefined vectors, BUT we don't support it for now. TBD.
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void undefined_interrupt_source_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __undefined_interrupt_source_handler(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* If user has registered a callback for this exception then call it. */
    //R_BSP_InterruptControl(BSP_INT_SRC_UNDEFINED_INTERRUPT, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
	while(1);
}

void uart_config() {
}


#include "../../para/isotee_para.h"
#include "../../para/isotee_syscall.h"



#define Inline static inline
#define	SVC_SERVICE_CALL	1
#define TFN_SERIAL_WRI_DAT		(10)
typedef int32_t ER_UINT;
typedef int32_t FN;
typedef uint32_t ID;
typedef uint32_t uint_t;
#define E_RSFN		(-10)		/* 予約機能コード */

#define CAL_SVC_5M(TYPE, FNCD, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3, TYPE4, PAR4, TYPE5, PAR5) \
		return ((TYPE)_svc_service_call_5((intptr_t)(PAR1), (intptr_t)(PAR2), (intptr_t)(PAR3), (intptr_t)(PAR4), (intptr_t)(PAR5), FNCD))

/*
 *  cal_svcサービスコールの呼出し
 */
Inline ER_UINT
cal_svc(FN fncd, intptr_t par1, intptr_t par2,
							intptr_t par3, intptr_t par4, intptr_t par5)
{
	if (fncd > 0) {
		CAL_SVC_5M(ER_UINT, fncd, intptr_t, par1, intptr_t, par2,
							intptr_t, par3, intptr_t, par4, intptr_t, par5);
	}
	else {
		return(E_RSFN);
	}
}

static inline ER_UINT
serial_wri_dat(ID portid, const char *buf, uint_t len)
{
	return(cal_svc(TFN_SERIAL_WRI_DAT, (intptr_t) portid, (intptr_t) buf,
													(intptr_t) len, 0, 0));
}


void uart_string_printf(char *pString) {
	serial_wri_dat(1, pString, strlen(pString));
}

void vPortEndScheduler() {
	while(1);
}

UBaseType_t uxCriticalNesting = 0xdeadbeef; /* 0xdeadbeef -> uninitialized */
BaseType_t	xYieldOnExitCritical = pdFALSE;

#pragma section P P_START
void isotee_vm_start() {
	// TODO: initialize data/bss sections
    /* Initialize C runtime environment */
    _INITSCT();

	uxCriticalNesting = 0; // TODO: in port start?
	xYieldOnExitCritical = pdFALSE;

    Processing_Before_Start_Kernel();
	vTaskStartScheduler();
	while(1);
}
#pragma section


/* Section start */
#pragma section C C$DSEC

/* MCU ROM and RAM structure definition */
extern const struct {
    uint8_t *rom_s;       /* Start address of the initialized data section in ROM */
    uint8_t *rom_e;       /* End address of the initialized data section in ROM   */
    uint8_t *ram_s;       /* Start address of the initialized data section in RAM */
}   _DTBL[] = {
    { __sectop("D"), __secend("D"), __sectop("R") },
};

/* Section start */
#pragma section C C$BSEC

/* MCU ROM and RAM structure definition */
extern const struct {
    uint8_t *b_s;         /* Start address of non-initialized data section */
    uint8_t *b_e;         /* End address of non-initialized data section */
}   _BTBL[] = {
    { __sectop("B"), __secend("B") },
};

/* Section start */
#pragma section
