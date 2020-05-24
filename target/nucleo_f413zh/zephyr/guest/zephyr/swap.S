/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Thread context switching for ARM Cortex-M
 *
 * This module implements the routines necessary for thread context switching
 * on ARM Cortex-M CPUs.
 */

#include <kernel_structs.h>
#include <offsets_short.h>
#include <toolchain.h>
#include <arch/cpu.h>
#include <syscall.h>

_ASM_FILE_PROLOGUE

GTEXT(__swap)
GTEXT(isotee_para_guest_interrupt_handler)
GTEXT(__return_from_irq)
GDATA(_k_neg_eagain)
GDATA(isotee_para_context_interrupt_suppressed)

GDATA(_kernel)

/**
 * Perform thread context switching.
 */

SECTION_FUNC(TEXT, __swap)
    /* Interrupts are disabled. r0 is the irq_lock key. */
    
    push {lr}

    /* load _kernel into r1 and current k_thread into r2 */
    ldr r1, =_kernel
    ldr r2, [r1, #_kernel_offset_to_current]

    /* save the interrupt disable state (irq_lock key) */
    str r0, [r2, #_thread_offset_to_basepri]

    /* addr of callee-saved regs in thread in r0 */
    ldr r0, =_thread_offset_to_callee_saved
    add r0, r2

    /* save callee-saved + SP in thread */
    mov ip, sp

#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    stmia r0, {v1-v8, ip}
#else
#error Unknown ARM architecture
#endif

    /* _kernel is still in r1 */

    /* fetch the thread to run from the ready queue cache */
    ldr r2, [r1, #_kernel_offset_to_ready_q_cache]

swap_from_irq:

    str r2, [r1, #_kernel_offset_to_current]

#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    /* load callee-saved + psp from thread */
    add r0, r2, #_thread_offset_to_callee_saved
    ldmia r0, {v1-v8, ip}
#else
#error Unknown ARM architecture
#endif

    mov sp, ip

    /* Restore previous interrupt disable state (irq_lock key) after SP is switched */
    ldr r0, [r2, #_thread_offset_to_basepri]
    movs.n r3, #0
    str r3, [r2, #_thread_offset_to_basepri]

#if defined(CONFIG_ISOTEE_GUEST_USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
    ldr r1, =isotee_para_context_interrupt_suppressed
    ldr r1, [r1]
    strb r0, [r1]
#else
    cmp r0, #0
    IT EQ
    svceq #BARE_METAL_HYPERCALL_INTERRUPT_ENABLE
#endif

    pop {pc}

/**
 * Entry point of interrupt handler
 *
 * Preconditions:
 * 1) Interrupted task was NOT in any critical section
 * 2) In interrupt-suppressed critical section NOW
 */
SECTION_FUNC(TEXT, isotee_para_guest_interrupt_handler)
	/**
	 * push following registers:
	 * saved_pc -> caller-saved (r0-r3, ip/r12, lr/r14) -> xPSR
	 */

    push {r0}   /* allocate space for 'pc' */
    push {r0-r3, ip, lr}
    mrs r0, APSR
    push {r0}
    ldr r0, =isotee_para_context
    ldr r0, [r0]
    ldr r0, [r0]
    str r0, [sp, #28] /* replace 'pc' in stack with 'saved_pc' */

    mov r0, sp
    ldr sp, =_interrupt_stack + CONFIG_ISR_STACK_SIZE
    push {r0}

    bl z_interrupt_handler

    ldr sp, [sp]

    /* check if we should swap */
    ldr r1, =_kernel
    ldr r2, [r1, #_kernel_offset_to_ready_q_cache]
    ldr r3, [r1, #_kernel_offset_to_current]
    cmp r2, r3

    /* no swap is required, return to the current thread */
    beq unlock_and_return_from_irq

    /* prepare to swap to another thread */

    ldr lr, =__return_from_irq
    push {lr}

    /* addr of callee-saved regs in thread in r0 */
    ldr r0, =_thread_offset_to_callee_saved
    add r0, r3

    /* save callee-saved + SP in thread */
    mov ip, sp

#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    stmia r0, {v1-v8, ip}
#else
#error Unknown ARM architecture
#endif

    b swap_from_irq

SECTION_FUNC(TEXT, unlock_and_return_from_irq)
#if defined(CONFIG_ISOTEE_GUEST_USE_INTERRUPT_SUPPRESSED_CRITICAL_SECTION)
    mov r0, #0 /* the irq lock key of _current is always 0 */
    ldr r1, =isotee_para_context_interrupt_suppressed
    ldr r1, [r1]
    strb r0, [r1]
#else
    svc #BARE_METAL_HYPERCALL_INTERRUPT_ENABLE
#endif
__return_from_irq:
    pop {r0}
    msr APSR_nzcvq, r0
    pop {r0-r3, ip, lr}
    pop {pc}
