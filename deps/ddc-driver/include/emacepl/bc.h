/*******************************************************************************
 * FILE: bc.h
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

#ifndef __BC_H__
#define __BC_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_BC_FUNCTIONS
{
    S16BIT (_DECL *aceBCConfigure)
    (
        S16BIT DevNum,
        U32BIT dwOptions
    );

    S16BIT (_DECL *aceBCDataBlkCreate)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT wDataBlkSize,
        U16BIT *pBuffer,
        U16BIT wBufferSize
    );

    S16BIT (_DECL *aceBCDataBlkDelete)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID
    );

    S16BIT (_DECL *aceBCDataBlkWrite)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U16BIT wOffset
    );

    S16BIT (_DECL *aceBCDataBlkRead)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U16BIT wOffset
    );

    S16BIT (_DECL *aceBCDataBlkRead32)
    (
        S16BIT DevNum,
        S16BIT nDataBlkID,
        U32BIT *pBuffer,
        U16BIT wBufferSize,
        U16BIT wOffset
    );

    S16BIT (_DECL *aceBCMsgCreateBCtoRT)
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

    S16BIT (_DECL *aceBCMsgCreateRTtoBC)
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

    S16BIT (_DECL *aceBCMsgCreateRTtoRT)
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

    S16BIT (_DECL *aceBCMsgCreateMode)
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

    S16BIT (_DECL *aceBCMsgCreateBcst)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        S16BIT nDataBlkID,
        U16BIT wSA,
        U16BIT wWC,
        U16BIT wMsgGapTime,
        U32BIT dwMsgOptions
    );

    S16BIT (_DECL *aceBCMsgCreateBcstRTtoRT)
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

    S16BIT (_DECL *aceBCMsgCreateBcstMode)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        S16BIT nDataBlkID,
        U16BIT wTR,
        U16BIT wModeCmd,
        U16BIT wMsgGapTime,
        U32BIT dwMsgOptions
    );

    S16BIT (_DECL *aceBCMsgCreate)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        S16BIT nDataBlkID1,
        U16BIT wBCCtrlWrd1,
        U16BIT wBCCmdWrd1_1,
        U16BIT wBCCmdWrd1_2,
        U16BIT wMsgGapTime1,
        S16BIT nDataBlkID2,
        U16BIT wBCCtrlWrd2,
        U16BIT wBCCmdWrd2_1,
        U16BIT wBCCmdWrd2_2,
        U16BIT wMsgGapTime2,
        U32BIT dwMsgOptions
    );

    S16BIT (_DECL *aceBCMsgModifyBCtoRT)
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

    S16BIT (_DECL *aceBCMsgModifyRTtoBC)
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

    S16BIT (_DECL *aceBCMsgModifyRTtoRT)
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

    S16BIT (_DECL *aceBCMsgModifyMode)
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

    S16BIT (_DECL *aceBCMsgModifyBcst)
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

    S16BIT (_DECL *aceBCMsgModifyBcstRTtoRT)
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

    S16BIT (_DECL *aceBCMsgModifyBcstMode)
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

    S16BIT (_DECL *aceBCMsgModify)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        S16BIT nDataBlkID1,
        U16BIT wBCCtrlWrd1,
        U16BIT wBCCmdWrd1_1,
        U16BIT wBCCmdWrd1_2,
        U16BIT wMsgGapTime1,
        S16BIT nDataBlkID2,
        U16BIT wBCCtrlWrd2,
        U16BIT wBCCmdWrd2_1,
        U16BIT wBCCmdWrd2_2,
        U16BIT wMsgGapTime2,
        U16BIT wModFlags
    );

    S16BIT (_DECL *aceBCMsgDelete)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID
    );

    S16BIT (_DECL *aceBCOpCodeCreate)
    (
        S16BIT DevNum,
        S16BIT nOpCodeID,
        U16BIT wOpCodeType,
        U16BIT wCondition,
        U32BIT dwParameter1,
        U32BIT dwParameter2,
        U32BIT dwReserved
    );

    S16BIT (_DECL *aceBCOpCodeDelete)
    (
        S16BIT DevNum,
        S16BIT nOpCodeID
    );

    S16BIT (_DECL *aceBCFrameCreate)
    (
        S16BIT DevNum,
        S16BIT nFrameID,
        U16BIT FrameType,
        S16BIT aOpCodeIDs[],
        U16BIT wOpCodeCount,
        U16BIT wMnrFrmTime,
        U16BIT wFlags
    );

    S16BIT (_DECL *acexBCFrameCreate)
    (
        S16BIT DevNum,
        S16BIT nFrameID,
        U16BIT FrameType,
        S16BIT aOpCodeIDs[],
        U16BIT wOpCodeCount,
        U32BIT dwMnrFrmTime,
        U16BIT wFlags
    );

    S16BIT (_DECL *aceBCFrameDelete)
    (
        S16BIT DevNum,
        S16BIT nFrameID
    );

    S16BIT (_DECL *aceBCStart)
    (
        S16BIT DevNum,
        S16BIT nMjrFrmID,
        S32BIT lMjrFrmCount
    );

    S16BIT (_DECL *aceBCStop)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCMsgGapTimerEnable)
    (
        S16BIT DevNum,
        U16BIT bEnable
    );

    S16BIT (_DECL *aceBCSetGPFState)
    (
        S16BIT DevNum,
        U16BIT wGPF,
        U16BIT wStateChange
    );

    S16BIT (_DECL *aceBCGetConditionCode)
    (
        S16BIT DevNum,
        U16BIT wConditionCode,
        U16BIT *pCurrentState
    );

    S16BIT (_DECL *aceBCSetWatchDogTimer)
    (
        S16BIT DevNum,
        U16BIT bEnable,
        U16BIT wTimeout
    );

    S16BIT (_DECL *aceBCGetMsgFromIDRaw)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        U16BIT *pBuffer,
        U16BIT bPurge
    );

    S16BIT (_DECL *acexBCGetMsgFromIDRawEx)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        U16BIT *pBuffer,
        U16BIT bPurge,
        U16BIT bBlock
    );

    S16BIT (_DECL *aceBCGetMsgHdrFromIDRaw)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        U16BIT *pBuffer,
        U16BIT bPurge
    );

    S16BIT (_DECL *acexBCGetMsgHdrFromIDRawEx)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        U16BIT *pBuffer,
        U16BIT bPurge,
        U16BIT bBlock
    );

    S16BIT (_DECL *aceBCGetMsgFromIDDecoded)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        MSGSTRUCT *pMsg,
        U16BIT bPurge
    );

    S16BIT (_DECL *acexBCGetMsgFromIDDecodedEx)
    (
        S16BIT DevNum,
        S16BIT nMsgBlkID,
        MSGSTRUCT *pMsg,
        U16BIT bPurge,
        U16BIT bBlock
    );

    S16BIT (_DECL *aceBCDecodeRawMsg)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        MSGSTRUCT *pMsg
    );

    S16BIT (_DECL *aceBCInstallHBuf)
    (
        S16BIT DevNum,
        U32BIT dwHBufSize
    );

    S16BIT (_DECL *aceBCUninstallHBuf)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCGetHBufMsgCount)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCGetHBufMsgsRaw)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U32BIT *pdwMSgCount,
        U32BIT *pdwMsgLostHBuf
    );

    S16BIT (_DECL *acexBCGetHBufMsgsRawEx)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U32BIT *pdwMSgCount,
        U32BIT *pdwMsgLostHBuf,
        U16BIT bBlock
    );

    S16BIT (_DECL *aceBCGetHBufMsgDecoded)
    (
        S16BIT DevNum,
        MSGSTRUCT *pMsg,
        U32BIT *pdwMSgCount,
        U32BIT *pdwMsgLostHBuf,
        U16BIT wMsgLoc
    );

    S16BIT (_DECL *acexBCGetHBufMsgDecodedEx)
    (
        S16BIT DevNum,
        U16BIT *pMsgId,
        MSGSTRUCT *pMsg,
        U32BIT *pdwMSgCount,
        U32BIT *pdwMsgLostHBuf,
        U16BIT wMsgLoc,
        U16BIT bBlock
    );

    S16BIT (_DECL *aceBCFrmToHBuf)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCFrmToHBuf32)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCGPQGetCount)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCGPQRead)
    (
        S16BIT DevNum,
        GPQENTRY *pGPQEntry
    );

    S16BIT (_DECL *aceBCSetMsgRetry)
    (
        S16BIT DevNum,
        U16BIT wNumOfRetries,
        U16BIT wFirstRetryBus,
        U16BIT wSecondRetryBus,
        U16BIT wReserved
    );

    S16BIT (_DECL *aceBCCreateImageFiles)
    (
        S16BIT DevNum,
        S16BIT nMjrFrmID,
        char *pszIFile,
        char *pszHFile
    );

    S16BIT (_DECL *aceBCGetHBufMetric)
    (
        S16BIT DevNum,
        HBUFMETRIC *pMetric,
        U16BIT bReset
    );

    S16BIT (_DECL *aceBCGetGPQMetric)
    (
        S16BIT DevNum,
        GPQMETRIC *pGPQMetric,
        U16BIT bReset
    );

    S16BIT (_DECL *aceBCAsyncMsgCreateBCtoRT)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateRTtoBC)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateRTtoRT)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateMode)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateBcst)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateBcstMode)
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

    S16BIT (_DECL *aceBCAsyncMsgCreateBcstRTtoRT)
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

    S16BIT (_DECL *aceBCSendAsyncMsgLP)
    (
        S16BIT DevNum,
        U16BIT *pMsgLeft,
        U16BIT wTimeFactor
    );

    S16BIT (_DECL *aceBCSendAsyncMsgHP)
    (
        S16BIT DevNum,
        U16BIT nMsgID,
        U16BIT wTimeFactor
    );

    S16BIT (_DECL *aceBCEmptyAsyncList)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCResetAsyncPtr)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCGetAsyncCount)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceBCWriteCanAddr)
    (
        S16BIT DevNum,
        U16BIT wRTAddr,
        U16BIT wCANBus
    );

    S16BIT (_DECL *aceBCDisableMessage)
    (
        S16BIT DevNum,
        U16BIT wMsgNum,
        BOOLEAN bDisable
    );

    S16BIT (_DECL *aceBCGetStatus)
    (
        S16BIT DevNum,
        U16BIT *wStatus
    );

    S16BIT (_DECL *aceBCExtTriggerEnable)
    (
        S16BIT s16DevNum,
        BOOLEAN bOnStart
    );

    S16BIT (_DECL *aceBCExtTriggerDisable)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCInitialize)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCFree)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCTimeSlotCreate)
    (
        S16BIT s16DevNum,
        U16BIT u16SlotTime,
        U16BIT u16SlotCount,
        U32BIT u16IrqMask
    );

    S16BIT (_DECL *acexBCTimeSlotItemCreate)
    (
        S16BIT s16DevNum,
        S16BIT s16SlotItemId,
        S16BIT s16MsgId,
        U16BIT u16SlotStart,
        U16BIT u16SlotInterval
    );

    S16BIT (_DECL *acexBCGetCurrTimeSlot)
    (
        S16BIT s16DevNum,
        U16BIT *pTimeSlot
    );

    S16BIT (_DECL *acexBCStart)
    (
        S16BIT s16DevNum,
        S16BIT s16MjrFrmID,
        S32BIT s32CycleCount,
        U16BIT u16Type
    );

    S16BIT (_DECL *acexBCDataStreamCreateBCRT)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        U16BIT u16RTAddr,
        U32BIT u32SAMask,
        U32BIT u32Options
    );

    S16BIT (_DECL *acexBCDataStreamCreateRTRT)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        U16BIT u16RTAddr1,
        U32BIT u32SAMask1,
        U16BIT u16RTAddr2,
        U32BIT u32SAMask2,
        U32BIT u32Options
    );

    S16BIT (_DECL *acexBCDataStreamDelete)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId
    );

    S16BIT (_DECL *acexBCDataStreamSend)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        U16BIT u16Priority,
        VOID *pBuffer,
        U16BIT u16BufferLen,
        U32BIT u32TimeoutMs
    );

    S16BIT (_DECL *acexBCDataStreamReceive)
    (
        S16BIT s16DevNum,
        S16BIT s16DataStrId,
        U16BIT u16Priority,
        VOID *pBuffer,
        U16BIT u16BufferLen,
        U32BIT u32TimeoutMs
    );

    S16BIT (_DECL *acexBCDataArrayCreateBCtoRT)
    (
        S16BIT s16DevNum,
        S16BIT s16DataArrayId,
        U16BIT u16RTAddr,
        U16BIT u16RTSubAddr,
        U16BIT u16Transfer,
        U16BIT u16WordCount,
        U32BIT u32Options
    );

    S16BIT (_DECL *acexBCDataArrayDelete)
    (
        S16BIT s16DevNum,
        S16BIT s16DataArrayId
    );

    S16BIT (_DECL *acexBCDataArraySend)
    (
        S16BIT s16DevNum,
        S16BIT s16DataArrayId,
        U16BIT u16Priority,
        VOID *pBuffer,
        U16BIT u16BufferLen,
        U16BIT u16WordCount
    );

    S16BIT (_DECL *acexBCAsyncMsgSendLP)
    (
        S16BIT s16DevNum,
        U16BIT s16MsgId,
        BOOLEAN bFlushMsg,
        U16BIT *pu16QueueCount
    );

    S16BIT (_DECL *acexBCAsyncMsgSendHP)
    (
        S16BIT s16DevNum,
        U16BIT s16MsgId,
        BOOLEAN bFlushMsg,
        U16BIT *pu16QueueCount
    );

    S16BIT (_DECL *acexBCAsyncQueueInfoLP)
    (
        S16BIT s16DevNum,
        ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
    );

    S16BIT (_DECL *acexBCAsyncQueueInfoHP)
    (
        S16BIT s16DevNum,
        ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
    );


    S16BIT (_DECL *acexBCMemWrdCreate)
    (
        S16BIT s16DevNum,
        S16BIT s16MemWrdId,
        U32BIT u32MemWord
    );

    S16BIT (_DECL *acexBCMemWrdDelete)
    (
        S16BIT s16DevNum,
        S16BIT s16MemWrdId
    );

    S16BIT (_DECL *acexBCMemWrdRead)
    (
        S16BIT s16DevNum,
        S16BIT s16MemWrdId,
        U32BIT *pu32MemWord
    );

    S16BIT (_DECL *acexBCMemWrdWrite)
    (
        S16BIT s16DevNum,
        S16BIT s16MemWrdId,
        U32BIT u32MemWord
    );

    S16BIT (_DECL *acexBCMemObjCreate)
    (
        S16BIT s16DevNum,
        S16BIT s16MemObjId,
        S16BIT s16MemItemId,
        U16BIT u16MemItemType,
        U16BIT u16Offset
    );

    S16BIT (_DECL *acexBCMemObjDelete)
    (
        S16BIT s16DevNum,
        S16BIT s16MemObjId
    );

    S16BIT (_DECL *acexBCSetMsgError)
    (
        S16BIT s16DevNum,
        S16BIT s16MsgID,
        ACEX_ERR_INJ *psError
    );

    S16BIT (_DECL *acexBCMsgErrorEnable)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCMsgErrorDisable)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCSetRespTimeout)
    (
        S16BIT s16DevNum,
        U32BIT u32Multiple
    );

    S16BIT (_DECL *acexBCStartReplay)
    (
        S16BIT s16DevNum,
        const CHAR *pFileName,
        S32BIT lLoopCount
    );

    S16BIT (_DECL *acexBCStartReplayRaw)
    (
        S16BIT s16DevNum,
        PVOID *pReplayData,
        U32BIT u32DataLength
    );

    S16BIT (_DECL *acexBCConfigureReplay)
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

    S16BIT (_DECL *acexBCGetStatusReplay)
    (
        S16BIT s16DevNum,
        U32BIT *u32Status
    );

    S16BIT (_DECL *acexBCOpCodeRead)
    (
        S16BIT s16DevNum,
        S16BIT s16OpCodeID,
        U32BIT *pu32OpCode,
        U32BIT *pu32Paramk
    );

    S16BIT (_DECL *acexBCOpCodeWrite)
    (
        S16BIT s16DevNum,
        S16BIT s16OpCodeID,
        U16BIT u16Selection,
        U32BIT u32OpCode,
        U32BIT u32Param
    );

    S16BIT (_DECL *acexBCDbcEnable)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCDbcDisable)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCImrTrigSelect)
    (
        S16BIT s16DevNum,
        U16BIT u16Select
    );

    S16BIT (_DECL *acexBCPause)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexBCContinue)
    (
        S16BIT s16DevNum
    );

} API_BC_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceBCConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL aceBCMsgGapTimerEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL aceBCDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL aceBCDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL aceBCDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL aceBCDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL aceBCDataBlkRead32
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U32BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL aceBCFrameCreate
(
    S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime,
    U16BIT wFlags
);

_EXTERN S16BIT _DECL acexBCFrameCreate
(
    S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U32BIT dwMnrFrmTime,
    U16BIT wFlags
);

_EXTERN S16BIT _DECL aceBCFrameDelete
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN S16BIT _DECL aceBCOpCodeCreate
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL aceBCOpCodeDelete
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL aceBCMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL aceBCMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL aceBCMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL aceBCMsgCreateMode
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

_EXTERN S16BIT _DECL aceBCMsgCreateBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL aceBCMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL aceBCMsgCreateBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL aceBCMsgCreate
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

_EXTERN S16BIT _DECL aceBCMsgModifyBCtoRT
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

_EXTERN S16BIT _DECL aceBCMsgModifyRTtoBC
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

_EXTERN S16BIT _DECL aceBCMsgModifyRTtoRT
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

_EXTERN S16BIT _DECL aceBCMsgModifyMode
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

_EXTERN S16BIT _DECL aceBCMsgModifyBcst
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

_EXTERN S16BIT _DECL aceBCMsgModifyBcstRTtoRT
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

_EXTERN S16BIT _DECL aceBCMsgModifyBcstMode
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

_EXTERN S16BIT _DECL aceBCMsgModify
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

_EXTERN S16BIT _DECL aceBCMsgDelete
(
    S16BIT DevNum,
    S16BIT nMsgBlkID
);

_EXTERN S16BIT _DECL aceBCStart
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    S32BIT lMjrFrmCount
);

_EXTERN S16BIT _DECL aceBCStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCCreateImageFiles
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL aceBCSetGPFState
(
    S16BIT DevNum,
    U16BIT wGPF,
    U16BIT wStateChange
);

_EXTERN S16BIT _DECL aceBCGetConditionCode
(
    S16BIT DevNum,
    U16BIT wConditionCode,
    U16BIT* pCurrentState
);

_EXTERN S16BIT _DECL aceBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL acexBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlock
);

_EXTERN S16BIT _DECL aceBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL acexBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlock
);

_EXTERN S16BIT _DECL aceBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL acexBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge,
    U16BIT bBlock
);

_EXTERN S16BIT _DECL aceBCDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL aceBCInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL aceBCUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCGPQRead
(
    S16BIT DevNum,
    GPQENTRY* pGPQEntry
);

_EXTERN S16BIT _DECL aceBCGPQGetCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCFrmToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCFrmToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL acexBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT bBlock
);

_EXTERN S16BIT _DECL aceBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL acexBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    U16BIT *pMsgId,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc,
    U16BIT bBlock
);

_EXTERN S16BIT _DECL aceBCSetMsgRetry
(
    S16BIT DevNum,
    U16BIT wNumOfRetries,
    U16BIT wFirstRetryBus,
    U16BIT wSecondRetryBus,
    U16BIT wReserved
);

_EXTERN S16BIT _DECL aceBCSetWatchDogTimer
(
    S16BIT DevNum,
    U16BIT bEnable,
    U16BIT wTimeOut
);

_EXTERN S16BIT _DECL aceBCGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL aceBCGetGPQMetric
(
    S16BIT DevNum,
    GPQMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateMode
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateBcst
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL aceBCAsyncMsgCreateBcstMode
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

_EXTERN S16BIT _DECL aceBCSendAsyncMsgLP
(
    S16BIT DevNum,
    U16BIT *pMsgLeft,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL aceBCSendAsyncMsgHP
(
    S16BIT DevNum,
    U16BIT nMsgID,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL aceBCEmptyAsyncList
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCResetAsyncPtr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCGetAsyncCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceBCWriteCanAddr
(
    S16BIT DevNum,
    U16BIT wRTAddr,
    U16BIT wCANBus
);

_EXTERN S16BIT _DECL aceBCDisableMessage
(
    S16BIT DevNum,
    U16BIT wMsgNum,
    BOOLEAN bDisable
);

_EXTERN S16BIT _DECL aceBCGetStatus
(
    S16BIT DevNum,
    U16BIT *wStatus
);

_EXTERN S16BIT _DECL aceBCExtTriggerEnable
(
    S16BIT s16DevNum,
    BOOLEAN bOnStart
);

_EXTERN S16BIT _DECL aceBCExtTriggerDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCInitialize
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL acexBCFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL acexBCTimeSlotCreate
(
    S16BIT DevNum,
    U16BIT u16SlotTime,
    U16BIT u16SlotCount,
    U32BIT u16IrqMask
);

_EXTERN S16BIT _DECL acexBCTimeSlotItemCreate
(
    S16BIT DevNum,
    S16BIT s16SlotItemId,
    S16BIT s16MsgId,
	U16BIT u16SlotStart,
    S16BIT u16SlotInterval
);

_EXTERN S16BIT _DECL acexBCGetCurrTimeSlot
(
    S16BIT DevNum,
    U16BIT *pTimeSlot
);

_EXTERN S16BIT _DECL acexBCDataStreamCreateBCRT
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr,
    U32BIT u32SAMask,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL acexBCDataStreamCreateRTRT
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr1,
    U32BIT u32SAMask1,
    U16BIT u16RTAddr2,
    U32BIT u32SAMask2,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL acexBCDataStreamDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId
);

_EXTERN S16BIT _DECL acexBCDataStreamSend
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL acexBCDataStreamReceive
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL acexBCDataArrayCreateBCtoRT
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16RTAddr,
    U16BIT u16RTSubAddr,
    U16BIT u16Transfer,
    U16BIT u16WordCount,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL acexBCDataArrayDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId
);

_EXTERN S16BIT _DECL acexBCDataArraySend
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U16BIT u16WordCount
);

_EXTERN S16BIT _DECL acexBCAsyncMsgSendLP
(
    S16BIT s16DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL acexBCAsyncMsgSendHP
(
    S16BIT s16DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL acexBCAsyncQueueInfoLP
(
    S16BIT s16DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL acexBCAsyncQueueInfoHP
(
    S16BIT s16DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL acexBCMemWrdCreate
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL acexBCMemWrdDelete
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId
);

_EXTERN S16BIT _DECL acexBCMemWrdRead
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT *pu32MemWord
);

_EXTERN S16BIT _DECL acexBCMemWrdWrite
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL acexBCMemObjCreate
(
    S16BIT s16DevNum,
    S16BIT s16MemObjId,
    S16BIT s16MemItemId,
    U16BIT u16MemItemType,
    U16BIT u16Offset
);

_EXTERN S16BIT _DECL acexBCMemObjDelete
(
    S16BIT s16DevNum,
    S16BIT s16MemObjId
);

_EXTERN S16BIT _DECL acexBCSetMsgError
(
    S16BIT s16DevNum,
    S16BIT s16MsgID,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL acexBCMsgErrorEnable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCMsgErrorDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCSetRespTimeout
(
    S16BIT s16DevNum,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL acexBCStartReplay
(
    S16BIT DevNum,
    const CHAR *pFileName,
    S32BIT lLoopCount
);

_EXTERN S16BIT _DECL acexBCStartReplayRaw
(
    S16BIT DevNum,
    PVOID *pReplayData,
    U32BIT u32DataLength
);

_EXTERN S16BIT _DECL acexBCConfigureReplay
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

_EXTERN S16BIT _DECL acexBCGetStatusReplay
(
    S16BIT s16DevNum,
    U32BIT  *u32Status
);

_EXTERN S16BIT _DECL acexBCOpCodeRead
(
    S16BIT s16DevNum,
    S16BIT s16OpCodeID,
    U32BIT *pu32OpCode,
    U32BIT *pu32Param
);

_EXTERN S16BIT _DECL acexBCOpCodeWrite
(
    S16BIT s16DevNum,
    S16BIT s16OpCodeID,
    U16BIT u16Selection,
    U32BIT u32OpCode,
    U32BIT u32Param
);

_EXTERN S16BIT _DECL acexBCDbcEnable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCDbcDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCImrTrigSelect
(
    S16BIT s16DevNum,
    U16BIT u16Select
);

_EXTERN S16BIT _DECL acexBCPause
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexBCContinue
(
    S16BIT s16DevNum
);

#endif /* __BC_H__ */
