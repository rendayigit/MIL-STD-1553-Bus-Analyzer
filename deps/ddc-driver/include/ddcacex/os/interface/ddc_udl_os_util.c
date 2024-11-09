/*******************************************************************************
 * FILE: ddc_udl_os_util.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide various utility functions. The 
 *  details of these functions are Operating System specific.
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
#include "os/include/ddc_os_private.h"
#include "driver_sdk/ddc_udl_private.h"
 
#define WAIT_TIMEOUT_MAX_MS             1000

/* ========================================================================== */
/* ========================================================================== */
void ddcUdlOsLibWaitMs
(
    S32BIT s32TimeoutMs
)
{    
    wait_queue_head_t wait;

    DDC_INIT_WAITQUEUE(&wait);
    
    /* verify timeout value does not exceed limit */
    if (s32TimeoutMs > WAIT_TIMEOUT_MAX_MS)
    {
        s32TimeoutMs = WAIT_TIMEOUT_MAX_MS;
    }

    DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(wait, 0, s32TimeoutMs);
}

/* ========================================================================== */
/* ========================================================================== */
void ddcUdlOsBoardDelayMs
(
    S32BIT s32TimeoutMs
)
{

}

/* ========================================================================== */
/* ========================================================================== */
void ddcUdlOsWaitMs
(
    S32BIT s32TimeoutMs
)
{
    wait_queue_head_t wait;

    DDC_INIT_WAITQUEUE(&wait);
    
    /* verify timeout value does not exceed limit */
    if (s32TimeoutMs > WAIT_TIMEOUT_MAX_MS)
    {
        s32TimeoutMs = WAIT_TIMEOUT_MAX_MS;
    }

    DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(wait, 0, s32TimeoutMs);
}

/* ========================================================================== */
/* ========================================================================== */
void ddcUdlOsWaitUs
(
    S32BIT s32TimeoutUs
)
{
    udelay(s32TimeoutUs);
}

/* ========================================================================== */
/* ========================================================================== */
void ddcUdlOsWaitMsInterruptable
(
    S32BIT s32TimeoutMs
)
{
    ddcUdlOsWaitMs(s32TimeoutMs);
}

/*******************************************************************************
 * Name:         ddcUdlOsGetClockMs
 *
 * Description:  Returns the number of milliseconds that elapsed since system boot.
 *               The roll over of the epoc is 49.7 days, if that matters.
 *  
 *               This could be a problem if a process is to exist for that long and
 *               a call to this function is made before and after the rollover for
 *               time comparison purposes.
 *
 * In:           none
 *
 * Out:          32-bit value representing milliseconds since system boot
 ******************************************************************************/
U32BIT ddcUdlOsGetClockMs
(
    void
)
{
    return jiffies_to_msecs(jiffies);
}
