/*******************************************************************************
 * FILE: ddc_os_types_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific definitions for common
 *  types that is to be used only within the UDL.
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

#ifndef _DDC_OS_TYPES_PRIVATE_H_
#define _DDC_OS_TYPES_PRIVATE_H_

#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/cdev.h>
#include "include/ddc_types.h"
#include "os/include/ddc_os_types.h"
#include "ddc_udl_os_bus_private.h"

struct DDC_OS_DEV_INFO
{
    struct DDC_OS_BUS_INFO sBusInfo;

    U8BIT u8DeviceNumber;               /* device number */

    wait_queue_head_t wait;             /* wait queue for ddcUdlOsWaitMs() */

    dev_t devMajorMinor1553[MAX_NUM_1553_CHANNELS];     /* Device major/minor numbers for Linux character devices dedicated to 1553 channel access */
    struct cdev charDevice1553[MAX_NUM_1553_CHANNELS];  /* Array of character device structures for 1553 channels */
    struct device *pDevice1553[MAX_NUM_1553_CHANNELS];  /* Array of pointers to device structures for 1553 channels */

    dev_t devMajorMinor429;     /* Device major/minor number for Linux character device dedicated to 429 access */
    struct cdev charDevice429;  /* Character device structure for 429 */
    struct device *pDevice429;  /* Pointer to device structure for 429 */

    dev_t devMajorMinorIO;      /* Device major/minor number for Linux character device dedicated to I/O access */
    struct cdev charDeviceIO;   /* Character device structure for I/O */
    struct device *pDeviceIO;   /* Pointer to device structure for I/O */
};

/* For registering Linux character devices */
extern dev_t acexpci_major;
extern struct class *acexpci_class;
extern const struct file_operations ddcUdlHookFileOperations;

/* spin lock */
#define DDC_ISR_LOCK_TYPE                   spinlock_t
#define DDC_ISR_FLAG_TYPE                   unsigned long
#define DDC_LOCK_DELETE(x)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#define DDC_ISR_LOCK_INIT(lock)             spin_lock_init(&lock)
#else
#define DDC_ISR_LOCK_INIT(lock)             lock = SPIN_LOCK_UNLOCKED
#endif /* if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */

#define DDC_ISR_LOCK_TAKE(lock, flag)       spin_lock_irqsave(&lock, flag)
#define DDC_ISR_LOCK_GIVE(lock, flag)       spin_unlock_irqrestore(&lock, flag)


#define DDC_KERNEL_MALLOC(pDeviceContext, size)             kmalloc(size, GFP_KERNEL)
#define DDC_KERNEL_FREE(pDeviceContext, pObj)               kfree(pObj)

#define DDC_KERNEL_VIRTUAL_MALLOC(pDeviceContext, size)     vmalloc(size)
#define DDC_KERNEL_VIRTUAL_FREE(pDeviceContext, pObj)       vfree(pObj)

#define DDC_KERNEL_GET_FREE_PAGE(pDeviceContext)            __get_free_page(GFP_KERNEL | GFP_DMA32)
#define DDC_KERNEL_FREE_PAGE(addr)                          free_page((long)addr)

#define DDC_KERNEL_GET_DMA_DESCRIPTOR(pDeviceContext)       DDC_KERNEL_GET_FREE_PAGE()
#define DDC_KERNEL_FREE_DMA_DESCRIPTOR(addr)                DDC_KERNEL_FREE_PAGE(addr)

#define DDC_IO_MAP(addr, len)               ioremap(addr, len)
#define DDC_IO_UNMAP(addr)                  iounmap(addr)

#define DDC_COPY_FROM_USER(dest, source, size_in_bytes) \
    copy_from_user(dest, source, size_in_bytes)

#define DDC_COPY_TO_USER(dest, source, size_in_bytes) \
    copy_to_user(dest, source, size_in_bytes) 

#endif /* _DDC_OS_TYPES_PRIVATE_H_ */
