/* USB device controller driver for STM32 devices */

/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 * Copyright (c) 2017 Christer Weinigel.
 * Copyright (c) 2017, I-SENSE group of ICCS
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief USB device controller driver for STM32 devices
 *
 * This driver uses the STM32 Cube low level drivers to talk to the USB
 * device controller on the STM32 family of devices using the
 * STM32Cube HAL layer.
 *
 * There is a bit of an impedance mismatch between the Zephyr
 * usb_device and the STM32 Cube HAL layer where higher levels make
 * assumptions about the low level drivers that don't quite match how
 * the low level drivers actually work.
 *
 * The usb_dc_ep_read function expects to get the data it wants
 * immediately while the HAL_PCD_EP_Receive function only starts a
 * read transaction and the data won't be available until call to
 * HAL_PCD_DataOutStageCallback. To work around this I've
 * had to add an extra packet buffer in the driver which wastes memory
 * and also leads to an extra copy of all received data.  It would be
 * better if higher drivers could call start_read and get_read_count
 * in this driver directly.
 *
 * To enable the driver together with the CDC_ACM high level driver,
 * add the following to your board's defconfig:
 *
 * CONFIG_USB=y
 * CONFIG_USB_DC_STM32=y
 * CONFIG_USB_CDC_ACM=y
 * CONFIG_USB_DEVICE_STACK=y
 *
 * To use the USB device as a console, also add:
 *
 * CONFIG_UART_CONSOLE_ON_DEV_NAME="CDC_ACM"
 * CONFIG_USB_UART_CONSOLE=y
 * CONFIG_UART_LINE_CTRL=y
 */

#include <soc.h>
#include <string.h>
#include <usb/usb_device.h>
#include <clock_control/stm32_clock_control.h>
#include <misc/util.h>
#include <gpio.h>
#include <syscall_handler.h>
#include "../para/isotee_para.h"

/* From 'dts_board_fixups.h' */
#if defined(DT_USB_HS_BASE_ADDRESS) || defined(USB) || defined(CONFIG_SOC_SERIES_STM32F1X) || defined(RCC_HSI48_SUPPORT) || defined(LL_RCC_USB_CLKSOURCE_NONE)
#error assert failed.
#endif

#define LOG_LEVEL CONFIG_USB_DRIVER_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(usb_dc_stm32);

//static int usb_dc_stm32_clock_enable(void)
Z_SYSCALL_HANDLER(isotee_driver_usb_dc_stm32_clock_enable)
{
    _Static_assert(HOST_AWARE_HYPERCALL_DRIVER_USB_DC_STM32_CLOCK_ENABLE == K_SYSCALL_ISOTEE_DRIVER_USB_DC_STM32_CLOCK_ENABLE);

	struct device *clk = device_get_binding(STM32_CLOCK_CONTROL_NAME);
	struct stm32_pclken pclken = {
		.bus = STM32_CLOCK_BUS_AHB2,
		.enr = LL_AHB2_GRP1_PERIPH_OTGFS,
	};

	if (clock_control_on(clk, (clock_control_subsys_t *)&pclken) != 0) {
		LOG_ERR("Unable to enable USB clock");
		return -EIO;
	}

	return 0;
}
