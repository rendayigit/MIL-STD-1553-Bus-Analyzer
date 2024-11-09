/*******************************************************************************
 * FILE: ddc_udl_os_bus_interrupt_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide the function prototypes for 
 *  OS specific, generic bus, IRQ hook routines. 
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

#ifndef _DDC_UDL_OS_BUS_INTERRUPT_PRIVATE_H_
#define _DDC_UDL_OS_BUS_INTERRUPT_PRIVATE_H_

#include "driver_sdk/ddc_udl_private.h"

/*---------------------------------------------------------------------------------
 * In some Linux Kernel, MSI can be enabled but no interrupt will be generated.
 * When this happens, DDC cards will not work at all. To avoid this problem,
 * MSI interrupts are not enabled by default. User has to uncomment the
 * DDC_MSI_SUPPORT definition below to enable MSI support.
 *---------------------------------------------------------------------------------*/
 /* #define DDC_MSI_SUPPORT */

 
/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;


extern S16BIT ddcUdlOsBusIrqHookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
);

extern S16BIT ddcUdlOsBusIrqUnhookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
);


#endif /* _DDC_UDL_OS_BUS_INTERRUPT_PRIVATE_H_ */
