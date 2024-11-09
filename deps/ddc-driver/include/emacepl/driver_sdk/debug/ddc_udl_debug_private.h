/*******************************************************************************
 * FILE: ddc_udl_debug_private.h
 *
 * DESCRIPTION:
 *
 *  DDC Device Driver debug header file
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

#ifndef _DDC_UDL_DEBUG_PRIVATE_H_
#define _DDC_UDL_DEBUG_PRIVATE_H_


#include "driver_sdk/debug/ddc_udl_debug.h"
#include "os/debug/ddc_udl_os_debug_private.h"


/* ========================================================================== */
/* ========================================================================== */

/* global debug table */
extern U32BIT u32DebugTable[];


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern S16BIT ddcUdlDebugSetTraceLevel
(
    U32BIT u32Module,
    U32BIT u32TraceLevel
);

extern S16BIT ddcUdlDebugClearTraceLevel
(
    U32BIT u32Module,
    U32BIT u32TraceLevel
);


#endif  /* _DDC_UDL_DEBUG_PRIVATE_H_ */
