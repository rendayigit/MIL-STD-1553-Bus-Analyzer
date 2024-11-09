/*******************************************************************************
 * FILE: ddc_udl_bus_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS independent bus functions.
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

#ifndef _DDC_UDL_BUS_PRIVATE_H_
#define _DDC_UDL_BUS_PRIVATE_H_

#include "os/include/ddc_os_types.h"

#define DDC_UDL_REGISTER_ADDRESS        0
#define DDC_UDL_MEMORY_ADDRESS          1
#define DDC_UDL_PLX_ADDRESS             2
#define DDC_UDL_DMA_ADDRESS             3

#define NumberofSupportedDevices 16
#define aSupportedDeviceSize NumberofSupportedDevices * 4

typedef struct
{
    U16BIT deviceId;
    U16BIT vendorId;

} DEVICE_LIST_TYPE;

extern const DEVICE_LIST_TYPE aSupportedDeviceList[];

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT ddcUdlBusInit
(
    void
);

extern void ddcUdlBusGetLocationInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT *pu16Location,
    U16BIT *pu16SubLocation
);

extern const DEVICE_LIST_TYPE * ddcUdlBusGetSupportedDeviceList
(
    void
);

extern U8BIT ddcUdlBusGetSupportedDeviceListCount
(
    void
);

#endif /* _DDC_UDL_BUS_PRIVATE_H_ */
