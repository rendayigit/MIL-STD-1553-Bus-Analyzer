/*******************************************************************************
 * FILE: ddc_udl_1553_mt.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to provide functions to support
 * configuration/management of the 1553 MT/MTi/MTiE improvements block.
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
#include "include/ddc_ioctl.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_sdk.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"


/******************************************************************************
 * Name:    mtInitialize
 *
 * Description:
 *      Parse UM registers and populate UM data structure.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/

/*-----------------------------------------------------------------------------
    Function: mtInitialize
    Description:  initialize master Unified MT module.

    For the current implementation, the MT configuration/mgmt memory
    region will begin at address 2000 for 1st channel.  User memory will begin
    after that region.

        1st channel mapping is as follows for 1M byte per channel card.
   (PC104+ has 2M byte per channel)

                                32 Bit-Address Mapping

                        0x0003FFFF  +-----------------+
                                    |                 |
                                    |                 |
                                    |                 |
                                    |                 |
                                    |   User Memory   |
                                    | (868,480 bytes) |
                                    |                 |
                                    |                 |
                                    |                 |
          User Memory - 0x0000A040  +-----------------+
                                    |                 |
                                    |    Imp Memory   |
                                    | (131,072 bytes) |
                                    |  Memory         |
                        0x00002040  +-----------------+
         Selective MT Lookup Table  |  (256 bytes)    |
                        0x00002000  +-----------------+
                                    |  MRT Config     |
                                    | (32,768 bytes)  |
    1553 Memory Address 0x00000000  +-----------------+

   Parameters:
               pDeviceContext - device-specific structure


   Returns:
            status (SUCCESS or ERROR CODE)
   --------------------------------------------------------------------------------------*/
S16BIT mtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT u16Channel = pMtiConfig->sConfigID.u16Channel;
    U32BIT u32Data;

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    if (pMT->state == ACEX_MOD_OPEN)
    {
        /* MTi portion of the Driver left in OPEN state, Force MTi to a CLOSE state prior
         *  to placing it in a RESET state */
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INIT_MODE, "Channel %d Forcing to a CLOSE state!\n", u16Channel);

        mtClose(pDeviceContext, u16Channel);
        return DDC_UDL_ERROR__STATE;
    }

    /* Assign/allocate base address and size of Selective Monitor Lookup table.
     * Add offset depending on channel.  */
    pMT->u32LookupTable32BitBA = MT_LKUP_TABLE_MEMORY_BASE_ADDRESS + *(pDeviceContext->pChannel1553[u16Channel]->pu32MemBA);

    pMT->u32LookupTableMemByteSize = MT_LKUP_TABLE_MEMORY_BYTE_SIZE;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INIT_MODE, "Channel %d Lookup Table Base Address is %X\n", u16Channel, pMT->u32LookupTable32BitBA);

    /* Reset Channel's MT Hardware */
    u32Data = MT_STROBE_RESET_MONITOR;
    DDC_REG_WRITE(pDeviceContext, (*(pMT->pu32RegBA) + REG_MT_STROBE_W), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INIT_MODE,
        "WRITE 1553 REG:%08x VALUE:%08x (MT RESET CH%d)\n", (*(pMT->pu32RegBA) + REG_MT_STROBE_W), u32Data, u16Channel);

    pMT->state = ACEX_MOD_RESET;

    pMT->u32RespTimeout = MT_CONFIG_NO_RESP_TIMEOUT_DEFAULT;

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtGetMtiCh10TimePkt
 *
 * Description:
 *      Description: Returns A Irig Chapter 10 Data Packet From The Pool.
 *      IRIG 64 bit BCD format
 *
 *      |6 6 6 6 5 5 5 5 5 5 5 5 5 5 4 4 4 4 4 4 4 4 4 4 3 3 3 3 3 3 3 3|
 *      |3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2|
 *      +-+---------------+---------------+---------------+-------------+
 *      |I|    Seconds    | 10th Seconds  |   Hours       |   minutes   |
 *      +-+---------------+---------------+---------------+-------------+
 *
 *      |3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0|
 *      |1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0|
 *      +---------------+---------------+-------------------------------+
 *      |    Months     |     Days      |         Years                 |
 *      +---------------+---------------+-------------------------------+
 *
 *      I = 1 if external IRIG signal detected
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pIoctlParams       input value
 * In   ppCh10Pkt       output value
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S32BIT mtGetMtiCh10TimePkt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT *pu16TimeCntr;
    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    S16BIT s16Timeout = (S16BIT)pIoctlParams->Param1;
    U32BIT u32EventTimeout = s16Timeout;
    MTI_CH10_TIME_PKT *pTimePkt;
    U16BIT u16Sec = 0;
    U16BIT u16HrMin = 0;
    U16BIT u16Day = 0;
    U16BIT u16Year = 0;
    U32BIT i;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_TIME_PKT, "ENTER->\n");

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    if (!pMT->bMtiTimeDataAvailable)
    {
        if (s16Timeout == 0)
        {
            return MTI_ERR_DATA_UNAVAILABLE;
        }

        /* zero out wait queue condition flag */
        DDC_ISR_LOCK_TAKE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
        if (pMT->u8MtiTimePoolEventFlag)
        {
            pMT->u8MtiTimePoolEventFlag--;
        }
        DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);

        /* Sleep and wait for the local ISR or ISR clean-up to wake us up and proceed */
        if (s16Timeout > 0)
        {
            int nResult = 0;

            nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                pMT->eMtiTimePoolEvent,
                pMT->u8MtiTimePoolEventFlag,
                u32EventTimeout);

            if (nResult)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_TIME_PKT, "nResult %d\n", nResult);
            }
            /* If the flag is still '0', the timeout expired */
            DDC_ISR_LOCK_TAKE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
            if (!pMT->u8MtiTimePoolEventFlag)
            {
                DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
                return MTI_ERR_TIMEOUT;
            }
            DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
        }
        else
        {
            DDC_WAIT_INTERRUPTABLE(pMT->eMtiTimePoolEvent, pMT->u8MtiTimePoolEventFlag);
        }
    }

    /* No errors - get the packet */
    pTimePkt = &pMT->pMtiTimeListEntry->sMtiTimePkt;
    pu16TimeCntr = (U16BIT*)GET_ACEX_MTI_CH10_TIME_PKT_TIME_CNTR_POINTER(pTimePkt);

    /* Fill packet header data */
    pTimePkt->u16PktSyncPattern = MTI_CH10_PKT_SYNC_PATT;
    pTimePkt->u16ChannelId = pDeviceContext->u16IrigPacketChannelId[u16Channel];
    pTimePkt->u32PktLength = sizeof(MTI_CH10_TIME_PKT);              /* 36 bytes */

    /* The U64BIT issue must be revisited */
    pTimePkt->u32DataLength = sizeof(U64BIT) + MTI_CH10_PKT_CHANNEL_DATA_SIZE;
    pTimePkt->u16SeqNumHdrVer = pMT->s8MtiTimePacketSeqNumber++;
    pTimePkt->u16SeqNumHdrVer = (U16BIT)(((pTimePkt->u16SeqNumHdrVer << 8) & 0xFF00) | MTI_CH10_PKT_HEADER_VER);
    pTimePkt->u16DatTypePktFlags = ((MTI_CH10_TIME_PKT_TYPE << 8) & 0xFF00);

    /* Relative Time Counter - Use Intra-Packet Time Stamp from the 1st Msg packet*/
    pu16TimeCntr[0] = (U16BIT)(pMT->u32MtiRelativeTimeDataLo & 0xFFFF);
    pu16TimeCntr[1] = (U16BIT)(pMT->u32MtiRelativeTimeDataLo >> 16);
    pu16TimeCntr[2] = (U16BIT)(pMT->u32MtiRelativeTimeDataHi & 0xFFFF);

    /* Calculate Header Checksum:
           16-bit arithmetic sum of all 16-bit words in the header excluding the Header Checksum */
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_TIME_PKT, "Calculate Checksum\n");

    pTimePkt->u16HeaderChksum = 0;

    for (i = 0; i < MTI_CH10_PKT_CHECKSUM_WORD_CNT; i++)
    {
        pTimePkt->u16HeaderChksum = (U16BIT)(pTimePkt->u16HeaderChksum + ((U16BIT *)pTimePkt)[i]);
    }

    /*                             (     Bits 4-7                           )  ( Bits 8-11                        )*/
    pTimePkt->u32ChnlSpecificData = MTI_CH10_TIME_PKT_CHANNEL_DATA_FMT_IRG_B | MTI_CH10_TIME_PKT_CHANNEL_DATA_DATE_DMY;

    /* Check for loss of exteranl IRIGB signal */
    if (pDeviceContext->u32MtiGlobalTimeDataHi & MT_IRIG_TIME_SOURCE_INTERNAL)
    {
        /* Force MSB bit of Global Time Hi to 0 therby comforming to Part 2 of IRIG 106 Chapter 10 standard */
        pDeviceContext->u32MtiGlobalTimeDataHi = (U32BIT)(pDeviceContext->u32MtiGlobalTimeDataHi & ~MT_IRIG_TIME_SOURCE_INTERNAL);
    }
    else
    {
        /* Channel Specific Data SRC bit definitions.
         * Bits 0 - 3 indicates the source of the time in the payload of each time packet.
         *  0x0 = Internal (Time derived from a clock in the Recorder)
         *  0x1 = External (Time derived from a clock not in the Recorder)
         *  0x2 = Internal from RMM (Time derived from the clock in the RMM)
         *  0x3  0xE = Reserved
         *  0xF = None
         */
        pTimePkt->u32ChnlSpecificData |= (U32BIT)MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_EXT;
    }

    /* fill in global timetag*/
    /*pTimePkt->u64TimeData = (U64BIT)pDeviceContext->u32MtiGlobalTimeDataLo | ((U64BIT)pDeviceContext->u32MtiGlobalTimeDataHi << 32);*/
    u16Sec = (U16BIT)((pDeviceContext->u32MtiGlobalTimeDataHi & 0xFFFF0000) >> 16);                                    /* Second and MilliSecond   */
    u16HrMin = (U16BIT) (pDeviceContext->u32MtiGlobalTimeDataHi & 0x0000FFFF);                                         /* Hour and Minute          */
    u16Day = (U16BIT)((pDeviceContext->u32MtiGlobalTimeDataLo & 0xFFFF0000) >> 16);                                    /* Day                      */
    u16Year = (U16BIT) (pDeviceContext->u32MtiGlobalTimeDataLo & 0x0000FFFF);                                          /* Year                     */
    pTimePkt->u64TimeData = ((U64BIT)u16Year << 48) | ((U64BIT)u16Day << 32) | ((U64BIT)u16HrMin << 16) | ((U64BIT)u16Sec);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_TIME_PKT, "TimeData %llx, Year %x Dat %x HrMin %x Sec %x\n", \
        pTimePkt->u64TimeData, u16Year, u16Day, u16HrMin, u16Sec);

    pMT->bMtiTimeDataAvailable = FALSE;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_TIME_PKT, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtSetMtiState
 *
 * Description:
 *      Sets The State Of The MTI Engine (RESET, READY, RUN). called during aceMTStart() procedure
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pIoctlParams       input values for Channel & State
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtSetMtiState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    MTI_STATE eState = (MTI_STATE)pIoctlParams->Param1;
    U32BIT u32Value;
    U32BIT i = 0;
    U8BIT u8Index;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "ENTER-> State = %d, Channel = %d\n", eState, u16Channel);

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    switch (eState)
    {
        case ACEX_MTI_RUN:
        {
            /* Save State */
            pMT->eMtiState = eState;

            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            pMT->u8MtiDataPoolEventFlag = 0;
            DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

            DDC_ISR_LOCK_TAKE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
            pMT->u8MtiTimePoolEventFlag = 0;
            DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);

            /* If Mti Interrupts And Time Packets Have Not Been Actived By Another Channel, Activate Them */
            if (pDeviceContext->u16MtiChannelCount == 0)
            {
                for (u8Index = 0; u8Index < pDeviceContext->u8BoardInstanceCount; u8Index++)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "MTI Interrupts Active\n");

                    /* IRIG Improvements and IRIG Format B always on, enable DMY & enable IRIG */
                    u32Value = IRIGB_CONTROL_DMY_FORMAT_ENABLE | IRIGB_CONTROL_CLOCK_ENABLE;
                    DDC_REG_WRITE(pDeviceContext, ((*(pDeviceContext->sIrigB_RX[u8Index].pu32RegBA)) + REG_IRIGB_CONTROL_RW), &u32Value);
                }
            }

            /* Increment Channel Count */
            pDeviceContext->u16MtiChannelCount++;

            /* Clear Device Overlow Flag */
            pMT->bMtiDeviceOverflow = FALSE;

            /* Enable MTI Command Stack.  For USB version of ACEX, this is not supported but might be for PCI!  */
            u32Value = MT_MTI_INT_STATUS_ON_OVERFLOW;
            DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_STATUS_RW), &u32Value);

            /* Issue an MT Start command */
            u32Value = MT_STROBE_START_RESUME_MONITOR;
            DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_STROBE_W), &u32Value);

            break;
        }

        case ACEX_MTI_RESET:
        case ACEX_MTI_READY:
        {
            pMT->eMtiState = eState;

            /* Issue an MT Stop command */
            u32Value = MT_STROBE_PAUSE_MONITOR;
            DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_STROBE_W), &u32Value);

            /* Disable MTI Command Stack.  For USB version of ACEX, this is not supported might be for PCI!  */
            u32Value = (U32BIT) ~MT_MTI_INT_STATUS_ON_OVERFLOW;
            DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_STATUS_RW), &u32Value);

            /* Decrement Channel Count */
            pDeviceContext->u16MtiChannelCount--;

            /* reset head and tail indexes, set state to free */
            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            pMT->u32MtiDataPktTail = pMT->u32MtiDataPktHead = 0;

            for (i = 0; i < MTI_MAX_NUM_BUF; i++)
            {
                pMT->pMtiDataListEntry[i]->State = BUF_STATE_FREE;
            }

            /* wake up any blocked getch10 packets*/
            if (pMT->u8MtiDataPoolEventFlag < 0xff)
            {
                pMT->u8MtiDataPoolEventFlag++;
            }

            DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

            DDC_WAKE_UP(&pMT->eMtiDataPoolEvent);

            for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
            {
                pMT->bMtiTimeDataAvailable = FALSE;

                /* wake up event, if any*/
                DDC_ISR_LOCK_TAKE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
                if (pMT->u8MtiTimePoolEventFlag < 0xff)
                {
                    pMT->u8MtiTimePoolEventFlag++;
                }
                DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);

                DDC_WAKE_UP(&pMT->eMtiTimePoolEvent);
            }

            /* wait for dma complete if necessary */
            if ((pDeviceContext->sDMA.u16CurrentChannel - 1) == u16Channel)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "MTI state change : waiting for DMA to complete!\n");
                DDC_WAIT_INTERRUPTABLE(pMT->eMtiDataPoolEvent, pMT->u8MtiDataPoolEventFlag);
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "MTI state change :  DMA to completed!\n");
            }

            /* Insure IRIG 1 second Interrupt is disabled */
            if (pDeviceContext->u16MtiChannelCount == 0)
            {
                for (u8Index = 0; u8Index < pDeviceContext->u8BoardInstanceCount; u8Index++)
                {
                    DDC_REG_READ(pDeviceContext, ((*(pDeviceContext->sIrigB_RX[u8Index].pu32RegBA)) + REG_IRIGB_CONTROL_RW), &u32Value);
                    u32Value &= ~(U32BIT)IRIGB_CONTROL_1_SEC_TIME_INT_ENABLE;

                    DDC_REG_WRITE(pDeviceContext, ((*(pDeviceContext->sIrigB_RX[u8Index].pu32RegBA)) + REG_IRIGB_CONTROL_RW), &u32Value);
                }
            }

            break;
        }

        default:
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "Unknown MTI State Set\n");
            break;
        }
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STATE, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtOpen
 *
 * Description:
 *      Configures the Monitor Terminal-Improved (MTI) resources for
 *      operation.  This includes creating the receive buffer pools
 *      and RxMsg completetion queue.
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pMtiConfig      input for MT configuration values
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig,
    MT_MTI_HW_INFO *pMtiHwInfo
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT u16Channel = pMtiConfig->sConfigID.u16Channel;
    U32BIT u32MtiDataBufferBlkNum = 0;
    U32BIT u32MtiDataBufferSize = 0;
    U32BIT u32TempHold = 0;
    U32BIT u32UmtControlRegValue = 0;
    U32BIT u32BufCnt = 0;
    U32BIT u32BufCnt2 = 0;
    U32BIT u32IrqTimeInterval = 0;
    U8BIT i = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "ENTER->Channel : %d\n", u16Channel);

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    if (pMT->state == ACEX_MOD_OPEN) /* we do not want to open again if already opened */
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* Check Zero Copy Flag Is False, Return Parameter Error If True */
    /* Zero copy is not implemented
    if (pMtiConfig->fZeroCopyEnable)
    {
        return MTI_ERR_PARAMETER;
    }*/

    /*----- CONFIG MTI RELATED REGs ACCORDING TO INPUT pMtiConfig -----*/

    /* Initialize MTI Interrupt Mask Register */
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_ENABLE_RW), &pMtiConfig->u32IntConditions);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "MTI Interrupt Register set with the following value %X\n", pMtiConfig->u32IntConditions);

    /* Initialize MTI Interrupt Word Count */
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_NUMBER_OF_WORDS_RW), &pMtiConfig->u32IrqDataLen);

    /* Initialize MTI Interrupt Message Count */
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_NUMBER_OF_MSGS_RW), &pMtiConfig->u32IrqMsgCnt);

    /* Initialize MTI Interrupt Time Count */
    u32IrqTimeInterval = pMtiConfig->u16IrqTimeInterval;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_TIME_INTERVAL_RW),  &(u32IrqTimeInterval));

    /*
     * Save Mti Memory 32-Bit (DW) Start and End Address
     * original calculation was unnecessarily complicated and
     * had problem when a card was not as assumed in contiguous memory.
     */
    pMT->u32MtiMem32BitStartAddr = ((U32BIT)pMtiConfig->u32DevBufWordAddr >> 1);
    pMT->u32MtiMem32BitEndAddr = (U32BIT)(((U32BIT)pMtiConfig->u32DevBufWordAddr >> 1) + (pMtiConfig->u32DevBufByteSize / sizeof(U32BIT)));
    pMT->u32MtiStackSizeWords = (U32BIT)(pMtiConfig->u32DevBufByteSize / sizeof(U16BIT));

    /* Adjust to offset value from the channel base address */
    pMtiConfig->u32DevBufWordAddr -= ((*pDeviceContext->pChannel1553[u16Channel]->pu32MemBA) << 1);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Device Memory DWord Size : 0x%X\n",                              \
        (U32BIT)(pMtiConfig->u32DevBufByteSize / sizeof(U32BIT)));

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Absolute 32-Bit Device Memory Start Address : 0x%X\n",           \
        pMT->u32MtiMem32BitStartAddr);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Absolute 32-Bit Device Memory End Address : 0x%X\n",             \
        pMT->u32MtiMem32BitEndAddr);

    /* Initialize MTI Device Memory Start Address */
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_STK_ADDRESS_RW), &pMtiConfig->u32DevBufWordAddr);

    /* For debug purpose */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_STK_ADDRESS_RW), &u32TempHold);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Relative 16-Bit Device Memory Start Address : 0x%X\n", u32TempHold);

    pMT->u32MtiStackPercentFull = pMT->u32MtiStackSizeWords;
    pMT->u32MtiStackPercentHigh = pMT->u32MtiStackSizeWords;
    pMT->u32MtiStackOverflowCount = 0;

    /* Initialize MTI Device Memory Size (Size In words) */
    switch (pMtiConfig->u32DevBufByteSize)
    {
        case MTI_DEVBUF_SIZE_4K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_1024_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_8K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_2048_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_16K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_4096_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_32K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_8192_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_64K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_16384_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_128K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_32768_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_256K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_65536_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_512K:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_131072_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_1M:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_262144_DW;
            break;
        }

        case MTI_DEVBUF_SIZE_2M:
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_524288_DW;
            break;
        }

        default: /* MTI_DEVBUF_SIZE_128K */
        {
            u32UmtControlRegValue = MT_CONFIG_MT_CMD_DATA_STACK_SIZE_32768_DW;
            break;
        }
    }

    /* Enable/Disable 1553A Mode Code format setting */
    if (pMtiConfig->b1553aMc)
    {
        u32UmtControlRegValue |= MT_CONFIG_1553A_MCODES_ENABLE;
    }
    
	/* Enable/Disable Override Mode Code T/R Error format setting */
    if(pMtiConfig->b1553aOverrideMcTrErrEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_OVERRIDE_MC_TR_ERR_ENABLE;
    }
	
    /* Enalbe/Disable Broadcast Disable setting */
    if (pMtiConfig->bBcstDisable)
    {
        u32UmtControlRegValue |= MT_CONFIG_BROADCAST_DISABLE;
    }

    /* Enable/Disable Error Monitor setting */
    if (pMtiConfig->bMtiErrorMonitorEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_MODE_MTIE_ENABLE;
    }

    /* Enable/Disable Replay Monitor setting */
    if (pMtiConfig->bMtiReplayMonitorEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_MODE_MTIR_ENABLE;
    }

    /* Enable/Disable Triggered Start */
    if (pMtiConfig->bMtiTriggerStartEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_TRIGGER_START_ENABLE;
    }

    /* Enable/Disable Triggered Stop */
    if (pMtiConfig->bMtiTriggerStopEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_TRIGGER_STOP_ENABLE;
    }

    /* Enable/Disable MTi Block Status */
    if (pMtiConfig->bMtiBswTypeDisable)
    {
        u32UmtControlRegValue &= ~MT_CONFIG_MTI_BLOCK_STATUS_ENABLE;
    }
    else
    {
        u32UmtControlRegValue |= MT_CONFIG_MTI_BLOCK_STATUS_ENABLE;
    }

    /* OR in response timeout */
    u32UmtControlRegValue = (u32UmtControlRegValue & ~MT_CONFIG_NO_RESP_TIMEOUT_MASK) | pMT->u32RespTimeout;

    /* OR in RT Busy processing for proper METS BSW reporting. Only applicable to MTi/MTiE mode of operation */
    if (pMtiConfig->bBusyIllegalBitEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_BUSY_BIT_NODATA_VALID_FORMAT_ENABLE;
    }

    /* OR in EOM Time Tag Stamp enalbe */
    if (pMtiConfig->bEomTtEnable)
    {
        u32UmtControlRegValue |= MT_CONFIG_EOM_TTAG_ENABLE;
    }

    /* Check to see if bus A monitoring is disabled */
    if (pMtiConfig->bBusAMonitoringDisable)
    {
        u32UmtControlRegValue |= MT_CONFIG_CHA_DISABLE;
    }

    /* Check to see if bus B monitoring is disabled */
    if (pMtiConfig->bBusBMonitoringDisable)
    {
        u32UmtControlRegValue |= MT_CONFIG_CHB_DISABLE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "pMT->u32RespTimeout =  0x%X\n", pMT->u32RespTimeout);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Config Register =  0x%X after Response Timeout Setting\n", u32UmtControlRegValue);

    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_CONFIG_RW), &u32UmtControlRegValue);

    /* For Debug Purpose */
    u32UmtControlRegValue = 0;
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_CONFIG_RW), &u32UmtControlRegValue);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Config Register : 0x%X\n", u32UmtControlRegValue);

    /*----- PREPARE MTI DATA PKT LIST -----*/

    /* Save Mti Channel ID */
    pMT->u16MtiPacketChannelId = pMtiConfig->u16Ch10ChnlId;

    /* Set Default IRIG Channel ID.  Call to IOCTL_IRIG_SET_ID will set unique value */
    pDeviceContext->u16IrigPacketChannelId[u16Channel] = pMtiConfig->u16Ch10ChnlId;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Channel Id : 0x%X\n", pMtiConfig->u16Ch10ChnlId);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Delete Linked List Entries\n");

    if (pMT->bMtiLookasideListDataInit)
    {
        mtFreeDataPktList(pDeviceContext, u16Channel);
    }

    pMT->s8MtiDataPacketSeqNumber = 0;

    /* Allocate the channel Data Packet buffers: Number of buffers * size of one buffer */
    u32MtiDataBufferBlkNum = pMtiConfig->u32NumBufBlks;

    if (u32MtiDataBufferBlkNum > MTI_MAX_NUM_BUF)
    {
        u32MtiDataBufferBlkNum = MTI_MAX_NUM_BUF;
    }

    u32MtiDataBufferSize = (U32BIT)(pMtiConfig->u32BufBlkByteSize + MTI_POOL_OVERHEAD_SIZE);

    for (u32BufCnt = 0; u32BufCnt < u32MtiDataBufferBlkNum; u32BufCnt++)
    {
        struct _MTI_DATA_LIST_ENTRY *pMtiDataListEntry = pMT->pMtiDataListEntry[u32BufCnt];

        /* Allocate requested buffer size plus 80 bytes due to nature */
       
#if DDC_DMA_MT
        pMtiDataListEntry->mtDMA_Size = (size_t)u32MtiDataBufferSize;  
        pMtiDataListEntry->pDataPkt = DDC_DMA_MALLOC(
            pDeviceContext,
            pMtiDataListEntry->mtDMA_Size,
            &pMtiDataListEntry->mtDMA_Addr,
            DDC_MEMORY_REGION__MT_DMA);

        if (pMtiDataListEntry->pDataPkt == NULL)
        {
            /* Clean up currently allocated pages and return error */
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "ALLOC FAILED!\n");

            for (u32BufCnt2 = 0; u32BufCnt2 < u32BufCnt; u32BufCnt2++)
            {
                struct _MTI_DATA_LIST_ENTRY *pMtiDataListEntry2 = pMT->pMtiDataListEntry[u32BufCnt2];

                /* free previously allocated MT DMA buffers */
                if (pMtiDataListEntry2->pDataPkt)
                {
                    DDC_DMA_FREE(
                        pDeviceContext,
                        pMtiDataListEntry2->mtDMA_Size,
                        pMtiDataListEntry2->pDataPkt,
                        pMtiDataListEntry2->mtDMA_Addr,
                        DDC_MEMORY_REGION__MT_DMA);

                    pMtiDataListEntry2->pDataPkt = NULL;
                }
            }

            return MTI_ERR_BUFFER_UNAVAILABLE;
        }
#else
        pMtiDataListEntry->pDataPkt = DDC_KERNEL_MALLOC(pDeviceContext, u32MtiDataBufferSize);
        
        if (pMtiDataListEntry->pDataPkt == NULL)
        {
            /* Clean up currently allocated pages and return error */
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "ALLOC FAILED!\n");

            for (u32BufCnt2 = 0; u32BufCnt2 < u32BufCnt; u32BufCnt2++)
            {
                struct _MTI_DATA_LIST_ENTRY *pMtiDataListEntry2 = pMT->pMtiDataListEntry[u32BufCnt2];

                /* free previously allocated MT DMA buffers */
                if (pMtiDataListEntry2->pDataPkt)
                {
                    DDC_KERNEL_FREE(pDeviceContext, pMtiDataListEntry2->pDataPkt);
                    pMtiDataListEntry2->pDataPkt = NULL;
                }
            }

            return MTI_ERR_BUFFER_UNAVAILABLE;
        }    
#endif /* DDC_DMA_MT */
    }

    /* Save Some Information */
    pMT->u32MtiDataPoolBufferSize = u32MtiDataBufferSize;
    pMT->u32MtiDataPoolCount = u32MtiDataBufferBlkNum;
    pMT->bMtiLookasideListDataInit = TRUE;

    /*----- INITIALIZE TIME DATA -----*/
    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        pMT->bMtiTimeDataAvailable = FALSE;
    }

    pMT->s8MtiTimePacketSeqNumber = 0;

    /*----- PREPARE RETURNED pMtiHwInfo -----*/

    /* Assign base Address and size for the Selective Monitor Lookup Table for library usage */
    pMtiHwInfo->u32LookupTable32BitBA = pMT->u32LookupTable32BitBA;
    pMtiHwInfo->u32LookupTable32BitSize = pMT->u32LookupTableMemByteSize;

    /* Return memory organization addresses */
    pMtiHwInfo->u32MtiMem32BitStartAddrOffset = *(pDeviceContext->pChannel1553[u16Channel]->pu32MemBA);
    pMtiHwInfo->u32MtiMem32BitStartAddr = pMT->u32MtiMem32BitStartAddr;
    pMtiHwInfo->u32MtiMem32BitEndAddr = pMT->u32MtiMem32BitEndAddr;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "Selective Monitor Lookup Tabel BA is %X and size is %d\n", pMtiHwInfo->u32LookupTable32BitBA, pMtiHwInfo->u32LookupTable32BitSize);

    /* Set 1553 channel specific interrupt mask */
    gen1553InterruptSet(pDeviceContext, (U8BIT)u16Channel, GENERAL_INT_MASK_MT_INT_ENABLED);

    /* Set MTI BSW disable */
    pMT->bMtiBswTypeDisable = pMtiConfig->bMtiBswTypeDisable;

    /* Set MTIe enable */
    pMT->bMtiErrorMonitorEnable = pMtiConfig->bMtiErrorMonitorEnable;

    /* Set MTR disable - overrides MTI BSW and MTIe */
    pMT->bMtiReplayMonitorEnable = pMtiConfig->bMtiReplayMonitorEnable;

    /* Set MTI customer chapter 10 data type enable */
    pMT->bMtiCustomDataTypeEnable = pMtiConfig->bMtiCustomDataTypeEnable;

    /* Set MTI trigger start enable */
    pMT->bMtiTriggerStartEnable = pMtiConfig->bMtiTriggerStartEnable;

    /* Set MTI trigger stop enable */
    pMT->bMtiTriggerStopEnable = pMtiConfig->bMtiTriggerStopEnable;

    pMT->state = ACEX_MOD_OPEN;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_OPEN, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtClose
 *
 * Description:
 *      Closes MT operaton.
 *
 * In   pDeviceContext  Device-specific structure
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32Data;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CLOSE, "ENTER->\n");

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    /* Disable all MTI Interrupts for this channel */
    u32Data = MT_MTI_INT_DISABLE_ALL;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_ENABLE_RW), &u32Data);

    /* Reset MTi Channel Hardware */
    u32Data = MT_STROBE_RESET_MONITOR;
    DDC_REG_WRITE(pDeviceContext, (*(pMT->pu32RegBA) + REG_MT_STROBE_W), &u32Data);

    /* Free Data Pkt List */
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CLOSE, "Delete Linked List Entries\n");

    if (pMT->bMtiLookasideListDataInit)
    {
        mtFreeDataPktList(pDeviceContext, u16Channel);
    }

    /* Disable MT master interrupt mask in the 1553 General Component */
    gen1553InterruptClear(pDeviceContext, (U8BIT)u16Channel, GENERAL_INT_MASK_MT_INT_ENABLED);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CLOSE, "EXIT\n");

    pMT->state = ACEX_MOD_CLOSED;

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtStoreTimeData
 *
 * Description:
 *      Retrieve Time Data from device and stores them in driver.
 *
 * In   pDeviceContext  Device-specific structure
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mtStoreTimeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32GlobalIrigbTime[2]; /* 0 - high dwd, 1 - low dwd */
    U32BIT u32TimeData[2];        /* 0 = high dwd, 1 - low dwd */
    U8BIT i = 0;

    /* Read and store global IRIG Time; not channel based and must read MSB 1st */
    DDC_BLK_REG_READ(pDeviceContext, *(pDeviceContext->sIrigB_RX[0].pu32RegBA) + REG_IRIGB_LATCHED_1SEC_TIME_STAMP_MSB_RW, u32GlobalIrigbTime, 2);
    pDeviceContext->u32MtiGlobalTimeDataHi = u32GlobalIrigbTime[0];
    pDeviceContext->u32MtiGlobalTimeDataLo = u32GlobalIrigbTime[1];
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_TIME_WORK_ITEM, "u32MtiGlobalTimeDataHi = %x u32MtiGlobalTimeDataLo = %x\n", \
        u32GlobalIrigbTime[0], u32GlobalIrigbTime[1]);

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        pMT = &(pDeviceContext->pChannel1553[i]->sMT);

        /* Read Relative Time Tag Values*/
        DDC_BLK_REG_READ(pDeviceContext, *(pDeviceContext->pChannel1553[i]->pu32RegBA) + REG_GENERAL_TT_LATCHED_MSB, u32TimeData, 2);
        pMT->u32MtiRelativeTimeDataHi = u32TimeData[0];
        pMT->u32MtiRelativeTimeDataLo = u32TimeData[1];

        pMT->bMtiTimeDataAvailable = TRUE;

        /* wake up event, if any*/
        DDC_ISR_LOCK_TAKE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
        if (pMT->u8MtiTimePoolEventFlag < 0xff)
        {
            pMT->u8MtiTimePoolEventFlag++;
        }
        DDC_ISR_LOCK_GIVE(pMT->semMtiTimePkt, pMT->semMtiTimePktFlag);
        DDC_WAKE_UP_INTERRUPTIBLE(&pMT->eMtiTimePoolCallback, &pMT->eMtiTimePoolEvent);
    }
}

/******************************************************************************
 * Name:    mtSetRespTimeOut
 *
 * Description:
 *      This function sets the response timeout timer on the hardware.
 *      Valid Response Time Out range is from 0.5us to 127.5us
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtSetRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    U32BIT u32RespTimeout;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_RESP_TIME_OUT, "ENTER-> Channel %d Response Timeout = %d(* 0.5us)\n", \
        u16Channel, (int)pIoctlParams->Param2);

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    /* Insure only Response time-out bits are being set */
    pMT->u32RespTimeout = (DDC_IOCTL_U32(pIoctlParams->Param2) << MT_CONFIG_NO_RESP_TIMEOUT_BIT_ALIGN) & MT_CONFIG_NO_RESP_TIMEOUT_MASK;

    /* Timeout is in 0.5us units, therfore actual time out is input parameter
    * multiplied by 0.5 (e.g. if pIoctlParams->Param2 = 37, timeout is 18.5us */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_CONFIG_RW), &u32RespTimeout);

    u32RespTimeout &= (U32BIT) ~MT_CONFIG_NO_RESP_TIMEOUT_MASK;
    u32RespTimeout |= pMT->u32RespTimeout;

    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_CONFIG_RW), &u32RespTimeout);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_RESP_TIME_OUT, "MT Config Reg %X\n", (unsigned int)u32RespTimeout);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtGetMtiCh10DataPkt
 *
 * Description:
 *      Returns A Irig 106 Chapter 10 Data Packet From The Pool.
 *
 *      +---------------------------------------------------------------+
 *      |3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0|
 *      |1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0|
 *      +-------------------------------+-------------------------------+ -+
 *      |   CHANNEL ID                  |    PACKET SYNC PATTERN        |  |
 *      +-------------------------------+-------------------------------+  |
 *      |                     PACKET LENGTH (bytes)                     |  |
 *      +---------------------------------------------------------------+  |
 *      |                     DATA LENGTH  (bytes)                      |  |
 *      +---------------+---------------+----------------+--------------+  |
 *      |  DATA TYPE    |  PACKET FLAGS |SEQUENCE NUMBER |HEADER VERSION|  |
 *      +---------------+---------------+----------------+--------------+  |
 *      |                RELATIVE TIME COUNTER                          |  |
 *      +-------------------------------+-------------------------------+  +-> Packet Header
 *      |   HEADER CHECKSUM             |    RELATIVE TIME COUNTER      |  |
 *      +-------------------------------+-------------------------------+  |
 *      |                          TIME (LSLW)                          |  |
 *      +---------------------------------------------------------------+  |
 *      |                          TIME (MSLW)                          |  |
 *      +-------------------------------+-------------------------------+  |
 *      |   SECONDARY HEADER CHECKSUM   |           RESERVED            |  |
 *      +-------------------------------+-------------------------------+  |
 *      |                     CHANNEL SPECIFIC DATA                     | -+
 *      +---------------------------------------------------------------+
 *      |      MSG n TT 1 (LSB)         |        MSG n TT 2             | -+
 *      +-------------------------------+-------------------------------+  |
 *      |      MSG n TT 3               |        MSG n TT 4 (MSB)       |  |
 *      +-------------------------------+-------------------------------+  |
 *      |           BSW                 |        GAP TIME               |  +-> Packet Body
 *      +-------------------------------+-------------------------------+  |
 *      |    LENGTH WORD (BYTES)        |        DATA n (CMD)           |  |
 *      +-------------------------------+-------------------------------+  |
 *      |          DATA n + 1           |        DATA n + 2             | -+
 *      +-------------------------------+-------------------------------+
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pIoctlParams       input value
 * In   ppCh10Pkt       output values
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S32BIT mtGetMtiCh10DataPkt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32Pad;
    U16BIT *pu16TimeCntr;
    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    S32BIT s32Timeout = (S32BIT)pIoctlParams->Param1;
    struct _MTI_DATA_LIST_ENTRY *pDataListTailEntry = NULL;
    ACEX_MTI_CH10_DATA_PKT *pCh10Pkt = NULL;
    U16BIT *pPacketData = NULL;
    U32BIT i, ByteCount;
    U8BIT *pwDataTemp = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "ENTER->Channel %d\n", u16Channel);

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    /* If not, go into error check, and wait mode */
    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    if ((pMT->u32MtiDataPktTail == pMT->u32MtiDataPktHead) &&
        (pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail]->State == BUF_STATE_FREE))
    {
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

        /*
            If An Empty List Condition Exists, Check For The Following:

            1. Check For Device Memory Overflow.  This Requires A Restart Of MTI To Corrrect.

            2. Check For A Timout Value. If Not Return Data Unavailable.

            3. If Timeout Value, Wait For Timeout To Expire.
                Then Return Packet Or Timeout Error
        */
        if (pMT->bMtiDeviceOverflow)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Device Overflow\n");
            return MTI_ERR_DEVICE_OVERFLOW;
        }

        if (s32Timeout == 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Data Unavailable\n");
            return MTI_ERR_DATA_UNAVAILABLE;
        }

        /* zero out wait queue condition flag */
        DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
        if (pMT->u8MtiDataPoolEventFlag)
        {
            pMT->u8MtiDataPoolEventFlag--;
        }
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

        /* Sleep and wait for the local ISR or ISR clean-up to wake us up and proceed */
        if (s32Timeout > 0)
        {
            int nResult = 0;

            nResult =  DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                pMT->eMtiDataPoolEvent,
                pMT->u8MtiDataPoolEventFlag,
                s32Timeout);

            if (nResult)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "nResult %d\n", nResult);
            }
            /* If the flag is still '0', the timeout expired */
            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            if (!pMT->u8MtiDataPoolEventFlag)
            {
                DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
                return MTI_ERR_TIMEOUT;
            }
            DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
        }
        else
        {
            DDC_WAIT_INTERRUPTABLE(pMT->eMtiDataPoolEvent, pMT->u8MtiDataPoolEventFlag);
        }
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    }

    /* Check Mti State - If Not Run State The Pool Event Has Been Terminated By The MtiSetState Routine.
     *  Return Without Data */
    if ((pMT->eMtiState != ACEX_MTI_RUN) && (pMT->eMtiState != ACEX_MTI_READY))
    {
        return MTI_ERR_DATA_UNAVAILABLE;
    }

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Mti Data Pkt Head: %d\n", pMT->u32MtiDataPktHead);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Mti Data Pkt Tail: %d\n", pMT->u32MtiDataPktTail);

    pDataListTailEntry = pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail];

    if (pDataListTailEntry->State != BUF_STATE_DATA)
    {
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
        return MTI_ERR_DATA_UNAVAILABLE;
    }
    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    /* If a buffer error condition is flagged, return the error */
    if (pDataListTailEntry->u32ErrorCode)
    {
        return pDataListTailEntry->u32ErrorCode;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Tail Entry MsgCount: %d\n", pDataListTailEntry->u32MsgCount);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "Tail Entry WordCount: %d\n", pDataListTailEntry->u32WordCount);

    /* No errors.  Get the packet */
    pCh10Pkt = (ACEX_MTI_CH10_DATA_PKT *)(pDataListTailEntry->pDataPkt);

    /* Fill packet header data */
    pCh10Pkt->u16PktSyncPattern = MTI_CH10_PKT_SYNC_PATT;
    pCh10Pkt->u16ChannelId = pMT->u16MtiPacketChannelId;
    ByteCount = (pDataListTailEntry->u32WordCount * 2);

    pCh10Pkt->u32PktLength = ByteCount + MTI_CH10_PKT_HEADER_SIZE + MTI_CH_SPEC_DATA_HDR_SIZE;

    /* Get pad value */
    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    pDataListTailEntry = pMT->pMtiDataListEntry[pMT->u32MtiDataPktTail];
    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    u32Pad = pDataListTailEntry->copyOffset;

    /* Get Pointer To Packet Data */
    pPacketData = (U16BIT*) GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER(pCh10Pkt);

    /* If WordCount is odd add filler bytes to packet length */
    if (pDataListTailEntry->u32WordCount % 2)
    {
        pCh10Pkt->u32PktLength += MTI_DATA_PKT_FILLER_SPACE;

        /* Add Fill Word */
        pwDataTemp = (U8BIT*)pPacketData + ByteCount + u32Pad;
        *pwDataTemp = 0;
        *(pwDataTemp + 1) = 0;
    }

    pCh10Pkt->u32DataLength = ByteCount + MTI_CH_SPEC_DATA_HDR_SIZE;
    pCh10Pkt->u16SeqNumHdrVer = pMT->s8MtiDataPacketSeqNumber++;
    pCh10Pkt->u16SeqNumHdrVer = (U16BIT)(((pCh10Pkt->u16SeqNumHdrVer << 8) & 0xFF00) | MTI_CH10_PKT_HEADER_VER);
    pCh10Pkt->u16DatTypePktFlags = ((MTI_CH10_PKT_DATA_TYPE << 8) & 0xFF00);

    /* Replace chapter 10 data type with DDC custom data type if enabled */
    if (pMT->bMtiCustomDataTypeEnable)
    {
        /* If DDC BSW formatting is enabled set data type = 0xF1 */
        if (!pMT->bMtiBswTypeDisable)
        {
            pCh10Pkt->u16DatTypePktFlags = ((MTI_CH10_PKT_DATA_TYPE_MTI << 8) & 0xFF00);
        }

        /* If DDC MTIe is enabled set data type = 0xF2 */
        if (pMT->bMtiErrorMonitorEnable)
        {
            pCh10Pkt->u16DatTypePktFlags = ((MTI_CH10_PKT_DATA_TYPE_MTIE << 8) & 0xFF00);
        }

        /* If DDC MTR is enabled set data type = 0xF3 */
        if (pMT->bMtiReplayMonitorEnable)
        {
            pCh10Pkt->u16DatTypePktFlags = ((MTI_CH10_PKT_DATA_TYPE_MTR << 8) & 0xFF00);
        }
    }

    /* Note that extra pad bytes must be added to conpensate the shifts.  */
    pu16TimeCntr = (U16BIT*)GET_ACEX_MTI_CH10_PKT_TIME_CNTR_POINTER(pCh10Pkt);
    pu16TimeCntr[0] = pPacketData[u32Pad / 2 + 0];
    pu16TimeCntr[1] = pPacketData[u32Pad / 2 + 1];
    pu16TimeCntr[2] = pPacketData[u32Pad / 2 + 2];

    /* Calculate Header Checksum*/
    pCh10Pkt->u16HeaderChksum = 0;

    for (i = 0; i < MTI_CH10_PKT_CHECKSUM_WORD_CNT; i++)
    {
        pCh10Pkt->u16HeaderChksum = (U16BIT)(pCh10Pkt->u16HeaderChksum + ((U16BIT *)pCh10Pkt)[i]);
    }

    pCh10Pkt->u32ChnlSpecificData = MTI_CH10_DATA_PKT_CHANNEL_DATA_TTB | pDataListTailEntry->u32MsgCount;

    /* check for device overflow condition*/
    if (pCh10Pkt->u16DatTypePktFlags & MTI_HW_OVERFLOW_BIT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "overflow occurred... clearing condition \n");

        /* clear condition */
        pMT->bMtiDeviceOverflow = TRUE;

        /* toggle overflow clear bits */

        /*ddcWriteReg32(Card,Channel,REG_MTI_IRQ_CTL_RW ,IMR3_MTI_CTL_OVR_CLR);
           ddcWriteReg32(Card,Channel,REG_MTI_IRQ_CTL_RW ,0); */
    }

    /* For Debug */
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "SYNC %04x ChID %04x PktLen %08x DataLen %08x SeqNumHdrVer %04x DatTypePktFlags %04x \n",
        pCh10Pkt->u16PktSyncPattern,
        pCh10Pkt->u16ChannelId,
        pCh10Pkt->u32PktLength,
        pCh10Pkt->u32DataLength,
        pCh10Pkt->u16SeqNumHdrVer,
        pCh10Pkt->u16DatTypePktFlags);

    pu16TimeCntr = (U16BIT*)GET_ACEX_MTI_CH10_PKT_TIME_CNTR_POINTER(pCh10Pkt);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, " RelTimCntr0 %04x RelTimCntr1 %04x RelTimCntr2 %04x HdrChkSum %04x ChnlSpecificData %08x\n",
        pu16TimeCntr[0], pu16TimeCntr[1], pu16TimeCntr[2],
        pCh10Pkt->u16HeaderChksum,
        pCh10Pkt->u32ChnlSpecificData);

    pPacketData = (U16BIT*) GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER(pCh10Pkt);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "IntraPktTimeStmp0 %04x IntraPktTimeStmp1 %04x IntraPktTimeStmp2 %04x IntraPktTimeStmp3 %04x\n",
        pPacketData[0], pPacketData[1], pPacketData[2], pPacketData[3]);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "BlkStatus %04x  Gap %04x  Len %04x  Cmd %04x\n",
        pPacketData[4], pPacketData[5], pPacketData[6], pPacketData[7]);

/*
    {
        int i;
        U16BIT *pu16Data = &pCh10Pkt->u16MsgData[8];
        int lenWords = pCh10Pkt->u16MsgData[6]/2;

        if (lenWords==0)
            lenWords = 32;

        lenWords--;

        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "  Data\n");
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "   ");
        for (i=0; i<lenWords;i++)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "%04x ", *pu16Data);
            pu16Data++;
            if (!(i%4))
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "\n   ");
        }
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "\n");
    }
 */

    /* Check to see if an error condition exists */
    if (pMT->u32MtiErrorCode)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "An error happened in Mti %d\n", pMT->u32MtiErrorCode);
        pMT->u32MtiErrorCode = 0;

        /* return MTI_ERR_BUFFER_UNAVAILABLE;*/ /* comment out, otherwise Tail will never get freed and no more pkt will be stored */
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT, "EXIT\n");

    return MTI_ERR_NONE;
}

/******************************************************************************
 * Name:    mtFreeDataPktListTail
 *
 * Description:
 *      This function frees tail entry and increment tail to next one.
 *
 * In   pDeviceContext  Device-specific structure
 * In   pIoctlParams        IOCTL structure
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mtFreeDataPktListTail
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32Tail;
    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    u32Tail = pMT->u32MtiDataPktTail;

    /* mark list entry as free*/
    pMT->pMtiDataListEntry[u32Tail]->State = BUF_STATE_FREE;

    /* Increment tail one and roll over in needed */
    pMT->u32MtiDataPktTail++;

    if (pMT->u32MtiDataPktTail == pMT->u32MtiDataPoolCount)
    {
        pMT->u32MtiDataPktTail = 0;
    }

    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
}

/******************************************************************************
 * Name:    mtFreeDataPktList
 *
 * Description:
 *      This function frees the Mti Data List.
 *
 * In   pDeviceContext  Device-specific structure
 * In   u16Channel      channel
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mtFreeDataPktList
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32BufCnt = 0;

    /* see if this device does not have any 1553 channels */
    if (pDeviceContext->u8Num1553Channels == 0)
    {
        /* this device does not support 1553 - exit */
        return;
    }

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    pMT->u32MtiDataPktHead = pMT->u32MtiDataPktTail = 0;
    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    for (u32BufCnt = 0; u32BufCnt < MTI_MAX_NUM_BUF; u32BufCnt++)
    {
        struct _MTI_DATA_LIST_ENTRY *pMtiDataListEntry = pMT->pMtiDataListEntry[u32BufCnt];

        if (pMtiDataListEntry == NULL)
        {
            continue;
        }
#if DDC_DMA_MT
        /* free MT DMA buffer */
        if (pMtiDataListEntry->pDataPkt)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pMtiDataListEntry->mtDMA_Size,
                pMtiDataListEntry->pDataPkt,
                pMtiDataListEntry->mtDMA_Addr,
                DDC_MEMORY_REGION__MT_DMA);

            pMtiDataListEntry->pDataPkt = NULL;
        }
#else
        if (pMtiDataListEntry->pDataPkt)
        {
            DDC_KERNEL_FREE(pDeviceContext, pMtiDataListEntry->pDataPkt);
            pMtiDataListEntry->pDataPkt = NULL;
        }    
#endif /* DDC_DMA_MT */
        pMtiDataListEntry->State = BUF_STATE_FREE;
        pMtiDataListEntry->bOverflow = 0;
        pMtiDataListEntry->u16AlignOffset = 0;
        pMtiDataListEntry->u32ErrorCode = 0;
        pMtiDataListEntry->u32MsgCount = 0;
        pMtiDataListEntry->u32WordCount = 0;
        pMtiDataListEntry->pDataPkt = NULL;
    }

    pMT->u32MtiDataPoolBufferSize = 0;
    pMT->u32MtiDataPoolCount = 0;
    pMT->bMtiLookasideListDataInit = FALSE;
}

/******************************************************************************
 * Name:    mtInterruptHandler
 *
 * Description:
 *      Unified MTi interrupt handler.  The handler first insures Workitem state is free
 *      prior to storing data and Enqueueing for call back.
 *      MTi packet Start Address, Number of Words and Number of Messages
 *      are stored in the work item contex for the call back to retrieve.
 *
 * In   pDeviceContext      input value for instance information associated with this particular device.
 * In   u8Ch                1553 Channel
 * In   u32mtIntStatis      MTi Interrupt Status
 * In   u32StartAddress     MTi packet start address
 * In   u32NumOfWords       Number of words in MTi packet
 * In   u32NumOfMsgs        Number of messages in MTi packet
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
void mtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32mtIntStatus,
    U32BIT u32StartAddress,
    U32BIT u32NumOfWords,
    U32BIT u32NumOfMsgs
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    struct _MTI_DATA_LIST_ENTRY *pMtiEntry = NULL;
    U32BIT u32DmaStartAddress = 0;
    U8BIT *pu8DmaBuf = NULL;
#if (!DDC_DMA_MT)
    BOOLEAN  bRollOverOccurred = FALSE;
#endif /* DDC_DMA_MT */

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "ENTER->\n");

    pMT = &(pDeviceContext->pChannel1553[u8Ch]->sMT);

    if (u32mtIntStatus & MT_MTI_INT_STATUS_ON_OVERFLOW)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "CH %d MTI Data Packet Hardware Overflow Interrupt Occured\n", u8Ch);
        pMT->bMtiDeviceOverflow = TRUE;

        DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
        pMtiEntry = pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead];
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

        pMtiEntry->pDataPkt->u16DatTypePktFlags = (U16BIT)(pMtiEntry->pDataPkt->u16DatTypePktFlags | MTI_HW_OVERFLOW_BIT); /* need? */
        pMT->u32MtiStackOverflowCount++;
    }
    else
    {
        /* 1 Message must at least exist for processing to begins! */
        if (u32NumOfMsgs)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "WORD CNT: %d\n", u32NumOfWords);

            /* Only process a DMA xfer if channel is in RUN state */
            if (pMT->eMtiState != ACEX_MTI_RUN)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "Interrupt when card not in RUN state!\n");
                return;
            }

            if (u32NumOfWords > (pMT->u32MtiDataPoolBufferSize / 2)) /* u32MtiDataPoolBufferSize[] is in byte, but u32NumOfWords is in 16bits ?? by BZ */
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "Word Cnt too large: %d\n", u32NumOfWords);
                return;
            }

            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            if (pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State != BUF_STATE_FREE)
            {

                DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "Mti Data Pkt Head %d not Free, State %d\n", \
                    pMT->u32MtiDataPktHead,                                                     \
                    pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State);

                DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

                /* Flag that all buffers are used */
                pMT->u32MtiErrorCode = MTI_ERR_BUFFER_UNAVAILABLE;

                /* NO DMA will occur */

                /* Inform firmware, free to re-use the space even not read */
                DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_FREE_MEM_COUNT_RW), &u32NumOfWords);
                return;
            }

            /* Update WordCount and Message Count in list entry*/
            pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->u32WordCount = u32NumOfWords; /* words to bytes */
            pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->u32MsgCount = u32NumOfMsgs;

            u32DmaStartAddress = (U32BIT)((u32StartAddress << 1) + ((*(pDeviceContext->pChannel1553[u8Ch]->pu32MemBA) << 2)));                     /* words(16bit) & 32bit to bytes*/
            pu8DmaBuf = (U8BIT *)GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER (((U8BIT*)pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->pDataPkt));

            DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "u32NumOfMsgs %d: u32DmaStartAddress 0x%08x, u32NumOfWords %d\n", u32NumOfMsgs, u32DmaStartAddress, u32NumOfWords);

#if DDC_DMA_MT

            dmaMtSetup(pDeviceContext, u8Ch, u32NumOfWords, u32DmaStartAddress, pu8DmaBuf);
#else
            {
            U32BIT u32WordCountRemaining;
            U32BIT u32WordsToRead;
            U32BIT u32WordAddress;
            U16BIT *pu16ReadBuffer;

            pu16ReadBuffer = (U16BIT *) pu8DmaBuf;

            /* convert the Start Address from a byte address to a word address */
            u32WordAddress = (u32DmaStartAddress >> 1);

            u32WordCountRemaining = u32NumOfWords;

            while (u32WordCountRemaining > 0)
                {
                u32WordsToRead = u32WordCountRemaining;

                /* read maximum of 4k bytes */
                if (u32WordsToRead > (MTI_DWORD_CHUNK_SIZE << 1))
                {
                    /* shift by 1 to convert DWORD value to WORD value */
                    u32WordsToRead = (MTI_DWORD_CHUNK_SIZE << 1);
                }

                /* adjust read size if the read will go beyond the end of the MTI memory (rollover) */
                if ((u32WordAddress + u32WordsToRead) >= (pMT->u32MtiMem32BitEndAddr << 1))
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER,"mtiDataMemRead: rollover, End Address = 0x%08X", pMT->u32MtiMem32BitEndAddr);

                    bRollOverOccurred = TRUE;
                    u32WordsToRead = (pMT->u32MtiMem32BitEndAddr << 1) - u32WordAddress;
                }
                else
                {
                    bRollOverOccurred = FALSE;
                }

                /* read the data from the device */
                DDC_16BIT_BLK_MEM_READ(pDeviceContext, u32WordAddress, pu16ReadBuffer, u32WordsToRead);

                /* update the read buffer to the next location */
                pu16ReadBuffer += u32WordsToRead;

                if (bRollOverOccurred)
                {
                    /* set the next address to read to the start of MTi memory */
                    u32WordAddress = (pMT->u32MtiMem32BitStartAddr << 1);
                }
                else
                {
                    /* update WORD address */
                    u32WordAddress += u32WordsToRead;
                }

                /* subtract off the number of words just read */
                u32WordCountRemaining -= u32WordsToRead;
            }

            DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
            pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State = BUF_STATE_DATA;
                DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

                /* Update MTi metrics. Free count register returns amount of free space in word (U16BIT) units */
            /* Inform firmware, free to re-use space just read from for future writes. */
            /* Must write number of words read to the MTI Free Memory Count register */
                DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_FREE_MEM_COUNT_RW), &u32NumOfWords);

                pMT->u32MtiStackPercentFull = 100 - ((u32NumOfWords * 100) / pMT->u32MtiStackSizeWords);

                if (pMT->u32MtiStackPercentHigh < pMT->u32MtiStackPercentFull)
                {
                    pMT->u32MtiStackPercentHigh = pMT->u32MtiStackPercentFull;
                }

                DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

            /* increment head ptr */
                pMT->u32MtiDataPktHead++;

            /* roll the head pointer to the beginning if necessary */
                if (pMT->u32MtiDataPktHead == pMT->u32MtiDataPoolCount)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "MTI Data Pkt List Rolling over to zero.\n");
                    pMT->u32MtiDataPktHead = 0;
                }

                /* wake up any blocked getch10 packets*/
                if (pMT->u8MtiDataPoolEventFlag < 0xff)
                {
                    pMT->u8MtiDataPoolEventFlag++;
                }

                DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

                DDC_WAKE_UP_INTERRUPTIBLE(&pMT->eMtiDataPoolCallback, &pMT->eMtiDataPoolEvent);
            }
#endif /* DDC_DMA_MT */

        } /* if (u32NumOfMsgs) */
    }     /* else portion of if (u32mtIntStatus & MT_MTI_INT_STATUS_ON_OVERFLOW) */

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_HANDLER, "EXIT\n");
}

/******************************************************************************
 * Name:    mtMtiInit
 *
 * Description:
 *      Initialize MTI Time parameters.
 *
 * In   pDeviceContext  Device-specific structure
 * In   u8Ch            1553 Channel
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtMtiInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    ACEX_1553_CHANNEL_TYPE *pCh;
    U16BIT i;
    U16BIT j;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INIT_MODE, "ENTER-> Channel %d\n", u8Ch);

    pMT = &(pDeviceContext->pChannel1553[u8Ch]->sMT);
    pCh = pDeviceContext->pChannel1553[u8Ch];

    pMT->state = ACEX_MOD_RESET;

    for (i = 0; i < MTI_MAX_NUM_BUF; i++)
    {
        pMT->pMtiDataListEntry[i] = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(*pMT->pMtiDataListEntry[0]));

        if (pMT->pMtiDataListEntry[i] == NULL)
        {
            /* free previously allocated entries */
            for (j = 0; j < MTI_MAX_NUM_BUF; j++)
            {
                if (pMT->pMtiDataListEntry[j])
                {
                    DDC_KERNEL_FREE(pDeviceContext, pMT->pMtiDataListEntry[j]);
                    pMT->pMtiDataListEntry[j] = NULL;
                }
            }

            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        memset(pMT->pMtiDataListEntry[i], 0 , sizeof(*pMT->pMtiDataListEntry[i]));

        pMT->pMtiDataListEntry[i]->pDataPkt = NULL;
    }

    pMT->pMtiTimeListEntry = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(*pMT->pMtiTimeListEntry));

    if (pMT->pMtiTimeListEntry == NULL)
    {
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }


    pMT->pMtiTimeListEntry->State = BUF_STATE_FREE;

    /* Initialize MTI Data Packet Link Lists */
    mtFreeDataPktList(pDeviceContext, u8Ch);

    pDeviceContext->u16MtiChannelCount = 0;

    /* Adjust memory available to user and new start address due to space
       allocation for Lookup Table */
    pCh->u32UserMemSizeBytes -= MT_LKUP_TABLE_MEMORY_BYTE_SIZE;

    /* Due to 32bit address acheme, size must be converted from an 8-bit to 32-bit prior to adding offset */
    pCh->u32UserMemBA += (MT_LKUP_TABLE_MEMORY_BYTE_SIZE / 4);

    pMT->u32BlockOnIrqEnable = FALSE;

    /* Initialize MTi metric values */
    pMT->u32MtiStackPercentFull = 0;
    pMT->u32MtiStackPercentHigh = 0;
    pMT->u32MtiStackOverflowCount = 0;

    /* This should never occur, but initialze size just incase MTi work item gets called without */
    /* being configured 1st */
    pMT->u32MtiStackSizeWords = 1;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INIT_MODE, "EXIT\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtSetStrobeReg
 *
 * Description:
 *      Closes MT operaton.
 *
 *      This function enable's one of the following MTi strobe register functions
 *
 *      MT_STROBE_HOST_TRIG_INT
 *      MT_STROBE_PAUSE_MONITOR
 *      MT_STROBE_START_RESUME_MONITOR
 *      MT_STROBE_RESET_MONITOR
 *
 * In   pDeviceContext  input value for instance information associated with this particular device.
 * In   pIoctlParams       input value
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtSetStrobeReg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32RegisterValue;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_SET_STROBE_REGISTER, "ENTER->\n");

    pMT = &(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT);

    u32RegisterValue = (U32BIT)pIoctlParams->Param1;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_STROBE_W), (U32BIT *) &u32RegisterValue);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtGetMtiCh10Status
 *
 * Description:
 *      Gets The State Of The MTI Engine (RESET, READY, RUN).
 *
 * In   pDeviceContext  Device-specific structure
 * In   pIoctlParams       input values for Channel & State
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtGetMtiCh10Status
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    MT_MTI_INFO *pMtiCh10Info
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U32BIT u32TempHold = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_STATUS, "ENTER->Channel = %d\n", (int)pIoctlParams->Channel);

    pMT = &(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT);

    /* Read the MTi interrupt queue status to transfer First Message Address, */
    /* Total Lenght and Number of messages from the FIFO to their associated registers */ /* TT1162 move the reading here */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_QUEUE_STATUS_RW), &u32TempHold);

    if (pDeviceContext->u16DriverType == ACEX_QPRM_DRIVER)
    {
        /* We read this 3 times to allow for faster Q-Prime acess speed */
        DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_QUEUE_STATUS_RW), &u32TempHold);
        DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_QUEUE_STATUS_RW), &u32TempHold);
    }

    /* read the MTI INT QUEUE STATUS first to make sure MtiCh10Status Info */
    /* has been transferred from the FIFO to their associated registers */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_QUEUE_STATUS_RW), &u32TempHold);

    /* Retrieve Number of Messages in Chapter 10 packet, if available */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_NUMBER_OF_MSGS_RW), &u32TempHold);
    pMtiCh10Info->u32NumOfMsgs = u32TempHold;

    /* Retrieve total length of Chapter 10 packet, if available */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_TOTAL_LENGTH_RW), &u32TempHold);
    pMtiCh10Info->u32TotalLength = u32TempHold;

    /* Retrieve 1st Message Address in Chapter 10 packet, if available */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_FIRST_MSG_ADDRESS_RW), &u32TempHold);
    pMtiCh10Info->u32FirstMsgAddress = u32TempHold;

    if (pMtiCh10Info->u32TotalLength != 0)
    {
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_STATUS, "Number of Messages is %d, Length is %d, Start Address is %X\n",
            pMtiCh10Info->u32NumOfMsgs, pMtiCh10Info->u32TotalLength, pMtiCh10Info->u32FirstMsgAddress);
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtMtiInterruptConfig
 *
 * Description:
 *      Configures the Monitor Terminal-Improved (MTI) Interrupt conditions.
 *
 * In   pDeviceContext  Device-specific structure
 * In   pMtiConfig      input for MT configuration values
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtMtiInterruptConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT u16Channel = pMtiConfig->sConfigID.u16Channel;
    U32BIT u32RegData = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_CONFIG, "ENTER->Channel : %d\n", u16Channel);

    pMT = &(pDeviceContext->pChannel1553[u16Channel]->sMT);

    /* Initialize MTI Interrupt Mask Register */
    u32RegData = pMtiConfig->u32IntConditions;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_ENABLE_RW), &u32RegData);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_CONFIG, "MTi Interrupt Register set with the following value %X\n", pMtiConfig->u32IntConditions);

    /* Initialize MTI Interrupt Word Count */
    u32RegData = pMtiConfig->u32IrqDataLen;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_NUMBER_OF_WORDS_RW), &u32RegData);

    /* Initialize MTI Interrupt Message Count */
    u32RegData = pMtiConfig->u32IrqMsgCnt;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_NUMBER_OF_MSGS_RW), &u32RegData);

    /* Initialize MTI Interrupt Time Count */
    u32RegData = (U32BIT)pMtiConfig->u16IrqTimeInterval;
    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_INT_SET_TIME_INTERVAL_RW), &u32RegData);

    /* Set 1553 channel specific interrupt mask */
    gen1553InterruptSet(pDeviceContext, (U8BIT)u16Channel, GENERAL_INT_MASK_MT_INT_ENABLED);

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_INT_CONFIG, "EXIT, Word Count: %d, Msg Count: %d, Time Value %d\n",
        pMtiConfig->u32IrqDataLen, pMtiConfig->u32IrqMsgCnt, pMtiConfig->u16IrqTimeInterval);

    if (pMtiConfig->u32BlockOnIrqEnable)
    {
        pMT->u32BlockOnIrqEnable = TRUE;
    }
    else
    {
        pMT->u32BlockOnIrqEnable = FALSE;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mtMtiGetMetrics
 *
 * Description:
 *      Return MTi Metrics.
 *
 * In   pDeviceContext  Device-specific structure
 * In   pIoctlParams       Input value
 * In   pMtiConfig      Metric results storage
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mtMtiGetMetrics
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    MT_MTI_METRICS *pMtiMetrics
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;

    pMT = &(pDeviceContext->pChannel1553[pIoctlParams->Channel]->sMT);

    /* Update MTi metrics. Free count register returns amount of free space in word (U16BIT) units */
    /* Convert to percent values */
    pMtiMetrics->u32MtiStackPercentHigh =
        ((U32BIT)100 - ((pMT->u32MtiStackPercentHigh * (U32BIT)100) / pMT->u32MtiStackSizeWords));

    pMtiMetrics->u32MtiStackPercentFull =
        ((U32BIT)100 - ((pMT->u32MtiStackPercentFull * (U32BIT)100) / pMT->u32MtiStackSizeWords));

    pMtiMetrics->u32MtiStackOverflowCount = pMT->u32MtiStackOverflowCount;

    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_METRICS, "Channel %d Percent Full: %d\n", (int)pIoctlParams->Channel, pMtiMetrics->u32MtiStackPercentFull);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_METRICS, "Channel %d Percent High: %d\n", (int)pIoctlParams->Channel, pMtiMetrics->u32MtiStackPercentHigh);
    DDC_DBG_PRINT(DDC_DBG_MODULE_MT, DDC_DBG_MT_GET_METRICS, "Channel %d Overflow Count: %d\n", (int)pIoctlParams->Channel, pMtiMetrics->u32MtiStackOverflowCount);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlMtCleanup
 *
 * Description:
 *      This function performs all necessary MT cleanup.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlMtCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _ACEX_1553_MT_TYPE *pMT = NULL;
    U16BIT i;
    U8BIT u8Ch;

    for (u8Ch = 0; u8Ch < pDeviceContext->u8Num1553Channels; u8Ch++)
    {
        pMT = &(pDeviceContext->pChannel1553[u8Ch]->sMT);

        for (i = 0; i < MTI_MAX_NUM_BUF; i++)
        {
            if (pMT->pMtiDataListEntry[i])
            {
                DDC_KERNEL_FREE(pDeviceContext, pMT->pMtiDataListEntry[i]);
                pMT->pMtiDataListEntry[i] = NULL;
            }
        }

        if (pMT->pMtiTimeListEntry)
        {
            DDC_KERNEL_FREE(pDeviceContext, pMT->pMtiTimeListEntry);
        }
    }
}
