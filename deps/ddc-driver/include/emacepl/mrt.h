/*******************************************************************************
 * FILE: mrt.h
 *
 * DESCRIPTION:
 *
 *  TODO
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

#ifndef __MRT_H__
#define __MRT_H__

/* ========================================================================== */
/*                              MACRO DEFINITIONS                             */
/* ========================================================================== */
#define ACEX_MRT_OPT_RESET_CMDSTK           0x00000001 /* MRT Start Option */
#define ACEX_MRT_START_ALL_CONFIGURED_RTS   -1         /* this is used by acexMRTStart to start multiple RTs */

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_MRT_FUNCTIONS
{
    S16BIT (_DECL *acexMRTConfigure)
    (
        S16BIT DevNum,
        U16BIT wCmdStkSize,
        U32BIT u32GblDataStkType,
        U16BIT u16GblDataStkBlkID
    );
    
    S16BIT (_DECL *acexMRTStart)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U32BIT u32Options
    );
    
    S16BIT (_DECL *acexMRTStop)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTEnableRT)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U32BIT u32Options
    );
    
    S16BIT (_DECL *acexMRTDisableRT)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTDataBlkMapToRTSA)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        S16BIT nDataBlkID,
        U16BIT wSA,
        U16BIT wMsgType,
        U16BIT wIrqOptions,
        U16BIT wLegalizeSA
    );
    
    S16BIT (_DECL *acexMRTDataBlkUnmapFromRTSA)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        S16BIT nDataBlkID,
        U16BIT u16SA,
        U16BIT u16MsgType
    );
    
    S16BIT (_DECL *acexMRTConfigRTBITWrd)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wBITLoc,
        U16BIT wBITBusyInh
    );
    
    S16BIT (_DECL *acexMRTReadRTBITWrd)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wBITLoc,
        U16BIT *pBITWrd
    );
    
    S16BIT (_DECL *acexMRTWriteRTBITWrd)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wBITWrd
    );
    
    S16BIT (_DECL *acexMRTSetRTBusyBitsTbl)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT dwSAMask
    );
    
    S16BIT (_DECL *acexMRTClearRTBusyBitsTbl)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT dwSAMask
    );
    
    S16BIT (_DECL *acexMRTGetRTBusyBitsTblStatus)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT *pdwSABusyBits
    );
    
    S16BIT (_DECL *acexMRTGetRTStatusBits)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT *pwStatusBits
    );
    
    S16BIT (_DECL *acexMRTSetRTStatusBits)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wStatusBits
    );
    
    S16BIT (_DECL *acexMRTClearRTStatusBits)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wStatusBits
    );
    
    S16BIT (_DECL *acexMRTGetRTMsgLegalityStatus)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT *pdwWC_MCMask
    );
    
    S16BIT (_DECL *acexMRTEnableRTMsgLegality)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT dwWC_MCMask
    );
    
    S16BIT (_DECL *acexMRTDisableRTMsgLegality)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT dwWC_MCMask
    );
    
    S16BIT (_DECL *acexMRTEnableRTModeCodeIrq)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wModeCodeType,
        U16BIT wModeCodeIrq
    );
    
    S16BIT (_DECL *acexMRTDisableRTModeCodeIrq)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wModeCodeType,
        U16BIT wModeCodeIrq
    );
    
    S16BIT (_DECL *acexMRTGetRTModeCodeIrqStatus)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wModeCodeType,
        U16BIT *pwMCIrqStatus
    );
    
    S16BIT (_DECL *acexMRTWriteRTModeCodeData)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wModeCode,
        U16BIT wMCData
    );
    
    S16BIT (_DECL *acexMRTReadRTModeCodeData)
    (
        S16BIT DevNum,
        S8BIT s8RtAddr,
        U16BIT wModeCode,
        U16BIT *pMCData
    );
    
    S16BIT (_DECL *acexMRTDataStreamCreate)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        S8BIT s8RTAddr,
        U32BIT u32SAMask
    );
    
    S16BIT (_DECL *acexMRTDataStreamDelete)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        S8BIT s8RTAddr
    );
    
    S16BIT (_DECL *acexMRTDataStreamSend)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        S8BIT s8RTAddr,
        VOID *pBuffer,
        U16BIT u16BufferBytes,
        S32BIT s32TimeoutMs
    );
    
    S16BIT (_DECL *acexMRTDataStreamReceive)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        S8BIT s8RTAddr,
        VOID *pBuffer,
        U16BIT u16BufferBytes,
        S32BIT s32TimeoutMs
    );
    
    S16BIT (_DECL *acexMRTDataArrayCreate)
    (
        S16BIT s16DevNum,
        U16BIT u16ID,
        S8BIT s8RTAddr,
        U16BIT u16SA,
        S16BIT s16DblkID,
        U32BIT u32BufWdSize,
        BOOLEAN bContinuous
    );
    
    S16BIT (_DECL *acexMRTDataArrayDelete)
    (
        S16BIT s16DevNum,
        U16BIT u16ID,
        S8BIT s8RTAddr
    );
    
    S16BIT (_DECL *acexMRTDataArraySend)
    (
        S16BIT s16DevNum,
        U16BIT u16ID,
        S8BIT s8RTAddr,
        U8BIT *pBuf
    );
    

    S16BIT (_DECL *acexMRTSetMsgError)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        ACEX_ERR_INJ *psError
    );
    
    S16BIT (_DECL *acexMRTMsgErrorEnable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTMsgErrorDisable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTSetRespTime)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        U32BIT u32Multiple
    );
    
    S16BIT (_DECL *acexMRTRespTimeEnable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTRespTimeDisable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTSetRespTimeout)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        U32BIT u32Multiple
    );
    
    S16BIT (_DECL *acexMRTDbcEnable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        U32BIT u32RtHoldoffTime
    );
    
    S16BIT (_DECL *acexMRTDbcDisable)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr
    );
    
    S16BIT (_DECL *acexMRTImrMapToRTSA)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        U16BIT u16SA,
        U16BIT u16MsgType,
        U32BIT u32ImrType
    );
    
    S16BIT (_DECL *acexMRTImrTrigSelect)
    (
        S16BIT s16DevNum,
        S8BIT s8RtAddr,
        U16BIT u16Select
    );
    
} API_MRT_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL acexMRTConfigure
(
    S16BIT DevNum,
    U16BIT wCmdStkSize,
    U32BIT u32GblDataStkType,
    U16BIT u16GblDataStkBlkID
);

_EXTERN S16BIT _DECL acexMRTStart
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL acexMRTStop
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTEnableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL acexMRTDisableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTDataBlkMapToRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL acexMRTDataBlkUnmapFromRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT u16SA,
    U16BIT u16MsgType
);

_EXTERN S16BIT _DECL acexMRTConfigRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

_EXTERN S16BIT _DECL acexMRTReadRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL acexMRTWriteRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL acexMRTSetRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL acexMRTClearRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL acexMRTGetRTBusyBitsTblStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL acexMRTGetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL acexMRTSetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL acexMRTClearRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL acexMRTGetRTMsgLegalityStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwWC_MCMask
);

_EXTERN S16BIT _DECL acexMRTEnableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL acexMRTDisableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL acexMRTEnableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL acexMRTDisableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL acexMRTGetRTModeCodeIrqStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL acexMRTWriteRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL acexMRTReadRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL acexMRTDataStreamCreate
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    U32BIT u32SAMask
);

_EXTERN S16BIT _DECL acexMRTDataStreamDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL acexMRTDataStreamSend
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL acexMRTDataStreamReceive
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL acexMRTDataArrayCreate
(
    S16BIT s16DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U16BIT u16SA,
    S16BIT s16DblkID,
    U32BIT u32BufWdSize,
    BOOLEAN bContinuous
);

_EXTERN S16BIT _DECL acexMRTDataArrayDelete
(
    S16BIT s16DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL acexMRTDataArraySend
(
    S16BIT s16DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U8BIT *pBuf
);

_EXTERN S16BIT _DECL acexMRTSetMsgError
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL acexMRTMsgErrorEnable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTMsgErrorDisable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTSetRespTime
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL acexMRTRespTimeEnable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTRespTimeDisable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTSetRespTimeout
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL acexMRTDbcEnable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32RtHoldoffTime
);

_EXTERN S16BIT _DECL acexMRTDbcDisable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL acexMRTImrMapToRTSA
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U16BIT u16SA,
    U16BIT u16MsgType,
    U32BIT u32ImrType
);

_EXTERN S16BIT _DECL acexMRTImrTrigSelect
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U16BIT u16Select
);

#endif /* __MRT_H__ */
