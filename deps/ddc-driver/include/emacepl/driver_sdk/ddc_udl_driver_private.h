/*******************************************************************************
 * FILE: ddc_udl_driver_private.h
 *
 * DESCRIPTION:
 *
 *  This file contains driver only definitions.
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

#ifndef _DDC_UDL_DRIVER_PRIVATE_H_
#define _DDC_UDL_DRIVER_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "bus/ddc_udl_bus_private.h"

#define INIT_ALL_DEVICES        -1


struct _DDC_UDL_DEVICE_CONTEXT;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern void ddcUdlInitDeviceCount
(
    void
);

S16BIT ddcUdlInit
(
    void
);

extern void ddcUdlExit
(
    void
);

extern void ddcUdlIncrementDeviceCount
(
    void
);

extern U8BIT ddcUdlGetDeviceCount
(
    void
);

extern const DEVICE_LIST_TYPE * ddcUdlGetSupportedDeviceList
(
    void
);

extern U8BIT ddcUdlGetSupportedDeviceListCount
(
    void
);

extern struct _DDC_UDL_DEVICE_CONTEXT *ddcUdlCreateDevice
(
    void
);

extern struct _DDC_UDL_DEVICE_CONTEXT *ddcUdlGetDeviceContext
(
    U8BIT u8DeviceNumber
);

extern int ddcUdlDeviceOpen
(
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
);

extern int ddcUdlDeviceClose
(
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
);

extern void ddcUdlSetDeviceCloseBusyStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Index,
    BOOLEAN bBusy
);

extern S16BIT ddcUdlDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlLateDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#endif /* _DDC_UDL_DRIVER_PRIVATE_H_ */
