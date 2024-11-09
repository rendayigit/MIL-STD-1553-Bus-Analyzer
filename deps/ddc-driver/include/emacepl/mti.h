/*******************************************************************************
 * FILE: mti.h
 *
 * DESCRIPTION:
 *
 *  Monitor Terminal improvements public include file.
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

#ifndef __MTI_H__
#define __MTI_H__

/* ========================================================================== */
/*                              MACRO DEFINITIONS                             */
/* ========================================================================== */
/* Time Packet Chanel Specif Data Signal Source Indication */
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_INT      0x00000000L
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_EXT      0x00000001L
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_RMM      0x00000001L /* Time derived from clk in RMM)*/
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_NONE     0x0000000FL

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct API_MTI_FUNCTIONS
{
    S16BIT (_DECL *aceMTIConfigure)
    (
        S16BIT s16DevNum,
        U32BIT u32DevBufByteSize,
        U32BIT u32NumBufBlks,
        U32BIT u32BufBlkByteSize,
        BOOLEAN fZeroCopyEnable,
        U32BIT u32IrqDataLen,
        U32BIT u32IrqMsgCnt,
        U16BIT u16IrqTimeInterval,
        U32BIT u32IntConditions,
        U16BIT u16Ch10ChnlId,
        U8BIT u8HdrVer,
        U8BIT u8RelAbsTime,
        U8BIT u8Ch10Checksum,
        U32BIT dwOptions
    );
    
    S16BIT (_DECL *aceMTISetExternalClk)
    (
        S16BIT s16DevNum,
        U16BIT u16TTRes,
        U16BIT u16ExtCtrl
    );
    
    S16BIT (_DECL *aceMTIStart)
    (
        S16BIT s16DevNum
    );
    
    S16BIT (_DECL *aceMTIStop)
    (
        S16BIT s16DevNum
    );
    
    S16BIT (_DECL *aceMTIInitiateHostIrq)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceMTIGetCh10DataPkt)
    (
        S16BIT s16DevNum,
        PMTI_CH10_DATA_PKT *ppCh10Pkt,
        S16BIT s16Timeout
    );
    
    S16BIT (_DECL *aceMTIFreeCh10DataPkt)
    (
        S16BIT s16DevNum,
        PMTI_CH10_DATA_PKT *ppCh10Pkt
    );
    
    S16BIT (_DECL *aceMTIGetCh10TimePkt)
    (
        S16BIT s16DevNum,
        PMTI_CH10_TIME_PKT *ppCh10Pkt,
        S16BIT Timeout
    );
    
    S16BIT (_DECL *aceMTIFreeCh10TimePkt)
    (
        S16BIT s16DevNum,
        PMTI_CH10_TIME_PKT *ppCh10Pkt
    );
    
    S16BIT (_DECL *aceMTIPause)
    (
        S16BIT s16DevNum
    );
    
    S16BIT (_DECL *aceMTIContinue)
    (
        S16BIT s16DevNum
    );
    
    S16BIT (_DECL *aceMTICh10TimeFmt)
    (
        S16BIT s16DevNum,
        U16BIT u16Format
    );
    
    S16BIT (_DECL *aceMTICh10TimePktEnable)
    (
        S16BIT s16DevNum,
        BOOLEAN bEnable
    );
    
    S16BIT (_DECL *aceMtiGetMetrics)
    (
        S16BIT DevNum,
        ACEX_MTI_METRICS *pMtiMetrics
    );
    
    S16BIT (_DECL *aceMTIDecodeRawMsg)
    (
        PMTI_CH10_DATA_PKT pCh10Pkt,
        U16BIT **ppCurMsg,
        MSGSTRUCT *pDecMsg
    );
	    
    S16BIT (_DECL *aceMTISetCh10TimeRange)
    (
        S16BIT DevNum,
        ACE_MTI_IRIG_RANGE Range
    );
    
    S16BIT (_DECL *aceMTIGetCh10TimeRange)
    (
        S16BIT DevNum,
        ACE_MTI_IRIG_RANGE *pRange
    );
    
    S16BIT (_DECL *aceMTISetCh10TimePktId)
    (
        S16BIT DevNum,
        U16BIT u16ChannelId
    );
    

    S16BIT (_DECL *acexMTISetRespTimeout)
    (
        S16BIT s16DevNum,
        U32BIT u32Multiple
    );
    
	/* MCS 9.3.19 MTI Decode 1553A -- Add DevNum*/
	S16BIT(_DECL *aceMTIDecodeRawMsgEx)
	(
		S16BIT DevNum,
		PMTI_CH10_DATA_PKT pCh10Pkt,
		U16BIT **ppCurMsg,
		MSGSTRUCT *pDecMsg,
		U32BIT u32Options  /* Future expansion */
	);

} API_MTI_FUNCTIONS;

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */

/*  This structure describes MTI Status (Debug Information). */
typedef struct MTI_STATUS
{
    U32BIT u32IntConditions;
    U32BIT u32IrqDataLen;
    U32BIT u32IrqMsgCnt;
    U16BIT u16IrqTimeInterval;
    U32BIT u32NumRxPkts;
    U32BIT u32BufPoolEmpty;
    U32BIT u32NumDroppedPkts;
    U32BIT u32DmaInitFailures;
    U32BIT u32NumOverflow;

    /* queue-specific values */
    U32BIT u32QueueSize;
    U32BIT u32EntriesInQueue;
    U32BIT u32NextRead;
    U32BIT u32LastWritten;
    
} MTI_STATUS, *PMTI_STATUS;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceMTIConfigure
(
    S16BIT DevNum,
    U32BIT u32DevBufByteSize,
    U32BIT u32NumBufBlks,
    U32BIT u32BufBlkByteSize,
    BOOLEAN fZeroCopyEnable,
    U32BIT u32IrqDataLen,
    U32BIT u32IrqMsgCnt,
    U16BIT u16IrqTimeInterval,
    U32BIT u32IntConditions,
    U16BIT u16Ch10ChnlId,
    U8BIT u8HdrVer,
    U8BIT u8RelAbsTime,
    U8BIT u8Ch10Checksum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL aceMTISetExternalClk
(
    S16BIT DevNum,
    U16BIT wTTRes,
    U16BIT wExtCtrl
);

_EXTERN S16BIT _DECL aceMTIStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceMTIInitiateHostIrq
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceMTIGetCh10DataPkt
(
    S16BIT DevNum,
    PMTI_CH10_DATA_PKT *ppCh10DataPkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL aceMTIFreeCh10DataPkt
(
    S16BIT DevNum,
    PMTI_CH10_DATA_PKT *ppCh10DataPkt
);

_EXTERN S16BIT _DECL aceMTIGetCh10TimePkt
(
    S16BIT DevNum,
    PMTI_CH10_TIME_PKT *ppCh10TimePkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL aceMTIFreeCh10TimePkt
(
    S16BIT DevNum,
    PMTI_CH10_TIME_PKT *ppCh10TimePkt
);

_EXTERN S16BIT _DECL aceMTIPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceMTIContinue
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceMTICh10TimeFmt
(
    S16BIT DevNum,
    U16BIT Format
);

_EXTERN S16BIT _DECL aceMTICh10TimePktEnable
(
    S16BIT DevNum,
    BOOLEAN bEnable
);

_EXTERN S16BIT _DECL aceMTIDecodeRawMsg
(
    PMTI_CH10_DATA_PKT pCh10Pkt,
    U16BIT **ppCurMsg,
    MSGSTRUCT *pDecMsg
);

/* MCS 9.3.19 MTI Decode 1553A -- Add DevNum*/
_EXTERN S16BIT _DECL aceMTIDecodeRawMsgEx
(
	S16BIT DevNum,
	PMTI_CH10_DATA_PKT pCh10Pkt,
	U16BIT **ppCurMsg,
	MSGSTRUCT *pDecMsg,
	U32BIT u32Options  /* Future expansion */
);

_EXTERN S16BIT _DECL aceMTISetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE Range
);

_EXTERN S16BIT _DECL aceMTIGetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE *pRange
);

_EXTERN S16BIT _DECL aceMTISetCh10TimePktId
(
    S16BIT DevNum,
    U16BIT u16ChannelId
);

_EXTERN S16BIT _DECL aceMtiGetMetrics
(
    S16BIT DevNum,
    ACEX_MTI_METRICS *pMtiMetrics
);

_EXTERN S16BIT _DECL acexMTISetRespTimeout
(
    S16BIT s16DevNum,
    U32BIT u32Multiple
);

#endif /* ifndef __MTI_H__ */
