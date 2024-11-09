/*******************************************************************************
 * FILE: ddc_udl_1553.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the General 1553 Component.
 *
 *  This component will exist for each 1553 channel. This component
 *  contains the following:
 *
 *      Time Tag functions
 *      1553 component interrupt enables (MT, IMP, RT, BC, Ram, TT Roll-over)
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
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_types.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "driver_sdk/ddc_udl_dma_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_bc_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "core/1553/ddc_udl_1553_imp_private.h"
#include "core/1553/ddc_udl_1553_error_inj_private.h"
#include "core/1553/ddc_udl_1553_trigger_private.h"



/*******************************************************************************
 * Name:    _ddcUdlChannel1553Alloc
 *
 * Description:
 *      This function allocates the memory for all 1553 channel structures.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static S16BIT _ddcUdlChannel1553Alloc
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT i;

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        pDeviceContext->pChannel1553[i] = (struct _ACEX_1553_CHANNEL_TYPE *) DDC_KERNEL_MALLOC(pDeviceContext, sizeof(*pDeviceContext->pChannel1553[i]));

        if (pDeviceContext->pChannel1553[i] == NULL)
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        memset(pDeviceContext->pChannel1553[i], 0, sizeof(*pDeviceContext->pChannel1553[i]));
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdl1553ChannelInit
 *
 * Description:
 *      Initializes the 1553 Channel structure with hardware interface
 *      information.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT ddcUdl1553ChannelInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    UM_DEVICE_INFO *pUmDevicePtr = NULL;
    U16BIT s16Result = DDC_UDL_ERROR__SUCCESS;
    U16BIT numDevicesIndex;
    size_t i;
    size_t j;
    BOOLEAN bDeviceFound = FALSE;
    U8BIT u8StartChIndex = 0; /* Start channel index */

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE, "DEVCNT:%d\n", pDeviceContext->u8Num1553Channels);

    if (pDeviceContext->u8Num1553Channels == 0)
    {
        /* if no 1553 channels, exit */
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    s16Result = _ddcUdlChannel1553Alloc(pDeviceContext);

    if (s16Result != DDC_UDL_ERROR__SUCCESS)
    {
        return s16Result;
    }

    if (pUmInfo == NULL)
    {
        return DDC_UDL_ERROR__NULL_PTR;
    }

    /* search for 1553 virtual device */
    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        if ((pUmDevicePtr->umDevType == UM_DEVICE_ID_MIL_STD_1553) ||
            (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIL_STD_1553_SF) ||
            (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIL_STD_1553_MF))
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE, "# Channels:%d\n", pDeviceContext->u8Num1553Channels);

            for (i = u8StartChIndex; i < (pUmDevicePtr->umDevNumInstances + u8StartChIndex) && (i < pDeviceContext->u8Num1553Channels); i++)
            {
                /* populate channel information */
                pDeviceContext->pChannel1553[i]->u16ChannelNum = (U16BIT)i;
                pDeviceContext->pChannel1553[i]->pu32MemSize = &(pUmDevicePtr->umDevMemSize);
                pDeviceContext->pChannel1553[i]->pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i - u8StartChIndex]);
                pDeviceContext->pChannel1553[i]->u32ChannelMemDwdMask = *pDeviceContext->pChannel1553[i]->pu32MemSize - 1;

                /* initialize memory pool available to user - this will be altered when IMP and RTX modules are loaded */
                pDeviceContext->pChannel1553[i]->u32UserMemBA = *(pDeviceContext->pChannel1553[i]->pu32MemBA);
                pDeviceContext->pChannel1553[i]->u32UserMemSizeBytes = pUmDevicePtr->umDevMemSize * 4;

                DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE, "\n");
                DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                    "ch:%d MEM: BA:%08x  Size:%d  MEM mask 0x%08x\n",
                    (int)i,
                    *(pDeviceContext->pChannel1553[i]->pu32MemBA),
                    *(pDeviceContext->pChannel1553[i]->pu32MemSize),
                    pDeviceContext->pChannel1553[i]->u32ChannelMemDwdMask);

                for (j = 0; j < pUmDevicePtr->umDevNumComponents; j++)
                {
                    switch (pUmDevicePtr->umComponentInfo[j].umComponentType)
                    {
                        case UM_COMPONENTS_ID_MIL_STD_1553_SF_GLOBAL:
                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_GLOBAL:
                        {
                            pDeviceContext->pChannel1553[i]->pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "GLOBAL REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_SF_BCI:
                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_BCI:
                        {
                            pDeviceContext->pChannel1553[i]->sBC.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sBC.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "BC REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sBC.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sBC.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_SF_RTX:
                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_RTX:
                        {
                            pDeviceContext->pChannel1553[i]->sRT.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sRT.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "MRT REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sRT.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sRT.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE:
                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_MTIE:
                        {
                            pDeviceContext->pChannel1553[i]->sMT.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sMT.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "MTI REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sMT.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sMT.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP:
                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP:
                        {
                            /* shared IMP */
                            pDeviceContext->pChannel1553[i]->sImpBC.u32ComponentType = pUmDevicePtr->umComponentInfo[j].umComponentType;
                            pDeviceContext->pChannel1553[i]->sImpBC.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sImpBC.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->pChannel1553[i]->sImpRT.u32ComponentType = pUmDevicePtr->umComponentInfo[j].umComponentType;
                            pDeviceContext->pChannel1553[i]->sImpRT.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sImpRT.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "IMP REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sImpBC.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sImpBC.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_BC:
                        {
                            /* UM Register base address always starts at an index value of 0, therefore,
                             * for multiple FPGAs, must deduct u8StartChIndex from current channel # (i) to
                             * obtain proper channel assignment */
                            pDeviceContext->pChannel1553[i]->sImpBC.u32ComponentType = pUmDevicePtr->umComponentInfo[j].umComponentType;
                            pDeviceContext->pChannel1553[i]->sImpBC.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sImpBC.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "IMP-BC REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sImpBC.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sImpBC.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_MRT:
                        {
                            /* UM Register base address always starts at an index value of 0, therefore,
                             * for multiple FPGAs, must deduct u8StartChIndex from current channel # (i) to
                             * obtain proper channel assignment */
                            pDeviceContext->pChannel1553[i]->sImpRT.u32ComponentType = pUmDevicePtr->umComponentInfo[j].umComponentType;
                            pDeviceContext->pChannel1553[i]->sImpRT.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sImpRT.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "IMP-MRT REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sImpRT.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sImpRT.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_EI:
                        {
                            /* UM Register base address always starts at an index value of 0, therefore,
                             * for multiple FPGAs, must deduct u8StartChIndex from current channel # (i) to
                             * obtain proper channel assignment */
                            pDeviceContext->pChannel1553[i]->sErrorInj.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sErrorInj.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "EI REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sErrorInj.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sErrorInj.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_REPLAY:
                        {
                            /* UM Register base address always starts at an index value of 0, therefore,
                             * for multiple FPGAs, must deduct u8StartChIndex from current channel # (i) to
                             * obtain proper channel assignment */
                            pDeviceContext->pChannel1553[i]->sReplay.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sReplay.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "REPLAY REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sReplay.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sReplay.pu32RegSize));
                            break;
                        }

                        case UM_COMPONENTS_ID_MIL_STD_1553_MF_TRIGGERS:
                        {
                            /* UM Register base address always starts at an index value of 0, therefore,
                             * for multiple FPGAs, must deduct u8StartChIndex from current channel # (i) to
                             * obtain proper channel assignment */
                            pDeviceContext->pChannel1553[i]->sTrigger.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i - u8StartChIndex]);
                            pDeviceContext->pChannel1553[i]->sTrigger.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE,
                                "TRG REG: BA:%08x  Size:%d\n",
                                *(pDeviceContext->pChannel1553[i]->sTrigger.pu32RegBA),
                                *(pDeviceContext->pChannel1553[i]->sTrigger.pu32RegSize));
                            break;
                        }

                        default:
                        {
                            DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INIT_MODE, "*** Not a 1553 Component ***\n");
                            break;
                        }
                    }
                }
            }

            /* Update for next FPGA mapping, if exist */
            u8StartChIndex = (U8BIT)i;
            bDeviceFound = TRUE;
        }
    }

    if (bDeviceFound)
    {
        return DDC_UDL_ERROR__SUCCESS;
    }
    else
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }
}

/*******************************************************************************
 * Name:    ddcUdl1553ChannelCleanup
 *
 * Description:
 *      This function performs any cleanup for the 1553 Channel structure.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdl1553ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT i;

    /* free Triggers */
    trigger1553Free(pDeviceContext);

    /* free BC memory */
    bcFree(pDeviceContext);

#if DDC_DMA_RT 
    /* free pu8RtDmaTarget */
    if (pDeviceContext->pu8RtDmaTarget)
    {
        DDC_DMA_FREE(
            pDeviceContext,
            pDeviceContext->RtDmaSize,
            pDeviceContext->pu8RtDmaTarget,
            pDeviceContext->RtDmaAddr,
            DDC_MEMORY_REGION__RT_DMA);

        pDeviceContext->pu8RtDmaTarget = NULL;
    }
#endif /* DDC_DMA_RT */

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        /* free IRQ status memory*/
        if (pDeviceContext->pChannel1553[i]->pu32IrqStatusQ)
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pChannel1553[i]->pu32IrqStatusQ);
            pDeviceContext->pChannel1553[i]->pu32IrqStatusQ = NULL;
        }

        if (pDeviceContext->pChannel1553[i]->sReplay.pu32IrqStatusQ)
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pChannel1553[i]->sReplay.pu32IrqStatusQ);
            pDeviceContext->pChannel1553[i]->sReplay.pu32IrqStatusQ = NULL;
        }

        /* DO NOT ADD ANY CODE THAT REFERENCES pChannel1553 AFTER THIS! */
        if (pDeviceContext->pChannel1553[i])
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pChannel1553[i]);

            pDeviceContext->pChannel1553[i] = NULL;
        }
    }
}

/*******************************************************************************
 * Name:    gen1553ChannelInit
 *
 * Description:
 *      This function will perform a board reset for the given channel.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel number
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
U32BIT gen1553ChannelInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    /* Reset 1553 channel */
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, (1 << u8Ch));

    /* Reset BC */
    bcCloseAction(pDeviceContext, u8Ch);

    /* Reset RT */
    mrtClose(pDeviceContext, u8Ch);

    /* Reset MT */
    mtClose(pDeviceContext, u8Ch);

    pDeviceContext->pChannel1553[u8Ch]->u32IrqAioInterruptMask = 0x00000000;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553SetRamParityChecking
 *
 * Description:
 *      This function enables or disables RAM parity checking for
 *      hardware containing 17-bit buffered RAM.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553SetRamParityChecking
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32State = 0;
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[pIoctlParams->Channel];

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_RAM_PAR_CHK, "ENTER-> Value Set is %d\n", (int)pIoctlParams->Param1);

    DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_INT_MASK, &u32State);

    if (pIoctlParams->Param1) /* enalbe */
    {
        u32State |= GENERAL_INT_MASK_RAM_PARITY_DETECTED;
    }
    else /* disable */
    {
        u32State &= ~GENERAL_INT_MASK_RAM_PARITY_DETECTED;
    }

    DDC_REG_WRITE(pDeviceContext, (*(pCh->pu32RegBA) + REG_GENERAL_INT_MASK), &u32State);

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_RAM_PAR_CHK, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553SetTimeTagResolution
 *
 * Description:
 *      This function sets the resolution of the Time Tag Register.
 *
 *      The valid wTTRes values are:
 *      ACEX_TT_64US  -> 64us resolution
 *      ACEX_TT_32US  -> 32us resolution
 *      ACEX_TT_16US  -> 16us resolution
 *      ACEX_TT_8US   -> 8us resolution
 *      ACEX_TT_4US   -> 4us resolution
 *      ACEX_TT_2US   -> 2us resolution
 *      ACEX_TT_1US   -> 1us resolution
 *      ACEX_TT_500NS -> 500ns resolution
 *      ACEX_TT_250NS -> 250ns resolution
 *      ACEX_TT_TEST  -> Increment manually
 *                      Time-tag counter incremented by 1 when host
 *                      writes the Time-Test-Clk, bit 7 of the
 *                      Global Control Pulse register.
 *      ACEX_TT_EXT   -> Use external time tag clock (TAG_CLK input pin)
 *                      Time-tag counter incremented by 1 on the
 *                      external clock rising edge.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553SetTimeTagResolution
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Channel = pIoctlParams->Channel;
    U32BIT u32TTResolution = 0;
    U32BIT u32BdCfg = 0;
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u32Channel];

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_SET_TT_RES, "ENTER-> Set Value is %d\n", (int)pIoctlParams->Param1);

    /* Time Tag Resolution parameter is part of the Global Contorl Register.
     * Therefore register must be read prior to setting appropriate bits */
    DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32TTResolution));

    /* Align to bit location in register */
    pIoctlParams->Param1 >>= GENERAL_CTRL_TT_RESOLUTION_BIT_SHIFT_ALIGN;
    u32TTResolution = (u32TTResolution & ~GENERAL_CTRL_TT_RESOLUTION_MASK) | DDC_IOCTL_U32(pIoctlParams->Param1);
    DDC_REG_WRITE(pDeviceContext, (*(pCh->pu32RegBA) + REG_GENERAL_CTRL), &u32TTResolution);

    DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32TTResolution));
    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_SET_TT_RES, "1553 Gen Control value is %X\n", u32TTResolution);

    /* On dual FPGA channels 2 and 3 ext TT enable is on the 2nd FPGA */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u32Channel]]->pu32RegBA) + REG_BD_CONFIG, &(u32BdCfg));

    /* if we are using the external TT clk, enable in HW */
    if (pIoctlParams->Param1 == ACEX_TT_EXT)
    {
        u32BdCfg |= BD_CONFIG_MASK_EXT_TT_CLK_EN;
    }
    else
    {
        u32BdCfg &= ~BD_CONFIG_MASK_EXT_TT_CLK_EN;
    }

    DDC_REG_WRITE( pDeviceContext, *(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u32Channel]]->pu32RegBA) + REG_BD_CONFIG, &u32BdCfg);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553SetTimeTagRolloverPoint
 *
 * Description:
 *      This function sets the timetag rollover point.
 *
 *      Values are:
 *      0  - 16 bits
 *      1  - 17 bits
 *      2  - 18 bits
 *      3  - 19 bits
 *      4  - 20 bits
 *      5  - 21 bits
 *      6  - 22 bits
 *      7  - 48 bits
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553SetTimeTagRolloverPoint
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Channel = pIoctlParams->Channel;
    U32BIT u32TTRolloverPoint = 0;
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u32Channel];

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_TT_RO_PT, "ENTER->Set Value is %d\n", (int)pIoctlParams->Param1);

    /* Time Tag Resolution parameter is part of the Global Contorl Register.
     * Therefore register must be read prior to setting appropriate bits */
    DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32TTRolloverPoint));

    /* Align to bit locations in register */
    pIoctlParams->Param1 <<= GENERAL_CTRL_TT_ROLLOVER_POINT_LSHIFT_ALIGN;
    u32TTRolloverPoint = (u32TTRolloverPoint & ~GENERAL_CTRL_TT_ROLLOVER_POINT_MASK) | DDC_IOCTL_U32(pIoctlParams->Param1);

    DDC_REG_WRITE(pDeviceContext, (*(pCh->pu32RegBA) + REG_GENERAL_CTRL), &u32TTRolloverPoint);

    DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32TTRolloverPoint));

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_TT_RO_PT, "EXIT, 1553 Gen Control value is %X\n", u32TTRolloverPoint);
    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553ExtTTCntCtrl
 *
 * Description:
 *      This function enables/disables the external timetag reset/cnt pin.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553ExtTTCntCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Channel = 0;
    U32BIT u32Ctrl = DDC_IOCTL_U32(pIoctlParams->Param1);
    struct _ACEX_1553_CHANNEL_TYPE *pCh = NULL;
    U32BIT u32BdCfg = 0;
    U32BIT u32GenCtrl = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_EXT_TT_CNT_CTRL, "ENTER->Ctrl: %d\n", (int)pIoctlParams->Param1);

    for (u32Channel = 0; u32Channel < pDeviceContext->u8BoardInstanceCount; u32Channel++)
    {
        /* Set the Global Ctrl bit */
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[u32Channel]->pu32RegBA) + REG_BD_CONFIG, &(u32BdCfg));

        if (u32Ctrl)
        {
            u32BdCfg |= BD_CONFIG_MASK_EXT_TT_CNT_EN;
        }
        else
        {
            u32BdCfg &= ~BD_CONFIG_MASK_EXT_TT_CNT_EN;
        }

        DDC_REG_WRITE( pDeviceContext, *(pDeviceContext->pBoardInfo[u32Channel]->pu32RegBA) + REG_BD_CONFIG, &u32BdCfg);
    }

    /* Set the Ctrl bit for each channel */
    for (u32Channel = 0; u32Channel < pDeviceContext->u8Num1553Channels; u32Channel++)
    {
        pCh = pDeviceContext->pChannel1553[u32Channel];

        DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32GenCtrl));

        if (u32Ctrl)
        {
            u32GenCtrl |= GENERAL_CTRL_EXT_TT_COUNT_ENABLE;
        }
        else
        {
            u32GenCtrl &= ~GENERAL_CTRL_EXT_TT_COUNT_ENABLE;
        }

        DDC_REG_WRITE(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_CTRL, &(u32GenCtrl));
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_EXT_TT_CNT_CTRL, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553InterruptClear
 *
 * Description:
 *      This function removes the interrupt mask settings passed in the call
 *      from the existing master interrupt mask settings.  Hw is updated.
 *
 *      NOTE: to clear all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 *      Interrupt Bits
 *      --------------
 *
 *      6 : IMP INTERRUPT
 *      5 : MT INTERRUPT
 *      4 : RT INTERRUPT
 *      3 : BC INTERRUPT
 *      2 : Ram Self Test Complete (auto clear when read)
 *      1 : Ram Parity Error       (auto clear when read)
 *      0 : Time Tag Rollover      (auto clear when read)
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * In   u32IntMask      interrupt mask
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553InterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    pCh->sGen1553Reg.u32IntEn = pCh->sGen1553Reg.u32IntEn & ~(u32IntMask);

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->pu32RegBA)) + REG_GENERAL_INT_MASK), &(pCh->sGen1553Reg.u32IntEn));

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INT_CLEAR,
        "WRITE 1553 CH %d INT - Mask: %08x  REG:%08x VALUE:%08x\n",
        u8Ch, u32IntMask, (*(pCh->pu32RegBA)) + REG_GENERAL_INT_MASK, pCh->sGen1553Reg.u32IntEn);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553InterruptSet
 *
 * Description:
 *      This function adds the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *
 *      NOTE: to set all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 *         Interrupt Bits
 *      --------------
 *
 *      9 : Triggers INTERRUPT
 *      8 : Imp BC INTERRUPT
 *      7 : Imp RT INTERRUPT
 *      6 : IMP INTERRUPT
 *      5 : MT INTERRUPT
 *      4 : RT INTERRUPT
 *      3 : BC INTERRUPT
 *      2 : Ram Self Test Complete (auto clear when read)
 *      1 : Ram Parity Error       (auto clear when read)
 *      0 : Time Tag Rollover      (auto clear when read)
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * In   u32IntMask      interrupt mask
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553InterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    pCh->sGen1553Reg.u32IntEn = pCh->sGen1553Reg.u32IntEn | u32IntMask;

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->pu32RegBA)) + REG_GENERAL_INT_MASK), &(pCh->sGen1553Reg.u32IntEn));

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_INT_SET,
        "WRITE 1553 CH %d INT - Mask: %08x  REG:%08x VALUE:%08x\n",
        u8Ch, u32IntMask, (*(pCh->pu32RegBA)) + REG_GENERAL_INT_MASK, pCh->sGen1553Reg.u32IntEn);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    gen1553MemClear
 *
 * Description:
 *      Clears Ram block.
 *
 *      Note that for USB a 4k dwd block is the largest size that can be
 *      successfully written at a time.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         starting address (32-bit addressing)
 * In   u32NumDwds      number of 32-bit words to clear
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553MemClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32NumDwds,
    U32BIT u32Addr
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT *pTempBuffer = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_MEM_CLEAR, "ENTER->Addr:0x%08x NumDwds:%d\n", u32Addr, u32NumDwds);

    /* allocate temp buffer */
    pTempBuffer = DDC_KERNEL_MALLOC(pDeviceContext, u32NumDwds * sizeof(U32BIT));

    if (!pTempBuffer)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_MEM_CLEAR, "Memory Allocation Failure\n");
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    /* fill buffer with all 0's and write it to hw memory */
    memset(pTempBuffer, 0, u32NumDwds * sizeof(U32BIT));
    DDC_BLK_MEM_WRITE(pDeviceContext, u32Addr, pTempBuffer, u32NumDwds, ACEX_32_BIT_ACCESS);

    DDC_KERNEL_FREE(pDeviceContext, pTempBuffer);

    return status;
}

/*******************************************************************************
 * Name:    gen1553SetIRQ
 *
 * Description:
 *      Sets IRQ
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Mask         IRQ mask value
 * In   u16Enable       enable bit
 * In   u8Channel       channel
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT gen1553SetIRQ
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Mask,
    U16BIT u16Enable,
    U8BIT u8Channel
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_1553, DDC_DBG_1553_SET_IRQ,
        "u32Mask: %08x  u16Enable: %d  u8Channel: %d\n",
        u32Mask, u16Enable, u8Channel);
	
    if (u16Enable)
    {
        pDeviceContext->pChannel1553[u8Channel]->u32IrqUsrEv |= u32Mask;
    }
    else
    {
        pDeviceContext->pChannel1553[u8Channel]->u32IrqUsrEv &= ~u32Mask;
    }

    /* ------------------------------------------------------------- */
    /*                    RT INTERRUPT MASK SET                      */
    /* ------------------------------------------------------------- */

    if (u32Mask & ACE_IMR2_RT_CSTK_50P_ROVER)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER);
        }
    }

    if (u32Mask & ACE_IMR2_RT_CIRC_50P_ROVER)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CBUF_50_ROLLOVER);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CBUF_50_ROLLOVER);
        }
    }

    if (u32Mask & ACE_IMR1_RT_MODE_CODE)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_MODE_CODE);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_MODE_CODE);
        }
    }

    if (u32Mask & ACE_IMR1_RT_SUBADDR_EOM)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM);
        }
    }

    if (u32Mask & ACE_IMR1_RT_CIRCBUF_ROVER)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CBUF_ROLLOVER);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CBUF_ROLLOVER);
        }
    }

    if (u32Mask & ACE_IMR1_RT_ADDR_PAR_ERR)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_ADDR_PARITY);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_ADDR_PARITY);
        }
    }

    if (u32Mask & ACE_IMR2_RT_ILL_CMD)
    {
        if (u16Enable) /* bEnable */
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_ILLEGAL_CMD);
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_ILLEGAL_CMD);
        }
    }

    /* ------------------------------------------------------------- */
    /*                    BC INTERRUPT MASK SET                      */
    /* ------------------------------------------------------------- */

    if (u32Mask & ACE_IMR1_BC_STATUS_SET)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_STATUS_SET);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_STATUS_SET);
        }
    }

    if (u32Mask & ACE_IMR1_BC_MSG_EOM)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_SELECT_EOM);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_SELECT_EOM);
        }
    }

    if (u32Mask & ACE_IMR1_BC_RETRY)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_RETRY);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_RETRY);
        }
    }

    if (u32Mask & ACE_IMR2_BC_UIRQ0)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_IRQ_0);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_IRQ_0);
        }
    }

    if (u32Mask & ACE_IMR2_BC_UIRQ1)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_IRQ_1);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_IRQ_1);
        }
    }

    if (u32Mask & ACE_IMR2_BC_UIRQ2)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_IRQ_2);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_IRQ_2);
        }
    }

    if (u32Mask & ACE_IMR2_BC_UIRQ3)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_IRQ_3);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_IRQ_3);
        }
    }

    if (u32Mask & ACE_IMR2_BC_TRAP)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_TRAP);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_TRAP);
        }
    }

    if (u32Mask & ACE_IMR2_BC_CALLSTK_ERR)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_CALL_STACK_ERROR);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_CALL_STACK_ERROR);
        }
    }

    if (u32Mask & ACE_IMR2_BC_OPCODE_PARITY)
    {
        if (u16Enable) /* bEnable */
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_OPCODE_PARITY_ERROR);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_OPCODE_PARITY_ERROR);
        }
    }

    if (u32Mask & ACE_IMR2_GPQ_ISQ_ROVER)
    {
        if (u16Enable)
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_GP_QUEUE_ROLLOVER);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_GP_QUEUE_ROLLOVER);
        }
    }

    if (u32Mask & ACE_IMR1_BC_END_OF_FRM)
    {
        if (u16Enable)
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_SELECT_EOM);
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_SELECT_EOM);
        }
    }

    /* ------------------------------------------------------------- */
    /*                    GLOBAL INTERRUPT MASK SET                  */
    /* ------------------------------------------------------------- */

    if (u32Mask & ACE_IMR1_TT_ROVER)
    {
        if (u16Enable) /* bEnable */
        {
            gen1553InterruptSet(pDeviceContext, u8Channel, GENERAL_INT_STATUS_TT_ROLLOVER);
        }
        else
        {
            gen1553InterruptClear(pDeviceContext, u8Channel, GENERAL_INT_STATUS_TT_ROLLOVER);
        }
    }

    if (u32Mask & ACE_IMR1_RAM_PAR_ERR)
    {
        if (u16Enable) /* bEnable */
        {
            gen1553InterruptSet(pDeviceContext, u8Channel, GENERAL_INT_STATUS_RAM_PARITY_DETECTED);
        }
        else
        {
            gen1553InterruptClear(pDeviceContext, u8Channel, GENERAL_INT_STATUS_RAM_PARITY_DETECTED);
        }
    }

    if (u32Mask & ACE_IMR2_BIT_TRIGGER)
    {
        if (u16Enable)
        {
            gen1553InterruptSet(pDeviceContext, u8Channel, GENERAL_INT_MASK_TRG_INT_ENABLED);
        }
        else
        {
            gen1553InterruptClear(pDeviceContext, u8Channel, GENERAL_INT_MASK_TRG_INT_ENABLED);
        }
    }

    if (u32Mask & ACE_IMR2_BIT_COMPLETE)
    {
        if (u16Enable)
        {
            gen1553InterruptSet(pDeviceContext, u8Channel, GENERAL_INT_STATUS_RAM_SELF_TEST_DONE);
        }
        else
        {
            gen1553InterruptClear(pDeviceContext, u8Channel, GENERAL_INT_STATUS_RAM_SELF_TEST_DONE);
        }
    }

    /* ------------------------------------------------------------- */
    /*                    SHARED INTERRUPT MASK SET                  */
    /* ------------------------------------------------------------- */

    if (u32Mask & ACE_IMR1_EOM)
    {
        if (u16Enable)
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_EOM);
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_EOM);

            /* TODO: Add MT EOM */
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_EOM);
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_EOM);

            /* TODO: Add MT EOM */
        }
    }

    if (u32Mask & ACE_IMR1_FORMAT_ERR)
    {
        if (u16Enable)
        {
            bcInterruptSet(pDeviceContext, u8Channel, REG_BC_FORMAT_ERROR);
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_FORMAT_ERROR);

            /* TODO: Add MT EOM */
        }
        else
        {
            bcInterruptClear(pDeviceContext, u8Channel, REG_BC_FORMAT_ERROR);
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_FORMAT_ERROR);

            /* TODO: Add MT EOM */
        }
    }

    if (u32Mask & ACE_IMR1_BCRT_CMDSTK_ROVER)
    {
        if (u16Enable)
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER);

            /* Not supported for BC */
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER);

            /* Not supported for BC */
        }
    }

    if (u32Mask & ACE_IMR1_BCRT_TX_TIMEOUT)
    {
        if (u16Enable)
        {
            mrtInterruptSet(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_XMTR_TIMEOUT);

            /* Not supported for BC */
        }
        else
        {
            mrtInterruptClear(pDeviceContext, u8Channel, MRT_INT_ENABLE_MASK_XMTR_TIMEOUT);

            /* Not supported for BC */
        }
    }

    /* ------------------------------------------------------------- */
    /*                    MT INTERRUPT MASK SET                      */
    /* ------------------------------------------------------------- */

    /* All MT interrupt masks are processed using MTi simulation via mtMtiInterruptConfig function.
       mtMtiInterruptConfig function enables ACE_IMR2_MTI_INTERRUPT mask */

    return DDC_UDL_ERROR__SUCCESS;
}

/*-----------------------------------------------------------------------------
   Function: gen1553CheckMfCapable

   Description:
      This function checks if the channel is capable of MF concurrency. At
      present, a channel is MF capable only if the channel has one IMP
      for BC and another one for MRT.

   In  pDeviceContext     - device-specific structure
   In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     not used
            Param2:     not used
            Param3:     not used
            Param4:     not used
   Out  pBytesReturned    - bytes returned
   Out  pRdData           - rturn value

   Returns:
        NTSTATUS
   -----------------------------------------------------------------------------*/
S16BIT gen1553CheckMfCapable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];

    *pBytesReturned = sizeof(U32BIT);
    if ((pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP) ||
        (pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP))
    {
        /* not MF capable because of shared IMP */
        *pRdData = FALSE;
    }
    else
    {
        /* MF capable because of separated IMPs */
        *pRdData = TRUE;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*============================================================================*/
/*                         BUS Coupling and Amplitude                         */
/*============================================================================*/

/*******************************************************************************
 * Name:    gen1553GetCoupling
 *
 * Description:
 *      This function gets the coupling and termination for a particular
 *      channel. It is assumed that Bus A & B have been set the same.
 *
 * In   pDeviceContext  device-specific structure
 * In   pCoupling       pointer to coupling structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void gen1553GetCoupling
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    ACEX_COUPLING *pCoupling
)
{
    U32BIT dwData = 0;
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    U16BIT u16SingleFpgaCh = u16Ch;

    /* On dual FPGA channels 2 and 3 coupling configuration is on the 2nd FPGA */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_COUPLING_TERM), &dwData);
    u16SingleFpgaCh = (U16BIT)(u16SingleFpgaCh - (pDeviceContext->sBoardInstanceIndex[u16Ch] << 1));

    pCoupling->coupling = ACEX_GET_CH_COUPLING(u16SingleFpgaCh, dwData);
    pCoupling->termination = ACEX_GET_CH_TERMINATION(u16SingleFpgaCh, dwData);
}

/*******************************************************************************
 * Name:    gen1553SetCoupling
 *
 * Description:
 *      This function sets the coupling and termination for a particular
 *      channel. It is assumed that Bus A & B will be set the same.
 *
 * In   pDeviceContext  device-specific structure
 * In   pCoupling       pointer to coupling structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void gen1553SetCoupling
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT dwData = 0;
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    U32BIT u32Coupling;
    U32BIT u32Termination;
    U16BIT u16SingleFpgaCh = u16Ch;

    u32Coupling = DDC_IOCTL_U32(pIoctlParams->Param1);
    u32Termination = DDC_IOCTL_U32(pIoctlParams->Param2);

    /* On dual FPGA channels 2 and 3 coupling configuration is on the 2nd FPGA */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_COUPLING_TERM), &dwData);
    u16SingleFpgaCh = (U16BIT)(u16SingleFpgaCh - (pDeviceContext->sBoardInstanceIndex[u16Ch] << 1));

    /* clear out the coupling and termination for the desired channel */
    dwData &= ACEX_CLEAR_CH_COUPLING_TERM(u16SingleFpgaCh);

    dwData |= ACEX_SET_CH_COUPLING(u16SingleFpgaCh, u32Coupling);
    dwData |= ACEX_SET_CH_TERMINATION(u16SingleFpgaCh, u32Termination);

    /* write the value back */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_COUPLING_TERM), &dwData);
}

/*******************************************************************************
 * Name:    GetAmplitude
 *
 * Description:
 *      This function gets the amplitude for a particular channel.
 *
 * In   pDeviceContext  pointer to device context
 * In   pAmplitude      pointer to amplitude structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void gen1553GetAmplitude
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pAmplitude
)
{
    U32BIT dwData = 0;
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);

    /* On dual FPGA channels 2 and 3 coupling configuration is on the 2nd FPGA */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_VAR_AMP_XCVR), &dwData);
    *pAmplitude = ACEX_GET_CH_AMPLITUDE(u16Ch, dwData);
}

/*******************************************************************************
 * Name:    SetAmplitude
 *
 * Description:
 *      This function sets the amplitude for a particular channel.
 *
 *      BIT     DESCRIPTION
 *      -----------------------------------------
 *      27:18    1553 Channel_2 Transmit Amplitude
 *      11:2    1553 Channel_1 Transmit Amplitude
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to IOCTL structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void gen1553SetAmplitude
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT dwData = 0;
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    U32BIT u32Amplitude;

    u32Amplitude = DDC_IOCTL_U32(pIoctlParams->Param1);

    /* On dual FPGA channels 2 and 3 coupling configuration is on the 2nd FPGA */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_VAR_AMP_XCVR), &dwData);

    /* first clear out the amplitude for the desired channel */
    dwData &= ACEX_CLEAR_CH_AMPLITUDE(u16Ch);

    dwData |= ACEX_SET_CH_AMPLITUDE(u16Ch, u32Amplitude);

    /* write the value back */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[pDeviceContext->sBoardInstanceIndex[u16Ch]]->pu32RegBA) + REG_BD_CH1_2_1553_VAR_AMP_XCVR), &dwData);
}

