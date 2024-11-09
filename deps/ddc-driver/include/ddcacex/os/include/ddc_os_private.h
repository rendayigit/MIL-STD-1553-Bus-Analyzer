/*******************************************************************************
 * FILE: ddc_os_private.h
 *
 * DESCRIPTION:
 *
 *  This file contains all OS specific include files and defines.
 *  
 ******************************************************************************
 * Non-Disclosure Statement
 * ------------------------
 * This software is the sole property of Data Device Corporation.  All
 * rights, title, ownership, or other interests in the software remain
 * the property of Data Device Corporation.  This software may be used
 * in accordance with applicable licenses.  Any unauthorized use,
 * duplication, transmission, distribution, or disclosure is expressly
 * forbidden.
 *
 * This non-disclosure statement may not be removed or modified without
 * prior written consent of Data Device Corporation.
 ******************************************************************************
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2014 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef _DDC_OS_PRIVATE_H_
#define _DDC_OS_PRIVATE_H_

#include "include/ddc_types.h"

#include <linux/stddef.h>

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
#include <generated/utsrelease.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
#include <linux/utsrelease.h>
#else
#include <linux/config.h>
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

#if defined (CONFIG_SMP)
#define __SMP__
#endif /* CONFIG_SMP */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/dma-mapping.h>  /* for dma_alloc_coherent */
#include <linux/list.h>         /* for DDC_COPY_FROM_USER */
#include <linux/spinlock.h>     /* for DDC_COPY_FROM_USER */
#include <linux/vmalloc.h>      /* for vmalloc */
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/delay.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 0)
#include <linux/interrupt.h>
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)
#include <asm/system.h>
#endif

#include <asm/irq.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 9)
#include <asm/uaccess.h>        /* for DDC_COPY_FROM_USER */
#else
#include <linux/uaccess.h>      /* for DDC_COPY_FROM_USER */
#endif

#if DDC_ARM
#include <linux/slab.h>
#endif

/* ========================================================================== */
/*                                  DEFINES                                   */
/* ========================================================================== */
#define DDC_UDL_OS_BD_OPEN_AT_DRIVER_LOAD       FALSE
#define DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD      FALSE

/* The kernel source allocates "MINORBITS" (20) bits for minor numbers, per major number *
 * NOTE: Of our 1,048,576 minor numbers, we use 256 for 1553, 32 for 429, and 32 for I/O */

/* The 1553 minor number block is 256 numbers from 0 to 255, supporting "MAX_NUM_DEVICES" (32) devices with "MAX_NUM_1553_CHANNELS" (8) 1553 channels each */
#define DDC_MIN_MINOR_1553              0
#define DDC_MAX_MINOR_1553              (MAX_NUM_DEVICES * MAX_NUM_1553_CHANNELS) - 1
#define DDC_NUM_MINOR_1553              (DDC_MAX_MINOR_1553 - DDC_MIN_MINOR_1553) + 1

/* The 429 minor number block is 32 numbers from 256 to 287, supporting "MAX_NUM_DEVICES" (32) devices */
#define DDC_MIN_MINOR_ARINC_429         DDC_MAX_MINOR_1553 + 1
#define DDC_MAX_MINOR_ARINC_429         (DDC_MIN_MINOR_ARINC_429 + MAX_NUM_DEVICES) - 1
#define DDC_NUM_MINOR_ARINC_429         (DDC_MAX_MINOR_ARINC_429 - DDC_MIN_MINOR_ARINC_429) + 1

/* The I/O minor number block is 32 numbers from 288 to 319, supporting "MAX_NUM_DEVICES" (32) devices */
#define DDC_MIN_MINOR_DISCRETE          DDC_MAX_MINOR_ARINC_429 + 1
#define DDC_MAX_MINOR_DISCRETE          (DDC_MIN_MINOR_DISCRETE + MAX_NUM_DEVICES) - 1
#define DDC_NUM_MINOR_DISCRETE          (DDC_MAX_MINOR_DISCRETE - DDC_MIN_MINOR_DISCRETE) + 1

/* The acexpci minor number block (a superset) is 320 numbers from 0 to 319 */
#define DDC_MIN_MINOR                   DDC_MIN_MINOR_1553
#define DDC_MAX_MINOR                   DDC_MAX_MINOR_DISCRETE
#define DDC_NUM_MINOR                   (DDC_MAX_MINOR - DDC_MIN_MINOR) + 1

#endif /* _DDC_OS_PRIVATE_H_ */
