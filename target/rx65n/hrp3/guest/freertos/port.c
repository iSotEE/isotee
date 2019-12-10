/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "task.h"
#include "../../para/isotee_para.h"

/**
 * First execution of a task (after pxPortInitialiseStack)
 *
 * Preconditions:
 * 1) In interrupt-disabled critical section
 * 2) r6: pxCode, r7: pvParameters
 */
#pragma inline_asm prvPortTaskFirstExec
static void prvPortTaskFirstExec( void ) {
	int		#SVC_PARA_INTERRUPT_ENABLE
	mov.l	r7, r1
	jsr		r6
_vPortTaskFirstExec_exit:
	; Should not reach here as TaskCode must NEVER return
	bra		_vPortTaskFirstExec_exit
}

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters ) {
	*pxTopOfStack = (StackType_t) prvPortTaskFirstExec;	/* PC for task startup */
	pxTopOfStack -= 8; 									/* Allocate space for r6-r13 */
	pxTopOfStack[0] = (StackType_t) pxCode;				/* pxCode -> r6 */
	pxTopOfStack[1] = (StackType_t) pvParameters;		/* pvParameters -> r7 */
	return pxTopOfStack;
}

static uint32_t prev_cycles;

/**
 * Body of interrupt handling
 *
 * Preconditions:
 * 1) In interrupt-suppressed critical section
 *
 * Postconditions:
 * 1) In interrupt-disabled critical section
 *
 * @return 0 if no need to yield
 */
uint8_t ucPortInterruptHandler( void ) {

	for (int i = 0; i < 2; i++) {
		while (1) {
			isotee_irq_t irq = isotee_para_interrupt_poll();
			if (irq >= ISOTEE_VIRTUAL_INTERRUPT_NUMBER) break;
			extern void EINT_Trig_isr(void *ectrl);
			EINT_Trig_isr(NULL);
		}
		if (i == 0) isotee_para_interrupt_disable();
	}

	uint32_t new_cycles = isotee_para_timer_read() - prev_cycles;
	while (new_cycles >= ISOTEE_CYCLES_PER_MS) {
		if (xTaskIncrementTick() != pdFALSE && xYieldOnExitCritical == pdFALSE) {
			xYieldOnExitCritical = pdTRUE;
			uart_string_printf("iSotEE xYieldOnExitCritical pdTRUE in IRQ\n");
		}
		new_cycles -= ISOTEE_CYCLES_PER_MS;
		prev_cycles += ISOTEE_CYCLES_PER_MS;
	}
	isotee_para_interrupt_suppress_off();
	isotee_para_interrupt_pending_clear();
	return (xYieldOnExitCritical != pdFALSE);
}

/**
 * Return to a task from interrupt handler
 *
 * Preconditions:
 * 1) Interrupted task was NOT in any critical section
 * 2) In interrupt-disabled critical section NOW
 */
#pragma section P P_ASM
#pragma inline_asm prvPortInterruptReturn
void prvPortInterruptReturn( void ) {
	int		#SVC_PARA_INTERRUPT_ENABLE
#if defined(__RXV2)
	popm	r3-r5						; ACC0
	mvtaclo	r5, A0
	mvtachi	r4, A0
	mvtacgu	r3, A0
	popm	r3-r5						; ACC1
	mvtaclo	r5, A1
	mvtachi	r4, A1
	mvtacgu	r3, A1
#else
	#error "Unsupported ISA"
#endif
	popc	fpsw
	popc	psw
	popm	r14-r15
	popm	r1-r5
}
#pragma section

BaseType_t xPortStartScheduler( void ) {
	isotee_para_initialize();

	prev_cycles = isotee_para_timer_read();

	isotee_para_interrupt_suppress_off();
	isotee_para_interrupt_pending_clear();

	extern void prvStartFirstTask();
	prvStartFirstTask();

	/* Should not get here. */

	return pdFAIL;
}


long write(long fileno, const unsigned char *buf, long count) {
	/* NOT SUPPORTED BY DEFAULT */
	/* unused */ ((void)fileno);
	/* unused */ ((void)buf);
	/* unused */ ((void)count);
    vAssertCalled();
	return -1;
}
