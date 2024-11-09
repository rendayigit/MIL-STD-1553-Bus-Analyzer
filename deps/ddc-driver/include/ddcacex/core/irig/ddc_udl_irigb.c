/*******************************************************************************
 * FILE: ddc_udl_irigb.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the 1553 IRIG B block.
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
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_um_regmap_private.h"
#include "driver_sdk/ddc_udl_iodev_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"


/******************************************************************************
 * Name:    irigbInitialize
 *
 * Description:
 *      initialize device IRIG B Block, get reg info from board info
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT irigbInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    U16BIT numDevicesIndex;
    U16BIT numComponentsIndex;
    UM_DEVICE_INFO  *pUmDevicePtr;
    U8BIT u8InstanceIndex = 0;
    BOOLEAN bDeviceFound = FALSE;

    pDeviceContext->u8IrigbInstanceCount = u8InstanceIndex;

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* search for Board virtual device */

    DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
        "There are %d devices.\n", pUmInfo->numDevices);

    /* search for Board virtual device */
    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        if (pUmDevicePtr->umDevType == UM_DEVICE_ID_IRIG_B)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
                "Device %d has %d components.\n", 
                numDevicesIndex, 
                pUmDevicePtr->umDevNumComponents);

            for (numComponentsIndex = 0; numComponentsIndex < pUmDevicePtr->umDevNumComponents; numComponentsIndex++)
            {
                switch (pUmDevicePtr->umComponentInfo[numComponentsIndex].umComponentType)
                {
                    case UM_COMPONENTS_ID_IRIG_B_RX:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
                            "Device %d component %d is RX\n", 
                            numDevicesIndex, 
                            numComponentsIndex);

                        pDeviceContext->sIrigB_RX[u8InstanceIndex].pu32RegSize = &(pUmDevicePtr->umComponentInfo[numComponentsIndex].umComponentRegSize);
                        pDeviceContext->sIrigB_RX[u8InstanceIndex].pu32RegBA = &(pUmDevicePtr->umComponentInfo[numComponentsIndex].umRegBaseAddr[0]);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
                            "IRIGB RX Base Address is %08X and size is %08X\n", 
                            *pDeviceContext->sIrigB_RX[u8InstanceIndex].pu32RegBA,
                            *pDeviceContext->sIrigB_RX[u8InstanceIndex].pu32RegSize);
                            
                        break;
                    }
                    case UM_COMPONENTS_ID_IRIG_B_TX:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT,
                            "Device %d component %d is TX\n", 
                            numDevicesIndex, 
                            numComponentsIndex);

                        pDeviceContext->sIrigB_TX[u8InstanceIndex].pu32RegSize = &(pUmDevicePtr->umComponentInfo[numComponentsIndex].umComponentRegSize);
                        pDeviceContext->sIrigB_TX[u8InstanceIndex].pu32RegBA = &(pUmDevicePtr->umComponentInfo[numComponentsIndex].umRegBaseAddr[0]);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT,
                            "IRIGB TX Base Address is %08X and size is %08X\n",
                            *pDeviceContext->sIrigB_TX[u8InstanceIndex].pu32RegBA, 
                            *pDeviceContext->sIrigB_TX[u8InstanceIndex].pu32RegSize);
                            
                        break;
                    }
                    default:
                    {
                        /* Unknown Type */
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
                            "Unknown IRIG-B Type: 0x%04X\n",
                            pUmDevicePtr->umComponentInfo[numComponentsIndex].umComponentType);

                        continue;
                    }
                }
            }

            bDeviceFound = TRUE;
            u8InstanceIndex = (U8BIT)(u8InstanceIndex + 1);

            /* Enable IRIG Board level interrupt */
            ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_IRIG_1_SEC);
        }
    }

    if (!bDeviceFound)
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    pDeviceContext->u8IrigbInstanceCount = u8InstanceIndex;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INIT, 
        "There are %d IrigB-RX instances\n", pDeviceContext->u8IrigbInstanceCount);
        
    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    irigbOpen
 *
 * Description:
 *      initializes the IRIG
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT irigbOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* Initialize IRIG B */
    s16Result = irigbInitialize(pDeviceContext);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_OPEN, "IRIG-B Component Initialized; returned result is %d\n", s16Result);

    return s16Result;
}

/******************************************************************************
 * Name:    irigbClose
 *
 * Description:
 *      clears the IRIG interrupt
 *
 * In   pDev    input value for instance information associated with this particular device
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
void irigbClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDev
)
{
    /* Clear IRIG Board level interrupt */
    ddcUdlBdInterruptClear(pDev, BD_INT_STATUS_MASK_IRIG_1_SEC);
}

/******************************************************************************
 * Name:    irigInterruptSet
 *
 * Description:
 *      Sets The State Of The MTI Engine (RESET, READY, RUN).
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pIoctlParams    input values for Channel & State
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT irigInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    BOOLEAN bEnable = (BOOLEAN)pIoctlParams->Param1;
    U32BIT u32TempHold = 0;
    U8BIT u8Index;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INT_SET, 
        "ENTER-> State = %d, Channel = %d, Instances = %d\n",
        bEnable, pIoctlParams->Channel, pDeviceContext->u8BoardInstanceCount);

    for (u8Index = 0; u8Index < pDeviceContext->u8BoardInstanceCount; u8Index++)
    {
        DDC_REG_READ(pDeviceContext, ((*(pDeviceContext->sIrigB_RX[u8Index].pu32RegBA)) + REG_IRIGB_CONTROL_RW), &u32TempHold);

        /* Enable IRIG 1 second interrupt */
        if (bEnable)
        {
            u32TempHold |= IRIGB_CONTROL_1_SEC_TIME_INT_ENABLE;

            /* Enable DMY format also */
            u32TempHold |= IRIGB_CONTROL_DMY_FORMAT_ENABLE;
        }
        else
        {
            /* Since the IRIG is shared across all channels, disable interrupts only if */
            /* no channels are using MTi mode of operation.                             */
            if (pDeviceContext->u16MtiChannelCount == 0)
            {
                u32TempHold &= ~(U32BIT)IRIGB_CONTROL_1_SEC_TIME_INT_ENABLE;
            }
        }

        DDC_REG_WRITE(pDeviceContext, ((*(pDeviceContext->sIrigB_RX[u8Index].pu32RegBA)) + REG_IRIGB_CONTROL_RW), &u32TempHold);

        DDC_DBG_PRINT(DDC_DBG_MODULE_IRIGB, DDC_DBG_IRIGB_INT_SET, "End\n");
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    irigbSetIRIGTx
 *
 * Description:
 *      Description: Sets The State Of The IRIG Transmitter
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pIRIGTx         input values for Channel & State
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT irigbSetIRIGTx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    IRIG_TX *pIRIGTx
)
{
    U32BIT u32Data1, u32Data2;
    struct _DDC_UDL_DEVICE_CONTEXT *pDc;

    U8BIT u8Index = 0;

    pDc = pDeviceContext;

    /* use the CPLD context if there is one */
    if (pDc->pDeviceContextCPLD)
    {
        pDc = (struct _DDC_UDL_DEVICE_CONTEXT *)pDc->pDeviceContextCPLD;
    }

    /* return error if the card does not support IRIGB TX */
    if (!(pDc->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_OUT_DIGITAL))
    {
        return DDC_UDL_ERROR__INVALID_DEVICE_REQUEST;
    }

    /* parse the data structure and put the data in the correct bit locations */
    u32Data1 = ACEX_SET_IRIG_TX_CONTROL_REG_1(pIRIGTx->u16Enable, pIRIGTx->u16Days, pIRIGTx->u16Hours, pIRIGTx->u16Minutes, pIRIGTx->u16Seconds);
    u32Data2 = ACEX_SET_IRIG_TX_CONTROL_REG_2(pIRIGTx->u16Year, pIRIGTx->u32Control);

    /*  IO only device has a different way to access IRIGB TX registers */
    if (pDc->u16DriverType == ACEX_IO_DRIVER)
    {
        /* write the IRIG Tx Registers */
        DDC_REG_WRITE(pDc, REG_IODEV_IRIGB_TX_CONTROL_2, &u32Data2);
        DDC_REG_WRITE(pDc, REG_IODEV_IRIGB_TX_CONTROL_1, &u32Data1);
    }
    else
    {
        /* Capability register does not report number of IRIG Tx.
         * Therefore must assume only one until firmware modification are made to report number of IRIG Tx available
         * in a capability register. */
            /* write the IRIG Tx Registers */
            DDC_REG_WRITE(pDc, ((*(pDc->sIrigB_TX[u8Index].pu32RegBA)) + IRIGB_TX_CONTROL_REG_2), &u32Data2);
            DDC_REG_WRITE(pDc, ((*(pDc->sIrigB_TX[u8Index].pu32RegBA)) + IRIGB_TX_CONTROL_REG_1), &u32Data1);
        }

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    irigbGetIRIGTx
 *
 * Description:
 *      Gets The State Of The IRIG Transmitter
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pIRIGTx         input values for Channel & State
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT irigbGetIRIGTx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    IRIG_TX *pIRIGTx
)
{
    U32BIT u32Data1 = 0;
    U32BIT u32Data2 = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    struct _DDC_UDL_DEVICE_CONTEXT *pDc;

    pDc = pDeviceContext;

    /* use the CPLD context if there is one */
    if (pDc->pDeviceContextCPLD)
    {
        pDc = (struct _DDC_UDL_DEVICE_CONTEXT *)pDc->pDeviceContextCPLD;
    }

    /* return error if the card does not support IRIGB TX */
    if (!(pDc->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_OUT_DIGITAL))
    {
        u32Data1 = 0;
        u32Data2 = 0;

        pIRIGTx->u16IRIGBTxSupported = 0;
        status = DDC_UDL_ERROR__INVALID_DEVICE_REQUEST;
    }
    else
    {
        /*  IO only device has a different way to access IRIGB TX registers */
        if (pDc->u16DriverType == ACEX_IO_DRIVER)
        {
            /* read the IRIG Tx Registers */
            DDC_REG_READ(pDc, REG_IODEV_IRIGB_TX_CONTROL_1, &u32Data1);
            DDC_REG_READ(pDc, REG_IODEV_IRIGB_TX_CONTROL_2, &u32Data2);
        }
        else
        {
            /* read the IRIG Tx Registers */
            DDC_REG_READ(pDc, ((*(pDc->sIrigB_TX[0].pu32RegBA)) + IRIGB_TX_CONTROL_REG_1), &u32Data1);
            DDC_REG_READ(pDc, ((*(pDc->sIrigB_TX[0].pu32RegBA)) + IRIGB_TX_CONTROL_REG_2), &u32Data2);
        }

        pIRIGTx->u16IRIGBTxSupported = 1;
        status = DDC_UDL_ERROR__SUCCESS;
    }

    /* parse the bits and fill in the data structure */
    pIRIGTx->u16Days = (U16BIT)ACEX_GET_IRIG_TX_DAYS(u32Data1);
    pIRIGTx->u16Enable = (U16BIT)ACEX_GET_IRIG_TX_ENABLE(u32Data1);
    pIRIGTx->u16Hours = (U16BIT)ACEX_GET_IRIG_TX_HOURS(u32Data1);
    pIRIGTx->u16Minutes = (U16BIT)ACEX_GET_IRIG_TX_MINUTES(u32Data1);
    pIRIGTx->u16Seconds = (U16BIT)ACEX_GET_IRIG_TX_SECONDS(u32Data1);
    pIRIGTx->u16Year = (U16BIT)ACEX_GET_IRIG_TX_YEAR(u32Data2);
    pIRIGTx->u32Control = (U32BIT)ACEX_GET_IRIG_TX_CONTROL(u32Data2);

    return status;
}
