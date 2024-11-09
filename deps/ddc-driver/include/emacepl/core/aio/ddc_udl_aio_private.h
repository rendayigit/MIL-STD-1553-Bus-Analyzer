/*******************************************************************************
 * FILE: ddc_udl_aio_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide function definitions of the 
 *  AIO module interface.
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

#ifndef _DDC_UDL_AIO_PRIVATE_H_
#define _DDC_UDL_AIO_PRIVATE_H_

struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT GetAioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT GetAioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT GetAioInput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern U32BIT GetAioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT SetAioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT SetAioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void SetAioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT Avionics
);


#endif /* _DDC_UDL_AIO_PRIVATE_H_ */
