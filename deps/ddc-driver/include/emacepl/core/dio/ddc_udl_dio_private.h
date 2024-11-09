/*******************************************************************************
 * FILE: ddc_udl_dio_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide function definitions of the
 *  DIO module interface.
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

#ifndef _DDC_UDL_DIO_PRIVATE_H_
#define _DDC_UDL_DIO_PRIVATE_H_

struct _DDC_UDL_DEVICE_CONTEXT;


extern S16BIT GetDioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
);

extern S16BIT GetDioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
);

extern S16BIT GetDioInput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
);

extern U32BIT GetDioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT SetDioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete,
    U32BIT level
);

extern S16BIT SetDioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete,
    U32BIT level
);

extern void SetDioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
);

#endif /* _DDC_UDL_DIO_PRIVATE_H_ */
