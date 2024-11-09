/*******************************************************************************
 * FILE: ddc_udl_os_interrupt.c
 *
 * DESCRIPTION:
 *
 *  This module contains the OS specific functions to connect and 
 *  disconnect interrupts for the bus in use.
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

#include "os/include/ddc_os_types.h"
#include "driver_sdk/ddc_udl_private.h"
#include "ddc_udl_os_bus_interrupt_private.h"

/*******************************************************************************
 * Name:    ddcUdlOsIrqHookISR
 *
 * Description:
 *      This function hooks up the Interrupt Service Routine
 *
 *
 * In   pDeviceContext  device-specific structure
 * In   u8CardNumber    card number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsIrqHookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
)
{
    return ddcUdlOsBusIrqHookISR(pDeviceContext, u8CardNumber);
}

/*******************************************************************************
 * Name:    ddcUdlOsIrqUnhookISR
 *
 * Description:
 *      This function unhooks the Interrupt Service Routine
 *
 *
 * In   pDeviceContext  device-specific structure
 * In   u8CardNumber    card number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsIrqUnhookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
)
{
    return ddcUdlOsBusIrqUnhookISR(pDeviceContext, u8CardNumber);
}
