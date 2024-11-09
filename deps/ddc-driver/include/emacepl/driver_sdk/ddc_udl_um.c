/*******************************************************************************
 * FILE: ddc_udl_um.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to parse the
 *  Unified Memory Map description and organize the configuration information
 *  into a UM data structure.  The Unified Memory Map description
 *  identifies the configuration of the device.
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
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"


/* define strings to store in UM structure */
#define TYPE_CAPABILITIES_STRING                "CAPABILITIES           "
#define TYPE_BRD_GLOBAL_STRING                  "BRD_GLOBAL             "

#define TYPE_BRD_RESETS_STRING                  "BRD_RESETS             "
#define TYPE_BRD_SW_LOCK_STRING                 "BRD SW LOCK            "
#define TYPE_BRD_GLOBAL_VOLT_MON_X8_STRING      "BRD VOLT MON X8        "

#define TYPE_IRIG_B_STRING                      "IRIG_B                 "

#define TYPE_IRIG_B_RX_STRING                   "IRIG_B RX              "
#define TYPE_IRIG_B_TX_STRING                   "IRIG_B TX              "

#define TYPE_MIO_UART_STRING                    "UART                   "

/* Cast UART support */
#define TYPE_MIO_CAST_UART_GLOBAL_STRING        "UART CAST SERIAL GLOBAL"
#define TYPE_MIO_CAST_UART_SERIAL_STRING        "UART CAST SERIAL ASYNC "
#define TYPE_MIO_CAST_UART_HDLC_SDLC_STRING     "UART CAST HDLC SDLC    "

#define TYPE_MIL_STD_1553_STRING                "MIL_STD_1553           "

#define TYPE_MIL_STD_1553_SF_STRING             "MIL_STD_1553 SF        "
#define TYPE_MIL_STD_1553_MF_STRING             "MIL_STD_1553 MF        "

#define TYPE_1553_SF_GLOBAL_STRING              "1553 SF GLOBAL         "
#define TYPE_1553_SF_BCI_STRING                 "1553 SF BC             "
#define TYPE_1553_SF_RTX_STRING                 "1553 SF MRT            "
#define TYPE_1553_SF_MTIE_STRING                "1553 SF MTiE           "
#define TYPE_1553_SF_IMP_STRING                 "1553 SF IMP            "

#define TYPE_1553_MF_GLOBAL_STRING              "1553 MF GLOBAL         "
#define TYPE_1553_MF_BCI_STRING                 "1553 MF BC             "
#define TYPE_1553_MF_RTX_STRING                 "1553 MF MRT            "
#define TYPE_1553_MF_MTIE_STRING                "1553 MF MTiE           "
#define TYPE_1553_MF_IMP_STRING                 "1553 MF IMP            "
#define TYPE_1553_MF_IMP_BC_STRING              "1553 MF IMP-BC         "
#define TYPE_1553_MF_IMP_MRT_STRING             "1553 MF IMP-MRT        "
#define TYPE_1553_MF_EI_STRING                  "1553 MF EI             "
#define TYPE_1553_MF_REPLAY_STRING              "1553 MF REPLAY         "
#define TYPE_1553_MF_TRIGGERS_STRING            "1553 MF TRIGGERS       "
#define TYPE_CAN_BUS_STRING                     "CAN BUS                "
#define TYPE_QPRM_DMA                           "QPRIME_DMA             "
#define TYPE_QPRM_GENERAL                       "QPRIME_GENERAL         "
#define TYPE_QPRM_BIST                          "QPRIME_BIST            "

#define TYPE_ARINC_429_GLOBAL_STRING            "ARINC_429_GLOBAL       "

#define TYPE_ARINC_429_GLOBAL_TX_STRING         "429 GLOBAL TX          "
#define TYPE_ARINC_429_GLOBAL_RX_STRING         "429 GLOBAL RX          "
#define TYPE_ARINC_429_TX_STRING                "429 TX                 "
#define TYPE_ARINC_429_RX_STRING                "429 RX                 "

#define TYPE_ARINC_429_GLOBAL_TX_MF_STRING      "429 GLOBAL TX MF       "
#define TYPE_ARINC_429_GLOBAL_RX_MF_STRING      "429 GLOBAL RX MF       "
#define TYPE_ARINC_429_TX_MF_STRING             "429 TX MF              "
#define TYPE_ARINC_429_RX_MF_STRING             "429 RX MF              "

#define TYPE_ARINC_717_GLOBAL_STRING            "717 GLOBAL             "
#define TYPE_ARINC_717_RX_TX_STRING             "717 RX TX              "

#define TYPE_UNKNOWN_STRING                     "UNKNOWN                "


/******************************************************************************
 * Name:    _umInitEndiannessMode
 *
 * Description:
 *      Parse UM registers and populate UM data structure.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
static S16BIT _umInitEndiannessMode
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32Data;
    S8BIT s8BrdModelNum[10];

    /* Default to assuming HW does the swapping, This information is required by the REG_READ and REG_WRITE functions. */
    /* We will perform some tests to find out if our assumtion is correct */
    pDeviceContext->eEndiannessMode = DDC_ENDIANNESS_DO_HW_SWAP;

    /* Always read first register which has no purpose other then providing useful debugging messages */
    status = DDC_REG_READ(pDeviceContext, 0, &u32Data);

    /* Set and test little endian HW Swapping */
    u32Data = 0x00000000;
    DDC_REG_WRITE(pDeviceContext, 0, &u32Data);

    status = DDC_REG_READ(pDeviceContext, 0, &u32Data);
    if (status)
        return status;

    s8BrdModelNum[0] = (S8BIT)((u32Data >> 24) & 0x00FF);
    s8BrdModelNum[1] = (S8BIT)((u32Data >> 16) & 0x00FF);
    s8BrdModelNum[2] = '\0';

    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_ENDIAN_TEST, "Litte endianness test reads %s\n", s8BrdModelNum);
    if (((s8BrdModelNum[0] == 'B') && (s8BrdModelNum[1] == 'U')) ||
        ((s8BrdModelNum[0] == 'Q') && (s8BrdModelNum[1] == 'P')) ||
        ((s8BrdModelNum[0] == 'D') && (s8BrdModelNum[1] == 'D')))

    {
        pDeviceContext->eEndiannessMode = DDC_ENDIANNESS_DO_HW_SWAP;
        return status;
    }

    /* Set and test big endian HW Swapping */
    u32Data = 0xFFFFFFFF;
    DDC_REG_WRITE(pDeviceContext, 0, &u32Data);

    status = DDC_REG_READ(pDeviceContext, 0, &u32Data);
    if (status)
        return status;

    s8BrdModelNum[0] = (S8BIT)((u32Data >> 24) & 0x00FF);
    s8BrdModelNum[1] = (S8BIT)((u32Data >> 16) & 0x00FF);
    s8BrdModelNum[2] = '\0';

    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_ENDIAN_TEST, "Big endianness test reads %s\n", s8BrdModelNum);
    if (((s8BrdModelNum[0] == 'B') && (s8BrdModelNum[1] == 'U')) ||
        ((s8BrdModelNum[0] == 'Q') && (s8BrdModelNum[1] == 'P')) ||
        ((s8BrdModelNum[0] == 'D') && (s8BrdModelNum[1] == 'D')))

    {
        pDeviceContext->eEndiannessMode = DDC_ENDIANNESS_DO_HW_SWAP;
        return status;
    }

    /* Fall back to SW swapping */
    pDeviceContext->eEndiannessMode = DDC_ENDIANNESS_DO_SW_SWAP;

    return status;
}

/*******************************************************************************
 * Name:    _ddcUdlUmInit
 *
 * Description:
 *      This function allocates the memory for the UM ROM structure. If
 *      successful, set struct to 0.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static S16BIT _ddcUdlUmInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    pDeviceContext->pUmInfo = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(struct _UM_INFO_TYPE));

    if (pDeviceContext->pUmInfo == NULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "ERROR: malloc failed for pDeviceContext->pUmInfo\n");

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    memset(pDeviceContext->pUmInfo, 0, sizeof(*pDeviceContext->pUmInfo));

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    umInitialize
 *
 * Description:
 *      Parse UM registers and populate UM data structure.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT umInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = NULL;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U16BIT numDevicesIndex;
    UM_DEVICE_INFO *pTempUmDevice;
    U32BIT regAddr = 0x00000000;
    U16BIT u16Index;
    U16BIT numComponentsIndex;
    UM_COMPONENT_INFO *pTempUmComponent;
    U32BIT u32Data = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "ENTER->\n");

    status = _ddcUdlUmInit(pDeviceContext);

    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        return status;
    }

    pUmInfo = pDeviceContext->pUmInfo;

    status = _umInitEndiannessMode(pDeviceContext);

    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        return status;
    }

    /* Init Dev and Comp Masks to zero */
    pDeviceContext->u32DeviceMask = 0;
    pDeviceContext->u32ComponentMask = 0;


    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Calling first REG_READ\n");

    /* read Board Model Number */
    status = DDC_REG_READ(pDeviceContext, regAddr++, &u32Data);
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Board Model Number 1 failed\n");
        return status;
    }

    if (u32Data == 0xFFFFFFFF) /* check if data is invalid */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Invalid data read (%08X)\n", u32Data);

        return DDC_UDL_ERROR__HARDWARE_CONFIGURATION;
    }

    pUmInfo->s8BrdModelNum[0] = (S8BIT)((u32Data >> 24) & 0x00ff);
    pUmInfo->s8BrdModelNum[1] = (S8BIT)((u32Data >> 16) & 0x00ff);
    pUmInfo->s8BrdModelNum[2] = (S8BIT)((u32Data >> 8) & 0x00ff);
    pUmInfo->s8BrdModelNum[3] = (S8BIT)((u32Data) & 0x00ff);

    status = DDC_REG_READ(pDeviceContext, regAddr++, &u32Data);
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Board Model Number 2 failed\n");
        return status;
    }

    pUmInfo->s8BrdModelNum[4] = (S8BIT)((u32Data >> 24) & 0x00ff);
    pUmInfo->s8BrdModelNum[5] = (S8BIT)((u32Data >> 16) & 0x00ff);
    pUmInfo->s8BrdModelNum[6] = (S8BIT)((u32Data >> 8) & 0x00ff);
    pUmInfo->s8BrdModelNum[7] = (S8BIT)((u32Data) & 0x00ff);

    pUmInfo->s8BrdModelNum[8] = '\0';
    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Board Model Number is %s\n", pUmInfo->s8BrdModelNum);

    /* read data archive Number */
    status = DDC_REG_READ(pDeviceContext, regAddr++, &(pUmInfo->u32DataArcNum));
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read data archive Number failed\n");
        return status;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Archive Number is 0x%08x\n", (unsigned int)pUmInfo->u32DataArcNum);

    /* read FW release version number, major version number */
    status = DDC_REG_READ(pDeviceContext, regAddr++, &(pUmInfo->firmwareRelVersion));
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read FW release version number failed\n");
        return status;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Firmware Release Version is 0x%08x\n", (unsigned int)pUmInfo->firmwareRelVersion);

    /* read FW internal version number, minor version number */
    status = DDC_REG_READ(pDeviceContext, regAddr++, &(pUmInfo->firmwareIntVersion));
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read FW internal version number failed\n");
        return status;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Firmware Internal Version is 0x%08x\n", (unsigned int)pUmInfo->firmwareIntVersion);

    /* read device count */
    status = DDC_REG_READ(pDeviceContext, regAddr++, &(pUmInfo->numDevices));
    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read device count failed\n");
        return status;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Number of Devices is %d\n", (unsigned int)pUmInfo->numDevices);

    /* exit now for IO only device */
    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Exit for IO device\n");
        return status;
    }


    /* read device information */
    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        /* parse UM for virtual devices on this physical device */
        pTempUmDevice = &pUmInfo->umDeviceInfo[numDevicesIndex];

        /* Read Virtual Device Type */
        status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umDevType));
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "\n");
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device #%d\n", (unsigned int)numDevicesIndex);
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Type is 0x%08x\n", (unsigned int)pTempUmDevice->umDevType);

        if (status)
        {
            return status;
        }

        /* mask off delimiter for now*/
        pTempUmDevice->umDevType = pTempUmDevice->umDevType & 0x0000FFFF;

        switch (pTempUmDevice->umDevType)
        {
            case UM_DEVICE_ID_CAPABILITIES:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_CAPABILITIES_STRING, strlen(TYPE_CAPABILITIES_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_CAPABILITIES;
                break;
            }

            case UM_DEVICE_ID_BRD_GLOBAL:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_BRD_GLOBAL_STRING, strlen(TYPE_BRD_GLOBAL_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_BRD_GLOBAL;
                break;
            }

            case UM_DEVICE_ID_MIL_STD_1553:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_MIL_STD_1553_STRING, strlen(TYPE_MIL_STD_1553_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIL_STD_1553;
                break;
            }

            case UM_DEVICE_ID_MIL_STD_1553_SF:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_MIL_STD_1553_SF_STRING, strlen(TYPE_MIL_STD_1553_SF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIL_STD_1553_SF;
                break;
            }

            case UM_DEVICE_ID_MIL_STD_1553_MF:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_MIL_STD_1553_MF_STRING, strlen(TYPE_MIL_STD_1553_MF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIL_STD_1553_MF;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_GLOBAL:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_GLOBAL_STRING, strlen(TYPE_ARINC_429_GLOBAL_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_GLOBAL;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_TX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_TX_STRING, strlen(TYPE_ARINC_429_TX_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_TX;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_RX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_RX_STRING, strlen(TYPE_ARINC_429_RX_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_RX;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF:
            case UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF_2:
            case UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_TX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_GLOBAL_TX_MF_STRING, strlen(TYPE_ARINC_429_GLOBAL_TX_MF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_GLOBAL_TX_MF;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF:
            case UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF_2:
            case UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_RX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_GLOBAL_RX_MF_STRING, strlen(TYPE_ARINC_429_GLOBAL_RX_MF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_GLOBAL_RX_MF;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_TX_MF:
            case UM_DEVICE_ID_ARINC_429_TX_MF_2:
            case UM_DEVICE_ID_EMBEDDED_ARINC_429_TX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_TX_MF_STRING, strlen(TYPE_ARINC_429_TX_MF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_TX_MF;
                break;
            }

            case UM_DEVICE_ID_ARINC_429_RX_MF:
            case UM_DEVICE_ID_ARINC_429_RX_MF_2:
            case UM_DEVICE_ID_EMBEDDED_ARINC_429_RX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_429_RX_MF_STRING, strlen(TYPE_ARINC_429_RX_MF_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_429_RX_MF;
                break;
            }

            case UM_DEVICE_ID_IRIG_B:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_IRIG_B_STRING, strlen(TYPE_IRIG_B_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_IRIG_B;
                break;
            }

            case UM_DEVICE_ID_MIO_UART:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_MIO_UART_STRING, strlen(TYPE_MIO_UART_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIO_UART;
                break;
            }

            case UM_DEVICE_ID_MIO_CAST_GLOBAL_UART:
            {
                sprintf(pTempUmDevice->umName, "%s", TYPE_MIO_CAST_UART_GLOBAL_STRING);
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIO_CAST_UART_SDLC_HDLC;
                break;
            }
            case UM_DEVICE_ID_MIO_CAST_ASYNC_UART:
            {
                sprintf(pTempUmDevice->umName, "%s", TYPE_MIO_CAST_UART_SERIAL_STRING);
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIO_CAST_UART_SDLC_HDLC;
                break;
            }
            case UM_DEVICE_ID_MIO_CAST_SYNC_ASYNC_UART:
            {
                sprintf(pTempUmDevice->umName, "%s", TYPE_MIO_CAST_UART_HDLC_SDLC_STRING);
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_MIO_CAST_UART_SDLC_HDLC;
                break;
            }

            case UM_DEVICE_ID_CAN_BUS:
            {
                sprintf(pTempUmDevice->umName, "%s", TYPE_CAN_BUS_STRING);
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_CAN_BUS;
                break;
            }

            case UM_DEVICE_ID_ARINC_717_GLOBAL:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_717_GLOBAL_STRING, strlen(TYPE_ARINC_717_GLOBAL_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_717_GLOBAL;
                break;
            }

            case UM_DEVICE_ID_ARINC_717_RX_TX:
            {
                strncpy((char *)pTempUmDevice->umName, TYPE_ARINC_717_RX_TX_STRING, strlen(TYPE_ARINC_717_RX_TX_STRING));
                pDeviceContext->u32DeviceMask |= UM_DEVICE_MASK_ARINC_717_RX_TX;
                break;
            }

            default:
            {
                strncpy((char *)pTempUmDevice->umName, "ILLEGAL", strlen("ILLEGAL"));
                break;
            }
        }

        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Name is %s\n", pTempUmDevice->umName);

        /* Read Virtual Device Rev */
        status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umDevRev));

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Virtual Device Rev failed\n");
            return status;
        }

        /* Read number of instances of this Virtual Device Type */
        status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umDevNumInstances));

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Virtual Device Type failed\n");
            return status;
        }

        /* Read Phys Mem Size for this Virtual Device */
        status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umDevMemSize));

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Phys Mem Size failed\n");
            return status;
        }

        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Rev is 0x%X.\n", (unsigned int)pTempUmDevice->umDevRev);
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Num Instances is %d.\n", (unsigned int)pTempUmDevice->umDevNumInstances);
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Mem Size is 0x%X.\n", (unsigned int)pTempUmDevice->umDevMemSize);

        for (u16Index = 0; u16Index < pTempUmDevice->umDevNumInstances; ++u16Index)
        {
            /* Read Mem Base Address */
            status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umMemBaseAddr[u16Index]));
            if (status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Mem Base Address failed\n");
                return status;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Mem Base Address is 0x%X.\n", (unsigned int)pTempUmDevice->umMemBaseAddr[u16Index]);
        }

        /* read Device Component Num  */
        status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmDevice->umDevNumComponents));

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Device Component Num failed\n");
            return status;
        }

        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "\n");
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Num Components is %d.\n", (unsigned int)pTempUmDevice->umDevNumComponents);
        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "\n");

        DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Device Mask is 0x%X.\n", (unsigned int)pDeviceContext->u32DeviceMask);

        for (numComponentsIndex = 0; numComponentsIndex < pTempUmDevice->umDevNumComponents; ++numComponentsIndex)
        {
            pTempUmComponent = &pTempUmDevice->umComponentInfo[numComponentsIndex];

            /* read Device Component Type  */
            status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmComponent->umComponentType));

            if (status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Device Component Type failed\n");
                return status;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Component Type is 0x%08x\n", (unsigned int)pTempUmComponent->umComponentType);

            pTempUmComponent->umComponentType = pTempUmComponent->umComponentType & 0x0000FFFF; /* mask off delimiter for now*/

            switch (pTempUmComponent->umComponentType)
            {
                case UM_COMPONENTS_ID_CAPABILITIES:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_CAPABILITIES_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_CAPABILITIES;
                    break;
                }

                case UM_COMPONENTS_ID_BRD_GLOBAL:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_BRD_GLOBAL_STRING, strlen(TYPE_BRD_GLOBAL_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_BRD_GLOBAL;
                    break;
                }

                case UM_COMPONENTS_ID_BRD_RESETS:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_BRD_RESETS_STRING, strlen(TYPE_BRD_RESETS_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_BRD_RESETS;
                    break;
                }

                case UM_COMPONENTS_ID_BRD_MEM_CONFIG:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_BRD_SW_LOCK_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_BRD_MEM_CONFIG;
                    break;
                }

                case UM_COMPONENTS_ID_BRD_GLOBAL_VOLT_MON_X8:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_BRD_GLOBAL_VOLT_MON_X8_STRING, strlen(TYPE_BRD_GLOBAL_VOLT_MON_X8_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_BRD_GLOBAL_VOLT_MON_X8;
                    break;
                }

                /* SF components */
                case UM_COMPONENTS_ID_MIL_STD_1553_SF_GLOBAL:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_SF_GLOBAL_STRING, strlen(TYPE_1553_SF_GLOBAL_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_GLOBAL;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_SF_BCI:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_SF_BCI_STRING, strlen(TYPE_1553_SF_BCI_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_BCI;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_SF_RTX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_SF_RTX_STRING, strlen(TYPE_1553_SF_RTX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_RTX;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_SF_MTIE_STRING, strlen(TYPE_1553_SF_MTIE_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_MTIE;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_SF_IMP_STRING, strlen(TYPE_1553_SF_IMP_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_IMP;
                    break;
                }

                /* MF components */
                case UM_COMPONENTS_ID_MIL_STD_1553_MF_GLOBAL:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_GLOBAL_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_GLOBAL;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_BCI:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_BCI_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_BCI;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_RTX:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_RTX_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_RTX;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_MTIE:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_MTIE_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_MTIE;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP:  /* should not happen */
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_IMP_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_IMP;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_BC:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_IMP_BC_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_IMP;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_MRT:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_IMP_MRT_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_IMP;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_EI:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_1553_MF_EI_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_EI;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_REPLAY:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_MF_REPLAY_STRING, strlen(TYPE_1553_MF_REPLAY_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_REPLAY;
                    break;
                }

                case UM_COMPONENTS_ID_MIL_STD_1553_MF_TRIGGERS:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_1553_MF_TRIGGERS_STRING, strlen(TYPE_1553_MF_TRIGGERS_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIL_STD_1553_TRIGGERS;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_GLOBAL_TX_STRING, strlen(TYPE_ARINC_429_GLOBAL_TX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_GLOBAL_TX;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_GLOBAL_RX_STRING, strlen(TYPE_ARINC_429_GLOBAL_RX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_GLOBAL_RX;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_TX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_TX_STRING, strlen(TYPE_ARINC_429_TX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_TX;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_RX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_RX_STRING, strlen(TYPE_ARINC_429_RX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_RX;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX_MF:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_GLOBAL_TX_MF_STRING, strlen(TYPE_ARINC_429_GLOBAL_TX_MF_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_GLOBAL_TX_MF;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX_MF:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_GLOBAL_RX_MF_STRING, strlen(TYPE_ARINC_429_GLOBAL_RX_MF_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_GLOBAL_RX_MF;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_TX_MF:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_TX_MF_STRING, strlen(TYPE_ARINC_429_TX_MF_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_TX_MF;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_429_RX_MF:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_429_RX_MF_STRING, strlen(TYPE_ARINC_429_RX_MF_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_429_RX_MF;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_717_PROG_CH_GLOBAL:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_717_GLOBAL_STRING, strlen(TYPE_ARINC_717_GLOBAL_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_717_GLOBAL;
                    break;
                }

                case UM_COMPONENTS_ID_ARINC_717_PROG_CH:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_ARINC_717_RX_TX_STRING, strlen(TYPE_ARINC_717_RX_TX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_ARINC_717_PROG_CH;
                    break;
                }

                case UM_COMPONENTS_ID_IRIG_B_RX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_IRIG_B_RX_STRING, strlen(TYPE_IRIG_B_RX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_IRIG_B_RX;
                    break;
                }

                case UM_COMPONENTS_ID_IRIG_B_TX:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_IRIG_B_TX_STRING, strlen(TYPE_IRIG_B_TX_STRING));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_IRIG_B_TX;
                    break;
                }

                case UM_COMPONENTS_ID_CAN_BUS:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_CAN_BUS_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_CAN_BUS;
                    break;
                }
                case UM_COMPONENTS_ID_MIO_UART:
                {
                    sprintf((char *)pTempUmComponent->umName, "%s", TYPE_MIO_UART_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_UART;
                    break;
                }

                /* Cast UART */
                case UM_COMPONENTS_ID_MIO_CAST_UART_GLOBAL:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_MIO_CAST_UART_GLOBAL_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC;
                    break;
                }
                case UM_COMPONENTS_ID_MIO_CAST_UART_SERIAL:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_MIO_CAST_UART_SERIAL_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC;
                    break;
                }
                case UM_COMPONENTS_ID_MIO_CAST_UART_HDLC:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_MIO_CAST_UART_HDLC_SDLC_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC;
                    break;
                }
                case UM_COMPONENTS_ID_MIO_CAST_UART_SDLC:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_MIO_CAST_UART_HDLC_SDLC_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC;
                    break;
                }
                case UM_COMPONENTS_ID_MIO_CAST_UART_ASYNC:
                {
                    sprintf(pTempUmComponent->umName, "%s", TYPE_MIO_CAST_UART_HDLC_SDLC_STRING);
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC;
                    break;
                }
                /* End of Cast UART Components */

                case UM_COMPONENTS_ID_QPRM_GENERAL:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_QPRM_GENERAL, strlen(TYPE_QPRM_GENERAL));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_QPRM_GENERAL;
                    break;
                }

                case UM_COMPONENTS_ID_QPRM_DMA:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_QPRM_DMA, strlen(TYPE_QPRM_DMA));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_QPRM_DMA;
                    break;
                }

                case UM_COMPONENTS_ID_QPRM_BIST:
                {
                    strncpy((char *)pTempUmComponent->umName, TYPE_QPRM_BIST, strlen(TYPE_QPRM_BIST));
                    pDeviceContext->u32ComponentMask |= UM_COMPONENTS_MASK_QPRM_BIST;
                    break;
                }

                default:
                {
                    strncpy((char *)pTempUmComponent->umName, "ILLEGAL", strlen("ILLEGAL"));
                    break;
                }
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "\n");
            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Component Name is %s\n", pTempUmComponent->umName);

            /* read Component Rev */
            status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmComponent->umComponentRev));

            if (status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Component Rev failed\n");
                return status;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Component Rev is 0x%X.\n", (unsigned int)pTempUmComponent->umComponentRev);


            /* read Component Reg Size */
            status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmComponent->umComponentRegSize));

            if (status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Component Reg Size failed\n");
                return status;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Component Reg Size is 0x%X.\n", (unsigned int)pTempUmComponent->umComponentRegSize);

            /* read Component Reg Base Address */
            for (u16Index = 0; u16Index < pTempUmDevice->umDevNumInstances; u16Index++)
            {
                status = DDC_REG_READ(pDeviceContext, regAddr++, &(pTempUmComponent->umRegBaseAddr[u16Index]));
                if (status)

                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "umInitialize: Read Component Reg BA failed\n");
                    return status;
                }

                DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "Reg Base Address is 0x%X.\n", (unsigned int)pTempUmComponent->umRegBaseAddr[u16Index]);
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_UM, DDC_DBG_UM_INIT, "\n");

        } /* numComponentsIndex */
    } /* numDevicesIndex */

    return status;
}

/*******************************************************************************
 * Name:    ddcUdlUmCleanup
 *
 * Description:
 *      This function performs all necessary UM cleanup.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlUmCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    if (pDeviceContext->pUmInfo)
    {
        DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pUmInfo);

        pDeviceContext->pUmInfo = NULL;
    }
}
