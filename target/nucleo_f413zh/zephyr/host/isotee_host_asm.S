/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>
#include <arch/cpu.h>
#include <linker/sections.h>
#include "../para/isotee_para.h"

_ASM_FILE_PROLOGUE

GDATA(_isotee_bare_metal_hypercall_table)
GTEXT(_isotee_bare_metal_hypercall_interrupt_enable)
GTEXT(_isotee_bare_metal_hypercall_interrupt_disable)
GTEXT(_isotee_bare_metal_hypercall_interrupt_mask)
GTEXT(_isotee_bare_metal_hypercall_interrupt_unmask)
GTEXT(_isotee_bare_metal_hypercall_interrupt_poll)
GTEXT(_isotee_z_clock_isr)

GDATA(_isotee_guest_basepri)

SECTION_FUNC(TEXT, _isotee_bare_metal_hypercall_table)
_isotee_bare_metal_hypercall_table:
    .word _isotee_bare_metal_hypercall_interrupt_enable
    .word _isotee_bare_metal_hypercall_interrupt_disable
    .word _isotee_bare_metal_hypercall_interrupt_mask
    .word _isotee_bare_metal_hypercall_interrupt_unmask
_end:

.if _end-_isotee_bare_metal_hypercall_table != CONFIG_ISOTEE_BARE_METAL_HYPERCALL_NUM * 4
    .err /* The implementation must match CONFIG_ISOTEE_BARE_METAL_HYPERCALL_NUM */
.endif

_isotee_bare_metal_hypercall_interrupt_enable:
    mov r0, #0
    msr BASEPRI, r0
    ldr r1, =_isotee_guest_basepri
    str r0, [r1]
    bx lr

_isotee_bare_metal_hypercall_interrupt_disable:
    mrs r1, BASEPRI
    str r1, [r0] /* set return value in the stack frame */
    mov r0, #Z_EXC_PRIO(CONFIG_ISOTEE_GUEST_IRQ_PRIO)
    msr BASEPRI, r0
    ldr r1, =_isotee_guest_basepri
    str r0, [r1]
    isb /* Make the effect of disabling interrupts be realized immediately */
    bx lr

_isotee_bare_metal_hypercall_interrupt_mask:
    ldr r0, [r0] /* read r0 from the stack frame */
    cmp r0, #ISOTEE_VIRTUAL_INTERRUPT_NUMBER
    IT HS
    bxhs lr
    ldr r1, =_isotee_virtual_irqn_to_physical
    ldr r0, [r1, r0, LSL #2]
    b z_arch_irq_disable

_isotee_bare_metal_hypercall_interrupt_unmask:
    ldr r0, [r0] /* read r0 from the stack frame */
    cmp r0, #ISOTEE_VIRTUAL_INTERRUPT_NUMBER
    IT HS
    bxhs lr
    ldr r1, =_isotee_virtual_irqn_to_physical
    ldr r0, [r1, r0, LSL #2]
    b z_arch_irq_enable

_isotee_z_clock_isr:
    ldr r0, =_isotee_thread_trap_psp
    tst lr, #0x4
    ITT NE          /* if we came from thread mode: */
    mrsne r1, PSP
    strne r1, [r0]  /* update _isotee_thread_trap_psp */
    b z_clock_isr
