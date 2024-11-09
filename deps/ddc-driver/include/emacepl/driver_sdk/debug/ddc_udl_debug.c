/*******************************************************************************
 * FILE: ddc_udl_debug.c
 *
 * DESCRIPTION:
 *
 *  This file provides driver debug support.
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
#include "include/ddc_error_list.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"


U32BIT u32DebugTable[DDC_DBG_MODULE_MAX] = { 0 };

/*******************************************************************************
 * Name:    ddcUdlDebugSetTraceLevel
 *
 * Description:
 *      This function sets the trace level for the corresponding module.
 *
 * In   u32Module           Module index
 * In   u32TraceLevel       Trace Level for module
 * Out  none
 *
 * Returns: error condition on invalid option
 ******************************************************************************/
S16BIT ddcUdlDebugSetTraceLevel
(
    U32BIT u32Module,
    U32BIT u32TraceLevel
)
{
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;

    if (u32Module < DDC_DBG_MODULE_MAX)
    {
        u32DebugTable[u32Module] |= u32TraceLevel;
    }
    else
    {
        /* ERROR: module value out of range */
        s16Result = DDC_UDL_ERROR__PARAMETER;
    }

    return s16Result;
}

/*******************************************************************************
 * Name:    ddcUdlDebugClearTraceLevel
 *
 * Description:
 *      This function clears the trace level for the corresponding module.
 *
 * In   u32Module           Module index
 * In   u32TraceLevel       Trace Level for module
 * Out  none
 *
 * Returns: error condition on invalid option
 ******************************************************************************/
S16BIT ddcUdlDebugClearTraceLevel
(
    U32BIT u32Module,
    U32BIT u32TraceLevel
)
{
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;

    if (u32Module < DDC_DBG_MODULE_MAX)
    {
        u32DebugTable[u32Module] &= ~(u32TraceLevel);
    }
    else
    {
        /* ERROR: module value out of range */
        s16Result = DDC_UDL_ERROR__PARAMETER;
    }

    return s16Result;
}
