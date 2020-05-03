/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>
#include <linker/sections.h>
#include "vector_table.h"

_ASM_FILE_PROLOGUE

GDATA(_interrupt_stack)

SECTION_SUBSEC_FUNC(exc_vector_table,_vector_table_section,_vector_table)
    ldr sp, =_interrupt_stack + CONFIG_ISR_STACK_SIZE
    bl _PrepC
