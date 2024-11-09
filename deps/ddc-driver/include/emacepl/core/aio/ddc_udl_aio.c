/*******************************************************************************
 * FILE: ddc_udl_aio.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support 
 *  configuration/management of the AIO.
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
#include "driver_sdk/ddc_udl_iodev_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"

#define AIO_CAP_CHANNEL_CHECK  1

#define ACEX_IO_VALUE_SWAP(u32RegVal)   ((u32RegVal & 0xFF000000) | ((u32RegVal & 0x00FF0000) >> 8) | ((u32RegVal & 0x0000FF00) << 8) | (u32RegVal & 0x000000FF))

#define AIO_MAX_BITS    16

/* Discrete IO Direction Masks */
static const U32BIT DDC_UDL_AIO_DIR_BIT[AIO_MAX_BITS] =
{
    0x00000100L, 0x00000200L, 0x00000400L, 0x00000800L,
    0x00001000L, 0x00002000L, 0x00004000L, 0x00008000L,
    0x01000000L, 0x02000000L, 0x04000000L, 0x08000000L,
    0x10000000L, 0x20000000L, 0x40000000L, 0x80000000L
};

/* Discrete IO State Masks */
static const U32BIT DDC_UDL_AIO_STATE_BIT[AIO_MAX_BITS] =
{
    0x00000001L, 0x00000002L, 0x00000004L, 0x00000008L,
    0x00000010L, 0x00000020L, 0x00000040L, 0x00000080L,
    0x00010000L, 0x00020000L, 0x00040000L, 0x00080000L,
    0x00100000L, 0x00200000L, 0x00400000L, 0x00800000L
};


/******************************************************************************
 * Name:          AioRegRead
 *
 * Description:   Returns AIO register value
 *
 * In             struct _DDC_UDL_DEVICE_CONTEXT *dev_ext - logical device number
 * Out            u32RegVal - AIO register value
 * Out            NTSTATUS.
 *****************************************************************************/
static S16BIT AioRegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *u32RegVal
)
{
    U32BIT u32RegData;
    S16BIT status;

    /* use the CPLD context if there is one */
    if (pDeviceContext->pDeviceContextCPLD)
    {
        pDeviceContext = (struct _DDC_UDL_DEVICE_CONTEXT *)pDeviceContext->pDeviceContextCPLD;
    }

    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        status = DDC_REG_READ(pDeviceContext,
            REG_IODEV_AIO,
            &u32RegData);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            return status;
        }

        /* change to compatible format */
        *u32RegVal = ACEX_IO_VALUE_SWAP(u32RegData);
    }
    else
    {
        status = DDC_REG_READ(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1,
            &u32RegData);
            
        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            return status;
        }

        *u32RegVal = u32RegData;
    }

    /* get only the AIO channels that are supported */
    *u32RegVal &= pDeviceContext->u32AvionicIOMask;

    return status;
}

/******************************************************************************
 * Name:          AioRegWrite
 *
 * Description:   Write AIO register with given value
 *
 * In             struct _DDC_UDL_DEVICE_CONTEXT *dev_ext - logical device number
 * In             u32RegVal - AIO register value to write
 * Out            NTSTATUS.
 *****************************************************************************/
S16BIT AioRegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32RegVal
)
{
    U32BIT u32RegData;
    S16BIT status;

    /* use the CPLD context if there is one */
    if (pDeviceContext->pDeviceContextCPLD)
    {
        pDeviceContext = (struct _DDC_UDL_DEVICE_CONTEXT *)pDeviceContext->pDeviceContextCPLD;
    }

    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        /* change to compatible format and set only the AIO channels that are supported */
        u32RegData = ACEX_IO_VALUE_SWAP(u32RegVal & pDeviceContext->u32AvionicIOMask);

        status = DDC_REG_WRITE(pDeviceContext, REG_IODEV_AIO, &u32RegData);
    }
    else
    {
        /* set only the AIO channels that are supported */
        u32RegData = u32RegVal & pDeviceContext->u32AvionicIOMask;

        status = DDC_REG_WRITE(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1,
            &u32RegData);
    }

    return status;
}

/******************************************************************************
 * Name:    GetAioOutput
 *
 * Description:
 *      Returns The Level Of A Discrete Output Or An Error Code
 *
 * In   dev_ext     logical device number
 * In   pIoctlParams   channel number to read
 * Out  none
 *
 * Returns: value of discrete, high or low else error if channel out of range.
 *****************************************************************************/
S16BIT GetAioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Discrete = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32RegValue;

    /* Verify channel range */
#if AIO_CAP_CHANNEL_CHECK
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumAvionicIO)
    {
        return DDC_UDL_ERROR__AVIONIC;
    }
#endif /* AIO_CAP_CHANNEL_CHECK */

    /* Read in AIO register value. AIO register WORD address, 0x41D << 1 */
    AioRegRead(pDeviceContext, &u32RegValue);

    /* return discrete output value */
    if (u32RegValue & DDC_UDL_AIO_STATE_BIT[u32Discrete - 1])
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_OUTPUT, "GetAioOutput: ch%d AVIONIC_HIGH, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_HIGH;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_OUTPUT, "GetAioOutput: ch%d AVIONIC_LOW, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_LOW;
    }
}

/******************************************************************************
 * Name:    GetAioDirection
 *
 * Description:
 *          Gets direction of a Discrete Avionic channel
 *
 * In   pDeviceContext  logical device number
 * In   pIoctlParams       channel number to retrieve direction.
 * Out  none
 *
 * Returns: AVIONIC_OUTPUT or AVIONIC_INPUT if successful else
 *          error if channel is out of range.
 *****************************************************************************/
S16BIT GetAioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Discrete = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32RegValue;

    /* Check channel range */
#if AIO_CAP_CHANNEL_CHECK
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumAvionicIO)
    {
        return DDC_UDL_ERROR__AVIONIC;
    }
#endif /* AIO_CAP_CHANNEL_CHECK */

    /* Read in AIO register value */
    AioRegRead(pDeviceContext, &u32RegValue);

    /* Return discrete direction */
    if (u32RegValue & DDC_UDL_AIO_DIR_BIT[u32Discrete - 1])
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_DIRECTION, "GetAioDirection: ch%d AVIONIC_OUTPUT, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_OUTPUT;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_DIRECTION, "GetAioDirection: ch%d AVIONIC_INPUT, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_INPUT;
    }
}

/******************************************************************************
 * Name:    GetAioInput
 *
 * Description:
 *      Returns Level of a Avionic Discrete Input
 *
 * In   pDeviceContext  logical device number
 * In   pIoctlParams       channel number to retrieve direction
 * Out  none
 *
 * Returns: AVIONIC_HIGH or AVIONIC_LOW if successful else
 *          error if channel is out of range.
 *****************************************************************************/
S16BIT GetAioInput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Discrete = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32RegValue;

    /* Check channel range */
#if AIO_CAP_CHANNEL_CHECK
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumAvionicIO)
    {
        return DDC_UDL_ERROR__AVIONIC;
    }
#endif /* AIO_CAP_CHANNEL_CHECK */

    /* read in DIO register value */
    AioRegRead(pDeviceContext, &u32RegValue);

    /* Check if discrete direction is input */
    if (u32RegValue & DDC_UDL_AIO_DIR_BIT[u32Discrete - 1])
    {
        return DDC_UDL_ERROR__AVIONIC;
    }

    /* return discrete input value */
    if (u32RegValue & DDC_UDL_AIO_STATE_BIT[u32Discrete - 1])
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_INPUT, "GetAioInput: ch%d AVIONIC_HIGH, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_HIGH;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_INPUT, "GetAioInput: ch%d AVIONIC_LOW, 0x%08x\n", u32Discrete, u32RegValue);
        return AVIONIC_LOW;
    }
}

/******************************************************************************
 * Name:    GetAioAll
 *
 * Description:
 *      Returns Direction and Level of a Avionic Discretes
 *
 * In   pDeviceContext  logical device number
 * Out  none
 *
 * Returns: Avionic Discretes Register value
 *****************************************************************************/
U32BIT GetAioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegValue;

    /* read in DIO register value */
    AioRegRead(pDeviceContext, &u32RegValue);
    DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_GET_ALL, "GetAioAll: 0x%08x\n", u32RegValue);

    return ACEX_IO_VALUE_SWAP(u32RegValue);
}

/******************************************************************************
 * Name:    SetAioOutput
 *
 * Description:
 *      Sets Avionic Discrete Level
 *
 * In   pDeviceContext  logical device number
 * In   pIoctlParams       Discrete channel number to set (high/low)
 * Out  None
 *
 * Returns: 0 if function successful else Error if channel out of range.
 *****************************************************************************/
S16BIT SetAioOutput
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Discrete = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT level = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32RegValue;

    /* Verify channel in range */
#if AIO_CAP_CHANNEL_CHECK
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumAvionicIO)
    {
        return DDC_UDL_ERROR__AVIONIC;
    }
#endif /* AIO_CAP_CHANNEL_CHECK */

    /* Read in AIO register value */
    AioRegRead(pDeviceContext, &u32RegValue);

    /* Check if discrete direction is output */
    if (!(u32RegValue & DDC_UDL_AIO_DIR_BIT[u32Discrete - 1]))
    {
        return DDC_UDL_ERROR__AVIONIC;
    }

    /* set value to the Discrete bit */
    if (level)
    {
        u32RegValue |= DDC_UDL_AIO_STATE_BIT[u32Discrete - 1];
    }
    else
    {
        u32RegValue &= ~DDC_UDL_AIO_STATE_BIT[u32Discrete - 1];
    }

    /* Write to AIO register */
    DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_SET_OUTPUT, "SetAioOutput: ch%d 0x%08x\n", u32Discrete, u32RegValue);
    AioRegWrite(pDeviceContext, u32RegValue);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    SetAioDirection
 *
 * Description:
 *      Sets direction of a Discrete Avionic channel
 *
 * In   pDeviceContext  logical device number
 * In   pIoctlParams       channel number and direction to set (input or output)
 * Out  None
 *
 * Returns: 0 if function successful else Error if channel out of range
 *****************************************************************************/
S16BIT SetAioDirection
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Discrete = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT level = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32RegValue;

    /* Verify channel in range */
#if AIO_CAP_CHANNEL_CHECK
    if (u32Discrete < 1 || u32Discrete > pDeviceContext->u8NumAvionicIO)
    {
        return DDC_UDL_ERROR__AVIONIC;
    }
#endif /* AIO_CAP_CHANNEL_CHECK */

    /* read in AIO register value.
     * AIO register WORD address, 0x41D << 1 */
    AioRegRead(pDeviceContext, &u32RegValue);

    /* Set Discrete bit */
    if (level)
    {
        u32RegValue |= DDC_UDL_AIO_DIR_BIT[u32Discrete - 1];
    }
    else
    {
        u32RegValue &= ~DDC_UDL_AIO_DIR_BIT[u32Discrete - 1];
    }

    /* Write to AIO register.
     * AIO register WORD address, 0x41D << 1 */
    DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_SET_DIRECTION, "SetAioDirection: ch%d 0x%08x\n", u32Discrete, u32RegValue);
    AioRegWrite(pDeviceContext, u32RegValue);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    SetAioAll
 *
 * Description:
 *      Sets Direction and Level of a all Avionic Discretes in a single call
 *
 * In   pDeviceContext  logical device number
 * In   Acionics        Discretes Register value
 * Out
 *
 * Returns: none
 *****************************************************************************/
void SetAioAll
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT Avionics
)
{
    U32BIT u32RegValue;

    u32RegValue = ACEX_IO_VALUE_SWAP(Avionics);

    /* Write to AIO register */
    DDC_DBG_PRINT(DDC_DBG_MODULE_AIO, DDC_DBG_AIO_SET_ALL, "SetAioAll: 0x%08x\n", u32RegValue);
    AioRegWrite(pDeviceContext, u32RegValue);
}

