/*******************************************************************************
 * FILE: ddc_udl_1553_rt.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support operation of
 *  the Multi-RT HW Block.
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
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_common_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "core/1553/ddc_udl_1553_rt.h"
#include "core/1553/ddc_udl_1553_imp_private.h"


#define ACEX_BROADCAST_RT   31

/* For Read/Write Mode Code Data */
#define ACE_RT_MCDATA_BCST_CODE_BITMASK     0X0020
#define ACEX_BROADCAST_RT 31

const char szModState[4][8] =
{
    "RESET ", "CLOSED", "OPEN  ", "ERROR "
};

/******************************************************************************
 * Name:    mrtInterruptHandler
 *
 * Description:
 *      This is the interrupt handler for Multi-RT.  The function reviews the
 *      contents of the MRT interrupt status register (u32IntStatus), and takes
 *      appropriate action as necessary.
 *
 * Preconditions:
 *      MRT interrupts must be enabled
 *
 *  Postconditions:
 *      All MRT interrupts notified in u32IntStatus will have been serviced
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel of device generating interrupt
 * In   u32IntStatus    contents of MRT interrupt status register
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntStatus
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[u8Ch]->sRT);
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    U32BIT u32Data = 0;
    U32BIT i;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: mrtInterruptHandler enter, IntStatus:0x%08x\n", u8Ch, u32IntStatus);

    if (u32IntStatus & (MRT_INT_ENABLE_MASK_CMD_STK_OVERFLOW
            | MRT_INT_ENABLE_MASK_ADDR_PARITY | MRT_INT_ENABLE_MASK_ILLEGAL_CMD | MRT_INT_ENABLE_MASK_FORMAT_ERROR))
    {
        if (u32IntStatus & MRT_INT_ENABLE_MASK_CMD_STK_OVERFLOW)
        {

            pRT->stats.u32NumCmdStackOverflows++;

            /* read the number of entries counted, which should be >= stack entry size */
            DDC_REG_READ(pDeviceContext, *(pRT->pu32RegBA) + REG_MRT_CMD_STK_ENTRIES, &(pRT->sCmdStk.u32OverflowNumEntries));

            /* update number of lost messages - if an overflow occurs, we
               count all messages as being lost currently in the stack and
               reset the pointer - this prevents repeated interrupts due to
               stack overflow if the stack is not serviced often enough */
            pRT->stats.u32NumCmdStkLost += pRT->sCmdStk.u32OverflowNumEntries;

            mrtRtCmdStkSync(pDeviceContext, u8Ch);

            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER,
                "ch%d: CMD STK OVERFLOW (num:%d) CumLost:%d  ThisTime:%d ------->>>>>>>>\n",
                u8Ch, pRT->stats.u32NumCmdStackOverflows++,
                pRT->stats.u32NumCmdStkLost,
                pRT->sCmdStk.u32OverflowNumEntries);
        }

        if (u32IntStatus & MRT_INT_ENABLE_MASK_ADDR_PARITY)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: ADDR_PARITY\n", u8Ch);
            pRT->stats.u32NumAddrParityErrs++;
            pCh->u32IrqEv |= ACE_IMR1_RT_ADDR_PAR_ERR;
        }

        if (u32IntStatus & MRT_INT_ENABLE_MASK_ILLEGAL_CMD)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: ILLEGAL_CMD\n", u8Ch);
            pRT->stats.u32NumIllegalCmds++;
            pCh->u32IrqEv |= ACE_IMR2_RT_ILL_CMD;
        }

        if (u32IntStatus & MRT_INT_ENABLE_MASK_FORMAT_ERROR)
        {
			DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: FORMAT_ERROR\n", u8Ch);
            pRT->stats.u32NumFmtErrs++;
            pCh->u32IrqEv |= ACE_IMR1_FORMAT_ERR;
        }
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_ISQ_ROLLOVER)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: ISQ_ROLLOVER\n", u8Ch);
        pRT->stats.u32NumIsqRollovers++;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: CMDSTK_50_ROLLOVER\n", u8Ch);
        pRT->stats.u32NumCmdStk50Rollovers++;
        mrtHbufImpPostQueue(pDeviceContext, u8Ch);
        pCh->u32IrqEv |= ACE_IMR2_RT_CSTK_50P_ROVER;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_CBUF_50_ROLLOVER)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: CBUF_50_ROLLOVER\n", u8Ch);
        pRT->stats.u32NumCircBuf50Rollovers++;
        pCh->u32IrqEv |= ACE_IMR2_RT_CIRC_50P_ROVER;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_XMTR_TIMEOUT)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: XMTR_TIMEOUT\n", u8Ch);
        pRT->stats.u32NumTxTimouts++;
        pCh->u32IrqEv |= ACE_IMR1_BCRT_TX_TIMEOUT;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: CMDSTK_100_ROLLOVER\n", u8Ch);
        pRT->stats.u32NumCmdStkRollovers++;
        mrtHbufImpPostQueue(pDeviceContext, u8Ch);
        pCh->u32IrqEv |= ACE_IMR1_BCRT_CMDSTK_ROVER;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_CBUF_ROLLOVER)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: CBUF_100_ROLLOVER\n", u8Ch);
        pRT->stats.u32NumCircBufRollovers++;
        pCh->u32IrqEv |= ACE_IMR1_RT_CIRCBUF_ROVER;

        if (pRT->bDataArrayEnabled == TRUE)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER,
                "ch%d: CBUF_100_ROLLOVER - DATA ARRAY CHECK ENABLED\n", u8Ch);

            /* check if any data arrays are in continuous mode */
            for (i = 0; i < (ACEX_MRT_MAX_DATA_ARRAYS * 32); i++)
            {
                if (pRT->sDataArrayTable[i].eState == DA_CONTINOUS)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER,
                        "ch%d: Data Array %d CONTINUOUS DETECTED\n", u8Ch, i);

                    DDC_MEM_READ(pDeviceContext, pRT->sDataArrayTable[i].u32TxPtrOffsetAddressMRT, &u32Data, ACEX_32_BIT_ACCESS);

                    if (u32Data == pRT->sDataArrayTable[i].u32TxTfrCompleteOffset)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER,
                            "ch%d: Data Array %d Loop Completed\n", u8Ch, i);

                        DDC_MEM_WRITE(pDeviceContext,
                            pRT->sDataArrayTable[i].u32TxPtrOffsetAddressMRT, &(pRT->sDataArrayTable[i].u32TxTfrStartOffset), ACEX_32_BIT_ACCESS);
                    }
                }
            }
        }
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: SA_CTRLWD_EOM\n", u8Ch);
        pRT->stats.u32NumSACWEoms++;
        pCh->u32IrqEv |= ACE_IMR1_RT_SUBADDR_EOM;

        if (pRT->eStreamState[MRT_STREAM_DBLK_TX] == MRT_STREAM_PENDING)
        {
            pRT->eStreamState[MRT_STREAM_DBLK_TX] = MRT_STREAM_COMPLETE;
        }

        if (pRT->eStreamState[MRT_STREAM_DBLK_RX] == MRT_STREAM_PENDING)
        {
            pRT->eStreamState[MRT_STREAM_DBLK_RX] = MRT_STREAM_COMPLETE;
        }
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_MODE_CODE)
    {

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "ch%d: MODE_CODE\n", u8Ch);
        pRT->stats.u32NumMCs++;
        pCh->u32IrqEv |= ACE_IMR1_RT_MODE_CODE;
    }

    if (u32IntStatus & MRT_INT_ENABLE_MASK_EOM)
    {

        pRT->stats.u32NumEoms++;
        pCh->u32IrqEv |= ACE_IMR1_EOM;
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, "%d %d: EOM\n", u8Ch, pRT->stats.u32NumEoms);
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INTERRUPT_HANDLER, " ch%d: mrtInterruptHandler closed\n", u8Ch);
}

/******************************************************************************
 * Name:    mrtInitialize
 *
 * Description:
 *      initialize master multi-rt module.
 *
 *      For SF+, MRT RT specific configuration must occupy the first 32K of memory.
 *
 *      For example, Ch0 memory layout would appear as follows:
 *
 *          1553 Memory Address 0 \------------\
 *                                \    MRT     \
 *                                \ CFG MEMORY \
 *                                \------------\
 *                                \ MTi Tbl    \
 *                                \------------\
 *                                \            \
 *                                \ IMP MEMORY \
 *                                \            \
 *                                \            \
 *                                \            \
 *                                \------------\
 *                                \  User      \
 *                                \  Memory    \
 *                                \  Blks      \
 *                                \            \
 *                                \            \
 *                                \            \
 *                                \            \
 *                                \            \
 *                                \------------\
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            1553 channel number
 * Out  none
 *
 * Returns: status (SUCCESS or ERROR CODE)
 *****************************************************************************/
S16BIT mrtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    int i;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INITIALIZE, "ENTER->Ch[%d]\n", u8Ch);

    /* reset resources in reverse order from mrtOpen */
    /* allocate memory space for Multi-RT config/mgmt */
    pCh->sRT.u32MrtMemSize = ACEX_MRT_MEMSIZE; /* bytes */
    pCh->sRT.u32MrtBA = *(pCh->pu32MemBA) + ACEX_MRT_MEMORY_BA;

    /* adjust memory available to user, and the starting address to that
       memory region */
    pCh->u32UserMemSizeBytes -= ACEX_MRT_MEMSIZE;

    pCh->u32UserMemBA += (ACEX_MRT_MEMSIZE / 4); /* DW addressing */

    /* reset MRT HW */
    if (!pCh->bRtAutoBoot)
    {
        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, (BD_RESET_1553_CH0_RT << (u8Ch * 4)));
    }

    pCh->sRT.u16EventCond = 0;
    DDC_INIT_WAITQUEUE_IRQ(&pCh->sRT.waitqueueCallback, &pCh->sRT.waitqueueEvent);

    pCh->sRT.state = ACEX_MOD_RESET;

    /* clear 1553a flags */
    for (i = 0; i < NUM_RTS; i++)
    {
        pCh->sRT.b1553a[i] = FALSE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_INITIALIZE, "END-> UserMemBA32: 0x%08x MRT STATE: %d\n", (int)pCh->u32UserMemBA, pCh->sRT.state);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtInterruptSet
 *
 * Description:
 *      This function adds the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *      Interrupt bits to be set should be set to 1 in the u32IntMask
 *      parameter.
 *
 *      NOTE: to set all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 *      Interrupt Bits
 *      -----------------------------------
 *      12: Overflow Cmd Stack
 *      11: RT Illegal Cmd
 *      10: Interrupt Status Queue Rollover
 *       9: RT Cmd Stack 50% Rollover
 *       8: RT Circ Buffer 50% Rollover
 *       7: Transmitter Timeout
 *       6: RT Cmd Stack 100% Rollover
 *       5: RT Address Parity
 *       4: RT Circ Buffer 100% Rollover
 *       3: RT SubAddress Ctrl Wd EOM
 *       2: Format Error
 *       1: RT Mode Code
 *       0: EOM
 *      -----------------------------------
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            selected channel of device
 * In   u32IntMask      mask bits to set - value is or'd with existing mask
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* update local copy */
    pCh->sRT.sRtxCfgReg.u32IntMask = pCh->sRT.sRtxCfgReg.u32IntMask | u32IntMask;

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK), &(pCh->sRT.sRtxCfgReg.u32IntMask));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CONFIG_CTRL, "WRITE MRT INT EN REG:%08x VALUE:%08x\n\n",
        ((*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK), pCh->sRT.sRtxCfgReg.u32IntMask);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtInterruptClear
 *
 * Description:
 *      This function adds the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *      Interrupt bits to be cleared should be set to 1 in the u32IntMask
 *      parameter.
 *
 *      NOTE: to clear all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 *      Interrupt Bits
 *      -----------------------------------
 *      12: Overflow Cmd Stack
 *      11: RT Illegal Cmd
 *      10: Interrupt Status Queue Rollover
 *       9: RT Cmd Stack 50% Rollover
 *       8: RT Circ Buffer 50% Rollover
 *       7: Transmitter Timeout
 *       6: RT Cmd Stack 100% Rollover
 *       5: RT Address Parity
 *       4: RT Circ Buffer 100% Rollover
 *       3: RT SubAddress Ctrl Wd EOM
 *       2: Format Error
 *       1: RT Mode Code
 *       0: EOM
 *      -----------------------------------
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            selected channel of device
 * In   u32IntMask      mask bits to clear - bits set to one are cleared in interrupt mask
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* update local copy  */
    pCh->sRT.sRtxCfgReg.u32IntMask = pCh->sRT.sRtxCfgReg.u32IntMask & ~(u32IntMask);

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK), &(pCh->sRT.sRtxCfgReg.u32IntMask));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CONFIG_CTRL, "WRITE MRT INT EN REG:%08x VALUE:%08x\n",
        (int)((*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK), (int)pCh->sRT.sRtxCfgReg.u32IntMask);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtClose
 *
 * Description:
 *      closes the MRT handler
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel of device to close
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    U32BIT u32Register;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    /* verify that all VRT's are closed, if NOT return error */

    /* reset resources in reverse order from mrtOpen */
    gen1553InterruptClear(pDeviceContext, u8Ch, GENERAL_INT_STATUS_RT_INT_ENABLED);

    mrtInterruptClear(pDeviceContext, u8Ch, 0xFFFFFFFF); /* disable all interrupts */

    /* Clear global config register */
    pCh->sRT.sRtxCfgReg.u32RTGConfig = 0;

    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG;

    /* set RT Global Configuration Register */
    DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    impRtClose(pDeviceContext, u8Ch);

    pCh->sRT.state = ACEX_MOD_CLOSED;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CONFIG_CTRL, "END-> MRT STATE=%d\n", pCh->sRT.state);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtOpen
 *
 * Description:
 *      This function configures MRT registers according to the configure information
 *      passed from app.
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxGCfg       Configure information
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_MRT_CONFIG_TYPE *psRtxGCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxGCfg->sConfigID.u16Channel];
    U32BIT u32Register, i;
    U32BIT u32Data = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U8BIT u8RT;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN, "ENTER-> (CH%d) MRT STATE=%d\n",
        psRtxGCfg->sConfigID.u16Channel, pCh->sRT.state);

    if (pCh->sRT.state == ACEX_MOD_OPEN) /* we do not want to open again if we are open */
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* reset MRT HW */
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, (BD_RESET_1553_CH0_RT << (psRtxGCfg->sConfigID.u16Channel * 4)));

    /* clear MRT VRT MEMORY - this is needed so that when global MRT resources are enabled
       unknown settings here could result in undesirable behavior */

    /* cleanup the configuration memory area for each RT */
    for (i = 0; i < NUM_RTS; i++)
    {
        status = gen1553MemClear(pDeviceContext, RTX_VRT_MEMORY_OFFSET, pCh->sRT.u32MrtBA + (i * RTX_VRT_MEMORY_OFFSET));

        if (status)
        {
            return status;
        }
    }

    /* Preset the 'last command' and 'last status' RT address bits for the case when the associated
       'T' mode codes are issued prior to any other messages that would normally update these locations */
    for (u8RT = 0; u8RT <= RTX_BROADCAST_RT; u8RT++)
    {
        u32Register = pCh->sRT.u32MrtBA + ((u8RT << 8) + MEM_RT_MC_LAST_CMD_STS);
        u32Data = (u8RT << 11) | (u8RT << 27);
        DDC_MEM_WRITE(pDeviceContext, u32Register, &u32Data, 0);
    }

    /* we don't have to read from hardware it is always INTERNAL */
    pCh->sRT.u16RtSource = ACE_RT_INTERNAL_ADDR;

    /* set RT Cmd Stack BA */
    pCh->sRT.sRtxCfgReg.u32CmdStkPtr = psRtxGCfg->u32CmdBA;
    pCh->sRT.sCmdStk.u32CmdStkPtrBA = psRtxGCfg->u32CmdBA;

    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_CMD_STK_PTR;
    u32Data = pCh->sRT.sRtxCfgReg.u32CmdStkPtr & pCh->u32ChannelMemDwdMask;
    DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN, "\nCh%d: mrtOpen -- u32CmdStkPointer 0x%08x, BA 0x%08x, mask 0x%08x, Dwd Size 0x%08x, SW index 0x%08x\n\n",
        psRtxGCfg->sConfigID.u16Channel,
        u32Data, pCh->sRT.sRtxCfgReg.u32CmdStkPtr, pCh->u32ChannelMemDwdMask,
        pCh->sRT.sCmdStk.u32RtCmdStkDwdSz, pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex);

    if (impRtOpen(pDeviceContext, (U8BIT)psRtxGCfg->sConfigID.u16Channel) != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN, "impRtOpen CH %d FAIL\n", psRtxGCfg->sConfigID.u16Channel);

        return DDC_UDL_ERROR__HARDWARE_CONFIGURATION;
    }

    /* set RT Isq BA */
    pCh->sRT.sRtxCfgReg.u32IsqPtr = psRtxGCfg->u32IsqBA;
    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_ISQP_RW;
    DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32IsqPtr));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN,
        "WRITE RT Isq BA: REG:0x%08x DATA:0x%08x\n", (int)u32Register, (int)pCh->sRT.sRtxCfgReg.u32IsqPtr);

    pCh->sRT.sCmdStk.u32RtCmdStkDwdSz = (0x0100 << (psRtxGCfg->u16CmdStkType >> 13));
    pCh->sRT.sCmdStk.u32RtCmdStkMsgSz = pCh->sRT.sCmdStk.u32RtCmdStkDwdSz / 4; /* 4 dwds per msg */
    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN, "RT CMD STK size:%d (dwds)\n", (int)pCh->sRT.sCmdStk.u32RtCmdStkDwdSz);

    /* Clear last entry in Rt Cmd Stack */
    ddcUdlBdClearMemory(pDeviceContext, (pCh->sRT.sRtxCfgReg.u32CmdStkPtr * 2), pCh->sRT.sCmdStk.u32RtCmdStkDwdSz * 2);

    /* **** set global configuration register bits **** */
    /* set RT Command Stack Size: 8 right shift = 13 right shift - 5 left shift */
    pCh->sRT.sRtxCfgReg.u32RTGConfig = psRtxGCfg->u16CmdStkType >> 8;

    /* Enable MRT/SRT Mode */
    if (psRtxGCfg->u8MrtEnable)
    {
        pCh->sRT.sRtxCfgReg.u32RTGConfig |= MRT_GCONFIG_MRT_MODE_EN;
        pCh->sRT.bMode = ACEX_MRT_MODE;
    }
    else
    {
        pCh->sRT.sRtxCfgReg.u32RTGConfig &= (~MRT_GCONFIG_MRT_MODE_EN);
        pCh->sRT.bMode = ACEX_SRT_MODE;
    }

    /* Set TT SOM/EOM mask */
    pCh->sRT.sRtxCfgReg.u32RTGConfig |= psRtxGCfg->u16SomEom;

    /* set global data stack (if requested) */
    if (psRtxGCfg->u16GblDataStkType != 0)
    {
        /* global data stack has been requested, so configure it here */
        pCh->sRT.sRtxCfgReg.u32RTGConfig |= (psRtxGCfg->u16GblDataStkType | MRT_GCONFIG_GBL_DSTK_EN);

        /* load  global data stack pointer */
        pCh->sRT.sRtxCfgReg.u32GblDataStkPtr = psRtxGCfg->u32GblDataStkBA;
        u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_GBL_DATA_STK_PTR;
        u32Data = pCh->sRT.sRtxCfgReg.u32GblDataStkPtr & pCh->u32ChannelMemDwdMask;
        DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN,
            "WRITE GLBL Data Stack BA: REG:0x%08x DATA:0x%08x\n", (int)u32Register, (int)u32Data);
    }

    /* set RT Global Configuration Register */
    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG;
    DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));
    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN,
        "WRITE RT GLBL CONFIG: REG:0x%08x DATA:0x%08x\n", (int)u32Register, (int)pCh->sRT.sRtxCfgReg.u32RTGConfig);

    /* TODO : IS THIS STILL NEEDED - INT ACCESS MAY NOT BE REQUIRED AT THE LIBRARY LEVEL */
    pCh->sRT.sRtxCfgReg.u32IntMask = psRtxGCfg->u32IntConditions;
    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK;

    /* set global interrupt masks */
    DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32IntMask));
    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN,
        "WRITE RT GLBL INTERRUPT: REG:0x%08x DATA:0x%08x\n", (int)u32Register, (int)pCh->sRT.sRtxCfgReg.u32IntMask);

    /* store global configuration parameters locally */
    memcpy(&(pCh->sRT.sRtxGCfg), psRtxGCfg, sizeof(ACEX_MRT_CONFIG));

    pCh->sRT.state = ACEX_MOD_OPEN;

    /* read statistics to clear them out */
    memset(&(pCh->sRT.stats), 0, sizeof(ACEX_1553_MRT_STAT_TYPE));
    mrtUpdateRtStkStats(pDeviceContext, (U8BIT)psRtxGCfg->sConfigID.u16Channel);
    memset(&(pCh->sRT.stats), 0, sizeof(ACEX_1553_MRT_STAT_TYPE));
#ifndef DDC_VXB_GEN2
    /* enable the Hardware error interrupts */
    mrtInterruptSet(pDeviceContext,
                    (U8BIT)psRtxGCfg->sConfigID.u16Channel,
                    (MRT_INT_ENABLE_MASK_CMD_STK_OVERFLOW +
                    MRT_INT_ENABLE_MASK_ILLEGAL_CMD +
                    MRT_INT_ENABLE_MASK_FORMAT_ERROR +
                    MRT_INT_ENABLE_MASK_XMTR_TIMEOUT +
                    MRT_INT_ENABLE_MASK_ADDR_PARITY ));
#endif
    /* enable the RT interrupt */
    gen1553InterruptSet(pDeviceContext, (U8BIT)psRtxGCfg->sConfigID.u16Channel, GENERAL_INT_STATUS_RT_INT_ENABLED);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_OPEN,
        "END->  STATE:%d\n"
        "RT Cmd Stack BA:0x%08x\n"
        "RT Cmd Stack SZ:%d dwds (%d msgs)\n"
        "      RT Isq BA:0x%08x\n"
        "GLBL DataStk BA:0x%08x\n"
        "GLBL CFG REGSET:0x%08x\n"
        "UsrIntCondition:0x%08x\n",
        pCh->sRT.state,
        (int)pCh->sRT.sRtxCfgReg.u32CmdStkPtr,
        (int)pCh->sRT.sCmdStk.u32RtCmdStkDwdSz,
        (int)pCh->sRT.sCmdStk.u32RtCmdStkMsgSz,
        (int)pCh->sRT.sRtxCfgReg.u32IsqPtr,
        (int)pCh->sRT.sRtxCfgReg.u32GblDataStkPtr,
        (int)pCh->sRT.sRtxCfgReg.u32RTGConfig,
        (int)pCh->sRT.sRtxCfgReg.u32IntMask);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtRtCmdStkSync
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtRtCmdStkSync
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[u8Ch]->sRT);
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    U32BIT u32Data;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK, "mrtRtCmdStkSync: called (CH%d) STATE=%d\n", u8Ch, pRT->state);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    if (pRT->sCmdStk.bResyncInProgress == TRUE)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        return;
    }

    pRT->sCmdStk.bResyncInProgress = TRUE;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* temporarily disable RT */
    pRT->sRtxCfgReg.u32RTGConfig = pRT->sRtxCfgReg.u32RTGConfig & (~MRT_GCONFIG_MODULE_EN);
    DDC_REG_WRITE(pDeviceContext, (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    /* Align IMP cmd stack pointer with MRT's so that both the IMP and MRT are synchronized
       with the same cmd stack pointer */
    if (DDC_REG_READ(pDeviceContext, *(pRT->pu32RegBA) + REG_MRT_CMD_STK_PTR, &u32Data) == DDC_UDL_ERROR__SUCCESS)
    {
        DDC_REG_WRITE(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_STK_PTR), &u32Data);
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK,
        "\nCh%d: mrtRtCmdStkSync -- u32CmdStkPointer 0x%08x, BA 0x%08x, mask 0x%08x, Dwd Size 0x%08x, SW index 0x%08x\n\n",
        u8Ch,
        u32Data,
        pCh->sRT.sRtxCfgReg.u32CmdStkPtr,
        pCh->u32ChannelMemDwdMask,
        pCh->sRT.sCmdStk.u32RtCmdStkDwdSz,
        pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex);

    /* clear cmd stack counter */
    u32Data = 0;
    DDC_REG_WRITE(pDeviceContext, *(pRT->pu32RegBA) + REG_MRT_CMD_STK_ENTRIES, &u32Data);

    /* re-enable RT */
    pRT->sRtxCfgReg.u32RTGConfig = pRT->sRtxCfgReg.u32RTGConfig | MRT_GCONFIG_MODULE_EN;

    DDC_REG_WRITE(pDeviceContext, (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pRT->sCmdStk.bResyncInProgress = FALSE;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
}

/*
    TODO: add locking mechanism to make sure that we are not running
    mrtRtCmdStkSync while this occurs.  mrtRtCmdStkSync takes precendence over this
    code
 */

/******************************************************************************
 * Name:    mrtSetCmdStkPtrToLatest
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtSetCmdStkPtrToLatest
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    U32BIT u32CurrentCmdStkPtr = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK, "ENTER-> (CH%d) STATE=%d\n", u8Ch, pCh->sRT.state);

    /* read latest Cmd Stk Value */
    DDC_REG_READ(pDeviceContext, (*(pCh->sRT.pu32RegBA) + REG_MRT_CMD_STK_PTR), &u32CurrentCmdStkPtr);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK,
        "\nCh%d: mrtRtCmdStkSync -- u32CmdStkPointer 0x%p, BA 0x%08x, mask 0x%08x, Dwd Size 0x%08x, SW index 0x%08x\n\n",
        u8Ch,
        mrtSetCmdStkPtrToLatest,
        pCh->sRT.sRtxCfgReg.u32CmdStkPtr,
        pCh->u32ChannelMemDwdMask,
        pCh->sRT.sCmdStk.u32RtCmdStkDwdSz,
        pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex);

    impSetRtCmdStkPtr(pDeviceContext, pCh, u32CurrentCmdStkPtr);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtHbufImpPostQueue
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtHbufImpPostQueue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    U32BIT u32Data;
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[u8Ch]->sRT);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ENTER->(CH%d) STATE=%d\n", u8Ch, pRT->state);

    if (pRT->sHbuf.state != ACEX_MOD_OPEN)
    {
        return;
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    pRT->sCmdStk.u32RtCmdStkStatUpdateCounter++;

    if (pRT->sCmdStk.u32RtCmdStkStatUpdateCounter > RTX_CMDSTK_STAT_UPDATE_RATE) /* update RT Cmd Stk statistics */
    {
        pRT->sCmdStk.u32RtCmdStkStatUpdateCounter = 0;

        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        mrtUpdateRtStkStats(pDeviceContext, u8Ch);
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }

    /* check if the hBuf is full - we consider it full if
       there is not enough room for a default transfer size -
       which is currently 64 messages */
    if (pRT->sHbuf.u32HbufNumEntries > (pRT->sHbuf.u32StkMsgCapacity - pRT->sHbuf.u32MaxTfrCmdSize))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "Driver Mirror Hbuf full NumEntries:%d  avail:%d\n",
            (unsigned int)pRT->sHbuf.u32HbufNumEntries, (unsigned int)(pRT->sHbuf.u32StkMsgCapacity - MRT_CMD_STK_MSG_TFR_SZ));
        u32Data = 0;
    }
    else
    {
        u32Data = pRT->sHbuf.u32NumRequested;
    }

    /* only request a transfer if we are not full */
    if ((u32Data != 0) && (pRT->sHbuf.bImpCmdPending == FALSE) && (pRT->sHbuf.bDmaXferPending == FALSE))
    {
        /* if we are not full and have not posted a command, do it now */
        /*KdPrint(("mrtHbufImpPostQueue: Posting Cmd on Imp Blk\n"));  */
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "Ch %d Requesting %d msgs\n", u8Ch, (unsigned int)u32Data);
        pRT->sHbuf.bImpCmdPending = TRUE;

        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        /* set improvement block to read stack */
        impPostQueue(pDeviceContext, u8Ch,
            (IMP_MRT_CMD_MODE_CTRL_MASK | IMP_DONT_PACK_DATA | IMP_16_BIT_CMD_ENABLE) | RT_HBUF_IMP_ID | (u32Data * MRT_CMD_STK_BLK_BYTE_SZ / 2), u32Data);
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }

}

/******************************************************************************
 * Name:    mrtHbufCtrl
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT mrtHbufCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *pRtAccess,
    size_t OutputBufferLength,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[pRtAccess->sConfigID.u16Channel];

    U32BIT u32NextRd;
    U8BIT *pu8SrcBA = 0;        /* pointer to source BA */
    U8BIT *pu8Src;          /* pointer to source */
    U8BIT *pu8Dest;         /* pointer to destination */
    U32BIT u32NumMsgWords = 0;
    U32BIT u32NumMsgBytes = 0;
    U32BIT u32NumOfMsg = 0;

    U32BIT u32Register;
    U32BIT u32CmdBufByteSize;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ENTER->(Ch%d)\n", pRtAccess->sConfigID.u16Channel);

    *pBytesReturned = 0;

    /* look at data transfer first - priority 1 */
    if (pRtAccess->u32Data == ACEX_MRT_HBUF_GET)
    {
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        if (pCh->sRT.sHbuf.u32HbufNumEntries > 0)
        {
            u32NumOfMsg = (U32BIT)(OutputBufferLength / (ACE_MSGSIZE_RT * 2));
            if (u32NumOfMsg > pCh->sRT.sHbuf.u32HbufNumEntries)
            {
                /* read what we have in HBuf */
                u32NumOfMsg = pCh->sRT.sHbuf.u32HbufNumEntries;
            }
            *pBytesReturned = u32NumOfMsg * ACE_MSGSIZE_RT * 2;

            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_GET NumEntries:%d\n", pCh->sRT.sHbuf.u32HbufNumEntries);

            if (pCh->sRT.sHbuf.pu8hbufMemory)
            {
                U32BIT u32LenToEOB = 0;
                U32BIT u32LeftOver = 0;
                U32BIT u32Bufloc = 0;
                U32BIT u32TempBuf = 0;

                pu8Dest = (U8BIT *)pRdData;
                pu8SrcBA = (U8BIT *)pCh->sRT.sHbuf.pu8hbufMemory;

                while (u32NumOfMsg)
                {
                    u32NextRd = pCh->sRT.sHbuf.u32HbufNxtRd;
                    pu8Src = pCh->sRT.sHbuf.pu8hbufMemory + u32NextRd;

                    /* get msg length - for now only use 16 lsbs of 32bit value */
                    u32NumMsgBytes = *((U16BIT*)pu8Src);

                    if (u32NumMsgBytes & 0x1)
                    {
                        u32NumMsgBytes++;                              /* if odd number of words, add filler because we are not packing */
                    }

                    /* limit to ACE_MSGSIZE_RT */
                    if (u32NumMsgBytes > ACE_MSGSIZE_RT)
                    {
                        u32NumMsgBytes = ACE_MSGSIZE_RT;
                    }

                    /* wds to bytes & add 4 to include msglengt field size */
                    u32NumMsgBytes = (u32NumMsgBytes * 2) + 4;         /* wds to bytes & add 4 to include msglengtfieldsize */
                    u32Bufloc = u32NextRd + u32NumMsgBytes;

                    /* Is Data greater then buffer size? If so then wrap it. */
                    if (u32Bufloc >= pCh->sRT.sHbuf.u32HBufByteSize)
                    {
                        /* Find data length to end of buffer, also find overflow data if we have some */
                        u32LenToEOB = (pCh->sRT.sHbuf.u32HBufByteSize - u32NextRd) ;
                        u32LeftOver = u32NumMsgBytes - u32LenToEOB;

                        if (u32LenToEOB > 4)
                        {
                            /* copy data to end of buffer */
                            memcpy(pu8Dest, pu8Src + 4, u32LenToEOB - 4);   /* add 4 to bypass msglength field size */
                        }

                        /* if data left over copy to beginning of buffer */
                        if (u32LeftOver)
                        {

                            if (u32LenToEOB > 4)
                            {
                                /* Roll buffer over - use pu8Dest */
                                memcpy(pu8Dest + (u32LenToEOB - 4), pu8SrcBA, u32LeftOver);  /* add 4 to bypass msglength field size */
                            }
                            else
                            {
                                if (u32LenToEOB == 0)
                                {
                                    u32NumMsgWords = *((U16BIT*)pu8SrcBA);
                                    if (u32NumMsgWords & 0x1)
                                    {
                                        u32NumMsgWords++;
                                    }

                                    /* limit to ACE_MSGSIZE_RT */
                                    if (u32NumMsgWords > ACE_MSGSIZE_RT)
                                    {
                                        u32NumMsgWords = ACE_MSGSIZE_RT;
                                    }

                                    /* wds to bytes & add 4 to include msglengt field size */
                                    u32NumMsgBytes = (u32NumMsgWords * 2) + 4;

                                    memcpy(pu8Dest, pu8SrcBA + 4, (u32NumMsgBytes - 4));   /* add 4 to bypass msglength field size */  /* add 4 to bypass msglength field size */
                                    u32LeftOver = u32NumMsgBytes;
                                    /*KdPrint(("\n u32LeftOver %x \n", u32LeftOver)); */
                                }
                                else
                                {
                                    u32TempBuf = 4 - u32LenToEOB;
                                    /* Roll buffer over - use pu8Dest */
                                    memcpy(pu8Dest, (pu8SrcBA + u32TempBuf), u32LeftOver- u32TempBuf);  /* add 4 to bypass msglength field size */
                                }
                            }

                            pCh->sRT.sHbuf.u32HbufNxtRd = u32LeftOver;
                        }
                        else
                        {
                            pCh->sRT.sHbuf.u32HbufNxtRd = 0;
                        }
                    }
                    else
                    {
                        /* read one message a time */
                        memcpy(pu8Dest, pu8Src + 4, (u32NumMsgBytes - 4)); /* add 4 to bypass msglengthfieldsize */
                        pCh->sRT.sHbuf.u32HbufNxtRd += u32NumMsgBytes;
                    }

                    pCh->sRT.sHbuf.u32HbufNumEntries--;
                    pCh->sRT.sHbuf.u32CmdRdIndex++;

                    if (pCh->sRT.sHbuf.u32CmdRdIndex >= pCh->sRT.sHbuf.u32StkMsgCapacity)
                    {
                        pCh->sRT.sHbuf.u32CmdRdIndex = 0;
                        /*pCh->sRT.sHbuf.u32HbufNxtRd = 0;*/
                    }
                    u32NumOfMsg--;
                    pu8Dest += ACE_MSGSIZE_RT * 2;
                }
            }
            DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        }
        else
        {
            DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

            /* kick off a transfer from the RT Cmd Stk */
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_GET call impPostQueue to kickoff tfr\n");

            /* this work around method is due to simultaneous read/write issue for PCI boards in multi-core system, BZ */
            mrtHbufImpPostQueue(pDeviceContext, (U8BIT)(pRtAccess->sConfigID.u16Channel));
        }
    }
    else
    {
        /* HBUF control operation */
        u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_REG_INT_ENABLE_MASK;

        switch (pRtAccess->u32Data)
        {
            case ACEX_MRT_HBUF_ENABLE:
            {
                /* create resources to support hbuf mechanism */
                /* determine the size of the local CMD Stack buffer to create */
                /* get the CMD STK SIZE IDENTIFIER by reading the RT Global Config Reg Cmd Stk Bits 5-7 */
                u32CmdBufByteSize = (pCh->sRT.sRtxCfgReg.u32RTGConfig & MRT_GCONFIG_MASK_RT_CMD_STK_SZ) >> 5;

                /* calculate number of messages in stack

                   RtCmdStkSize (0-3) - we shift left by this amount with 64 as baseline to establish size
                   (64 msgs per blk)

                    RtCmdStkSize    Msgs
                    ------------    -----
                        0           64
                        1           128
                        2           256
                        3           512

                   NOTE: u32CmdBufByteSize temporarily holds the RtCmdStkSize, wo we can use it below to determine
                   StkMsgCapacity */
                pCh->sRT.sHbuf.u32StkMsgCapacity = 64;
                pCh->sRT.sHbuf.u32StkMsgCapacity = pCh->sRT.sHbuf.u32StkMsgCapacity << u32CmdBufByteSize;

                switch (pCh->sRT.sHbuf.u32StkMsgCapacity)
                {
                    case 64:
                    { pCh->sRT.sHbuf.u32MaxTfrCmdSize = 32; }
                                                                 break;
                    case 128:
                    { pCh->sRT.sHbuf.u32MaxTfrCmdSize = 64; }
                                                                 break;
                    case 256:
                    { pCh->sRT.sHbuf.u32MaxTfrCmdSize = 128; }
                                                                  break;
                    case 512:
                    { pCh->sRT.sHbuf.u32MaxTfrCmdSize = 128; }
                                                                  break;

                    default:
                    { pCh->sRT.sHbuf.u32MaxTfrCmdSize = 32; }
                                                                 break;
                }

                /* determine the number of bytes to allocate */
                u32CmdBufByteSize = pCh->sRT.sHbuf.u32StkMsgCapacity * MRT_CMD_STK_MSG_BYTE_SZ; /* num msgs times 80 bytes per message */
                pCh->sRT.sHbuf.pu8hbufMemory = DDC_KERNEL_MALLOC(pDeviceContext, u32CmdBufByteSize);

                if (!pCh->sRT.sHbuf.pu8hbufMemory)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_ENABLE Memory Allocation Failure\n");
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_ENABLE CmdStkBufByteSize:%d\n", u32CmdBufByteSize);

                pCh->sRT.sHbuf.u32HbufNumEntries = 0;
                pCh->sRT.sHbuf.u32HbufLstWr = 0;
                pCh->sRT.sHbuf.u32HbufNxtRd = 0;
                pCh->sRT.sHbuf.u32NumRequested = pCh->sRT.sHbuf.u32MaxTfrCmdSize;
                pCh->sRT.sHbuf.u32CmdIndex = 0;
                pCh->sRT.sHbuf.u32CmdRdIndex = 0;
                pCh->sRT.sHbuf.bImpCmdPending = FALSE;
                pCh->sRT.sHbuf.bDmaXferPending = FALSE;
                pCh->sRT.sHbuf.state = ACEX_MOD_OPEN;
                pCh->sRT.sHbuf.u32HBufByteSize = u32CmdBufByteSize;

                /* enable interrupts */
                mrtInterruptSet(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel,
                    (MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER +
                        MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER +
                        MRT_INT_ENABLE_MASK_CMD_STK_OVERFLOW +
                        MRT_INT_ENABLE_MASK_ILLEGAL_CMD +
                        MRT_INT_ENABLE_MASK_FORMAT_ERROR +
                        MRT_INT_ENABLE_MASK_XMTR_TIMEOUT +
                        MRT_INT_ENABLE_MASK_ADDR_PARITY));

                gen1553InterruptSet(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel, GENERAL_INT_STATUS_RT_INT_ENABLED);

                /* set global interrupt masks */
                DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32IntMask));

                DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_ENABLE-WRITE RT GLBL INTERRUPT: REG:0x%08x DATA:0x%08x\n",
                    u32Register, pCh->sRT.sRtxCfgReg.u32IntMask);
                break;
            }

            case ACEX_MRT_HBUF_DISABLE:
            {
                U32BIT u32TimeOutCount = 500; /* wait for 500 ms */

                DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_DISABLE\n");

                /* wait for any pending hbuf request to complete */
                while (u32TimeOutCount)
                {
                    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                    if ((pCh->sRT.sHbuf.bImpCmdPending == FALSE) && (pCh->sRT.sHbuf.bDmaXferPending == FALSE))
                    {
                        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                        break;
                    }
                    else
                    {
                        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                    }

                    ddcUdlOsWaitMsInterruptable(1);
                    u32TimeOutCount--;
                }

                /* disable interrupts */
                mrtInterruptClear(pDeviceContext,
                    (U8BIT)pRtAccess->sConfigID.u16Channel, (MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER + MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER));

                gen1553InterruptClear(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel, GENERAL_INT_STATUS_RT_INT_ENABLED);

                /* set global interrupt masks ? set or clear? by BZ*/
                DDC_REG_WRITE(pDeviceContext, u32Register, &(pCh->sRT.sRtxCfgReg.u32IntMask));

                DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_HBUF, "ACEX_MRT_HBUF_DISABLE-WRITE RT GLBL INTERRUPT: REG:0x%08x DATA:0x%08x\n",
                    u32Register, pCh->sRT.sRtxCfgReg.u32IntMask);

                /* Free and set object to NULL so improvemance interrupt does not try and read it if interrupt comes in late */
                DDC_KERNEL_FREE(pDeviceContext, pCh->sRT.sHbuf.pu8hbufMemory);
                pCh->sRT.sHbuf.pu8hbufMemory = NULL;

                pCh->sRT.sHbuf.state = ACEX_MOD_CLOSED;
                break;
            }
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    mrtUpdateRtStkStats
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtUpdateRtStkStats
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[u8Ch]->sRT);
    U32BIT u32OverflowEntries = pRT->sCmdStk.u32OverflowNumEntries;

    /* retrieve last num cmd stk entries */
    DDC_REG_READ(pDeviceContext, *(pRT->pu32RegBA) + REG_MRT_CMD_STK_ENTRIES, &(pRT->stats.u32LastNumCmdStkEntries));

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    if (pRT->stats.u32LastNumCmdStkEntries > pRT->stats.u32HighNumCmdStkEntries)
    {
        pRT->stats.u32HighNumCmdStkEntries = pRT->stats.u32LastNumCmdStkEntries;
    }

    if (u32OverflowEntries > pRT->stats.u32HighNumCmdStkEntries)
    {
        pRT->stats.u32HighNumCmdStkEntries = u32OverflowEntries;
    }
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK, "mrtUpdateRtStkStats: High#CmdStkEntries:%d CumLstCmdStk:%d  Lst#CmdStkEntries:%d\n",
        pRT->stats.u32HighNumCmdStkEntries, pRT->stats.u32NumCmdStkLost, pRT->stats.u32LastNumCmdStkEntries);
}

/******************************************************************************
 * Name:    mrtOpenBrdcst
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxCfg
 * Out  none
 *
 * Returns:
 *****************************************************************************/
ACEX_RT_HW_INFO *mrtOpenBrdcst
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_CONFIG_ID *psRtxCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxCfg->u16Channel];
    VRT_TYPE *pVrt;

    /* check if we are open already, if so return an error !!!!!!!!!!!!!!!!!!!! */

    pVrt = &(pCh->sRT.sVRT[RTX_BROADCAST_RT]);

    /* clear virtual RT structure */
    memset(pVrt, 0, sizeof(VRT_TYPE));

    pVrt->u32CfgMemBA = (pCh->sRT.u32MrtBA) + (RTX_VRT_MEMORY_OFFSET * RTX_BROADCAST_RT);

    /* END RT CONFIGURATION OFFSET */
    /* set RT table memory locations in hardware */
    pVrt->sVrtHwInfo.u32RtIllegalizationRxTableBA = pVrt->u32CfgMemBA + MEM_RT_ILL_TABLE_BA;        /* loc for each of 32 SA's */
    pVrt->sVrtHwInfo.u32RtIllegalizationTxTableBA = pVrt->u32CfgMemBA + MEM_RT_ILL_TABLE_BA + 32;   /* offset from 32 Rx SA's above*/
    pVrt->sVrtHwInfo.u32ImpRxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_IMP_RXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32ImpTxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_IMP_TXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32ModeCodeDataTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_DATA_BA;
    pVrt->sVrtHwInfo.u32ConfigurationBA = pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA;
    pVrt->sVrtHwInfo.u32SACtrlBA = pVrt->u32CfgMemBA + MEM_RT_SA_CTRL_BA;
    pVrt->sVrtHwInfo.u32RxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_RXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32TxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_TXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32BusyBitRxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_BB_RX_TABLE_BA;
    pVrt->sVrtHwInfo.u32BusyBitTxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_BB_TX_TABLE_BA;
    pVrt->sVrtHwInfo.u32StatusInCtrlTableBA = pVrt->u32CfgMemBA + MEM_RT_STATUS_INCTRL_TABLE_BA;
    pVrt->sVrtHwInfo.u32ModeCodeSelTxIntTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_SEL_TX_INT_BA;
    pVrt->sVrtHwInfo.u32ModeCodeSelRxIntTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_SEL_RX_INT_BA;
    pVrt->sVrtHwInfo.u32DbcHoldoffTimeBA = pVrt->u32CfgMemBA + MEM_RT_DBC_HOLDOFF_TIME_BA;
    pVrt->sVrtHwInfo.u32ImrTrigSelectBA = pVrt->u32CfgMemBA + MEM_RT_IMR_TRIG_SELECT_BA;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_CMD_STK,
        "IllegalizationRxTbl:%08x RxLkupTbl:%08x TxLkupTbl:%08x\n"
        "        BB Rx Tbl:%08x BB Tx Tbl:%08x StatusInCtrl:%08x\n",

        (int)pVrt->sVrtHwInfo.u32RtIllegalizationRxTableBA,
        (int)pVrt->sVrtHwInfo.u32RxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32TxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32BusyBitRxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32BusyBitTxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32StatusInCtrlTableBA);


   rtSetNoRespTimeOut(pDeviceContext, psRtxCfg->u16Channel, RTX_BROADCAST_RT, 37);

    return &(pVrt->sVrtHwInfo);
}

/* ========================================================================== */
/*                           RT Specific Functions                            */
/* ========================================================================== */

/******************************************************************************
 * Name:    rtInternalBITWdRd
 *
 * Description:
 *      Reads the Interal BIT Word for the specified RT.  The AceX procedure
 *      to accomplish this is as follows:
 *
 *      1 - Read MRT Register 2 (Broadcast Selected Control Register) and
 *          test if the Broadcast Selected bit was Set for this RT.
 *      2 - If Broadcast Selected bit was NOT set, read the Last Bit
 *          Word location (offset 0xB8) for this RT.
 *      3 - If Broadcast Selected bit was set, read the Last Bit
 *          Word location (offset 0xB8) at RT31 (broadcast RT).
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxBitWd
 * In   pu16Data
 * Out  none
 *
 * Returns: error condition
 *****************************************************************************/
S16BIT rtInternalBITWdRd
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtxBitWd,
    U16BIT *pu16Data
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxBitWd->sConfigID.u16Channel];
    U32BIT u32Tmp;
    U32BIT u32RtMask = 0x1 << psRtxBitWd->u16RtAddr;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    VRT_TYPE *pVrt;

    /* read Broadcast Selected Control register */
    status = DDC_REG_READ(pDeviceContext, *(pCh->pu32RegBA) + REG_MRT_BRDCST_SEL_CTRL, &u32Tmp);

    if (status)
    {
        return status;
    }

    pVrt = &(pCh->sRT.sVRT[psRtxBitWd->u16RtAddr]);

    /* determine if bit was set for this RT */
    if ((u32Tmp & u32RtMask)) /* bit was set, read from broadcast area (rt31) */
    {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "Broadcast read (RT 31)\n");
        status = DDC_MEM_READ(pDeviceContext, (pCh->sRT.sVRT[RTX_BROADCAST_RT].u32CfgMemBA + MEM_RT_MODE_LAST_BITWD_BA), &u32Tmp, ACEX_32_BIT_ACCESS);
    }
    else
    { /* bit was NOT set, read from this rt */
        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "RT %d read\n", psRtxBitWd->u16RtAddr);
        status = DDC_MEM_READ(pDeviceContext, (pVrt->u32CfgMemBA + MEM_RT_MODE_LAST_BITWD_BA), &u32Tmp, ACEX_32_BIT_ACCESS);
    }
    *pu16Data = (U16BIT)(u32Tmp & 0x0000ffff);

    return status;
}

/******************************************************************************
 * Name:    rtConfigSet
 *
 * Description:
 *      Sets Options specified in psRtxCfg u32Options field.  It will be
 *      bitwise or'd with any existing configuration options.
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxCfg
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
S16BIT rtConfigSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxCfg->sConfigID.u16Channel];
    VRT_TYPE *pVrt;
    VRT_TYPE *pVrtBcst;

    if (pCh->sRT.state != ACEX_MOD_OPEN)
    {
        /* we do not want to open again if we are open */
        return DDC_UDL_ERROR__STATE;
    }

    pVrt = &(pCh->sRT.sVRT[psRtxCfg->u16RtAddr]);

    if (psRtxCfg->u32Options & ACE_RT_OPT_CLR_SREQ)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_CLR_SERV_REQUEST;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_LOAD_TT)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_LD_TT_ON_MODE_SYNC;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_CLEAR_TT)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_CLR_TT_ON_MODE_SYNC;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_ALT_STS)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_ALT_STATUS_WD_EN;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_IL_RX_D)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_ILL_RX_TFR_DIS;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_BSY_RX_D)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_BUSY_RX_TFR_DIS;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_SET_RTFG)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_RT_FAIL_FLAG_WRAP_EN;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_1553A_MC)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_1553A_MC_EN;
        pCh->sRT.b1553a[psRtxCfg->u16RtAddr] = TRUE;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_MC_O_BSY)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_MODE_CMDS_OVERRIDE_BUSY;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_BCST_DIS)
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_BRDCST_DIS;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_ILL_DIS)              /* disable illegalization */
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_ILL_DIS;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_1553A_ERESP_EN)       /* 1553A Error Response enabled */
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_1553A_ERROR_RESP_EN;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_INHIBIT_BTWD_IF_BUSY) /* Inhibit BITWD if Busy */
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_INHIB_BITWD_IF_BUSY;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_EXTERN_BTWD_IF_BUSY)  /* Extern BITWD if Busy */
    {
        pVrt->u32Config = pVrt->u32Config | RT_CONFIG_EXT_BITWD_IF_BUSY;
    }

    DDC_MEM_WRITE(pDeviceContext, (pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA), &(pVrt->u32Config), ACEX_32_BIT_ACCESS);

    /* TT1478 - ACE_RT_OPT_LOAD_TT | ACE_RT_OPT_CLEAR_TT options do not work with broadcast mode codes */
    /* 'OR' together the options of the current RT with RT31 (broadcast) */
    pVrtBcst = &(pCh->sRT.sVRT[ACEX_BROADCAST_RT]);
    pVrtBcst->u32Config |= pVrt->u32Config;
    DDC_MEM_WRITE(pDeviceContext, (pVrtBcst->u32CfgMemBA + MEM_RT_CONFIG_BA), &(pVrtBcst->u32Config), 0);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
        "rtConfigSet: Write RT CFG: MEM:0x%08x  DATA:0x%08x\n",
        (int)(pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA), (int)pVrt->u32Config);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    rtConfigClr
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxCfg
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
S16BIT rtConfigClr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxCfg->sConfigID.u16Channel];
    VRT_TYPE *pVrt;
    VRT_TYPE *pVrtBcst;

    if (pCh->sRT.state != ACEX_MOD_OPEN) /* we do not want to open again if we are open */
    {
        return DDC_UDL_ERROR__STATE;
    }

    pVrt = &(pCh->sRT.sVRT[psRtxCfg->u16RtAddr]);

    if (psRtxCfg->u32Options & ACE_RT_OPT_CLR_SREQ)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_CLR_SERV_REQUEST);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_LOAD_TT)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_LD_TT_ON_MODE_SYNC);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_CLEAR_TT)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_CLR_TT_ON_MODE_SYNC);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_ALT_STS)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_ALT_STATUS_WD_EN);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_IL_RX_D)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_ILL_RX_TFR_DIS);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_BSY_RX_D)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_BUSY_RX_TFR_DIS);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_SET_RTFG)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_RT_FAIL_FLAG_WRAP_EN);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_1553A_MC)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_1553A_MC_EN);
        pCh->sRT.b1553a[psRtxCfg->u16RtAddr] = FALSE;
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_MC_O_BSY)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_MODE_CMDS_OVERRIDE_BUSY);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_BCST_DIS)
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_BRDCST_DIS);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_ILL_DIS)              /* disable illegalization */
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_ILL_DIS);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_1553A_ERESP_EN)       /* 1553A Error Response enabled */
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_1553A_ERROR_RESP_EN);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_INHIBIT_BTWD_IF_BUSY) /* Inhibit BITWD if Busy */
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_INHIB_BITWD_IF_BUSY);
    }

    if (psRtxCfg->u32Options & ACE_RT_OPT_EXTERN_BTWD_IF_BUSY)  /* Extern BITWD if Busy */
    {
        pVrt->u32Config = pVrt->u32Config & ~(RT_CONFIG_EXT_BITWD_IF_BUSY);
    }

    DDC_MEM_WRITE(pDeviceContext, (pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA), &(pVrt->u32Config), ACEX_32_BIT_ACCESS);

    /* TT1478 - ACE_RT_OPT_LOAD_TT | ACE_RT_OPT_CLEAR_TT options do not work with broadcast mode codes */
    /* clear the options from RT31 (broadcast) using the current RT options */
    pVrtBcst = &(pCh->sRT.sVRT[ACEX_BROADCAST_RT]);
    pVrtBcst->u32Config &= ~(pVrt->u32Config);
    DDC_MEM_WRITE(pDeviceContext, (pVrtBcst->u32CfgMemBA + MEM_RT_CONFIG_BA), &(pVrtBcst->u32Config), 0);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
        "rtConfigClr: Write RT CFG: MEM:0x%08x  DATA:0x%08x\n",
        (int)(pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA), (int)pVrt->u32Config);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    rtClose
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxCfg
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void rtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxCfg->sConfigID.u16Channel];
    U32BIT u32Data;
    VRT_TYPE *pVrt;

    pVrt = &(pCh->sRT.sVRT[psRtxCfg->u16RtAddr]);

    /* disable this RT - first update local copy of mask */
    u32Data = 0x1;
    u32Data = ~(u32Data << psRtxCfg->u16RtAddr);

    pCh->sRT.sRtxCfgReg.u32RTEnable = pCh->sRT.sRtxCfgReg.u32RTEnable & u32Data;

    /* write modified RTEnable settings out to the card */
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_RT_ENABLE), &(pCh->sRT.sRtxCfgReg.u32RTEnable));

    /* TODO: CLEAR OUT ILLEGALIZATION TABLE ON PHY DEVICE HERE & ANY OTHER
       PART OF HARDWARE MEMORY THAT NEEDS TO BE CLEARED FOR THIS RT */

    pVrt->state = ACEX_MOD_CLOSED;
    memset(pVrt, 0, sizeof(VRT_TYPE));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "END-> STATE:%d\n", pVrt->state);
}

/******************************************************************************
 * Name:    rtLatchCtrl
 *
 * Description:
 *      This function sets the RT Latch Control bit, then writes the
 *      combined RT Address & RT Address Parity bits to the Global
 *      RT Config register.  This control is only applicable in SRT
 *      compatibility mode.
 *
 * TODO FOR PCI: we may want to not allow latch control if TRACKING is set
 *               on bit 21 of global config.
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtAccess
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void rtLatchCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtAccess->sConfigID.u16Channel];
    U32BIT u32Data;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "ENTER->\n");

    /* set latch of RT Address */
    pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig | MRT_GCONFIG_LATCH_SRT_ADDRESS;

    /* write modified GConfig settings out to the card */
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG), &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    /* set RT Address w/parity - parity/rtaddr bits begin at bit 22 of the RT Global Config register*/
    u32Data = psRtAccess->u16RtAddr;

    pCh->sRT.sRtxCfgReg.u32RTGConfig =
        pCh->sRT.sRtxCfgReg.u32RTGConfig
        | MRT_GCONFIG_LATCH_SRT_ADDRESS | ((u32Data << 22) & ((MRT_GCONFIG_SRT_ADDRESS) |(MRT_GCONFIG_SRT_ADDRESS_PARITY)));

    /* write modified GConfig settings out to the card */
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG), &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    /* remove latch bit in global config */
    pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig & ~(MRT_GCONFIG_LATCH_SRT_ADDRESS);

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG), &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    DDC_REG_READ(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG), &u32Data);
    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "Read RT Global Config: 0x%08x\n", u32Data);
}

/******************************************************************************
 * Name:    rtSetNoRespTimeOut
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u16Channel
 * In   u16RtAddr
 * In   u32NRTO
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
S16BIT rtSetNoRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U16BIT u16RtAddr,
    U32BIT u32NRTO
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    U32BIT u32Data = u32NRTO << RT_CONFIG_NO_RESP_TIMEOUT_SHIFT;
    VRT_TYPE *pVrt;

    if (pCh->sRT.state != ACEX_MOD_OPEN)
    {
        /* we do not want to open again if we are open */
        return DDC_UDL_ERROR__STATE;
    }

    pVrt = &(pCh->sRT.sVRT[u16RtAddr]);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL, "RT:%d NoRespTimeOut:%d\n", u16RtAddr, (int)u32NRTO);

    DDC_MEM_WRITE(pDeviceContext, (pVrt->u32CfgMemBA + MEM_RT_NO_RESP_TIMEOUT), &(u32Data), ACEX_32_BIT_ACCESS);

    return DDC_UDL_ERROR__SUCCESS;
}

/******************************************************************************
 * Name:    rtSourceCtrl
 *
 * Description:
 *      This function sets the RT Source Control bit.
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtAccess
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
S16BIT rtSourceCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtAccess->sConfigID.u16Channel];
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    if (psRtAccess->u32Data == ACE_RT_EXTERNAL_ADDR)
    {
        /* EXTERNAL */
        /* this code is for PCI devices only */
        pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig & ~(MRT_GCONFIG_MRT_MODE_EN);
        pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig & ~(MRT_GCONFIG_SRT_ADDRESS_SOURCE);
    }
    else
    {
        /* INTERNAL */
        pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig | MRT_GCONFIG_SRT_ADDRESS_SOURCE;
    }

    /* write modified GConfig settings out to the card */
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG), &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    return status;
}

/******************************************************************************
 * Name:    rtStart
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtAccess
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void rtStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
)
{
    U32BIT u32Register;
    U32BIT u32Data = 0;

    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtAccess->sConfigID.u16Channel];

    /* set RT Global configuration */
    pCh->sRT.sRtxCfgReg.u32RTGConfig |= MRT_GCONFIG_MODULE_EN;

    /* sync software CmdStkPtr Index */
    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_CMD_STK_PTR;
    DDC_REG_READ(pDeviceContext, u32Register, &u32Data);
    pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex = 0;

    if (u32Data)
    {
        pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex = u32Data - (pCh->sRT.sRtxCfgReg.u32CmdStkPtr & pCh->u32ChannelMemDwdMask);
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
        "\nCh%d: rtStart -- u32CmdStkPointer 0x%08x, BA 0x%08x, mask 0x%08x, Dwd Size 0x%08x, SW index 0x%08x\n\n",
        psRtAccess->sConfigID.u16Channel,
        u32Data, pCh->sRT.sRtxCfgReg.u32CmdStkPtr, pCh->u32ChannelMemDwdMask,
        pCh->sRT.sCmdStk.u32RtCmdStkDwdSz, pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex);

    /* set MRT GCONFIG register */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_QPRM)
    {
        if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_RT_AUTO_BOOT)
        {
            if (pCh->bRtAutoBoot)
            {
                /* if RT_AUTO_BOOT is on, set RT Configured bit (bit 17) to clear Busy bit */
                pCh->sRT.sRtxCfgReg.u32RTGConfig |= MRT_GCONFIG_MASK_RT_CONFIGURED;

                DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
                    "ch%d clear BUSY bit 0x%08x \n",
                    psRtAccess->sConfigID.u16Channel,
                    pCh->sRT.sRtxCfgReg.u32RTGConfig);
            }
        }
    }

    DDC_REG_WRITE(pDeviceContext, (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));

    /* enable this RT - first update local copy of RT Enable mask */
    pCh->sRT.sRtxCfgReg.u32RTEnable |= psRtAccess->u32Data;

    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_RT_ENABLE), &(pCh->sRT.sRtxCfgReg.u32RTEnable));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
        "Write RT ENABLE: REG:0x%08x  DATA:0x%08x\n",
        (int)((*(pCh->sRT.pu32RegBA)) + REG_MRT_RT_ENABLE),
        (int)pCh->sRT.sRtxCfgReg.u32RTEnable);

}

/******************************************************************************
 * Name:    rtStop
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtAccess
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void rtStop
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtAccess->sConfigID.u16Channel];

    /* disable this RT - first update local copy of RT Enable mask */

    pCh->sRT.sRtxCfgReg.u32RTEnable = pCh->sRT.sRtxCfgReg.u32RTEnable & (~(psRtAccess->u32Data));

    /* write it out to the card */
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sRT.pu32RegBA)) + REG_MRT_RT_ENABLE), &(pCh->sRT.sRtxCfgReg.u32RTEnable));

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_CONFIG_CTRL,
        "Write RT ENABLE: REG:0x%08x  DATA:0x%08x\n",
        (int)((*(pCh->sRT.pu32RegBA)) + REG_MRT_RT_ENABLE),
        (int)pCh->sRT.sRtxCfgReg.u32RTEnable);

    if (ACEX_SRT_MODE == pCh->sRT.bMode)
    {
        pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig & (~MRT_GCONFIG_MODULE_EN);

        DDC_REG_WRITE(pDeviceContext, (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));
    }
    else
    {
        /* In MRT mode, must verify no other RTs are enabled prior to disabling Global RT mode */
        if (!(pCh->sRT.sRtxCfgReg.u32RTEnable))
        {
            pCh->sRT.sRtxCfgReg.u32RTGConfig = pCh->sRT.sRtxCfgReg.u32RTGConfig & (~MRT_GCONFIG_MODULE_EN);

            DDC_REG_WRITE(pDeviceContext, (*(pCh->sRT.pu32RegBA)) + REG_MRT_GCONFIG, &(pCh->sRT.sRtxCfgReg.u32RTGConfig));
        }
    }
}

/******************************************************************************
 * Name:    rtOpen
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   psRtxCfg
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
ACEX_RT_HW_INFO *rtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[psRtxCfg->sConfigID.u16Channel];
    VRT_TYPE *pVrt;
    U32BIT u32Register, u32Data;
    U8BIT u8RT;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_OPEN, "ENTER->\n");

    /* check if we are open already if so return an error !!!!!!!!!!!!!!!!!!!! */

    pVrt = &(pCh->sRT.sVRT[psRtxCfg->u16RtAddr]);

    /* clear virtual RT structure */
    memset(pVrt, 0, sizeof(VRT_TYPE));

    pVrt->u32CfgMemBA = (pCh->sRT.u32MrtBA) + (RTX_VRT_MEMORY_OFFSET * psRtxCfg->u16RtAddr);

    /* clear memory slot in hardware for this RT */
    gen1553MemClear(pDeviceContext, RTX_VRT_MEMORY_OFFSET, pVrt->u32CfgMemBA);

    /* Preset the 'last command' and 'last status' RT address bits for the case when the associated
       'T' mode codes are issued prior to any other messages that would normally update these locations */
    u8RT = (U8BIT)(psRtxCfg->u16RtAddr);
    u32Register = pCh->sRT.u32MrtBA + ((u8RT << 8) + MEM_RT_MC_LAST_CMD_STS);
    u32Data = (u8RT << 11) | (u8RT << 27);
    DDC_MEM_WRITE(pDeviceContext, u32Register, &u32Data, 0);

    /* RT CONFIGURATION OFFSET */
    /* Determine requested configuration options and load here */

    /* FORCE ILLEGALIZATION FOR NOW - THIS WILL PROBABLY CHANGE AS WE DEVELOP RT
       CONFIG FURTHER

       The following configuration bits are always set (as per flexcore):
             Double Buffer Enable
             Busy Lookup Table Enable
     */
    pVrt->u32Config = RT_CONFIG_RX_DB_EN | RT_CONFIG_BUSY_LKUP_TBL_EN;

    /* configure optional fields and write configuration to hw mem field */
    rtConfigSet(pDeviceContext, psRtxCfg);

    rtSetNoRespTimeOut(pDeviceContext, psRtxCfg->sConfigID.u16Channel, psRtxCfg->u16RtAddr, psRtxCfg->u16RespTimeOut);

    /* END RT CONFIGURATION OFFSET */

    /* set RT table memory locations in hardware */
    pVrt->sVrtHwInfo.u32RtIllegalizationRxTableBA = pVrt->u32CfgMemBA + MEM_RT_ILL_TABLE_BA;        /* loc for each of 32 SA's */
    pVrt->sVrtHwInfo.u32RtIllegalizationTxTableBA = pVrt->u32CfgMemBA + MEM_RT_ILL_TABLE_BA + 32;   /* offset from 32 Rx SA's above*/
    pVrt->sVrtHwInfo.u32ImpRxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_IMP_RXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32ImpTxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_IMP_TXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32ModeCodeDataTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_DATA_BA;
    pVrt->sVrtHwInfo.u32ConfigurationBA = pVrt->u32CfgMemBA + MEM_RT_CONFIG_BA;
    pVrt->sVrtHwInfo.u32SACtrlBA = pVrt->u32CfgMemBA + MEM_RT_SA_CTRL_BA;
    pVrt->sVrtHwInfo.u32RxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_RXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32TxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_TXDP_TABLE_BA;
    pVrt->sVrtHwInfo.u32BusyBitRxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_BB_RX_TABLE_BA;
    pVrt->sVrtHwInfo.u32BusyBitTxLkupTableBA = pVrt->u32CfgMemBA + MEM_RT_BB_TX_TABLE_BA;
    pVrt->sVrtHwInfo.u32StatusInCtrlTableBA = pVrt->u32CfgMemBA + MEM_RT_STATUS_INCTRL_TABLE_BA;
    pVrt->sVrtHwInfo.u32ModeCodeSelTxIntTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_SEL_TX_INT_BA;
    pVrt->sVrtHwInfo.u32ModeCodeSelRxIntTableBA = pVrt->u32CfgMemBA + MEM_RT_MC_SEL_RX_INT_BA;
    pVrt->sVrtHwInfo.u32DbcHoldoffTimeBA = pVrt->u32CfgMemBA + MEM_RT_DBC_HOLDOFF_TIME_BA;
    pVrt->sVrtHwInfo.u32ImrTrigSelectBA = pVrt->u32CfgMemBA + MEM_RT_IMR_TRIG_SELECT_BA;
    pVrt->sVrtHwInfo.u32ImrModeCodeBA = pVrt->u32CfgMemBA + MEM_RT_IMR_MODE_CODE_BA;

    pVrt->state = ACEX_MOD_OPEN;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_RT_OPEN,
        "END-> STATE:%d\n"
        "IllegalizationRxTbl:%08x RxLkupTbl:%08x TxLkupTbl:%08x\n"
        "        BB Rx Tbl:%08x BB Tx Tbl:%08x StatusInCtrl:%08x\n",
        pVrt->state,
        (int)pVrt->sVrtHwInfo.u32RtIllegalizationRxTableBA,
        (int)pVrt->sVrtHwInfo.u32RxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32TxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32BusyBitRxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32BusyBitTxLkupTableBA,
        (int)pVrt->sVrtHwInfo.u32StatusInCtrlTableBA);

    return &(pVrt->sVrtHwInfo);
}

/******************************************************************************
 * Name:    mrtImpReadCmd
 *
 * Description:
 *      This function is called to process IOCTL_IMP_READ/IOCTL_IMP_MRT_CMD.
 *      It triggers IMP to get data from CMD/Data Stack to IMP memory and then
 *      DMA-transfers from IMP memory to pRdData.
 *
 * In   pDeviceContext      input value for instance information associated with this particular device
 * In   u16Channel
 * In   u32NumCmd           Number of Cmd requested
 * In   u32OutputBufferLen  The length of the buffer in app
 * Out  pBytesReturned      the length of data got from device
 * Out  pRdData             pointer to a temporary buffer to store data from IMP memory
 *
 * Returns: none
 *****************************************************************************/
S16BIT mrtImpReadCmd
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U32BIT u32NumCmd,
    U32BIT u32OutputBufferLen,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    U32BIT u32MrtCmdCount = 0;
    int nResult;

    struct _ACEX_1553_RT_TYPE *pRT = NULL;

    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadCmd ENTER\n", u16Channel);

    pRT = &(pDeviceContext->pChannel1553[u16Channel]->sRT);

    /* return bytes count */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    *pBytesReturned = 0;
    pRdData[0] = (U32BIT)(*pBytesReturned);

    if ( pDeviceContext->u8RtDmaBusy == TRUE)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        return status;
    }
    pDeviceContext->u8RtDmaBusy = TRUE;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* read the number of entries in the RT command stack */
    DDC_REG_READ(pDeviceContext, *(pRT->pu32RegBA) + REG_MRT_CMD_STK_ENTRIES, &u32MrtCmdCount);

    /* exit if there is no command or nothing in the RT command stack */
    if ((u32NumCmd == 0) || (u32MrtCmdCount == 0))
    {
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        pDeviceContext->u8RtDmaBusy = FALSE;
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        return status;
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* read cmd from IMP module via DMA */
    pRT->sCmdStk.u32RtCmdStkStatUpdateCounter++;

    if (pRT->sCmdStk.u32RtCmdStkStatUpdateCounter > (RTX_CMDSTK_STAT_UPDATE_RATE / 2))
    {
        pRT->sCmdStk.u32RtCmdStkStatUpdateCounter = 0;
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        mrtUpdateRtStkStats(pDeviceContext, (U8BIT)u16Channel);
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }

    pRT->pu8Buf = (U8BIT *)pRdData;

    if (pRT->u16EventCond)
    {
        pRT->u16EventCond--;
    }

    if (0 == pRT->u16EventCond)
    {
        pRT->u32RdByte = 0;
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadCmd requesting %d cmds\n", u16Channel, u32NumCmd);
        impPostQueue(   pDeviceContext,
                        u16Channel,
                        (IMP_MRT_CMD_MODE_CTRL_MASK | IMP_DONT_PACK_DATA | IMP_16_BIT_CMD_ENABLE | IMP_FLUSH_ENABLE) + RT_DATA_TFR_IMP_ID + (u32OutputBufferLen / 2),
                        u32NumCmd);

        nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(pRT->waitqueueEvent,
                                                    pRT->u16EventCond,
                                                    500);

        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        if (!nResult)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadCmd timed out\n", u16Channel);
        }
    }

    *pBytesReturned = pRT->u32RdByte;

    pDeviceContext->u8RtDmaBusy = FALSE;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadCmd returned %d bytes\n", u16Channel, (unsigned int)(*pBytesReturned));
    return status;
}

/******************************************************************************
 * Name:    mrtImpReadData
 *
 * Description:
 *      This function is called to process IOCTL_IMP_READ/IOCTL_IMP_MRT_DATA.
 *      It triggers IMP to get data from Data Stack to IMP memory and then
 *      DMA-transfers from IMP memory to pRdData.
 *
 * In   pDeviceContext      input value for instance information associated with this particular device
 * In   u16Channel
 * In   u16RtAddr           RT address
 * In   u16RtSA             RT subaddress
 * In   u32OutputBufferLen  The length of the buffer in app
 * Out  pBytesReturned      the length of data got from device
 * Out  pRdData             pointer to a temporary buffer to store data from IMP memory
 *
 * Returns: none
 *****************************************************************************/
S16BIT mrtImpReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U16BIT u16RtAddr,
    U16BIT u16RtSA,
    U32BIT u32OutputBufferLen,
    size_t *pBytesReturned,
    U32BIT *pRdData
)
{
    int nResult = 0;

    struct _ACEX_1553_RT_TYPE *pRT = NULL;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadData ENTER\n", u16Channel);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pRT = &(pDeviceContext->pChannel1553[u16Channel]->sRT);

    pRT->sCmdStk.u32RtCmdStkStatUpdateCounter++;

    if (pRT->sCmdStk.u32RtCmdStkStatUpdateCounter > (RTX_CMDSTK_STAT_UPDATE_RATE / 2))
    {
        pRT->sCmdStk.u32RtCmdStkStatUpdateCounter = 0;
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        mrtUpdateRtStkStats(pDeviceContext, (U8BIT)u16Channel);
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }

    pRT->pu8Buf = (U8BIT *)pRdData;
    if (pRT->u16EventCond)
    {
        pRT->u16EventCond--;
    }

    if (0 == pRT->u16EventCond)
    {
        pRT->u32RdByte = 0;

        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        impPostQueue(   pDeviceContext,
                        u16Channel,
                        (IMP_MRT_DATA_MODE_CTRL_MASK | IMP_DONT_PACK_DATA | IMP_16_BIT_CMD_DISABLE | IMP_FLUSH_DISABLE) + RT_DATA_TFR_IMP_ID + (u32OutputBufferLen / 2),
                        (U32BIT)((u16RtAddr << 5) | u16RtSA));

        nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(pRT->waitqueueEvent,
                                                    pRT->u16EventCond,
                                                    5000);
        DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        if (!nResult)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadData timed out\n", u16Channel);
        }
    }

    *pBytesReturned = pRT->u32RdByte;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_IMP, "Ch %d: mrtImpReadData returned %d bytes\n", u16Channel, (unsigned int)(*pBytesReturned));
    return status;
}

/******************************************************************************
 * Name:    mrtStreamCtrl
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pRtAccess
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtStreamCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *pRtAccess
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[pRtAccess->sConfigID.u16Channel]->sRT);

    if (pRtAccess->u32Data == ACEX_MRT_STREAMING_ENABLE_TX)
    {
        /* todo : add state change ???? or setup feature enable bit */
        mrtInterruptSet(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel, MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM);

        pRT->eStreamState[MRT_STREAM_DBLK_TX] = MRT_STREAM_RESET;

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_STREAMING, "mrtStreamCtrl: ENABLE\n");
    }
    else if (pRtAccess->u32Data == ACEX_MRT_STREAMING_ENABLE_RX)
    {
        /* todo : add state change ???? or setup feature enable bit */
        mrtInterruptSet(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel, MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM);

        pRT->eStreamState[MRT_STREAM_DBLK_RX] = MRT_STREAM_RESET;

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_STREAMING, "mrtStreamCtrl: ENABLE\n");
    }
    else
    { /* assume streaming disable */
      /* todo : add state change ???? or setup feature enable bit */

        mrtInterruptClear(pDeviceContext, (U8BIT)pRtAccess->sConfigID.u16Channel, MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM);

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_STREAMING, "mrtStreamCtrl: DISABLE\n");
    }
}

/******************************************************************************
 * Name:    rtSendStream
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pRTStream
 * In   pData
 * In   pNumBytes
 * Out  none
 *
 * Returns: TRUE when data transfer completed
 *****************************************************************************/
BOOLEAN rtSendStream
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MRT_STREAM_TYPE *pRTStream,
    void *pData,
    U32BIT *pNumBytes
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[pRTStream->u16Chnl]->sRT);

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_STREAMING,
        "rtSendStream called: Ch:%d STATE:%d\n", pRTStream->u16Chnl, pRT->eStreamState[MRT_STREAM_DBLK_TX]);

    pRTStream->u16Direction = MRT_STREAM_DBLK_TX;

    *pNumBytes = 0;

	ddcUdlOsBoardDelayMs(0);
    if ((pRT->eStreamState[MRT_STREAM_DBLK_TX] == MRT_STREAM_RESET) || (pRTStream->sDirection[MRT_STREAM_DBLK_TX].s32Timeout == 0))
    {
        /* copy  data into remaining buffers */
        DDC_16BIT_BLK_MEM_WRITE(pDeviceContext, pRTStream->sDirection[MRT_STREAM_DBLK_TX].u32DwdBufPtr << 1, /* shift to convert to 16-bit address */
            pData, pRTStream->sDirection[MRT_STREAM_DBLK_TX].u32UsrBufByteSize >> 1 /* # 16-bit wds */);

        if (pRTStream->sDirection[MRT_STREAM_DBLK_TX].s32Timeout == 0) /* dont wait */
        {
            *pNumBytes = pRTStream->sDirection[MRT_STREAM_DBLK_TX].u32UsrBufByteSize;

            return TRUE;
        }

        /* set to pending because we will wait for an event */
        pRT->eStreamState[MRT_STREAM_DBLK_TX] = MRT_STREAM_PENDING;

        return FALSE;
    }
    else if (pRT->eStreamState[MRT_STREAM_DBLK_TX] == MRT_STREAM_COMPLETE)
    {
        /* transaction completed */
        pRT->eStreamState[MRT_STREAM_DBLK_TX] = MRT_STREAM_RESET;

        *pNumBytes = pRTStream->sDirection[MRT_STREAM_DBLK_TX].u32UsrBufByteSize;

        return TRUE;
    }

    return FALSE;
}

/******************************************************************************
 * Name:    rtReceiveStream
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pRTStream
 * In   pNumBytes
 * Out  none
 *
 * Returns: TRUE when data transfer completed
 *****************************************************************************/
BOOLEAN rtReceiveStream
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MRT_STREAM_TYPE *pRTStream,
    U32BIT *pNumBytes
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[pRTStream->u16Chnl]->sRT);
    void *pBuf;

    DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_STREAMING,
        "rtReceiveStream called: Ch:%d STATE:%d\n", pRTStream->u16Chnl, (int)pRT->eStreamState[MRT_STREAM_DBLK_RX]);

    *pNumBytes = 0;

    pRTStream->u16Direction = MRT_STREAM_DBLK_RX;


	ddcUdlOsBoardDelayMs(0);
    if ((pRT->eStreamState[MRT_STREAM_DBLK_RX] >= MRT_STREAM_COMPLETE) || (pRTStream->sDirection[MRT_STREAM_DBLK_RX].s32Timeout == 0)) /* if we received an interrupt event, or if timeout indicated don't wait, return data */
    {
        pRT->eStreamState[MRT_STREAM_DBLK_RX] = MRT_STREAM_RESET;

        /* data is available so copy it here */
        pBuf = pRTStream->pRequest;

        DDC_16BIT_BLK_MEM_READ( pDeviceContext,
            pRTStream->sDirection[MRT_STREAM_DBLK_RX].u32DwdBufPtr << 1,                     /* shift to convert to 16bit addr */
            (U16BIT *)pBuf,
            pRTStream->sDirection[MRT_STREAM_DBLK_RX].u32UsrBufByteSize >> 1 /* bytes to words*/);

        *pNumBytes = pRTStream->sDirection[MRT_STREAM_DBLK_RX].u32UsrBufByteSize;

        return TRUE;
    }
    else if (pRT->eStreamState[MRT_STREAM_DBLK_RX] == MRT_STREAM_RESET)
    {
        pRT->eStreamState[MRT_STREAM_DBLK_RX] = MRT_STREAM_PENDING;
    }

    return FALSE;
}

/******************************************************************************
 * Name:    mrtDataArrayCtrl
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   pRtDA
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void mrtDataArrayCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_DATA_ARRAY *pRtDA
)
{
    struct _ACEX_1553_RT_TYPE *pRT = &(pDeviceContext->pChannel1553[pRtDA->sRtAccess.sConfigID.u16Channel]->sRT);
    ACEX_RT_DATA_ARRAY *pDA;

    if (pRtDA->sRtAccess.u32Data == ACEX_MRT_DATA_ARRAY_ENABLE)
    {
        pDA = &(pRT->sDataArrayTable[(pRtDA->sRtAccess.u16RtAddr * ACEX_MRT_MAX_DATA_ARRAYS) + pRtDA->u16ID]);

        if (pDA->eState == DA_OFF)
        {
            pDA = &(pRT->sDataArrayTable[(pRtDA->sRtAccess.u16RtAddr * ACEX_MRT_MAX_DATA_ARRAYS) + pRtDA->u16ID]);
            memcpy(pDA, pRtDA, sizeof(ACEX_RT_DATA_ARRAY));

            pDA->eState = DA_ON;

            pRT->bDataArrayEnabled = TRUE;

            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_ARRAY, "mrtDataArrayCtrl: ENABLE ID:%d\n", pRtDA->u16ID);
        }
        else
        {
            pDA->eState = DA_ON;

            DDC_MEM_WRITE(pDeviceContext, pDA->u32TxPtrOffsetAddressMRT, &(pDA->u32TxTfrStartOffset), ACEX_32_BIT_ACCESS);

            DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_ARRAY, "mrtDataArrayCtrl: ARM ONE SHOT ID:%d\n", pRtDA->u16ID);
        }
    }

    if (pRtDA->sRtAccess.u32Data == ACEX_MRT_DATA_ARRAY_ENABLE_CONTINUOUS)
    {
        /* todo : add state change ???? or setup feature enable bit */
        mrtInterruptSet(pDeviceContext, (U8BIT)pRtDA->sRtAccess.sConfigID.u16Channel, MRT_INT_ENABLE_MASK_CBUF_ROLLOVER);

        pDA = &(pRT->sDataArrayTable[(pRtDA->sRtAccess.u16RtAddr * ACEX_MRT_MAX_DATA_ARRAYS) + pRtDA->u16ID]);
        memcpy(pDA, pRtDA, sizeof(ACEX_RT_DATA_ARRAY));

        pDA->eState = DA_CONTINOUS;

        pRT->bDataArrayEnabled = TRUE;

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_ARRAY, "mrtDataArrayCtrl: CONTINUOUS ENABLE ID:%d\n", pRtDA->u16ID);
    }
    else if (pRtDA->sRtAccess.u32Data == ACEX_MRT_DATA_ARRAY_DISABLE)
    {
        pDA = &(pRT->sDataArrayTable[(pRtDA->sRtAccess.u16RtAddr * ACEX_MRT_MAX_DATA_ARRAYS) + pRtDA->u16ID]);
        pDA->eState = DA_OFF;

        memset(pDA, 0, sizeof(ACEX_RT_DATA_ARRAY));

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_ARRAY, "mrtDataArrayCtrl: DISABLE ID:%d\n", pRtDA->u16ID);
    }
    else
    {
        /* assume data array disable */

        /* todo : add state change ???? or setup feature enable bit */

        pRT->bDataArrayEnabled = FALSE;

        mrtInterruptClear(pDeviceContext, (U8BIT)pRtDA->sRtAccess.sConfigID.u16Channel, MRT_INT_ENABLE_MASK_CBUF_ROLLOVER);

        DDC_DBG_PRINT(DDC_DBG_MODULE_RT, DDC_DBG_RT_DATA_ARRAY, "mrtDataArrayCtrl: DISABLE INTERRUPT\n");
    }
}

/******************************************************************************
 * Name:    mrtReadModeCodeData
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u16Channel
 * In   u8RtAddr
 * In   wModeCode
 * Out  U16BIT mode code data
 *
 * Returns: TRUE when data transfer completed
 *****************************************************************************/
U16BIT mrtReadModeCodeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U8BIT u8RtAddr,
    U16BIT wModeCode
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    VRT_TYPE *pVrt;
    U16BIT ReadData = 0;
    U16BIT MCDAddrOffset = wModeCode;

    if (wModeCode & ACE_RT_MCDATA_BCST_CODE_BITMASK)
    {
        pVrt = &(pCh->sRT.sVRT[ACEX_BROADCAST_RT]);

#if DDC_PPC
        MCDAddrOffset = (U16BIT)(wModeCode & ~(ACE_RT_MCDATA_BCST_CODE_BITMASK));
        MCDAddrOffset = (U16BIT)((MCDAddrOffset - (MCDAddrOffset & 0x1)) + !(MCDAddrOffset & 0x1));
#else
        MCDAddrOffset = (U16BIT)(wModeCode & ~(ACE_RT_MCDATA_BCST_CODE_BITMASK));
#endif
    }
    else
    {
        pVrt = &(pCh->sRT.sVRT[u8RtAddr]);

#if DDC_PPC
        MCDAddrOffset = wModeCode;
        MCDAddrOffset = (U16BIT)((MCDAddrOffset - (MCDAddrOffset & 0x1)) + !(MCDAddrOffset & 0x1));
#else
        MCDAddrOffset = wModeCode;
#endif
    }

    DDC_16BIT_BLK_MEM_READ(pDeviceContext, (pVrt->u32CfgMemBA << 1) + (MEM_RT_MC_DATA_BA << 1) + MCDAddrOffset, &ReadData, 1);

    return ReadData;
}

/******************************************************************************
 * Name:    mrtWriteModeCodeData
 *
 * Description:
 *
 * In   pDeviceContext  input value for instance information associated with this particular device
 * In   u16Channel
 * In   u8RtAddr
 * In   wModeCode
 * In   wModeCodeData
 * Out  none
 *
 * Returns: TRUE when data transfer completed
 *****************************************************************************/
void mrtWriteModeCodeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U8BIT u8RtAddr,
    U16BIT wModeCode,
    U16BIT wModeCodeData
)
{
    ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Channel];
    VRT_TYPE *pVrt;

    U16BIT WriteData = wModeCodeData;
    U16BIT MCDAddrOffset = wModeCode;

    if (wModeCode & ACE_RT_MCDATA_BCST_CODE_BITMASK)
    {
        pVrt = &(pCh->sRT.sVRT[ACEX_BROADCAST_RT]);

#if DDC_PPC
        MCDAddrOffset = (U16BIT)(wModeCode & ~(ACE_RT_MCDATA_BCST_CODE_BITMASK));
        MCDAddrOffset = (U16BIT)((MCDAddrOffset - (MCDAddrOffset & 0x1)) + !(MCDAddrOffset & 0x1));
#else
        MCDAddrOffset = (U16BIT)(wModeCode & ~(ACE_RT_MCDATA_BCST_CODE_BITMASK));
#endif
    }
    else
    {
        pVrt = &(pCh->sRT.sVRT[u8RtAddr]);

#if DDC_PPC
        MCDAddrOffset = wModeCode;
        MCDAddrOffset = (U16BIT)((MCDAddrOffset - (MCDAddrOffset & 0x1)) + !(MCDAddrOffset & 0x1));
#else
        MCDAddrOffset = wModeCode;
#endif
    }

    DDC_16BIT_BLK_MEM_WRITE(pDeviceContext, (pVrt->u32CfgMemBA << 1) + (MEM_RT_MC_DATA_BA << 1) + MCDAddrOffset, &WriteData, 1);
}
