/*******************************************************************************
 * FILE: ddc_udl_1553_imp.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the 1553 improvements block.
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
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_imp_private.h"


/* ========================================================================== */
/*                          IMP RT MODULE FUNCTIONS                           */
/* ========================================================================== */

/*******************************************************************************
 * Name:    impRtInterruptHandler
 *
 * Description:
 *      Improvements block MRT interrupt handler.  The handler first reads
 *      improvement block status to determine action to take.  If multiple
 *      entries are on the output fifo, they will be serviced here.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void impRtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    struct _ACEX_1553_RT_TYPE *pRT = NULL;

    U32BIT u32Desc[3]; /* holds imp blk reg reads */
    BOOLEAN bFirstTime = FALSE;
    U32BIT u32NumEntries = 0;
    U32BIT j = 0;

    U32BIT u32StartAddr = 0;
    U32BIT u32NumWds = 0;
#if (!DDC_DMA_RT)
    U32BIT u32NumDWds = 0;
#endif /* (!DDC_DMA_RT) */
    U32BIT u32NumCmds = 0;
    U32BIT u32HostID = 0;
    U32BIT u32Data = 0;
#if (DDC_DMA_RT)
    U32BIT u32AbsolutStartAddr = 0;
#endif

    U8BIT *pBuf = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "ENTER->Channel:%d\n", u8Ch);

    /* retrieve output fifo status and first entry from fifo (if there is one) */
    DDC_BLK_REG_READ(pDeviceContext, *(pCh->sImpRT.pu32RegBA) + ACEX_1553_IMP_REG_OUT_FIFO_STATUS, u32Desc, 3);

    /* determine the number of entries on the queue, so use number of dwds field
       in status register - shift 2 bit postions because this field begins at bit
       position 2 - divide two because 2 dwds per entry */
    u32NumEntries = (u32Desc[0] & IMP_OUTFIFO_STATUS_NUM_ENTRIES);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "**** OUTFIFO STATUS ****  NumEntries: %d \n", u32NumEntries);

    if (u32Desc[0] & IMP_OUTFIFO_STATUS_FULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, " OFIFO FULL\n");

        pCh->sImpRT.stats.u32NumOutFifoFull++;
    }

    /* read entries off output fifo */
    for (j = 0; j < u32NumEntries; j++)
    {
        /* we performed a block read upfront with the first fifo entry to           */
        /* improve usb bus efficiency, so we do not have to read from the output    */
        /* fifo the first time thru this loop                                       */
        if (bFirstTime == FALSE)
        {
            bFirstTime = TRUE;
        }
        else
        {
            DDC_BLK_REG_READ(pDeviceContext, *(pCh->sImpRT.pu32RegBA) + ACEX_1553_IMP_REG_OUTPUT_DATA_Q_LO, &(u32Desc[1]), 2);
        }

        u32StartAddr = (u32Desc[1] & ACEX_1553_IMP_OUTFIFO_DQLO_ADDR_START);
        u32NumWds = (u32Desc[2] & ACEX_1553_IMP_OUTFIFO_DQHI_NUM_WDS);
#if (!DDC_DMA_RT)
        u32NumDWds = u32NumWds >> 1;      /* odd word will be truncated */
#endif /* (!DDC_DMA_RT) */
        u32NumCmds = ((u32Desc[1] & ACEX_1553_IMP_OUTFIFO_DQLO_NUM_CMDS) >> 20);
        u32HostID = ((u32Desc[2] & ACEX_1553_IMP_OUTFIFO_DQHI_HOST_ID) >> 8);

        if (u32NumCmds > 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "Channel:%d **** OUT DQ LO/HI **** MemAddr:%08X   NumCmds:%d / NumWds:%d\n", \
                u8Ch, u32StartAddr, u32NumCmds, u32NumWds);
        }

        pRT = &(pDeviceContext->pChannel1553[u8Ch]->sRT);
        switch (u32HostID)
        {
            case RT_HBUF_IMP_ID:
            {
#if (DDC_DMA_RT)            
                DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                if (pRT->sHbuf.pu8hbufMemory == NULL)
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                    break;
                }   
                
                /* get pointer to the starting address for copying of msgs */
                pBuf = pRT->sHbuf.pu8hbufMemory;
                         
                pBuf = pBuf + pRT->sHbuf.u32HbufLstWr;
                pRT->sHbuf.u32HbufLstWr = pRT->sHbuf.u32HbufLstWr + (u32NumWds * 2); /* move to DMA complete function if DMA */
                pRT->sHbuf.u32CmdIndex += u32NumCmds; /* adjust our index */

                if (pRT->sHbuf.u32CmdIndex >= pRT->sHbuf.u32StkMsgCapacity)
                {
                    pRT->sHbuf.u32CmdIndex = 0;
                }                
                
                pRT->sHbuf.bImpCmdPending = FALSE;
                /* get the data here */
                if (u32NumCmds > 0)
                {
                    /* read data from improvement memory into driver Hbuf */

                    pRT->sHbuf.bDmaXferPending = TRUE;
                    u32AbsolutStartAddr = (u32StartAddr << 1) + (*(pCh->pu32MemBA) << 2); /* words(16bit) & 32bit to bytes      */

                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                    dmaImpRtHbufSetup(pDeviceContext, u8Ch, u32NumWds, u32AbsolutStartAddr, (U8BIT*) pDeviceContext->pu8RtDmaTarget, u32NumCmds, pBuf);

                    /* <-- DMA */
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                }
                
                break;
            }
                           
#else  /* Not DMA */
            
                DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "Ch%d: RT_HBUF_IMP_ID\n", u8Ch);

                /* MRT Internal HBUF TRANSFER CMD WAS PENDING - WE PROCESS IT HERE */
                if (pRT->sHbuf.pu8hbufMemory == NULL)
                {
                    break;
                }

                /* get pointer to the starting address for copying of msgs */
                pBuf = pRT->sHbuf.pu8hbufMemory;

                DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                pBuf = pBuf + pRT->sHbuf.u32HbufLstWr;
                pRT->sHbuf.u32HbufLstWr = pRT->sHbuf.u32HbufLstWr + (u32NumWds * 2); /* move to DMA complete function if DMA */

                pRT->sHbuf.u32NumRequested = pRT->sHbuf.u32NumRequested - u32NumCmds;

                /* move to DMA complete function if DMA */
                pRT->sHbuf.u32CmdIndex += u32NumCmds; /* adjust our index */

                if (pRT->sHbuf.u32CmdIndex >= pRT->sHbuf.u32StkMsgCapacity)
                {
                    pRT->sHbuf.u32HbufLstWr = 0; /* back to the beginning */
                    pRT->sHbuf.u32CmdIndex = 0;
                }

                /*  adjust number of messages to request for next time - we do this so that we
                   guarantee we never overflow our buffer */

                if ((pRT->sHbuf.u32NumRequested > 0) && (pRT->sHbuf.u32NumRequested < pRT->sHbuf.u32MaxTfrCmdSize))
                {
                    /* do nothing */
                }
                else
                {
                    pRT->sHbuf.u32NumRequested = pRT->sHbuf.u32MaxTfrCmdSize; /* reset to default message size */
                }

                pRT->sHbuf.bImpCmdPending = FALSE;
                DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "Channel:%d ImpCmd Complete\n", u8Ch);

                if (u32NumCmds > 0)
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                    /* KdPrint((" **** OUT DQ LO ****\nMemAddr:%08X ",u32StartAddr));
                       KdPrint(("NumCmds:%d\n",u32NumCmds));
                       KdPrint((" **** OUT DQ HI ****\nNumWds:%d ",u32NumWds));
                       KdPrint(("STARTADDR:%08x ",(u32StartAddr>>1)));*/

                    DDC_BLK_MEM_READ(pDeviceContext, (u32StartAddr >> 1) + *(pCh->pu32MemBA), (U32BIT *)pBuf, u32NumDWds, ACEX_32_BIT_ACCESS);

                    /* if an odd number of words, we need an extra read because the passed buffer will
                       not accomodate the filler word */
                    if (u32NumWds % 2)
                    {
                        DDC_MEM_READ(pDeviceContext, (u32StartAddr >> 1) + u32NumDWds + *(pCh->pu32MemBA), &u32Data, ACEX_32_BIT_ACCESS);

                        /* KdPrint(("\n\n Extra byte: %04x\n",u32Tmp));  */
                        *((pBuf + ((u32NumWds - 1) * 2))) = (U8BIT)(u32Data & 0x00ff);
                        *((pBuf + ((u32NumWds - 1) * 2)) + 1) = (U8BIT)((u32Data >> 8) & 0x00ff);
                    }

                    /*KdPrint(( "NumCmds:%d ADDR:%08x \n%02x %02x %02x %02x\n",u32NumCmds,pBuf,*pBuf,*(pBuf+1),*(pBuf+2),*(pBuf+3)));
                       KdPrint(( "%02x %02x %02x %02x\n",*(pBuf+4),*(pBuf+5),*(pBuf+6),*(pBuf+7)));
                       KdPrint(( "%02x %02x %02x %02x\n",*(pBuf+8),*(pBuf+9),*(pBuf+10),*(pBuf+11)));
                       KdPrint(( "%02x %02x %02x %02x\n",*(pBuf+12),*(pBuf+13),*(pBuf+14),*(pBuf+15)));*/
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                }

              /* update number of available entries once transfer is completed */
                DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                pRT->sHbuf.u32HbufNumEntries += u32NumCmds;
                DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                /*
                    KdPrint((    "HBuf\n"
                                  "------------------------------------\n"
                                  "Command Index :%08d\n"
                                  " Last Written :%08d\n"
                                  "  Num Entries :%08d\n"
                                  "    Next Read :%08d\n"
                                  "Num Requested :%08d\n"
                                  "     Capacity :%08d\n"
                                  "------------------------------------\n",
                                  pRT->sHbuf.u32CmdIndex,
                                  pRT->sHbuf.u32HbufLstWr,
                                  pRT->sHbuf.u32HbufNumEntries,
                                  pRT->sHbuf.u32HbufNxtRd,
                                  pRT->sHbuf.u32NumRequested,
                                  pRT->sHbuf.u32StkMsgCapacity
                                  ));
                     pBuf = (U8*)WdfMemoryGetBuffer( pRT->sHbuf.hbufMemory,NULL);
                    for (i=0; i<pDeviceContext->pChannel1553[u8Ch]->sRT.sHbuf.u32HbufLstWr;i++)
                    {
                      KdPrint(("%02X\n", *(pBuf+i)));
                    }
                 */

                if ((u32NumCmds >= pRT->sHbuf.u32MaxTfrCmdSize) /*|| (u32NumCmds==0)*/)
                {
                    mrtHbufImpPostQueue(pDeviceContext, u8Ch);
                }

                break;
            }
#endif   /* Not DMA */

            /* IOCTL TRANSFER REQUEST */
            case RT_DATA_TFR_IMP_ID:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "Ch%d: RT_DATA_TFR_IMP_ID\n", u8Ch);

                /* READ MESSAGE FROM MEMORY - WE ARE READING TO A TEMPORARY LOCATION FOR NOW */
                /* MESSAGE PROCESSING SECTION */

                if (u32NumCmds > 0)                                     /* read data from improvement memory into driver temporary buffer */
                {
                    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                    pBuf = pRT->pu8Buf;                                /* the destination buffer pointer */
#if DDC_DMA_RT

                    u32AbsolutStartAddr =
                        (u32StartAddr << 1) + (*(pCh->pu32MemBA) << 2); /* the source address in IMP MEM, words(16bit) & 32bit to bytes*/

                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                    dmaImpRtCmdDataSetup(pDeviceContext, u8Ch, u32NumWds, u32AbsolutStartAddr, (U8BIT*) pDeviceContext->pu8RtDmaTarget, pBuf);

#else
                    DDC_BLK_MEM_READ(pDeviceContext, (u32StartAddr >> 1) + *(pCh->pu32MemBA), (U32BIT *)pBuf, u32NumDWds, ACEX_32_BIT_ACCESS);

                    /* if an odd number of words, we need an extra read because the passed buffer will
                       not accomodate the filler word */
                    if (u32NumWds % 2)
                    {
                        DDC_MEM_READ(pDeviceContext, (u32StartAddr >> 1) + u32NumDWds + *(pCh->pu32MemBA), &u32Data, ACEX_32_BIT_ACCESS);

                        /* KdPrint(("\n\n Extra byte: %04x\n",u32Tmp));  */
                        *((pBuf + ((u32NumWds - 1) * 2))) = (U8BIT)(u32Data & 0x00ff);
                        *((pBuf + ((u32NumWds - 1) * 2)) + 1) = (U8BIT)((u32Data >> 8) & 0x00ff);
                    }

                    /* the number of words transferred */
                    pRT->u32RdByte = u32NumWds << 1;

                    /* wake up the waiting process */
                    pRT->u16EventCond++;

                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                    DDC_WAKE_UP_INTERRUPTIBLE(&pRT->waitqueueCallback, &pRT->waitqueueEvent);
#endif /* DDC_DMA_RT */
                }
                else
                {
                    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                    pRT->u32RdByte = 0;

                    /* wake up the waiting process */
                    pRT->u16EventCond++;

                    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                    DDC_WAKE_UP_INTERRUPTIBLE(&pRT->waitqueueCallback, &pRT->waitqueueEvent);
                }

                break;
            }

            default:
            {
                /* clear the IMP pending flag */
                DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
                pRT->sHbuf.bImpCmdPending = FALSE;
                DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

                DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "Invalid HostId %08x\n", u32HostID);
                break;
            }
        }
    }

    /* re-arm improvement blk trigger */
    u32Data = ACEX_1553_IMP_BLK_MASK_BLK_TRIGGER;
    DDC_REG_WRITE( pDeviceContext, ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER,
        "WRITE IMP TRIG REG:%08x VALUE:%08x (IMP TRIGGER)\n"
         "Exit impInterruptHandler\n",
        ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG), u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_INT_HANDLER, "END->\n");
}

/*******************************************************************************
 * Name:    impInterruptSet
 *
 * Description:
 *      Sets the improvement interrupt
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * In   u32Data         interrupt data
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void impInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32Data
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    U32BIT u32Register;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INT_SET, "ENTER->\n");

    /* configure interrupt mask - default */
    u32Data = ACEX_1553_IMP_INT_EN_MASK_HOST_INITIATE;
    u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK);

    DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);

    pCh->sImpRT.sImpCfgReg.u32IntMaskEn = u32Data; /* store mask locally */
}

/*******************************************************************************
 * Name:    impPostQueue
 *
 * Description:
 *      Improvements block specific generic post function.  This will format and
 *      post a request for the improvements block to process.
 *
 * In   pDeviceContext  device-specific structure
 * In   u16Ch           channel
 * In   u32InFifoHi     InDataQue HI setting
 * In   u32InFifoLo     InDataQue LO setting
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void impPostQueue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32InFifoHi,
    U32BIT u32InFifoLo
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];
    U32BIT u32Data;
    U32BIT u32Desc[3]; /* IMP WORK QUEUE DESCRIPTOR: 0 = Q_LO, 1 = Q_HI, 2 = BLK TRIGGER */

    /* set interrupt enable mask to int on queue transfer parameter */
    u32Data = ACEX_1553_IMP_INT_EN_MASK_NUM_QUEUE_TFRS;
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK), &u32Data);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pCh->sImpRT.sImpCfgReg.u32IntMaskEn = u32Data;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_POST_QUEUE,
        "WRITE IMP REG:%08x VALUE:%08x (IMP INT MASK)\n", ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INT_EN_MASK), u32Data);

    /* INPUT DATA QUEUE OCCUPIES ADJACENT LOCATIONS, SO PERFORM BLOCK WRITE OF DESCRIPTOR */
    u32Desc[0] = u32InFifoLo;                            /* InDataQue LO set for 1 MRT CMD */
    u32Desc[1] = u32InFifoHi;                            /* InDataQue HI setting */
    u32Desc[2] = ACEX_1553_IMP_BLK_MASK_BLK_TRIGGER; /* trigger improvments block */

    /* Q LO is first register, so set as starting point */

    /*DDC_BLK_REG_WRITE(pDeviceContext,((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INPUT_DATA_Q_LO),u32Desc,3);*/
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INPUT_DATA_Q_LO), u32Desc);
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INPUT_DATA_Q_HI), u32Desc + 1);
    DDC_REG_WRITE(pDeviceContext, ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_IMP_BLK_TRIG), u32Desc + 2);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_POST_QUEUE, "WRITE IMP INPUT Q: LO:%08x HI:%08x Trigger:%08x\n", u32Desc[0], u32Desc[1], u32Desc[2]);
}

/*******************************************************************************
 * Name:    impInitialize
 *
 * Description:
 *      initialize device Improvements Block.  This includes clearing
 *      the imp data structure, issuing a reset to the imp block, and allocating
 *      a fixed 128 KBytes for imp block usage.
 *
 *      This should only need to be called when the device is first added to
 *      the system.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT impInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    U32BIT u32Data;
    U32BIT u32Register;

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE, "ENTER->\n");

    if (u8Ch >= pDeviceContext->u8Num1553Channels)
    {
        return DDC_UDL_ERROR__CHANNEL;
    }

    /* Initialize BC and MRT members to zero */
    memset(&(pCh->sImpBC.stats), 0, sizeof(ACEX_1553_IMP_STATS_TYPE));
    memset(&(pCh->sImpRT.stats), 0, sizeof(ACEX_1553_IMP_STATS_TYPE));

    pCh->sImpBC.state = ACEX_MOD_CLOSED;
    pCh->sImpRT.state = ACEX_MOD_CLOSED;

    /* Reset BC and MRT improvements.
       Both MRT and BC can be reset simultaneously by setting a single
       appropriate bit in Board Reset Register 1 */
    u32Data = (BD_RESET_1553_CH0_IMP_BCMRT << (u8Ch * 4));
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, u32Data);
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
        "WRITE BD RESET REG:%d VALUE:0x%08x (RESET IMPROVEMENTS)\n", REG_BD_RESET_COMPONENT_SF, u32Data);

    /* always reserve memory for ImpBc */
    u32Data = (pCh->u32UserMemBA - *(pCh->pu32MemBA)) * 2;     /* calculate offset: *2 to convert from 32 to 16-bit Addressing Offset */
    u32Register = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_BA);
    DDC_REG_WRITE( pDeviceContext, u32Register, &u32Data);

    pCh->sImpBC.sImpCfgReg.u32TgtMemBA = u32Data;                  /* store BA locally */
    DDC_DBG_PRINT(DDC_DBG_IMP_INITIALIZE, DDC_DBG_IMP_INITIALIZE,
        "WRITE ImpBc REG:%08x VALUE:%08x (IMP MEM BaseOffset)\n", u32Register, u32Data);

    /* set smaller IMP memory size if it is a Q-prime based device */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_QPRM)
    {
        pCh->u32ImpMemSizeDwds = ACEX_SF_1553_IMP_MEMSIZE_DWD_QPRM;
    }
    else
    {
        pCh->u32ImpMemSizeDwds = ACEX_SF_1553_IMP_MEMSIZE_DWD;
    }

    /* adjust memory available to user memory size and base address*/
    pCh->u32UserMemSizeBytes -= pCh->u32ImpMemSizeDwds * 4;
    pCh->u32UserMemBA += pCh->u32ImpMemSizeDwds;        /* DW addressing */

    /* Reserving memory now*/
    u32Data = pCh->u32ImpMemSizeDwds * 2;
    u32Register = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_SIZE);
    DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);

    pCh->sImpBC.sImpCfgReg.u32TgtMemSzDWD = pCh->u32ImpMemSizeDwds;             /* store size locally */
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
        "WRITE ImpBc REG:%08x VALUE:%08x (IMP MEM SIZE)\n", u32Register, u32Data);

    /* check if BC and MRT have separate IMP modules */
    if ((pCh->sImpBC.u32ComponentType != UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP) &&
        (pCh->sImpBC.u32ComponentType != UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP))
    {
        /* reserve memory for ImpRT on top of ImpBc */
        u32Data = (pCh->u32UserMemBA - *(pCh->pu32MemBA)) * 2;       /* calculate offset: *2 to convert from 32 to 16-bit Addressing Offset */
        u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_BA);
        DDC_REG_WRITE( pDeviceContext, u32Register, &u32Data);

        pCh->sImpRT.sImpCfgReg.u32TgtMemBA = u32Data;               /* store BA locally */
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
            "WRITE ImpRT REG:%08x VALUE:%08x (IMP MEM BaseOffset)\n", u32Register, u32Data);

        /* adjust memory available to user memory size and base address*/
        pCh->u32UserMemSizeBytes -= pCh->u32ImpMemSizeDwds * 4;
        pCh->u32UserMemBA += pCh->u32ImpMemSizeDwds;               /* DW addressing */

        /* Reserving memory now */
        u32Data = pCh->u32ImpMemSizeDwds * 2;
        u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_SIZE);
        DDC_REG_WRITE( pDeviceContext, u32Register, &u32Data);

        pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD = pCh->u32ImpMemSizeDwds;            /* store size locally */
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
            "WRITE ImpRT REG:%08x VALUE:%08x (IMP MEM SIZE)\n", u32Register, u32Data);
    }
    else
    {
        /* ImpRT shares the memoty with ImpBc */
        u32Register = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_BA);
        pCh->sImpRT.sImpCfgReg.u32TgtMemBA = pCh->sImpBC.sImpCfgReg.u32TgtMemBA;                /* store BA locally */
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
            "WRITE ImpRT REG:%08x VALUE:%08x (IMP MEM BaseOffset)\n", u32Register, u32Data);

        u32Register = ((*(pCh->sImpBC.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_SIZE);
        pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD = pCh->sImpBC.sImpCfgReg.u32TgtMemSzDWD;           /* store size locally */
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_INITIALIZE,
            "WRITE ImpRT REG:%08x VALUE:%08x (IMP MEM SIZE)\n", u32Register, u32Data);
    }

    pCh->sImpBC.state = ACEX_MOD_RESET;
    pCh->sImpRT.state = ACEX_MOD_RESET;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    impRtOpen
 *
 * Description:
 *      Open the improvement component.  Currently, open will
 *      configure the improvements block as follows:
 *
 *      1-Interrupt on Every completed command (a command is posted onto the
 *         input fifo
 *      2-Wait until command completes to interrupt
 *
 *      The board component master interrupt settings required
 *      for the improvements block are configured in a local variable.  This
 *      var will be used in the interrupt handler to rearm the improvments
 *      interrupt.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
S16BIT impRtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    U32BIT u32Data;
    U32BIT u32Register;

    if (pCh->sImpRT.state != ACEX_MOD_RESET)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* initialize members to zero */
    memset(&(pCh->sImpRT.stats), 0, sizeof(ACEX_1553_IMP_STATS_TYPE));

    /* reset improvements */
    if ((pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP) ||
        (pCh->sImpBC.u32ComponentType == UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP))
    {
        /* reset shared IMP module */
        u32Data = (BD_RESET_1553_CH0_IMP_BCMRT << (u8Ch * 4));
        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_SF, u32Data);
    }
    else
    {
        /* reset MRT IMP module */
        u32Data = (BD_RESET_1553_CH0_IMP_MRT << (u8Ch * 2));
        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_COMPONENT_MF, u32Data);
    }

    /* adjust memory available to user to exclude improvements memory area */
    u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_BA);
    DDC_REG_WRITE( pDeviceContext, u32Register, &(pCh->sImpRT.sImpCfgReg.u32TgtMemBA));
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_OPEN,
        "WRITE ImpRT REG (TARGET MEM BA):%08x VALUE:%08x\n", (int)u32Register, (int)pCh->sImpRT.sImpCfgReg.u32TgtMemBA);

    u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_TARGET_MEM_SIZE);
    u32Data = pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD * 2;
    DDC_REG_WRITE( pDeviceContext, u32Register, &u32Data);
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_OPEN,
        "WRITE ImpRT REG (TARGET MEM SIZE):%08x VALUE:%08x\n", (int)u32Register, (int)pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD);

    /* Set the Int Num Que Transfers parameter - every cmd completion for now */
    u32Data = 1; /* interrupt on every queue transfer */
    u32Register = ((*(pCh->sImpRT.pu32RegBA)) + ACEX_1553_IMP_REG_INT_NUM_QUEUE_TFR);
    DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);
    pCh->sImpRT.sImpCfgReg.u32IntNumQTfr = u32Data; /* store size locally */
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_OPEN,
        "WRITE ImpRT REG (IMP NUM QUE TFRS):%08x VALUE:%08x\n", (int)u32Register, (int)u32Data);

    /* set IMP behavior*/
    u32Data = IMP_IGNORE_TXRX_IMP_INPUT_CTRL;
    DDC_REG_WRITE(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + ACEX_1553_IMP_REG_IGNORE_TXRX_DATA), &u32Data);

    pCh->sImpRT.sImpCfgReg.u32IgnoreDataTfr = u32Data;
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_OPEN,
        "WRITE ImpRT REG:%08x VALUE:%08x \n",
        (unsigned int)(*(pCh->sImpRT.pu32RegBA) + ACEX_1553_IMP_REG_IGNORE_TXRX_DATA), (unsigned int)u32Data);

    u32Data = RTX_HBUF_IMPCMD_TIMEOUT;
    DDC_REG_WRITE(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_MSG_TIMEOUT), &u32Data);

    pCh->sImpRT.sImpCfgReg.u32IntMsgTime = u32Data;

    /* set start of cmd stk pointer */
    u32Register = (*(pCh->sRT.pu32RegBA)) + REG_MRT_CMD_STK_PTR;
    u32Data = pCh->sRT.sRtxCfgReg.u32CmdStkPtr & pCh->u32ChannelMemDwdMask;
    DDC_REG_WRITE(pDeviceContext, u32Register, &u32Data);
    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_OPEN, "\nCh%d: impRtOpen -- u32CmdStkPointer 0x%08x, BA 0x%08x, mask 0x%08x, Dwd Size 0x%08x, SW index 0x%08x\n\n",
        u8Ch,
        u32Data, pCh->sRT.sRtxCfgReg.u32CmdStkPtr, pCh->u32ChannelMemDwdMask,
        pCh->sRT.sCmdStk.u32RtCmdStkDwdSz, pCh->sRT.sRtxCfgReg.u32CmdStkPtrSwIndex);

    /* Set Bd Interrupt Bits to Enable Improvements Ints */
    pCh->sImpRT.u32BdIntMask = (BD_INT_STATUS_MASK_1553_0 << u8Ch) | BD_INT_STATUS_MASK_INT_REQ;

    /* set 1553 channel specific interrupt mask to Enable Improvements Ints */
    pCh->sImpRT.u321553ChIntMask = GENERAL_INT_MASK_IMP_INT_ENABLED | GENERAL_INT_MASK_MRT_IMP_INT_ENABLED;

    /* enable IMP master interrupt mask in the 1553 General Component */
    gen1553InterruptSet(pDeviceContext, u8Ch, pCh->sImpRT.u321553ChIntMask);

    pCh->sImpRT.state = ACEX_MOD_OPEN;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    impRtClose
 *
 * Description:
 *      Closes the RT improvements handler
 *      interface information.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            channel
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void impRtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];

    /* disable improvement interrupts */

    /* disable IMP master interrupt mask in the 1553 General Component */
    gen1553InterruptClear(pDeviceContext, u8Ch, pCh->sImpRT.u321553ChIntMask);

    pCh->sImpRT.state = ACEX_MOD_RESET;

    /* clear configuration settings in reverse order from impOpen */
}

/*******************************************************************************
 * Name:    impSyncRtCmdStkPtr
 *
 * Description:
 *      This function is used to write the improvements Rt Cmd Stack Pointer with
 *      the same address in the Mrt Rt Cmd Stk Pointer register.  This is called
 *      when the Rt Cmd Stk is reset.
 *
 * In   pDeviceContext  device-specific structure
 * In   pCh             channel
 * In   pu32RtCmdStkPtr RT command stack pointer
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void impSyncRtCmdStkPtr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _ACEX_1553_CHANNEL_TYPE *pCh,
    U32BIT u32RtCmdStkPtr
)
{
    /* load latest stk pointer into imp block */
    DDC_REG_WRITE(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_STK_PTR), &(u32RtCmdStkPtr));
}

/*******************************************************************************
 * Name:    impSetRtCmdStkPtr
 *
 * Description:
 *      Sets the RT command stack pointer
 *
 * In   pDeviceContext          device-specific structure
 * In   pCh                     channel
 * In   u32CurrentCmdStkPtr     RT command stack pointer
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
void impSetRtCmdStkPtr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _ACEX_1553_CHANNEL_TYPE *pCh,
    U32BIT u32CurrentCmdStkPtr
)
{
    U32BIT u32Data = 0;

    /* read current imp stk pointer value */
    DDC_REG_READ(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_STK_PTR), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_CMD_STK_POINTER,
        "CMDSTKPTR:%08X IMPPTR:%08X\n", (int)u32CurrentCmdStkPtr, (int)u32Data);

    /* if the pointers are the same, don't do anything */
    if (u32Data != u32CurrentCmdStkPtr) /* if imp stk pointer is not the same, check if we rolled over */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_CMD_STK_POINTER, "POINTERS NOT THE SAME\n");

        if (u32CurrentCmdStkPtr == (pCh->sRT.sRtxCfgReg.u32CmdStkPtr & pCh->u32ChannelMemDwdMask))
        {
            /* we are at the beginning of the stack, so the last entry is at the end */
            u32Data = ((pCh->sRT.sRtxCfgReg.u32CmdStkPtr & pCh->u32ChannelMemDwdMask) +
                pCh->sRT.sCmdStk.u32RtCmdStkDwdSz - RT_CMD_SIZE);
        }
        else
        {
            /* point to previous entry */
            u32Data = (u32CurrentCmdStkPtr - 4);
        }

        /* load latest stk pointer into imp block */
        DDC_REG_WRITE(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_STK_PTR), &(u32Data));
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_CMD_STK_POINTER, "POINTERS SAME\n");
    }

    DDC_REG_READ(pDeviceContext, (*(pCh->sImpRT.pu32RegBA) + REG_IMP_RT_CMD_STK_PTR), &u32Data);

    DDC_DBG_PRINT(DDC_DBG_MODULE_IMP, DDC_DBG_IMP_RT_CMD_STK_POINTER,
        "CMDSTKPTR:%08X IMPPTR:%08X\n", (int)u32CurrentCmdStkPtr, (int)u32Data);
}
