/*******************************************************************************
 * FILE: ddc_udl_iodev.c
 *
 * DESCRIPTION:
 *
 *  This file provides PCI(e)-specific communication routines
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
#include "driver_sdk/ddc_udl_iodev_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_version_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_driver_private.h"


/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */

static S16BIT ioDevBindingInitQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

static S16BIT ioDevBindingInitCPLD
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

static void ioDevBindingQprmCallBackFromCPLD
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDevConCPLD
);

static void ioDevBindingCpldCallBackFromQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);


/* ========================================================================== */


/* ========================================================================== */
/* ========================================================================== */


/*-------------------------------------------------------------------------------
   Function:
        ioDevOpen

   Description:
        This function initializes IO only device.

   Parameters:
      In pDeviceContext - device-specific structure

   Returns:
      none
   ---------------------------------------------------------------------------------*/
S16BIT ioDevOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegVal = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    pDeviceContext->eState = ACEX_OPEN;

    /* clear hardware version info */
    memset(&pDeviceContext->sHwVersionInfo, 0, sizeof(pDeviceContext->sHwVersionInfo));

    /* fill in structure for hardware version info API */
    pDeviceContext->sHwVersionInfo.dwFamilyNumber = (FAMILY)DDC_ACEX;
    pDeviceContext->sHwVersionInfo.dwDriverVersion = ACEX_DRIVER_VERSION;
    memcpy(pDeviceContext->sHwVersionInfo.szDriverVersion, ACEX_DRIVER_VERSION_STR, sizeof(ACEX_DRIVER_VERSION_STR));
    pDeviceContext->sHwVersionInfo.dwHdlNumber = pDeviceContext->pUmInfo->u32DataArcNum;
    pDeviceContext->sHwVersionInfo.dwCapabilities = 0;

    pDeviceContext->u8BoardInstanceCount = 1;

    if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67112Y") != NULL)
    {
        sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName, "BU-%s%d", "67112Y", pDeviceContext->pUmInfo->numDevices);
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67112Z") != NULL)
    {
        sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName, "BU-%s%d", "67112Z", pDeviceContext->pUmInfo->numDevices);
    }

    /* FW version*/
    pDeviceContext->sHwVersionInfo.dwHdlVersion = pDeviceContext->pUmInfo->firmwareRelVersion;
    pDeviceContext->sHwVersionInfo.dwFwVersion = pDeviceContext->pUmInfo->firmwareIntVersion;

    /* set IRIGB capability */
    DDC_REG_READ(pDeviceContext, REG_IODEV_DEV_CAPABILITY_2, &u32RegVal);
    if (u32RegVal & IODEV_BOARD_CONFIG_IRIGB_TX_ENABLE)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_IRIG_OUT_DIGITAL;
    }

    /* initialize device IO counts */
    DDC_REG_READ(pDeviceContext, REG_IODEV_DEV_CAPABILITY, &u32RegVal);

    pDeviceContext->u8NumDiscreteIO = 0;
    pDeviceContext->u8NumAvionicIO = (U8BIT)(u32RegVal & 0XFF);
    pDeviceContext->u32AvionicIOMask = ddcUdlBdGenerateAvionicIOMask(pDeviceContext);
    pDeviceContext->u8Num1553Channels = 0;
    pDeviceContext->u8NumDed429Tx = 0;
    pDeviceContext->u8NumDed429Rx = 0;
    pDeviceContext->u8NumRS232 = 0;
    pDeviceContext->u8NumRS485 = 0;

    /* turn off 10MHz clock output by default */
    u32RegVal = 0;
    DDC_REG_WRITE(pDeviceContext, REG_IODEV_BOARD_CONFIG, &u32RegVal);

    /* set board load completed flag */
    pDeviceContext->bBoardLoadCompleted = TRUE;
    return status;
}

/*-------------------------------------------------------------------------------
   Function:
        ioDevBinding

   Description:
        This routine binds XMC devices into one virtual board.

   Parameters:
      In pDeviceContext - device-specific structure
   ---------------------------------------------------------------------------------*/
void ioDevBinding
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    switch (pDeviceContext->u16DriverType)
    {
        case ACEX_IO_DRIVER:
        {
            /* start CPLD device binding */
            ioDevBindingInitCPLD(pDeviceContext);
            break;
        }

        case ACEX_QPRM_DRIVER:
        {
            /* start QPRM device binding */
            ioDevBindingInitQPRM(pDeviceContext);
            break;
        }

        default:
        {
            /* doing nothing */
            break;
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
        ioDevBindingInitQPRM

   Description:
        This function initializes the QPRM binding process.

   Parameters:
      In pDeviceContext - device-specific structure
   ---------------------------------------------------------------------------------*/
static S16BIT ioDevBindingInitQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (pDeviceContext->u16DriverType == ACEX_QPRM_DRIVER)
    {
        ioDevBindingCpldCallBackFromQPRM(pDeviceContext);
    }

    return status;
}

/*-------------------------------------------------------------------------------
   Function:
        ioDevBindingInitCPLD

   Description:
        This function initializes the CPLD binding process.

   Parameters:
      In pDeviceContext - device-specific structure
   ---------------------------------------------------------------------------------*/
static S16BIT ioDevBindingInitCPLD
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        ioDevBindingQprmCallBackFromCPLD(pDeviceContext);
    }

    return status;
}

/*--------------------------------------------------------------------------
   Function: ioDevUpdateQPRM

   Description: This function is to replace QPRM information witn those from
                its CPLD module.

   In:  pDeviceContext  QPRM device context
   In:  pDevConCPLD     CPLD device context
   -----------------------------------------------------------------------------*/
void ioDevUpdateQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _DDC_UDL_DEVICE_CONTEXT *pDevConCPLD
)
{
        /* save the CPLD device context pointer for future use */
    pDeviceContext->pDeviceContextCPLD = pDevConCPLD;

    /* update Model Number Name from CPLD device*/
    memcpy(&pDeviceContext->pUmInfo->s8BrdModelNum, &pDevConCPLD->pUmInfo->s8BrdModelNum, 10);
    memcpy(&pDeviceContext->sHwVersionInfo.szModelName, &pDevConCPLD->sHwVersionInfo.szModelName, 32);

    /* update DIO count, AIO count and capability from CPLD device*/
    pDeviceContext->u8NumDiscreteIO = pDevConCPLD->u8NumDiscreteIO;
    pDeviceContext->u8NumAvionicIO = pDevConCPLD->u8NumAvionicIO;
    pDeviceContext->u32AvionicIOMask = ddcUdlBdGenerateAvionicIOMask(pDeviceContext);
    pDeviceContext->sHwVersionInfo.dwCapabilities |= pDevConCPLD->sHwVersionInfo.dwCapabilities;

    pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO = pDevConCPLD->u8NumAvionicIO;
    pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO = pDeviceContext->u8NumDiscreteIO;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_UPDATE_QPRM, "Board Model Number updated to %s\n",
        pDeviceContext->pUmInfo->s8BrdModelNum);
    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_UPDATE_QPRM, "QPRM Bus %d Dev %d attached to its CPLD device!\n",
        pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum);
}

/*--------------------------------------------------------------------------
   Function: ioDevBindingQprmCallBackFromCPLD

   Description: This function is called from CPLD device to let all QPRM
                devices to attach to their CPLD device.

                The QPRM and CPLD will belong to one board only if they
                are in the same PCI BUS.

   In:  pDevConCPLD  CPLD device context
   -----------------------------------------------------------------------------*/
static void ioDevBindingQprmCallBackFromCPLD
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDevConCPLD
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext;
    U32BIT i;
    U8BIT u8CardCount = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "ioDevBindingQprmCallBackFromCPLD: ENTER ---\n");

    /* exit if CPLD device context is NULL */
    if (!pDevConCPLD)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "CPLD device unknown\n");
        return;
    }

    /* exit if it is not notified by CPLD */
    if (pDevConCPLD->u16DriverType != ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Exit because it is not notified by CPLD\n");
        return;
    }

    u8CardCount = ddcUdlGetDeviceCount();
    
    /* find all QPRM devices and update them */
    for (i = 0; i < u8CardCount; i++)
    {
        pDeviceContext = ddcUdlGetDeviceContext((U8BIT)i);

        if (!pDeviceContext)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Skipped because it is not a valid device desciptor\n");
            continue;
        }

        /* skip if it is not a QPRM device */
        if (pDeviceContext->u16DriverType != ACEX_QPRM_DRIVER)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Skipped because it is not a QPRM device\n");
            continue;
        }

        /* skip if the device is not ready */
        if (pDeviceContext->bBoardLoadCompleted == FALSE)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Skipped because the device load is not completed\n");
            continue;
        }
        
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "QPRM %d pDev 0x%p on Bus %d notified by dev %d pDev 0x%p\n",
            pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDeviceContext, pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum,
            pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDevConCPLD);

        /* skip if QPRM and CPLD are not in the same BUS */
        if (pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum != pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Skipped because QPRM is on Bus %d but CPLD is on Bus %d\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum);
            continue;
        }

        /* skip if QPRM and CPLD are the same device */
        if (pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum == pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "Skipped because QPRM is on Dev %d but CPLD is on Dev %d\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum);
            continue;
        }

        /* update QPRM device */
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "ioDevBindingQprmCallBackFromCPLD: CPLD %d:%d binding with QPRM %d:%d\n",
            pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum,
            pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum);
            
        ioDevUpdateQPRM(pDeviceContext, pDevConCPLD);
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_CPLD, "ioDevBindingQprmCallBackFromCPLD: EXIT ---\n");
}

/*-------------------------------------------------------------------
   Function: ioDevBindingCpldCallBackFromQPRM

   Description: This function is called from QPRM device to allow it
                to attach to its CPLD device.

                The QPRM and CPLD will belong to one board only if they
                are in the same PCI BUS.

   In:  Context     struct _DDC_UDL_DEVICE_CONTEXT *
        Arg1        NULL
        Arg2        NULL
   --------------------------------------------------------------------*/
static void ioDevBindingCpldCallBackFromQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDevConCPLD;
    U32BIT i;
    U8BIT u8CardCount = 0;
    
    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "ioDevBindingCpldCallBackFromQPRM: ENTER ===\n");

    /* exit if device context is NULL */
    if (!pDeviceContext)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "unknown device\n");
        return;
    }

    /* exit if it is not notified by QPRM */
    if (pDeviceContext->u16DriverType != ACEX_QPRM_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Exit because it is not notified by QPRM\n");
        return;
    }

    u8CardCount = ddcUdlGetDeviceCount();
    
    /* find CPLD module and update this QPRM device */
    for (i = 0; i < u8CardCount; i++)
    {
        pDevConCPLD = ddcUdlGetDeviceContext((U8BIT)i);
        
        if (!pDevConCPLD)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Skipped because it is not a valid device descriptor\n");
            continue;
        }

        /* skip if the device is not ready */
        if (pDevConCPLD->bBoardLoadCompleted == FALSE)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Skipped because the device load is not completed\n");
            continue;
        }

        /* skip if it is not notified by CPLD */
        if (pDevConCPLD->u16DriverType != ACEX_IO_DRIVER)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Skipped because it is not a CPLD module\n");
            continue;
        }

        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "CPLD %d pDev 0x%p on Bus %d notified by dev %d pDev 0x%p\n",
            pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDevConCPLD, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum,
            pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDeviceContext);

        /* skip if QPRM and CPLD are not in the same BUS */
        if (pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum != pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Skipped because QPRM is on Bus %d but CPLD is on Bus %d\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum);
            continue;
        }

        /* skip if QPRM and CPLD are the same device */
        if (pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum == pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "Skipped because QPRM is on Dev %d but CPLD is on Dev %d\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum);
            continue;
        }

        DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "ioDevBindingCpldCallBackFromQPRM: QPRM %d:%d binding with CPLD %d:%d\n",
            pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum,
            pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciBusNum, pDevConCPLD->ddcOsDevInfo.sBusInfo.u16PciDevNum);

        /* update QPRM device */
        ioDevUpdateQPRM(pDeviceContext, pDevConCPLD);

        /* exit after the QPRM device is attached to its CPLD module */
        break;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_IODEV, DDC_DBG_IODEVICE_BINDING_FROM_QPRM, "ioDevBindingCpldCallBackFromQPRM: EXIT ===\n");
}
