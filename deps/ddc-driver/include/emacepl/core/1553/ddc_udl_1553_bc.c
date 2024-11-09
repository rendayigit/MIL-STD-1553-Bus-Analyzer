/*******************************************************************************
 * FILE: ddc_udl_bc.c
 *
 * DESCRIPTION:
 *
 *  This module provides interface to the Bus Controller (BC) hardware and the
 *  BC Run-Time Library.
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
#include "os/include/ddc_os_types_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_bc.h"
#include "core/1553/ddc_udl_1553_bc_private.h"


#define DDC_1553_ADDR_SHIFT     1

/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */

static void bcInitValues
(
    struct _ACEX_1553_BC_TYPE *pBC
);

static S16BIT _bcImpStop
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

static S16BIT _bcGpqUpdate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

static void _bcCalculateHBufMetric
(
    struct _ACEX_1553_BC_TYPE *pBC
);

static S16BIT _bcEnableHBufMetric
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
);

static U32BIT _bcGetUserIrqEvent
(
    U32BIT u32IntStatus
);


/*******************************************************************************
 * Name:    bcInterruptHandler
 *
 * Description:
 *      The BC Interrupt handler processes UIRQ4 and TTAG_ROLLOVER
 *      interrupts. Upon receiving the above interrupts, IMP will be stopped
 *      to trigger a host initiated interrupt from IMP, which in turn allowing
 *      driver to fetch messages and process them.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * In   u32IntStatus    BC interrrupt status
 * In   u32GenIntStatus general interrrupt status
 *
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void bcInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntStatus,
    U32BIT u32GenIntStatus
)
{
    U32BIT u32IrqEv;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pCh->sBC);
    U32BIT u32ToGetMsg = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* convert INT status to user INT flags*/
    u32IrqEv = _bcGetUserIrqEvent(u32IntStatus);

    /* get out if there is no interested interrupt bits */
    if (!(u32IntStatus & BC_INT_STS_MASK))
    {
        /* inform user directly */
        pCh->u32IrqEv |= u32IrqEv;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - NO BC interrupt, status = 0x%08x\n",
            u16Ch, (unsigned int)u32IntStatus);

        return;
    }

    /* skip the interrupt if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* inform user directly */
        pCh->u32IrqEv |= u32IrqEv;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - BC is OFF, status = 0x%08x\n",
            u16Ch, (unsigned int)u32IntStatus);

        return;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - BC is OFF Pending, status = 0x%08x\n",
            u16Ch, u32IntStatus);

        return;
    }

    /* skip the interrupt if BC HBuf/msgBuf is not turned on */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF) && !(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* inform user directly */
        pCh->u32IrqEv |= u32IrqEv;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - BC Host/User Buffer is not Installed, status = 0x%08x\n",
            u16Ch, u32IntStatus);

        return;
    }

    /* Test if this is a TT interrupt */
    if (u32GenIntStatus & BC_INT_STS_TTAG_ROLLOEVR)
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_INT_TTAG_ROLLOVER)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
                "CH%d - TT ROLLOVER occurred!\n", u16Ch);

            /* TT interrupt will not be needed so the following code is commented out
             * u32ToGetMsg++;
             */
        }
        else
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
                "CH%d - TT ROLLOVER OFF!\n", u16Ch);
        }
    }

    /* test if this is a BC msg interrupt */
    if ((u32IntStatus & (BC_INT_MASK_UIRQ0 | BC_INT_MASK_UIRQ1 | BC_INT_MASK_UIRQ2 | BC_INT_MASK_UIRQ3 | BC_INT_MASK_UIRQ4)) ||
        (u32IrqEv & (ACE_IMR1_BC_MSG_EOM | ACE_IMR1_EOM)))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_INT_UIRQ4)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
                "CH%d - UIRQ4 occurred!\n", u16Ch);

            u32ToGetMsg++;
        }
        else
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
                "CH%d - UIRQ4 is turned OFF!\n", u16Ch);
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* test if this is an Async Msg INT */
    if (u32IntStatus & BC_INT_STS_ASYNC)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - Async INT occurred!\n", u16Ch);

        u32ToGetMsg++;
    }

    /* prepare to get messages from IMP module if there is a valid BC interrupt */
    if (u32ToGetMsg)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - Stop IMP!\n", u16Ch);

        /* increment INT count */
        pBC->u32IntCount++;

        /* stop IMP directly now */
        status = _bcImpStop(pDeviceContext, u16Ch);

        if ((DDC_UDL_ERROR__SUCCESS == status) && (u32IrqEv & (ACE_IMR2_BC_UIRQ0 | ACE_IMR2_BC_UIRQ1 | ACE_IMR2_BC_UIRQ2 | ACE_IMR2_BC_UIRQ3)))
        {
            /* There will be an IMP interrupt for an user interrupt. As such, we will hold
               user interrupt information until the IMP interrupt service is completed.

               To make sure that the IMP interrupt is serviced in the next interrupt
               service, the IrqEv information is saved into u32IrqEvThis. While in the
               IMP interrupt service, the u32IrqEvLast will be popped up to user when some
               msgs are moved into the HBuf. */
            pBC->u32IrqEvThis |= u32IrqEv;
        }
        else
        {
            /* Otherwise, inform user directly */
            pCh->u32IrqEv |= u32IrqEv;
        }
    }
    else
    {
        /* There will be no IMP interrupt, so inform user directly*/
        pCh->u32IrqEv |= u32IrqEv;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_HANDLER,
            "CH%d - Not served BC INT 0x%08x!\n",
            u16Ch, u32IntStatus);
    }
}

/*******************************************************************************
 * Name:    bcImpInterruptHandler
 *
 * Description:
 *      This is the BC Improvement Interrupt handler. In this function, msgs are
 *      read from IMP in an unit of 4K block transfer. All msgs are processed according
 *      to HostID. In the end, IMP will be re-started and user interrupts will be
 *      returned.
 *
 * In pDeviceContext    device-specific structure
 * In u16Ch             1553 channel number
 * In u32ImpIntStatus   IMP interrupt status
 *
 * In u32NumEntries     number of msgs in the IMP
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcImpInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32ImpIntStatus
)
{
    U32BIT u32NumMsgs;
    U32BIT u32NumDWds;
    U32BIT u32StartAddress;
    U32BIT u32NumMsgsToGet;
    U32BIT u32NumWords;
    U32BIT u32NumEntries;

#if BC_IMP_MSG_METRIC_DBG
    U32BIT u32NumEntriesFromRegB;
    S32BIT s32MsgDiff;
#endif /* BC_IMP_MSG_METRIC_DBG */

    U8BIT *pBuf;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pCh->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
        "ENTER->CH%d - BC IMP INT occurred\n", u16Ch);

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);

    /* skip the interrupt if BC mode is not turned on */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - BC is OFF, status = 0x%08x\n", u16Ch, u32ImpIntStatus);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* exit if BC HBuf/msgBuf is not installed*/
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF) && !(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - BC Host/User Buffer is not installed\n", u16Ch);

        return DDC_UDL_ERROR__SUCCESS;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - BC is OFF Pending, status = 0x%08x\n", u16Ch, u32ImpIntStatus);

        /* Wake up the blocked thread so it can terminate */
        DDC_WAKE_UP_INTERRUPTIBLE(&pBC->waitqueueMsgCallback, &pBC->waitqueueMsgEvent);
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* set BC BUSY mode */
    pBC->u32ModeFlag |= BC_MODE_FLAG_INT_BUSY;

    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* calculate number of entries */
    u32NumWords = (u32ImpIntStatus >> BC_IMP_WORD_COUNT_SHIFT) & 0xFFFF;
    u32NumEntries = u32NumWords / (pBC->u32CmdDataDwords << 1);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
        "u32NumEntries %d u32NumWords %d\n", u32NumEntries, u32NumWords);

#if BC_IMP_MSG_METRIC_DBG

    DDC_BLK_REG_READ(pDeviceContext, *(pCh->sImpBC.pu32RegBA) + ACEX_1553_IMP_REG_OUT_FIFO_STATUS, &u32NumEntriesFromRegB, 1);

    u32NumEntriesFromRegB &= 0xFF;
    pBC->u32MsgCountFromRegB += u32NumEntriesFromRegB;

    if (pBC->u32MaxMsg < u32NumEntries)
    {
        pBC->u32MaxMsg = u32NumEntries;
    }

    if (pBC->u32MaxMsgFromRegB < u32NumEntriesFromRegB)
    {
        pBC->u32MaxMsgFromRegB = u32NumEntriesFromRegB;
    }

    s32MsgDiff = u32NumEntriesFromRegB - u32NumEntries;

    if (pBC->s32MaxDiff < s32MsgDiff)
    {
        pBC->s32MaxDiff = s32MsgDiff;
    }

    if (pBC->s32MinDiff > s32MsgDiff)
    {
        pBC->s32MinDiff = s32MsgDiff;
    }

    if (u32NumEntriesFromRegB != u32NumEntries)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - RegB Msg# %d, Words Msg# %d\n", u16Ch, u32NumEntriesFromRegB, u32NumEntries);
    }

    if ((u32NumEntriesFromRegB >= 174) || (u32NumEntries >= 174))
    {
        pBC->u32FullCount++;
    }

    if (s32MsgDiff < 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - Last msg# %d, Msg# RegB/E %d/%d, WordsTxed %d\n",
            u16Ch, pBC->u32MsgCount, u32NumEntriesFromRegB, u32NumEntries, u32NumWords);
    }

#endif /* BC_IMP_MSG_METRIC_DBG */

    /* increment IMP INT and msg counts */

    pBC->u32ImpIntCount++;
    pBC->u32MsgCount += u32NumEntries;

    /* get buffer pointer*/
    pBuf = pBC->sTempBuf.pu8hBuf;
    if (!pBuf)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "CH%d - BC temp Buffer is not available\n", u16Ch);
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* get the IMP start address from IMP structure  */
    u32StartAddress = *pCh->pu32MemBA + (pCh->sImpBC.sImpCfgReg.u32TgtMemBA >> 1);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
        "pCh->pu32MemBA : 0x%x - pCh->sImpBC.sImpCfgReg.u32TgtMemBA >> 1 : 0x%x, StartAddr = 0x%x\n",
        *pCh->pu32MemBA, pCh->sImpBC.sImpCfgReg.u32TgtMemBA >> 1, u32StartAddress);

    /* fetch all messages in IMP */
    u32NumMsgs = u32NumEntries;

    while (u32NumMsgs) /* There should be no loop for Linux PC104+, since driver have large enough TempBuf */
    {
        /* fetch msgs that the tempBuf can hold*/
        if (u32NumMsgs > pBC->sTempBuf.u32Entries)
        {
            u32NumMsgsToGet = pBC->sTempBuf.u32Entries;
        }
        else
        {
            u32NumMsgsToGet = u32NumMsgs;
        }

        u32NumDWds = u32NumMsgsToGet * pBC->u32CmdDataDwords;

#if (DDC_DMA_BC)

        /* setup DMA xfer to read msgs to pBuf from IMP */

        dmaImpBcSetup(pDeviceContext, (U8BIT)u16Ch, u32NumDWds << 2, u32StartAddress << 2, pBuf, u32NumMsgsToGet);

#else

        /* read msgs to pBuf from IMP */
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
            "DDC_BLK_MEM_READ : CH%d - NumMsgs = %d, StartAddr = 0x%x\n", u16Ch, u32NumMsgs, u32StartAddress);

        status = DDC_BLK_MEM_READ(pDeviceContext, u32StartAddress, (U32BIT *)pBuf, u32NumDWds, ACEX_32_BIT_ACCESS);
        if (status)
        {
            /* get out if hardware has problem */
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_HANDLER,
                "CH%d - NumMsgs = %d, StartAddr = %d\n", u16Ch, u32NumMsgs, u32StartAddress);

            break;
        }

        /* save all msgs to buffers */
        bcImpMsgProcess(pBC, pBuf, u32NumMsgsToGet);

#endif /* DDC_DMA_BC */

        /* read more msgs in the next loop */
        u32NumMsgs -= u32NumMsgsToGet;
        u32StartAddress += u32NumDWds;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);

    /* clear BUSY from BC mode*/
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_INT_BUSY;

    /* turn BC OFF if it is in OFF Pending mode */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_USER_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

#if (DDC_DMA_BC)
    if (0 == u32NumEntries)
    {
#endif /* DDC_DMA_BC */

    /* re-start IMP */
    status = bcImpStart(pDeviceContext, u16Ch);

#if (DDC_DMA_BC)
}

#endif /* DDC_DMA_BC */

    if (u32NumEntries)
    {
        /* Wake up the blocked thread so it can terminate */
        pBC->wMsgEventCond = 1;
        DDC_WAKE_UP_INTERRUPTIBLE(&pBC->waitqueueMsgCallback, &pBC->waitqueueMsgEvent);
    }

    /* shift interrupt event information */
    pBC->u32IrqEvLast = pBC->u32IrqEvThis;
    pBC->u32IrqEvThis = 0;

    /* Inform user the interrupt information after some delay even without msgs */
    pCh->u32IrqEv |= pBC->u32IrqEvLast;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _bcGetUserIrqEvent
 *
 * Description:
 *      This function converts the u32IntStatus to the user defined IRQ event.
 *
 * In   u32IntStatus    interrrupt status
 *
 * Out  none
 *
 * Returns: user defined IRQ event
 ******************************************************************************/
static U32BIT _bcGetUserIrqEvent
(
    U32BIT u32IntStatus
)
{
    U32BIT u32IrqEv = 0;

    if (u32IntStatus & REG_BC_STATUS_SET)
    {
        u32IrqEv |= ACE_IMR1_BC_STATUS_SET;
    }

   if (u32IntStatus & REG_BC_SELECT_EOM)
    {
        u32IrqEv |= ACE_IMR1_BC_MSG_EOM;
    }

    if (u32IntStatus & REG_BC_RETRY)
    {
        u32IrqEv |= ACE_IMR1_BC_RETRY;
    }

    if (u32IntStatus & REG_BC_IRQ_0)
    {
        u32IrqEv |= ACE_IMR2_BC_UIRQ0;
    }

    if (u32IntStatus & REG_BC_IRQ_1)
    {
        u32IrqEv |= ACE_IMR2_BC_UIRQ1;
    }

    if (u32IntStatus & REG_BC_IRQ_2)
    {
        u32IrqEv |= ACE_IMR2_BC_UIRQ2;
    }

    if (u32IntStatus & REG_BC_IRQ_3)
    {
        u32IrqEv |= ACE_IMR2_BC_UIRQ3;
    }

    if (u32IntStatus & REG_BC_TRAP)
    {
        u32IrqEv |= ACE_IMR2_BC_TRAP;
    }

    if (u32IntStatus & REG_BC_CALL_STACK_ERROR)
    {
        u32IrqEv |= ACE_IMR2_BC_CALLSTK_ERR;
    }

    if (u32IntStatus & REG_BC_OPCODE_PARITY_ERROR)
    {
        u32IrqEv |= ACE_IMR2_BC_OPCODE_PARITY;
    }

    if (u32IntStatus & REG_BC_EOM)
    {
        u32IrqEv |= ACE_IMR1_EOM;
    }

    if (u32IntStatus & REG_BC_FORMAT_ERROR)
    {
        u32IrqEv |= ACE_IMR1_FORMAT_ERR;
    }

    if (u32IntStatus & REG_BC_GP_QUEUE_ROLLOVER)
    {
        u32IrqEv |= ACE_IMR2_GPQ_ISQ_ROVER;
    }

    return u32IrqEv;
}

/*******************************************************************************
 * Name:    bcImpMsgProcess
 *
 * Description:
 *      This function processes the msgs fetched from IMP accoding to the HostID
 *      given in the control block that indicates the message types:
 *             BC_HOST_ID_MSG
 *             BC_HOST_ID_STREAM
 *             BC_HOST_ID_ARRAY
 *      If a message is the data-stream or data-array message, the message will be
 *      counted and then discarded.
 *
 *      If a message is the normal message, the message will be saved into msgBuf
 *      & dataBuf indexed by the msgIndex and dataIndex, and/or HBuf if HBuf is
 *      installed. The msgIndex and dataIndex are located in the BC Control and
 *      Status Block.
 *
 *      When HBuf is full, msg loss count is incremented, and both the head and
 *      tail index advances to keep the fresh msgs in the buffer.
 *
 * In   pBC        - point to BC strucutre
 * In   pTempBuf   - pointer to the temp buffer that has all msgs read from IMP
 * In   u32NumMsgs - number of msgs in the tempBuffer
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void bcImpMsgProcess
(
    struct _ACEX_1553_BC_TYPE *pBC,
    U8BIT *pTempBuf,
    U32BIT u32NumMsgs
)
{
    U32BIT i;
    ACEX_BC_CTRLWRDS *pCtrlWords;
    ACEX_BC_DATASTR *pDataStr;
    ACEX_BC_DATA_ARRAY *pDataArray;

    U8BIT u8HostId;

    U16BIT u16MsgIdx;
    U16BIT u16DataIdx;

    U16BIT u16DataStrID;
    U16BIT u16DataArrayID;

    U8BIT *pMsgBuf = NULL;
    U8BIT *pDataBuf = NULL;
    U8BIT *pHBuf = NULL;
    U8BIT *pTempBufAddr = pTempBuf;
    U8BIT *pBufIndexed = NULL;

    U32BIT u32CmdBytes = pBC->u32CmdDwords << 2;
    U32BIT u32DataBytes = pBC->u32DataDwords << 2;

    U32BIT u32CmdDataBytes = pBC->u32CmdDataDwords << 2;

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);

    /* get msg and host buffer pointers*/
    if (pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF)
    {
        pMsgBuf = pBC->sMsgBuf.hBuf;
        pDataBuf = pBC->sDataBuf.hBuf;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_HBUF)
    {
        pHBuf = pBC->sHBuf.hBuf;
    }

    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* loop through all messages fetched from IMP */
    for (i = 0; i < u32NumMsgs; i++)
    {
        pCtrlWords = (ACEX_BC_CTRLWRDS *)pTempBufAddr;

		/* Avoid breaking strict-aliasing rules [-Wstrict-aliasing] */
        *(U16BIT*)(&(pCtrlWords->u8HostId)) = (U16BIT)DDC_BYTE_ORDER_S(*(U16BIT*)(&(pCtrlWords->u8HostId)));
        u8HostId = pCtrlWords->u8HostId;

        /* process each feature according to HostID */
        switch (u8HostId)
        {
            case BC_HOST_ID_STREAM:
            {
                /* get data stream ID */
                u16DataStrID = pCtrlWords->u16MsgIdx;

                /* process data stream messages */
                if ((u16DataStrID > 0) && (u16DataStrID < NUM_DATASTR_CHANNELS))
                {
                    /* get dataStr spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slDataStr), pBC->slDataStrFlag);

                    /* check data streaming direction */
                    if ((pCtrlWords->u8Dir & BC_CTRLWRDS_DATASTR_DIR_MASK) == BC_CTRLWRDS_DATASTR_DIR_TX)
                    {
                        /* data stream out from RT1 */
                        pDataStr = &pBC->sDataStrChan[u16DataStrID].sSnd;
                    }
                    else
                    {
                        /* data stream in to RT1 */
                        pDataStr = &pBC->sDataStrChan[u16DataStrID].sRcv;
                    }

                    /* increment msg count */
                    pDataStr->u16MsgCount++;

                    /* set the first error only */
                    if ((pDataStr->u16MsgErrBsw == 0) && (pCtrlWords->u16BlkStsWord & BC_BSW_ERROR_FLAG))
                    {
                        pDataStr->u16MsgErrLocation = pDataStr->u16MsgCount;
                        pDataStr->u16MsgErrBsw = pCtrlWords->u16BlkStsWord;
                    }

                    /* release dataStr spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slDataStr), pBC->slDataStrFlag);
                }

                break;
            }

            case BC_HOST_ID_ARRAY:
            {
                /* get data array ID */
                u16DataArrayID = pCtrlWords->u16MsgIdx;

                /* process data stream messages */
                if ((u16DataArrayID > 0) && (u16DataArrayID < NUM_DATARRAY_CHANNELS))
                {
                    /* get dataArray spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

                    /* point to data array information */
                    pDataArray = &pBC->sDataArrayChan[u16DataArrayID].sSnd;

                    /* increment available msgs to post */
                    if (pDataArray->u16MsgToPostAvail < pDataArray->u16MsgToPostMax)
                    {
                        pDataArray->u16MsgToPostAvail++;
                    }
                    else
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                            "Too many data array messages\n");
                    }

                    /* set the first error only */
                    if ((pDataArray->u16MsgErrBsw == 0) && (pCtrlWords->u16BlkStsWord & BC_BSW_ERROR_FLAG))
                    {
                        pDataArray->u16MsgErrLocation = pDataArray->u16MsgToPostAvail;
                        pDataArray->u16MsgErrBsw = pCtrlWords->u16BlkStsWord;
                    }

                    /* release dataArray spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);
                }

                break;
            }

            case BC_HOST_ID_HP:
            case BC_HOST_ID_LP:
            case BC_HOST_ID_MSG:
            default:
            {
                if (u8HostId == BC_HOST_ID_LP)
                {
                    /* get LPQ spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slLpq), pBC->slLpqFlag);

                    /* Decrement Lp Msg count */
                    if (pBC->u16LpMsgCount)
                    {
                        pBC->u16LpMsgCount--;
                    }

                    /* release LPQ spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);
                }

                if (u8HostId == BC_HOST_ID_HP)
                {
                    /* get HPQ spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slHpq), pBC->slHpqFlag);

                    /* Decrement Hp Msg count */
                    if (pBC->u16HpMsgCount)
                    {
                        pBC->u16HpMsgCount--;
                    }

                    /* release HPQ spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);
                }

                /* save the msg into msgBuf and dataBuf (should be always installed)*/
                DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
                if (pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF)
                {
                    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

                    /* get msg and dataBuf index */
                    u16MsgIdx = pCtrlWords->u16MsgIdx;
                    u16DataIdx = pCtrlWords->u16DataIdx;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                        "MsgIdx %d, DataIdx %d\n", u16MsgIdx, u16DataIdx);

                    /*---------------------------------------------
                            save msg header to msgBuf
                       -----------------------------------------------*/

                    /* get MsgBuf spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slMsgBuf), pBC->slMsgBufFlag);

                    /* check msgBuf index range */
                    if (u16MsgIdx >= pBC->sMsgBuf.u32MaxMsgIdx)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                            "MsgBuf Index = %d, out of range!\n", u16MsgIdx);

                        /* skip this msg */
                    }
                    else
                    {
                        /* copy the msg to msgBuf */
                        pBufIndexed = pMsgBuf + u16MsgIdx * u32CmdBytes;
                        memcpy((void *)pBufIndexed, (void *)pTempBufAddr, u32CmdBytes);
                    }

                    /* release MsgBuf spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slMsgBuf), pBC->slMsgBufFlag);

                    /*---------------------------------------------
                            save msg data to dataBuf
                       -----------------------------------------------*/

                    /* get DataBuf spin lock*/
                    DDC_ISR_LOCK_TAKE((pBC->slDataBuf), pBC->slDataBufFlag);

                    /* check dataBuf index range */
                    if (u16DataIdx >= pBC->sDataBuf.u32MaxDataIdx)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                            "dataBuf index out of range %d!\n", u16DataIdx);

                        /* skip this data block */
                    }
                    else
                    {
                        /* copy the data to dataBuf */
                        pBufIndexed = pDataBuf + u16DataIdx * u32DataBytes;
                        memcpy((void *)pBufIndexed, (void *)(pTempBufAddr + u32CmdBytes), u32DataBytes);
                    }

                    /* release MsgBuf spin lock*/
                    DDC_ISR_LOCK_GIVE((pBC->slDataBuf), pBC->slDataBufFlag);
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
                }

                /* save the msg into HBuf if it is installed */
                DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
                if (pBC->u32ModeFlag & BC_MODE_FLAG_HBUF)
                {
                    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

                    /*---------------------------------------------
                            save msg to Hbuf
                       -----------------------------------------------*/

                    /* get HBuf spin lock */
                    DDC_ISR_LOCK_TAKE((pBC->slHBuf), pBC->slHBufFlag);

                    /* copy msg to HBuf */
                    pBufIndexed = pHBuf + (pBC->sHBuf.u32Tail << 2); /* byte index */
                    memcpy((void *)pBufIndexed, (void *)pTempBufAddr, u32CmdDataBytes);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                        "pBuf 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", \
                        *pTempBuf,
                        *(pTempBuf + 1),     *(pTempBuf + 2),     *(pTempBuf + 3),     *(pTempBuf + 4),
                        *(pTempBuf + 5),     *(pTempBuf + 6),     *(pTempBuf + 7),     *(pTempBuf + 8),
                        *(pTempBuf + 9),     *(pTempBuf + 10),    *(pTempBuf + 11),    *(pTempBuf + 12),
                        *(pTempBuf + 13),    *(pTempBuf + 14),    *(pTempBuf + 15),    *(pTempBuf + 16),
                        *(pTempBuf + 17),    *(pTempBuf + 18),    *(pTempBuf + 19),    *(pTempBuf + 20));

                    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_MSG_PROCESS,
                        "Hbuf 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", \
                        *pBufIndexed,
                        *(pBufIndexed + 21),     *(pBufIndexed + 22),     *(pBufIndexed + 23),     *(pBufIndexed + 24),
                        *(pBufIndexed + 25),     *(pBufIndexed + 26),     *(pBufIndexed + 27),     *(pBufIndexed + 28),
                        *(pBufIndexed + 29),     *(pBufIndexed + 30),    *(pBufIndexed + 31),    *(pBufIndexed + 32),
                        *(pBufIndexed + 33),    *(pBufIndexed + 34),    *(pBufIndexed + 35),    *(pBufIndexed + 36),
                        *(pBufIndexed + 37),    *(pBufIndexed + 38),    *(pBufIndexed + 39),    *(pBufIndexed + 40));

                    /* increment Host buffer tail index */
                    pBC->sHBuf.u32Tail += pBC->u32CmdDataDwords;

                    if (pBC->sHBuf.u32Tail >= pBC->sHBuf.u32DwordsSize)
                    {
                        /* roll back to the top of the buffer if it is at the bottom */
                        pBC->sHBuf.u32Tail = 0;
                    }

                    /* When HBuf is full, increment msg loss count, and increment both the
                       head and tail index to keep the fresh msgs in the buffer */
                    if (pBC->sHBuf.u32Count >= pBC->sHBuf.u32NumEntries)
                    {
                        pBC->sHBuf.u32Lost++;
                        pBC->sHBuf.u32Head = pBC->sHBuf.u32Tail;
                    }
                    else
                    {
                        pBC->sHBuf.u32Count++;
                    }

                    /* release HBuf spin lock */
                    DDC_ISR_LOCK_GIVE((pBC->slHBuf), pBC->slHBufFlag);
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
                }

                break;
            }
        }

        /* next msg */
        pTempBufAddr += u32CmdDataBytes;
    }
}

/*******************************************************************************
 * Name:    bcInterruptSet
 *
 * Description:
 *      This function adds the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *      Interrupt bit to be set should be set to 1 in the u32IntMask
 *      parameter.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * In   u32IntMask      interrupt mask bits to add
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntMask
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* update local copy */
    pBC->u32IntMask |= u32IntMask;

    status = DDC_REG_WRITE(pDeviceContext, ((*(pBC->pu32RegBA)) + REG_BC_INT_MASK), &(pBC->u32IntMask));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_SET,
        "REG:%08x VALUE:%08x\n",
        (int)((*(pBC->pu32RegBA)) + REG_BC_INT_MASK), (int)pBC->u32IntMask);

    return status;
}

/*******************************************************************************
 * Name:    bcInterruptClear
 *
 * Description:
 *      This function clears the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *      Interrupt bit to be cleared should be set to 1 in the u32IntMask
 *      parameter.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * In   u32IntMask      interrupt mask bits to add
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntMask
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* update local copy */
    pBC->u32IntMask &= ~u32IntMask;

    status = DDC_REG_WRITE(pDeviceContext, ((*(pBC->pu32RegBA)) + REG_BC_INT_MASK), &(pBC->u32IntMask));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INTERRUPT_SET,
        "REG:%08x VALUE:%08x\n",
        (int)((*(pBC->pu32RegBA)) + REG_BC_INT_MASK), (int)pBC->u32IntMask);

    return status;
}

/*******************************************************************************
 * Name:    bcImpInterruptSet
 *
 * Description:
 *      This function sets interrupt mask for BC operations. The possible interrupts
 *      Set IMP interrupt mask to get interrupt when
 *          1. host initiated interrupt     -- BC Host buffer, typical
 *          2. number queue transfer        -- BC polling message/data
 *          3. number of words interrupt    -- BC Host buffer, words limit
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT bcImpInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Desc[3]; /* IMP WORK QUEUE DESCRIPTOR:
                            0 = INT mask,
                            1 = number of words,
                            2 = number of msgs */

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* calculate IMP interrupt number of queue transferred.
       The full mark is currently set to xx percent of the queue size (256).

       Note: it is unnecessary to set both words and queue interrupt conditions
       together if they mean the same thing, such as in the current case. The
       reason to set both here is unique to USB. In order to make blk register
       write, the address has to be contiguous, such as now IMP register 3, 4,
       and 5. As such, number of words interrupt is also opened and set here.
       It is redundent!
     */
    pBC->sHBuf.u32ImpQueueFullMark = BC_IMP_OUTPUT_QUEUE_FULL_MARK;
    pBC->sHBuf.u32ImpIntMsgs = (BC_IMP_OUTPUT_QUEUE_ENTRY * pBC->sHBuf.u32ImpQueueFullMark) / 100;
    pBC->sHBuf.u32ImpIntWords = pBC->sHBuf.u32ImpIntMsgs * (pBC->u32CmdDataDwords << 1);

    /*
       Set IMP interrupt mask to get interrupt when
        1. host initiated interrupt          -- BC Host buffer use, typical
        2. number of words trfrs interrupt   -- BC Host buffer use, output words limit to prevent overflow
        3. number of msgs txfrs interrupt    -- BC Host buffer use, output queue limit to prevent overflow
     */
    pCh->sImpBC.sImpCfgReg.u32IntMaskEn |= ACEX_1553_IMP_INT_EN_MASK_HOST_INITIATE | ACEX_1553_IMP_INT_EN_MASK_NUM_QUEUE_TFRS | ACEX_1553_IMP_INT_EN_MASK_NUM_WDS;

    u32Desc[0] = pCh->sImpBC.sImpCfgReg.u32IntMaskEn;
    u32Desc[1] = pBC->sHBuf.u32ImpIntWords;
    u32Desc[2] = pBC->sHBuf.u32ImpIntMsgs;
    status = DDC_BLK_REG_WRITE(pDeviceContext, ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK), u32Desc, 3);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_SET,
        "REG:%08x(IMP INT EN) VALUE:%08x, INT Words: 0x%08x, INT Msgs: 0x%08x\n",
        (int)((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK),
        (int)u32Desc[0], u32Desc[1], u32Desc[2]);

    return status;
}

/*******************************************************************************
 * Name:    bcImpInterruptClear
 *
 * Description:
 *      This function clears interrupt mask for BC IMP operations.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
S16BIT bcImpInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Data;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* clear interrupt number of words*/
    pBC->sHBuf.u32ImpIntMsgs = 0;
    pBC->sHBuf.u32ImpIntWords = 0;

    /* We will remove BC HBuf specific interrupt masks.
       ACEX_1553_IMP_INT_EN_MASK_NUM_QUEUE_TFRS interrupt
       stays since it is also used by RT */
    pCh->sImpBC.sImpCfgReg.u32IntMaskEn &= ~(ACEX_1553_IMP_INT_EN_MASK_HOST_INITIATE | ACEX_1553_IMP_INT_EN_MASK_NUM_QUEUE_TFRS | ACEX_1553_IMP_INT_EN_MASK_NUM_WDS);

    u32Data = pCh->sImpBC.sImpCfgReg.u32IntMaskEn;
    status = DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_INTERRUPT_CLEAR,
        "REG:%08x VALUE:%08x\n",
        (int)((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK),
        (int)pCh->sImpBC.sImpCfgReg.u32IntMaskEn);

    return status;
}

/*******************************************************************************
 * Name:    bcImpStart
 *
 * Description:
 *      This function brings IMP module back to operational state.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
S16BIT bcImpStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Data;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* skip the interrupt if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* set the IMP-block-trigger bit to IMP's control register */
    u32Data = ACEX_1553_IMP_BLK_MASK_BLK_TRIGGER;
    status = DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_START,
        "REG:%08x VALUE:%08x\n",
        (int)((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG),
        (int)u32Data);

    return status;
}

/*******************************************************************************
 * Name:    _bcImpStop
 *
 * Description:
 *      This function stops IMP module. An IMP interrupt will be triggered to
 *      driver when IMP module is ready for data fetch.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT _bcImpStop
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Data;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* skip the interrupt if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* set the host-initiated-interrupt bit to IMP's control register */
    u32Data = ACEX_1553_IMP_BLK_MASK_HOST_INITIATE;
    status = DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG), &u32Data);

    return status;
}

/*******************************************************************************
 * Name:    bcImpOpen
 *
 * Description:
 *      This function opens the improvement component to let BC to control IMP
 *      input queue. Due to the limitation of the current IMP hardware, the host
 *      or BC driver will not able to send any commands to IMP module.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcImpOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Data;
    U32BIT u32Addr;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (pCh->sImpBC.state != ACEX_MOD_RESET)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* initialize members to zero */
    memset(&(pCh->sImpBC.stats), 0, sizeof(ACEX_1553_IMP_STATS_TYPE));

    /* reset improvements */
    if ((pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP) ||
        (pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP))
    {
        u32Data = (BD_RESET_1553_CH0_IMP_BCMRT << (u16Ch * 4));

        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, u32Data);
    }
    else
    {
        u32Data = (BD_RESET_1553_CH0_IMP_BC << (u16Ch * 2));

        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_MF, u32Data);
    }

    /* adjust memory available to user to exclude improvements memory area */
    u32Addr = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_BA);
    DDC_REG_WRITE(pDeviceContext, u32Addr, &(pCh->sImpBC.sImpCfgReg.u32TgtMemBA));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_OPEN,
        "WRITE IMP REG (TARGET MEM BA):%08x VALUE:%08x\n",
        (int)u32Addr, (int)pCh->sImpBC.sImpCfgReg.u32TgtMemBA);

    u32Addr = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_SIZE);
    u32Data = pCh->sImpBC.sImpCfgReg.u32TgtMemSzDWD * 2;
    DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_OPEN,
        "WRITE IMP REG (TARGET MEM SIZE):%08x VALUE:%08x\n",
        (int)u32Addr, (int)pCh->sImpBC.sImpCfgReg.u32TgtMemSzDWD);

    /* Following bits are set/cleared for BC operation:
        1. bit0 = 1: IMP waits for EOM bit set before transfer a message
        2. bit1 = 1: Reset Output Fifo when Reading Reg B
        3. bit3 = 0: let BC component to control IMP module
     */
    u32Data = IMP_IGNORE_TXRX_WAIT_FOR_EOM | IMP_IGNORE_TXRX_RESET_OUT_FIFO | IMP_IGNORE_TXRX_BC_INPUT_CTRL;
    u32Addr = *pCh->sImpBC.pu32RegBA + ACEX_1553_IMP_REG_IGNORE_TXRX_DATA;

    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_OPEN,
        "WRITE 1553 REG:%08x DATA:%08x\n",
        (unsigned int)u32Addr, (unsigned int)u32Data);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_IMP_OPEN,
            "CH%d IMP int Mask 0x%08x\n", u16Ch, (unsigned int)u32Data);

        return status;
    }

    pCh->sImpBC.sImpCfgReg.u32IgnoreDataTfr = u32Data; /* store size locally */

    /* Set Bd Interrupt Bits to Enable Improvements Ints */
    pCh->sImpBC.u32BdIntMask = (BD_INT_STATUS_MASK_1553_0 << u16Ch) | BD_INT_STATUS_MASK_INT_REQ;

    /* set 1553 channel specific interrupt mask */
    pCh->sImpBC.u321553ChIntMask = GENERAL_INT_MASK_IMP_INT_ENABLED | GENERAL_INT_MASK_BC_IMP_INT_ENABLED | GENERAL_INT_MASK_REPLAY_INT_ENABLED;

    /* enable IMP master interrupt mask in the 1553 General Component */
    gen1553InterruptSet(pDeviceContext, (U8BIT)u16Ch, pCh->sImpBC.u321553ChIntMask);

    pCh->sImpBC.state = ACEX_MOD_OPEN;

    return status;
}

/*******************************************************************************
 * Name:    bcImpClose
 *
 * Description:
 *      This function closes the improvement component from BC.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void bcImpClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];

    /* disable IMP master interrupt mask in the 1553 General Component */
    gen1553InterruptClear(pDeviceContext, (U8BIT)u16Ch, pCh->sImpBC.u321553ChIntMask);

    pCh->sImpBC.state = ACEX_MOD_RESET;
}

#if BC_ASYNC_DEBUG

/*******************************************************************************
 * Name:    bcRegE
 *
 * Description:
 *      This function read and display BC Reg E.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcRegE
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    char *msg
)
{
    U32BIT u32Addr;
    U32BIT u32Value;
    S16BIT status;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);

    /* Debug Reg E*/
    u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_STS;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REG_E,
            "CH%d - Read Reg E\n", u16Ch);

        return status;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REG_E,
        "READ REG:%08x VALUE:%08x - %s\n", u32Addr, u32Value, msg);

    return status;
}

#endif /* BC_ASYNC_DEBUG */

/*******************************************************************************
 * Name:    bcSetIntForBufOperation
 *
 * Description:
 *      This function sets interrupt bits for Host and Msg/Data buffer operations.
 *
 * In   pDeviceContext      device-specific structure
 * In   psBuf               point to buffer information
 *       sConfigID          u16Type | u16Channel
 *       u32BufferSize      not used
 *       u32UserIrqMask     HBuf BC INT Mask  - UIRQ4
 *       u32DevIrqMask      HBuf GEN INT Mask - TTAG_ROLLOVER
 * Out pMyBuf               pointer to the my buffer information. Only first 4 fields are used!
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcSetIntForBufOperation
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_BC_BUF_SETTINGS *psBuf,
    ACEX_BC_MSGBUF *pMyBuf
)
{
    U16BIT u16Ch = psBuf->sConfigID.u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32IntFlag = 0;

    /* BC interrupt mask */
    u32IntFlag = psBuf->u32UserIrqMask & ACEX_BC_UIRQ4;

    if (u32IntFlag)
    {
        pMyBuf->u32IntMask = (u32IntFlag << BC_INT_MASK_UIRQ_SHIFT);

        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        pMyBuf->u32ModeFlag |= (u32IntFlag << BC_MODE_FLAG_UIRQ_SHIFT);
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
    }

    pMyBuf->u32IntMask |= BC_INT_STS_ASYNC; /* add LP/HP INT always */

    /* enable BC interrupts */
    status = bcInterruptSet(pDeviceContext, (U8BIT)u16Ch, (pMyBuf->u32IntMask));

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_SET_INT_FOR_BUF_OP,
            "bcInterruptSet Fail - CH%d Mask 0x%08x\n", u16Ch, (int)pMyBuf->u32IntMask);

        return status;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= pMyBuf->u32ModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* GEN interrtupt mask */
#if 0   /* remove TT interrupt */
    u32IntFlag = psBuf->u32DevIrqMask & ACEX_IRQ_TT_ROLLOVER;

    if (u32IntFlag)
    {
        pMyBuf->u32GenIntMask = (u32IntFlag << BC_INT_MASK_TT_ROLLOVER_SHIFT);
        pMyBuf->u32GenModeFlag |= (u32IntFlag << BC_MODE_FLAG_TT_ROLLOVER_SHIFT);
    }
#endif

    pMyBuf->u32GenIntMask = 0;
    pMyBuf->u32GenModeFlag = 0;

    pMyBuf->u32GenIntMask |= GENERAL_INT_STATUS_BC_INT_ENABLED; /* add BC INT always */

    /* enable GEN interrupts */
    status = gen1553InterruptSet(pDeviceContext, (U8BIT)u16Ch, pMyBuf->u32GenIntMask);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_SET_INT_FOR_BUF_OP,
            "gen1553InterruptSet Fail - CH%d Mask 0x%08x\n", u16Ch, (int)pMyBuf->u32GenIntMask);

        return status;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= pMyBuf->u32GenModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcHbufEnableAction
 *
 * Description:
 *      This function enables HBuf. When this function failed, bcHbufDisable
 *      must be called to clean up the configuration.
 *
 * In   pDeviceContext  device-specific structure
 * In   psBuf           point to buffer structure as following
 *                          sConfigID       u16Type | u16Channel
 *                          u32BufferSize   HBuf size (4K-4M word)
 *                          u32UserIrqMask  HBuf BC INT Mask  - UIRQ4
 *                          u32DevIrqMask   HBuf GEN INT Mask - TTAG_ROLLOVER
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcHbufEnableAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_BC_BUF_SETTINGS *psBuf
)
{
    U32BIT u32HbufWordSize;

    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U16BIT u16Ch = psBuf->sConfigID.u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_ENABLE_ACTION,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_ENABLE_ACTION,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return if BC HBuf is already enabled */
    if (pBC->u32ModeFlag & BC_MODE_FLAG_HBUF)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_ENABLE_ACTION,
            "CH%d - HBuf already enabled\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_HBUF_ENABLED;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* delete already allocated memory if it is not yet*/
    if (pBC->u32BufFlag & BC_HBUF_FLAG_INIT)
    {
        DDC_KERNEL_VIRTUAL_FREE(pDeviceContext, pBC->sHBuf.hBuf);
        pBC->u32BufFlag &= ~BC_HBUF_FLAG_INIT;
    }

    /* set HBuf information */
    memset(&pBC->sHBuf, 0, sizeof(ACEX_BC_HBUF));

    /* validate HBuf size in the range of 4K - 4M words */
    u32HbufWordSize = psBuf->u32BufferSize;

    if (u32HbufWordSize < BC_HBUF_MIN)
    {
        u32HbufWordSize = BC_HBUF_MIN;
    }

    if (u32HbufWordSize > BC_HBUF_MAX)
    {
        u32HbufWordSize = BC_HBUF_MAX;
    }

    pBC->sHBuf.u32NumEntries = (u32HbufWordSize >> 1) / pBC->u32CmdDataDwords; /* remove unused dwords */
    pBC->sHBuf.u32DwordsSize = pBC->sHBuf.u32NumEntries * pBC->u32CmdDataDwords;

    /* allocate memory */
    pBC->sHBuf.hBuf = DDC_KERNEL_VIRTUAL_MALLOC(pDeviceContext, pBC->sHBuf.u32DwordsSize << 2);

    if (!pBC->sHBuf.hBuf)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_ENABLE_ACTION,
            "DDC_KERNEL_VIRTUAL_MALLOC for HBuf - CH%d bufSize=0x%d DWords\n", u16Ch, (unsigned int)pBC->sHBuf.u32DwordsSize);

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    pBC->u32BufFlag |= BC_HBUF_FLAG_INIT;

    /* set interrupt bits */
    status = bcSetIntForBufOperation(pDeviceContext, psBuf, (void*) &pBC->sHBuf);

    if (status)
    {
        return status;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_HBUF;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcHbufEnable
 *
 * Description:
 *      This function enables HBuf. Note that UserIrq and DevIrq information is
 *      not given by RTL. Instead, it is hard coded in driver. If bcHbufEnable
 *      is failed, bcHbufDisable must be called to clean up the partial settings.
 *
 * In   pDeviceContext  device-specific structure
 * In   pParams         pointer to IO command from user
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcHbufEnable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
)
{
    ACEX_BC_BUF_SETTINGS sBuf;
    S16BIT status;

    ACEX_BC_HBUF_INSTALL_INFO *psHBufInfo = (ACEX_BC_HBUF_INSTALL_INFO *)pParams;
    sBuf.sConfigID.u16Channel = psHBufInfo->sConfigID.u16Channel;
    sBuf.u32BufferSize = psHBufInfo->u32BufferSize;
    sBuf.u32UserIrqMask = ACEX_BC_UIRQ4;
    sBuf.u32DevIrqMask = ACEX_IRQ_TT_ROLLOVER;

    status = bcHbufEnableAction(pDeviceContext, &sBuf);

    if (status)
    {
        bcHbufDisable(pDeviceContext, pParams);
    }
    else
    {
        /* enable Metric always  */
        _bcEnableHBufMetric(pDeviceContext, pParams);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcHbufDisableAction
 *
 * Description:
 *      This function disables HBuf.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcHbufDisableAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32IntMask;
    U32BIT u32ModeFlag;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    S16BIT status1 = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_DISABLE_ACTION,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_HBUF;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* disable BC interrupts */
    u32IntMask = pBC->sHBuf.u32IntMask & (~pBC->sMsgBuf.u32IntMask); /* clear the bits not used in Msg buffer */

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    u32ModeFlag = pBC->sHBuf.u32ModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    status = bcInterruptClear(pDeviceContext, u16Ch, u32IntMask);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_DISABLE_ACTION,
            "bcInterruptClear\n");

        /* fall through. Do not return */
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~(u32ModeFlag & (~pBC->sMsgBuf.u32ModeFlag));
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* disable GEN interrupts*/
    u32IntMask = pBC->sHBuf.u32GenIntMask & (~pBC->sMsgBuf.u32GenIntMask); /* clear the bits not used in Msg buffer */

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    u32ModeFlag = pBC->sHBuf.u32GenModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    status1 = gen1553InterruptClear(pDeviceContext, (U8BIT)u16Ch, u32IntMask);

    if (status1)
    {
        status |= status1;
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF_DISABLE_ACTION,
            "gen1553InterruptClear\n");

        /* fall through. Do not return */
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~(u32ModeFlag & (~pBC->sMsgBuf.u32GenModeFlag));
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* free HBuf memory*/
    if (pBC->u32BufFlag & BC_HBUF_FLAG_INIT)
    {
        DDC_KERNEL_VIRTUAL_FREE(pDeviceContext, pBC->sHBuf.hBuf);

        /* get mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slHBuf), pBC->slHBufFlag);

        pBC->u32BufFlag &= ~BC_HBUF_FLAG_INIT;

        /* release mutex*/
        DDC_ISR_LOCK_GIVE(pBC->slHBuf, pBC->slHBufFlag);
    }

    /* zero HBuf structure */
    memset(&pBC->sHBuf, 0, sizeof(ACEX_BC_HBUF));

    return status;
}

/*******************************************************************************
 * Name:    bcHbufDisable
 *
 * Description:
 *      This function disables HBuf.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcHbufDisable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
)
{
    ACEX_BC_BUF_SETTINGS *psBuf = (ACEX_BC_BUF_SETTINGS *) pParams;
    U16BIT u16Ch = psBuf->sConfigID.u16Channel;

    return bcHbufDisableAction(pDeviceContext, u16Ch);
}

/*******************************************************************************
 * Name:    bcMsgBufEnableAction
 *
 * Description:
 *      This function enables MsgBuf. When this function failed, bcMsgBufFree
 *      must be called to clean up the partial settings.
 *
 * In   pDeviceContext  device-specific structure
 * In   psBuf           point to buffer information
 *          sConfigID       - u16Type | u16Channel
 *          u32BufferSize   - msgBuf size (number of msgs)
 *          u32UserIrqMask  - msgBuf BC INT Mask  - UIRQ4
 *          u32DevIrqMask   - msgBuf GEN INT Mask - TTAG_ROLLOVER
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcMsgBufEnableAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_BC_BUF_SETTINGS *psBuf
)
{
    U16BIT u16Ch = psBuf->sConfigID.u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_ENABLE_ACTION,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_ENABLE_ACTION,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return if BC msgBuf is already enabled */
    if (pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_ENABLE_ACTION,
            "CH%d - msgBuf already enabled\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_HBUF_ENABLED;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* delete already allocated memory if it is still there*/
    if (pBC->u32BufFlag & BC_MSGBUF_FLAG_INIT)
    {
        DDC_KERNEL_FREE(pDeviceContext, pBC->sMsgBuf.hBuf);
        DDC_KERNEL_FREE(pDeviceContext, pBC->sDataBuf.hBuf);
        pBC->u32BufFlag &= ~BC_MSGBUF_FLAG_INIT;
    }

    /* zero msgBuf structure */
    memset(&pBC->sMsgBuf, 0, sizeof(ACEX_BC_MSGBUF));

    /* set max msg index and dwords for the msgBuf */
    pBC->sMsgBuf.u32MaxMsgIdx = psBuf->u32BufferSize;
    pBC->sMsgBuf.u32Dwords = pBC->sMsgBuf.u32MaxMsgIdx * pBC->u32CmdDwords;

    /* allocate memory */
    pBC->sMsgBuf.hBuf = DDC_KERNEL_MALLOC(pDeviceContext, pBC->sMsgBuf.u32Dwords << 2);

    if (!pBC->sMsgBuf.hBuf)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_ENABLE_ACTION,
            "msgBuf - CH%d bufSize=0x%d DWords\n", u16Ch, (int)pBC->sMsgBuf.u32Dwords);

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    /* zero dataBuf structure */
    memset(&pBC->sDataBuf, 0, sizeof(ACEX_BC_DATABUF));

    /* set max dataID and dwords for the dataBuf */
    pBC->sDataBuf.u32MaxDataIdx = ACEX_BC_MAX_INDEX_DATA;
    pBC->sDataBuf.u32Dwords = pBC->sDataBuf.u32MaxDataIdx * pBC->u32DataDwords;

    /* allocate memory */
    pBC->sDataBuf.hBuf = DDC_KERNEL_MALLOC(pDeviceContext, pBC->sDataBuf.u32Dwords << 2);

    if (!pBC->sDataBuf.hBuf)
    {
        DDC_KERNEL_FREE(pDeviceContext, pBC->sMsgBuf.hBuf);
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_ENABLE_ACTION,
            "dataBuf - CH%d bufSize=0x%d DWords\n", u16Ch, (int)pBC->sDataBuf.u32Dwords);

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    pBC->u32BufFlag |= BC_MSGBUF_FLAG_INIT;

    /* set interrupt bits */
    status = bcSetIntForBufOperation(pDeviceContext, psBuf, (ACEX_BC_MSGBUF *) &pBC->sMsgBuf);

    if (status)
    {
        return status;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_MSGBUF;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcMsgBufCreate
 *
 * Description:
 *      This function creates msgBuf. The function is called automatically in driver
 *      regardless if HBuf is installed.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcMsgBufCreate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    ACEX_BC_BUF_SETTINGS sBuf;

    sBuf.sConfigID.u16Channel = u16Ch;
    sBuf.u32BufferSize = ACEX_BC_MAX_INDEX_MSG;
    sBuf.u32UserIrqMask = ACEX_BC_UIRQ4;
    sBuf.u32DevIrqMask = ACEX_IRQ_TT_ROLLOVER;

    return bcMsgBufEnableAction(pDeviceContext, &sBuf);
}

/*******************************************************************************
 * Name:    bcMsgBufFree
 *
 * Description:
 *      This function frees msgBuf. When Msg buffer is freed, the BC functionality
 *      will be freed!
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcMsgBufFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    S16BIT status1 = DDC_UDL_ERROR__SUCCESS;

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_MSGBUF;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* disable BC interrupts */
    status = bcInterruptClear(pDeviceContext, u16Ch, pBC->sMsgBuf.u32IntMask);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_FREE,
            "bcInterruptClear Fail\n");


        /* fall through. Do not return */
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~pBC->sMsgBuf.u32ModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* disable GEN interrupts*/
    status1 = gen1553InterruptClear(pDeviceContext, (U8BIT)u16Ch, pBC->sMsgBuf.u32GenIntMask);

    if (status1)
    {
        status |= status1;
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG_BUF_FREE,
            "gen1553InterruptClear Fail\n");


        /* fall through. Do not return */
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~pBC->sMsgBuf.u32GenModeFlag;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* free msgBuf memory*/
    if (pBC->u32BufFlag & BC_MSGBUF_FLAG_INIT)
    {
        /* get mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slMsgBuf), pBC->slMsgBufFlag);

        DDC_KERNEL_FREE(pDeviceContext, pBC->sMsgBuf.hBuf);

        /* release mutex*/
        DDC_ISR_LOCK_GIVE(pBC->slMsgBuf, pBC->slMsgBufFlag);

        /* get mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slDataBuf), pBC->slDataBufFlag);

        DDC_KERNEL_FREE(pDeviceContext, pBC->sDataBuf.hBuf);

        /* release mutex*/
        DDC_ISR_LOCK_GIVE(pBC->slDataBuf, pBC->slDataBufFlag);

        pBC->u32BufFlag &= ~BC_MSGBUF_FLAG_INIT;
    }

    /* zero msgBuf structure */
    memset(&pBC->sMsgBuf, 0, sizeof(ACEX_BC_MSGBUF));

    /* zero dataBuf structure */
    memset(&pBC->sDataBuf, 0, sizeof(ACEX_BC_DATABUF));

    return status;
}

/*******************************************************************************
 * Name:    bcTempBufCreate
 *
 * Description:
 *      This function creates temp buf to get data from IMP.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcTempBufCreate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* initialize tempBuf information */
    pBC->sTempBuf.u32DWords = BC_TEMPBUF_DWORDS;
    pBC->sTempBuf.u32Entries = pBC->sTempBuf.u32DWords / pBC->u32CmdDataDwords;

    pBC->sTempBuf.pu8hBuf = DDC_KERNEL_MALLOC(pDeviceContext, pBC->sTempBuf.u32DWords << 2);

    if (!pBC->sTempBuf.pu8hBuf)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_TEMP_BUF,
            "CH%d TempBuf = %d DWords, entries = %d\n", u16Ch, (int)pBC->sTempBuf.u32DWords, (int)pBC->sTempBuf.u32Entries);

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_TEMP_BUF,
        "CH%d TempBuf = 0x%d DWords, entries = %d\n", u16Ch, (int)pBC->sTempBuf.u32DWords, (int)pBC->sTempBuf.u32Entries);

    pBC->u32BufFlag |= BC_TEMPBUF_FLAG_INIT;

    return status;
}

/*******************************************************************************
 * Name:    bcTempBufFree
 *
 * Description:
 *      This function frees temp buf.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void bcTempBufFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);

    /* free tempBuf information */
    if (pBC->u32BufFlag & BC_TEMPBUF_FLAG_INIT)
    {
        DDC_KERNEL_FREE(pDeviceContext, pBC->sTempBuf.pu8hBuf);

        /* zero all HBuf information */
        memset(&pBC->sTempBuf, 0, sizeof(ACEX_BC_TEMPBUF));
    }

    pBC->u32BufFlag &= ~BC_TEMPBUF_FLAG_INIT;
}

/*******************************************************************************
 * Name:    _bcEnableHBufMetric
 *
 * Description:
 *      This function enable BC Metric.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT _bcEnableHBufMetric
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
)
{
    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pParams)->u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return if BC HBuf is already enabled */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - HBuf is not enabled\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_HBUF_ENABLED;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    pBC->sHBuf.u32MetricEna = TRUE;

    return status;
}

/*******************************************************************************
 * Name:    bcGetBufMsg
 *
 * Description:
 *      This function reads one message from a Buf at given location and
 *      puts the message into the given msg return buffer.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Index        index to the message location in HBuf
 * In   u32DataType     data type
 *                          BC_MSG_FULL     - return full message
 *                          BC_MSG_CTRLSTS  - return CTRLSTS block only
 *                          BC_MSG_DATA     - return data block only
 * In   u32WordOffset   words offset location to get data
 * Out  pRtnData        pointer to the return buffer
 *
 * Returns: none
 ******************************************************************************/
static void bcGetBufMsg
(
    U32BIT *pBuf,
    U32BIT u32Index,
    U32BIT u32DataType,
    U32BIT u32WordOffset,
    U16BIT *pRtnData
)
{
    ACEX_BC_CTRLWRDS *pCtrlWords;
    int i;
    U32BIT  *pIndexedBuf;
    U16BIT  *pu16SrcData;
    U32BIT u32Bytes;

    U16BIT  *pu16Data = (U16BIT *)pRtnData;

    pIndexedBuf = pBuf + u32Index;

    if (u32DataType & BC_MSG_ID)
    {
        /* fill in the msgIdx*/
        pCtrlWords = (ACEX_BC_CTRLWRDS *)pIndexedBuf;
        *pu16Data = pCtrlWords->u16MsgIdx;
        pu16Data++;
    }

    /* pack cmd and status into 10 words by getting rid of the high 16 bits */
    if (u32DataType & BC_MSG_CTRLSTS)
    {
        for (i = 0; i < BC_CMD_STS_DWORDS; i++)
        {
#if DDC_PPC
            *pu16Data = (U16BIT)(((*pIndexedBuf) & 0xFFFF0000) >> 16);
#else
            *pu16Data = (U16BIT)((*pIndexedBuf) & 0xFFFF);
#endif
            pu16Data++;
            pIndexedBuf++;
        }
    }

    /* copy data from the words offset*/
    if (u32DataType & BC_MSG_DATA)
    {
        if (u32WordOffset > BC_DATA_DWORDS)
        {
            u32WordOffset = BC_DATA_DWORDS;
        }

        pIndexedBuf = pBuf + u32Index + BC_CMD_STS_DWORDS;
        pu16SrcData = (U16BIT *)pIndexedBuf;
        pu16SrcData += u32WordOffset;
        u32Bytes = ((BC_DATA_DWORDS << 1) - u32WordOffset) << 1;

        memcpy((void *)pu16Data, (void *)pu16SrcData, u32Bytes);
    }
}

/*******************************************************************************
 * Name:    bcGetMsgHeader
 *
 * Description:
 *      This function reads the message header from msgBuf at given msgID.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           channel number
 * In   u32MsgIdx       msgBuf index
 * In   u32Blocking     set to be in blocking call behavior
 * In   u32CheckingImp  set to stop IMP to get message
 * In   u32Purge        set to clear BSW
 * Out  pRtnData        pointer to the return buffer
 * Out  pDataInx        data index to return
 *
 * Returns: none
 ******************************************************************************/
static void bcGetMsgHeader
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32MsgIdx,
    U32BIT u32Blocking,
    U32BIT u32CheckingImp,
    U32BIT u32Purge,
    U16BIT *pRtnData,
    U16BIT *pDataInx
)
{
    U8BIT *pMsgBuf;
    U8BIT *pIndexedBuf;
    U32BIT u32DwordIndex;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);

    ACEX_BC_CTRLWRDS *psCtrlWrd;

    /* return without message if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* set data index to 0 if it is terminated */
        *pDataInx = 0;
        return;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* get msgBuffer location */
    pMsgBuf = pBC->sMsgBuf.hBuf;

    /* locate the message */
    u32DwordIndex = u32MsgIdx * pBC->u32CmdDwords;

    /* get data index */
    pIndexedBuf = pMsgBuf + (u32DwordIndex << 2);
    psCtrlWrd = (ACEX_BC_CTRLWRDS *)(pIndexedBuf);

    if (u32CheckingImp)
    {
        /* There is no new message in the msgBuffer if BlkSts is 0.
           But, do not return to user yet. see if IMP has message */
        if (psCtrlWrd->u16BlkStsWord == 0)
        {
            /* stop IMP directly now */
            _bcImpStop(pDeviceContext, u16Ch);

            if (u32Blocking)
            {
                /* wait for the msg event */
                pBC->wMsgEventCond = 0;
                DDC_WAIT_INTERRUPTABLE(pBC->waitqueueMsgEvent, pBC->wMsgEventCond);
            }
        }
    }

    /* get msgBuf mutex */
    DDC_ISR_LOCK_TAKE((pBC->slMsgBuf), pBC->slMsgBufFlag);

    bcGetBufMsg((U32BIT *)pMsgBuf, u32DwordIndex, BC_MSG_CTRLSTS, 0, pRtnData);

    if (pBC->u32MsgEventMsgIdx == u32MsgIdx)
    {
        /* get data index */
        *pDataInx = psCtrlWrd->u16DataIdx;
    }
    else
    {
        /* set data index to 0 if it is terminated */
        *pDataInx = 0;
    }

    /* clear BSW if u32Purge is set */
    if (u32Purge)
    {
        psCtrlWrd->u16BlkStsWord = 0;
    }

    /* release msgBuf mutex */
    DDC_ISR_LOCK_GIVE((pBC->slMsgBuf), pBC->slMsgBufFlag);
}

/*******************************************************************************
 * Name:    bcGetMsgData
 *
 * Description:
 *      This function reads one message data from dataBuf.
 *
 * In   pBC             point to BC structure
 * In   u32DataIdx      dataBuf index
 * In   u32WordOffset   words offset location to get data
 * In   u32WordCount    words count of data
 * Out  pRtnData        pointer to the return buffer
 *
 * Returns: none
 ******************************************************************************/
static void bcGetMsgData
(
    struct _ACEX_1553_BC_TYPE *pBC,
    U32BIT u32DataIdx,
    U32BIT u32WordOffset,
    U32BIT u32WordCount,
    U16BIT *pRtnData
)
{
    U8BIT *pDataBuf;
    U8BIT *pIndexedBuf;
    U32BIT u32Bytes;

    /* get msgBuffer location */
    pDataBuf = pBC->sDataBuf.hBuf;

    /* locate the message */
    pIndexedBuf = pDataBuf + ((u32DataIdx * pBC->u32DataDwords) << 2) + (u32WordOffset << 1);
    u32Bytes = u32WordCount << 1;

    /* get MsgBuf spin lock */
    DDC_ISR_LOCK_TAKE((pBC->slDataBuf), pBC->slDataBufFlag);

    memcpy((void *)pRtnData, (void *)pIndexedBuf, u32Bytes);

    /* release MsgBuf spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slDataBuf), pBC->slDataBufFlag);
}

/*******************************************************************************
 * Name:    bcGetHBufMsg
 *
 * Description:
 *      This function return messages from HBuf to user. The HBuf has head
 *      and tail index for reading msgs to user and recording new msgs from
 *      the device respectively.
 *
 * In   pDeviceContext      device-specific structure
 * In   pIoctlParams              pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     set to return MsgId
 *          Param2:     types
 *                          0 NEXT_PURGE    - get the next msg and advance head index by one
 *                          1 NEXT_NPURGE   - get the next msg and do not change head index
 *                          2 LAST_PURGE    - get the last msg and reset head to tail
 *                          3 LATEST_NPURGE - get the last msg and do not change tail index
 *                          4 FILL_BUFFER   - fill buffer as many msgs as possible begining at the next msg
 *          Param3:     size of buffer in words, used only in FILL_BUFFER case
 *          Param4:     not used
 * Out  pBytesReturned      bytes returned
 * Out  pRdData             point to data buffer
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetHBufMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U32BIT *pHBuf;
    U16BIT *pRtnBuf;
    U32BIT u32MsgIndex;
    U32BIT u32NumOfMsgs;
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);

    U32BIT u32ReturnMsgId = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32Type = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_TYPE);
    U32BIT u32Blocking = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_BLOCKING);
    U32BIT u32CheckingImp = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_CHECK_IMP);
    U32BIT u32WordCount = DDC_IOCTL_U32(pIoctlParams->Param3);
    U32BIT u32CmdDataWords = pBC->u32HBufMsgWords;
    U32BIT u32DataType = BC_MSG_FULL;

    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    *pBytesReturned = 0;

    /* exit if it is  NULL pointer */
    if (pRdData == NULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - pRdData = NULL\n", u16Ch);

        return DDC_UDL_ERROR__NULL_PTR;
    }

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC HBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - BC HBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - BC is OFF Pending\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pBC->u32ModeFlag |= BC_MODE_FLAG_USER_BUSY;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    pHBuf = (U32BIT *)pBC->sHBuf.hBuf;

    /* check if we want to return msgId */
    if (u32ReturnMsgId)
    {
        u32CmdDataWords++;
        u32DataType |= BC_MSG_ID;
    }

    /* make sure the buffer can hold at least one message */
    if (u32WordCount < u32CmdDataWords)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "Insufficient buffer size: msgSize = %d, bufferSize = %d words\n",
            u32CmdDataWords, (unsigned int)u32WordCount);

        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        return DDC_UDL_ERROR__BUFFER_SIZE;
    }

    if (u32CheckingImp)
    {
        if (pBC->sHBuf.u32Count == 0)
        {
            /* stop IMP directly now */
            _bcImpStop(pDeviceContext, u16Ch);

            if (u32Blocking)
            {
                /* wait for the msg event */
                pBC->wMsgEventCond = 0;
                DDC_WAIT_INTERRUPTABLE(pBC->waitqueueMsgEvent, pBC->wMsgEventCond);
            }
            else
            {
                /* return without message */
                DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
                pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;
                DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
                return status;
            }
        }
    }

    pRtnBuf = (U16BIT *)pRdData;
    u32NumOfMsgs = 0;

    /* get HBuf spin lock */
    DDC_ISR_LOCK_TAKE((pBC->slHBuf), pBC->slHBufFlag);

    _bcCalculateHBufMetric(pBC);

    switch (u32Type)
    {
        case BC_HBUF_READ_NEXT_PURGE:
        case BC_HBUF_READ_NEXT_NPURGE:
        {
            /* get the next unread msg */
            if (pBC->sHBuf.u32Count)
            {
                /* get the next message */
                bcGetBufMsg(pHBuf, pBC->sHBuf.u32Head, u32DataType, 0, pRtnBuf);

                /* check if want to purge from HBuf */
                if (u32Type == BC_HBUF_READ_NEXT_PURGE)
                {
                    /* decrement msg count in the HBuf */
                    pBC->sHBuf.u32Count--;

                    /* advance head index */
                    pBC->sHBuf.u32Head += pBC->u32CmdDataDwords;

                    if (pBC->sHBuf.u32Head >= pBC->sHBuf.u32DwordsSize)
                    {
                        /* roll back to the top of the buffer if it is at the bottom */
                        pBC->sHBuf.u32Head = 0;
                    }
                }

                /* increment msg number in the returned buffer */
                ++u32NumOfMsgs;
            }

            break;
        }

        case BC_HBUF_READ_LATEST_PURGE:
        case BC_HBUF_READ_LATEST_NPURGE:
        {
            /* get the latest msg */
            if (pBC->sHBuf.u32Count)
            {
                /* get the last message*/
                if (pBC->sHBuf.u32Tail == 0)
                {
                    u32MsgIndex = pBC->sHBuf.u32DwordsSize - BC_CMD_DATA_DWORDS;
                }
                else
                {
                    u32MsgIndex = pBC->sHBuf.u32Tail - BC_CMD_DATA_DWORDS;
                }

                bcGetBufMsg(pHBuf, u32MsgIndex, u32DataType, 0, pRtnBuf);

                /* check if set head index to tail index */
                if (u32Type == BC_HBUF_READ_LATEST_PURGE)
                {
                    /* advance head index to tail index */
                    pBC->sHBuf.u32Count = 0;
                    pBC->sHBuf.u32Head = pBC->sHBuf.u32Tail;
                }

                /* increment msg number in the returned buffer */
                ++u32NumOfMsgs;
            }

            break;
        }

        case BC_HBUF_READ_FILL_BUFFER:
        default:
        {
            /*  Fill the user buffer from the next unread msg
                and increment head index, until buffer is full or
                head index = tail index */
            while (pBC->sHBuf.u32Count)
            {
                /* get the next message */
                bcGetBufMsg(pHBuf, pBC->sHBuf.u32Head, u32DataType, 0, pRtnBuf);

                /* increment msg number in the returned buffer */
                ++u32NumOfMsgs;

                /* decrement msg count in the HBuf */
                pBC->sHBuf.u32Count--;

                /* advance head index */
                pBC->sHBuf.u32Head += pBC->u32CmdDataDwords;

                if (pBC->sHBuf.u32Head >= pBC->sHBuf.u32DwordsSize)
                {
                    /* roll back to the top of the buffer if it is at the bottom */
                    pBC->sHBuf.u32Head = 0;
                }

                /* stop if return buffer does not have enough space to hold another msg */
                u32WordCount -= u32CmdDataWords;

                if (u32WordCount < u32CmdDataWords)
                {
                    break;
                }

                pRtnBuf += u32CmdDataWords;
            } /* while loop*/

            break;
        }
    }

    /* release HBuf spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slHBuf), pBC->slHBufFlag);

    /* turn BC OFF if it is in OFF Pending mode */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;

    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_INT_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* return to user */
    *pBytesReturned = (u32NumOfMsgs * u32CmdDataWords) << 1;

    return status;
}

/*******************************************************************************
 * Name:    _bcCalculateHBufMetric
 *
 * Description:
 *      This function calculates BC HBuf Metric information.
 *
 * In   pBC     BC structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void _bcCalculateHBufMetric
(
    struct _ACEX_1553_BC_TYPE *pBC
)
{
    if ((pBC->sHBuf.u32MetricEna) && (pBC->sHBuf.u32NumEntries))
    {
        if (pBC->sHBuf.u32Count == 0)
        {
            pBC->sHBuf.u32PctFull = 0;
        }
        else
        {
            pBC->sHBuf.u32PctFull = pBC->sHBuf.u32Count * 100 / pBC->sHBuf.u32NumEntries;
        }

        if (pBC->sHBuf.u32PctFull > pBC->sHBuf.u32HighPct)
        {
            pBC->sHBuf.u32HighPct = pBC->sHBuf.u32PctFull;
        }
    }
}

/*******************************************************************************
 * Name:    bcCalculateGPQMetric
 *
 * Description:
 *      This function calculates BC GPQ Metric information.
 *
 * In   pBC     BC structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void bcCalculateGPQMetric
(
    struct _ACEX_1553_BC_TYPE *pBC
)
{
    if (pBC->sGPQ.u32NumEntries)
    {
        if (pBC->sGPQ.u32Count == 0)
        {
            pBC->sGPQ.u32PctFull = 0;
        }
        else
        {
            pBC->sGPQ.u32PctFull = pBC->sGPQ.u32Count * 100 / pBC->sGPQ.u32NumEntries;
        }

        if (pBC->sGPQ.u32PctFull > pBC->sGPQ.u32HighPct)
        {
            pBC->sGPQ.u32HighPct = pBC->sGPQ.u32PctFull;
        }
    }
}

/*******************************************************************************
 * Name:    bcGetMetric
 *
 * Description:
 *      This function returns GPQ/HBuf metric information.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     0 ACEX_BC_METRIC_HBUF   - get HBuf info
 *                      1 ACEX_BC_METRIC_GPQ    - get GPQ info
 *          Param2:     size of buffer in bytes
 *          Param3:     High Pct Full reset flag
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetMetric
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    ACEX_BC_HBUF_METRIC *psHBufMetric;
    ACEX_BC_GPQ_METRIC *psGPQMetric;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32BufType = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32Reset = DDC_IOCTL_U32(pIoctlParams->Param3);

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_METRIC,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    if (u32BufType == ACEX_BC_METRIC_HBUF)
    {
        /* return immediately if BC MsgBuf is not installed */
        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF))
        {
            DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_METRIC,
                "CH%d - BC HBuf is not yet installed\n", u16Ch);

            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        psHBufMetric = (ACEX_BC_HBUF_METRIC *)pRdData;

        /* get HBuf spin lock */
        DDC_ISR_LOCK_TAKE((pBC->slHBuf), pBC->slHBufFlag);

        _bcCalculateHBufMetric(pBC);

        psHBufMetric->u32Count = pBC->sHBuf.u32Count;
        psHBufMetric->u32Lost = pBC->sHBuf.u32Lost;
        psHBufMetric->u32PctFull = pBC->sHBuf.u32PctFull;
        psHBufMetric->u32HighPct = pBC->sHBuf.u32HighPct;

        if (u32Reset)
        {
            pBC->sHBuf.u32HighPct = 0;
        }

        /* release HBuf spin lock */
        DDC_ISR_LOCK_GIVE((pBC->slHBuf), pBC->slHBufFlag);

        *pBytesReturned = sizeof(ACEX_BC_HBUF_METRIC);
    }
    else
    {
        /* update GPQ entry and lost counts */
        status = _bcGpqUpdate(pDeviceContext, (U16BIT)pIoctlParams->Channel);

        if (status)
        {
            return status;
        }

        psGPQMetric = (ACEX_BC_GPQ_METRIC *)pRdData;

        /* get GPQ mutex */
        DDC_ISR_LOCK_TAKE((pBC->slGpq), pBC->slGpqFlag);

        bcCalculateGPQMetric(pBC);

        psGPQMetric->u32Lost = pBC->sGPQ.u32Lost;
        psGPQMetric->u16PctFull = (U16BIT)(pBC->sGPQ.u32PctFull & 0xFFFF);
        psGPQMetric->u16HighPct = (U16BIT)(pBC->sGPQ.u32HighPct & 0xFFFF);

        if (u32Reset)
        {
            pBC->sGPQ.u32HighPct = 0;
        }

        /* release GPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slGpq), pBC->slGpqFlag);

        *pBytesReturned = sizeof(ACEX_BC_GPQ_METRIC);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcSetMsgBuffer
 *
 * Description:
 *      This function initializes a msgBuffer block for a given msg index with
 *      the given default message header.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     Msg Idx         0-2047
 *          Param2:     number of DWords in message
 *          Param3:     not used
 *          Param4:     not used
 * In   pDfltMsgHeader  point to a default BC message
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcSetMsgBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pDfltMsgHeader
)
{
    U8BIT *pMsgBuf;
    U8BIT *pMsgIndexed;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32MsgIdx = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32DfltMsgDwords = DDC_IOCTL_U32(pIoctlParams->Param2);

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC MsgBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - BC msgBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return if msgBuf index is out of range */
    if (u32MsgIdx >= pBC->sMsgBuf.u32MaxMsgIdx)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - msgBuf index out of range %d\n", u16Ch, (unsigned int)pBC->sMsgBuf.u32MaxMsgIdx);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pBC->u32ModeFlag |= BC_MODE_FLAG_USER_BUSY;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* limit the default msg header size */
    if (u32DfltMsgDwords > pBC->u32CmdDwords)
    {
        u32DfltMsgDwords = pBC->u32CmdDwords;
    }

    /* get msgBuffer location */
    pMsgBuf = pBC->sMsgBuf.hBuf;

    /* locate the message */
    pMsgIndexed = pMsgBuf + ((u32MsgIdx * pBC->u32CmdDwords) << 2);

    /* get MsgBuf spin lock */
    DDC_ISR_LOCK_TAKE((pBC->slMsgBuf), pBC->slMsgBufFlag);

    /* if Big Endian, swap data pointer words*/
    pDfltMsgHeader[ACE_BC_MSG_DATA_PTR_OFFSET] = DDC_WORD_ORDER_L(pDfltMsgHeader[ACE_BC_MSG_DATA_PTR_OFFSET]);

    /* clear the message */
    memset((void *)pMsgIndexed, 0, (pBC->u32CmdDwords << 2));
    memcpy((void *)pMsgIndexed, (void *)pDfltMsgHeader, (u32DfltMsgDwords << 2));

    /* release MsgBuf spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slMsgBuf), pBC->slMsgBufFlag);

    /* turn BC OFF if it is in OFF Pending mode */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_INT_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcSeDataBuffer
 *
 * Description:
 *      This function initializes a dataBuffer block for a given data index with
 *      the given default data information.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     data Idx        0-2047
 *          Param2:     number of words in data block
 *          Param3:     not used
 *          Param4:     not used
 * In   pDfltMsgHeader  point to a BC msg header
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcSeDataBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pDfltData
)
{
    U8BIT *pDataBuf;
    U8BIT *pDataIndexed;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32DataIdx = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32DfltDataWords = DDC_IOCTL_U32(pIoctlParams->Param2);

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC DataBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - BC dataBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return if dataBuf index is out of range */
    if (u32DataIdx >= pBC->sDataBuf.u32MaxDataIdx)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - dataBuf index is out of range %d\n", u16Ch, (unsigned int)pBC->sDataBuf.u32MaxDataIdx);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pBC->u32ModeFlag |= BC_MODE_FLAG_USER_BUSY;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* limit the default msg data size */
    if (u32DfltDataWords > (pBC->u32DataDwords << 1))
    {
        u32DfltDataWords = pBC->u32DataDwords << 1;
    }

    /* get dataBuffer location */
    pDataBuf = pBC->sDataBuf.hBuf;

    /* locate the data block */
    pDataIndexed = pDataBuf + ((u32DataIdx * pBC->u32DataDwords) << 2);

    /* get DataBuf spin lock */
    DDC_ISR_LOCK_TAKE((pBC->slDataBuf), pBC->slDataBufFlag);

    /* clear the data block and set with new data */
    memset((void *)pDataIndexed, 0, (pBC->u32DataDwords << 2));
    memcpy((void *)pDataIndexed, (void *)pDfltData, (u32DfltDataWords << 1));

    /* release DataBuf spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slDataBuf), pBC->slDataBufFlag);

    /* turn BC OFF if it is in OFF Pending mode */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_INT_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcGetMsg
 *
 * Description:
 *      This function returns the msg header and data from msg and data buffer
 *      with the given msg index.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     msg index
 *          Param2:     msg type
 *                          0 ACEX_BC_MESSAGE_FULL - full message, 42 words
 *                          1 ACEX_BC_MESSAGE_HDR  - msg header only, 10 words
 *          Param3:     set to clear BSW
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdMsg          point to msg buffer
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdMsg
)
{
    U16BIT *pRtnBuf;
    U32BIT u32WordCount;
    U16BIT u16DataIdx = 0;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32MsgIdx = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32Type = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_TYPE);
    U32BIT u32Blocking = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_BLOCKING);
    U32BIT u32CheckingImp = DDC_IOCTL_U32(pIoctlParams->Param2 & ACEX_GETMSG_CHECK_IMP);
    U32BIT u32Purge = DDC_IOCTL_U32(pIoctlParams->Param3);

    *pBytesReturned = 0;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC MsgBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - BC msgBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - BC is OFF Pending\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pBC->u32ModeFlag |= BC_MODE_FLAG_USER_BUSY;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* return if msgBuf index is out of range */
    if (u32MsgIdx >= pBC->sMsgBuf.u32MaxMsgIdx)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - msgBuf index out of range %d\n", u16Ch, (unsigned int)pBC->sMsgBuf.u32MaxMsgIdx);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* set word count */
    if (u32Type == ACEX_BC_MESSAGE_FULL)
    {
        /* full msg */
        u32WordCount = BC_MSG_CMD_DATA_WORDS;
    }
    else
    {
        /* header only */
        u32WordCount = BC_MSG_CMDSTS_WORDS;
    }

    /* get msg header */
    pBC->u32MsgEventMsgIdx = u32MsgIdx;
    pRtnBuf = (U16BIT *)pRdMsg;

    bcGetMsgHeader(pDeviceContext, u16Ch, u32MsgIdx, u32Blocking, u32CheckingImp, u32Purge, pRtnBuf, &u16DataIdx);

    /* if the event coming from the terminating, return error */
    if (pBC->u32MsgEventMsgIdx != u32MsgIdx)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_MSG,
            "CH%d - BC msgBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* get msg data if needed */
    if (u32Type == ACEX_BC_MESSAGE_FULL)
    {
        /* get data from data buffer */
        pRtnBuf += BC_MSG_CMDSTS_WORDS;
        bcGetMsgData(pBC, (U32BIT)u16DataIdx, 0, BC_MSG_DATA_WORDS, pRtnBuf);
    }

    /* turn BC OFF if it is in OFF Pending mode */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;

    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_INT_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    *pBytesReturned = u32WordCount << 1;
    return status;
}

/*******************************************************************************
 * Name:    bcGetData
 *
 * Description:
 *      This function returns msg data with given data index.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     data Idx
 *          Param2:     word count
 *          Param3:     words offset to get data
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to data buffer
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32DataIdx = DDC_IOCTL_U32(pIoctlParams->Param1 % ACEX_BC_MAX_INDEX_MSG);
    U32BIT u32WordCount = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32WordOffset = DDC_IOCTL_U32(pIoctlParams->Param3);

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC MsgBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_MSGBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - BC datagBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - BC is OFF Pending\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pBC->u32ModeFlag |= BC_MODE_FLAG_USER_BUSY;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* return if msgBuf index is out of range */
    if (u32DataIdx >= pBC->sDataBuf.u32MaxDataIdx)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA,
            "CH%d - dataBuf index out of range %d\n", u16Ch, (unsigned int)pBC->sDataBuf.u32MaxDataIdx);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    bcGetMsgData(pBC, u32DataIdx, u32WordOffset, u32WordCount, (U16BIT *)pRdData);

    /* turn BC OFF if it is in OFF Pending mode */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_USER_BUSY;

    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_INT_BUSY))
    {
        if (pBC->u32ModeFlag & BC_MODE_FLAG_OFF_PENDING)
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
        }
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    *pBytesReturned = u32WordCount << 1;

    return status;
}

/*******************************************************************************
 * Name:    bcCmdStackRead
 *
 * Description:
 *      This function returns msg stack information with given address.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     cmd stack dword (32-bit) address
 *          Param2:     words count
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to data buffer
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcCmdStackRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U16BIT   *pBuffer;
    U32BIT i;
    U32BIT u32CmdStsBuf[BC_CMD_STS_DWORDS];

    U32BIT u32Address = DDC_IOCTL_U32(pIoctlParams->Param1);             /* dword address*/
    U32BIT u32Length = DDC_IOCTL_U32(pIoctlParams->Param2);              /* number of 16 bit words .....architecture independent */

    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* read cmd stack .......address is always guaranteed to be 32 bit aligned. no need to check for partial 32 bit reads*/
    status = DDC_BLK_MEM_READ( pDeviceContext, u32Address, u32CmdStsBuf, BC_CMD_STS_DWORDS, ACEX_32_BIT_ACCESS_16_BIT_HW_MEM);
    if (status)
    {
        /* get out if hardware has problem */
        *pBytesReturned = 0;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CMD_STACK,
            "DDC_BLK_MEM_READ @ Addr = %d\n", (int)u32Address);

        return status;
    }

    /* return cmd status to user */
    pBuffer = (U16BIT *)pRdData;
    for (i = 0; i < u32Length; i++)
    {
        pBuffer[i] = (U16BIT)(u32CmdStsBuf[i] & 0xFFFF); /* lower 16 bits */
    }

    *pBytesReturned = u32Length << 1;
    return status;
}

/*******************************************************************************
 * Name:    bcDataStackRead
 *
 * Description:
 *      This function returns msg data stack with given address.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     data block dword (32-bit) address
 *          Param2:     words length
 *          Param3:     words offset
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to data buffer
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcDataStackRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U32BIT u32Address32 = DDC_IOCTL_U32(pIoctlParams->Param1);
    U32BIT u32Length = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32Offset = DDC_IOCTL_U32(pIoctlParams->Param3);
    U32BIT u32Address16 = (u32Address32 << DDC_1553_ADDR_SHIFT) + u32Offset;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* read all data in */
    status = DDC_16BIT_BLK_MEM_READ( pDeviceContext, u32Address16, pRdData, u32Length);
    if (status)
    {
        /* get out if hardware has problem */
        *pBytesReturned = 0;

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_DATA_STACK,
            "DDC_BLK_MEM_READ @ Addr = %d\n", (int)u32Address16);

        return status;
    }

    *pBytesReturned = u32Length << 1;
    return status;
}

/*******************************************************************************
 * Name:    bcGetFrameToHBuf
 *
 * Description:
 *      This function triggers an event to stop IMP module. When IMP completes its
 *      current job, it interrupts driver. When IMP interrupt, driver starts to read all
 *      messages to HBuf.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetFrameToHBuf
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC HBuf is not installed */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HBUF))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d - BC HBuf is not yet installed\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* stop IMP to trigger data fetch event */
    status = _bcImpStop(pDeviceContext, u16Ch);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_HBUF,
            "CH%d Stop IMP\n", u16Ch);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcGpqClear
 *
 * Description:
 *      This function clears GPQ information.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGpqClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* clear GPQ pointer in device */
    status = DDC_REG_WRITE(pDeviceContext, ((*(pBC->pu32RegBA)) + REG_BC_GPQ), &(pBC->sGPQ.u32BaseAddr));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
        "WRITE REG:%08x VALUE:%08x\n",
        (unsigned int)((*(pBC->pu32RegBA)) + REG_BC_GPQ),
        (unsigned int)pBC->sGPQ.u32BaseAddr);

    /* get GPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slGpq), pBC->slGpqFlag);

    /* clear GPQ informaiton */
    pBC->sGPQ.u32Head = 0;
    pBC->sGPQ.u32Tail = 0;

    pBC->sGPQ.u32Count = 0;
    pBC->sGPQ.u32Lost = 0;
    pBC->sGPQ.u32PctFull = 0;
    pBC->sGPQ.u32HighPct = 0;

    /* release GPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slGpq), pBC->slGpqFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcGpqCalculateCount
 *
 * Description:
 *      This function calculates GPQ count using the existing tail and head
 *      information in the GPQ structure.
 *
 * In   pBC     pointer to BC structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void bcGpqCalculateCount
(
    struct _ACEX_1553_BC_TYPE *pBC
)
{
    if (pBC->sGPQ.u32Tail >= pBC->sGPQ.u32Head)
    {
        /* normal */
        pBC->sGPQ.u32Count = (pBC->sGPQ.u32Tail - pBC->sGPQ.u32Head) >> 1;
    }
    else
    {
        /* rollover */
        pBC->sGPQ.u32Count = (pBC->sGPQ.u32DwordsSize + pBC->sGPQ.u32Tail - pBC->sGPQ.u32Head) >> 1;
    }
}

/*******************************************************************************
 * Name:    bcGpqGetCount
 *
 * Description:
 *      This function gets GPQ count.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         GPQ count
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGpqGetCount
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U32BIT u32Tail;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if GPQ is not initialized*/
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_GPQ))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d - GPQ is not initialized\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* read GPQ tail location */
    status = DDC_REG_READ(pDeviceContext, ((*(pBC->pu32RegBA)) + REG_BC_GPQ), &u32Tail);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d: Read GPQ tail address\n", u16Ch);

        return status;
    }

    /* modify tail to point to even word */
    if ((u32Tail % 2) != 0)
    {
        --u32Tail;
    }

    /* get GPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slGpq), pBC->slGpqFlag);

    pBC->sGPQ.u32Tail = u32Tail & BC_GPQ_ADDR_MASK;

    /* calculate count */
    bcGpqCalculateCount(pBC);

    *pRdData = (U16BIT)(pBC->sGPQ.u32Count & 0xFFFF);

    /* release GPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slGpq), pBC->slGpqFlag);

    *pBytesReturned = sizeof(U16BIT);

    return status;
}

/*******************************************************************************
 * Name:    bcGpqRead
 *
 * Description:
 *      This function updates GPQ entry and lost counts.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           channel number  0-31
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT _bcGpqUpdate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U16BIT u16Value;
    U32BIT u32PrevHead;
    U32BIT u32GpqHeader;
    size_t u32BytesReturned;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* fill count and tail location to GPQ structure */
    status = bcGpqGetCount(pDeviceContext, u16Ch, &u32BytesReturned, &u16Value);

    if (status)
    {
        return status;
    }

    /* get information of previous entry */
    if (pBC->sGPQ.u32Head == 0)
    {
        /* last entry in the GPQ stack */
        u32PrevHead = pBC->sGPQ.u32DwordsSize - BC_GPQ_ENTRY_DWORDS;
    }
    else
    {
        /* previously read entry */
        u32PrevHead = pBC->sGPQ.u32Head - BC_GPQ_ENTRY_DWORDS;
    }

    /* read GPQ header from the last entry */
    status = DDC_MEM_READ(pDeviceContext, (pBC->sGPQ.u32BaseAddr + u32PrevHead), &u32GpqHeader, ACEX_32_BIT_ACCESS);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d: Read GPQ header from the last entry\n", u16Ch);

        return status;
    }

    /* if header entry is not zero then we have an overrun condition */
    if (u32GpqHeader != 0)
    {
        /* get GPQ mutex */
        DDC_ISR_LOCK_TAKE((pBC->slGpq), pBC->slGpqFlag);

        pBC->sGPQ.u32Lost++;

        if (pBC->sGPQ.u32Tail < pBC->sGPQ.u32Head)
        {
            /* rollover */
            pBC->sGPQ.u32Lost += (pBC->sGPQ.u32DwordsSize + pBC->sGPQ.u32Tail - pBC->sGPQ.u32Head) >> 1;
        }
        else
        {
            /* normal */
            pBC->sGPQ.u32Lost += (pBC->sGPQ.u32Tail - pBC->sGPQ.u32Head) >> 1;
        }

        /* set to GPQ location to tail */
        pBC->sGPQ.u32Head = pBC->sGPQ.u32Tail + BC_GPQ_ENTRY_DWORDS;

        if (pBC->sGPQ.u32Head >= pBC->sGPQ.u32DwordsSize)
        {
            pBC->sGPQ.u32Head = 0;
        }

        /* recalculate count */
        bcGpqCalculateCount(pBC);

        /* set the HighPct to 100 since overrun happened */
        pBC->sGPQ.u32HighPct = 100;

        /* release GPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slGpq), pBC->slGpqFlag);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcGpqRead
 *
 * Description:
 *      This function reads GPQ data.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGpqRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
)
{
    U32BIT u32Addr;
    U32BIT u32Data[2];

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    ACEX_BC_GPQ_ENTRY *pGPQEntry = (ACEX_BC_GPQ_ENTRY *)pRdData;

    *pBytesReturned = 0;

    /* exit if there is no GPQ */
    if (pBC->sGPQ.u32Count == 0)
    {
        pGPQEntry->u32GPQHeader = 0;
        pGPQEntry->u32GPQData = 0;
        return status;
    }

    /* read GPQ entry */
    u32Addr = pBC->sGPQ.u32BaseAddr + pBC->sGPQ.u32Head;
    status = DDC_BLK_MEM_READ(pDeviceContext, u32Addr, u32Data, 2, ACEX_32_BIT_ACCESS);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d: Read GPQ header and data\n", u16Ch);

        pGPQEntry->u32GPQHeader = 0;
        pGPQEntry->u32GPQData = 0;
        return status;
    }

    pGPQEntry->u32GPQHeader = u32Data[0];
    pGPQEntry->u32GPQData = u32Data[1];

    /* purge the entry */
    u32Data[0] = 0;
    u32Data[1] = 0;
    status = DDC_BLK_MEM_WRITE(pDeviceContext, u32Addr, u32Data, 2, ACEX_32_BIT_ACCESS);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d: Clear GPQ header and data\n", u16Ch);

        return status;
    }

    /* get GPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slGpq), pBC->slGpqFlag);

    /* increment head to next entry */
    pBC->sGPQ.u32Head += BC_GPQ_ENTRY_DWORDS;

    if (pBC->sGPQ.u32Head >= pBC->sGPQ.u32DwordsSize)
    {
        pBC->sGPQ.u32Head = 0;
    }

    /* release GPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slGpq), pBC->slGpqFlag);

    *pBytesReturned = sizeof(ACEX_BC_GPQ_ENTRY);
    return status;
}

/*******************************************************************************
 * Name:    bcPostLpAsyncMsg
 *
 * Description:
 *      This function posts a low priority async message.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     address and data block pointer operator
 *                      of a user supplied message block
 *          Param3:     message time provided in 100us increments
 *                      0 - 7 (100us - 800us)
 *          Param4:     bMsgIrq | HostID | DataArrayID
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to ACEX_BC_ASYNC_STATUS structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcPostLpAsyncMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    void *pRdData
)
{
    U32BIT u32Count;
    U32BIT u32Addr;
    U32BIT u32Value;

    U32BIT u32MsgAddrAndDbpo;
    U32BIT u32MsgTime;
    U16BIT u16DataArrayID;
    U8BIT u8HostID;
    BOOLEAN bMsgIrq;

    ACEX_BC_DATA_ARRAY *pDataArray;
    ACEX_BC_ASYNC_STATUS *pAsyncStatus;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    *pBytesReturned = sizeof(ACEX_BC_ASYNC_STATUS);
    pAsyncStatus = (ACEX_BC_ASYNC_STATUS *)pRdData;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - not in BC mode\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_INVALID_MODE;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC LP is not initialized */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_LPQ))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - BC LPQ is not yet initialized\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_INVALID_MODE;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* low priority msg address and time (in 100us)*/
    u32MsgAddrAndDbpo = DDC_IOCTL_U32(pIoctlParams->Param2 & (BC_LPQ_MSG_ADDR_MASK | BC_LPQ_MSG_DBPO_MASK));
    u32MsgTime = DDC_IOCTL_U32(pIoctlParams->Param3 & BC_LPQ_MSG_TIME_MASK) << BC_LPQ_MSG_TIME_SHIFT;
    bMsgIrq = (BOOLEAN)((pIoctlParams->Param4 >> 31) & 0x1);
    u8HostID = (U8BIT)((pIoctlParams->Param4 >> 16) & 0x00FF);            /* 8-bit HostID */
    u16DataArrayID = (U16BIT)((pIoctlParams->Param4 & 0xFFFF) % NUM_DATARRAY_CHANNELS);

    pDataArray = &pBC->sDataArrayChan[u16DataArrayID].sSnd;

    /* do something for data array function */
    if (u8HostID == BC_HOST_ID_ARRAY)
    {
        /* get dataArray mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

        /* return error if there is no room to post a data array message */
        if (pDataArray->u16MsgToPostAvail == 0)
        {
            /* release dataArray spin lock*/
            DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);

            /* trigger the IMP to fetch data into driver */
            _bcImpStop(pDeviceContext, u16Ch);

            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_DATARRAY_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        /* release dataArray spin lock*/
        DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);
    }

    /* get LPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slLpq), pBC->slLpqFlag);

    /* check if the queue is full  */
    if (pBC->sLPQ.u32Count >= pBC->sLPQ.u32Entries)
    {
        /* read LP async status register */
        u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_STS;
        status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Count);

        if (status)
        {
            /* release LPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - read LPQ count\n", u16Ch);

            /* return queue full since we are not able to get the count from device */
            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_QUEUE_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        pBC->sLPQ.u32Count = u32Count & BC_LPQ_MSG_COUNT_MASK;

        /* return if queue is full */
        if (pBC->sLPQ.u32Count >= pBC->sLPQ.u32Entries)
        {
            /* release LPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_QUEUE_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }
    }

    /* post the msg to BC */
    u32Addr = pBC->sLPQ.u32BaseAddr + pBC->sLPQ.u32Tail;
    u32Value = BC_ASYNC_DATA_FIXED | u32MsgTime | u32MsgAddrAndDbpo; /* both ctrl and data; frame time gap */

    if (bMsgIrq)
    {
        u32Value |= BC_ASYNC_QUEUE_HALF_FULL; /* set INT to check Msg completion */
    }

    status = DDC_MEM_WRITE(pDeviceContext, u32Addr, &u32Value, ACEX_32_BIT_ACCESS);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE MEM:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        /* release LPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - post LPQ msg\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_USB_FAIL;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* modify message count */
    switch (u8HostID)
    {
        case BC_HOST_ID_ARRAY:
        {
            /* release LPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

            /* get dataArray mutex*/
            DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

            /* decrement open message count */
            pDataArray->u16MsgToPostAvail--;

            if (pDataArray->u16MsgToPostAvail <= pDataArray->u16MsgToPostEmptyMark)
            {
                /* release dataArray mutex*/
                DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);

                /* trigger the IMP to fetch data into driver */
                _bcImpStop(pDeviceContext, u16Ch);
            }
            else
            {
                /* release dataArray mutex*/
                DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);
            }

            /* get LPQ mutex */
            DDC_ISR_LOCK_TAKE((pBC->slLpq), pBC->slLpqFlag);

            break;
        }

        case BC_HOST_ID_LP:
        {
            /* Increment Async message count */
            pBC->u16LpMsgCount++;
            break;
        }

        default:
        {
            /* Doing nothing */
            break;
        }
    }

    /* increment tail address*/
    pBC->sLPQ.u32Tail++;

    if (pBC->sLPQ.u32Tail >= pBC->sLPQ.u32Entries)
    {
        pBC->sLPQ.u32Tail = 0;
    }

    /* Increment message count in the queue */
    pBC->sLPQ.u32Count++;

    /* set the Number-of-Messages-Posted information */
    u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_NOMP;
    u32Value = 1 << BC_LPQ_NOMP_SHIFT;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);
    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", u32Addr, u32Value);

    if (status)
    {
        /* release LPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - set NOMP info\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_USB_FAIL;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_SUCCESS;
    pAsyncStatus->u16Count = pBC->u16LpMsgCount;

    /* release LPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcGetLpAsyncMsgCount
 *
 * Description:
 *      This function returns the message count in the LP async message queue.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     number of dwords in message
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetLpAsyncMsgCount
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U32BIT u32Count;
    U32BIT u32Addr;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    *pRdData = 0;
    *pBytesReturned = sizeof(U32BIT);

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC LP is not initialized */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_LPQ))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - BC LPQ is not yet initialized\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* get LPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slLpq), pBC->slLpqFlag);

    /* read back LP async status register */
    u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_STS;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Count);

    if (status)
    {
        /* release LPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - read LPQ count\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* release LPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);

    *pRdData = u32Count & BC_LPQ_MSG_COUNT_MASK;

    return status;
}

/*******************************************************************************
 * Name:    bcGetAsyncQueueInfo
 *
 * Description:
 *      This function returns the asynchronous message queue information.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     queue type, ACEX_BC_QUEUE_LP/ACEX_BC_QUEUE_HP
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGetAsyncQueueInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U32BIT u32Count;
    U32BIT u32Addr;
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    U32BIT u32QueueType = DDC_IOCTL_U32(pIoctlParams->Param1);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - not in BC mode\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    psQueueInfo = (ACEX_BC_ASYNC_QUEUE_INFO *)pRdData;

    if (u32QueueType == ACEX_BC_QUEUE_LP)
    {
        /* return immediately if BC LP is not initialized */
        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        if (!(pBC->u32ModeFlag & BC_MODE_FLAG_LPQ))
        {
            DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - BC LPQ is not yet initialized\n", u16Ch);

            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* read back LP async status register */
        u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_STS;
        status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Count);

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - read LPQ count\n", u16Ch);

            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        /* set return value*/
        psQueueInfo->u16HwQueueCount = (U16BIT)(u32Count & 0xFFFF);

        /* get LPQ mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slLpq), pBC->slLpqFlag);

        psQueueInfo->u16QueuedMsgCount = pBC->u16LpMsgCount;

        /* release LPQ mutex*/
        DDC_ISR_LOCK_GIVE((pBC->slLpq), pBC->slLpqFlag);
    }
    else
    {
        /* return immediately if BC HP is not initialized */
        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HPQ))
        {
            DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - BC HPQ is not yet initialized\n", u16Ch);

            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* read back HP async status register */
        u32Addr = *pBC->pu32RegBA + REG_BC_HP_ASYNC_STS;
        status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Count);

        if (status)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - read HPQ count\n", u16Ch);

            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        /* set return value*/
        psQueueInfo->u16HwQueueCount = (U16BIT)(u32Count & 0xFFFF);

        /* get HPQ mutex*/
        DDC_ISR_LOCK_TAKE((pBC->slHpq), pBC->slHpqFlag);

        psQueueInfo->u16QueuedMsgCount = pBC->u16HpMsgCount;

        /* release HPQ mutex*/
        DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);
    }

    *pBytesReturned = sizeof(ACEX_BC_ASYNC_QUEUE_INFO);
    return status;
}

/*******************************************************************************
 * Name:    bcPostHpAsyncMsg
 *
 * Description:
 *      This function posts a high priority async message.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     not used
 *          Param2:     address and data block pointer operator
 *                      of a user supplied message block
 *          Param3:     not used
 *          Param4:     bMsgIrq | HostID | DataArrayID
 * In   pBytesReturned  bytes returned
 * Out  pRdData         point to ACEX_BC_ASYNC_STATUS structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
static S16BIT bcPostHpAsyncMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    S16BIT *pRdData
)
{
    U32BIT u32Count;
    U32BIT u32Addr;
    U32BIT u32Value;
    U32BIT u32MsgAddrAndDbpo;
    U16BIT u16DataArrayID;
    U8BIT u8HostID;
    BOOLEAN bMsgIrq;

    ACEX_BC_DATA_ARRAY *pDataArray;
    ACEX_BC_ASYNC_STATUS *pAsyncStatus;

    U16BIT u16Ch = (U16BIT)(pIoctlParams->Channel & 0xFFFF);
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    *pBytesReturned = sizeof(ACEX_BC_ASYNC_STATUS);
    pAsyncStatus = (ACEX_BC_ASYNC_STATUS *)pRdData;

    /* return immediately if BC mode is not turned on */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_ON))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - not in BC mode\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_INVALID_MODE;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* return immediately if BC HP is not initialized */
    if (!(pBC->u32ModeFlag & BC_MODE_FLAG_HPQ))
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - BC HPQ is not yet initialized\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_INVALID_MODE;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* high priority msg address and data block pointer operator */
    u32MsgAddrAndDbpo = DDC_IOCTL_U32(pIoctlParams->Param2 & (BC_HPQ_MSG_ADDR_MASK | BC_HPQ_MSG_DBPO_MASK));
    bMsgIrq = (BOOLEAN)((pIoctlParams->Param4 >> 31) & 0x1);
    u8HostID = (U8BIT)((pIoctlParams->Param4 >> 16) & 0x00FF);            /* 8-bit HostID */
    u16DataArrayID = (U16BIT)((pIoctlParams->Param4 & 0xFFFF) % NUM_DATARRAY_CHANNELS);

    pDataArray = &pBC->sDataArrayChan[u16DataArrayID].sSnd;

    /* do something for data array function */
    if (u8HostID == BC_HOST_ID_ARRAY)
    {
        /* get dataArray spin lock*/
        DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

        /* return error if there is no room to post a data array message */
        if (pDataArray->u16MsgToPostAvail == 0)
        {
            /* release dataArray spin lock*/
            DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);

            /* trigger the IMP to fetch data into driver */
            _bcImpStop(pDeviceContext, u16Ch);

            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_DATARRAY_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        /* release dataArray spin lock*/
        DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);
    }

    /* get HPQ mutex */
    DDC_ISR_LOCK_TAKE((pBC->slHpq), pBC->slHpqFlag);

    /* check if the queue is full  */
    if (pBC->sHPQ.u32Count >= pBC->sHPQ.u32Entries)
    {
        /* read HP async status register */
        u32Addr = *pBC->pu32RegBA + REG_BC_HP_ASYNC_STS;
        status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Count);

        if (status)
        {
            /* release HPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

            DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
                "CH%d - read HPQ count\n", u16Ch);

            /* return queue full since we are not able to get the count from device */
            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_QUEUE_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }

        pBC->sHPQ.u32Count = u32Count & BC_HPQ_MSG_COUNT_MASK;

        /* return if queue is full */
        if (pBC->sHPQ.u32Count >= pBC->sHPQ.u32Entries)
        {
            /* release HPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

            pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_QUEUE_FULL;
            pAsyncStatus->u16Count = 0;
            return DDC_UDL_ERROR__BC_STATUS_MODE;
        }
    }

    /* post the msg to BC */
    u32Addr = pBC->sHPQ.u32BaseAddr + pBC->sHPQ.u32Tail;
    u32Value = BC_ASYNC_DATA_FIXED | u32MsgAddrAndDbpo; /* both ctrl and data */

    if (bMsgIrq)
    {
        u32Value |= BC_ASYNC_QUEUE_HALF_FULL; /* set INT to check Msg completion */
    }

    status = DDC_MEM_WRITE(pDeviceContext, u32Addr, &u32Value, ACEX_32_BIT_ACCESS);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);


    if (status)
    {
        /* release HPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - post HPQ msg\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_USB_FAIL;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* modify message count */
    switch (u8HostID)
    {
        case BC_HOST_ID_ARRAY:
        {
            /* release HPQ mutex */
            DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

            /* get dataArray spin lock*/
            DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

            /* decrement open message count */
            pDataArray->u16MsgToPostAvail--;

            if (pDataArray->u16MsgToPostAvail <= pDataArray->u16MsgToPostEmptyMark)
            {
                /* release dataArray spin lock*/
                DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);

                /* trigger the IMP to fetch data into driver */
                _bcImpStop(pDeviceContext, u16Ch);
            }
            else
            {
                /* release dataArray spin lock*/
                DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);
            }

            /* get HPQ mutex */
            DDC_ISR_LOCK_TAKE((pBC->slHpq), pBC->slHpqFlag);

            break;
        }

        case BC_HOST_ID_HP:
        {
            /* Increment HP message count */
            pBC->u16HpMsgCount++;
            break;
        }

        default:
        {
            /* Doing nothing */
            break;
        }
    }

    /* increment tail address*/
    pBC->sHPQ.u32Tail++;

    if (pBC->sHPQ.u32Tail >= pBC->sHPQ.u32Entries)
    {
        pBC->sHPQ.u32Tail = 0;
    }

    /* Increment message count in the queue */
    pBC->sHPQ.u32Count++;

    /* set the Number-of-Messages-Posted information */
    u32Addr = *pBC->pu32RegBA + REG_BC_HP_ASYNC_NOMP;
    u32Value = 1 << BC_HPQ_NOMP_SHIFT;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG: %08x VALUE:%08x\n", u32Addr, u32Value);

    if (status)
    {
        /* release HPQ mutex */
        DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - set NOMP info\n", u16Ch);

        pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_USB_FAIL;
        pAsyncStatus->u16Count = 0;
        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    pAsyncStatus->s16Status = ACEX_BC_ASYNC_STS_SUCCESS;
    pAsyncStatus->u16Count = pBC->u16HpMsgCount;

    /* release HPQ mutex */
    DDC_ISR_LOCK_GIVE((pBC->slHpq), pBC->slHpqFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcPostAsyncMsg
 *
 * Description:
 *      This function posts an async message.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     message type
 *                         ACEX_BC_QUEUE_LP 0
 *                         ACEX_BC_QUEUE_HP 1
 *          Param2:     address of a user supplied message block
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to ACEX_BC_ASYNC_STATUS structure
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcPostAsyncMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    void *pRdData
)
{
    U32BIT u32QueueType;

    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* post asynchronuous message*/
    u32QueueType = DDC_IOCTL_U32(pIoctlParams->Param1);

    if (u32QueueType == ACEX_BC_QUEUE_LP)
    {
        /* low priority */
        status = bcPostLpAsyncMsg(pDeviceContext, pIoctlParams, pBytesReturned, pRdData);
    }
    else
    {
        /* high priority */
        status = bcPostHpAsyncMsg(pDeviceContext, pIoctlParams, pBytesReturned, pRdData);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcGpqInit
 *
 * Description:
 *      This function initializes BC GPQ memory area.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 *          Channel:    channel number  0-31
 *          Param1:     general purpose queue base address
 *          Param2:     general purpose queue entry number
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Addr;
    U32BIT u32Value;
    U32BIT u32Data[2];

    ACEX_BC_QUEUE_INFO *psBcQueueInfo = (ACEX_BC_QUEUE_INFO *) pIoctlParams;

    U16BIT u16Ch = psBcQueueInfo->sConfigID.u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* clear all fields */
    memset(&pBC->sGPQ, 0, sizeof(ACEX_BC_GPQ));

    /* initialize GPQ information */
    pBC->sGPQ.u32BaseAddr = psBcQueueInfo->u32BaseAddr;
    pBC->sGPQ.u32DwordsSize = psBcQueueInfo->u32QueueSize;
    pBC->sGPQ.u32NumEntries = pBC->sGPQ.u32DwordsSize >> 1;

    /* the base address will be 19 bits in GPQ register 0x08 */
    u32Addr = *pBC->pu32RegBA + REG_BC_GPQ;
    u32Value = pBC->sGPQ.u32BaseAddr;

    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
        "WRITE REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "CH%d - set base address, status = 0x%08x\n", u16Ch, status);

        return status;
    }

    /* clear the last GPQ entry to detect overrun condition */
    u32Data[0] = 0;
    u32Data[1] = 0;
    u32Addr = pBC->sGPQ.u32BaseAddr + ((pBC->sGPQ.u32NumEntries - 1) << 1);
    status = DDC_BLK_MEM_WRITE(pDeviceContext, u32Addr, u32Data, 2, ACEX_32_BIT_ACCESS);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_GPQ,
            "Memory clear @ addr = %d, status = 0x%08x\n", (unsigned int)u32Addr, status);

        return status;
    }

    /* set mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_GPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcGpqFree
 *
 * Description:
 *      This function frees BC GPQ.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcGpqFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* clear mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_GPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcLpqInit
 *
 * Description:
 *      This function initializes BC Low Priority Asynchronous message Queue
 *      memory area.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 *          Channel:    channel number  0-31
 *          Param1:     LPQ base address
 *          Param2:     LPQ entry number
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
S16BIT bcLpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Addr;
    U32BIT u32Value;
    U32BIT u32QueueBits;

    ACEX_BC_QUEUE_INFO *psBcQueueInfo = (ACEX_BC_QUEUE_INFO *) pIoctlParams;

    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pIoctlParams)->u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* initialize queued msg count to zero */
    pBC->u16LpMsgCount = 0;

    /* initialize LPQ information */
    pBC->sLPQ.u32BaseAddr = psBcQueueInfo->u32BaseAddr;
    pBC->sLPQ.u32Entries = psBcQueueInfo->u32QueueSize;
    pBC->sLPQ.u32DwordsSize = pBC->sLPQ.u32Entries * BC_LPQ_ENTRY_DWORDS;
    pBC->sLPQ.u32Tail = 0;
    pBC->sLPQ.u32Count = 0;

    /* enable LPQ */
    u32Addr = *pBC->pu32RegBA + REG_BC_CONFIG;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "READ REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Read config reg\n", u16Ch);

        return status;
    }

    u32Value |= BC_CONFIG_LPQ_ENABLE;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE MEM:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address\n", u16Ch);

        return status;
    }

    /* set base address and queue size to device */
    switch (pBC->sLPQ.u32Entries)
    {
        default:
        case ACEX_MEMSZ_BC_LPQ_512:
        {
            u32QueueBits = BC_QSIZE_BITS_512;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_256:
        {
            u32QueueBits = BC_QSIZE_BITS_256;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_128:
        {
            u32QueueBits = BC_QSIZE_BITS_128;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_64:
        {
            u32QueueBits = BC_QSIZE_BITS_64;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_32:
        {
            u32QueueBits = BC_QSIZE_BITS_32;
            break;
        }
    }

    u32Addr = *pBC->pu32RegBA + REG_BC_LP_ASYNC_CTRL;
    u32Value = BC_LPQ_RESET | (pBC->sLPQ.u32BaseAddr << BC_LPQ_BA_SHIFT) | u32QueueBits;

    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address\n", u16Ch);

        return status;
    }

    /* set mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_LPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcLpqFree
 *
 * Description:
 *      This function frees LP Queue.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcLpqFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Addr;
    U32BIT u32Value;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* disable LPQ */
    u32Addr = *pBC->pu32RegBA + REG_BC_CONFIG;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "READ REG:%08x VALUE:%08x\n", (int)u32Addr, (int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Read config reg fail\n", u16Ch);

        return status;
    }

    u32Value &= ~BC_CONFIG_LPQ_ENABLE;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (int)u32Addr, (int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address fail\n", u16Ch);

        return status;
    }

    /* clear mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_LPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcHpqInit
 *
 * Description:
 *      This function initializes BC High Priority Asynchronous message Queue
 *      memory area.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 *          Channel:    channel number  0-31
 *          Param1:     HPQ base address
 *          Param2:     HPQ entry number
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcHpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Addr;
    U32BIT u32Value;
    U32BIT u32QueueBits;

    ACEX_BC_QUEUE_INFO *psBcQueueInfo = (ACEX_BC_QUEUE_INFO *) pIoctlParams;

    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pIoctlParams)->u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* initialize queued msg count to zero */
    pBC->u16HpMsgCount = 0;

    /* initialize HPQ information */
    pBC->sHPQ.u32BaseAddr = psBcQueueInfo->u32BaseAddr;
    pBC->sHPQ.u32Entries = psBcQueueInfo->u32QueueSize;
    pBC->sHPQ.u32DwordsSize = psBcQueueInfo->u32QueueSize * BC_HPQ_ENTRY_DWORDS;
    pBC->sHPQ.u32Tail = 0;
    pBC->sHPQ.u32Count = 0;

    /* enable HPQ */
    u32Addr = *pBC->pu32RegBA + REG_BC_CONFIG;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "READ REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Read config reg\n", u16Ch);

        return status;
    }

    u32Value |= BC_CONFIG_HPQ_ENABLE;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address\n", u16Ch);

        return status;
    }

    /* set base address and queue size to device */
    switch (pBC->sLPQ.u32Entries)
    {
        default:
        case ACEX_MEMSZ_BC_LPQ_512:
        {
            u32QueueBits = BC_QSIZE_BITS_512;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_256:
        {
            u32QueueBits = BC_QSIZE_BITS_256;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_128:
        {
            u32QueueBits = BC_QSIZE_BITS_128;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_64:
        {
            u32QueueBits = BC_QSIZE_BITS_64;
            break;
        }

        case ACEX_MEMSZ_BC_LPQ_32:
        {
            u32QueueBits = BC_QSIZE_BITS_32;
            break;
        }
    }

    u32Addr = *pBC->pu32RegBA + REG_BC_HP_ASYNC_CTRL;
    u32Value = BC_HPQ_RESET | (pBC->sHPQ.u32BaseAddr << BC_HPQ_BA_SHIFT) | u32QueueBits;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (unsigned int)u32Addr, (unsigned int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address\n", u16Ch);

        return status;
    }

    /* set BC flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_HPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcHpqFree
 *
 * Description:
 *      This function frees HP Queue.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcHpqFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Addr;
    U32BIT u32Value;

    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* disable HPQ */
    u32Addr = *pBC->pu32RegBA + REG_BC_CONFIG;
    status = DDC_REG_READ(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "READ REG:%08x VALUE:%08x\n", (int)u32Addr, (int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Read config reg fail\n", u16Ch);

        return status;
    }

    u32Value &= ~BC_CONFIG_HPQ_ENABLE;
    status = DDC_REG_WRITE(pDeviceContext, u32Addr, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
        "WRITE REG:%08x VALUE:%08x\n", (int)u32Addr, (int)u32Value);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_ASYNC,
            "CH%d - Set base address\n", u16Ch);

        return status;
    }

    /* clear mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_HPQ;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcDataStreamInit
 *
 * Description:
 *      This function initializes information for data stream.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     dataStrID
 *          Param2:     number of messages to send
 *          Param3:     number of messages to receive
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcDataStreamInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U8BIT u8DataStrId;
    U16BIT u16MsgNumSnd;
    U16BIT u16MsgNumRcv;

    ACEX_BC_DATASTR *pDataStr;

    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u8DataStrId =  (U8BIT)(pIoctlParams->Param1 & BC_DATASTR_ID_MASK);
    u16MsgNumSnd = (U16BIT)(pIoctlParams->Param2 & BC_DATASTR_MSGNUM_MASK);
    u16MsgNumRcv = (U16BIT)(pIoctlParams->Param3 & BC_DATASTR_MSGNUM_MASK);

    pDataStr = &pBC->sDataStrChan[u8DataStrId].sRcv;
    pDataStr->u16MsgNum = u16MsgNumRcv;
    pDataStr->u16MsgCount = 0;
    pDataStr->u16MsgErrLocation = 0;
    pDataStr->u16MsgErrBsw = 0;

    pDataStr = &pBC->sDataStrChan[u8DataStrId].sSnd;
    pDataStr->u16MsgNum = u16MsgNumSnd;
    pDataStr->u16MsgCount = 0;
    pDataStr->u16MsgErrLocation = 0;
    pDataStr->u16MsgErrBsw = 0;

    return status;
}

/*******************************************************************************
 * Name:    bcDataStreamCheckCompletion
 *
 * Description:
 *      This function checks if the data stream is completed.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     dataStrID
 *          Param2:     set if it is out from RT1
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure ACEX_BC_DATASTR_STATUS
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcDataStreamCheckCompletion
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U8BIT u8DataStrId;
    U32BIT u32Sender;

    U16BIT u16MsgCount;
    U16BIT u16MsgNum;

    ACEX_BC_DATASTR_STATUS *pDataStrStatus;
    ACEX_BC_DATASTR *pDataStr;

    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u8DataStrId = ((U8BIT)(pIoctlParams->Param1)) & BC_DATASTR_ID_MASK;
    u32Sender = DDC_IOCTL_U32(pIoctlParams->Param2);

    /* trigger the IMP to fetch data into driver */
    _bcImpStop(pDeviceContext, u16Ch);

    *pBytesReturned = sizeof(ACEX_BC_DATASTR_STATUS);
    pDataStrStatus = (ACEX_BC_DATASTR_STATUS *)pRdData;

    /* check data streaming direction */
    if (u32Sender)
    {
        /* data stream out from RT1 */
        pDataStr = &pBC->sDataStrChan[u8DataStrId].sSnd;
    }
    else
    {
        /* data stream in to RT1 */
        pDataStr = &pBC->sDataStrChan[u8DataStrId].sRcv;
    }

    /* get dataStr spin lock*/
    DDC_ISR_LOCK_TAKE((pBC->slDataStr), pBC->slDataStrFlag);

    u16MsgCount = pDataStr->u16MsgCount;
    u16MsgNum = pDataStr->u16MsgNum;

    /* check if data stream completed already */
    if ((u16MsgNum == 0) || (u16MsgCount >= u16MsgNum))
    {
        /* Data streaming completed */
        pDataStrStatus->bComplete = TRUE;
    }
    else
    {
        pDataStrStatus->bComplete = FALSE;
    }

    pDataStrStatus->u16MsgErrLocation = pDataStr->u16MsgErrLocation;
    pDataStrStatus->u16MsgErrBsw = pDataStr->u16MsgErrBsw;

    /* release dataStr spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slDataStr), pBC->slDataStrFlag);

    return status;
}

/*******************************************************************************
 * Name:    bcDataArrayInit
 *
 * Description:
 *      This function initializes information for data array.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     dataArrayID
 *          Param2:     number of messages to send
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcDataArrayInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U8BIT u8DataArrayId;
    U16BIT u16MsgNum;

    ACEX_BC_DATA_ARRAY *pDataArray;

    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u8DataArrayId = (U8BIT)(pIoctlParams->Param1 & BC_DATARRAY_ID_MASK);
    u16MsgNum = (U16BIT)(pIoctlParams->Param2 & BC_DATARRAY_MSGNUM_MASK);
    pDataArray = &pBC->sDataArrayChan[u8DataArrayId].sSnd;

    pDataArray->u16MsgToPostMax = u16MsgNum;
    pDataArray->u16MsgToPostAvail = u16MsgNum;
    pDataArray->u16MsgErrLocation = 0;
    pDataArray->u16MsgErrBsw = 0;
    pDataArray->u16MsgToPostEmptyMark = (U16BIT)((u16MsgNum * (100 - BC_DATARRAY_MSG_FULL_MARK)) / 100);

    if (pDataArray->u16MsgToPostEmptyMark == 0)
    {
        pDataArray->u16MsgToPostEmptyMark++;
    }

    return status;
}

/*******************************************************************************
 * Name:    bcDataArrayCheckStatus
 *
 * Description:
 *      This function checks the data array status.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to the command from user
 *          Channel:    channel number  0-31
 *          Param1:     dataStrID
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         point to a return info structure ACEX_BC_DATA_ARRAY_STATUS
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcDataArrayCheckStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U8BIT u8DataArrayId;

    ACEX_BC_DATA_ARRAY_STATUS *pDataArrayStatus;
    ACEX_BC_DATA_ARRAY *pDataArray;

    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u8DataArrayId = ((U8BIT)(pIoctlParams->Param1)) & BC_DATARRAY_ID_MASK;

    /* trigger the IMP to fetch data into driver */
    _bcImpStop(pDeviceContext, u16Ch);

    *pBytesReturned = sizeof(ACEX_BC_DATA_ARRAY_STATUS);
    pDataArrayStatus = (ACEX_BC_DATA_ARRAY_STATUS *)pRdData;

    /* point to data array information */
    pDataArray = &pBC->sDataArrayChan[u8DataArrayId].sSnd;

    /* get dataArray spin lock*/
    DDC_ISR_LOCK_TAKE((pBC->slDataArray), pBC->slDataArrayFlag);

    pDataArrayStatus->u16MsgToPostAvail = pDataArray->u16MsgToPostAvail;
    pDataArrayStatus->u16MsgErrLocation = pDataArray->u16MsgErrLocation;
    pDataArrayStatus->u16MsgErrBsw = pDataArray->u16MsgErrBsw;

    /* release dataStr spin lock */
    DDC_ISR_LOCK_GIVE((pBC->slDataArray), pBC->slDataArrayFlag);

    return status;
}

/*-------------------------------------------------------------------------------
   Function:
       bcReplayInit

   Description:
        This function initializes replay engine
        memory area.

        Note: All OSes should use this as common
              code prior to core!!!!!!!!!!!!

   Parameters:
      In pDeviceContext - device-specific structure
      In pIoctlParams         - point to IO command from user
            Channel:    channel number  0-31
            Param1:     replay address base
            Param2:     replay address size
            Param3:     not used
            Param4:     not used

   Returns:
      NTSTATUS
   ---------------------------------------------------------------------------------*/
S16BIT bcReplayInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    ACEX_BC_REPLAY_INFO *psBcReplayInfo = (ACEX_BC_REPLAY_INFO *) pIoctlParams;

    U32BIT u32RegVal;
    U16BIT u16Channel = ((ACEX_CONFIG_ID *) pIoctlParams)->u16Channel;
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    struct _ACEX_1553_BC_TYPE *pBC = &pCh->sBC;
    S16BIT Status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Channel >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
            "CH%d - channel out of range\n", u16Channel);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* initialize replay information */
    pBC->sReplayBuf.u32BaseAddr = (psBcReplayInfo->u32BaseAddr << 2);
    pBC->sReplayBuf.u32MemSize = (psBcReplayInfo->u32MemSize << 2);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
        "CH%d - replay BA 0x%08x, Size 0x%08x\n",
        u16Channel, pBC->sReplayBuf.u32BaseAddr, pBC->sReplayBuf.u32MemSize);


    /* Allocate replay DMA buffer */
    pDeviceContext->sDMA.sizetReplayDMA = pBC->sReplayBuf.u32MemSize / 2;

#if DDC_DMA_REPLAY
    pDeviceContext->sDMA.pu8ReplayDMATarget = DDC_DMA_MALLOC(
        pDeviceContext,
        pDeviceContext->sDMA.sizetReplayDMA,
        &pDeviceContext->sDMA.dmaAddrReplayDMA,
        DDC_MEMORY_REGION__BC_REPLAY_DMA);

    if (!pDeviceContext->sDMA.pu8ReplayDMATarget)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
            "CH%d - DMA memory allocation failed\n", u16Channel);

        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }
#endif /* DDC_DMA_REPLAY */

    /* initialize replay buffer address register (relative addressing) */
    u32RegVal = (psBcReplayInfo->u32BaseAddr << 1) - (*pCh->pu32MemBA << 1); /* 16-bit address */

    DDC_REG_WRITE(pDeviceContext, (*(pCh->sReplay.pu32RegBA) + REG_REPLAY_BUFFER_ADDRESS), &u32RegVal);

    /* initialize replay buffer size register (relative addressing) */
    u32RegVal = BC_REPLAY_BUFFER_LEN_128K;

    DDC_REG_WRITE(pDeviceContext, (*(pCh->sReplay.pu32RegBA) + REG_REPLAY_BUFFER_LENGTH), &u32RegVal);

    /* initialize replay DMA event */
    pBC->sReplayBuf.wDmaCmpltEventCond = 0;

    /* enable replay interrupt */
    gen1553InterruptSet(pDeviceContext, (U8BIT)u16Channel, GENERAL_INT_STATUS_REPLAY_INT_ENABLED);

    return Status;
}

/*-------------------------------------------------------------------
   Function: bcMuxInit

   Routine Description:
    This function sets the amplitude for a particular channel.

    BIT     DESCRIPTION
    -----------------------------------------
    0    1553 Channe3_4 Bus Sharing Enabled
    1    1553 Channe1_2 Bus Sharing Enabled

   Arguments:
        pDeviceContext  - pointer to device context
        pIoctlParams - pointer to ACEX_BC_MUX_INFO structure

   Return Value:
    NTSTATUS
   -----------------------------------------------------------------------*/
S16BIT bcMuxInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    ACEX_BC_MUX_INFO *psBcMuxInfo = (ACEX_BC_MUX_INFO *) pIoctlParams;

    U16BIT u16Ch = (U16BIT) psBcMuxInfo->sConfigID.u16Channel;
    U32BIT u32BoardInst;
    U32BIT u32MuxEnable;
    U32BIT u32RegVal = 0;

    /* determine board instance */
    u32BoardInst = pDeviceContext->sBoardInstanceIndex[u16Ch];

    /* read current register contents */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[u32BoardInst]->pu32RegBA) + REG_BD_CHAN_SHARE_MUX), &u32RegVal);

    u32MuxEnable = psBcMuxInfo->u32MuxEnable;

    /* enable or disable channel mux based on channel */
    if (u32MuxEnable & ACEX_BC_CHAN_MUX_ENA)
    {
        if (u16Ch == 0 || u16Ch == 1)
        {
            u32RegVal |= 1;
        }
        else
        {
            u32RegVal |= 2;
        }
    }
    else
    {
        if (u16Ch == 0 || u16Ch == 1)
        {
            u32RegVal &= ~1;
        }
        else
        {
            u32RegVal &= ~2;
        }
    }

    /* write updated register contents */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[u32BoardInst]->pu32RegBA) + REG_BD_CHAN_SHARE_MUX), &u32RegVal);

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
       bcReplayStatus

   Description:
        This function checks the replay engine status.

   Parameters:
      In  pDeviceContext     - device-specific structure
      In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     not used
            Param2:     not used
            Param3:     not used
            Param4:     not used
      Out  pBytesReturned    - bytes returned
      Out  pRdData           - points to a return info structure ACEX_BC_REPLAY_STATUS

   Returns:
      NTSTATUS
   ---------------------------------------------------------------------------------*/
S16BIT bcReplayStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    S16BIT Status = DDC_UDL_ERROR__SUCCESS;

    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Channel]->sBC);

    ACEX_BC_REPLAY_STATUS *pReplayStatus;

    *pBytesReturned = sizeof(ACEX_BC_REPLAY_STATUS);
    pReplayStatus = (ACEX_BC_REPLAY_STATUS *) pRdData;

    pReplayStatus->u32BaseAddr = pBC->sReplayBuf.u32BaseAddr;
    pReplayStatus->u32MemSize = pBC->sReplayBuf.u32MemSize;

    return Status;
}

/*-------------------------------------------------------------------------------
   Function:
       bcReplayDma

   Description:
        This function queues a bc replay dma request.

   Parameters:
      In  pDeviceContext     - device-specific structure
      In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     replay memory page
            Param2:     replay memory offset
            Param3:     replay data length in bytes;
            Param4:     not used
      Out  pBytesReturned    - bytes returned
      In   pWrData           - points to a replay data buffer

   Returns:
      NTSTATUS
   ---------------------------------------------------------------------------------*/
S16BIT bcReplayDma
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
)
{
    S16BIT Status = DDC_UDL_ERROR__SUCCESS;

    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    U16BIT u16MemPage = (U16BIT)pIoctlParams->Param1;
    U32BIT u32MemOffset = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32DataLength = DDC_IOCTL_U32(pIoctlParams->Param3);

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    struct _ACEX_1553_BC_TYPE *pBC = &pCh->sBC;

    /* calculate replay start address based on memory base address, page, and offset */
    U32BIT u32AddrBase = pBC->sReplayBuf.u32BaseAddr;  /* in bytes */
    U32BIT u32AddrStart = u32AddrBase + ((pBC->sReplayBuf.u32MemSize / 2) * u16MemPage) + u32MemOffset;  /* in bytes */

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
        "ch%d: DMA BA %08X, StartAddr %08X, Offset %d, Page %d, dataBytes %d (0x%08X)\n",
        u16Channel, u32AddrBase, u32AddrStart, u32MemOffset, u16MemPage, u32DataLength, u32DataLength);

#if DDC_DMA_REPLAY

    /* setup DMA */
    dmaReplaySetup(pDeviceContext, (U8BIT)u16Channel, u32DataLength, u32AddrStart, (U8BIT *)pWrData);

    /* Enable DMA Complete interrupt bits */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ | BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT);

    /* start DMA and wait for DMA completion */
    pBC->sReplayBuf.wDmaCmpltEventCond = 0;
    dmaQueueStart(pDeviceContext);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
        "ch%d: wait for DMA completion\n",  u16Channel);

    DDC_WAIT_INTERRUPTABLE(pBC->sReplayBuf.waitqueueDmaCmpltEvent, pBC->sReplayBuf.wDmaCmpltEventCond);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_REPLAY,
        "ch%d: DMA completed\n",  u16Channel);
#else
    DDC_BLK_MEM_WRITE(pDeviceContext, u32AddrStart / 4, pWrData, u32DataLength / 4, ACEX_32_BIT_ACCESS);
#endif

    return Status;
}

/*-------------------------------------------------------------------------------
   Function:
       bcReplayWrite

   Description:
        This function queues a bc replay dma request.

   Parameters:
      In  pDeviceContext     - device-specific structure
      In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     replay memory page
            Param2:     replay memory offset
            Param3:     replay data length in bytes;
            Param4:     not used
      Out  pBytesReturned    - bytes returned
      In   pWrData           - points to a replay data buffer

   Returns:
      NTSTATUS
   ---------------------------------------------------------------------------------*/
S16BIT bcReplayWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
)
{
    S16BIT Status = DDC_UDL_ERROR__SUCCESS;

    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;
    U16BIT u16MemPage = (U16BIT)pIoctlParams->Param1;
    U32BIT u32MemOffset = DDC_IOCTL_U32(pIoctlParams->Param2);
    U32BIT u32DataLength = DDC_IOCTL_U32(pIoctlParams->Param3 >> 2); /* dwords */

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    struct _ACEX_1553_BC_TYPE *pBC = &pCh->sBC;

    /* calculate replay start address based on memory base address, page, and offset */
    U32BIT u32AddrBase = pBC->sReplayBuf.u32BaseAddr;
    U32BIT u32AddrStart = u32AddrBase + ((pBC->sReplayBuf.u32MemSize / 2) * u16MemPage) + u32MemOffset;

    /* convert start byte address to 32-bit address */
    u32AddrStart >>= 2;

    /* write replay message data - this is a patch until the random dma failure is understood,  dma is of little value here right now */
    DDC_BLK_MEM_WRITE(pDeviceContext, u32AddrStart, pWrData, u32DataLength, ACEX_32_BIT_ACCESS);

    return Status;
}

/*-------------------------------------------------------------------------------
   Function:
       bcReplayRaw

   Description:
        This function raises the IRQL Level To DISPATCH and executes a single
        raw replay message.

   Parameters:
      In  pDeviceContext     - device-specific structure
      In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     not used
            Param2:     not used
            Param3:     not used
            Param4:     not used

   Returns:
      S16BIT
   ---------------------------------------------------------------------------------*/
S16BIT  bcReplayRaw
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
/*-------------------------------------------------------------------------------*/
    S16BIT Status = DDC_UDL_ERROR__SUCCESS;

    U32BIT u32RegVal;

    U16BIT u16Channel = (U16BIT)pIoctlParams->Channel;

    u32RegVal = DDC_IOCTL_U32(pIoctlParams->Param1); /* irq mask value */

    /* write replay irq mask */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pChannel1553[u16Channel]->sReplay.pu32RegBA) + REG_REPLAY_IRQ_MASK), &u32RegVal);

    u32RegVal = 1; /* start replay */

    /* start replay engine */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pChannel1553[u16Channel]->sReplay.pu32RegBA) + REG_REPLAY_CONTROL), &u32RegVal);

    return Status;
}

/*******************************************************************************
 * Name:    bcExtTriggerAvailInfo
 *
 * Description:
 *      This function returns external trigger availability information.
 *
 * In   pDeviceContext  device-specific structure
 * Out  pBytesReturned  bytes returned
 * Out  pRdData         rturn value
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcExtTriggerAvailInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    /* PCI compatible devices may be available for the external trigger */
    *pBytesReturned = sizeof(U32BIT);

    switch (pDeviceContext->sHwVersionInfo.dwModelNumber)
    {
        case BU67104C_PC104P_ACEX_MODEL_NUMBER:
        case BU67105C_PC104P_ACEX_MODEL_NUMBER:
        case BUQPRIME_CARD_ACEX_MODEL_NUMBER:
        {
            /* External trigger is available, using direct input */
            *pRdData = ACEX_BC_EXT_TRIGGER_AVAIL | ACEX_BC_EXT_TRIGGER_USING_DIRECT_INPUT;
            break;
        }

        case BU67106K_PCIE_ACEX_MODEL_NUMBER:
        case BU67107FM_PMC_ACEX_MODEL_NUMBER:
        case BU67108C_PC104P_ACEX_MODEL_NUMBER:
        case BU67109C_PC104P_ACEX_MODEL_NUMBER:
        case BU67110FM_PMC_HD_ACEX_MODEL_NUMBER:
        case BU67206XK_PCIE_MF_MODEL_NUMBER:
        case BU67210FM_PMC_HD_MF_MODEL_NUMBER:
        {
            /* External trigger is available, using discrete IO input */
            *pRdData = ACEX_BC_EXT_TRIGGER_AVAIL | ACEX_BC_EXT_TRIGGER_USING_DIO;
            break;
        }

        default:
        {
            /* External trigger is not available in other cards */
            *pRdData = ACEX_BC_EXT_TRIGGER_NOT_AVAIL;
            break;
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    bcOpenAction
 *
 * Description:
 *      This function opens BC using given configuration information for the
 *      specified channel.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcOpenAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32Data;

    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pIoctlParams)->u16Channel;
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pCh->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_OPEN,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    if (pBC->u32ModeFlag & BC_MODE_FLAG_ON)
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* Fix for TT919, try to reset driver in case a prior application shutdown abnormally*/
        bcCloseAction(pDeviceContext, u16Ch);
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
    }

    /* reset BC HW */
    u32Data = (BD_RESET_1553_CH0_BC << (u16Ch * 4));
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, u32Data);

    /* reset Replay HW*/
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_REPLAY)
    {
        /* reset Replay HW - NOP for SF */
        u32Data = (BD_RESET_1553_CH0_REPLAY << (u16Ch * 4));
        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_REPLAY, u32Data);
    }

    /* set to initial values */
    bcInitValues(pBC);

    /* set BC mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_ON;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    /* initialize tempBuf */
    status = bcTempBufCreate(pDeviceContext, u16Ch);

    if (status)
    {
        return status;
    }

    /* initialize msgBuf */
    status = bcMsgBufCreate(pDeviceContext, u16Ch);

    if (status)
    {
        return status;
    }

    /* do not install HBuf here. HBuf will be installed by a separate call */

    /* open IMP */
    status = bcImpOpen(pDeviceContext, u16Ch);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_OPEN,
            "bcImpOpen Ch%d\n", u16Ch);

        return status;
    }

    /* set BC specific IMP interrupt mask */
    status = bcImpInterruptSet(pDeviceContext, u16Ch);

    if (status)
    {
        return status;
    }

    /* start IMP to process IMP input queue */
    status = bcImpStart(pDeviceContext, u16Ch);

    if (status)
    {
        return status;
    }

#if BC_ASYNC_DEBUG
    bcRegE(pDeviceContext, u16Ch, "BC Opened: ");
#endif /* BC_ASYNC_DEBUG */

    return status;
}

/*******************************************************************************
 * Name:    bcOpen
 *
 * Description:
 *      This function opens BC using given configuration information for the
 *      specified channel. If the open fails, close it before exit.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    S16BIT status;

    status = bcOpenAction(pDeviceContext, pIoctlParams);

    if (status)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_OPEN,
            "Open channel failed. Close the channel...\n");

        bcClose(pDeviceContext, pIoctlParams);
    }

    return status;
}

/*******************************************************************************
 * Name:    bcCloseAction
 *
 * Description:
 *      This function close BC for a given channel.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcCloseAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
#if BC_IMP_MSG_METRIC_DBG
    U32BIT u32QueueCount;
    U32BIT u32QueueStatus;
#endif /* BC_IMP_MSG_METRIC_DBG */
    U32BIT u32BcDelayTimeOutCount;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* exit if channel number out of range */
    if (u16Ch >= (U16BIT)pDeviceContext->u8Num1553Channels)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
            "CH%d - channel out of range\n", u16Ch);

        return DDC_UDL_ERROR__BC_STATUS_MODE;
    }

    /* summary information*/
    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - INT Count ........ %d\n", u16Ch, (int)pBC->u32IntCount);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - IMP INT Count .... %d\n", u16Ch, (int)pBC->u32ImpIntCount);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - MSG Count ........ %d\n", u16Ch, (int)pBC->u32MsgCount);

#if BC_IMP_MSG_METRIC_DBG

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Total MSG Count From RegB ... %d\n\n", u16Ch, pBC->u32MsgCountFromRegB);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Max MSG Count from INT ...... %d\n", u16Ch, pBC->u32MaxMsg);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Max MSG Count From RegB ..... %d\n", u16Ch, pBC->u32MaxMsgFromRegB);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Output Queue Full Count ..... %d\n", u16Ch, pBC->u32FullCount);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Max MSG Count Diff .......... %d\n", u16Ch, pBC->s32MaxDiff);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - Min MSG Count Diff .......... %d\n\n", u16Ch, pBC->s32MinDiff);


    /* read in IMP input queue statistics from BC test registers: B and C */
    DDC_BLK_REG_READ(pDeviceContext, *(pBC->pu32RegBA) + REG_BC_TEST_MODE, &u32QueueCount, 1);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - IMP Input Queue count ....... %d\n", u16Ch, (u32QueueCount >> 17) & 0xFFFF);           /* one extra right shift for divide 2 */

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - IMP Output Queue count ...... %d\n", u16Ch, (u32QueueCount >> 1) & 0xFFFF);            /* one extra right shift for divide 2 */


    DDC_BLK_REG_READ(pDeviceContext, *(pBC->pu32RegBA) + REG_BC_HOST_ACCESS_CTRL, &u32QueueStatus, 1);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - IMP Input Queue Max Value ... %d\n", u16Ch, (u32QueueStatus >> 23) & 0x1FF);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_CLOSE,
        "CH%d - IMP Queue Status ............ 0x%x\n\n", u16Ch, u32QueueStatus & 0xF);


#endif /* BC_IMP_MSG_METRIC_DBG */

    /* set BC mode off if BC is not in BUSY state */
    u32BcDelayTimeOutCount = (ACEX_BC_CLOSE_TIMEOUT_MS / ACEX_BC_CLOSE_DELAY_MS) + 1;

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag |= BC_MODE_FLAG_OFF_PENDING;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    while (u32BcDelayTimeOutCount)
    {
        DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
        if (!(pBC->u32ModeFlag & (BC_MODE_FLAG_INT_BUSY | BC_MODE_FLAG_USER_BUSY)))
        {
            pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;

            DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
            break;
        }
        DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

        /* wait for ACEX_BD_RESET_DELAY_MS ms */
        ddcUdlOsWaitMsInterruptable(ACEX_BC_CLOSE_DELAY_MS);
        u32BcDelayTimeOutCount--;
    }

    /* close IMP */
    bcImpClose(pDeviceContext, u16Ch);

    /* clear BC specific IMP interrupt mask */
    status |= bcImpInterruptClear(pDeviceContext, u16Ch);

    /* disable HBuf */
    status |= bcHbufDisableAction(pDeviceContext, u16Ch);

    /* free msgBuf */
    status |= bcMsgBufFree(pDeviceContext, u16Ch);

    /* Free tempBuf */
    bcTempBufFree(pDeviceContext, u16Ch);

    /* Free GPQ */
    status |= bcGpqFree(pDeviceContext, u16Ch);

    /* Free LPQ */
    status |= bcLpqFree(pDeviceContext, u16Ch);

    /* Free HPQ */
    status |= bcHpqFree(pDeviceContext, u16Ch);

    /* free pending thread */
    pBC->u32MsgEventMsgIdx = 0xFFFFFFFF;
    pBC->wMsgEventCond = 1;

    DDC_WAKE_UP(&pBC->waitqueueMsgEvent);

    /* set to initial values */
    bcInitValues(pBC);

    /* clear BC mode flag */
    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_ON;
    pBC->u32ModeFlag &= ~BC_MODE_FLAG_OFF_PENDING;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);

#if DDC_DMA_REPLAY
    /* free replay DMA buffer */
    if (pDeviceContext->sDMA.pu8ReplayDMATarget)
    {
        DDC_DMA_FREE(
            pDeviceContext,
            pDeviceContext->sDMA.sizetReplayDMA,
            pDeviceContext->sDMA.pu8ReplayDMATarget,
            pDeviceContext->sDMA.dmaAddrReplayDMA,
            DDC_MEMORY_REGION__BC_REPLAY_DMA);
    }
#endif

    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);
    return status;
}

/*******************************************************************************
 * Name:    bcClose
 *
 * Description:
 *      This function close BC using given configuration information for the
 *      specified channel.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pIoctlParams)->u16Channel;

#if BC_ASYNC_DEBUG
    bcRegE(pDeviceContext, u16Ch, "BC Closed: ");
#endif /* BC_ASYNC_DEBUG */

    return bcCloseAction(pDeviceContext, u16Ch);
}

/*******************************************************************************
 * Name:    bcSetState
 *
 * Description:
 *      This function sets BC states. In case of BC state transits from RUN state
 *      to READY/RESET states, IMP will be flushed. In other words, all messages
 *      in IMP will be fetched into BC.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          point to IO command from user
 *          Channel:    channel number  0-31
 *          Param1:     current state
 *          Param2:     not used
 *          Param3:     not used
 *          Param4:     not used
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = ((ACEX_CONFIG_ID *)pIoctlParams)->u16Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_SET_STATE,
        "bcSetState %d\n", (int)pIoctlParams->Param1);

    /* save the state */
    pBC->state = (ACEX_MOD_STATE)pIoctlParams->Param1;

    return status;
}

/*-----------------------------------------------------------------------------
    Function: bcSetRespTimeOut

    Description: This function sets the response timeout timer on the hardware.
        Valid Response Time Out range is from 0.5us to 127.5us

    Parameters:
        In pDeviceContext   - input value for instance information
                                associated with this particular device.
        In pIoctlParams           - input value

    Returns:
        NTSTATUS
   ------------------------------------------------------------------------------*/
S16BIT bcSetRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    U32BIT u32RespTimeout = 0;

    DDC_REG_READ(pDeviceContext,  ((*(pBC->pu32RegBA)) + REG_BC_CONFIG), &u32RespTimeout);

    u32RespTimeout &= ~BC_CONFIG_RESP_TIMEOUT_MASK;
    u32RespTimeout |= DDC_IOCTL_U32(pIoctlParams->Param2 << BC_CONFIG_RESP_TIMEOUT_SHIFT);

    DDC_REG_WRITE(pDeviceContext, ((*(pBC->pu32RegBA)) + REG_BC_CONFIG), &u32RespTimeout);

    /* Initialize global response timeout variable */
    pBC->u32RespTimeout = u32RespTimeout;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    bcInitValues
 *
 * Description:
 *      This function sets default BC values.
 *
 * In   pBC     pointer to BC structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void bcInitValues
(
    struct _ACEX_1553_BC_TYPE *pBC
)
{
    U8BIT u8DataStrId;
    U8BIT u8DataArrayId;

    ACEX_BC_DATASTR *pDataStr;
    ACEX_BC_DATA_ARRAY *pDataArray;

    DDC_ISR_LOCK_TAKE(pBC->slBcMode, pBC->slBcModeFlag);
    pBC->u32ModeFlag = 0;
    DDC_ISR_LOCK_GIVE(pBC->slBcMode, pBC->slBcModeFlag);

    pBC->u32BufFlag = 0;
    pBC->u32IntCount = 0;
    pBC->u32ImpIntCount = 0;
    pBC->u32MsgCount = 0;

    pBC->u32IntMask = 0;

    pBC->u32IrqEvThis = 0;
    pBC->u32IrqEvLast = 0;

    pBC->u32MsgCountFromRegB = 0;
    pBC->u32MaxMsg = 0;
    pBC->u32MaxMsgFromRegB = 0;
    pBC->u32FullCount = 0;
    pBC->s32MaxDiff = -1000000;
    pBC->s32MinDiff = 1000000;

    /* the data size from IMP */
    pBC->u32CmdDwords = BC_CMD_STS_DWORDS;
    pBC->u32DataDwords = BC_DATA_DWORDS;
    pBC->u32CmdDataDwords = pBC->u32CmdDwords + pBC->u32DataDwords;

    /* the message size to return */
    pBC->u32HBufMsgWords = pBC->u32CmdDwords + (pBC->u32DataDwords << 1);

    /* initialize all data stream channel information */
    for (u8DataStrId = 0; u8DataStrId < NUM_DATASTR_CHANNELS; u8DataStrId++)
    {
        pDataStr = &pBC->sDataStrChan[u8DataStrId].sRcv;
        pDataStr->u16MsgNum = 0;
        pDataStr->u16MsgCount = 0;
        pDataStr->u16MsgErrLocation = 0;
        pDataStr->u16MsgErrBsw = 0;

        pDataStr = &pBC->sDataStrChan[u8DataStrId].sSnd;
        pDataStr->u16MsgNum = 0;
        pDataStr->u16MsgCount = 0;
        pDataStr->u16MsgErrLocation = 0;
        pDataStr->u16MsgErrBsw = 0;
    }

    /* initialize all data array channel information */
    for (u8DataArrayId = 0; u8DataArrayId < NUM_DATARRAY_CHANNELS; u8DataArrayId++)
    {
        pDataArray = &pBC->sDataArrayChan[u8DataArrayId].sSnd;

        pDataArray->u16MsgToPostMax = BC_DATARRAY_MSG_MAX;
        pDataArray->u16MsgToPostAvail = BC_DATARRAY_MSG_MAX;
        pDataArray->u16MsgErrLocation = 0;
        pDataArray->u16MsgErrBsw = 0;
        pDataArray->u16MsgToPostEmptyMark = (U16BIT)((pDataArray->u16MsgToPostMax * (100 - BC_DATARRAY_MSG_FULL_MARK)) / 100);
    }
}

/*******************************************************************************
 * Name:    bcInitialize
 *
 * Description:
 *      This function initializes BC when USB hardware is going to be re-initialized.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           1553 channel number
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    struct _ACEX_1553_BC_TYPE *pBC = &(pDeviceContext->pChannel1553[u16Ch]->sBC);
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* initialize INT spin lock */
    DDC_ISR_LOCK_INIT(pBC->slBcMode);

    /* initialize HBuf spin lock */
    DDC_ISR_LOCK_INIT(pBC->slHBuf);

    /* initialize MsgBuf spin lock */
    DDC_ISR_LOCK_INIT(pBC->slMsgBuf);

    /* initialize DataBuf spin lock */
    DDC_ISR_LOCK_INIT(pBC->slDataBuf);

    /* Initialize Data Stream spin lock */
    DDC_ISR_LOCK_INIT(pBC->slDataStr);

    /* Initialize Data Array spin lock */
    DDC_ISR_LOCK_INIT(pBC->slDataArray);

    /* initialize Gpq spin lock */
    DDC_ISR_LOCK_INIT(pBC->slGpq);

    /* initialize Lpq spin lock */
    DDC_ISR_LOCK_INIT(pBC->slLpq);

    /* initialize Hpq spin lock */
    DDC_ISR_LOCK_INIT(pBC->slHpq);

    /* initialize msg wait queue */
    pBC->wMsgEventCond = 2;

    DDC_INIT_WAITQUEUE_IRQ(&pBC->waitqueueMsgCallback, &pBC->waitqueueMsgEvent);

    /* initialize replay DMA event */
    pBC->sReplayBuf.wDmaCmpltEventCond = 0;
    DDC_INIT_WAITQUEUE_IRQ(&pBC->sReplayBuf.waitqueueDmaCmpltCallback, &pBC->sReplayBuf.waitqueueDmaCmpltEvent);

    /* set to initial values */
    pBC->state = ACEX_MOD_RESET;
    bcInitValues(pBC);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BC, DDC_DBG_BC_INIT,
        "Done!\n");

    return status;
}

/*******************************************************************************
 * Name:    bcFree
 *
 * Description:
 *      This function frees BC when USB device is going to be removed.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: NTSTATUS
 ******************************************************************************/
S16BIT bcFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT i;

    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        status |= bcCloseAction(pDeviceContext, (U16BIT)i);

        /* continue, we do no care if it is success */
    }

    return status;
}
