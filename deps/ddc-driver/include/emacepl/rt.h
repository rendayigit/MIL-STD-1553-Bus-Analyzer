/*******************************************************************************
 * FILE: rt.h
 *
 * DESCRIPTION:
 *
 *  Remote Terminal public include file.
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

#ifndef __RT_H__
#define __RT_H__

/* =========================================================================*/
/*                  RT FUNCTION POINTER TEMPLATE                            */
/* =========================================================================*/
typedef struct API_RT_FUNCTIONS
{
    /* RT API */
    S16BIT (_DECL *aceRTConfigure)
    (
        S16BIT DevNum,
        U16BIT wCmdStkSize,
        U32BIT dwOptions
    );
    
    S16BIT (_DECL *aceRTSetAddress)
    (
        S16BIT DevNum,
        U16BIT wRTAddress
    );
    
    S16BIT (_DECL *aceRTGetAddress)
    (
        S16BIT DevNum,
        U16BIT *pRTAddress
    );
    
    S16BIT (_DECL *aceRTDataBlkCreate)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT wDataBlkType,
        U16BIT *pBuffer,
        U16BIT wBufferSize
    );
    
    S16BIT (_DECL *aceRTDataBlkDelete)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID
    );
    
    S16BIT (_DECL *aceRTDataBlkWrite)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U16BIT wOffset
    );
    
    S16BIT (_DECL *aceRTDataBlkRead)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U16BIT wOffset
    );
    
    S16BIT (_DECL *aceRTDataBlkCircBufInfo)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT *pUserRWOffset,
        U16BIT *pAceRWOffset
    );
    
    S16BIT (_DECL *aceRTModeCodeIrqEnable)
    (
        S16BIT DevNum,
        U16BIT wModeCodeType,
        U16BIT wModeCodeIrq
    );
    
    S16BIT (_DECL *aceRTModeCodeIrqDisable)
    (
        S16BIT DevNum,
        U16BIT wModeCodeType,
        U16BIT wModeCodeIrq
    );
    
    S16BIT (_DECL *aceRTModeCodeIrqStatus)
    (
        S16BIT DevNum,
        U16BIT wModeCodeType,
        U16BIT *pwMCIrqStatus
    );
    
    S16BIT (_DECL *aceRTModeCodeReadData)
    (
        S16BIT DevNum,
        U16BIT wModeCode,
        U16BIT *pMCData
    );
    
    S16BIT (_DECL *aceRTModeCodeWriteData)
    (
        S16BIT DevNum,
        U16BIT wModeCode,
        U16BIT wMCData
    );
    
    S16BIT (_DECL *aceRTMsgLegalityEnable)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT dwWC_MCMask
    );
    
    S16BIT (_DECL *aceRTMsgLegalityDisable)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT dwWC_MCMask
    );
    
    S16BIT (_DECL *aceRTMsgLegalityStatus)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U16BIT wSA,
        U32BIT *pdwWC_MCMask
    );
    
    S16BIT (_DECL *aceRTBusyBitsTblSet)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT dwSAMask
    );
    
    S16BIT (_DECL *aceRTBusyBitsTblClear)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT dwSAMask
    );
    
    S16BIT (_DECL *aceRTBusyBitsTblStatus)
    (
        S16BIT DevNum,
        U16BIT wOwnAddrOrBcst,
        U16BIT wTR,
        U32BIT *pdwSABusyBits
    );
    
    S16BIT (_DECL *aceRTStatusBitsSet)
    (
        S16BIT DevNum,
        U16BIT wStatusBits
    );
    
    S16BIT (_DECL *aceRTStatusBitsClear)
    (
        S16BIT DevNum,
        U16BIT wStatusBits
    );
    
    S16BIT (_DECL *aceRTStatusBitsStatus)
    (
        S16BIT DevNum,
        U16BIT *pwStatusBits
    );
    
    S16BIT (_DECL *aceRTDataBlkMapToSA)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT wSA,
        U16BIT wMsgType,
        U16BIT wIrqOptions,
        U16BIT wLegalizeSA
    );
    
    S16BIT (_DECL *aceRTDataBlkUnmapFromSA)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT wSA,
        U16BIT wMsgType
    );
    
    S16BIT (_DECL *aceRTStart)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTStop)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTBITWrdRead)
    (
        S16BIT DevNum,
        U16BIT wBITLoc,
        U16BIT *pBITWrd
    );
    
    S16BIT (_DECL *aceRTBITWrdWrite)
    (
        S16BIT DevNum,
        U16BIT wBITWrd
    );
    
    S16BIT (_DECL *aceRTGetStkMsgsRaw)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        U16BIT wBufferSize
    );
    
    S16BIT (_DECL *aceRTGetStkMsgDecoded)
    (
        S16BIT DevNum,
        MSGSTRUCT *pMsg,
        U16BIT wMsgLoc
    );
    
    S16BIT (_DECL *aceRTInstallHBuf)
    (
        S16BIT DevNum,
        U32BIT dwHBufSize
    );
    
    S16BIT (_DECL *aceRTUninstallHBuf)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTStkToHBuf)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTStkToHBuf32)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTGetHBufMsgsRaw)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U32BIT *pdwMsgCount,
        U32BIT *pdwMsgLostStk,
        U32BIT *pdwMsgLostHBuf
    );
    
    S16BIT (_DECL *aceRTGetHBufMsgCount)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTGetHBufMsgDecoded)
    (
        S16BIT DevNum,
        MSGSTRUCT *pMsg,
        U32BIT *pdwMsgCount,
        U32BIT *pdwMsgLostStk,
        U32BIT *pdwMsgLostHBuf,
        U16BIT wMsgLoc
    );
    
    S16BIT (_DECL *aceRTCreateImageFiles)
    (
        S16BIT DevNum,
        char *pszIFile,
        char *pszHFile
    );
    
    S16BIT (_DECL *aceRTGetHBufMetric)
    (
        S16BIT DevNum,
        HBUFMETRIC *pMetric,
        U16BIT bReset
    );
    
    S16BIT (_DECL *aceRTGetStkMetric)
    (
        S16BIT DevNum,
        STKMETRIC *pMetric,
        U16BIT bReset
    );
    
    S16BIT (_DECL *aceRTSetAddrSource)
    (
        S16BIT DevNum,
        U16BIT wRTSource
    );
    
    S16BIT (_DECL *aceRTGetAddrSource)
    (
        S16BIT DevNum,
        U16BIT *wRTSource
    );
    
    S16BIT (_DECL *aceRTRelatchAddr)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTAcquireAddress)
    (
        S16BIT DevNum,
        U16BIT *wRTAddr,
        U16BIT wCANBus
    );
    
    S16BIT (_DECL *aceRTDecodeRawMsg)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        MSGSTRUCT *pMsg
    );
    
    S16BIT (_DECL *aceRTBITWrdConfig)
    (
        S16BIT DevNum,
        U16BIT wBITLoc,
        U16BIT wBITBusyInh
    );
    
}API_RT_FUNCTIONS;

/* =========================================================================*/
/*                  RT PUBLIC FUNCTION PROTOTYPES                           */
/* =========================================================================*/
_EXTERN S16BIT _DECL aceRTConfigure
(
    S16BIT DevNum,
    U16BIT wCmdStkSize,
    U32BIT wOptions
);

_EXTERN S16BIT _DECL aceRTSetAddress
(
    S16BIT DevNum,
    U16BIT wRTAddress
);

_EXTERN S16BIT _DECL aceRTGetAddress
(
    S16BIT DevNum,
    U16BIT *pRTAddress
);

_EXTERN S16BIT _DECL aceRTDataBlkCircBufInfo
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pUserRWOffset,
    U16BIT *pAceRWOffset
);

_EXTERN S16BIT _DECL aceRTDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL aceRTDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL aceRTDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL aceRTDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL aceRTDataBlkMapToSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL aceRTDataBlkUnmapFromSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType
);

_EXTERN S16BIT _DECL aceRTModeCodeIrqEnable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL aceRTModeCodeIrqDisable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL aceRTModeCodeIrqStatus
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL aceRTModeCodeReadData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL aceRTModeCodeWriteData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL aceRTSetAddrSource
(
    S16BIT DevNum,
    U16BIT wRTSource
);

_EXTERN S16BIT _DECL aceRTGetAddrSource
(
    S16BIT DevNum,
    U16BIT *wRTSource
);

_EXTERN S16BIT _DECL aceRTMsgLegalityEnable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL aceRTMsgLegalityDisable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL aceRTMsgLegalityStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwMsgLegality
);

_EXTERN S16BIT _DECL aceRTBusyBitsTblSet
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL aceRTBusyBitsTblClear
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL aceRTBusyBitsTblStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL aceRTStatusBitsSet
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL aceRTStatusBitsClear
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL aceRTStatusBitsStatus
(
    S16BIT DevNum,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL aceRTBITWrdConfig
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

_EXTERN S16BIT _DECL aceRTBITWrdRead
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL aceRTBITWrdWrite
(
    S16BIT DevNum,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL aceRTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL aceRTDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL aceRTGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL aceRTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL aceRTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL aceRTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL aceRTCreateImageFiles
(
    S16BIT DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL aceRTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL aceRTGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL aceRTRelatchAddr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTAcquireAddress
(
    S16BIT DevNum,
    U16BIT *wRTAddr,
    U16BIT wCANBus
);

#endif /* ifndef __RT_H__ */
