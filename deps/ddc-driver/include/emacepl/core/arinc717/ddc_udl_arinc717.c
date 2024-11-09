/*******************************************************************************
 * FILE: ddc_udl_arinc717.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to provide functions to support ARINC 717.
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
#include "os/include/ddc_os_private.h"
#include "include/ddc_error_list.h"
#include "include/ddc_device_ids.h"
#include "include/ddc_arinc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/arinc717/ddc_udl_arinc717_private.h"


/*******************************************************************************
 * Name:    ARINC717Initialize
 *
 * Description:
 *      This function initializes all the ARINC 717 data structures with the
 *      base register and memory locations for the various ARINC 717 components,
 *      Removes channels off the bus
 *
 * In   pDeviceContext          device-specific structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT arinc717Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT numDevicesIndex;
    size_t i, j;
    UM_DEVICE_INFO       *pUmDevicePtr;
    U32BIT u32RegisterValue;
    ENHANCED_CAPABILITY_INFO sCapabilityInfo;

    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - NumProg:%d\n", pDeviceContext->u8NumProg717);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - NumTx:%d\n", pDeviceContext->u8NumDed717Tx);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - NumRx:%d\n", pDeviceContext->u8NumDed717Rx);

    /* Initialize all members to zero */
    for (i = 0; i < MAX_NUM_717_PROG_CHANNELS; i++)
    {
        memset(&(pDeviceContext->sChannelArinc717[i]), 0, sizeof(ARINC_717_PROG_TYPE));
        pDeviceContext->sChannelArinc717[i].state = ACEX_MOD_RESET;
    }

    memset(&(pDeviceContext->sArincGlobal717), 0, sizeof(ARINC_717_PROG_TYPE));

    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        if ((pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_717_GLOBAL) ||
            (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_717_RX_TX))
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - DeviceIndex:%d, DeviceType: 0x%04X, NumberInstances: %d, NumComponents: %d\n",
                numDevicesIndex, pUmDevicePtr->umDevType,
                pUmDevicePtr->umDevNumInstances, pUmDevicePtr->umDevNumComponents);

            for (j = 0; j < pUmDevicePtr->umDevNumComponents; j++)
            {
                switch (pUmDevicePtr->umComponentInfo[j].umComponentType)
                {
                    case UM_COMPONENTS_ID_ARINC_717_PROG_CH_GLOBAL:
                    {
                        pDeviceContext->sArincGlobal717.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[0]);
                        pDeviceContext->sArincGlobal717.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                        pDeviceContext->sArincGlobal717.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[0]);
                        pDeviceContext->sArincGlobal717.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - Global Register: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArincGlobal717.pu32RegBA),
                            *(pDeviceContext->sArincGlobal717.pu32RegSize));

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - Global Memory: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArincGlobal717.pu32MemBA),
                            *(pDeviceContext->sArincGlobal717.pu32MemSize));
                        break;
                    }

                    case UM_COMPONENTS_ID_ARINC_717_PROG_CH:
                    {
                        /* During initialization phase, all channels will be undefined */
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sChannelArinc717[i].pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sChannelArinc717[i].pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sChannelArinc717[i].pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sChannelArinc717[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - Register BA:%08X  Size:%d\n",
                                *(pDeviceContext->sChannelArinc717[i].pu32RegBA),
                                *(pDeviceContext->sChannelArinc717[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "717 - Memory BA:%08X  Size:%d\n",
                                *(pDeviceContext->sChannelArinc717[i].pu32MemBA),
                                *(pDeviceContext->sChannelArinc717[i].pu32MemSize));
                        }
                        break;
                    }

                    default:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INITIALIZE, "Not a 717 Component\n");
                        break;
                    }
                } /* switch */
            } /* for components */
        } /* if umDevType */
    } /* for numDevicesIndex */

    /* Retrieve channel count */
    ddcUdlBdReadEnhancedCapabilities(pDeviceContext, &sCapabilityInfo);

    if (sCapabilityInfo.channelCount717Prog != 0)
    {
        /* Insure all ARINC 717 channels disconnected from bus interface */
        u32RegisterValue = 0;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P717_RELAY, &u32RegisterValue);

        /* Enable 717 board level interrupt */
        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_ARINC_717);

        /* Enable Global ARINC 717 interrupts */
        u32RegisterValue = REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_CONFIG, &u32RegisterValue);
    }

    /* Initialize each channel configuration options */
    for (i = 0; i < sCapabilityInfo.channelCount717Prog; i++)
    {
        /* Default 717 programmable channel configuration */
        u32RegisterValue = ARINC_717_PROG_SLOPE_RATE_398PF_MASK |             /* tx slope rate of 398pf */
            ARINC_717_PROG_SPEED_64WPS_MASK |                                 /* speed of 64 words/second */
            ARINC_717_PROG_BPRZ_HBP_SELECT_MASK |                             /* BPRZ mode */
            ARINC_717_PROG_BUFFER_MODE_DOUBLE_MASK |                          /* Rx & Tx use double buffer mode */
            ARINC_717_PROG_RESET_MASK;                                        /* Reset all protocols */

        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_ENABLE_MASK;              /* tx & rx disabled */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_CHANNEL_MODE_MASK;        /* default as an Rx */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK;     /* Disable internal loop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_EXT_WRAP_AROUND_MASK;     /* Disable external loop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_TX_STOP_MASK;             /* Remove transmitter from stop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_RX_AUTO_DETECT_MASK;

        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[i].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        /* Take 717 engine out of reset */
        u32RegisterValue &= ~ARINC_717_PROG_RESET_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[i].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        /* Set Frame count to continuous */
        u32RegisterValue = 0;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[i].pu32RegBA) + REG_ARINC_717_CH_TX_FRAME_COUNT, &u32RegisterValue);

        /* Initialize state machines */
        pDeviceContext->eArinc717ProgState[i] = ARINC_717_RESET;
        pDeviceContext->eArinc717ProgChState[i] = ARINC_717_UNDEFINED;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8Speed = ARINC_717_PROG_SPEED_64_WPS;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8BufMode = ARINC_717_PROG_BUF_MODE_DOUBLE;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8SlopeRate = ARINC_717_PROG_TX_SLOPE_CONTROL_68PF;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8Protocol = ARINC_717_PROG_BPRZ_MASK;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8RxAutoDetect = TRUE;

        /* Channel type defaults to Rx, but for processing purpose it will be noted as undefined */
        pDeviceContext->sArinc717ProgChMemConfig[i].u8ChannelType = ARINC717_UNDEFINED_CHANNEL;
        pDeviceContext->sArinc717ProgChMemConfig[i].u8WrapAroundMode = ARINC_717_PROG_WRAP_NONE;
        pDeviceContext->sArinc717ProgChMemConfig[i].u16FrameCount = 0;
    }

    if (sCapabilityInfo.channelCount717Prog != 0)
    {
        /* Disable all channels mode interrutps */
        u32RegisterValue = 0;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_CH_INT_ENABLE, &u32RegisterValue);

        u32RegisterValue = REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE;

        /* Enable all ARINC 717 channel interrupts. Excludes mode interrupts */
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_CONFIG, &u32RegisterValue);
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlARINC717ChannelCleanup
 *
 * Description:
 *      This function free all the ARINC 717 resources.
 *
 * In   pDeviceContext          device-specific structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ddcUdlARINC717ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
#if DDC_DMA_717
    {
        U8BIT i;

        for (i = 0; i < pDeviceContext->u8NumProg717; i++)
        {
            if (pDeviceContext->sChannelArinc717[i].pu8MemoryBuf != NULL)
            {
                DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->sChannelArinc717[i].pu8MemoryBuf);
                pDeviceContext->sChannelArinc717[i].pu8MemoryBuf = NULL;
            }
        }
    }
#endif /* DDC_DMA_717 */

    /* DD-40000 compatible devices only */
    {
#if DDC_DMA_717
        U8BIT i;

        /* free ARINC 717 RX DMA buffer */
        if (pDeviceContext->ARINC717RxDMATarget)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pDeviceContext->ARINC717RxDMA_Size,
                pDeviceContext->ARINC717RxDMATarget,
                pDeviceContext->ARINC717RxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_717_RX_DMA);

            pDeviceContext->ARINC717RxDMATarget = NULL;
        }

        /* free ARINC 717 buffer */
        for (i=0; i < pDeviceContext->u8NumProg717; i++)
        {
            /* free memory buffer */
            if (pDeviceContext->sChannelArinc717[i].pu8MemoryBuf)
            {
                DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->sChannelArinc717[i].pu8MemoryBuf);
                pDeviceContext->sChannelArinc717[i].pu8MemoryBuf = NULL;
            }
        }

        /* free ARINC 717 TX DMA buffer */
        if (pDeviceContext->ARINC717TxDMATarget)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pDeviceContext->ARINC717TxDMA_Size,
                pDeviceContext->ARINC717TxDMATarget,
                pDeviceContext->ARINC717TxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_717_TX_DMA);

            pDeviceContext->ARINC717TxDMATarget = NULL;
        }
#endif /* DDC_DMA_717 */
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ARINC717Interrupts
 *
 * Description:
 *      This function turns on the desired interrupts for ARINC 717 or
 *      retrieves current interrupt setting; depending on input parameter.
 *
 * In   pDeviceContext  Device context structure pointer
 * In   pConfig         ARINC 717 programmable configuration structure
 * Out  *pRdData        Current interrupt settings
 * In   bConfig         if TRUE overrides state machine check
 *
 * Returns: Current interrupt settings
 ******************************************************************************/
S16BIT arinc717Interrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig,
    U32BIT *pRdData,
    U8BIT bConfig
)
{
    U32BIT u32RegisterValue = 0;
    U8BIT u8Channel = (U8BIT)pConfig->u8Channel;

    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumProg717) || (u8Channel == 0))
    {
        return DDC_UDL_ERROR__ARINC_717_INVALID_CHANNEL;
    }

    if (pRdData == NULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INTERRUPT, "arinc717Interrupts Start - Ch %d, pRdData = NULL.\n", pConfig->u8Channel);
        return DDC_UDL_ERROR__NULL_PTR;
    }

    u8Channel--;

    if ((pDeviceContext->eArinc717ProgState[u8Channel] == ARINC_717_RESET) && (bConfig == FALSE))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INTERRUPT, "arinc717Interrupts - Ch %d, Invalid state for operation.\n", pConfig->u8Channel);
        return DDC_UDL_ERROR__ARINC_717_INVALID_STATE;
    }

    /* For arrary assignments, channel number is base 0 */

    /* Read current channel interrupt status */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_INT_ENABLE,
        &u32RegisterValue);

    /* 1st parameter in structure determines a get or set; if TRUE, set desired interrupts */
    if (pConfig->bGetInterrupt == FALSE)
    {
        /* 2nd parameter in structure determines if enablinlg or disabling interrupts; if TRUE, enalbe desired interrupts */
        if (pConfig->bEnableInterrupt == TRUE)
        {
            /* Mask out reserved bits prior to enabling desired interrupts. 3rd parameter in structure */
            u32RegisterValue |= (pConfig->u32Interrupts & (U32BIT)ARINC_717_PROG_INT_NOT_USED_ENA);
        }
        else
        {
            /* Mask out reserved bits prior to disabling desired interrupts. 3rd parameter in structure */
            u32RegisterValue &= ~(pConfig->u32Interrupts & (U32BIT)ARINC_717_PROG_INT_NOT_USED_ENA);
        }

        /* Note, channel number passed to this function must be 0 based.  */
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_INT_ENABLE, &u32RegisterValue);
    }

    *pRdData = u32RegisterValue;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_INTERRUPT, "arinc717Interrupts Start  - Channel %d Status: 0x%08X\n", pConfig->u8Channel, *pRdData);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    arinc717ProgrammableOpen
 *
 * Description:
 *      This function configures all desired parameters of the ARINC 717
 *      engine.
 *
 * In   pDeviceContext    - device-specific structure
 * In   pConfig           - will contain configuration parameters
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT arinc717ProgrammableOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
)
{
    S16BIT u32Status = DDC_UDL_ERROR__SUCCESS;
    U8BIT u8Channel = pConfig->u8Channel;
    U32BIT u32RegisterValue = 0;
    U32BIT u32RegisterValue_2;
    U32BIT u32Dummy;

    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumProg717) || (u8Channel == 0))
    {
        return DDC_UDL_ERROR__ARINC_717_INVALID_CHANNEL;
    }

    /* Deduct 1 from channel number due to arrrays are 0 based */
    u8Channel--;

    if ((pDeviceContext->sChannelArinc717[u8Channel].state == ACEX_MOD_OPEN) &&
        ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_RESET_OPT) != ARINC_717_PROGRMMABLE_RESET_OPT))
    {
        /* we do not want to open again if already opened, unless a channel reset is requested */
        return DDC_UDL_ERROR__STATE;
    }

    /* Read current configuration */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG,
        &u32RegisterValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "arinc717ProgrammableOpen - Current config register value: %08X\n", u32RegisterValue);

    /* Channel type must always be set and come 1st; other parameters dependent on type */
    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_TYPE_OPT) == ARINC_717_PROGRMMABLE_TYPE_OPT)
    {
        /* Transmitter/Receiver selection */
        if (pConfig->u8Type == ARINC717_TX_CHANNEL)
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8ChannelType = pConfig->u8Type;
            u32RegisterValue |= (U32BIT)ARINC_717_PROG_CHANNEL_MODE_MASK;
        }
        else if (pConfig->u8Type == ARINC717_RX_CHANNEL)
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8ChannelType = pConfig->u8Type;
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_CHANNEL_MODE_MASK;
        }
        else /* defaults to Rx, but will denoted it as ARINC717_UNDEFINED_CHANNEL (Undefined) if above not satisfied */
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8ChannelType = ARINC717_UNDEFINED_CHANNEL;
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_CHANNEL_MODE_MASK;
        }
    }
    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_SLOPE_OPT) == ARINC_717_PROGRMMABLE_SLOPE_OPT)
    {
        /* 1st reset appropreiate bit locations 0 */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_SLOPE_RATE_MASK;

        /* Transmitter slope rate selection */
        switch (pConfig->u8SlopeControl)
        {
            case ARINC_717_PROG_TX_SLOPE_CONTROL_68PF:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8SlopeRate = pConfig->u8SlopeControl;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SLOPE_RATE_65PF_MASK;
                break;
            }
            case ARINC_717_PROG_TX_SLOPE_CONTROL_398PF:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8SlopeRate = pConfig->u8SlopeControl;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SLOPE_RATE_398PF_MASK;
                break;
            }
            case ARINC_717_PROG_TX_SLOPE_CONTROL_538PF:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8SlopeRate = pConfig->u8SlopeControl;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SLOPE_RATE_538PF_MASK;
                break;
            }
            case ARINC_717_PROG_TX_SLOPE_CONTROL_868PF:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8SlopeRate = pConfig->u8SlopeControl;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SLOPE_RATE_868PF_MASK;
                break;
            }
            default:
            {
                return DDC_UDL_ERROR__ARINC_717_INVALID_RATE_SLOPE;
            }
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_WRAP_AROUND_OPT) == ARINC_717_PROGRMMABLE_WRAP_AROUND_OPT)
    {
        /* 1st reset appropreiate bit locations 0 */
        u32RegisterValue &= ~((U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK | ARINC_717_PROG_EXT_WRAP_AROUND_MASK);

        switch (pConfig->u8WrapAroundMode)
        {
            case ARINC_717_PROG_EXT_WRAP_AROUND:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8WrapAroundMode = pConfig->u8WrapAroundMode;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_EXT_WRAP_AROUND_MASK;
                u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK;
                break;
            }
            case ARINC_717_PROG_INT_WRAP_AROUND:
            {
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8WrapAroundMode = pConfig->u8WrapAroundMode;
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK;
                u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_EXT_WRAP_AROUND_MASK;
                break;
            }
            case ARINC_717_PROG_WRAP_NONE:
            {
                /* No action required do due initialized to none, above */
                u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK;
                u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_EXT_WRAP_AROUND_MASK;
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8WrapAroundMode = ARINC_717_PROG_WRAP_NONE;
                break;
            }
            default:
            {
                return DDC_UDL_ERROR__ARINC_717_INVALID_WRAP_AROUND;
            }
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_STOP_OPT) == ARINC_717_PROGRMMABLE_STOP_OPT)
    {
        /* Stop transmitter operation set in set run state function; do nothing */
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_AUTO_DETECT_OPT) == ARINC_717_PROGRMMABLE_AUTO_DETECT_OPT)
    {
        /* Enable Receiver's auto detection of transmit speed */
        if (pConfig->bRxAutoDetect == TRUE)
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8RxAutoDetect = TRUE;
            u32RegisterValue |= (U32BIT)ARINC_717_PROG_RX_AUTO_DETECT_MASK;
        }
        else
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8RxAutoDetect = FALSE;
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_RX_AUTO_DETECT_MASK;
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_PROTOCOL_OPT) == ARINC_717_PROGRMMABLE_PROTOCOL_OPT)
    {
        /* Place Tx and Rx in Bipolar Return to Zero protocol mode */
        if (pConfig->u8ProtocolType == ARINC_717_PROG_BPRZ_MASK)
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Protocol = pConfig->u8ProtocolType;
            u32RegisterValue |= (U32BIT)ARINC_717_PROG_BPRZ_HBP_SELECT_MASK;
        }

        /* Place Tx and Rx in Harvard BiPhase protocol mode */
        else if (pConfig->u8ProtocolType == ARINC_717_PROG_HBP_MASK)
        {
            pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Protocol = pConfig->u8ProtocolType;
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_BPRZ_HBP_SELECT_MASK;
        }
        else
        {
            return DDC_UDL_ERROR__ARINC_717_INVALID_PROTOCOL_MODE;
        }
    }

    /* Buffer mode must come prior to speed setting; memory configuration dependent on speed and buffer mode */
    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_BUFFER_MODE_OPT) == ARINC_717_PROGRMMABLE_BUFFER_MODE_OPT)
    {
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_BUFFER_MODE_MASK;

        switch (pConfig->u8BufferMode)
        {
            case ARINC_717_PROG_BUF_MODE_SINGLE:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_BUFFER_MODE_SINGLE_MASK;
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8BufMode = pConfig->u8BufferMode;
                break;
            }
            case ARINC_717_PROG_BUF_MODE_DOUBLE:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_BUFFER_MODE_DOUBLE_MASK;
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8BufMode = pConfig->u8BufferMode;
                break;
            }
            case ARINC_717_PROG_BUF_MODE_CIRCULAR:
            {
                /* Circular mode not available for transmitter mode channels */
                if (pConfig->u8Type == ARINC717_TX_CHANNEL)
                {
                    return DDC_UDL_ERROR__ARINC_717_INVALID_BUFFER_MODE;
                }
                else
                {
                    pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8BufMode = pConfig->u8BufferMode;
                    u32RegisterValue |= (U32BIT)ARINC_717_PROG_BUFFER_MODE_CIRCULAR_MASK;
                }
                break;
            }
            default:
            {
                return DDC_UDL_ERROR__ARINC_717_INVALID_BUFFER_MODE;
            }
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_SPEED_OPT) == ARINC_717_PROGRMMABLE_SPEED_OPT)
    {
        /* Reset appropriate bits to 0 1st */
        u32RegisterValue &= ~(U32BIT)((U32BIT)ARINC_717_PROG_SPEED_MASK << ARINC_717_PROG_SPEED_SHIFT);

        /* Speed selection */
        switch (pConfig->u8Speed)
        {
            case ARINC_717_PROG_SPEED_32_WPS:
            {
                /* NOTE: The speed is actually 64 words per second. The transmitter will transmit word
                 *  padded with zero for odd word count data. The receiver will not record odd word count
                 *  data transmitted data */

                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_32WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_32_WPS;
                break;
            }
            case ARINC_717_PROG_SPEED_64_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_64WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_64_WPS;
                break;
            }
            case ARINC_717_PROG_SPEED_128_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_128WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_128_WPS;
                break;
            }
            case ARINC_717_PROG_SPEED_256_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_256WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_256WPS_MASK;
                break;
            }
            case ARINC_717_PROG_SPEED_512_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_512WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_512WPS_MASK;
                break;
            }
            case ARINC_717_PROG_SPEED_1024_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_1024WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_1024WPS_MASK;
                break;
            }
            case ARINC_717_PROG_SPEED_2048_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_2048WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_2048_WPS;
                break;
            }
            case ARINC_717_PROG_SPEED_4096_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_4096WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_4096_WPS;
                break;
            }
            case ARINC_717_PROG_SPEED_8192_WPS:
            {
                u32RegisterValue |= (U32BIT)ARINC_717_PROG_SPEED_8192WPS_MASK;

                /* Save value in global device storage */
                pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_8192_WPS;
                break;
            }
            default:    /* For invalid speed, exit with error condition */
            {
                return DDC_UDL_ERROR__ARINC_717_INVALID_SPEED;
            }
        } /* switch (pConfig->u8Speed) */
    }

    if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_FRAME_COUNT_OPT) == ARINC_717_PROGRMMABLE_FRAME_COUNT_OPT)
    {
        /* Set Frame count */
        pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u16FrameCount = pConfig->u16FrameCount;
        u32RegisterValue_2 = pConfig->u16FrameCount;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_TX_FRAME_COUNT, &u32RegisterValue_2);
    }

    /* Check for Reset option last due to priority is highest and overrides all other configuration options */
    if (((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_RESET_OPT) == ARINC_717_PROGRMMABLE_RESET_OPT) &&
        (pConfig->bReset == TRUE))
    {
        /* If reset was one of the above configuration options, all above configurations will be ignore and
         * the following default place will be in place. Channel will be still connected to interface bus if
         * it was previously enabled */

        /* Reset Channel
         *   Causes all channel interrupts to clear
         *   Configuration set to the following default values
         *   - Slope rate set to 68Pf
         *   - All wrap around modes disabled
         *   - Tx stop disabled
         *   - Rx speed auto detect disabled
         *   - Tx/Rx in single buffer mode
         *   - Tx/Rx in Harvard Bi phase mode
         *   - Channel default to Rx mode
         *   - Channel is disabled
         *   - Tx/Rx speed set to 32 words per second
         */
        u32RegisterValue = (U32BIT)ARINC_717_PROG_RESET_MASK; /* Insure Channel is also disabled */

        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        /* Reset not self clearing, therefore clear it after setting it */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_RESET_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        /* Default 717 programmable channel configuration */
        u32RegisterValue = ARINC_717_PROG_SLOPE_RATE_398PF_MASK |             /* tx slope rate of 398pf */
            ARINC_717_PROG_SPEED_64WPS_MASK |                                 /* speed of 64 words/second */
            ARINC_717_PROG_BPRZ_HBP_SELECT_MASK |                             /* BPRZ mode */
            ARINC_717_PROG_BUFFER_MODE_DOUBLE_MASK;                           /* Rx & Tx use double buffer mode */

        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_ENABLE_MASK;              /* tx & rx disabled */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_CHANNEL_MODE_MASK;        /* default as an Rx */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_INT_WRAP_AROUND_MASK;     /* Disable internal loop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_EXT_WRAP_AROUND_MASK;     /* Disable external loop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_TX_STOP_MASK;             /* Remove transmitter from stop mode */
        u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_RX_AUTO_DETECT_MASK;

        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        /* Initialize state machine */
        pDeviceContext->eArinc717ProgState[u8Channel] = ARINC_717_RESET;
        pDeviceContext->eArinc717ProgChState[u8Channel] = ARINC_717_UNDEFINED;
        pDeviceContext->sChannelArinc717[u8Channel].state = ACEX_MOD_RESET;
        pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed = ARINC_717_PROG_SPEED_64_WPS;

        /* Set Frame count to continuous */
        pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u16FrameCount = 0;
        u32RegisterValue = 0;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_TX_FRAME_COUNT, &u32RegisterValue);
    }
    else
    {
        /* Set new configurations */
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "arinc717ProgrammableOpen - Current config register value: %08X\n", u32RegisterValue);

        if ((pConfig->u32ConfigOption & ARINC_717_PROGRMMABLE_TYPE_OPT) == ARINC_717_PROGRMMABLE_TYPE_OPT)
        {
            /* Read current bus interface configuration */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P717_RELAY, &u32RegisterValue);

            /* Connect desired channel to bus interface */
            u32RegisterValue |= ((U32BIT)ARINC_717_PROGRAMMABLE_BUS_ISOLATION_ENABLE << u8Channel);
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P717_RELAY, &u32RegisterValue);

            /* disable 429 for this channel as it is shared with 717 */
            if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) || (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
                (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX) ||
                (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H))
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX_2, &u32RegisterValue);
                u32RegisterValue &= ~(ARINC_429_PROGRAMMABLE2_CH1_TX << u8Channel);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX_2, &u32RegisterValue);
            }

            if (pConfig->u8Type == ARINC717_TX_CHANNEL)
            {
                /* Tx Marker 0 & 1 interrupts must always be set if channel set as transmitter.
                 * Tx Marker 0 interrupt occurs when tx protocol engine reads 1st location of the 1st buffer
                 * and Tx Marker 1 interrupt occures when tx protocol engine read 1st location of second buffer.
                 * In Double buffer mode, their are only 2 buffers, 1st & 2nd.  Interrtupt ping pongs between them.
                 * In Single Buffer mode, Tx Marker 0 occurs protocol engine reads 1st location of the buffer and
                 * Tx Marker 1 occurs when protocol engine reads mid-point location of the buffer. */

                pConfig->bGetInterrupt = FALSE; /* Set interrupts */
                pConfig->bEnableInterrupt = TRUE; /* Enable interrupts */
                pConfig->u32Interrupts = ARINC_717_PROG_INT_TX_MARKER0_ENA | ARINC_717_PROG_INT_TX_MARKER1_ENA; /* Interrupts */
                arinc717Interrupts(pDeviceContext, pConfig, &u32Dummy, TRUE);

                /* Enable Global level Channel interrupt */
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);

                u32RegisterValue |= ((U32BIT)ARINC_717_PROGRAMMABLE_GLOBAL_CH_INT_ENABLE << u8Channel);

                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);
            }
            else if (pConfig->u8Type == ARINC717_RX_CHANNEL)
            {
                /* Received sub-frame interrupts must always be set if channel set as receiver.
                 * Half sub-frame when receiver interrupt occurs when rx protocol engine receives word on a mid-frame.
                 * Sub-frame interrupt occures when rx protocol engine receiver last word of a sub-frame.
                 * In Double buffer mode, their are only 2 buffers, 1st & 2nd.  Interrtupt ping pongs between them.
                 *  */

                pConfig->bGetInterrupt = FALSE; /* Set interrupts */
                pConfig->bEnableInterrupt = TRUE; /* Enable interrupts */

                /* In double buffer mode, ARINC_717_PROG_INT_RX_50_PC_MEM_ENA signifies when Firmware has
                 *  completed writing received data to primary buffer and ARINC_717_PROG_INT_RX_100_PC_MEM_ENA sginifies
                 *  when Firmware has has completed writing received data to secondary buffer. */
                pConfig->u32Interrupts = ARINC_717_PROG_INT_RX_50_PC_MEM_ENA | ARINC_717_PROG_INT_RX_100_PC_MEM_ENA;
                arinc717Interrupts(pDeviceContext, pConfig, &u32Dummy, TRUE);

                /* Enable Global level Channel interrupt */
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);

                u32RegisterValue |= ((U32BIT)ARINC_717_PROGRAMMABLE_GLOBAL_CH_INT_ENABLE << u8Channel);

                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);
            }
        }
        pDeviceContext->sChannelArinc717[u8Channel].state = ACEX_MOD_OPEN;
        pDeviceContext->eArinc717ProgState[u8Channel] = ARINC_717_READY;
    }

    return u32Status;
}

/*******************************************************************************
 * Name:    arinc717ProgrammableClose
 *
 * Description:
 *      This function closes operations for the desired ARINC 717 channel
 *      engine.
 *
 * In   pDeviceContext    - device-specific structure
 * In   U16BIT u16Channel - desired ARINC 717 channel to close
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT arinc717ProgrammableClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
)
{
    S16BIT u32Status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32RegisterValue = 0;
    U8BIT u8Channel = pConfig->u8Channel;

    /* Max channel check */
    if (u8Channel > pDeviceContext->u8NumProg717)
    {
        return DDC_UDL_ERROR__ARINC_717_INVALID_CHANNEL;
    }

    /* Deduct 1 from channel number due to arrrays are 0 based */
    u8Channel--;

    /* Disconnect channel from bus interface */

    /* Read current bus configuration */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_BD_P717_RELAY, &u32RegisterValue);
    u32RegisterValue &= ~((U32BIT)ARINC_717_PROGRAMMABLE_BUS_ISOLATION_ENABLE >> u8Channel);
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P717_RELAY, &u32RegisterValue);

    /* Disable Global level Channel interrupt */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);

    u32RegisterValue &= ((U32BIT)ARINC_717_PROGRAMMABLE_GLOBAL_CH_INT_ENABLE >> u8Channel);

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_ENABLE, &u32RegisterValue);

    /* Read current configuration */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG,
        &u32RegisterValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CLOSE, "arinc717ProgrammableClose - Channel %d Config register value: %08X\n", (u8Channel + 1), u32RegisterValue);

    /* Reset Channel
     *   Causes all channel interrupts to clear
     *   Configuration set to the following default values
     *   - Slope rate set to 68Pf
     *   - All wrap around modes disabled
     *   - Tx stop disabled
     *   - Rx speed auto detect disabled
     *   - Tx/Rx in single buffer mode
     *   - Tx/Rx in Harvard Bi phase mode
     *   - Channel default to Rx mode
     *   - Channel is disabled
     *   - Tx/Rx speed set to 64 words per second
     */
    u32RegisterValue |= (U32BIT)ARINC_717_PROG_RESET_MASK;
    u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_ENABLE_MASK;  /* Insure Channel is also disabled */

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

    /* Reset not self clearing, therefore clear it after setting it */
    u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_RESET_MASK;
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

    /* Initialize state machines */
    pDeviceContext->eArinc717ProgState[u8Channel] = ARINC_717_RESET;
    pDeviceContext->eArinc717ProgChState[u8Channel] = ARINC_717_UNDEFINED;
    pDeviceContext->sChannelArinc717[u8Channel].state = ACEX_MOD_CLOSED;

    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CLOSE, "arinc717ProgrammableClose - New Config register value: %08X\n", u32RegisterValue);

    /* Set Frame count to continuous */
    pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u16FrameCount = 0;
    u32RegisterValue = 0;
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_TX_FRAME_COUNT, &u32RegisterValue);

    return u32Status;
}

/*******************************************************************************
 * Name:    arinc717ProgrammableSetState
 *
 * Description:
 *      Sets The State Of The ARINC 717 Engine (RESET, READY, RUN).
 *
 * In   pDeviceContext  Device context structure pointer
 * In   pConfig         ARINC 717 programmable configuration structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT arinc717ProgrammableSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
)
{
    U8BIT u8Channel = (U8BIT)pConfig->u8Channel;
    U32BIT u32RegisterValue = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "arinc717ProgrammableSetState - Begin, State = %d, Channel = %d\n", pConfig->eState, pConfig->u8Channel);

    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumProg717) || (u8Channel == 0))
    {
        return DDC_UDL_ERROR__ARINC_717_INVALID_CHANNEL;
    }

    /* Register and status are 0 based arrays; adjust accordingly */
    u8Channel--;

    /* Read current configuration */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "ch%d: set state %d, read reg %08x, value = 0x%08x\n",
        u8Channel, pConfig->eState, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, u32RegisterValue);

    switch (pConfig->eState)
    {
        case ARINC_717_RUN:
        {
            /* Enable Transmitter/Receiver operations */
            u32RegisterValue |= (U32BIT)ARINC_717_PROG_ENABLE_MASK;
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_TX_STOP_MASK; /* insure Tx Stop not enabled */

            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "ch%d: ARINC_717_RUN, set state %d, write reg %08x, value = 0x%08x\n",
                u8Channel, pConfig->eState, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, u32RegisterValue);

            /* Set new configurations */
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

            /* Save State */
            pDeviceContext->eArinc717ProgState[u8Channel] = pConfig->eState;
            break;
        }

        case ARINC_717_PAUSE:
        {
            /* Gracefully Pause Transmitter operations */
            u32RegisterValue |= (U32BIT)ARINC_717_PROG_TX_STOP_MASK;

            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "ch%d: ARINC_717_PAUSE, set state %d, write reg %08x, value = 0x%08x\n",
                u8Channel, pConfig->eState, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, u32RegisterValue);

            /* Set new configurations */
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

            pDeviceContext->eArinc717ProgState[u8Channel] = pConfig->eState;
            break;
        }

        case ARINC_717_READY:
        case ARINC_717_RESET:
        {
            /* Disable Transmitter/Receiver operations */
            u32RegisterValue &= ~(U32BIT)ARINC_717_PROG_ENABLE_MASK;

            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "ch%d: ARINC_717_READY-RESET, set state %d, write reg %08x, value = 0x%08x\n",
                u8Channel, pConfig->eState, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, u32RegisterValue);

            /* Set new configurations */
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sChannelArinc717[u8Channel].pu32RegBA) + REG_ARINC_717_CH_CONFIG, &u32RegisterValue);

            pDeviceContext->eArinc717ProgState[u8Channel] = pConfig->eState;
            break;
        }

        default:
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "arinc717ProgrammableSetState: - Unknown ARINC 717 State Set\n");
            break;
        }
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_CONFIG, "arinc717ProgrammableSetState - End\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ARINC717LoadTxQueueData
 *
 * Description:
 *      This function loads transmitter buffer queue with data to be transmitted
 *      in bulk transfers
 *
 * In   pDeviceContext          device-specific structure
 * In   pIoctlParams            Transmission parameters
 * In   pWrData                 Raw Data to be transmitted
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ARINC717LoadTxQueueData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
)
{
    U32BIT u32AddrBaseAddress = 0;
    U32BIT u32DWords = 0;
    U8BIT u8Channel = (U8BIT)pIoctlParams->Channel;
    U32BIT u32Option = DDC_IOCTL_U32(pIoctlParams->Param2);

    if (pDeviceContext->eArinc717ProgState[u8Channel] == ARINC_717_RESET)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_TX_LOAD, "ARINC717LoadTxQueueData - Ch %d, Invalid state for operation.\n",
            pIoctlParams->Channel);
        return DDC_UDL_ERROR__ARINC_717_TX_QUEUE;
    }

    /* Max bulk transmit size for USB is 4K bytes, therefore for larger bulk transmits must in iterations
     * depending on ARINC 717 speed setting */
    switch (pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed)
    {
        case ARINC_717_PROG_SPEED_32_WPS: /* Sub-frame size = 32 DWs*/
        {
            u32DWords = 32;
            break;
        }

        case ARINC_717_PROG_SPEED_64_WPS: /* Sub-frame size = 32 DWs*/
        {
            u32DWords = 32;
            break;
        }

        case ARINC_717_PROG_SPEED_128_WPS: /* Sub-frame size = 64 DWs*/
        {
            u32DWords = 64;
            break;
        }

        case ARINC_717_PROG_SPEED_256_WPS: /* Sub-frame size = 128 DWs*/
        {
            u32DWords = 128;
            break;
        }

        case ARINC_717_PROG_SPEED_512_WPS: /* Sub-frame size = 256 DWs*/
        {
            u32DWords = 256;
            break;
        }

        case ARINC_717_PROG_SPEED_1024_WPS: /* Sub-frame size = 512 DWs*/
        {
            u32DWords = 512;
            break;
        }

        case ARINC_717_PROG_SPEED_2048_WPS: /* Sub-frame size = 1024 DWs*/
        {
            u32DWords = 1024;
            break;
        }

        case ARINC_717_PROG_SPEED_4096_WPS: /* Sub-frame size = 2048 DWs*/
        {
            u32DWords = 2048;
            break;
        }

        case ARINC_717_PROG_SPEED_8192_WPS: /* Sub-frame size = 4096 DWs*/
        {
            u32DWords = 4096;
            break;
        }

        default:    /* For invalid speed, exit with error condition */
        {
            return DDC_UDL_ERROR__ARINC_717_INVALID_SPEED;
        }
    }

    /*
     *  Verify amount of data is consistent with speed.
     *  pIoctlParams->Param1 contain byte size value;
     *  convert to Double Word size for comparaison purpose
     */
    if ((pIoctlParams->Param1 / sizeof(U32BIT)) != u32DWords)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_TX_LOAD, "ARINC717LoadTxQueueData - Ch %d Tx load size does not match for speed setting. Is %d bytes, Should be %d\n",
            (u8Channel + 1), (U32BIT)(pIoctlParams->Param1 / sizeof(U32BIT)), u32DWords);

        return DDC_UDL_ERROR__ARINC_717_TX_LOAD_BUFFER_SIZE;
    }

    /* For Double Buffer mode, need to ping pong between upper and lower half of buffer memory */

    /*
     *      ARINC 717 Double Buffer Mode Memory Configurarion
     *
     *  Start Address ---- +------------+ - Start Address of all odd Sub-Frames; base 1
     *                     |  Primary   |
     *                     |   Buffer   |   Max size of 4096 Double Words (16K bytes)
     *                     |            |
     *  Midpoint Address - +------------+ - Start Address of all even Sub-Frames; base 1
     *                     |  Secondary |
     *                     |   Buffer   |   Max size of 4096 Double Words (16K bytes)
     *                     |            |
     *  End Address ------ +------------+
     *
     */
    if (pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8BufMode == ARINC_717_PROG_BUF_MODE_DOUBLE)
    {
        if ((u32Option & ARINC_717_TX_DATA_PRIMARY_BUFFER_OPT) == ARINC_717_TX_DATA_PRIMARY_BUFFER_OPT)
        {
            u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;
        }
        else if ((u32Option & ARINC_717_TX_DATA_SECONDARY_BUFFER_OPT) == ARINC_717_TX_DATA_SECONDARY_BUFFER_OPT)
        {
            /* Mid memory address area for double buffering equating to ping-pong memory writes */
            u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;
            u32AddrBaseAddress += *pDeviceContext->sChannelArinc717[u8Channel].pu32MemSize / 2;
        }
    }
    else
    {
        u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;
    }

#if DDC_DMA_717
    {
        int nResult = 0;

        /* start the DMA transfer */
        dmaARINC717TxFrameSetup(pDeviceContext,
            u8Channel,
            (u32DWords << 2),                 /* convert # of 32-bit entries to bytes */
            (u32AddrBaseAddress << 2),
            (U8BIT*)pWrData);

        /* start DMA and wait for DMA completion */
        dmaQueueStart(pDeviceContext);
        pDeviceContext->u16ARINC717TxFrameEventCond[u8Channel] = 0;

        nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
            pDeviceContext->waitqueueARINC717TxFrameEvent[u8Channel],
            pDeviceContext->u16ARINC717TxFrameEventCond[u8Channel],
            500);

        if (nResult == 0)
        {
            /* timeout error */
            return DDC_UDL_ERROR__TIMEOUT;
        }
    }
#else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_TX_LOAD, "ARINC717LoadTxQueueData - Ch %d loading %d DWs at start address 0x%X\n",
            pIoctlParams->Channel, u32DWords, u32AddrBaseAddress);

#if DDC_PPC
        {
            U32BIT *pData = (U32BIT *) pWrData;
            int byteSwapIndex;

            /* byte-swap data */
            for (byteSwapIndex = 0; byteSwapIndex < u32DWords; byteSwapIndex++)
            {
                /* the data is stored as 16-bits, but written as 32-bits, we need to do a word swap now */
                pData[byteSwapIndex] = DDC_WORD_ORDER_L(pData[byteSwapIndex]);
            }
        }
#endif /* DDC_PPC */

        DDC_BLK_MEM_WRITE(pDeviceContext, u32AddrBaseAddress, pWrData, u32DWords, ACEX_32_BIT_ACCESS);
    }
#endif /* DDC_DMA_717 */

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ARINC717ReadData
 *
 * Description:
 *      This function loads transmitter buffer queue with data to be transmitted
 *      in bulk transfers
 *
 * In   pDeviceContext          device-specific structure
 * In   pIoctlParams               Receive parameters
 * In   OutputBufferLength      size of output buffer
 * In   pRdData                 pointer to buffer where read data will go
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ARINC717ReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pRdData,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    U32BIT u32AddrBaseAddress = 0;
    U32BIT u32DWords = 0;
    U8BIT u8Channel = (U8BIT)pIoctlParams->Channel;
    U32BIT u32Option = DDC_IOCTL_U32(pIoctlParams->Param2);

    *pBytesReturned = 0;
    if (pDeviceContext->eArinc717ProgState[u8Channel] == ARINC_717_RESET)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_RX_READ, "ARINC717ReadData - Ch %d, Invalid state for operation.\n",
            pIoctlParams->Channel);

        return DDC_UDL_ERROR__ARINC_717_TX_QUEUE;
    }

    /*
     *  Max bulk transmit size for USB is 4K bytes,
     *  therefore for larger bulk transmits must in iterations
     *  depending on ARINC 717 speed setting
     */
    switch (pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8Speed)
    {
        case ARINC_717_PROG_SPEED_32_WPS: /* Sub-frame size = 32 DWs*/
        {
            u32DWords = 32;
            break;
        }

        case ARINC_717_PROG_SPEED_64_WPS: /* Sub-frame size = 32 DWs*/
        {
            u32DWords = 32;
            break;
        }

        case ARINC_717_PROG_SPEED_128_WPS: /* Sub-frame size = 64 DWs*/
        {
            u32DWords = 64;
            break;
        }

        case ARINC_717_PROG_SPEED_256_WPS: /* Sub-frame size = 128 DWs*/
        {
            u32DWords = 128;
            break;
        }

        case ARINC_717_PROG_SPEED_512_WPS: /* Sub-frame size = 256 DWs*/
        {
            u32DWords = 256;
            break;
        }

        case ARINC_717_PROG_SPEED_1024_WPS: /* Sub-frame size = 512 DWs*/
        {
            u32DWords = 512;
            break;
        }

        case ARINC_717_PROG_SPEED_2048_WPS: /* Sub-frame size = 1024 DWs*/
        {
            u32DWords = 1024;
            break;
        }

        case ARINC_717_PROG_SPEED_4096_WPS: /* Sub-frame size = 2048 DWs*/
        {
            u32DWords = 2048;
            break;
        }

        case ARINC_717_PROG_SPEED_8192_WPS: /* Sub-frame size = 4096 DWs*/
        {
            u32DWords = 4096;
            break;
        }

        default:    /* For invalid speed, exit with error condition */
        {
            return DDC_UDL_ERROR__ARINC_717_INVALID_SPEED;
        }
    }

    /*
     *  Verify amount of data is consistent with speed.
     *  pIoctlParams->Param1 contain byte size value;
     *  convert to Double Word size for comparaison purpose
     */
    if ((OutputBufferLength / sizeof(U32BIT)) != u32DWords)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_RX_READ, "ARINC717ReadData - Ch %d Tx load size does not match for speed setting. Is %d bytes, Should be %d\n",
            (u8Channel + 1), (U32BIT)(pIoctlParams->Param1 / sizeof(U32BIT)), u32DWords);

        return DDC_UDL_ERROR__ARINC_717_TX_LOAD_BUFFER_SIZE;
    }

    /* For Double Buffer mode, need to ping pong between upper and lower half of buffer memory */

    /*
     *      ARINC 717 Double Buffer Mode Memory Configurarion
     *
     *  Start Address ---- +------------+ - Start Address of all odd Sub-Frames
     *                     |  Primary   |
     *                     |   Buffer   |   Max size of 4096 Double Words (16K bytes)
     *                     |            |
     *  Midpoint Address - +------------+ - Start Address of all even Sub-Frames
     *                     |  Secondary |
     *                     |   Buffer   |   Max size of 4096 Double Words (16K bytes)
     *                     |            |
     *  End Address ------ +------------+
     *
     */
    if (pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8BufMode == ARINC_717_PROG_BUF_MODE_DOUBLE)
    {
        if ((u32Option & ARINC_717_TX_DATA_PRIMARY_BUFFER_OPT) == ARINC_717_TX_DATA_PRIMARY_BUFFER_OPT)
        {
            u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;
        }
        else if ((u32Option & ARINC_717_TX_DATA_SECONDARY_BUFFER_OPT) == ARINC_717_TX_DATA_SECONDARY_BUFFER_OPT)
        {
            /* Mid memory address area for double buffering equating to ping-pong memory writes. */
            u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;
            u32AddrBaseAddress += *pDeviceContext->sChannelArinc717[u8Channel].pu32MemSize/2;
        }
    }
    else
    {
        u32AddrBaseAddress = *pDeviceContext->sChannelArinc717[u8Channel].pu32MemBA;

        if ((pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8WrapAroundMode == ARINC_717_PROG_INT_WRAP_AROUND) ||
            (pDeviceContext->sArinc717ProgChMemConfig[u8Channel].u8WrapAroundMode == ARINC_717_PROG_EXT_WRAP_AROUND))
        {
            /* Mid memory address area for double buffering equating to ping-pong memory writes. */
            u32AddrBaseAddress += *pDeviceContext->sChannelArinc717[u8Channel].pu32MemSize/2;
        }
   }

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC717, DDC_DBG_ARINC717_RX_READ, "ARINC717ReadData - Ch%d reads %d DWs at start address 0x%X\n",
        pIoctlParams->Channel, u32DWords, u32AddrBaseAddress);

#if DDC_DMA_717
    {
        int nResult = 0;

        /* start the DMA transfer */
        dmaARINC717RxFrameSetup(pDeviceContext,
            u8Channel,
            (u32DWords << 2),           /* convert # of 32-bit entries to bytes */
            (u32AddrBaseAddress << 2),  /* convert to byte address */
            (U8BIT *)pRdData);

        /* start DMA and wait for DMA completion */
        dmaQueueStart(pDeviceContext);
        pDeviceContext->u16ARINC717RxFrameEventCond[u8Channel] = 0;

        nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
            pDeviceContext->waitqueueARINC717RxFrameEvent[u8Channel],
            pDeviceContext->u16ARINC717RxFrameEventCond[u8Channel],
            500);

        if (nResult == 0)
        {
            /* timeout error */
            return DDC_UDL_ERROR__TIMEOUT;
        }
    }
#else
    DDC_BLK_MEM_READ( pDeviceContext, u32AddrBaseAddress, pRdData, u32DWords, ACEX_32_BIT_ACCESS);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *) pRdData;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < u32DWords; byteSwapIndex++)
        {
            /* the data is stored as 16-bits, but read out as 32-bits, we need to do a word swap now */
            pData[byteSwapIndex] = DDC_WORD_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

#endif /* DDC_DMA_717 */

    *pBytesReturned = u32DWords << 2;

    return DDC_UDL_ERROR__SUCCESS;
}
