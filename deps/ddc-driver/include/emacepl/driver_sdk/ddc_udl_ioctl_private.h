/*******************************************************************************
 * FILE: ddc_udl_ioctl_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide the OS independent IOCTL interface
 *  function prototype.
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

#ifndef _DDC_UDL_IOCTL_PRIVATE_H_
#define _DDC_UDL_IOCTL_PRIVATE_H_

#include "os/include/ddc_os_types.h"

/******************************************************************************
 * Name:    ddcUdlIoctl
 *
 * Description:
 *      Driver IOCTL method.
 *
 * In   pIoctlParams        Pointer to IOCTL Parameters
 * In   u32IoctlCmd         IOCTL Command
 * In   u8DeviceNumber      Device Number
 * In   u8ChannelNumber     Channel Number
 * In   u8IsCompatCall      set if called from compat_ioctl
 * Out  none
 *
 * Returns: DDC_UDL_ERROR__XXXXX error value
 *****************************************************************************/
extern S16BIT ddcUdlIoctl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_DATA_BUFFERS_TYPE *pIoctlDataBuffers,
    U32BIT u32IoctlCmd,
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8IsCompatCall,
    DDC_DEV_HANDLE_TYPE handleType
);

#endif /* _DDC_UDL_IOCTL_PRIVATE_H_ */
