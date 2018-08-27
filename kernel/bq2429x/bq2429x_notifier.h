/*
 * include/linux/power/bq2429x_notifier.h
 *
 * header file supporting bq2429x notifier call chain information
 *
 * Copyright (C) 2010 Samsung Electronics
 * Seung-Jin Hahn <sjin.hahn@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#ifndef __BQ2429X_NOTIFIER_H__
#define __BQ2429X__NOTIFIER_H__

#include <linux/device.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/sec_sysfs.h>
#include <linux/usb_notify.h>



/* bq2429x notifier call chain command */
typedef enum {
	BQ2429X_NOTIFY_CMD_DETACH	= 0,
	BQ2429X_NOTIFY_CMD_ATTACH,
	BQ2429X_NOTIFY_CMD_LOGICALLY_DETACH,
	BQ2429X_NOTIFY_CMD_LOGICALLY_ATTACH,
} bq2429x_notifier_cmd_t;


typedef enum {
	ATTACHED_DEV_NONE_BQ2429X = 0,
	ATTACHED_DEV_USB_SDP_BQ2429X,
	ATTACHED_DEV_USB_DCP_BQ2429X,
	ATTACHED_DEV_UNKNOWN_BQ2429X,
	ATTACHED_DEV_OTG_BQ2429X,
	ATTACHED_DEV_NUM,
} bq2429x_attached_dev_t;
/* bq2429x notifier call sequence,
 * largest priority number device will be called first. */
typedef enum {
	BQ2429X_NOTIFY_DEV_DOCK = 0,
	BQ2429X_NOTIFY_DEV_MHL,
	BQ2429X_NOTIFY_DEV_USB,
	BQ2429X_NOTIFY_DEV_TSP,
	BQ2429X_NOTIFY_DEV_CHARGER,
	BQ2429X_NOTIFY_DEV_CPUIDLE,
	BQ2429X_NOTIFY_DEV_CPUFREQ,
} bq2429x_notifier_device_t;

struct bq2429x_notifier_struct {
	bq2429x_attached_dev_t attached_dev;
	bq2429x_notifier_cmd_t cmd;
	struct blocking_notifier_head notifier_call_chain;
};

#define BQ2429X_NOTIFIER_BLOCK(name)	\
	struct notifier_block (name)

/* bq2429x notifier init/notify function
 * this function is for JUST bq2429x device driver.
 * DON'T use function anywhrer else!!
 */
extern void bq2429x_notifier_attach_attached_dev(bq2429x_attached_dev_t new_dev);
extern void bq2429x_notifier_detach_attached_dev(bq2429x_attached_dev_t cur_dev);
extern void bq2429x_notifier_logically_attach_attached_dev(bq2429x_attached_dev_t new_dev);
extern void bq2429x_notifier_logically_detach_attached_dev(bq2429x_attached_dev_t cur_dev);

/* bq2429x notifier register/unregister API
 * for used any where want to receive bq2429x attached device attach/detach. */
extern int bq2429x_notifier_register(struct notifier_block *nb,
		notifier_fn_t notifier, bq2429x_notifier_device_t listener);
extern int bq2429x_notifier_unregister(struct notifier_block *nb);

/* Choose a proper noti. interface for a test */
extern void bq2429x_notifier_set_new_noti(bool flag);

#endif /* __bq2429x_NOTIFIER_H__ */
