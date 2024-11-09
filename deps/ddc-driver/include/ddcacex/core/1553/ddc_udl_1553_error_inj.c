/*******************************************************************************
 * FILE: ddc_udl_1553_error_inj.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support Multi-Function
 *  components.
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
#include "core/1553/ddc_udl_1553_private.h"


/********************************************************************************
        Error Injection
 *********************************************************************************/

/*-------------------------------------------------------------------------------
   Function:
        errorInj1553Initialize

   Description:
        This function initializes Error Injection component for the given channel.

   Parameters:
      In pDeviceContext - device-specific structure
      In u16Ch          - 1553 channel number

   Returns:
      none
   ---------------------------------------------------------------------------------*/
void errorInj1553Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U16BIT u16RT;
    U32BIT u32Data;
    U32BIT u32RegAddr;

    /* initialize EI module if it is detected */
    if (pDeviceContext->pChannel1553[u16Ch]->sErrorInj.pu32RegBA)
    {
        u32Data = 0;
        u32RegAddr = *(pDeviceContext->pChannel1553[u16Ch]->sErrorInj.pu32RegBA);

        /* disable BC */
        DDC_REG_WRITE(pDeviceContext, (u32RegAddr + ACEX_REG_EI_BC_ENABLE), &u32Data);

        /* disable all RTs */
        DDC_REG_WRITE(pDeviceContext, (u32RegAddr + ACEX_REG_EI_RT_ENABLE), &u32Data);

        /* clear RT error information in EI memory */
        for (u16RT = 0; u16RT <= 32; u16RT++)
        {
            DDC_REG_WRITE(pDeviceContext, (u32RegAddr + ACEX_EI_CONFIG_REG_A_BA + u16RT), &u32Data);
            DDC_REG_WRITE(pDeviceContext, (u32RegAddr + ACEX_EI_CONFIG_REG_B_BA + u16RT), &u32Data);
            DDC_REG_WRITE(pDeviceContext, (u32RegAddr + ACEX_EI_CONFIG_REG_C_BA + u16RT), &u32Data);
        }
    }
}

