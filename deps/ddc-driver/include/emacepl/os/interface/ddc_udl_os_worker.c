/*******************************************************************************
 * FILE: ddc_udl_os_worker.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide thread management routines.
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

#include "os/include/ddc_os_types.h"
#include "driver_sdk/ddc_udl_private.h"

typedef struct _DDC_IRQ_WORK_STRUCT
{
    struct work_struct worker;

    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext;
    
} DDC_IRQ_WORK_STRUCT;


/* work queue declarations*/
DDC_IRQ_WORK_STRUCT irq_workqueue[ACEXUSB_MAX_CARDS]; /* one workqueue per card */
 
/* ========================================================================== */
/* ========================================================================== */

void ddcScheduleWork
(
    void (*function)(void *),
    void *data
)
{

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
    INIT_WORK(&(irq_workqueue[u8CardNum].worker), function, &(irq_workqueue[u8CardNum]));
#else
    INIT_WORK(&(irq_workqueue[u8CardNum].worker), function);
#endif

    schedule_work(&(irq_workqueue[u8CardNum].worker));
    
}