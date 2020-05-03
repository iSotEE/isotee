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
GTEXT(__return_from_irq)
GDATA(_k_neg_eagain)

GDATA(_kernel)

/**
 *
 * @brief PendSV exception handler, handling context switches
 *
 * The PendSV exception is the only execution context in the system that can
 * perform context switching. When an execution context finds out it has to
 * switch contexts, it pends the PendSV exception.
 *
 * When PendSV is pended, the decision that a context switch must happen has
 * already been taken. In other words, when __pendsv() runs, we *know* we have
 * to swap *something*.
 */

SECTION_FUNC(TEXT, __swap)
    /* Interrupts are disabled. r0 is the irq_lock() key. */
    
    push {lr}

    /* load _kernel into r1 and current k_thread into r2 */
    ldr r1, =_kernel
    ldr r2, [r1, #_kernel_offset_to_current]

    /* addr of callee-saved regs in thread in r0 */
    ldr r0, =_thread_offset_to_callee_saved
    add r0, r2

    /* save callee-saved + SP in thread */
    mov ip, sp

#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    stmia r0, {v1-v8, ip}
#ifdef CONFIG_FP_SHARING
    add r0, r2, #_thread_offset_to_preempt_float
    vstmia r0, {s16-s31}
#endif /* CONFIG_FP_SHARING */
#else
#error Unknown ARM architecture
#endif

#if 0 // TODO: support for isotee
    /*
     * Prepare to clear PendSV with interrupts unlocked, but
     * don't clear it yet. PendSV must not be cleared until
     * the new thread is context-switched in since all decisions
     * to pend PendSV have been taken with the current kernel
     * state and this is what we're handling currently.
     */
    ldr v4, =_SCS_ICSR
    ldr v3, =_SCS_ICSR_UNPENDSV
#endif

    /* _kernel is still in r1 */

    /* fetch the thread to run from the ready queue cache */
    ldr r2, [r1, #_kernel_offset_to_ready_q_cache]

    str r2, [r1, #_kernel_offset_to_current]

#if 0 // TODO: support for isotee
    /*
     * Clear PendSV so that if another interrupt comes in and
     * decides, with the new kernel state based on the new thread
     * being context-switched in, that it needs to reschedule, it
     * will take, but that previously pended PendSVs do not take,
     * since they were based on the previous kernel state and this
     * has been handled.
     */

    /* _SCS_ICSR is still in v4 and _SCS_ICSR_UNPENDSV in v3 */
    str v3, [v4, #0]
#endif

    /* Restore previous interrupt disable state (irq_lock key) */
    ldr r0, [r2, #_thread_offset_to_basepri]
    movs.n r3, #0
    str r3, [r2, #_thread_offset_to_basepri]

#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)

#if 0 // TODO: support this
    /* BASEPRI not available, previous interrupt disable state
     * maps to PRIMASK.
     *
     * Only enable interrupts if value is 0, meaning interrupts
     * were enabled before irq_lock was called.
     */
    cmp r0, #0
    bne _thread_irq_disabled
    cpsie i
_thread_irq_disabled:

TODO: OR-FOR-CORTEX-V7
    /* restore BASEPRI for the incoming thread */
    msr BASEPRI, r0
#endif

#ifdef CONFIG_FP_SHARING
    add r0, r2, #_thread_offset_to_preempt_float
    vldmia r0, {s16-s31}
#endif

    /* load callee-saved + psp from thread */
    add r0, r2, #_thread_offset_to_callee_saved
    ldmia r0, {v1-v8, ip}
#else
#error Unknown ARM architecture
#endif

    mov sp, ip

    pop {pc}

SECTION_FUNC(TEXT, __return_from_irq)
    pop {r0}
    msr APSR_nzcvq, r0
    pop {r0-r3, ip, lr}
    pop {pc}
