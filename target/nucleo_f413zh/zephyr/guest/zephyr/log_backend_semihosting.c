/*
 * Copyright (c) 2020 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log_backend.h>
#include <logging/log_core.h>
#include <logging/log_msg.h>
#include <logging/log_output.h>
#include <device.h>
#include <assert.h>

static __attribute__((optimize(0)))
int32_t _semihosting_call(uint32_t service, void *op1) {
	register int32_t ret __asm__("r0");
    __asm__ volatile("bkpt #0xab": "=r"(ret));
    return ret;
}

static int char_out(u8_t *data, size_t length, void *ctx) {
    ARG_UNUSED(ctx);
    uint32_t params[3] = {2 /*stderr*/, (uint32_t)data, length};
    _semihosting_call(0x5 /*SYS_WRITE*/, params);
    return length;
}

static u8_t buf[64];

LOG_OUTPUT_DEFINE(log_output, char_out, buf, 64);

static void put(const struct log_backend *const backend,
		struct log_msg *msg)
{
	log_msg_get(msg);

	u32_t flags = LOG_OUTPUT_FLAG_LEVEL | LOG_OUTPUT_FLAG_TIMESTAMP;

	if (IS_ENABLED(CONFIG_LOG_BACKEND_SHOW_COLOR)) {
		flags |= LOG_OUTPUT_FLAG_COLORS;
	}

	if (IS_ENABLED(CONFIG_LOG_BACKEND_FORMAT_TIMESTAMP)) {
		flags |= LOG_OUTPUT_FLAG_FORMAT_TIMESTAMP;
	}

	log_output_msg_process(&log_output, msg, flags);

	log_msg_put(msg);

}

static void log_backend_semihosting_init(void)
{
	struct device *dev = NULL;

	log_output_ctx_set(&log_output, dev);
}

static void panic(struct log_backend const *const backend)
{
	log_output_flush(&log_output);
}

static void dropped(const struct log_backend *const backend, u32_t cnt)
{
	ARG_UNUSED(backend);

	log_output_dropped_process(&log_output, cnt);
}

static void sync_string(const struct log_backend *const backend,
		     struct log_msg_ids src_level, u32_t timestamp,
		     const char *fmt, va_list ap)
{
	u32_t flags = LOG_OUTPUT_FLAG_LEVEL | LOG_OUTPUT_FLAG_TIMESTAMP;
	u32_t key;

	if (IS_ENABLED(CONFIG_LOG_BACKEND_SHOW_COLOR)) {
		flags |= LOG_OUTPUT_FLAG_COLORS;
	}

	if (IS_ENABLED(CONFIG_LOG_BACKEND_FORMAT_TIMESTAMP)) {
		flags |= LOG_OUTPUT_FLAG_FORMAT_TIMESTAMP;
	}

	key = irq_lock();
	log_output_string(&log_output, src_level, timestamp, fmt, ap, flags);
	irq_unlock(key);
}

static void sync_hexdump(const struct log_backend *const backend,
			 struct log_msg_ids src_level, u32_t timestamp,
			 const char *metadata, const u8_t *data, u32_t length)
{
	u32_t flags = LOG_OUTPUT_FLAG_LEVEL | LOG_OUTPUT_FLAG_TIMESTAMP;
	u32_t key;

	if (IS_ENABLED(CONFIG_LOG_BACKEND_SHOW_COLOR)) {
		flags |= LOG_OUTPUT_FLAG_COLORS;
	}

	if (IS_ENABLED(CONFIG_LOG_BACKEND_FORMAT_TIMESTAMP)) {
		flags |= LOG_OUTPUT_FLAG_FORMAT_TIMESTAMP;
	}

	key = irq_lock();
	log_output_hexdump(&log_output, src_level, timestamp,
			metadata, data, length, flags);
	irq_unlock(key);
}

const struct log_backend_api log_backend_semihosting_api = {
	.put = IS_ENABLED(CONFIG_LOG_IMMEDIATE) ? NULL : put,
	.put_sync_string = IS_ENABLED(CONFIG_LOG_IMMEDIATE) ?
			sync_string : NULL,
	.put_sync_hexdump = IS_ENABLED(CONFIG_LOG_IMMEDIATE) ?
			sync_hexdump : NULL,
	.panic = panic,
	.init = log_backend_semihosting_init,
	.dropped = IS_ENABLED(CONFIG_LOG_IMMEDIATE) ? NULL : dropped,
};

LOG_BACKEND_DEFINE(log_backend_semihosting, log_backend_semihosting_api, true);
