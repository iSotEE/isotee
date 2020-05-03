/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <toolchain.h>
#include <kernel_structs.h>
#include <wait_q.h>

/**
 *
 * Initialize a new thread from its stack space
 *
 * The control structure (thread) is put at the lower address of the stack. An
 * initial context, to be "restored" by __return_from_irq(), is put at the other end of
 * the stack, and thus reusable by the stack when not needed anymore.
 *
 */

struct _isotee_esf {
	u32_t return_address;
	u32_t xpsr;
	sys_define_gpr_with_alias(a1, r0);
	sys_define_gpr_with_alias(a2, r1);
	sys_define_gpr_with_alias(a3, r2);
	sys_define_gpr_with_alias(a4, r3);
	sys_define_gpr_with_alias(ip, r12);
	sys_define_gpr_with_alias(lr, r14);
	sys_define_gpr_with_alias(pc, r15); /* PC must be the last */
};

void z_new_thread(struct k_thread *thread, k_thread_stack_t *stack,
		 size_t stackSize, k_thread_entry_t pEntry,
		 void *parameter1, void *parameter2, void *parameter3,
		 int priority, unsigned int options)
{
	char *pStackMem = Z_THREAD_STACK_BUFFER(stack);
	char *stackEnd;
	/* Offset between the top of stack and the high end of stack area. */
	u32_t top_of_stack_offset = 0U;

	Z_ASSERT_VALID_PRIO(priority, pEntry);

	stackEnd = pStackMem + stackSize;

	struct _isotee_esf *pInitCtx;

	z_new_thread_init(thread, pStackMem, stackSize, priority,
			 options);

	/* carve the thread entry struct from the "base" of the stack */
	pInitCtx = (struct _isotee_esf *)(STACK_ROUND_DOWN(stackEnd -
		(char *)top_of_stack_offset - sizeof(struct _isotee_esf)));

    extern void __return_from_irq();
	pInitCtx->return_address = (u32_t)__return_from_irq;

	pInitCtx->pc = (u32_t)z_thread_entry;

	/* force ARM mode by clearing LSB of address (See native 'thread.c') */
//	pInitCtx->pc &= 0xfffffffe;

	pInitCtx->a1 = (u32_t)pEntry;
	pInitCtx->a2 = (u32_t)parameter1;
	pInitCtx->a3 = (u32_t)parameter2;
	pInitCtx->a4 = (u32_t)parameter3;
	pInitCtx->xpsr =
		0x01000000UL; /* clear all, thumb bit is 1, even if RO */

	thread->callee_saved.psp = (u32_t)pInitCtx;
	thread->arch.basepri = 0;

	/* swap_return_value can contain garbage */

	/*
	 * initial values in all other registers/thread entries are
	 * irrelevant.
	 */
}
