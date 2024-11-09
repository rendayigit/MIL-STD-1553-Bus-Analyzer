/*******************************************************************************
 * FILE: ddc_udl_bus_pci_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS independent PCI access
 *  routines.
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

#ifndef _DDC_UDL_BUS_PCI_PRIVATE_H_
#define _DDC_UDL_BUS_PCI_PRIVATE_H_

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT ddcUdlBusPciInitDevice
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    BOOLEAN bEnable
);

extern void ddcUdlBusPciPlx9000_EepromReadByOffset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Offset,
    U32BIT *pu32Value
);

extern void ddcUdlBusPciPlx9000_EepromWriteByOffset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Offset,
    U32BIT u32Value
);

#endif /* _DDC_UDL_BUS_PCI_PRIVATE_H_ */
