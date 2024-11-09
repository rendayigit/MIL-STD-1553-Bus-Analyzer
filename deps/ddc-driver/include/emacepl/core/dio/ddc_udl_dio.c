/*******************************************************************************
 * FILE: ddc_udl_dio.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support the
 *  configuration and management of the Discrete I/O (DIO).
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
#include "include/ddc_ioctl.h"
#include "include/ddc_error_list.h"
#include "include/ddc_arinc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"


#define DIO_MAX_BITS    9

/* Discrete IO Direction Masks */
static const U32BIT DDC_UDL_DIO_DIR_BIT[DIO_MAX_BITS] =
{
   0x00000100L, 0x00000200L, 0x00000400L, 0x00000800L,
   0x00001000L, 0x00002000L, 0x00004000L, 0x00008000L,
   0x01000000L
};

/* Discrete IO State Masks */
static const U32BIT DDC_UDL_DIO_STATE_BIT[DIO_MAX_BITS] =
{
   0x00000001L, 0x00000002L, 0x00000004L, 0x00000008L,
   0x00000010L, 0x00000020L, 0x00000040L, 0x00000080L,
   0x00010000L,
};

 /******************************************************************************
 * Name:    GetDioOutput
 *
 * Description:
 *      Returns The Level Of A Discrete Output Or An Error Code
 *
 * In   dev_ext     logical device number
 * In   u32Discrete 
 * Out  none
 *
 * Returns: value of discrete, high or low else error if channel out of range
 *****************************************************************************/
S16BIT GetDioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
)
{
    U32BIT u32RegValue = 0x00000000;
    S16BIT status;

    /* Verify channel range */
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumDiscreteIO)
    {
        return ERR_DISCRETE;
    }

    status = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_OUTPUT, "IOCTL_GET_DIO_OUTPUT read error\n");
        return status;
    }

    if ((u32RegValue & DDC_UDL_DIO_DIR_BIT[u32Discrete - 1]) == FALSE) /* Check discrete direction is output */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_OUTPUT, "IOCTL_GET_DIO_OUTPUT error, not output\n");
        u32RegValue = FALSE;
        return ERR_DISCRETE;
    }

    if (u32RegValue & DDC_UDL_DIO_STATE_BIT[u32Discrete - 1])
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_OUTPUT, "IOCTL_GET_DIO_OUTPUT, return TRUE\n");
        u32RegValue = TRUE;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_OUTPUT, "IOCTL_GET_DIO_OUTPUT, return FALSE\n");
        u32RegValue = FALSE;
    }

    return (S16BIT)u32RegValue;
}

/******************************************************************************
 * Name:    GetDioDirection
 *
 * Description:
 *          Gets direction of a Discrete I/O channel
 *
 * In   pDeviceContext  logical device number
 * In   u32Discrete     
 * Out  none
 *
 * Returns: DISC_OUTPUT or DISC_INPUT if successful else
 *          error if channel is out of range
 *****************************************************************************/
S16BIT GetDioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
)
{
    U32BIT u32RegValue = 0x00000000;

    /* Check channel range */
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumDiscreteIO)
    {
        return ERR_DISCRETE;
    }

    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (u32RegValue & DDC_UDL_DIO_DIR_BIT[u32Discrete - 1]) /* Return discrete direction */
    {
        u32RegValue = DISC_OUTPUT;
    }
    else
    {
        u32RegValue = DISC_INPUT;
    }

    return (S16BIT)u32RegValue;
}

/******************************************************************************
 * Name:    GetDioInput
 *
 * Description:
 *      Returns Level of a Discrete I/O Input
 *
 * In   pDeviceContext  logical device number
 * In   u32Discrete
 * Out  none
 *
 * Returns: DISC_HIGH or DISC_LOW if successful else
 *          error if channel is out of range
 *****************************************************************************/
S16BIT GetDioInput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
)
{
    U32BIT u32RegValue = 0x00000000;
    S16BIT status;

    /* Check channel range */
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumDiscreteIO)
    {
        return ERR_DISCRETE;
    }

    status = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_INPUT, "IOCTL_GET_DIO_INPUT read error\n");
        return status;
    }

    if (u32RegValue & DDC_UDL_DIO_DIR_BIT[u32Discrete - 1]) /* Check discrete direction is input */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_INPUT, "IOCTL_GET_DIO_OUTPUT error, not input\n");
        u32RegValue = FALSE;
        return ERR_DISCRETE;
    }

    if (u32RegValue & DDC_UDL_DIO_STATE_BIT[u32Discrete - 1])
    {
        u32RegValue = TRUE;
    }
    else
    {
        u32RegValue = FALSE;
    }

    return (S16BIT)u32RegValue;
}

/******************************************************************************
 * Name:    GetDioAll
 *
 * Description:
 *      Returns Direction and Level of a Discrete I/O
 *
 * In   pDeviceContext  logical device number
 * Out  none
 *
 * Returns: Discrete I/O Register value
 *****************************************************************************/
U32BIT GetDioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegValue = 0x00000000;
    S16BIT status;

    status = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_ALL, "IOCTL_GET_DIO_ALL read error\n");
        return status;
    }

    u32RegValue = (u32RegValue & 0xFF) | (((u32RegValue >> 8) & 0xFF) << 16);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_GET_ALL, "IOCTL_GET_DIO_ALL RdData = 0x%x\n", (int)u32RegValue);

    return u32RegValue;
}

/******************************************************************************
 * Name:    SetDioOutput
 *
 * Description:
 *      Sets Discrete I/O Level
 *
 * In   pDeviceContext  logical device number
 * In   u32Discrete
 * In   level
 * Out  None
 *
 * Returns: 0 if function successful else Error if channel out of range
 *****************************************************************************/
S16BIT SetDioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete,
    U32BIT level
)
{
    U32BIT u32RegValue;
    S16BIT status;

    /* Verify channel in range */
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumDiscreteIO)
    {
        return ERR_DISCRETE;
    }

    status = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_OUTPUT, "IOCTL_SET_DIO_OUTPUT read error\n");
        return status;
    }

    if (!(u32RegValue & DDC_UDL_DIO_DIR_BIT[u32Discrete - 1])) /* Check discrete direction is output */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_OUTPUT, "IOCTL_GET_DIO_OUTPUT error, not output\n");
        return ERR_DISCRETE;
    }

    if (level)
    {
        u32RegValue |= DDC_UDL_DIO_STATE_BIT[u32Discrete - 1];
    }
    else
    {
        u32RegValue &= ~DDC_UDL_DIO_STATE_BIT[u32Discrete - 1];
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_OUTPUT, "IOCTL_SET_DIO_OUTPUT WrData = 0x%x\n", (int)u32RegValue);
    
    status = DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    SetDioDirection
 *
 * Description:
 *      Sets direction of a Discrete I/O channel
 *
 * In   pDeviceContext  logical device number
 * In   u32Discrete
 * In   level
 * Out  None
 *
 * Returns: 0 if function successful else Error if channel out of range
 *****************************************************************************/
S16BIT SetDioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete,
    U32BIT level
)
{
    U32BIT u32RegValue;
    S16BIT status;

    /* Verify channel in range */
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumDiscreteIO)
    {
        return ERR_DISCRETE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_DIRECTION, 
        "IOCTL_SET_DIO_DIRECTION u32Discrete: 0x%08x level: 0x%08x\n",
        u32Discrete, level);
        
    status = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_DIRECTION, "IOCTL_SET_DIO_DIRECTION read error\n");
        return status;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_DIRECTION, "IOCTL_SET_DIO_DIRECTION read BD DIOCTL: 0x%08x\n", u32RegValue);

    if (level == 1)
    {
        u32RegValue |= DDC_UDL_DIO_DIR_BIT[u32Discrete - 1];
    }
    else
    {
        u32RegValue &= ~DDC_UDL_DIO_DIR_BIT[u32Discrete - 1];
    }

    status = DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);
    
    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_DIRECTION, "IOCTL_SET_DIO_DIRECTION write BD DIOCTL: 0x%08x\n", u32RegValue);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    SetDioAll
 *
 * Description:
 *      Sets Direction and Level of a all Discretes in a single call
 *
 * In   pDeviceContext  logical device number
 * In   u32Discrete
 * Out
 *
 * Returns: none
 *****************************************************************************/
void SetDioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Discrete
)
{
    U32BIT u32RegValue;

    u32RegValue = (u32Discrete & 0xFF) | (((u32Discrete >> 16) & 0xFF) << 8);
    
    DDC_DBG_PRINT(DDC_DBG_MODULE_DIO, DDC_DBG_DIO_SET_ALL, "IOCTL_SET_DIO_ALL WrData = 0x%x\n", (int)u32RegValue);
    
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL), &u32RegValue);
}
