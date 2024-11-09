/*******************************************************************************
 * FILE: ddc_udl_os_driver_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide various utility functions.
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

#ifndef _DDC_UDL_OS_DRIVER_PRIVATE_H_
#define _DDC_UDL_OS_DRIVER_PRIVATE_H_

extern S16BIT ddcUdlOsInit
(
    void
);

extern S16BIT ddcUdlOsDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT udlOsChannelRegistration
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
);

extern void udlOsChannelUnregistration
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlOsExit
(
    void
);

extern void ddcUdlOsDeviceAllocate
(
    U8BIT u8DeviceCount,
    struct _DDC_UDL_DEVICE_CONTEXT **ppDevice
);

extern void ddcUdlUnregisterDevice
(
    void
);

#endif /* _DDC_UDL_OS_DRIVER_PRIVATE_H_ */

