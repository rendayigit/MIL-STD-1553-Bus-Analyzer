/*******************************************************************************
 * FILE: ddc_udl_flash_private.h
 *
 * DESCRIPTION:
 *
 *  This file contains flash only definitions.
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

#ifndef _DDC_UDL_FLASH_PRIVATE_H_
#define _DDC_UDL_FLASH_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "driver_sdk/ddc_udl_private.h"
#include "include/ddc_ioctl.h"


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;


extern U8BIT flashMemBlkErase
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pCmdInfo
);

extern U8BIT flashMemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pCmdInfo,
    void *IOBuffer
);

extern U8BIT flashMemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pCmdInfo,
    void *IOBuffer
);

extern U8BIT flashMemWriteProtected
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);


#endif /* _DDC_UDL_FLASH_PRIVATE_H_ */
