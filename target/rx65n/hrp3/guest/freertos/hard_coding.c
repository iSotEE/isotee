#include "FreeRTOS.h"
#include "freertos_start.h"
#include "string.h"

#include "../../para/isotee_para.h"
#include "../../para/isotee_syscall.h"

#define TFN_SERIAL_WRI_DAT (10)
typedef uint32_t ID;

static inline ER_UINT
serial_wri_dat(ID portid, const char *buf, uint32_t len) {
	CAL_SVC_5M(ER_UINT, TFN_SERIAL_WRI_DAT, intptr_t, portid, intptr_t, buf, intptr_t, len, intptr_t, 0, intptr_t, 0);
}

void uart_string_printf(char *pString) {
	serial_wri_dat(1, pString, strlen(pString));
}

void uart_config(void) {}

void vPortEndScheduler(void) {
	while(1);
}

extern void EINT_Trig_isr();

static void test_swint_isr() {
	extern void *test_interrupt_handler;
	((void (*)(void)) test_interrupt_handler)();
}

void* const isotee_isr_table[ISOTEE_VIRTUAL_INTERRUPT_NUMBER] = {
	[ISOTEE_VIRTUAL_INTERRUPT_R_ETHER] = EINT_Trig_isr,
	[ISOTEE_VIRTUAL_INTERRUPT_TEST_SWINT] = test_swint_isr
};

long write(long fileno, const unsigned char *buf, long count) {
	/* NOT SUPPORTED BY DEFAULT */
	/* unused */ ((void)fileno);
	/* unused */ ((void)buf);
	/* unused */ ((void)count);
    vAssertCalled();
	return -1;
}

#pragma section P P_START
void isotee_vm_start() {
    /* Initialize C runtime environment */
    _INITSCT();
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
