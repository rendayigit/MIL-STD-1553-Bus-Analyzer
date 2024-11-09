/*******************************************************************************
 * FILE: ddc_udl_ioctl.c
 *
 * DESCRIPTION:
 *
 *  This file provides the driver side of the USER <=> KERNEL interface.
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
#include "os/interface/ddc_udl_os_hook.h"
#include "include/ddc_types.h"
#include "include/ddc_ioctl.h"
#include "include/ddc_device_ids.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "bus/ddc_udl_bus_private.h"
#include "bus/pci/ddc_udl_bus_pci_private.h"

#include "driver_sdk/ddc_udl_sdk.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_flash_private.h"
#include "driver_sdk/ddc_udl_interrupt_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"

#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_bc_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "core/1553/ddc_udl_1553_error_inj_private.h"
#include "core/1553/ddc_udl_1553_trigger_private.h"
#include "core/irig/ddc_udl_irigb_private.h"
#include "core/aio/ddc_udl_aio.h"
#include "core/aio/ddc_udl_aio_private.h"
#include "core/dio/ddc_udl_dio_private.h"
#include "core/arinc429/ddc_udl_arinc429_private.h"
#include "core/arinc717/ddc_udl_arinc717_private.h"
#include "core/can/ddc_udl_can.h"
#include "os/interface/ddc_udl_os_util_private.h"

/******************************************************************************
 * Name:    ddcUdlIoctl
 *
 * Description:
 *      Common IOCTL method. This function will execute the IOCTL command and
 *      return the status.
 *
 * Note:
 *      The status returned is not copied to the psIoctlDataBuffers structure.
 *      It is copied in the calling function, _ddcUdlHookIoctl. This way the
 *      IOCTL
 *
 * In   pDeviceContext          device context structure
 * In   psIoctlDataBuffers      pointer to IOCTL data buffer pointer structure
 * In   u32IoctlCmd             IOCTL command
 * In   u8DeviceNumber          Device Number
 * In   u8ChannelNumber         channel number
 * In   u8IsCompatCall          set if called from compat_ioctl
 *                                  DDC_IOCTL_CALL          0
 *                                  DDC_COMPAT_IOCTL_CALL   1
 *
 * Returns: status
 *****************************************************************************/
S16BIT ddcUdlIoctl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_DATA_BUFFERS_TYPE *psIoctlDataBuffers,
    U32BIT u32IoctlCmd,
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8IsCompatCall,
    DDC_DEV_HANDLE_TYPE handleType
)
{
    U32BIT i;
    U32BIT u32IntMask;

    void   *pvoidData = NULL;
    char   *pUserBuf = NULL;
    char   *pBuf = NULL;
    U32BIT u32Data = 0;
    U16BIT u16Data = 0;
    U32BIT u32DataLen = 0;
    size_t bytesReturned = 0;
    U32BIT u32BytesReturned = 0;
    S16BIT s16Status = DDC_UDL_ERROR__SUCCESS;

    /* pointers to DDC_IOCTL_DATA_BUFFERS_TYPE items */
    DDC_IOCTL_PARAMS *pIoctlParams;         /* ioctl param pointer*/
    DDC_IOCTL_PARAMS sIoctlParams;          /* ioctl param */
    U32BIT *pu32OutBufferBytesReturned;     /* # of bytes returned in output buffer */
    char *pInOutBuffer = NULL;                /* output or additional input buffer (optional from user) */
    U32BIT u32OutputBufferLength = 0;       /* length of output or additional input buffer */
    U32BIT *pRdData = NULL;


    /* cast the input buffer to the general purpose data type as most IOCTL commands use this structure.
       NOTE: sInBuffer may have DDC_IOCTL_PARAMS info or be a pointer to the data passed in depending a
       particular command.
    */
    pIoctlParams = &sIoctlParams;
    s16Status = DDC_COPY_FROM_USER((char *)pIoctlParams, (char *)psIoctlDataBuffers->sInBuffer.pDataBuffer, sizeof(DDC_IOCTL_PARAMS));
    if (s16Status)
    {
        return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
    }

    /* The following two user pointers are not usable directly.
       Need to call DDC_COPY_FROM_USER or DDC_COPY_TO_USER to access data */
    pInOutBuffer =  (char *)psIoctlDataBuffers->sInOutBuffer.pDataBuffer;
    u32OutputBufferLength = psIoctlDataBuffers->sInOutBuffer.u32BufferLength;

    pu32OutBufferBytesReturned = (U32BIT *)psIoctlDataBuffers->sOutBufferBytesReturned.pDataBuffer;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_CntrlRoutines,
        "IOCTL Group: %d  Type: %d\n",
        DDC_IOCTL__GROUP_GET_VAL(u32IoctlCmd),
        DDC_IOCTL__TYPE_GET_VAL(u32IoctlCmd));

    switch (u32IoctlCmd)
    {
        /* ========================================================================== */
        /* Device Group                                                               */
        /* ========================================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_GET_INFO:
        /* ---------------------------------------------------- */
        {
            /* determine the type of info requested */
            switch (pIoctlParams->Param1)
            {
                /* REGISTERS */
                case DDC_GET_INFO__REG_BASE_ADDR:
                {
                    switch (pIoctlParams->Param2)
                    {
                        case DDC_GET_INFO_ADDR__BD_GLOBAL:
                        {
                            s16Status = DDC_COPY_TO_USER(pInOutBuffer, (void *)(pDeviceContext->pBoardInfo[pIoctlParams->Channel]->pu32RegBA), sizeof(U32BIT));

                            if (s16Status)
                            {
                                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                            }
                            break;
                        }

                        default:
                        {
                            s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
                            break;
                        }
                    }

                    break;
                }

                /* MEMORY */


                /* ERROR */
                default:
                {
                    s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_DEV_INFO:
        /* ---------------------------------------------------- */
        {
            DEV_INFO *pDevInfo;
            U16BIT u16Location;
            U16BIT u16SubLocation;

            bytesReturned = sizeof(DEV_INFO);

            pDevInfo = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pDevInfo)
            {
                /* ddcUdlOsPrintUdlDynamicMemoryInfo();*/
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            memset(pDevInfo, 0, sizeof(*pDevInfo));

            pDevInfo->dwVendorID = DDC_VENDOR_ID;
            pDevInfo->dwDeviceID = pDeviceContext->u16DeviceID;

            ddcUdlBusGetLocationInfo(pDeviceContext, &u16Location, &u16SubLocation);
            pDevInfo->dwBusHub = u16Location;
            pDevInfo->dwSlotPort = u16SubLocation;

            if (pDeviceContext->u8Num1553Channels > 0)
            {
                pDevInfo->dwChanMemBA = pDeviceContext->pChannel1553[pIoctlParams->Channel]->u32UserMemBA;
                pDevInfo->dwChanMemLen = pDeviceContext->pChannel1553[pIoctlParams->Channel]->u32UserMemSizeBytes;
                pDevInfo->u32Capability = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability;
            }
            else
            {
                pDevInfo->dwChanMemBA = 0x00000000;
                pDevInfo->dwChanMemLen = 0x00000000;
                pDevInfo->u32Capability = 0x00000000;
            }

            memcpy(&(pDevInfo->sHwVersionInfo), &(pDeviceContext->sHwVersionInfo), sizeof(pDevInfo->sHwVersionInfo));

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) pDevInfo, sizeof(DEV_INFO));

            DDC_KERNEL_FREE(pDeviceContext, pDevInfo);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_ENHANCED_CAPABILITIES:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(ENHANCED_CAPABILITY_INFO);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                break;
            }

            ddcUdlBdReadEnhancedCapabilities(pDeviceContext, (PENHANCED_CAPABILITY_INFO)pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (void *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_HW_VERSION_INFO:
        /* ---------------------------------------------------- */
        {
            s16Status = DDC_COPY_TO_USER(pInOutBuffer, (void *) &(pDeviceContext->sHwVersionInfo), sizeof(HWVERSIONINFO));

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_HANDLE_INFO:
        /* ---------------------------------------------------- */
        {
            DEV_HANDLE_INFORMATION info;
            info.handleType = handleType;
            info.Channel = u8ChannelNumber;

            s16Status = DDC_COPY_TO_USER(pInOutBuffer, (void *) &(info), sizeof(info));

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_IO_COUNT:
        /* ---------------------------------------------------- */
        {
            CHANCOUNT_p pChanCount;

            bytesReturned = sizeof(CHANCOUNT_t);

            pChanCount = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pChanCount)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            pChanCount->bDiscrete = pDeviceContext->u8NumDiscreteIO;
            pChanCount->bAvionic = pDeviceContext->u8NumAvionicIO;
            pChanCount->b1553 = pDeviceContext->u8Num1553Channels;
            pChanCount->bTx = pDeviceContext->u8NumDed429Tx;
            pChanCount->bRx = pDeviceContext->u8NumDed429Rx;
            pChanCount->RS232 = pDeviceContext->u8NumRS232;
            pChanCount->RS485 = pDeviceContext->u8NumRS485;
            pChanCount->bBoardModel = (U8BIT)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum;
            pChanCount->bGroup = pChanCount->bTx;
            pChanCount->bDioTt = pDeviceContext->u8NumDioTt;

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pChanCount, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pChanCount);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_DEV_FEATURE_INFO:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(DEV_FEATURE_STATUS_INFO);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            bdGetFeatureInfo(pDeviceContext, pIoctlParams, (DEV_FEATURE_STATUS_INFO *) pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_BOARD_STATUS:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature - add features as necessary */
            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdThermalMon.u32Options & HAS_THERMAL_DETECTION_HARDWARE)
            {
                ddcUdlGetBoardStatus(pDeviceContext, pIoctlParams, (S16BIT *)pInOutBuffer);
            }
            else
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_BOARD_FEATURE:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature - add features as necessary */
            if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT)
            {
                ddcUdlSetBoardFeature(pDeviceContext, pIoctlParams, (void *)pInOutBuffer);
            }
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DEVICE_INIT:
        /* ---------------------------------------------------- */
        {
            s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DEVICE_CONFIGURE:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_DEVICE_CONFIGURE CALLED -");

            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, psIoctlDataBuffers->sInBuffer.u32BufferLength);
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)psIoctlDataBuffers->sInBuffer.pDataBuffer, psIoctlDataBuffers->sInBuffer.u32BufferLength);
            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                " case 0x%x\n", ((ACEX_CONFIG_ID *)pvoidData)->u16Type);

            /* retrieve type of configuration requested */
            switch (((ACEX_CONFIG_ID *)pvoidData)->u16Type)
            {
                /* ---------------------------------------------------- */
                case ACEX_CHAN_INIT:
                /* ---------------------------------------------------- */
                {
                    gen1553ChannelInit(pDeviceContext, (U8BIT)((ACEX_CONFIG_ID *)pvoidData)->u16Channel);
                    break;
                }

                /* ==================================================== */
                /*                   RT CONFIGURATION                   */
                /* ==================================================== */

                /* ---------------------------------------------------- */
                case ACEX_MRT_CONFIG:
                /* ---------------------------------------------------- */
                {
                    i = ((ACEX_CONFIG_ID *)pvoidData)->u16Channel;
                    s16Status = mrtOpen(pDeviceContext, (ACEX_MRT_CONFIG_TYPE *)pvoidData);

                    bytesReturned = sizeof(ACEX_MRT_HW_INFO);

                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
                    if (!pRdData)
                    {
                        s16Status = DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                        break;
                    }

                    ((ACEX_MRT_HW_INFO *)pRdData)->u32ConfigurationBA = *(pDeviceContext->pChannel1553[i]->sRT.pu32RegBA);
                    ((ACEX_MRT_HW_INFO *)pRdData)->u32GblDStkPtrBA = *(pDeviceContext->pChannel1553[i]->sRT.pu32RegBA) + REG_MRT_GBL_DATA_STK_PTR;
                    ((ACEX_MRT_HW_INFO *)pRdData)->u16RtSource = (pDeviceContext->pChannel1553[i]->sRT.u16RtSource);
                    ((ACEX_MRT_HW_INFO *)pRdData)->bMode = (pDeviceContext->pChannel1553[i]->sRT.bMode);
                    ((ACEX_MRT_HW_INFO *)pRdData)->u32MemBA = *(pDeviceContext->pChannel1553[i]->pu32MemBA);

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "ACEX_MRT_CONFIG copy to user fail\n");

                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    DDC_KERNEL_FREE(pDeviceContext, pRdData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MRT_FREE:
                /* ---------------------------------------------------- */
                {
                    s16Status = mrtClose(pDeviceContext, (U8BIT)((ACEX_CONFIG_ID *)pvoidData)->u16Channel);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MRT_CONFIG_UPDATE: /* update MRT Global Configuration Register */
                /* ---------------------------------------------------- */
                {
                    pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.sRtxCfgReg.u32RTGConfig = ((ACEX_RT_ACCESS *)pvoidData)->u32Data;

                     DDC_REG_WRITE(pDeviceContext,
                            (*(pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.pu32RegBA)) + REG_MRT_GCONFIG,
                            &(pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.sRtxCfgReg.u32RTGConfig));

                 break; }

                /* ---------------------------------------------------- */
                case ACEX_MRT_HBUF_CTRL:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                    if (!pRdData)
                    {
                        break;
                    }

                    s16Status = mrtHbufCtrl(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData), u32OutputBufferLength, &bytesReturned, (U32BIT *)pRdData);

                    if (s16Status)
                    {
                        DDC_KERNEL_FREE(pDeviceContext, pRdData);
                        break;
                    }

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MRT_CMDSTK_STATS:
                /* ---------------------------------------------------- */
                {
                    bytesReturned = sizeof(ACEX_1553_MRT_STAT_TYPE);

                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
                    if (!pRdData)
                    {
                        break;
                    }

                    memcpy((((ACEX_1553_MRT_STAT_TYPE *)pRdData)),
                        &(pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.stats),
                        sizeof(ACEX_1553_MRT_STAT_TYPE));

                    if (((ACEX_RT_ACCESS *)pvoidData)->u32Data == ACEX_MRT_RTCMD_HIGH_PCT_RESET) /* reset high % */
                    {
                        i = pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.stats.u32LastNumCmdStkEntries;
                        pDeviceContext->pChannel1553[((ACEX_CONFIG_ID *)pvoidData)->u16Channel]->sRT.stats.u32HighNumCmdStkEntries = i;
                    }

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_OPEN:
                /* ---------------------------------------------------- */
                {
                    pRdData = (U32BIT *)rtOpen(pDeviceContext, (ACEX_RT_CONFIG *)pvoidData);

                    bytesReturned = sizeof(ACEX_RT_HW_INFO);
                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_CONFIG_SET:
                /* ---------------------------------------------------- */
                {
                    s16Status = rtConfigSet(pDeviceContext, (ACEX_RT_CONFIG *)pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_CONFIG_CLR:
                /* ---------------------------------------------------- */
                {
                    s16Status = rtConfigClr(pDeviceContext, (ACEX_RT_CONFIG *)pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_BITWD_RD:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param3);
                    if (!pRdData)
                    {
                        break;
                    }

                    s16Status = rtInternalBITWdRd(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData), (U16BIT *)pRdData);

                    bytesReturned = pIoctlParams->Param3;
                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_START:
                /* ---------------------------------------------------- */
                {
                    rtStart(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData));
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_START_RESET_CMDSTK:
                /* ---------------------------------------------------- */
                {
                    mrtRtCmdStkSync(pDeviceContext, (U8BIT)((ACEX_CONFIG_ID *)pvoidData)->u16Channel);
                    rtStart(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData));
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_STOP:
                /* ---------------------------------------------------- */
                {
                    rtStop(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData));
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_CLOSE:
                /* ---------------------------------------------------- */
                {
                    rtClose(pDeviceContext, (ACEX_RT_CONFIG *)pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_LATCH_CTRL:
                /* ---------------------------------------------------- */
                {
                    rtLatchCtrl(pDeviceContext, (ACEX_RT_ACCESS *)pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_SOURCE_CTRL:
                /* ---------------------------------------------------- */
                {
                    s16Status = rtSourceCtrl(pDeviceContext, (ACEX_RT_ACCESS *)pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MRT_CMDSTK_SET_TO_LATEST:
                /* ---------------------------------------------------- */
                {
                    s16Status = mrtSetCmdStkPtrToLatest(pDeviceContext, (U8BIT)((ACEX_CONFIG_ID *)pvoidData)->u16Channel);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_STREAM_CTRL:
                /* ---------------------------------------------------- */
                {
                    mrtStreamCtrl(pDeviceContext, ((ACEX_RT_ACCESS *)pvoidData));
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MRT_GET_BRDCST_ADDRS:
                /* ---------------------------------------------------- */
                {
                    pRdData = (U32BIT *)mrtOpenBrdcst(pDeviceContext, (ACEX_CONFIG_ID *)pvoidData);

                    bytesReturned = sizeof(ACEX_RT_HW_INFO);
                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_RT_DATA_ARRAY_CTRL:
                /* ---------------------------------------------------- */
                {
                    mrtDataArrayCtrl(pDeviceContext, ((ACEX_RT_DATA_ARRAY *)pvoidData));
                    break;
                }


                /* ==================================================== */
                /*                  MT CONFIGURATION                    */
                /* ==================================================== */

                /* ---------------------------------------------------- */
                case ACEX_MT_INIT:
                /* ---------------------------------------------------- */
                {
                    s16Status = mtInitialize(pDeviceContext, (MT_MTI_CONFIG *)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "mtInitialize ERROR: s16Status=%d\n", s16Status);
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MT_CONFIG:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MT_MTI_HW_INFO));
                    if (!pRdData)
                    {
                        break;
                    }

                    s16Status = mtOpen(pDeviceContext, (MT_MTI_CONFIG *)pvoidData, (MT_MTI_HW_INFO *)pRdData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "mtOpen ERROR: s16Status=%d\n", s16Status);

                        DDC_KERNEL_FREE(pDeviceContext, pRdData);
                        break;
                    }

                    bytesReturned = sizeof(MT_MTI_HW_INFO);
                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_MT_FREE:
                /* ---------------------------------------------------- */
                {
                    s16Status = mtClose(pDeviceContext, ((ACEX_CONFIG_ID *)pvoidData)->u16Channel);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "mtClose ERROR: s16Status=%d\n", s16Status);

                        break;
                    }

                    break;
                }


                /* ==================================================== */
                /*                  BC CONFIGURATION                    */
                /* ==================================================== */

                /* ---------------------------------------------------- */
                case ACEX_BC_INIT:
                /* ---------------------------------------------------- */
                {
                    /* BC initialize */
                    s16Status = bcOpen(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_CONFIG:
                /* ---------------------------------------------------- */
                {
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_FREE:
                /* ---------------------------------------------------- */
                {
                    /* BC free */
                    s16Status = bcClose(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_HBUF_INSTALL:
                /* ---------------------------------------------------- */
                {
                    /* BC HBuf install */
                    s16Status = bcHbufEnable(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_HBUF_UNINSTALL:
                /* ---------------------------------------------------- */
                {
                    /* BC HBuf uninstall */
                    s16Status = bcHbufDisable(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_GPQ_INIT:
                /* ---------------------------------------------------- */
                {
                    /* BC General Purpose Queue init */
                    s16Status = bcGpqInit(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_LPQ_INIT:
                /* ---------------------------------------------------- */
                {
                    /* BC Low Priority Queue init */
                    s16Status = bcLpqInit(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_HPQ_INIT:
                /* ---------------------------------------------------- */
                {
                    /* BC High Priority Queue init */
                    s16Status = bcHpqInit(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_REPLAY_INIT:
                /* ---------------------------------------------------- */
                {
                    /* initialize replay interrupt s16Status queue index */
                    DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond,
                                      pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                    /* Flag replay interrupts as enabled */
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.bReplayIsrEnabled = TRUE;

                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQHead = 0;
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail = 0;
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond = 0;

                    DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond,
                                      pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                    /* Replay initialize */
                    s16Status = bcReplayInit(pDeviceContext, pvoidData);
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_BC_MUX_INIT:
                /* ---------------------------------------------------- */
                {
                    /* Channel Mux init */
                    s16Status = bcMuxInit(pDeviceContext, pvoidData);
                    break;
                }


                /* ==================================================== */
                /*                  ARINC CONFIGURATION                 */
                /* ==================================================== */

                /* ---------------------------------------------------- */
                case ACEX_ARINC_429_PROGRAMABLE:
                /* ---------------------------------------------------- */
                {
                    s16Status = ARINC429ProgrammableConfig(pDeviceContext, (PARINC_429_PROGRMMABLE_CONFIG)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "aring429ProgrammableConfig ERROR: s16Status=%d\n", s16Status);
                    }
                    else
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "aring429ProgrammableConfig Passed\n");
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_ARINC_717_CONFIG:
                /* ---------------------------------------------------- */
                {
                    s16Status = arinc717ProgrammableOpen(pDeviceContext, (PARINC_717_PROGRMMABLE_CONFIG)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "arinc717ProgrammableOpen failed \n");
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_ARINC_717_FREE:
                /* ---------------------------------------------------- */
                {
                    s16Status = arinc717ProgrammableClose(pDeviceContext, (PARINC_717_PROGRMMABLE_CONFIG)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "arinc717ProgrammableClose failed \n");
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_ARINC_717_STATE:
                /* ---------------------------------------------------- */
                {
                    s16Status = arinc717ProgrammableSetState(pDeviceContext, (PARINC_717_PROGRMMABLE_CONFIG)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "arinc717ProgrammableSetState failed \n");
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_ARINC_717_INTERRUPTS:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                    if (!pRdData)
                    {
                        s16Status = DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                        break;
                    }

                    s16Status = arinc717Interrupts(pDeviceContext, (PARINC_717_PROGRMMABLE_CONFIG)pvoidData, (U32BIT *)pRdData, FALSE);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "arinc717ProgrammableIntSet failed \n");

                        DDC_KERNEL_FREE(pDeviceContext, pRdData);
                        break;
                    }

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, u32OutputBufferLength);

                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_CAN_BUS_CONFIG:
                /* ---------------------------------------------------- */
                {
                    s16Status = canBusOpen(pDeviceContext, (CAN_BUS_CONFIG *)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "DdcSfpUsbEvtIoDeviceControl: canBusOpen failed \n");
                    }

                    bytesReturned = 0;
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_CAN_BUS_FREE:
                /* ---------------------------------------------------- */
                {
                    s16Status = canBusClose(pDeviceContext, (CAN_BUS_CONFIG *)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "DdcSfpUsbEvtIoDeviceControl: canBusClose failed \n");
                    }

                    bytesReturned = 0;
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_CAN_BUS_STATE:
                /* ---------------------------------------------------- */
                {
                    s16Status = canBusSetState(pDeviceContext, (CAN_BUS_CONFIG *)pvoidData);

                    if (s16Status)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                            "DdcSfpUsbEvtIoDeviceControl: canBusSetState failed \n");
                    }

                    bytesReturned = 0;
                    break;
                }

                /* ---------------------------------------------------- */
                case ACEX_CAN_FIRWARE_VERSION:
                /* ---------------------------------------------------- */
                {
                    DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
                        pDeviceContext->sCanBus.u32FirmwareVersionAddress,
                        &pDeviceContext->sCanBus.u32FirmwareVersion,
                        u32OutputBufferLength,
                        DDC_USB_ACC_TYPE_WAIT);

                    /*if (s16Status != DDC_UDL_ERROR__SUCCESS)
                    {
                        break;
                    }*/

                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "DdcSfpUsbEvtIoDeviceControl: canBusSetState fw VERSION %x u32FirmwareVersionAddress %x s16Status %x \n",
                        pDeviceContext->sCanBus.u32FirmwareVersion,
                        pDeviceContext->sCanBus.u32FirmwareVersionAddress,
                        s16Status);

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer),
                        (char *)&pDeviceContext->sCanBus.u32FirmwareVersion, u32OutputBufferLength);

                    break;
                }

                /* ==================================================== */
                /*              INVALID DEVICE REQUEST                  */
                /* ==================================================== */

                /* ------------------------------------------------------------ */
                default:
                /* ------------------------------------------------------------ */
                {
                    s16Status = DDC_UDL_ERROR__INVALID_DEVICE_REQUEST;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "Invalid IOCTL_DDCSFP_1553_CONFIGURE Request: %d\n", ((ACEX_CONFIG_ID *)pvoidData)->u16Type);

                    break;
                }
            }

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BOARD_COMPONENT_RESET:
        /* ---------------------------------------------------- */
        {
            ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, DDC_IOCTL_U32(pIoctlParams->Param1));
            break;
        }

        /* ==================================================== */
        /*                   REG  REG  REG  REG                 */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_REG_READ32:
        case IOCTL_REG_READ:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_REG_READ\n");

            s16Status = DDC_REG_READ(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1), &u32Data);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_REG_READ ERROR: s16Status=%d\n", s16Status);

                return s16Status;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_REG_WRITE32:
        case IOCTL_REG_WRITE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Temp = (U32BIT)pIoctlParams->Param2;

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_REG_WRITE\n");

            s16Status = DDC_REG_WRITE(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1), (U32BIT *) &(u32Temp));
            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_REG_READ_BLK:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Address;

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            /* convert relative to absolute address for every case except for default */
            switch (pIoctlParams->Param2)
            {
                /* ---------------------------------------------------- */
                case REG_TYPE_BD_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_BC_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sBC.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sBC.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_MRT_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sRT.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sRT.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_MT_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_1553_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_EI_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sErrorInj.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sErrorInj.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_REPLAY_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sReplay.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sReplay.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_TRG_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sTrigger.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sTrigger.pu32RegBA) + pIoctlParams->Param1);
                    break;
                }

                /* ---------------------------------------------------- */
                default:
                /* ---------------------------------------------------- */
                {
                    /* absolute address passed */
                    u32Address =  DDC_IOCTL_U32(pIoctlParams->Param1);
                    break;
                }
            }

            if (s16Status)
            {
                break;
            }

            s16Status = DDC_BLK_REG_READ(pDeviceContext, u32Address, pRdData, (u32OutputBufferLength / 4));

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_REG_READ_BLK Addr: 0x%08x, Val=0x%08x length: 0x%08x \n", u32Address, *pRdData, u32OutputBufferLength);


            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_REG_READ_BLK ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return s16Status;
            }

            bytesReturned = u32OutputBufferLength;
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_REG_WRITE_BLK:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Address;

            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param3); /* pIoctlParams->Param3 is number of bytes to write */
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), pIoctlParams->Param3);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            /* convert relative to absolute address for every case except for default */
            switch (pIoctlParams->Param2)
            {
                /* ---------------------------------------------------- */
                case REG_TYPE_BD_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_BC_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sBC.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sBC.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_MRT_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sRT.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sRT.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_MT_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_1553_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_EI_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sErrorInj.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sErrorInj.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_REPLAY_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sReplay.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sReplay.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                case REG_TYPE_TRG_OFFSET:
                /* ---------------------------------------------------- */
                {
                    if (!pDeviceContext->pChannel1553[pIoctlParams->Channel]->sTrigger.pu32RegBA)
                    {
                        s16Status = DDC_UDL_ERROR__NULL_PTR;
                        u32Address = 0;
                        break;
                    }

                    u32Address = (*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sTrigger.pu32RegBA) +  DDC_IOCTL_U32(pIoctlParams->Param1));
                    break;
                }

                /* ---------------------------------------------------- */
                default:
                /* ---------------------------------------------------- */
                {
                    /* absolute address passed */
                    u32Address =  DDC_IOCTL_U32(pIoctlParams->Param1);
                    break;
                }
            }

            if (s16Status)
            {
                break;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_REG_WRITE_BLK addr 0x%08x Val 0x%08x length: 0x%08x\n", u32Address, *(U32BIT *)pvoidData, pIoctlParams->Param3);

            s16Status = DDC_BLK_REG_WRITE(pDeviceContext, u32Address, pvoidData,  DDC_IOCTL_U32(pIoctlParams->Param3 / 4));

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_REG_WRITE_BLK ERROR: s16Status=%d\n", s16Status);
            }

            break;
        }


        /* ==================================================== */
        /*                   MEM  MEM  MEM  MEM                 */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_MEM_READ:
        /* ---------------------------------------------------- */
        {
            s16Status = DDC_MEM_READ(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), &u32Data, ACEX_32_BIT_ACCESS);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_MEM_READ ERROR: s16Status=%d\n", s16Status);

                return s16Status;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MEM_WRITE:
        /* ---------------------------------------------------- */
        {
            s16Status = DDC_MEM_WRITE(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), (U32BIT *) &(pIoctlParams->Param2), ACEX_32_BIT_ACCESS);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MEM_READ_BLK:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            if (pIoctlParams->Param2 == IOCTL_16_BIT_ACCESS)
            {
                /* 16-bit read */

                s16Status = DDC_16BIT_BLK_MEM_READ(pDeviceContext,
                    DDC_IOCTL_U32(pIoctlParams->Param1), /* in 16-bit */
                    (U16BIT *)pRdData,
                    (u32OutputBufferLength / 2)); /* bytes to 16-bit */

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_MEM_READ_BLK 16 BIT: Addr=0x%08x Data=0x%04x Cnt=0x%08x\n",
                    (unsigned int)pIoctlParams->Param1, (U16BIT)(*pRdData), u32OutputBufferLength / 2);
            }
            else
            {
                /* 32-bit read */

                s16Status = DDC_BLK_MEM_READ(pDeviceContext,
                    DDC_IOCTL_U32(pIoctlParams->Param1), /* in 32-bit */
                    pRdData,
                    (u32OutputBufferLength / 4),
                    DDC_IOCTL_U32(pIoctlParams->Param2)); /* bytes to 32-bit */

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_MEM_READ_BLK 32 BIT: Addr=0x%08x Data=0x%08x Cnt=0x%08x \n",
                    (unsigned int)pIoctlParams->Param1, (unsigned int)(*pRdData), u32OutputBufferLength / 4);
            }

            bytesReturned = u32OutputBufferLength;

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MEM_WRITE_BLK:
        /* ---------------------------------------------------- */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), u32OutputBufferLength);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            if (pIoctlParams->Param2 == IOCTL_16_BIT_ACCESS)
            {
                /* 16-bit write */

                s16Status = DDC_16BIT_BLK_MEM_WRITE(pDeviceContext,
                    DDC_IOCTL_U32(pIoctlParams->Param1), /* in 16-bit */
                    (U16BIT *)pvoidData,
                    (u32OutputBufferLength / 2)); /* bytes to 16-bit */

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_MEM_WRITE_BLK 16 BIT: Addr=0x%08x Data=0x%04x Cnt=0x%08x\n",
                    (unsigned int)pIoctlParams->Param1, *(U16BIT *)pvoidData, u32OutputBufferLength / 2);
            }
            else
            {
                /* 32-bit write */

                s16Status = DDC_BLK_MEM_WRITE(pDeviceContext,
                    DDC_IOCTL_U32(pIoctlParams->Param1), /* in 32-bit */
                    pvoidData,
                    (u32OutputBufferLength / 4),
                     DDC_IOCTL_U32(pIoctlParams->Param2)); /* bytes to 32-bit */

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_MEM_WRITE_BLK 32 BIT: Addr=0x%08x Data=0x%08x Cnt=0x%08x\n",
                    (unsigned int)pIoctlParams->Param1, *(unsigned int *)pvoidData, u32OutputBufferLength / 4);
            }

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BLK_READ:
            /* cmd:   BLK_REG_READ
              desc:  return dump of 32-bit registers starting at register offset identified in Param1.
              notes: it is assumed buffer length is on 32bit boundary..if not data may be truncated */
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BLK_READ ERROR: DDC_UDL_ERROR__INSUFFICIENT_MEMORY\n");

                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            if (pIoctlParams->Param2 == DDC_REGISTER_ACCESS)
            {
                s16Status = DDC_BLK_REG_READ(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), pRdData, (u32OutputBufferLength / 4));
            }
            else    /* assume memory access */
            {
                s16Status = DDC_BLK_MEM_READ(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), pRdData, (u32OutputBufferLength / 4), ACEX_32_BIT_ACCESS);
            }

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BLK_READ ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return s16Status;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, u32OutputBufferLength);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BLK_WRITE:
        /* ---------------------------------------------------- */
        /* cmd:   BLK_WRITE                   desc:            */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param3);
            if (!pvoidData)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BLK_WRITE ERROR: DDC_UDL_ERROR__INSUFFICIENT_MEMORY\n");

                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), pIoctlParams->Param3);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            if (pIoctlParams->Param2 == DDC_REGISTER_ACCESS)
            {
                s16Status = DDC_BLK_REG_WRITE(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1), pvoidData, DDC_IOCTL_U32(pIoctlParams->Param3 / 4));
            }
            else
            {
                s16Status = DDC_BLK_MEM_WRITE(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), pvoidData,  DDC_IOCTL_U32(pIoctlParams->Param3 / 4), ACEX_32_BIT_ACCESS);
            }

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BLK_WRITE ERROR: s16Status=%d\n", s16Status);
            }

            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_CLEAR_MEMORY:
        /* ---------------------------------------------------- */
        {
            s16Status = ddcUdlBdClearMemory(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), pIoctlParams->Param2);
            break;
        }


        /* ==================================================== */
        /*           FLASH   FLASH   FLASH   FLASH   FLASH      */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_FLASH_MEM_INFO:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(FLASH_INFO);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            /* return Start Address, Sector Count & Min Sector Size */
            ((FLASH_INFO *)pRdData)->StartAddress = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashStartAddr;
            ((FLASH_INFO *)pRdData)->SectorCount = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashNumClusters;
            ((FLASH_INFO *)pRdData)->MinSectorSize = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashClusterSize;

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_FLASH_MEM_WRITE_PROTECTED:
        /* ---------------------------------------------------- */
        {
            u32Data = flashMemWriteProtected(pDeviceContext);

            bytesReturned = sizeof(U32BIT);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, bytesReturned);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_FLASH_MEM_BLOCK_ERASE:
        /* ---------------------------------------------------- */
        {
            s16Status = flashMemBlkErase(pDeviceContext, pIoctlParams);

            if (s16Status)
            {
                return s16Status;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_FLASH_MEM_READ:
        /* ---------------------------------------------------- */
        {
            /* allocate pRdData large enough to contain data*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param2);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = flashMemRead(pDeviceContext, pIoctlParams, pRdData);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__READ;
            }

            bytesReturned = pIoctlParams->Param2;

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_FLASH_MEM_WRITE:
        /* ---------------------------------------------------- */
        {
            /* allocate pvoidData large enough to contain data*/
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param2);
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER(pvoidData, (char *)(pInOutBuffer), pIoctlParams->Param2);

            if (s16Status)
            {
                /* free malloc'd buffer*/
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            s16Status = flashMemWrite(pDeviceContext, pIoctlParams, pvoidData);

            /* free malloc'd buffer*/
            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            if (s16Status)
            {
                return s16Status;
            }

            break;
        }

        /* ==================================================== */
        /*               IRIG  IRIG  IRIG  IRIG  IRIG           */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_IRIG_GET_TX:
        /* ---------------------------------------------------- */
        {
            if (pDeviceContext->sHwVersionInfo.dwCapabilities &  HWVER_CAPABILITY_IRIG_OUT_DIGITAL)
            {
				bytesReturned = sizeof(IRIG_TX);

                pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
                if (!pRdData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                irigbGetIRIGTx(pDeviceContext, (IRIG_TX *)pRdData);

                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }
            else
            {
                bytesReturned = 0;
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_IRIG_SET_TX:
        /* ---------------------------------------------------- */
        {
            if (pDeviceContext->sHwVersionInfo.dwCapabilities &  HWVER_CAPABILITY_IRIG_OUT_DIGITAL)
            {
                bytesReturned = sizeof(IRIG_TX);

                pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
                if (!pvoidData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(psIoctlDataBuffers->sInBuffer.pDataBuffer), bytesReturned);

                if (s16Status)
                {
                    DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                    return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
                }

                irigbSetIRIGTx(pDeviceContext, (IRIG_TX *)pvoidData);

                DDC_KERNEL_FREE(pDeviceContext, pvoidData);

                bytesReturned = 0;
                s16Status = DDC_UDL_ERROR__SUCCESS;
            }
            else
            {
                bytesReturned = 0;
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_IRIG_SET_ID:
        /* ---------------------------------------------------- */
        {
            pDeviceContext->u16IrigPacketChannelId[pIoctlParams->Channel] = (U16BIT)pIoctlParams->Param1;
            bytesReturned = 0;
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_IRIG_SET_INT_STATE:
        /* ---------------------------------------------------- */
        {
            s16Status = irigInterruptSet(pDeviceContext, pIoctlParams);
            break;
        }


        /* ==================================================== */
        /*                       TIME TAG                       */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_SET_TT_RESOLUTION:
        /* ---------------------------------------------------- */
        {
            s16Status = gen1553SetTimeTagResolution(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_TT_ROLLOVER_POINT:
        /* ---------------------------------------------------- */
        {
            s16Status = gen1553SetTimeTagRolloverPoint(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_TT_INTERRUPT:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_SET_TT_INTERRUPT\n");

            u32IntMask = 1; /* Enable Time Tag Rollover Interrupt */

            if (pIoctlParams->Param1 == 1)
            {
                s16Status = gen1553InterruptSet(pDeviceContext, (U8BIT)pIoctlParams->Channel, u32IntMask);
            }
            else
            {
                s16Status = gen1553InterruptClear(pDeviceContext, (U8BIT)pIoctlParams->Channel, u32IntMask);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_EXT_TT_CNT_CTRL:
        /* ---------------------------------------------------- */
        {
            s16Status = gen1553ExtTTCntCtrl(pDeviceContext, pIoctlParams);
            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_DIO_TT_CFG:
        /* ---------------------------------------------------- */
        {
            s16Status = ddcUdlDioTtCfg(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DIO_TT_CTL:
        /* ---------------------------------------------------- */
        {
            s16Status = ddcUdlDioTtCtl(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DIO_TT_READ:
        /* ---------------------------------------------------- */
        {
            s16Status = ddcUdlDioTtRead(pDeviceContext, pRdData, u32OutputBufferLength);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DIO_TT_BLOCK_ON_IRQ:
        /* ---------------------------------------------------- */
        {
            /* See if interrupts are enabled */
            if (pDeviceContext->sDioTt.bIsrEnabled == FALSE)
            {
                /* Return error indicating interrupts are not enabled */
                s16Status = DDC_UDL_ERROR__ISR_NOT_ENABLED;

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_DIO_TT_BLOCK_ON_IRQ EXIT 1 <==== \n");

                break;
            }

            DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

            /* decrement the event count */
            if (pDeviceContext->sDioTt.u32IntQLen > 0)
            {
                pDeviceContext->sDioTt.u32IntQLen--;
            }

            /* wait if there is no IRQ s16Status in the queue */
            if (pDeviceContext->sDioTt.u32IntQLen == 0)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

                DDC_WAIT_INTERRUPTABLE(
                    pDeviceContext->sDioTt.waitqueueIrqEvent,
                    pDeviceContext->sDioTt.u32IntQLen);

                DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);
            }

            if (pDeviceContext->sDioTt.u32IntQLen)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

                s16Status = DDC_COPY_TO_USER(
                    (char*)(pInOutBuffer),
                    &pDeviceContext->sDioTt.sIntStatus[pDeviceContext->sDioTt.u32IntQTail],
                    sizeof(pDeviceContext->sDioTt.sIntStatus[0])); /* just use the size of the first item */

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }

                /* clear out the s16Status */
                pDeviceContext->sDioTt.sIntStatus[pDeviceContext->sDioTt.u32IntQTail] = 0x00000000;

                DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

                pDeviceContext->sDioTt.u32IntQTail++;

                if (pDeviceContext->sDioTt.u32IntQTail >= (DIOTT_IRQ_STATUS_QUEUE_SIZE - 1))
                {
                    pDeviceContext->sDioTt.u32IntQTail = 0;
                }
            }

            DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DIO_TT_RELEASE_IRQ:
        /* ---------------------------------------------------- */
        {   /* Cancel pending DIO TT interrupt request */

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_DIO_TT_RELEASE_IRQ: Wake up DIO TT Ch %d IRQ\n", u8ChannelNumber);

            /* Flag DIO TT interrupt processing as disabled */
            pDeviceContext->sDioTt.bIsrEnabled = FALSE;

            /* Wake up the blocked replay thread so it can terminate */
            DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);
            pDeviceContext->sDioTt.u32IntQLen = 1;

            /* initialize interrupt s16Status queue index */
            pDeviceContext->sDioTt.u32IntQHead = 0;
            pDeviceContext->sDioTt.u32IntQTail = 0;

            DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

            DDC_WAKE_UP(&pDeviceContext->sDioTt.waitqueueIrqEvent);

            break;
        }


        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_WAIT:
        /* ---------------------------------------------------- */
        {
            ddcUdlOsLibWaitMs((S32BIT)pIoctlParams->Param1);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_DDC_DEBUG:
        /* ---------------------------------------------------- */
        {
            switch (pIoctlParams->Param1)
            {
                case DDC_DEBUG_SET_TRACE_LEVEL:
                {
            /*
                        Param2  Module
                        Param3  Mask
                        Param4  Operation: 0=clear  1=set
            */

                    if (pIoctlParams->Param4 == 1)
            {
                        ddcUdlDebugSetTraceLevel(DDC_IOCTL_U32(pIoctlParams->Param2), DDC_IOCTL_U32(pIoctlParams->Param3));
            }
            else
            {
                        ddcUdlDebugClearTraceLevel(DDC_IOCTL_U32(pIoctlParams->Param2), DDC_IOCTL_U32(pIoctlParams->Param3));
                    }
                    break;
                }

                case DDC_DEBUG_PRINT_UDL_MEM_INFO:
                {
                    /*ddcUdlOsPrintUdlDynamicMemoryInfo();*/
                    break;
                }

                case DDC_DEBUG_PRINT_DMA_MEM_INFO:
                {
                    /*ddcUdlOsPrintDmaDynamicMemoryInfo();*/
                    break;
                }
            }
            break;
        }


        /* ========================================================================== */
        /* I/O & Serial Group                                                         */
        /* ========================================================================== */

        /* ==================================================== */
        /*                AIO  AIO  AIO  AIO  AIO               */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_GET_AIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            u32Data = GetAioOutput(pDeviceContext, pIoctlParams);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_AIO_DIRECTION:
        /* ---------------------------------------------------- */
        {
            u32Data = GetAioDirection(pDeviceContext, pIoctlParams);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_AIO_INPUT:
        /* ---------------------------------------------------- */
        {
            u32Data = GetAioInput(pDeviceContext, pIoctlParams);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_AIO_ALL:
        /* ---------------------------------------------------- */
        {
            u32Data = GetAioAll(pDeviceContext);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_AIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            SetAioOutput(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_AIO_DIRECTION:
        /* ---------------------------------------------------- */
        {
            SetAioDirection(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_AIO_ALL:
        /* ---------------------------------------------------- */
        {
            SetAioAll(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1));
            break;
        }


        /* ==================================================== */
        /*                DIO  DIO  DIO  DIO  DIO               */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_GET_DIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            u32Data = GetDioOutput(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1));
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            bytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_DIO_DIRECTION:
        /* ---------------------------------------------------- */
        {
            u32Data = GetDioDirection(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1));
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            bytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_DIO_INPUT:
        /* ---------------------------------------------------- */
        {
            u32Data = GetDioInput(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1));
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            bytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_GET_DIO_ALL:
        /* ---------------------------------------------------- */
        {
            u32Data = GetDioAll(pDeviceContext);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, sizeof(U32BIT));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            bytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_DIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            SetDioOutput(pDeviceContext,
                DDC_IOCTL_U32(pIoctlParams->Param1), /* discrete */
                DDC_IOCTL_U32(pIoctlParams->Param2)); /* level */

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_DIO_DIRECTION:
        /* ---------------------------------------------------- */
        {
            SetDioDirection(pDeviceContext,
                DDC_IOCTL_U32(pIoctlParams->Param1), /* discrete */
                DDC_IOCTL_U32(pIoctlParams->Param2)); /* level */

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_SET_DIO_ALL:
        /* ---------------------------------------------------- */
        {
            SetDioAll(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1));
            break;
        }

        /* ==================================================== */
        /*                CAN  CAN  CAN  CAN  CAN               */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_CAN_BUS_TX_DATA:
        /* ---------------------------------------------------- */
        {
            /* Load CAN Bus transmit data */

            /* allocate pvoidData large enough to contain data*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER(pRdData, (char *)(pInOutBuffer), u32OutputBufferLength);

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_CAN_BUS_TX_DATA s16Status=0x%08x\n", s16Status);

            if (s16Status)
            {
                /* free malloc'd buffer */
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            s16Status = canBusTransmitData(pDeviceContext, pIoctlParams, pRdData);

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_CAN_BUS_TX_DATA status2 =0x%08x\n", s16Status);

            /* free malloc'd buffer */
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_CAN_BUS_RX_DATA:
        /* ---------------------------------------------------- */
        {
            /* Retrieve CAN Bus data */

            /* allocate pRdData large enough to contain data*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            /* s16Status = canBusReadData(pDeviceContext, pIoctlParams, pRdData,(size_t *)&u32OutputBufferLength); */
            s16Status = (S16BIT)canBusReadData(pDeviceContext, pIoctlParams, pRdData, &bytesReturned);
            if (s16Status != DDC_UDL_ERROR__SUCCESS)
            {
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return s16Status;
            }

            /* If no data returned, must ensure 1st 2 DWORDS are copied.
             * 1st DWORD is message control & 2nd is # of messages */
            if (bytesReturned == 0)
            {
                bytesReturned = 8;
            }

            s16Status = (U16BIT)DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);
            break;
        }

        /* ========================================================================== */
        /* 1553 Group                                                                 */
        /* ========================================================================== */

        /* ==================================================== */
        /*              1553 IRQ  1553 IRQ  1553 IRQ            */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_CONFIGURE_INTERRUPT_CONDITIONS:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS *pIoConfigureOutput;

            /* allocate pRdData large enough to contain data*/
            pIoConfigureOutput = (DDC_IOCTL_PARAMS *)DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);

            if (!pIoConfigureOutput)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = ddcUdlBdConfigureIoInterruptConditions(
                pDeviceContext,
                (U16BIT)pIoctlParams->Channel,  /* 1553 Channel */
                pIoctlParams->Param1,   /* Command */
                pIoctlParams->Param2,   /* Rising Edge Enable/Disable */
                pIoctlParams->Param3,   /* Falling Edge Enable/Disable */
                pIoctlParams->Param4,   /* Reserved */
                pIoConfigureOutput
            );

            if (pIoctlParams->Param1 == ACEX_AIO_INTERRUPT_CONFIGURE__GET)
            {
                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pIoConfigureOutput, u32OutputBufferLength);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_ENABLE_IRQ:
        /* ---------------------------------------------------- */
        {
            /* initialize interrupt s16Status queue index */
            DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCondFlag);
            pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQHead = 0;
            pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQTail = 0;
            pDeviceContext->pChannel1553[u8ChannelNumber]->u16IrqEventCond = 0;
            pDeviceContext->pChannel1553[u8ChannelNumber]->u16BlockOnIrqReadyEventCond = 0;
            DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCondFlag);

            s16Status = irqEnableInterrupt(pDeviceContext, u8ChannelNumber, u8DeviceNumber);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_DISABLE_IRQ:
        /* ---------------------------------------------------- */
        {
            /* Cancel pending 1553 interrupt request */
            s16Status = irqDisableInterrupt(pDeviceContext, u8ChannelNumber, u8DeviceNumber);

            /* initialize interrupt s16Status queue index */
            DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCondFlag);
            pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQHead = 0;
            pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQTail = 0;
            DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCondFlag);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_BLOCK_ON_IRQ:
        /* ---------------------------------------------------- */
        {
            struct _ACEX_1553_CHANNEL_TYPE *p1553Ch = pDeviceContext->pChannel1553[u8ChannelNumber];

            /* See if interrupts are enabled */
            DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

            if (p1553Ch->bIsr1553Enabled == FALSE)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_1553_BLOCK_ON_IRQ failed: interrupts not enabled\n");

                /* Return error indicating interrupts are not enabled */
                s16Status = DDC_UDL_ERROR__ISR_NOT_ENABLED;
                break;
            }

            DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

            /* Sleep and wait for the local ISR or ISR clean-up to wake us up and proceed */
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "Wait for IRQ on channel %d\n", u8ChannelNumber);

            /* decrement the event count */
            DDC_ISR_LOCK_TAKE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);

            if (p1553Ch->u16IrqEventCond > 0)
            {
                p1553Ch->u16IrqEventCond--;
            }

            if (p1553Ch->u16BlockOnIrqReadyEventCond == 0)
            {
                /* let the library know that the IOCTL_1553_BLOCK_ON_IRQ call is about to block */
                p1553Ch->u16BlockOnIrqReadyEventCond++;
                DDC_ISR_LOCK_GIVE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);

                DDC_WAKE_UP(&p1553Ch->waitqueueBlockOnIrqReadyEvent);

                DDC_ISR_LOCK_TAKE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);
            }

            /* wait if there is no IRQ s16Status in the queue */
            if (p1553Ch->u16IrqEventCond == 0)
            {
                DDC_ISR_LOCK_GIVE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);

                DDC_WAIT_INTERRUPTABLE(p1553Ch->waitqueueIrqEvent, p1553Ch->u16IrqEventCond);
            }
            else
            {
                DDC_ISR_LOCK_GIVE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);
            }

            s16Status = DDC_COPY_TO_USER((char*)(pInOutBuffer),
                &p1553Ch->pu32IrqStatusQ[p1553Ch->u32IrqStatusQTail],
                sizeof(p1553Ch->pu32IrqStatusQ[0]));

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            /* point to the next entry of the IRQ s16Status queue */
            DDC_ISR_LOCK_TAKE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);
            if (p1553Ch->u16IrqEventCond)
            {
                p1553Ch->u32IrqStatusQTail++;

                if (p1553Ch->u32IrqStatusQTail == (IRQ_STATUS_QUEUE_SIZE - 1))
                {
                    p1553Ch->u32IrqStatusQTail = 0;
                }
            }
            DDC_ISR_LOCK_GIVE(p1553Ch->semIrqEventCond, p1553Ch->semIrqEventCondFlag);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_BLOCK_ON_IRQ_READY:
        /* ---------------------------------------------------- */
        {
            int nResult = 0;

            nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                pDeviceContext->pChannel1553[u8ChannelNumber]->waitqueueBlockOnIrqReadyEvent,
                pDeviceContext->pChannel1553[u8ChannelNumber]->u16BlockOnIrqReadyEventCond,
                500);

            if (nResult == 0)
            {
               DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_CntrlRoutines, "DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS - Timed Out!\n");

                /* timeout error */
                s16Status = DDC_UDL_ERROR__TIMEOUT;
                break;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_MODULE_INTERRUPT_STATE:
        /* ---------------------------------------------------- */
        {
            if (pIoctlParams->Param1)
            {
                gen1553InterruptSet(pDeviceContext, (U8BIT)pIoctlParams->Channel, DDC_IOCTL_U32(pIoctlParams->Param2));
            }
            else
            {
                gen1553InterruptClear(pDeviceContext, (U8BIT)pIoctlParams->Channel, DDC_IOCTL_U32(pIoctlParams->Param2));
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_IRQ_CONDITIONS:
        /* ---------------------------------------------------- */
        {
            gen1553SetIRQ(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1), (U16BIT)pIoctlParams->Param2, (U8BIT)pIoctlParams->Channel);
            break;
        }


        /* ==================================================== */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_RAM_PARITY_CHECKING:
        /* ---------------------------------------------------- */
        {
            s16Status = gen1553SetRamParityChecking(pDeviceContext, pIoctlParams);
            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_1553_IMP_READ:
        /* ---------------------------------------------------- */
        {
            /* general IMPROVEMENTS Read Control */

            switch (pIoctlParams->Param1)    /* IMPROVEMENT COMMANDS */
            {
                /* ---------------------------------------------------- */
                case IOCTL_IMP_MRT_CMD:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                    if (!pRdData)
                    {
                        break;
                    }

                    s16Status = mrtImpReadCmd(pDeviceContext,
                                            (U16BIT)pIoctlParams->Channel,
                                            DDC_IOCTL_U32(pIoctlParams->Param2 >> 16) & 0x0000ffff,
                                            u32OutputBufferLength,
                                            &bytesReturned,
                                            pRdData);

                    if (s16Status)
                    {
                        bytesReturned = 0;
                        DDC_KERNEL_FREE(pDeviceContext, pRdData);
                        break;
                    }

                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                        "bytesReturned: %d\n", (int)bytesReturned);

                    if (bytesReturned > 0)
                    {
                        s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    }

                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        bytesReturned = 0;
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case IOCTL_IMP_MRT_DATA:
                /* ---------------------------------------------------- */
                {
                    pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                    if (!pRdData)
                    {
                        break;
                    }

                    s16Status = mrtImpReadData(pDeviceContext,
                        (U16BIT)pIoctlParams->Channel,
                        (U16BIT)((pIoctlParams->Param2 >> 16) & 0x0000ffff),
                        (U16BIT)(pIoctlParams->Param2 & 0x0000ffff),
                        u32OutputBufferLength,
                        &bytesReturned,
                        pRdData);

                    if (s16Status)
                    {
                        DDC_KERNEL_FREE(pDeviceContext, pRdData);
                        break;
                    }

                    if (bytesReturned > 0)
                    {
                        s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                    }

                    DDC_KERNEL_FREE(pDeviceContext, pRdData);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    break;
                }

                /* ---------------------------------------------------- */
                case IOCTL_IMP_BC_CMD:
                /* ---------------------------------------------------- */
                {
                    break;
                }

                /* ---------------------------------------------------- */
                case IOCTL_IMP_BC_DATA:
                /* ---------------------------------------------------- */
                {
                    break;
                }

                /* ---------------------------------------------------- */
                default:
                /* ---------------------------------------------------- */
                {
                    break;
                }

            }    /* switch on pIoctlParams->Param1 */

            break;

        } /* IOCTL_IMP_READ */

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_RESP_TIME_OUT:
        /* ---------------------------------------------------- */
        {
            /* Param1 - Module, Param2 - TimeOut, Param3 - RT Addr */
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL SET RESP TIME OUT CALLED: Channe %d, Mode 0x%X, %d - 0.5us multiples\n",
                pIoctlParams->Channel, pIoctlParams->Param1, pIoctlParams->Param2);

            /* Determine Device's mode of opertion */
            if ((pIoctlParams->Param1 & ACEX_RESP_MTI))
            {
                s16Status = mtSetRespTimeOut(pDeviceContext, pIoctlParams);
            }

            if ((s16Status == DDC_UDL_ERROR__SUCCESS) && (pIoctlParams->Param1 & ACEX_RESP_MRT))
            {
                s16Status = rtSetNoRespTimeOut(pDeviceContext, (U16BIT)pIoctlParams->Channel, (U16BIT)pIoctlParams->Param3,  DDC_IOCTL_U32(pIoctlParams->Param2));
            }

            if ((s16Status == DDC_UDL_ERROR__SUCCESS) && (pIoctlParams->Param1 & ACEX_RESP_BC))
            {
                s16Status = bcSetRespTimeOut(pDeviceContext, pIoctlParams);
            }

            break;
        }

        /* ==================================================== */
        /*             1553 COUPLING  1553 COUPLING             */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_GET_COUPLING:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature */
            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_BUS_COUPLING)
            {
                bytesReturned = sizeof(ACEX_COUPLING);

                pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
                if (!pRdData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                gen1553GetCoupling(pDeviceContext, pIoctlParams, (ACEX_COUPLING *)pRdData);

                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }
            else
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_COUPLING:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature */
            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_BUS_COUPLING)
            {
                gen1553SetCoupling(pDeviceContext, pIoctlParams);
            }
            else
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ==================================================== */
        /*             1553 AMPLITUDE  1553 AMPLITUDE           */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_GET_AMPLITUDE:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature */
            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_VARIABLE_XCVR)
            {
                gen1553GetAmplitude(pDeviceContext, pIoctlParams, &u32Data);

                bytesReturned = sizeof(U32BIT);
                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)&u32Data, bytesReturned);
                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }
            else
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_SET_AMPLITUDE:
        /* ---------------------------------------------------- */
        {
            /* check to see if this device supports this feature */
            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_VARIABLE_XCVR)
            {
                gen1553SetAmplitude(pDeviceContext, pIoctlParams);
            }
            else
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ==================================================== */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_CHECK_MF_CAPABLE:
        /* ---------------------------------------------------- */
        {
            /* return information if the channel support MF concurrency */
            s16Status = gen1553CheckMfCapable(pDeviceContext, pIoctlParams, &bytesReturned, &u32Data);

            bytesReturned = sizeof(U32BIT);
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)&u32Data, bytesReturned);
            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }
            break;
        }


        /* ==================================================== */
        /*               1553 TRIGGER  1553 TRIGGER             */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_1553_TRG_RESET:
        /* ---------------------------------------------------- */
        {
            /* reset trigger */
            s16Status = trigger1553Reset(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_1553_TRG_IRQ_STATUS:
        /* ---------------------------------------------------- */
        {
            /* check the data array s16Status */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            /* read trigger IRQ s16Status */
            s16Status = trigger1553StatusRead(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ========================================================================== */
        /* 1553 BC Group                                                              */
        /* ========================================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_ASYNC_COUNT:
        /* ---------------------------------------------------- */
        {
            /* get async msg count*/
            s16Status = bcGetLpAsyncMsgCount(pDeviceContext, pIoctlParams, &bytesReturned, &u32Data);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetLpAsyncMsgCount ERROR: s16Status=%d\n", s16Status);

                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32Data, bytesReturned);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_DATA_BUFFER:
        /* ---------------------------------------------------- */
        {
            /* get msg data*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcGetData(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetData ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_FRAME_TO_HBUF:
        /* ---------------------------------------------------- */
        {
            /* get data from IMP to HBuf, triggered by user */
            bcGetFrameToHBuf(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_HBUF_MESSAGE:
        /* ---------------------------------------------------- */
        {
            /* Read BC HBuf messages.
               Do not return error code! */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                break;
            }

            s16Status = bcGetHBufMsg(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetHBufMsg ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                s16Status = DDC_UDL_ERROR__SUCCESS;
                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);
            /*
            JMN - Is this necessary?
            If above DDC_COPY_TO_USER is successful then it returns 0, if not then 1.
            This should be good enough and the following DDC_UDL_ERROR__SUCCESS should
            be removed. Can't think of a case why we should leave it unless
            this case should always return SUCCESS?????????????????????????
            */

            s16Status = DDC_UDL_ERROR__SUCCESS;

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_METRICS:
        /* ---------------------------------------------------- */
        {
            /* read BC GPQ/HBuf metric information */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcGetMetric(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetMetric ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_MESSAGE:
        /* ---------------------------------------------------- */
        {
            /* Get BC message with given message ID */
            /* Do not return error code!*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__INSUFFICIENT_MEMORY*/; /* Windows require not return error code */
            }

            s16Status = bcGetMsg(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetMsg ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__SUCCESS /*s16Status*/;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__COPY2USER_FAIL*/;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GPQ_CLEAR:
        /* ---------------------------------------------------- */
        {
            /* clear GPQ information*/
            s16Status = bcGpqClear(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GPQ_COUNT:
        /* ---------------------------------------------------- */
        {
            /* get GPQ count*/
            s16Status = bcGpqGetCount(pDeviceContext, (U16BIT)pIoctlParams->Channel, &bytesReturned, &u16Data);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGpqGetCount ERROR: s16Status=%d\n", s16Status);

                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u16Data, bytesReturned);

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                " ACEX_MRT_CONFIG\n");

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GPQ_READ:
        /* ---------------------------------------------------- */
        {
            /* read GPQ entry */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcGpqRead(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcGetData ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_POST_ASYNC_MESSAGE:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "bcPostAsyncMsg\n");

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(ACEX_BC_ASYNC_STATUS));
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcPostAsyncMsg(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcPostAsyncMsg ERROR: s16Status=%d\n", s16Status);

                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, sizeof(ACEX_BC_ASYNC_STATUS));
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_SET_MSG_BUFFER:
        /* ---------------------------------------------------- */
        {
            /* set BC message buffer */
            if (u32OutputBufferLength)
            {
                pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                if (!pvoidData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), u32OutputBufferLength);

                if (s16Status)
                {
                    DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                    return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
                }

                s16Status = bcSetMsgBuffer(pDeviceContext, pIoctlParams, pvoidData);
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            }
            else
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_BC_SET_MSG_BUFFER No Data\n");
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_SET_DATA_BUFFER:
        /* ---------------------------------------------------- */
        {
            /* set BC data buffer*/
            if (u32OutputBufferLength)
            {
                pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                if (!pvoidData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), u32OutputBufferLength);

                if (s16Status)
                {
                    DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                    return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
                }

                s16Status = bcSeDataBuffer(pDeviceContext, pIoctlParams, pvoidData);

                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            }
            else
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "IOCTL_BC_SET_DATA_BUFFER No Data\n");
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_STATE:
        /* ---------------------------------------------------- */
        {
            /* set state */
            s16Status = bcSetState(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_DATASTR_INIT:
        /* ---------------------------------------------------- */
        {
            /* start data stream */
            s16Status = bcDataStreamInit(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_DATASTR_STATUS:
        /* ---------------------------------------------------- */
        {
            /* check if the data stream is completed */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcDataStreamCheckCompletion(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_DATA_ARRAY_INIT:
        /* ---------------------------------------------------- */
        {
            /* start data array */
            s16Status = bcDataArrayInit(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_DATA_ARRAY_STATUS:
        /* ---------------------------------------------------- */
        {
            /* check the data array s16Status */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcDataArrayCheckStatus(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_GET_EXT_TRIGGER_AVAIL:
        /* ---------------------------------------------------- */
        {
            /* return external trigger availability information*/
            s16Status = bcExtTriggerAvailInfo(pDeviceContext, &bytesReturned, &u32Data);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcExtTriggerAvailInfo ERROR: s16Status=%d\n", s16Status);

                break;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)&u32Data, bytesReturned);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

           break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_ASYNC_QUEUE_INFO:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = bcGetAsyncQueueInfo(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_CMD_STACK_READ:
        /* ---------------------------------------------------- */
        {
            /* Get BC message stack with given message address*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__INSUFFICIENT_MEMORY*/; /* Windows require not return error code */
            }

            s16Status = bcCmdStackRead(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcCmdStackRead ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__SUCCESS /*s16Status*/;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__COPY2USER_FAIL*/;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_DATA_STACK_READ:
        /* ---------------------------------------------------- */
        {
            /* Get BC message data with given message data block address*/
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__INSUFFICIENT_MEMORY*/; /* Windows require not return error code */
            }

            s16Status = bcDataStackRead(pDeviceContext, pIoctlParams, &bytesReturned, (U16BIT*)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "bcDataStackRead ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__SUCCESS /*s16Status*/;
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__SUCCESS /*DDC_UDL_ERROR__COPY2USER_FAIL*/;
            }

            break;
        }


        /* ==================================================== */
        /*            BC REPLAY  BC REPLAY  BC REPLAY           */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_STATUS:
        /* ---------------------------------------------------- */
        {
            /* check the data array s16Status */
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            /* check the data array s16Status */
            s16Status = bcReplayStatus(pDeviceContext, pIoctlParams, &bytesReturned, pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_BLOCK_ON_IRQ:
        /* ---------------------------------------------------- */
        {
            /* See if replay interrupt is enabled */
            DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);
            if (!pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.bReplayIsrEnabled)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BLOCK_ON_IRQ failed: replay interrupts not enabled\n");

                /* Return error indicating interrupts are not enabled */
                s16Status = DDC_UDL_ERROR__ISR_NOT_ENABLED;
                break;
            }

            DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

            /* Sleep and wait for the local ISR or ISR clean-up to wake us up and proceed. */
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1, "Waits for Replay IRQ at channel %d\n", u8ChannelNumber);
            DDC_WAIT_INTERRUPTABLE(
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.waitqueueIrqEvent,
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond);


            DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

            if (pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                s16Status = DDC_COPY_TO_USER((char*)(pInOutBuffer),
                    &pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.pu32IrqStatusQ[pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail],
                    sizeof(U32BIT));

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "Replay: copy data error\n");
                }

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                    "Wake up Replay IRQ at channel %d, IrqStatus = %08X\n", u8ChannelNumber,
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.pu32IrqStatusQ[pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail]);

                DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond--;

                /* point to the next entry of the IRQ s16Status queue */
                pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail++;

                if (pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail == (REPLAY_IRQ_STATUS_QUEUE_SIZE - 1))
                {
                    pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail = 0;
                }

                DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);
            }
            else
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                        "IOCTL_BLOCK_ON_IRQ: no data\n");
                u32Data = 0;
                s16Status = DDC_COPY_TO_USER((char*)(pInOutBuffer),
                    &u32Data,
                    sizeof(U32BIT));
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_RELEASE_IRQ:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_BC_REPLAY_RELEASE_IRQt: Wake up replay ch %d irq\n", u8ChannelNumber);

            /* Wake up the blocked replay thread so it can terminate */
            DDC_ISR_LOCK_TAKE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

            /* Flag replay interrupt processing as disabled */
            pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.bReplayIsrEnabled = FALSE;

            pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond = 1;

            /* initialize replay interrupt s16Status queue index */
            pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQHead = 0;
            pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail = 0;

            DDC_ISR_LOCK_GIVE(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond, pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCondFlag);

            DDC_WAKE_UP(&pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.waitqueueIrqEvent);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_DMA:
        /* ---------------------------------------------------- */
        {
            /*BC_REPLAY_DATA_IOCTL_TYPE *pBcReplayDataIoctl = (BC_REPLAY_DATA_IOCTL_TYPE *)pIoctlParams;*/

            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, pIoctlParams->Param3);
            if (!pvoidData)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BC_REPLAY_DMA: DDC_KERNEL_MALLOC failed\n");

                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), pIoctlParams->Param3);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_BC_REPLAY_DMA: DDC_COPY_FROM_USER failed\n");

                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            /* start replay dma */
            s16Status = bcReplayDma(pDeviceContext, pIoctlParams, (U32BIT *)pvoidData);

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_RAW:
        /* ---------------------------------------------------- */
        {
            /* start replay dma */
            s16Status = bcReplayRaw(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_BC_REPLAY_WRITE:
        /* ---------------------------------------------------- */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), u32OutputBufferLength);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            /* write replay data */
            s16Status = bcReplayWrite(pDeviceContext, pIoctlParams, (U32BIT *)pvoidData);

            DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            break;
        }


        /* ========================================================================== */
        /* DDC_IOCTL_GROUP_1553_MT                                                    */
        /* ========================================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_MT_SET_STATE:
        /* ---------------------------------------------------- */
        {
            s16Status = mtSetMtiState(pDeviceContext, pIoctlParams);
            bytesReturned = 0;
            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_MT_SET_STROBE_REG:
        /* ---------------------------------------------------- */
        {
            s16Status = mtSetStrobeReg(pDeviceContext, pIoctlParams);
            bytesReturned = 0;
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MT_INTERUPT_CONFIG:
        /* ---------------------------------------------------- */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MT_MTI_CONFIG));
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), sizeof(MT_MTI_CONFIG));

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            s16Status = mtMtiInterruptConfig(pDeviceContext, (MT_MTI_CONFIG *)pvoidData);
            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MT_GET_METRICS_CONFIG:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MT_MTI_METRICS));
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = mtMtiGetMetrics(pDeviceContext, pIoctlParams, (MT_MTI_METRICS *)pRdData);
            bytesReturned = sizeof(MT_MTI_METRICS);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MT_FREE_MEM_COUNT:
        /* ---------------------------------------------------- */
        {
            /* fix for 64 bit */
            U32BIT u32WordCount = (U32BIT)pIoctlParams->Param1;

            DDC_REG_WRITE(pDeviceContext,
                ((*(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT.pu32RegBA)) + REG_MT_MTI_FREE_MEM_COUNT_RW), &u32WordCount);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MTI_GET_CH10_STATUS:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MT_MTI_INFO));
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = mtGetMtiCh10Status(pDeviceContext, pIoctlParams, (MT_MTI_INFO *)pRdData);

            if (s16Status)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "mtGetMtiCh10Status ERROR: s16Status=%d\n", s16Status);

                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return s16Status;
            }

            bytesReturned = sizeof(MT_MTI_INFO);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MTI_GET_CH10_DATA_PKT:
        /* ---------------------------------------------------- */
        {
            ACEX_MTI_CH10_DATA_PKT *pPkt;
            struct _ACEX_1553_MT_TYPE *pMT;

            pMT = &(pDeviceContext->pChannel1553[u8ChannelNumber]->sMT);

            s16Status = (S16BIT)mtGetMtiCh10DataPkt(pDeviceContext, pIoctlParams);

            if (s16Status)
            {
                if (s16Status < DDC_UDL_ERROR__SUCCESS)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "mtGetMtiCh10DataPkt ERROR: s16Status=%d\n", s16Status);
                }

                return s16Status;
            }

            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            pRdData = (U32BIT *)(pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail]->pDataPkt);
            DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

            pPkt = (ACEX_MTI_CH10_DATA_PKT *)pRdData;
            bytesReturned = pPkt->u32PktLength;

            /* copy the packet header first */
            pUserBuf = (char *)(pInOutBuffer);
            pBuf = (char *)pRdData;
            u32DataLen = (MTI_CH10_PKT_HEADER_SIZE + MTI_CH_SPEC_DATA_HDR_SIZE);
            s16Status = DDC_COPY_TO_USER(pUserBuf, pBuf, u32DataLen);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }
            else
            {
                /* copy the packet data */
                pUserBuf += u32DataLen;

                DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
                pBuf = (char *)GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER(((char *)pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail]->pDataPkt));

                /* data is shifted by copyOffset bytes */
                pBuf += (pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail]->copyOffset);
                DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

                u32DataLen = (U32BIT)(bytesReturned - u32DataLen);
                s16Status = DDC_COPY_TO_USER(pUserBuf, pBuf, u32DataLen);
                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }

            mtFreeDataPktListTail(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_MTI_GET_CH10_TIME_PKT:
        /* ---------------------------------------------------- */
        {
            s16Status = (S16BIT)mtGetMtiCh10TimePkt(pDeviceContext, pIoctlParams);

            if (s16Status)
            {
                if (s16Status < DDC_UDL_ERROR__SUCCESS)
                {
                   DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "mtGetMtiCh10TimePkt ERROR: s16Status=%d\n", s16Status);
                }

                return s16Status;
            }

            pRdData = (U32BIT *) &pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.pMtiTimeListEntry->sMtiTimePkt;
            bytesReturned = sizeof(MTI_CH10_TIME_PKT);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ========================================================================== */
        /* 1553 RT Group                                                              */
        /* ========================================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_RT_DATA_SEND_STREAM:
        /* ---------------------------------------------------- */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MRT_STREAM_TYPE));
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)psIoctlDataBuffers->sInBuffer.pDataBuffer, psIoctlDataBuffers->sInBuffer.u32BufferLength);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pRdData, (char *)(pInOutBuffer), u32OutputBufferLength);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            rtSendStream(pDeviceContext, (MRT_STREAM_TYPE *)pvoidData, (void *)pRdData, (U32BIT *) &bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_RT_DATA_RECEIVE_STREAM:
        /* ---------------------------------------------------- */
        {
            pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(MRT_STREAM_TYPE));
            if (!pvoidData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)psIoctlDataBuffers->sInBuffer.pDataBuffer, psIoctlDataBuffers->sInBuffer.u32BufferLength);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            (((MRT_STREAM_TYPE *)pvoidData)->pRequest) = (void *)pRdData;
            rtReceiveStream(pDeviceContext, (MRT_STREAM_TYPE *)pvoidData, (U32BIT *) &bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pvoidData);

            if (bytesReturned > 0)
            {
                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }

            DDC_KERNEL_FREE(pDeviceContext, pRdData);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_RT_READ_MODE_CODE_DATA:
        /* ---------------------------------------------------- */
        {
            u16Data = mrtReadModeCodeData(pDeviceContext, u8ChannelNumber, (U8BIT)pIoctlParams->Param1, (U16BIT)pIoctlParams->Param2);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), &u16Data, sizeof(U16BIT));

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_RT_WRITE_MODE_CODE_DATA:
        /* ---------------------------------------------------- */
        {
            mrtWriteModeCodeData(pDeviceContext, u8ChannelNumber, (U8BIT)pIoctlParams->Param1, (U16BIT)pIoctlParams->Param2, (U16BIT)pIoctlParams->Param3);
            break;
        }

        /* ========================================================================== */
        /* DDC_IOCTL_GROUP_ARINC                                                      */
        /* ========================================================================== */


        /* ==================================================== */
        /*           ARINC 429  ARINC 429  ARINC 429            */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_ENABLE_IRQ:
        /* ---------------------------------------------------- */
        {
            /* initialize 429 interrupt s16Status queue index */
            DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
            pDeviceContext->u32ArincIntQHead = 0;
            pDeviceContext->u32ArincIntQTail = 0;
            pDeviceContext->u32ArincIntQLen = 0;
            pDeviceContext->u16ArincBlockOnIrqReadyEventCond = 0;
            DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

            s16Status = irqEnableInterrupt429(pDeviceContext, u8DeviceNumber);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_DISABLE_IRQ:
        /* ---------------------------------------------------- */
        {
            ArincDisableIrq(pDeviceContext, u8DeviceNumber);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_BLOCK_ON_IRQ:
        /* ---------------------------------------------------- */
        {
            /* See if interrupts are enabled */
            if (!pDeviceContext->bArincSerialIsrEnabled)
            {
                /* Return error indicating interrupts are not enabled */
                s16Status = DDC_UDL_ERROR__ISR_NOT_ENABLED;

                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "IOCTL_ARINC_BLOCK_ON_IRQ EXIT 1 <==== \n");

                break;
            }

            DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

            /* decrement the event count */
            if (pDeviceContext->u32ArincIntQLen > 0)
            {
                pDeviceContext->u32ArincIntQLen--;
            }

            if (pDeviceContext->u16ArincBlockOnIrqReadyEventCond == 0)
            {
                /* let the library know that the IOCTL_ARINC_BLOCK_ON_IRQ call is about to block */
                pDeviceContext->u16ArincBlockOnIrqReadyEventCond++;

                DDC_WAKE_UP(&pDeviceContext->waitqueueArincBlockOnIrqReadyEvent);
            }

            /* wait if there is no IRQ s16Status in the queue */
            if (pDeviceContext->u32ArincIntQLen == 0)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                DDC_WAIT_INTERRUPTABLE(
                    pDeviceContext->waitqueueArincBlockOnIrqEvent,
                    pDeviceContext->u32ArincIntQLen);

                DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
            }

            if (pDeviceContext->u32ArincIntQLen)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                s16Status = DDC_COPY_TO_USER((char*)(pInOutBuffer), &pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQTail], sizeof(ARINC_INTERRUPT));

                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }

                /* clear out the s16Status */
                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQTail].u32MasterStatus = 0x00000000;

                DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                pDeviceContext->u32ArincIntQTail++;

                if (pDeviceContext->u32ArincIntQTail >= (ARINC_IRQ_STATUS_QUEUE_SIZE - 1))
                {
                    pDeviceContext->u32ArincIntQTail = 0;
                }
            }

            DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_BLOCK_ON_IRQ_READY:
        /* ---------------------------------------------------- */
        {
            int nResult = 0;

            nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                pDeviceContext->waitqueueArincBlockOnIrqReadyEvent,
                pDeviceContext->u16ArincBlockOnIrqReadyEventCond,
                500);

            if (nResult == 0)
            {
                /* timeout error */
                s16Status = DDC_UDL_ERROR__TIMEOUT;
                break;
            }
			else
			{
				s16Status = DDC_UDL_ERROR__SUCCESS;
			}

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_RELEASE_IRQ:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1, "IOCTL_ARINC_RELEASE_IRQ: Wake up ARINC IRQ\n");

            /* Wake up the blocked ARINC thread so it can terminate */
            DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
            pDeviceContext->u32ArincIntQHead = 0;
            pDeviceContext->u32ArincIntQTail = 0;
            pDeviceContext->u32ArincIntQLen = 0;
            pDeviceContext->u16ArincBlockOnIrqReadyEventCond = 0;
            DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

            DDC_WAKE_UP(&pDeviceContext->waitqueueArincBlockOnIrqEvent);

            break;
        }


        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_LOADTXONE:
        /* ---------------------------------------------------- */
        {
            ARINC429LoadTxQueueOne(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_LOADTXMORE:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);  /* was kmalloc(u32OutputBufferLength, GFP_KERNEL); */
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_COPY_TO_USER((char *)pRdData, (char *)(pInOutBuffer), u32OutputBufferLength);
            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
            }

            ARINC429LoadTxQueueMore(pDeviceContext,
                pIoctlParams,
                (U32BIT *)pRdData,
                u32OutputBufferLength);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_GET_TT_CONFIG:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(DDC_IOCTL_PARAMS);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            ARINC429GetTimeStampConfig(pDeviceContext, (DDC_IOCTL_PARAMS *)pRdData);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_SET_TT_CONFIG:
        /* ---------------------------------------------------- */
        {
            ARINC429SetTimeStampConfig(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_GET_PGRM_LOOPBACK:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(U32BIT);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            *pRdData = ARINC429GetLoopback(pDeviceContext, (S16BIT)pIoctlParams->Param1);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_SET_PGRM_LOOPBACK:
        /* ---------------------------------------------------- */
        {
            ARINC429SetLoopback(pDeviceContext, (S16BIT)pIoctlParams->Param1, (S16BIT)pIoctlParams->Param2);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_REG_READ:
        /* ---------------------------------------------------- */
        {
            /* only allow legacy devices to call this function */
            if ((pDeviceContext->u16DriverType == ACEX_DD429_PCIE_DRIVER) ||
                (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER))
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            bytesReturned = sizeof(U32BIT);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            *pRdData = ARINC429RegRead(pDeviceContext, pIoctlParams);

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_REG_WRITE:
        /* ---------------------------------------------------- */
        {
            /* only allow legacy devices to call this function */
            if ((pDeviceContext->u16DriverType == ACEX_DD429_PCIE_DRIVER) ||
                (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER))
            {
                s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;
            }

            ARINC429RegWrite(pDeviceContext, pIoctlParams);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_MEM_READ:
        /* ---------------------------------------------------- */
        {
            bytesReturned = sizeof(U32BIT);

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, (size_t)bytesReturned);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            *pRdData = ARINC429MemRead(pDeviceContext, pIoctlParams);

#if DDC_PPC

            /* 429 memory does not do byte swapping */
            *pRdData = DDC_BYTE_ORDER_L(*pRdData);
#endif /* DDC_PPC */

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);

            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                return DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_MEM_READ_MORE:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_ARINC_429_MEM_READ_MORE ENTER ====> \n");

            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = DDC_BLK_MEM_READ(pDeviceContext,  DDC_IOCTL_U32(pIoctlParams->Param1), pRdData, (u32OutputBufferLength / 4), ACEX_32_BIT_ACCESS);

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_ARINC_429_MEM_READ_MORE 32 BIT: Addr=0x%08x Data=0x%08x Cnt=0x%08x \n",
                (unsigned int)pIoctlParams->Param1, (unsigned int)(*pRdData), u32OutputBufferLength / 4);

            bytesReturned = u32OutputBufferLength;
            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            if (s16Status)
            {
                s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1,
                "IOCTL_ARINC_429_MEM_READ_MORE EXIT <==== \n");

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_MEM_WRITE:
        /* ---------------------------------------------------- */
        {
#if DDC_PPC

            /* 429 memory does not do byte swapping */
            pIoctlParams->Param2 = DDC_BYTE_ORDER_L(pIoctlParams->Param2);
#endif /* DDC_PPC */

            ARINC429MemWrite(pDeviceContext, pIoctlParams);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_SET_OLD_LOOPBACK:
        /* ---------------------------------------------------- */
        {
            ARINC429SetLegacyLoopback(pDeviceContext);
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_GET_OLD_LOOPBACK:
        /* ---------------------------------------------------- */
        {
            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_RX_HBUF_CTRL:
        /* ---------------------------------------------------- */
        {
            if (u32OutputBufferLength > 0)
            {
                pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                if (!pRdData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }
            }

            s16Status = ARINC429HostBufferControl(pDeviceContext,
                pIoctlParams,
                &bytesReturned,
                (U32BIT*)pRdData);

            if (s16Status)
            {
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
                return DDC_UDL_ERROR__READ;
            }

            if (u32OutputBufferLength > 0)
            {
                s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                DDC_KERNEL_FREE(pDeviceContext, pRdData);

                if (s16Status)
                {
                    return DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_429_GET_QUEUE_STATUS:
        /* ---------------------------------------------------- */
        {
            pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
            if (!pRdData)
            {
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            s16Status = ARINC429GetQueueStatus(pDeviceContext,
                pIoctlParams->Channel,
                &bytesReturned,
                (U32BIT*)pRdData);

            if (s16Status != DDC_UDL_ERROR__SUCCESS)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    "DdcSfpUsbEvtIoDeviceControl: ARINC429GetQueueStatus failed \n");
            }

            s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
            DDC_KERNEL_FREE(pDeviceContext, pRdData);

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_DD429X_COMMAND:
        /* ---------------------------------------------------- */
        {
            U32BIT *pLocalInOutBuffer = NULL;

            /* if the input/output buffer is defined, copy the contents from user space */
            if (u32OutputBufferLength)
            {
                pLocalInOutBuffer = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);

                if (!pLocalInOutBuffer)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = DDC_COPY_FROM_USER((char *)pLocalInOutBuffer, (char *)pInOutBuffer, u32OutputBufferLength);
                if (s16Status)
                {
                    DDC_KERNEL_FREE(pDeviceContext, pLocalInOutBuffer);
                    return DDC_UDL_ERROR__COPYFROMUSER_FAIL;
                }
            }

            s16Status = ARINC429ProcessCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, u32OutputBufferLength, &bytesReturned);

            if (bytesReturned > u32OutputBufferLength)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                    " - IOCTL_DD429X_COMMAND - Buffer Too Small - u32OutputBufferLength: %d Bytes   Needed: %d Bytes\n",
                    (int)u32OutputBufferLength, (int)sizeof(U32BIT));

                bytesReturned = 0;
                s16Status = DDC_UDL_ERROR__BUFFER_SIZE;

                DDC_KERNEL_FREE(pDeviceContext, pLocalInOutBuffer);

                break;
            }

            if (bytesReturned)
            {
                s16Status = DDC_COPY_TO_USER((void *)pInOutBuffer, (void *)pLocalInOutBuffer, bytesReturned);
                if (s16Status)
                {
                    s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                }
            }

            if (u32OutputBufferLength)
            {
                DDC_KERNEL_FREE(pDeviceContext, pLocalInOutBuffer);
            }

            break;
        }


        /* ==================================================== */
        /*           ARINC 717  ARINC 717  ARINC 717            */
        /* ==================================================== */

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_717_LOAD_TX:
        /* ---------------------------------------------------- */
        {
            /* Load transmit data */

            if (pIoctlParams == NULL)
            {
                break;
            }

            if (u32OutputBufferLength)
            {
                pvoidData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                if (!pvoidData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = DDC_COPY_FROM_USER((char *)pvoidData, (char *)(pInOutBuffer), u32OutputBufferLength);

                if (s16Status)
                {
                    return DDC_UDL_ERROR__COPY2USER_FAIL;
                }

                ARINC717LoadTxQueueData(pDeviceContext, pIoctlParams, (U32BIT *)pvoidData);

                DDC_KERNEL_FREE(pDeviceContext, pvoidData);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case IOCTL_ARINC_717_GET_RX:
        /* ---------------------------------------------------- */
        {
            /* Retrieve 717 data */
            if (pIoctlParams == NULL)
            {
                break;
            }

            if (u32OutputBufferLength)
            {
                pRdData = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);
                if (!pRdData)
                {
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                s16Status = ARINC717ReadData(pDeviceContext, pIoctlParams, pRdData, u32OutputBufferLength, &bytesReturned);

                if (bytesReturned > u32OutputBufferLength)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        " - IOCTL_ARINC_717_GET_RX - Buffer Too Small - "
                        " u32OutputBufferLength: %d Bytes   Needed: %d Bytes\n",
                        (int)u32OutputBufferLength, (int)sizeof(U32BIT));

                    bytesReturned = 0;
                    s16Status = DDC_UDL_ERROR__BUFFER_SIZE;
                    break;
                }

                if (bytesReturned)
                {
                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *)pRdData, bytesReturned);
                }
                DDC_KERNEL_FREE(pDeviceContext, pRdData);
            }
            break;
        }

        /* ------------------------ */
        /* PLX                      */
        /* ------------------------ */
        case IOCTL_PLX_READ:
        {
            switch (pIoctlParams->Param1)
            {
                case PLX_READ_REG:
                {
                    /* Param2 = register */
                    U32BIT u32RegVal = 0;

                    DDC_PLX_READ(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param2), &u32RegVal);

                    s16Status = DDC_COPY_TO_USER((char *)(pInOutBuffer), (char *) &u32RegVal, sizeof(u32RegVal));

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    bytesReturned = sizeof(U32BIT);

                    break;
                }

                case PLX_READ_EEPROM:
                {
                    size_t i;
                    U32BIT *pu32Data;
                    U32BIT u32Index = 0;

                    pu32Data = (U32BIT *) pRdData;

                    /* Param2 = address */
                    /* pRdData = location to put read data */
                    /* u32OutputBufferLength = # bytes to read */

                    for (i=0; i < u32OutputBufferLength; i+=4)
                    {
                        ddcUdlBusPciPlx9000_EepromReadByOffset(pDeviceContext, (U16BIT)(i), &pu32Data[u32Index]);
                        u32Index++;
                    }

                    bytesReturned = u32OutputBufferLength;
                    break;
                }

                default:
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "Invalid IOCTL_PLX_READ Param1 Option (%d)", (int)pIoctlParams->Param1);

                    break;
                }
            }

            break;
        }

        case IOCTL_PLX_WRITE:
        {
            switch (pIoctlParams->Param1)
            {
                case PLX_WRITE_REG:
                {
                    /* Param2 = register */
                    /* Param3 = value */
                    U32BIT u32RegVal = DDC_IOCTL_U32(pIoctlParams->Param3);

                    DDC_PLX_WRITE(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param2), u32RegVal);
                    break;
                }

                case PLX_WRITE_EEPROM:
                {
                    size_t i;
                    U32BIT *pu32Data = NULL;
                    U32BIT u32Index = 0;

                    pu32Data = DDC_KERNEL_MALLOC(pDeviceContext, u32OutputBufferLength);

                    if (!pu32Data)
                    {
                        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                    }

                    s16Status = DDC_COPY_FROM_USER(pu32Data, (char *)(pInOutBuffer), u32OutputBufferLength);

                    if (s16Status)
                    {
                        s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
                    }

                    /* Param2 = address */
                    /* pRdData = data to write */
                    /* u32OutputBufferLength = # bytes to write */

                    for (i=0; i < u32OutputBufferLength; i+=4)
                    {
                        ddcUdlBusPciPlx9000_EepromWriteByOffset(pDeviceContext, (U16BIT)(i), pu32Data[u32Index]);
                        u32Index++;
                    }

                    break;
                }

                default:
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                        "Invalid IOCTL_PLX_WRITE Param1 Option (%d)", (int)pIoctlParams->Param1);

                    break;
                }
            }

            break;
        }

        /* ==================================================================== */
        /* ==================================================================== */
        /*                        INVALID IOCTL VALUE                           */
        /* ==================================================================== */
        /* ==================================================================== */

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            s16Status = DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED;

            DDC_DBG_PRINT(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_failures,
                "ERROR: Invalid IOCTL Request - Group: %d  Type: %d\n",
                DDC_IOCTL__GROUP_GET_VAL(u32IoctlCmd),
                DDC_IOCTL__TYPE_GET_VAL(u32IoctlCmd));

            break;
        }
    }

    if (pu32OutBufferBytesReturned != 0)
    {
        S16BIT s16CopyToUserStatus = DDC_UDL_ERROR__SUCCESS;

        /* copy to 32-bit sized item to copy over to 32-bit sized return item */
        u32BytesReturned = (U32BIT)bytesReturned;

        s16CopyToUserStatus = DDC_COPY_TO_USER((void *)pu32OutBufferBytesReturned, (void *)&u32BytesReturned, sizeof(u32BytesReturned));

        if (s16CopyToUserStatus)
        {
            s16Status = DDC_UDL_ERROR__COPY2USER_FAIL;
        }
    }

    return s16Status;
}

