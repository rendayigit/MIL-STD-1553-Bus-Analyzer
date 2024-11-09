/*****************************************************************************
*                       Data Device Corporation
*                          105 Wilbur Place
*                         Bohemia N.Y. 11716
*
*       AceXtreme 'C' Run Time Library
*
*       Copyright (c) 2018 Data Device Corporation
*       All Rights Reserved.
*
*       Filename: devicex_common.h
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
*
*    Description:
*       This is the include file for devicex_common.c
*
*
******************************************************************************/

#ifndef __DEVICEX_COMMON_H__
#define __DEVICEX_COMMON_H__

#include "include/ddc_types.h"

#define RES_RELEASE_WAIT_TIME    500        /* in ms */

/* todo - move to os inc */
/*#include <semaphore.h>*/
typedef long LONG;
typedef unsigned long ULONG;


/* ========================================================================== */
/*                                GENERAL STRUCTURES                         */
/* ========================================================================== */
typedef struct _ACEX_DRV_CONTEXT
{
    DDC_IOCTL_SEMAPHORE ioctlDispatchSemaphore;   /* used for synchronization between library and ISR */
    DDC_IOCTL_SEMAPHORE intInterruptProcSemaphore;

    DDC_HANDLE       hDriverApp;           /* Handle to Linux device */
    DDC_HANDLE       hDriverIrq;
    DDC_HANDLE       hDriverMtiData;
    DDC_HANDLE       hDriverMtiTime;
    DDC_HANDLE       hDriverReplay;
    U32BIT           u32InterruptQueCount; /* Count of interrupt pending queue */
    U32BIT           u32InterruptQueHead;  /* Head of interrupt pending queue */
    U32BIT           u32InterruptQueTail;  /* Tail of interrupt pending queue */
    DDC_IRQ_STATUS_TYPE *psIrqStatus;      /* Array of interrupt status(s) for IQS */
    DDC_THREAD_HANDLE thDispatch;           /* Interrupt thread (Dispatch) */
    DDC_THREAD_HANDLE thReplay;             /* Interrupt thread (Replay */   
    DDC_THREAD_HANDLE thWorker;             /* Interrupt thread (Worker) */
    S16BIT           s16ThParam;           /* Parameter for new threads */
    DDC_CONDITION    thcQue;               /* Interrupt que condition object */
    DDC_MUTEX        thmQue;               /* Interrupt que mutex */
    DDC_THREAD_ID    thDispatchID;
    DDC_THREAD_ID    thWorkerID;
    DDC_THREAD_ID    thReplayID;
} ACEX_DRV_CONTEXT;

typedef struct _ACEX_OSCONTEXT
{
   DDC_HANDLE  hDriverApp;
   DDC_HANDLE  hDriverIrq;
   DDC_HANDLE  hDriverMtiData;
   DDC_HANDLE  hDriverMtiTime;
   DDC_HANDLE  hDriverReplay;
   DDC_HANDLE  hBlockEvent;
   DDC_HANDLE  hExitEvent;
   DDC_HANDLE  hThread[3];
   U32BIT      dwThreadID[3];
} ACEX_OSCONTEXT;

typedef struct _ACEX_IOINFO
{
    DDC_IOCTL_SEMAPHORE ioctlDispatchSemaphore;   /* used for synchronization between library and ISR */
    DDC_IOCTL_SEMAPHORE intInterruptProcSemaphore;

    U16BIT          bInitialized;       /* Initialization status */
    ACEX_OSCONTEXT  *pContext;          /* OS specific information */
    U32BIT          dwCardType;         /* The card being accessed */
    DDC_HANDLE      hDriverLib;         /* Driver handle for library */
    CHANCOUNT_t     strChanCount;       /* structure that holds number of IO's */
    void            (_DECL *pfIOIsr)(S16BIT s16DevNum, U32BIT u32IntSts); /* ISR */

    /* DDCCM information */
    U16BIT u16DeviceNum;
    U16BIT u16DeviceType;
    unsigned char channel;
    char sDeviceName[DDC_MAX_DEVICE_NAME_LEN];

}ACEX_IOINFO, *PACEX_IOINFO;


typedef struct _ACEX_COUPLING
{
    U32BIT coupling;
    U32BIT termination;

} ACEX_COUPLING;


/* ========================================================================== */
/*                                GENERAL DEFINITIONS                         */
/* ========================================================================== */
/* IOCTL failure macro */
#define ACEX_IOCTL_FAILURE       0

/* MRT Improvements IOCTL Command Code */
#define IOCTL_IMP_MRT_CMD                 0

/* Generic time delay */
#define WAIT_TIMEOUT_MAX_MS      1000

/* 1553 IOCTL ACCESS TYPES */
#define REG_TYPE_ABSOLUTE_ADDR   0
#define REG_TYPE_BD_OFFSET       1
#define REG_TYPE_BC_OFFSET       2
#define REG_TYPE_MRT_OFFSET      3
#define REG_TYPE_MTI_OFFSET      4
#define REG_TYPE_1553_OFFSET     5
#define REG_TYPE_EI_OFFSET       6
#define REG_TYPE_REPLAY_OFFSET   7
#define REG_TYPE_TRG_OFFSET      8

#define ACEX_16_BIT_ACCESS 0
#define ACEX_32_BIT_ACCESS -1

#define IOCTL_16_BIT_ACCESS 0
#define IOCTL_32_BIT_ACCESS 1

#if defined (PPC) || defined( __powerpc__ )
    #define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -2
#else
#define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -1
#endif

/* Dma Cache Line Size Padding Macro - 32 bytes accommodates all possible bus widths */
#define DMA_CACHE_LINE_PAD    32

/* ========================================================================== */
/*                         BUS CONTROLLER DEFINITIONS                         */
/* ========================================================================== */

/*  dwIrqMask parameter can be the following OR'd together */
#define ACE_IMR1_EOM                 0x00000001
#define ACE_IMR1_BC_STATUS_SET        0x00000002
#define ACE_IMR1_RT_MODE_CODE        0x00000002
#define ACE_IMR1_MT_PATTERN_TRIG    0x00000002
#define ACE_IMR1_FORMAT_ERR         0x00000004
#define ACE_IMR1_BC_END_OF_FRM        0x00000008
#define ACE_IMR1_BC_MSG_EOM         0x00000010
#define ACE_IMR1_RT_SUBADDR_EOM     0x00000010
#define ACE_IMR1_RT_CIRCBUF_ROVER    0x00000020
#define ACE_IMR1_TT_ROVER            0x00000040
#define ACE_IMR1_RT_ADDR_PAR_ERR    0x00000080
#define ACE_IMR1_BC_RETRY            0x00000100
#define ACE_IMR1_HSHAKE_FAIL        0x00000200
#define ACE_IMR1_MT_DATASTK_ROVER    0x00000400
#define ACE_IMR1_MT_CMDSTK_ROVER    0x00000800
#define ACE_IMR1_BCRT_CMDSTK_ROVER    0x00001000
#define ACE_IMR1_BCRT_TX_TIMEOUT    0x00002000
#define ACE_IMR1_RAM_PAR_ERR        0x00004000
#define ACE_IMR2_IRIG_INTERRUPT     0x00008000
#define ACE_IMR2_BIT_TRIGGER        0x00010000
#define ACE_IMR2_BIT_COMPLETE        0x00020000
#define ACE_IMR2_FLEX_DMA_ENABLE     0x00020000
#define ACE_IMR2_BC_UIRQ0            0x00040000
#define ACE_IMR2_BC_UIRQ1            0x00080000
#define ACE_IMR2_BC_UIRQ2            0x00100000
#define ACE_IMR2_BC_UIRQ3            0x00200000
#define ACE_IMR2_MT_DSTK_50P_ROVER    0x00400000
#define ACE_IMR2_MT_CSTK_50P_ROVER    0x00800000
#define ACE_IMR2_RT_CIRC_50P_ROVER    0x01000000
#define ACE_IMR2_RT_CSTK_50P_ROVER    0x02000000
#define ACE_IMR2_BC_TRAP            0x04000000
#define ACE_IMR2_BC_CALLSTK_ERR     0x08000000
#define ACE_IMR2_GPQ_ISQ_ROVER        0x10000000
#define ACE_IMR2_RT_ILL_CMD         0x20000000
#define ACE_IMR2_BC_OPCODE_PARITY    0x40000000
#define ACE_IMR2_MTI_INTERRUPT      0x80000000

/* ========================================================================== */
/*                         BUS CONTROLLER DEFINITIONS                         */
/* ========================================================================== */

/* ========================================================================== */
/*                         REMOTE TERMINAL DEFINITIONS                        */
/* ========================================================================== */

/* ========================================================================== */
/*                      MULTI REMOTE TERMINAL DEFINITIONS                     */
/* ========================================================================== */
/* ========================================================================== */
/*                          BUS MONITIOR DEFINITIONS                          */
/* ========================================================================== */

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

/*
  Windows needs multiple driver Handles to support
  blocking IRQ access to driver. Function returns
  proper driver handle. This call must be taylored
  for each OS.
*/
DDC_HANDLE ddcGetDrvHandle
(
    S16BIT s16DevNum,
    U32BIT u32DrvHandle
);

S16BIT _DECL _dvxChannelInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

void _DECL _dvxRegRead
(
    S16BIT s16DevNum,
    U32BIT u32RegAddrOffset,
    U32BIT *pu32Data,
    U32BIT u32NumBytes,
    U32BIT u32Type
);

void _DECL _dvxRegWrite
(
    S16BIT s16DevNum,
    U32BIT u32RegAddrOffset,
    U32BIT *pu32Data,
    U32BIT u32NumBytes,
    U32BIT u32Type
);

void _DECL _dvxMemRead
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    S32BIT s32WdOffset, /* >=0 for 16-bit access, -1 for 32-bit access */
    void        *pData,
    U32BIT u32NumBytes  /* number of bytes to read */
);

void _DECL _dvxMemWrite
(
    S16BIT s16DevNum,
    U32BIT u32MemAddr,
    S32BIT s32WdOffset, /* >=0 for 16-bit access */
    void        *pData,
    U32BIT u32NumBytes  /* number of bytes to read */
);

S16BIT _DECL _dvxBlkRead
(
    S16BIT s16DevNum,
    U16BIT u16Chnl,
    U32BIT u32RegBA,
    U32BIT *pu32Data,
    U32BIT u32DataByteLength,
    U8BIT       u8Type);

S16BIT _DECL _dvxBlkWrite
(
    S16BIT s16DevNum,
    U16BIT u16Chnl,
    U32BIT u32BA,
    U32BIT *pu32Data,
    U32BIT u32DataByteLength,
    U8BIT       u8Type
);

S16BIT _DECL _dvxConfigureIoInterruptConditions
(
    S16BIT s16DevNum,
    U16BIT u16Command,
    ACEX_IO_CONFIGURE_TYPE *pAioConfigure
);

S32BIT _DECL _dvxMrtRead
(
    S16BIT s16DevNum,
    U16BIT u16Chnl,
    U16BIT RtAddrNumCmds,
    U16BIT RtSAddr,
    U32BIT *pu32Data,
    U32BIT u32DataByteLength,
    U8BIT       u8Type
);

S16BIT _DECL _dvxMrtReadModeCodeData
(
    S16BIT s16DevNum,
    S8BIT  s8RtAddr,
    U16BIT wModeCode,
    U16BIT *pMCData

);

S16BIT _DECL _dvxMrtWriteModeCodeData
(
    S16BIT s16DevNum,
    S8BIT  s8RtAddr,
    U16BIT wModeCode,
    U16BIT MCData
);

S32BIT _DECL _dvxMrtControl
(
    S16BIT s16DevNum,
    void        *pSnd,
    U32BIT u32SndByteSize,
    void        *pRcv,
    U32BIT u32RcvByteSize,
    U32BIT *pu32Length
);

S16BIT _DECL _dvxMrtSetRespTimeout
(
    S16BIT s16DevNum,
    S8BIT  s8RtAddr,
    U32BIT u32RespTimeOut
);

S16BIT _DECL _dvxMrtSendStream
(
    S16BIT s16DevNum,
    ACEX_MRT_STREAM_TYPE    *pMrtStream
);

S16BIT _DECL _dvxMrtReceiveStream
(
    S16BIT s16DevNum,
    ACEX_MRT_STREAM_TYPE    *pMrtStream
);

U32BIT _DECL _dvxFlashMemRead
(
    S16BIT s16DevNum,
    U8BIT        u8Chnl,
    U32BIT u32MemAddr
);

VOID _DECL _dvxFlashMemWrite
(
    S16BIT s16DevNum,
    U8BIT        u8Chnl,
    U32BIT u32MemAddr,
    U32BIT u32MemData,
    char        *dataBuffer
);

VOID _DECL _dvxFlashMemErase
(
    S16BIT s16DevNum,
    U8BIT        u8Chnl,
    U32BIT u32MemAddr
);

S16BIT _DECL _dvxGetHwVersionInfo
(
    S16BIT s16DevNum,
    PHWVERSIONINFO pHwVersionInfo
);

VOID _DECL _dvxGetTimeTagValue(
                                S16BIT s16DevNum,
                                U64BIT    *ullTTValue);

VOID _DECL _dvxSetTimeTagValue(
                                S16BIT s16DevNum,
                                U64BIT    ullTTValue);

S16BIT _DECL _dvxExternalTTCntCtrl(S16BIT s16DevNum, U16BIT u16ExtCtrl);

S16BIT _DECL _dvxMtiInit
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxSetMtiConfig
(
    S16BIT s16DevNum,
    ACEX_MTI_CONFIG *pMtiConfig
);

S16BIT _DECL _dvxSetMtiState
(
    S16BIT s16DevNum,
    U16BIT u16State
);

S16BIT _DECL acexMTiFree
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxSetMtiRespTimeOut
(
    S16BIT s16DevNum,
    U32BIT u32RespTimeOut
);

S16BIT _DECL _dvxSetBCRespTimeOut
(
    S16BIT s16DevNum,
    U32BIT u32RespTimeOut
);

S16BIT _DECL _dvxSetIRQCond
(
    S16BIT s16DevNum,
    U16BIT bEnable,
    U32BIT dwIrqMask
);

S16BIT _DECL _dvxSetTimeTagRes
(
    S16BIT s16DevNum,
    U16BIT u16TTRes
);

S16BIT _DECL _dvxSetTimeTagRolloverPoint
(
    S16BIT s16DevNum,
    U16BIT u16TTRolloverPoint
);

S16BIT _DECL _dvxSetRamParityChecking
(
    S16BIT s16DevNum,
    BOOLEAN bEnable
);

S16BIT _DECL _dvxGetMtiCh10DataPkt
(
    S16BIT s16DevNum,
    PMTI_CH10_DATA_PKT    *ppCh10Pkt,
    S16BIT s16Timeout
);

S16BIT _DECL _dvxGetMtiCh10TimePkt
(
    S16BIT s16DevNum,
    PMTI_CH10_TIME_PKT    *ppCh10Pkt,
    S16BIT s16Timeout
);

S16BIT _DECL _dvxMTICh10TimePktEnable
(
    S16BIT s16DevNum,
    BOOLEAN bEnable
);

S16BIT _DECL _dvxSetMtiCh10TimePktFree
(
    S16BIT s16DevNum,
    PMTI_CH10_TIME_PKT    *ppCh10Pkt
);

S16BIT _DECL _dvxSetMTiStrobeReg
(
    S16BIT s16DevNum,
    U32BIT u32MtiFunction
);

S16BIT _DECL _dvxGetMtiCh10Status
(
    S16BIT s16DevNum,
    ACEX_MTI_INFO    *pUmtMtiInfo
);

S16BIT _DECL _dvxSetModuleInterruptState
(
    S16BIT s16DevNum,
    BOOLEAN bEnable,
    U32BIT u32Mask
);

S16BIT _DECL _dvxSetMtiStrobeReg
(
    S16BIT s16DevNum,
    U32BIT u32MtiFunction
);

S16BIT _DECL _dvxMtiFreeMemCount
(
    S16BIT s16DevNum,
    U32BIT u32WordCount
);

S16BIT _DECL _dvxMTIFreeConfig
(
    S16BIT s16DevNum,
    ACEX_MTI_CONFIG    *pMtiConfig
);

S16BIT _DECL _dvxMtiInterruptConfig
(
    S16BIT s16DevNum,
    ACEX_MTI_CONFIG *pMtiConfig
);

S16BIT _DECL _dvxBcInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcExtTriggerAvail
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT *pu32Avail
);

S16BIT _DECL _dvxBcDataArrayInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16DataStrId,
    U16BIT u16MsgCountSend
);

S16BIT _DECL _dvxBcDataArrayStatus
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16DataArrayId,
    ACEX_BC_DATA_ARRAY_STATUS    *pStatus
);

S16BIT _DECL _dvxBcDataStreamInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16DataStrId,
    U16BIT u16MsgCountSend,
    U16BIT u16MsgCountReceive
);

S16BIT _DECL _dvxBcDataStreamStatus
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16DataStrId,
    U16BIT u16Direction,
    ACEX_BC_DATASTR_STATUS    *pStatus
);

S16BIT _DECL _dvxBcConfig
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcFree
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcGetAsyncCount
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcGetDataBuffer
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT u16MsgId,
    U32BIT u32Size,
    U16BIT *pBuffer,
    U16BIT u16Offset
);

S16BIT _DECL _dvxBcCmdStackRead
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32MemAddr,
    U16BIT *pBuffer,
    U16BIT u16Length
);

S16BIT _DECL _dvxBcDataStackRead
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32MemAddr,
    U16BIT *pBuffer,
    U16BIT u16Length,
    U16BIT u16Offset
);

S16BIT _DECL _dvxBcGetFrameToHBuf
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcGetMessage
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT u16MsgId,
    U16BIT u16Type,
    U16BIT bCheckingImp,
    U16BIT bBlocking,
    U16BIT bPurge,
    U16BIT *pBuffer
);

S16BIT _DECL _dvxBcGpqClear
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcGpqCount
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT *pu16Count
);

S16BIT _DECL _dvxBcGpqInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32BaseAddr,
    U32BIT u32QueueSize
);

S16BIT _DECL _dvxBcGpqGetMetrics
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT u16Reset,
    GPQMETRIC    *pMetrics

);

S16BIT _DECL _dvxBcGpqRead
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    GPQENTRY32    *pGpqEntry
);

S16BIT _DECL _dvxBcHBufInstall
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32Size
);

S16BIT _DECL _dvxBcHBufUninstall
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxBcHBufGetMessage
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT bReturnMsgId,
    U16BIT u16Type,
    U16BIT bBlocking,
    U16BIT bCheckingImp,
    U32BIT u32Size,
    U16BIT *pBuffer,
    U32BIT *pMsgCount
);

S16BIT _DECL _dvxBcHBufGetMetrics
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT u16Reset,
    HBUFMETRIC    *pMetrics
);

S16BIT _DECL _dvxBcHpqInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32BaseAddr,
    U32BIT u32QueueSize
);

S16BIT _DECL _dvxBcLpqInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32BaseAddr,
    U32BIT u32QueueSize
);

S16BIT _DECL _dvxBcReplayInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32BaseAddr,
    U32BIT u32MemSize
);

S16BIT _DECL _dvxBcChanMuxInit
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32MuxEnable
);

S16BIT _DECL _dvxBcPostLpAsyncMessage
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32Addr,
    U16BIT u16MsgTime,
    U16BIT u16MsgType,
    U16BIT u16MsgId,
    BOOLEAN bMsgIrq,
    U16BIT *pu16Count
);

S16BIT _DECL _dvxBcPostHpAsyncMessage
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32Addr,
    U16BIT u16MsgType,
    U16BIT u16MsgId,
    BOOLEAN bMsgIrq,
    U16BIT *pu16Count
);

S16BIT _DECL _dvxBcAsyncQueueInfo
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT u32QueueType,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

S16BIT _DECL _dvxBcState
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U16BIT u16State
);

S16BIT _DECL _dvxBcSetDataBuffer
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16DataIndex,
    U16BIT u16Size,
    U16BIT *pBuffer,
    U16BIT u16Offset
);

S16BIT _DECL _dvxBcSetMsgBuffer
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    S16BIT s16MsgIndex,
    U16BIT u16Size,
    U32BIT *pBuffer
);

S16BIT _DECL _dvxEnableTimeTagInt
(
    S16BIT s16DevNum,
    S16BIT mask
);

DISC_LEVEL _DECL _dvxGetDiscOut
(
    S16BIT s16DevNum,
    S16BIT Discrete
);

S16BIT _DECL _dvxGetDiscDir
(
    S16BIT s16DevNum,
    S16BIT Discrete
);

DISC_LEVEL _DECL _dvxGetDiscIn
(
    S16BIT s16DevNum,
    S16BIT Discrete
);

S16BIT _DECL _dvxSetDiscOut
(
    S16BIT s16DevNum,
    S16BIT Discrete,
    DISC_LEVEL    Level
);

S16BIT _DECL _dvxSetDiscDir
(
    S16BIT s16DevNum,
    S16BIT Discrete,
    DISC_DIR    Direction
);

S16BIT _DECL _dvxGetDiscAll
(
    S16BIT s16DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

S16BIT _DECL _dvxSetDiscAll
(
    S16BIT s16DevNum,
    U16BIT Directions,
    U16BIT Levels
);

S16BIT _DECL _dvxWait
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT Milliseconds
);

S16BIT _DECL _dvxWaitCancel
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

U16BIT _DECL _dvAxRegRead
(
    S16BIT s16DevNum,
    U16BIT wRegAddr,
    U16BIT wMask
);

VOID _DECL _dvAxRegWrite
(    S16BIT s16DevNum,
    U16BIT wRegAddr,
    U16BIT wValue,
    U16BIT wMask
);

U16BIT _DECL _dvAxMemRead
(
    S16BIT s16DevNum,
    U32BIT dwMemAddr
);

S16BIT _DECL _dvAxMemWrite
(
    S16BIT s16DevNum,
    U32BIT dwMemAddr,
    U16BIT wValue
);

S16BIT _DECL _dvxClearMemory
(
    S16BIT s16DevNum,
    U32BIT u32Address16,   /* 16bit address */
    U32BIT u32NumWds       /* number of 16 bit wds */
);

S16BIT _DECL _dvxGetCoupling
(
    S16BIT s16DevNum,
    CHANNEL_COUPLING    *eCoupling,
    CHANNEL_TERMINATION *eTermination
);

S16BIT _DECL _dvxSetCoupling
(
    S16BIT s16DevNum,
    CHANNEL_COUPLING    eCoupling,
    CHANNEL_TERMINATION eTermination
);

S16BIT _DECL _dvxGetAmplitude
(
    S16BIT s16DevNum,
    U32BIT *pu32Amplitude
);

S16BIT _DECL _dvxSetAmplitude
(
    S16BIT s16DevNum,
    U32BIT u32Amplitude
);

S16BIT _DECL _dvxGetBoardStatus
(
    S16BIT s16DevNum,
    U32BIT cmd,
    void *pdata
);

S16BIT _DECL _dvxMtiGetMetrics
(
   S16BIT s16DevNum,
   ACEX_MTI_METRICS *pMtiMerics
);

S16BIT _DECL _dvxSetRespTimeOut
(
    S16BIT s16DevNum,
    U16BIT u16ModeMask,
    U32BIT u32Multiple
);

S16BIT _DECL _dvxGetChannelCount
(
    S16BIT s16DevNum,
    CHANCOUNT_t *pChanCount
);

S16BIT _DECL _dvxBoardReset
(
    S16BIT s16DevNum,
    U32BIT u32BdComponet
);

S16BIT _DECL _dvxGetIRIGTx
(
    S16BIT s16DevNum,
    PACE_IRIG_TX    pstructIRIG
);

S16BIT _DECL _dvxSetIRIGTx
(
    S16BIT s16DevNum,
    ACE_IRIG_TX    structIRIG
);

S16BIT _DECL _dvxCheckMfCapable
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    BOOLEAN *pbMfCapable
);

S16BIT _DECL _dvxBcGetReplayInfo
(
    S16BIT s16DevNum,
    ACEX_BC_REPLAY_STATUS *ReplayStatus
);

S16BIT _DECL _dvxBcReplayRaw
(
    S16BIT s16DevNum,
    PVOID *pReplayData,
    U32BIT u32DataLength
);

S16BIT _DECL _dvxBcReplayMsgDataDma
(
    S16BIT s16DevNum,
    U16BIT u16MemPage,
    U32BIT u32MemOffset,
    CHAR   *pMemBuffer,
    U32BIT u32MemDataLen
);

S16BIT _DECL _dvxBcReplayBlockOnIrq
(
    S16BIT s16DevNum,
    U32BIT *u32IrqStatus
);

AVIONIC_LEVEL _DECL _dvxGetAioOutput(S16BIT s16DevNum, S16BIT Channel);
AVIONIC_DIRECTION _DECL _dvxGetAioDir(S16BIT s16DevNum, S16BIT Channel);
AVIONIC_LEVEL _DECL _dvxGetAioInput(S16BIT s16DevNum, S16BIT Channel);
S16BIT _DECL _dvxSetAioOutput(S16BIT s16DevNum, S16BIT Channel, AVIONIC_LEVEL Level);
S16BIT _DECL _dvxSetAioDir(S16BIT s16DevNum, S16BIT Channel, AVIONIC_DIRECTION Direction);
S16BIT _DECL _dvxGetAioAll(S16BIT s16DevNum, U16BIT *Directions, U16BIT *Levels);
S16BIT _DECL _dvxSetAioAll(S16BIT s16DevNum, U16BIT Directions, U16BIT Levels);

S16BIT _DECL _dvxConfigureIoInterruptConditions
(
    S16BIT s16DevNum,
    U16BIT u16Command,
    ACEX_IO_CONFIGURE_TYPE *pAioConfigure
);

S16BIT _DECL _dvxMTISetCh10TimePktId
(
    S16BIT s16DevNum,
    U16BIT u16ChannelId
);

void _DECL _ddcFseek64(FILE *pFile, S64BIT s64Offset, int origin);
S64BIT _DECL _ddcFtell64(FILE *pFile);
S16BIT _DECL _dvxTrgReset
(
    S16BIT s16DevNum,
    U16BIT u16Channel
);

S16BIT _DECL _dvxTrgStausRead
(
    S16BIT s16DevNum,
    U16BIT u16Channel,
    U32BIT *pu32Status,
    U32BIT u32Dwords
);


/* Discrete I/O Time Tag  */
S16BIT _DECL _dvxDioTtCfg
(
    S16BIT s16DevNum,
    DIO_TT_CFG *psDioTtCfg,
    void(_DECL *pfDioTtIsr)(S16BIT s16DevNum, U32BIT u32IntSts)
);

S16BIT _DECL _dvxDioTtCtl
(
    S16BIT s16DevNum,
    U32BIT u32DioTtCtl
);

S16BIT _DECL _dvxDioTtRead
(
    S16BIT s16DevNum,
    U8BIT *pu8Data,
    U32BIT *pu32Bytes
);


/* todo */
/*16BIT GetLastError(void);*/


/* internal debug use only */
S16BIT _DECL _dvxDebug
(
    S16BIT s16DevNum,
    U32BIT cmd,
    void *pdata
);

S16BIT _DECL _dvxSetBoardFeature
(
    S16BIT s16DevNum,
    U32BIT u32Command,
    void *pdata
);

#endif /* __DEVICEX_COMMON_H__ */
