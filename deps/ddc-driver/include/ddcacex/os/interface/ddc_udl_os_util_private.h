/*******************************************************************************
 * FILE: ddc_udl_os_util_private.h
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

#ifndef _DDC_UDL_OS_UTIL_PRIVATE_H_
#define _DDC_UDL_OS_UTIL_PRIVATE_H_


#include "driver_sdk/ddc_udl_private.h"

 /******************************************************************************
 * Name:    ddcUdlOsWaitMs
 *
 * Description:
 *      This function implements a generic blocked wait for User Space to have
 *      a blocked time delay. Each channel has one timer with ceiling timeout
 *      value of 1000 ms.
 *
 * In   s32TimeoutMs    Timeout value in milliseconds
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
extern void ddcUdlOsLibWaitMs
(
    S32BIT s32TimeoutMs
);

 extern void ddcUdlOsBoardDelayMs
(
    S32BIT s32TimeoutMs
);

extern void ddcUdlOsWaitMs
(
    S32BIT s32TimeoutMs
);

extern void ddcUdlOsWaitUs
(
    S32BIT s32TimeoutUs
);

extern void ddcUdlOsWaitMsInterruptable
(
    S32BIT s32TimeoutMs
);

extern U32BIT ddcUdlOsGetClockMs
(
    void
);

#endif /* _DDC_UDL_OS_UTIL_PRIVATE_H_ */
