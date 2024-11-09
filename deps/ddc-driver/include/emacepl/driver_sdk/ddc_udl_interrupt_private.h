/*******************************************************************************
 * FILE: ddc_udl_interrupt_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide interrupt support function
 *  prototypes.
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

#ifndef _DDC_UDL_INTERRUPT_PRIVATE_H_
#define _DDC_UDL_INTERRUPT_PRIVATE_H_

#include "driver_sdk/ddc_udl_private.h"

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;


extern BOOLEAN ddcUdlIsr
(
    U8BIT u8DeviceNumber
);

extern U8BIT ddcUdlIsInterruptEnabled
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT irqEnableInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8DeviceNumber
);

extern S16BIT irqDisableInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8DeviceNumber
);

extern S16BIT irqEnableInterrupt429
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
);

extern S16BIT irqDisableInterrupt429
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
);

#endif /* _DDC_UDL_INTERRUPT_PRIVATE_H_ */
