/*
 * Userspace and service handler hooks
 *
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <offsets_short.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <kernel_structs.h>
#include <arch/cpu.h>
#include <syscall.h>

_ASM_FILE_PROLOGUE

GTEXT(z_arm_userspace_enter)
GTEXT(z_arm_do_syscall)
GTEXT(z_arch_user_string_nlen)
GTEXT(z_arch_user_string_nlen_fault_start)
GTEXT(z_arch_user_string_nlen_fault_end)
GTEXT(z_arch_user_string_nlen_fixup)
GDATA(_kernel)

/* Imports */
GDATA(_k_syscall_table)

/**
 *
 * User space entry function
 *
 * This function is the entry point to user mode from privileged execution.
 * The conversion is one way, and threads which transition to user mode do
 * not transition back later, unless they are doing system calls.
 *
 */
SECTION_FUNC(TEXT,z_arm_userspace_enter)
    /* move user_entry to lr */
    mov lr, r0

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* clear stack pointer limit before setting the PSP */
    mov r0, #0
    msr PSPLIM, r0
#endif

    /* set stack to privileged stack */
    ldr r0, =_kernel
    ldr r0, [r0, #_kernel_offset_to_current]
    ldr r0, [r0, #_thread_offset_to_priv_stack_start]    /* priv stack ptr */
    ldr ip, =CONFIG_PRIVILEGED_STACK_SIZE
    add r0, r0, ip

    mov ip, sp
    msr PSP, r0

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* set stack pointer limit to the start of the priv stack */
    ldr r0, =_kernel
    ldr r0, [r0, #_kernel_offset_to_current]
    ldr r0, [r0, #_thread_offset_to_priv_stack_start]    /* priv stack ptr */
    msr PSPLIM, r0
#endif

#if defined (CONFIG_ARM_MPU)
    /* Re-program dynamic memory map.
     *
     * Important note:
     * z_arch_configure_dynamic_mpu_regions() may re-program the MPU Stack Guard
     * to guard the privilege stack for overflows (if building with option
     * CONFIG_MPU_STACK_GUARD). There is a risk of actually overflowing the
     * stack while doing the re-programming. We minimize the risk by placing
     * this function immediately after we have switched to the privileged stack
     * so that the whole stack area is available for this critical operation.
     *
     * Note that the risk for overflow is higher if using the normal thread
     * stack, since we do not control how much stack is actually left, when
     * user invokes z_arm_userspace_enter().
     */
    push {r0,r1,r2,r3,ip,lr}
    ldr r0, =_kernel
    ldr r0, [r0, #_kernel_offset_to_current]
    bl z_arch_configure_dynamic_mpu_regions
    pop {r0,r1,r2,r3,ip,lr}
#endif

    /* load up stack info from user stack */
    ldr r0, [ip]
    ldr ip, [ip, #4]

#ifdef CONFIG_MPU_REQUIRES_POWER_OF_TWO_ALIGNMENT
    /* Guard is taken out of size, so adjust beginning and size of stack */
    subs ip, #MPU_GUARD_ALIGN_AND_SIZE
#endif

    /* push args to stack */
    push {r0,r1,r2,r3,ip,lr}

    /* clear the user stack area to clean out privileged data */
    /* from right past the guard right up to the end */
    mov r2, ip
#ifdef CONFIG_INIT_STACKS
    ldr r1,=0xaaaaaaaa
#else
    eors.n r1, r1
#endif
    bl memset

    pop {r0,r1,r2,r3,ip,lr}

    /* r0 contains user stack start, ip contains user stack size */
    add r0, r0, ip   /* calculate top of stack */

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* clear stack limit (stack protection not required in user mode) */
    push {r3}
    mov r3, #0
    msr PSPLIM, r3
    pop {r3}
#endif

    /* set stack to user stack */
    msr PSP, r0

    /* restore r0 */
    mov r0, lr

#ifdef CONFIG_EXECUTION_BENCHMARKING
    stm sp!,{r0-r3} /* Save regs r0 to r4 on stack */
    push {r0, lr}
    bl read_timer_end_of_userspace_enter
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
    pop {r0, r3}
    mov lr,r3
#else
    pop {r0, lr}
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */
    ldm sp!,{r0-r3} /* Restore r0 to r3 regs */

#endif /* CONFIG_EXECUTION_BENCHMARKING */

    /* change processor mode to unprivileged */
    push {r0, r1}
    ldr r0, =_kernel
    ldr r0, [r0, #_kernel_offset_to_current]
    ldr r1, [r0, #_thread_offset_to_mode]
    orrs r1, r1, #1
    mrs ip, CONTROL
    orrs ip, ip, #1
    /* Store (unprivileged) mode in thread's mode state variable */
    str r1, [r0, #_thread_offset_to_mode]
    dsb
    msr CONTROL, ip

    /* ISB is not strictly necessary here (stack pointer is not being
     * touched), but it's recommended to avoid executing pre-fetched
     * instructions with the previous privilege.
     */
    isb
    pop {r0, r1}

    /* jump to z_thread_entry entry */
    ldr ip, =z_thread_entry
    bx ip

/**
 *
 * Userspace system call function
 *
 * This function is used to do system calls from unprivileged code.  This
 * function is responsible for the following:
 * 1) Fixing up bad syscalls
 * 2) Configuring privileged stack and loading up stack arguments
 * 3) Dispatching the system call
 * 4) Restoring stack and calling back to the caller of the SVC
 *
 */
SECTION_FUNC(TEXT, z_arm_do_syscall)

    /* The function is executing in privileged mode. This implies that we
     * shall not be allowed to use the thread's default unprivileged stack,
     * (i.e push to or pop from it), to avoid a possible stack corruption.
     *
     * Rationale: since we execute in PRIV mode and no MPU guard or PSPLIM
     * register is guarding the end of the default stack, we won't be able
     * to detect any stack overflows.
     */

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* clear stack pointer limit before setting the PSP */
    mov ip, #0
    msr PSPLIM, ip
#endif

    /* setup privileged stack */
    ldr ip, =_kernel
    ldr ip, [ip, #_kernel_offset_to_current]
    ldr ip, [ip, #_thread_offset_to_priv_stack_start]    /* priv stack ptr */
    add ip, #CONFIG_PRIVILEGED_STACK_SIZE

    /* Store current SP and LR at the beginning of the priv stack */
    subs ip, #8
    str sp, [ip, #0]
    str lr, [ip, #4]

    /* switch to privileged stack */
    msr PSP, ip

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* Set stack pointer limit (needed in privileged mode) */
    ldr ip, =_kernel
    ldr ip, [ip, #_kernel_offset_to_current]
    ldr ip, [ip, #_thread_offset_to_priv_stack_start]    /* priv stack ptr */
    msr PSPLIM, ip
#endif

    /*
     * r0-r5 contain arguments
     * r6 contains call_id
     * r8 contains original LR
     */
    ldr ip, =K_SYSCALL_BAD
    cmp r6, ip
    bne valid_syscall

    /* BAD SYSCALL path */
    /* fixup stack frame on the privileged stack, adding ssf */
    mov ip, sp
    push {r4,r5,ip,lr}
    b dispatch_syscall

valid_syscall:
    /* push args to complete stack frame */
    push {r4,r5}

dispatch_syscall:
    ldr ip, =_k_syscall_table
    lsl r6, #2
    add ip, r6
    ldr ip, [ip]	/* load table address */
    /* execute function from dispatch table */
    blx ip

    /* restore LR */
    ldr lr, [sp,#12]

#if defined(CONFIG_BUILTIN_STACK_GUARD)
    /* clear stack limit (stack protection not required in user mode) */
    mov r3, #0
    msr PSPLIM, r3
#endif

    /* set stack back to unprivileged stack */
    ldr ip, [sp,#8]
    msr PSP, ip

    push {r0, r1}
    ldr r0, =_kernel
    ldr r0, [r0, #_kernel_offset_to_current]
    ldr r1, [r0, #_thread_offset_to_mode]
    orrs r1, r1, #1
    /* Store (unprivileged) mode in thread's mode state variable */
    str r1, [r0, #_thread_offset_to_mode]
    dsb
    /* drop privileges by setting bit 0 in CONTROL */
    mrs ip, CONTROL
    orrs ip, ip, #1
    msr CONTROL, ip

    /* ISB is not strictly necessary here (stack pointer is not being
     * touched), but it's recommended to avoid executing pre-fetched
     * instructions with the previous privilege.
     */
    isb
    pop {r0, r1}

    /* Zero out volatile (caller-saved) registers so as to not leak state from
     * kernel mode. The C calling convention for the syscall handler will
     * restore the others to original values.
     */
    mov r1, #0
    mov r2, #0
    mov r3, #0

    /*
     * return back to original function that called SVC, add 1 to force thumb
     * mode
     */
    mov ip, r8
    orrs ip, ip, #1
    bx ip


/*
 * size_t z_arch_user_string_nlen(const char *s, size_t maxsize, int *err_arg)
 */
SECTION_FUNC(TEXT, z_arch_user_string_nlen)
    push {r0, r1, r2, r4, r5, lr}

    /* sp+4 is error value, init to -1 */
    mov.w r3, #-1
    str	r3, [sp, #4]

    /* Perform string length calculation */
    movs r3, #0		/* r3 is the counter */

strlen_loop:
z_arch_user_string_nlen_fault_start:
    /* r0 contains the string. r5 = *(r0 + r3]). This could fault. */
    ldrb r5, [r0, r3]

z_arch_user_string_nlen_fault_end:
    cbz	r5, strlen_done
    cmp	r3, r1
    beq.n strlen_done

    adds r3, #1
    b.n	strlen_loop

strlen_done:
    /* Move length calculation from r3 to r0 (return value register) */
    mov	r0, r3

    /* Clear error value since we succeeded */
    movs r1, #0
    str	r1, [sp, #4]

z_arch_user_string_nlen_fixup:
    /* Write error value to err pointer parameter */
    ldr	r1, [sp, #4]
    str	r1, [r2, #0]

    add	sp, #12
    pop	{r4, r5, pc}

