/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel_structs.h>
#include <syscall_handler.h>
#include <app_memory/app_memdomain.h>
#include "../para/isotee_para.h"

K_APPMEM_PARTITION_DEFINE(part_guest);
struct k_mem_domain dom_guest;

K_THREAD_DEFINE(ISOTEE_GUEST, 512/*STACKSIZE*/, ((CONFIG_ISOTEE_GUEST_TASK_ENTRY) | 0x1) /* EPSR T-bit must be set to 1 */, NULL, NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, K_USER, K_FOREVER);

uint8_t _isotee_guest_readonly_mem[CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE]  __attribute__ ((aligned(CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE)));

#define _isotee_guest_context (*((isotee_para_context_t*)_isotee_guest_readonly_mem))

K_MEM_PARTITION_DEFINE(part_guest_ro, _isotee_guest_readonly_mem, sizeof(_isotee_guest_readonly_mem), K_MEM_PARTITION_P_RW_U_RO);

K_MEM_PARTITION_DEFINE(part_guest_rom, CONFIG_ISOTEE_GUEST_ROM_BASE, CONFIG_ISOTEE_GUEST_ROM_SIZE, K_MEM_PARTITION_P_RX_U_RX);
K_MEM_PARTITION_DEFINE(part_guest_ram, CONFIG_ISOTEE_GUEST_RAM_BASE, CONFIG_ISOTEE_GUEST_RAM_SIZE, K_MEM_PARTITION_P_RW_U_RW);
K_MEM_PARTITION_DEFINE(part_guest_usb, 0x50000000U, 0x40000U, K_MEM_PARTITION_P_RW_U_RW);

#if 0 //TODO: support this
isotee_para_context_t _isotee_guest_context __attribute__ ((aligned (CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE), pack(CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE))); /* TODO: read-only from guest */

K_MEM_PARTITION_DEFINE(part_guest_ro, &_isotee_guest_context, sizeof(_isotee_guest_context), K_MEM_PARTITION_P_RW_U_RO);
#endif

K_APP_BMEM(part_guest) uint8_t _isotee_guest_interrupt_suppressed; /* TODO: guest writable */
uint8_t _isotee_guest_interrupt_pending; /* TODO: guest writable / unused? */
K_APP_BMEM(part_guest) uint32_t _isotee_guest_interrupt_status_bitmap; /* TODO: guest writable? PENDING bitmap */

#define GUEST_IN_USER_MODE() ((_k_thread_obj_ISOTEE_GUEST.arch.mode & CONTROL_nPRIV_Msk) != 0)
#define GUEST_IS_RUNNING() (_current == &_k_thread_obj_ISOTEE_GUEST)
#define GUEST_SAVED_PSP() ((void*)_k_thread_obj_ISOTEE_GUEST.callee_saved.psp)

void *_isotee_thread_trap_psp = NULL;
intptr_t _isotee_guest_interrupt_handler = 0x0; /* TODO: init by guest */
intptr_t _isotee_guest_basepri = 0x0; /* TODO: init by guest */

static inline void modify_trap_frame() {
    if (!_isotee_guest_interrupt_suppressed && GUEST_IN_USER_MODE()) {
        NANO_ESF *psp = GUEST_IS_RUNNING() ? \
            _isotee_thread_trap_psp : GUEST_SAVED_PSP();
        if (psp != NULL && (psp->xpsr & 0x600FC00U) == 0) {
        	/* IMPORTANT: skip if EPSR has non-zero value */
			_isotee_guest_context.saved_pc = psp->pc | 0x1 /* EPSR T-bit must be set to 1 */;
			psp->pc = _isotee_guest_interrupt_handler;
            _isotee_guest_interrupt_suppressed = 1;
        }
    }
	_isotee_guest_interrupt_pending = 1;
}

static void _isotee_guest_systick_deliver(struct k_timer *unused) {
    if (_isotee_guest_basepri == 0) {
        __disable_irq();
        modify_trap_frame();
        __enable_irq();
    }
    _isotee_guest_context.timer_cycles = k_cycle_get_32();
}

K_TIMER_DEFINE(_isotee_guest_systick_timer, _isotee_guest_systick_deliver, NULL);

const uint32_t _isotee_virtual_irqn_to_physical[ISOTEE_VIRTUAL_INTERRUPT_NUMBER] = {
    [ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER] = I2C3_ER_IRQn,
    [ISOTEE_VIRTUAL_INTERRUPT_OTG_FS] = OTG_FS_IRQn,
};

static void _isotee_guest_interrupt_deliver(uint32_t virtual_interrupt) {
	irq_disable(_isotee_virtual_irqn_to_physical[virtual_interrupt]);
    __disable_irq();
    modify_trap_frame();
    _isotee_guest_interrupt_status_bitmap |= (1 << (31 - virtual_interrupt));
    __enable_irq();
}

//__syscall void isotee_para_initialize(void *guest_interrupt_handler);
Z_SYSCALL_HANDLER(isotee_para_initialize, guest_interrupt_handler)
{
    _Static_assert(HOST_AWARE_HYPERCALL_PARA_INITIALIZE == K_SYSCALL_ISOTEE_PARA_INITIALIZE);

    _isotee_guest_interrupt_handler = guest_interrupt_handler;
	_isotee_guest_interrupt_pending = 0;
	_isotee_guest_context.interrupt_suppressed = &_isotee_guest_interrupt_suppressed;
	_isotee_guest_context.interrupt_pending = &_isotee_guest_interrupt_pending;
	_isotee_guest_context.interrupt_status_bitmap = &_isotee_guest_interrupt_status_bitmap;
    _isotee_guest_interrupt_status_bitmap = 0;
    _isotee_guest_systick_deliver(NULL);

    IRQ_CONNECT(I2C3_ER_IRQn, CONFIG_ISOTEE_GUEST_IRQ_PRIO, _isotee_guest_interrupt_deliver, ISOTEE_VIRTUAL_INTERRUPT_I2C3_ER, 0);
    IRQ_CONNECT(OTG_FS_IRQn, CONFIG_ISOTEE_GUEST_IRQ_PRIO, _isotee_guest_interrupt_deliver, ISOTEE_VIRTUAL_INTERRUPT_OTG_FS, 0);
    irq_enable(I2C3_ER_IRQn); // TODO: move to guest side

    k_timer_start(&_isotee_guest_systick_timer, \
        K_MSEC(CONFIG_ISOTEE_GUEST_MSEC_PER_TICK), K_MSEC(CONFIG_ISOTEE_GUEST_MSEC_PER_TICK));

	return (intptr_t)&_isotee_guest_context;
}

void _isotee_start_guest() {
    k_mem_domain_init(&dom_guest, 0, NULL);
    k_mem_domain_add_partition(&dom_guest, &part_guest);
    k_mem_domain_add_partition(&dom_guest, &part_guest_ro);
    k_mem_domain_add_partition(&dom_guest, &part_guest_rom);
    k_mem_domain_add_partition(&dom_guest, &part_guest_ram);
    k_mem_domain_add_partition(&dom_guest, &part_guest_usb);
    k_mem_domain_add_thread(&dom_guest, ISOTEE_GUEST);
    k_thread_start(ISOTEE_GUEST);
}
