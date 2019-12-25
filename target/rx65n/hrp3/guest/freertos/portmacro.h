/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type definitions
 */
typedef uint32_t	StackType_t;
typedef uint32_t	UBaseType_t;
typedef int32_t		BaseType_t;

/*
 * Tick type
 */
typedef uint32_t TickType_t;
#define portMAX_DELAY			( ( TickType_t ) 0xffffffffUL)
#define portTICK_PERIOD_MS		( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portTICK_TYPE_IS_ATOMIC	( 1 )
#if configUSE_16_BIT_TICKS == 1
#error Ticks are always 32-bit!
#endif


/*
 * Target-dependent definitions
 */
#define portBYTE_ALIGNMENT	(8)		/* Alignment mainly for memory allocation. 4 seems good for RX too */
#define portSTACK_GROWTH	(-1)

/*
 * Task function macros
 */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*
 * portYIELD
 */
extern BaseType_t xYieldOnExitCritical;
extern void vPortYieldInCritical( void );

#define portYIELD() \
	do { \
		if (uxCriticalNesting == 0) { \
			portDISABLE_INTERRUPTS(); \
			vPortYieldInCritical(); \
			portENABLE_INTERRUPTS(); \
		} else { \
			xYieldOnExitCritical = pdTRUE; \
		} \
	} while (0)

#define portYIELD_FROM_ISR( x )	if( x != pdFALSE ) xYieldOnExitCritical = pdTRUE

/*
 * Task critical sections
 * Use nesting implementation in tasks.c
 */

extern UBaseType_t uxCriticalNesting;

#define portENTER_CRITICAL() \
	do { \
		UBaseType_t nesting = uxCriticalNesting; /* Read to register */ \
		if (nesting == 0) { \
			portDISABLE_INTERRUPTS(); \
			portASSERT_IF_IN_ISR(); \
		} \
		uxCriticalNesting = nesting + 1; /* MUST no be reordered */ \
	} while(0)

#define portEXIT_CRITICAL() \
	do { \
		UBaseType_t nesting = uxCriticalNesting; /* Read to register */ \
		if (nesting > 0) { \
			if ((uxCriticalNesting = nesting - 1) == 0) { \
				if (xYieldOnExitCritical == pdTRUE) { \
					xYieldOnExitCritical = pdFALSE; \
					vPortYieldInCritical(); \
				} \
				portENABLE_INTERRUPTS(); \
			} \
		} \
	} while(0)

#include "../../para/isotee_para.h"
#include "../../para/isotee_syscall.h"

extern void* const isotee_isr_table[ISOTEE_VIRTUAL_INTERRUPT_NUMBER];

//#define USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION

#if defined(USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
#define portENABLE_INTERRUPTS() isotee_para_interrupt_suppress_off();isotee_para_interrupt_pending_check()
#define portDISABLE_INTERRUPTS() isotee_para_interrupt_suppress_on()
#else /* Use interrupt-disabled critical section */
#define portENABLE_INTERRUPTS() isotee_para_interrupt_enable()
#define portDISABLE_INTERRUPTS() isotee_para_interrupt_disable()
#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
