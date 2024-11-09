/*******************************************************************************
 * FILE: TODO
 *
 * DESCRIPTION:
 *
 *      TODO
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

#ifndef __DDC_SOCK_CLNT_LIB_H__
#define __DDC_SOCK_CLNT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/***********************************************
    CONFIGR.C
************************************************/
_EXTERN S16BIT _DECL remoteAceInitialize
(
    S16BIT DevNum,
    U16BIT wAccess,
    U16BIT wMode,
    U32BIT dwMemWrdSize,
    U32BIT dwRegAddr,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL remoteAceFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceSetClockFreq
(
    S16BIT DevNum,
    U16BIT u16ClockIn
);

_EXTERN S16BIT _DECL remoteAceSetTimeTagValue
(
    S16BIT DevNum,
    U16BIT wTTValue
);

_EXTERN S16BIT _DECL remoteAceSetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT ullTTValue
);

_EXTERN S16BIT _DECL remoteAceGetTimeTagValue
(
    S16BIT DevNum,
    U16BIT* wTTValue
);

_EXTERN S16BIT _DECL remoteAceGetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT *ullTTValue
);

_EXTERN S16BIT _DECL remoteAceResetTimeTag
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceSetRespTimeOut
(
    S16BIT DevNum,
    U16BIT u16RespTimeOut
);

_EXTERN S16BIT _DECL remoteAceSetIrqConfig
(
    S16BIT DevNum,
    U16BIT wLvlOrPulse,
    U16BIT wAutoClear
);

_EXTERN S16BIT _DECL remoteAceSetTimeTagRes
(
    S16BIT DevNum,
    U16BIT wTTRes
);

_EXTERN S16BIT _DECL remoteAceSetIrqConditions(S16BIT DevNum,   U16BIT bEnable, U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    ));
_EXTERN S16BIT _DECL remoteAceSetDecoderConfig
(
    S16BIT DevNum,
    U16BIT wDoubleOrSingle,
    U16BIT wExpXingEnable
);

_EXTERN S16BIT _DECL remoteAceSetRamParityChecking
(
    S16BIT DevNum,
    U16BIT u16RamParityEnable
);

_EXTERN U16BIT _DECL remoteAceRegRead
(
    S16BIT DevNum,
    U16BIT wRegAddr
);

_EXTERN S16BIT _DECL remoteAceRegWrite
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U16BIT wValue
);

_EXTERN U16BIT _DECL remoteAceMemRead
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL remoteAceMemWrite
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN S16BIT _DECL remoteAceISQClear
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL remoteAceISQEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL remoteAceSetMetrics
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL remoteAceGetHwVersionInfo
(
    S16BIT DevNum,
    PHWVERSIONINFO pHwVersionInfo
);

_EXTERN S16BIT _DECL remoteAceSetAsyncIsr(S16BIT DevNum, void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    ));
_EXTERN S16BIT _DECL remoteAceGetCardType
(
    S16BIT DevNum,
    U32BIT *dwCardType
);

_EXTERN U32BIT _DECL remoteAceRegRead32
(
    S16BIT DevNum,
    U16BIT wRegAddr
);

_EXTERN S16BIT _DECL remoteAceRegWrite32
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U32BIT dwValue
);

_EXTERN U32BIT _DECL remoteAceMemRead32
(
    S16BIT DevNum,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL remoteAceMemWrite32
(
    S16BIT DevNum,
    U32BIT dwMemAddr,
    U32BIT dwValue
);

_EXTERN S16BIT _DECL remoteAceIOInitialize
(
    S16BIT DevNum,
    S16BIT wOptions
);

_EXTERN S16BIT _DECL remoteAceIOFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceGetChannelCount
(
    S16BIT DevNum,
    CHANCOUNT_t *pChanCount
);

_EXTERN S16BIT _DECL remoteAceGetIRIGTx
(
    S16BIT DevNum,
    PACE_IRIG_TX pstructIRIG
);

_EXTERN S16BIT _DECL remoteAceSetIRIGTx
(
    S16BIT DevNum,
    ACE_IRIG_TX structIRIG
);

_EXTERN S16BIT _DECL remoteAcexGetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING *peCoupling,
    CHANNEL_TERMINATION *peTermination
);

_EXTERN S16BIT _DECL remoteAcexSetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING eCoupling,
    CHANNEL_TERMINATION eTermination
);

_EXTERN S16BIT _DECL remoteAcexGetAmplitude
(
    S16BIT DevNum,
    U32BIT *pu32Amplitude
);

_EXTERN S16BIT _DECL remoteAcexSetAmplitude
(
    S16BIT DevNum,
    U32BIT u32Amplitude
);

_EXTERN S16BIT _DECL remoteAcexComponentRegRead
(
    S16BIT DevNum,
    U16BIT u16RegAddrOffest,
    U16BIT u16Length,
    U32BIT *pData,
    ACEX_COMPONENT eComponent
);

_EXTERN S16BIT _DECL remoteAcexEITxShutdownEnable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexEITxShutdownDisable
(
    S16BIT DevNum
);

_EXTERN U32BIT _DECL remoteAcexRegRead
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwRegType
);

_EXTERN S32BIT _DECL remoteAcexRegWrite
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwValue,
    U32BIT dwRegType
);

/* Debug */
_EXTERN S16BIT _DECL remoteSetAccessDebug
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteDebugRegRead
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwRegType,
    U32BIT *pData
);

_EXTERN S16BIT _DECL remoteDebugRegWrite
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT u32RegType,
    U32BIT u32Value
);

_EXTERN S16BIT _DECL remoteDebugMemRead
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT *pu32Data
);

_EXTERN S16BIT _DECL remoteDebugMemWrite
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT u32Value
);

_EXTERN S16BIT _DECL remoteDebugFlashInfo
(
    S16BIT DevNum,
    PFLASH_INFO pFlashInfo
);

_EXTERN S16BIT _DECL remoteDebugFlashRead
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT *pu32Data
);

_EXTERN S16BIT _DECL remoteDebugGetChCount
(
    S16BIT DevNum,
    PENHANCED_CAPABILITY_INFO pEnhancedCardCapabilities
);


/*************************************************
     BCR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceBCConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL remoteAceBCDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL remoteAceBCDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL remoteAceBCDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL remoteAceBCDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL remoteAceBCDataBlkRead32
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U32BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateBCtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateRTtoBC
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRTRx,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateBcstRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreateBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgCreate
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID1,
    U16BIT wBCCtrlWrd1,
    U16BIT wCmdWrd1_1,
    U16BIT wCmdWrd1_2,
    U16BIT wMsgGapTime1,
    S16BIT nDataBlkID2,
    U16BIT wBCCtrlWrd2,
    U16BIT wCmdWrd2_1,
    U16BIT wCmdWrd2_2,
    U16BIT wMsgGapTime2,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyBCtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyRTtoBC
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRTRx,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyBcstRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModifyBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgModify
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID1,
    U16BIT wBCCtrlWrd1,
    U16BIT wCmdWrd1_1,
    U16BIT wCmdWrd1_2,
    U16BIT wMsgGapTime1,
    S16BIT nDataBlkID2,
    U16BIT wBCCtrlWrd2,
    U16BIT wCmdWrd2_1,
    U16BIT wCmdWrd2_2,
    U16BIT wMsgGapTime2,
    U16BIT wModFlags
);

_EXTERN S16BIT _DECL remoteAceBCMsgDelete
(
    S16BIT DevNum,
    S16BIT nMsgBlkID
);

_EXTERN S16BIT _DECL remoteAceBCOpCodeCreate
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL remoteAceBCOpCodeDelete
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL remoteAceBCFrameCreate(S16BIT DevNum,
    S16BIT nFrameID, U16BIT wFrameType,
    S16BIT aOpCodeIDs[], U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime, U16BIT wFlags);
_EXTERN S16BIT _DECL remoteAcexBCFrameCreate(S16BIT DevNum,
    S16BIT nFrameID, U16BIT wFrameType,
    S16BIT aOpCodeIDs[], U16BIT wOpCodeCount,
    U32BIT dwMnrFrmTime, U16BIT wFlags);

_EXTERN S16BIT _DECL remoteAceBCFrameDelete
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN S16BIT _DECL remoteAceBCStart
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    S32BIT lMjrFrmCount
);

_EXTERN S16BIT _DECL remoteAcexBCStartReplay
(
    S16BIT DevNum,
    const CHAR *pFileName,
    S32BIT lLoopCount
);

_EXTERN S16BIT _DECL remoteAcexBCStartReplayRaw
(
    S16BIT DevNum,
    PVOID *pReplayData,
    U32BIT u32DataLength
);

_EXTERN S16BIT _DECL remoteAceBCStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCMsgGapTimerEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL remoteAceBCSetGPFState
(
    S16BIT DevNum,
    U16BIT wGPF,
    U16BIT wStateChange
);

_EXTERN S16BIT _DECL remoteAceBCGetConditionCode
(
    S16BIT DevNum,
    U16BIT wConditionCode,
    U16BIT *pCurrentState
);

_EXTERN S16BIT _DECL remoteAceBCSetWatchDogTimer
(
    S16BIT DevNum,
    U16BIT bEnable,
    U16BIT wTimeOut
);

_EXTERN S16BIT _DECL remoteAceBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL remoteAcexBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL remoteAceBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL remoteAcexBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL remoteAceBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL  remoteAcexBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL remoteAceBCDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL remoteAceBCInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL remoteAceBCUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL remoteAcexBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL remoteAceBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL remoteAcexBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    U16BIT *pMsgId,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL remoteAceBCFrmToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCFrmToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCGPQRead
(
    S16BIT DevNum,
    GPQENTRY* pGPQEntry
);

_EXTERN S16BIT _DECL remoteAceBCGPQGetCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCSetMsgRetry
(
    S16BIT DevNum,
    U16BIT wNumOfRetries,
    U16BIT wFirstRetryBus,
    U16BIT wSecondRetryBus,
    U16BIT wReserved
);

_EXTERN S16BIT _DECL remoteAceBCCreateImageFiles
(
    S16BIT DevNum,
    S16BIT nMjrFrameID,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL remoteAceBCGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL remoteAceBCGetGPQMetric
(
    S16BIT DevNum,
    GPQMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateBCtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateRTtoBC
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRTRx,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wRT,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateBcstRTtoRT
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSARx,
    U16BIT wWC,
    U16BIT wRTTx,
    U16BIT wSATx,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCAsyncMsgCreateBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions,
    U16BIT *pBuffer
);

_EXTERN S16BIT _DECL remoteAceBCSendAsyncMsgLP
(
    S16BIT DevNum,
    U16BIT *pMsgLeft,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL remoteAceBCSendAsyncMsgHP
(
    S16BIT DevNum,
    U16BIT nMsgID,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL remoteAceBCEmptyAsyncList
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCResetAsyncPtr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCGetAsyncCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceBCDisableMessage
(
    S16BIT DevNum,
    U16BIT wMsgNum,
    BOOLEAN bDisable
);

_EXTERN S16BIT _DECL remoteAcexBCInitialize
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCTimeSlotCreate
(
    S16BIT DevNum,
    U16BIT u16SlotTime,
    U16BIT u16SlotCount,
    U32BIT u32IrqMask
);

_EXTERN S16BIT _DECL remoteAcexBCTimeSlotItemCreate
(
    S16BIT DevNum,
    S16BIT s16SlotItemId,
    S16BIT s16MsgId,
    U16BIT u16SlotStart,
    U16BIT u16SlotInterval
);

_EXTERN S16BIT _DECL remoteAcexBCGetCurrTimeSlot
(
    S16BIT DevNum,
    U16BIT *pTimeSlot
);

_EXTERN S16BIT _DECL remoteAcexBCDataStreamCreateBCRT
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr,
    U32BIT u32SAMask,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexBCDataStreamCreateRTRT
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr1,
    U32BIT u32SAMask1,
    U16BIT u16RTAddr2,
    U32BIT u32SAMask2,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexBCDataStreamDelete
(
    S16BIT DevNum,
    S16BIT s16DataStrId
);

_EXTERN S16BIT _DECL remoteAcexBCDataStreamSend
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL remoteAcexBCDataStreamReceive
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL remoteAcexBCDataArrayCreateBCtoRT
(
    S16BIT DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16RTAddr,
    U16BIT u16RTSubAddr,
    U16BIT u16Transfer,
    U16BIT u16WordCount,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexBCDataArrayDelete
(
    S16BIT DevNum,
    S16BIT s16DataArrayId
);

_EXTERN S16BIT _DECL remoteAcexBCDataArraySend
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U16BIT u16WordCount
);

_EXTERN S16BIT _DECL remoteAceBCGetStatus
(
    S16BIT DevNum,
    U16BIT *wStatus
);

_EXTERN S16BIT _DECL remoteAceBCExtTriggerEnable
(
    S16BIT DevNum,
    BOOLEAN bOnStart
);

_EXTERN S16BIT _DECL remoteAceBCExtTriggerDisable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCAsyncMsgSendLP
(
    S16BIT DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL remoteAcexBCAsyncMsgSendHP
(
    S16BIT DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL remoteAcexBCAsyncQueueInfoLP
(
    S16BIT DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL remoteAcexBCAsyncQueueInfoHP
(
    S16BIT DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL remoteAcexBCMemWrdCreate
(
    S16BIT DevNum,
    S16BIT s16MemWrdID,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL remoteAcexBCMemWrdDelete
(
    S16BIT DevNum,
    S16BIT s16MemWrdID
);

_EXTERN S16BIT _DECL remoteAcexBCMemWrdRead
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT *pu32MemWord
);

_EXTERN S16BIT _DECL remoteAcexBCMemWrdWrite
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL remoteAcexBCMemObjCreate
(
    S16BIT DevNum,
    S16BIT s16MemObjID,
    S16BIT s16MemItemID,
    U16BIT u16MemItemType,
    U16BIT u16Offset
);

_EXTERN S16BIT _DECL remoteAcexBCMemObjDelete
(
    S16BIT DevNum,
    S16BIT s16MemObjID
);

_EXTERN S16BIT _DECL remoteAcexBCSetMsgError
(
    S16BIT DevNum,
    S16BIT s16MsgID,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL remoteAcexBCMsgErrorEnable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCMsgErrorDisable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCSetRespTimeout
(
    S16BIT DevNum,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL remoteAcexBCOpCodeRead
(
    S16BIT DevNum,
    S16BIT s16OpCodeID,
    U32BIT *pu32OpCode,
    U32BIT *pu32Param
);

_EXTERN S16BIT _DECL remoteAcexBCOpCodeWrite
(
    S16BIT DevNum,
    S16BIT s16OpCodeID,
    U16BIT u16Selection,
    U32BIT u32OpCode,
    U32BIT u32Param
);

_EXTERN S16BIT _DECL remoteAcexBCDbcEnable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCDbcDisable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCImrTrigSelect
(
    S16BIT DevNum,
    U16BIT u16Select
);

_EXTERN S16BIT _DECL remoteAcexBCPause
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCContinue
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL remoteAcexBCConfigureReplay
(
    S16BIT DevNum,
    U32BIT u32RtDisable,
    BOOLEAN bXtDisable,
    BOOLEAN bBcDisable,
    BOOLEAN bMtirErrDisable,
    U16BIT u16TTRes,
    U16BIT u16ChannelId,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexBCGetStatusReplay
(
    S16BIT DevNum,
    U32BIT *u32Status
);

/*************************************************
     RTR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceRTConfigure
(
    S16BIT DevNum,
    U16BIT wCmdStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL remoteAceRTSetAddress
(
    S16BIT DevNum,
    U16BIT wRTAddress
);

_EXTERN S16BIT _DECL remoteAceRTGetAddress
(
    S16BIT DevNum,
    U16BIT *pRTAddress
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkCircBufInfo
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pUserRWOffset,
    U16BIT *pAceRWOffset
);

_EXTERN S16BIT _DECL remoteAceRTModeCodeIrqEnable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL remoteAceRTModeCodeIrqDisable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL remoteAceRTModeCodeIrqStatus
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL remoteAceRTModeCodeReadData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL remoteAceRTModeCodeWriteData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL remoteAceRTMsgLegalityEnable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAceRTMsgLegalityDisable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAceRTMsgLegalityStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAceRTBusyBitsTblSet
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAceRTBusyBitsTblClear
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAceRTBusyBitsTblStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL remoteAceRTStatusBitsSet
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL remoteAceRTStatusBitsClear
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL remoteAceRTStatusBitsStatus
(
    S16BIT DevNum,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkMapToSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL remoteAceRTDataBlkUnmapFromSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType
);

_EXTERN S16BIT _DECL remoteAceRTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTBITWrdRead
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL remoteAceRTBITWrdWrite
(
    S16BIT DevNum,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL remoteAceRTGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL remoteAceRTGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL remoteAceRTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL remoteAceRTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL remoteAceRTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL remoteAceRTCreateImageFiles
(
    S16BIT DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL remoteAceRTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL remoteAceRTGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL remoteAceRTSetAddrSource
(
    S16BIT DevNum,
    U16BIT wRTSource
);

_EXTERN S16BIT _DECL remoteAceRTGetAddrSource
(
    S16BIT DevNum,
    U16BIT *pRTSource
);

_EXTERN S16BIT _DECL remoteAceRTRelatchAddr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL remoteAceRTBITWrdConfig
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

/*************************************************
     MRTR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAcexMRTConfigure
(
    S16BIT DevNum,
    U16BIT u16CmdStkSize,
    U32BIT u32GblDataStkType,
    U16BIT u16GblDataStkBlkID
);

_EXTERN S16BIT _DECL remoteAcexMRTStart
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexMRTStop
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTEnableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAcexMRTDisableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTDataBlkUnmapFromRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT u16SA,
    U16BIT u16MsgType
);

_EXTERN S16BIT _DECL remoteAcexMRTDataBlkMapToRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL remoteAcexMRTConfigRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

_EXTERN S16BIT _DECL remoteAcexMRTReadRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL remoteAcexMRTWriteRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL remoteAcexMRTSetRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAcexMRTClearRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAcexMRTGetRTBusyBitsTblStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL remoteAcexMRTGetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL remoteAcexMRTSetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL remoteAcexMRTClearRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL remoteAcexMRTGetRTMsgLegalityStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAcexMRTEnableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAcexMRTDisableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL remoteAcexMRTEnableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL remoteAcexMRTDisableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL remoteAcexMRTGetRTModeCodeIrqStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL remoteAcexMRTWriteRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL remoteAcexMRTReadRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL remoteAcexMRTDataStreamCreate
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    U32BIT u32SAMask
);

_EXTERN S16BIT _DECL remoteAcexMRTDataStreamDelete
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTDataStreamSend
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL remoteAcexMRTDataStreamReceive
(
    S16BIT DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL remoteAcexMRTDataArrayCreate
(
    S16BIT DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U16BIT u16SA,
    S16BIT s16DblkID,
    U32BIT u32BufWdSize,
    BOOLEAN bContinuous
);

_EXTERN S16BIT _DECL remoteAcexMRTDataArrayDelete
(
    S16BIT DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTDataArraySend
(
    S16BIT DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U8BIT *pBuf
);

_EXTERN S16BIT _DECL remoteAcexMRTSetMsgError
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL remoteAcexMRTMsgErrorEnable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTMsgErrorDisable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTSetRespTime
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL remoteAcexMRTRespTimeEnable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTRespTimeDisable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTSetRespTimeout
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL remoteAcexMRTDbcEnable
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32RtHoldoffTime
);

_EXTERN S16BIT _DECL remoteAcexMRTDbcDisable
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL remoteAcexMRTImrMapToRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT u16SA,
    U16BIT u16MsgType,
    U32BIT u32ImrType
);

_EXTERN S16BIT _DECL remoteAcexMRTImrTrigSelect
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT u16Select
);

/*************************************************
     MTR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceMTConfigure
(
    S16BIT DevNum,
    U16BIT u16MTStkType,
    U16BIT u16CmdStkSize,
    U16BIT u16DataStkSize,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL remoteAceMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL remoteAceMTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTSetHBufTrigger
(
    S16BIT DevNum,
    U16BIT u16HBufPercent,
    MTTRIGGER *pTrg
);

_EXTERN S16BIT _DECL remoteAceMTClearHBufTrigger
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTSwapStks
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT u16BufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL remoteAceMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL remoteAceMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTGetInfo
(
    S16BIT DevNum,
    MTINFO *pInfo
);

_EXTERN S16BIT _DECL remoteAceMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTContinue
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTEnableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAceMTDisableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL remoteAceMTGetRTFilter
(
    S16BIT DevNum,
    U16BIT u16RT,
    U16BIT u16TR,
    U32BIT *pSAMask
);

_EXTERN S16BIT _DECL remoteAceMTDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL remoteAceMTGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT u16BufferSize,
    U16BIT u16StkLoc
);

_EXTERN S16BIT _DECL remoteAceMTGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc,
    U16BIT wStkLoc
);

_EXTERN S16BIT _DECL remoteAceMTCreateImageFiles
(
    S16BIT s16DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL remoteAceMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL remoteAceMTGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT u16Stk,
    U16BIT bReset
);

/*************************************************
    MTIR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceMTIConfigure
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

_EXTERN S16BIT _DECL remoteAceMTISetExternalClk
(
    S16BIT DevNum,
    U16BIT u16TTRes,
    U16BIT u16ExtCtrl
);

_EXTERN S16BIT _DECL remoteAceMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTIStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTIInitiateHostIrq
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTIGetCh10DataPkt
(
    S16BIT DevNum,
    PMTI_CH10_DATA_PKT *ppCh10DataPkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL remoteAceMTIGetCh10TimePkt
(
    S16BIT DevNum,
    PMTI_CH10_TIME_PKT  *ppCh10TimePkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL remoteAceMTIPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTIContinue
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceMTICh10TimeFmt
(
    S16BIT DevNum,
    U16BIT u16Format
);

_EXTERN S16BIT _DECL remoteAceMTICh10TimePktEnable
(
    S16BIT DevNum,
    BOOLEAN bEnable
);

_EXTERN S16BIT _DECL remoteAceMTIGetMetrics
(
    S16BIT DevNum,
    ACEX_MTI_METRICS *pMtiMetrics
);

_EXTERN S16BIT _DECL remoteAceMTISetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE Range
);

_EXTERN S16BIT _DECL remoteAceMTIGetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE *pRange
);

_EXTERN S16BIT _DECL remoteAceMTISetCh10TimePktId
(
    S16BIT DevNum,
    U16BIT u16ChannelId
);

_EXTERN S16BIT _DECL remoteAceMTISetRespTimeout
(
    S16BIT DevNum,
    U32BIT u32Multiple
);

/*************************************************
     RTMTR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceRTMTConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
    U16BIT wMTStkType,
    U16BIT wMTCmdStkSize,
    U16BIT wMTDataStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL remoteAceRTMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwRTMTHBufSize
);

_EXTERN S16BIT _DECL remoteAceRTMTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL remoteAceRTMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL remoteAceRTMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

/*************************************************
     RTMTIR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceRTMTIConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
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

_EXTERN S16BIT _DECL remoteAceRTMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL remoteAceRTMTIStop
(
    S16BIT DevNum
);

/*************************************************
       DIOR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceGetDiscOut
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceSetDiscDir
(
    S16BIT DevNum,
    S16BIT Channel,
    DISC_DIR Direction
);

_EXTERN S16BIT _DECL remoteAceGetDiscDir
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceGetDiscIn
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceSetDiscOut
(
    S16BIT DevNum,
    S16BIT Channel,
    DISC_LEVEL Level
);

_EXTERN S16BIT _DECL remoteAceGetDiscAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL remoteAceSetDiscAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

_EXTERN S16BIT _DECL remoteAcexSetDiscConfigure
(
    S16BIT s16DevNum,
    S16BIT s16Discrete,
    ACEX_DISC_CONFIG sDiscConfig
);

_EXTERN S16BIT _DECL remoteAcexClrDiscConfigure
(
    S16BIT s16DevNum
);

/*************************************************
       AIOR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceGetAioOut
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceSetAioDir
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_DIRECTION Direction
);

_EXTERN S16BIT _DECL remoteAceGetAioDir
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceGetAioIn
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL remoteAceSetAioOut
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_LEVEL Level
);

_EXTERN S16BIT _DECL remoteAceGetAioAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL remoteAceSetAioAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

/*************************************************
       TESTR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAceTestRegisters
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL remoteAceTestMemory
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wValue
);

_EXTERN S16BIT _DECL remoteAceTestIrqs
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL remoteAceTestLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL remoteAceTestProtocol
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

/*************************************************
     TRGR.C
**************************************************/
_EXTERN S16BIT _DECL remoteAcexTRGReset
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL remoteAcexTRGEnable
(
    S16BIT DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL remoteAcexTRGDisable
(
    S16BIT DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL remoteAcexTRGConfigure
(
    S16BIT DevNum,
    U16BIT u16TrgId,
    ACEX_TRG_CONFIG sTrgConfig
);

_EXTERN S16BIT _DECL remoteAcexTRGEventSelect
(
    S16BIT DevNum,
    U16BIT u16TrgId,
    U16BIT u16Events,
    U16BIT u16EvtInput
);

_EXTERN S16BIT _DECL remoteAcexTRGEventEnable
(
    S16BIT DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL remoteAcexTRGEventDisable
(
    S16BIT DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL remoteAcexTRGGetTimeTag
(
    S16BIT DevNum,
    U64BIT *pu64TimeTagValue
);

_EXTERN S16BIT _DECL remoteAcexTRGGetStatus
(
    S16BIT DevNum,
    U32BIT *pu32TrgStatus,
    U32BIT *pu32IrqStatus
);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef __DDC_SOCK_CLNT_LIB_H__ */
