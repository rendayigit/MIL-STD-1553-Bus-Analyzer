/*******************************************************************************
 * FILE: ddc_udl_1553_mt_private.h
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

#ifndef _DDC_UDL_1553_MT_PRIVATE_H_
#define _DDC_UDL_1553_MT_PRIVATE_H_

#include "driver_sdk/ddc_udl_sdk.h"
#include "driver_sdk/ddc_udl_private.h"
#include "core/1553/ddc_udl_1553_common_private.h"

#define MTI_ERR_NONE                                DDC_UDL_ERROR__SUCCESS
#define MTI_ERR_TIMEOUT                             (int)(50)
#define MTI_ERR_DATA_UNAVAILABLE                    (int)(51)
#define MTI_ERR_BUFFER_OVERFLOW                     (int)(52)
#define MTI_ERR_BUFFER_UNAVAILABLE                  (int)(53)
#define MTI_ERR_INVALID_MALLOC                      (int)(-54)
#define MTI_ERR_INVALID_BUFFER                      (int)(-55)
#define MTI_ERR_PARAMETER                           (int)(-65)
#define MTI_ERR_DEVICE_OVERFLOW                     (int)(-73)

#define MTI_DATA_PKT_FILLER_SPACE                   2           /* Padding space onto the end of a pkt buffer for filler bytes                  */
#define MTI_ALIGN_SPACE                             2           /* Space either appended or prepended for odd 16-bit word boundary transfers    */

#define MTI_DWORD_CHUNK_SIZE                        0x400       /* 1024 DWords */

#define MTI_POOL_OVERHEAD_SIZE                      (sizeof(struct _MTI_DATA_LIST_ENTRY) + MTI_DATA_PKT_FILLER_SPACE + MTI_ALIGN_SPACE)

#define MTI_RESPTIME_MAX                            127         /* Max MT Response time out of 127us */

/* Device Memory Buffer Sizes */
#define MTI_DEVBUF_SIZE_4K                          0x001000
#define MTI_DEVBUF_SIZE_8K                          0x002000
#define MTI_DEVBUF_SIZE_16K                         0x004000
#define MTI_DEVBUF_SIZE_32K                         0x008000
#define MTI_DEVBUF_SIZE_64K                         0x010000
#define MTI_DEVBUF_SIZE_128K                        0x020000
#define MTI_DEVBUF_SIZE_256K                        0x040000
#define MTI_DEVBUF_SIZE_512K                        0x080000
#define MTI_DEVBUF_SIZE_1M                          0x100000
#define MTI_DEVBUF_SIZE_2M                          0x200000

#define MTI_CH10_PKT_HEADER_SIZE                    24
#define MTI_CH_SPEC_DATA_HDR_SIZE                   4
#define MTI_CH10_PKT_HEADER_WORDS                   12
#define MTI_CH10_PKT_SYNC_PATT                      0xEB25
#define MTI_CH10_PKT_HEADER_VER                     0x2
#define MTI_CH10_PKT_DATA_TYPE                      0x19
#define MTI_CH10_PKT_DATA_TYPE_MTI                  0xF1
#define MTI_CH10_PKT_DATA_TYPE_MTIE                 0xF2
#define MTI_CH10_PKT_DATA_TYPE_MTR                  0xF3
#define MTI_CH10_PKT_CHECKSUM_SIZE                  2
#define MTI_CH10_PKT_CHECKSUM_WORD_CNT              11
#define MTI_CH10_PKT_CHANNEL_DATA_SIZE              4

#define MTI_CH10_DATA_PKT_CHANNEL_DATA_TTB          0x80000000

#define MTI_HW_OVERFLOW_BIT                         0x0008

#define MT_IRIG_TIME_SOURCE_INTERNAL               0x80000000L
#define MT_IRIG_TIME_SOURCE_EXTERNAL               0x000000001L

#define MTI_CH10_TIME_PKT_CHANNEL_DATA_SRC_EXT      0x1
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_FMT_IRG_B    0x0
#define MTI_CH10_TIME_PKT_CHANNEL_DATA_DATE_DMY     0x200

#define MTI_CH10_TIME_PKT_SIZE                      (sizeof (struct _MTI_DATA_LIST_ENTRY) + sizeof(ULONG64))
#define MTI_CH10_TIME_PKT_TYPE                      0x11

/* 1553 MTI Time Packet Interrupt Disable */
#define IRIG_TIME_DMY_FMT_B_RESET                   0x00005         /* Disable Irig Improvements, Leave Irig Running On Device  */

/* 1553 MTI Time Packet Interrupt Initialization */
#define IRIG_TIME_DMY_FMT_B_INIT                    0x1C005         /* Enable Irig Improvements, DMY Format, DDC Format B       */

/* 1553 MTI Dma Address Alignment Macro */
#define DMA_ADDRESS_BOUNDARY_MASK                   0xFFFFFFFE

/* MTI data buffer states*/
typedef enum
{
    BUF_STATE_FREE = 0,
    BUF_STATE_DMA,
    BUF_STATE_DATA

} BUF_STATE;


typedef struct _MT_REG_TYPE
{
    U32BIT u32Channel;
    U32BIT u32IntMaskEn;    /* interrupt mask enable 02H WR */
    U32BIT u32IntNumWds;
    U32BIT u32IntNumQTfr;
    U32BIT u32IntBlkTime;
    U32BIT u32IntMsgTime;
    U32BIT u32TgtMemBA;
    U32BIT u32TgtMemSzDWD;
    U32BIT u32IgnoreDataTfr;
    U32BIT u32ImpBlkTrigHostInitInt;
    U32BIT u32IntStatus;
    U32BIT u32OutFifoStatus;
    U32BIT u32InFifoStatus;
    U32BIT u32RtCmdStkPtrLR;
    U32BIT u32RtCmdStkDWL;
    U32BIT u32RtGblDBufPtrLR;
    U32BIT u32MtCmdStkPtrLR;
    U32BIT u32MtCmdStkDWL;

} MT_REG_TYPE;

typedef struct _MT_MTI_INFO
{
    U32BIT u32NumOfMsgs;
    U32BIT u32TotalLength;
    U32BIT u32FirstMsgAddress; /* in 16bit */

} MT_MTI_INFO;


typedef struct _MT_MTI_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32DevBufByteSize;
    U32BIT u32DevBufWordAddr;
    U32BIT u32NumBufBlks;
    U32BIT u32BufBlkByteSize;
    BOOLEAN fZeroCopyEnable;
    U32BIT u32IrqDataLen;
    U32BIT u32IrqMsgCnt;
    U16BIT u16IrqTimeInterval;
    U32BIT u32IntConditions;
    U16BIT u16Ch10ChnlId;
    U8BIT u8HdrVer;
    U8BIT u8RelAbsTime;
    U8BIT u8Ch10Checksum;
    U8BIT bBcstDisable;
    U8BIT b1553aMc;
    U8BIT bMtiBswTypeDisable;
    U8BIT u32BlockOnIrqEnable;
    U8BIT bMtiErrorMonitorEnable;
    U8BIT bMtiReplayMonitorEnable;
    U8BIT bMtiCustomDataTypeEnable;
    U8BIT bBusyIllegalBitEnable;
    U8BIT bEomTtEnable;
    U8BIT bBusAMonitoringDisable;
    U8BIT bBusBMonitoringDisable;
    U8BIT bMtiTriggerStartEnable;
    U8BIT bMtiTriggerStopEnable;
	U8BIT b1553aOverrideMcTrErrEnable;
} MT_MTI_CONFIG;


typedef struct _MT_MTI_METRICS
{
    U32BIT u32MtiStackPercentFull;
    U32BIT u32MtiStackPercentHigh;
    U32BIT u32MtiStackOverflowCount;

} MT_MTI_METRICS;


typedef struct _MT_MTI_HW_INFO
{
    U32BIT u32LookupTable32BitBA;
    U32BIT u32LookupTable32BitSize;
    U32BIT u32MtiMem32BitEndAddr;
    U32BIT u32MtiMem32BitStartAddr;
    U32BIT u32MtiMem32BitStartAddrOffset;

} MT_MTI_HW_INFO;


struct _ACEX_1553_MT_TYPE
{
    U32BIT *pu32RegBA;                              /* ptr to 1553 MTX Registers base address */
    U32BIT *pu32RegSize;                            /* ptr to 1553 MTX Register size */
    U32BIT u32LookupTable32BitBA;                   /* Selective Monitor Lookup Table base address */
    U32BIT u32LookupTableMemByteSize;               /* Size of memory to Selective Monitor Lookup Table */
    U32BIT u32RespTimeout;                          /* Response Timeout */
    U32BIT u32BlockOnIrqEnable;                     /* Enables user bit for blocking on IRQ */

    BOOLEAN bMtiBswTypeDisable;                     /* Disables DDC BSW formatting in favor of IRIG Chapter 10 BSW formatting */
    BOOLEAN bMtiErrorMonitorEnable;                 /* Enables MTIe mode message capturing */
    BOOLEAN bMtiReplayMonitorEnable;                /* Enables MTR mode message capturing */
    BOOLEAN bMtiCustomDataTypeEnable;               /* Enables chapter 10 custom data type packet tagging */

    BOOLEAN bMtiTriggerStartEnable;
    BOOLEAN bMtiTriggerStopEnable;


    U32BIT u32MtiRelativeTimeDataHi;
    U32BIT u32MtiRelativeTimeDataLo;                /* the relative time data retrieved from GENERAL_TT_LATCHED reg when irig 1sec interrupt occurs */

    U32BIT u32MtiErrorCode;                         /* will be set to error code when error happens, for example, when data pool is full */

    S8BIT s8MtiDataPacketSeqNumber;

    /* MTI Data PKT */
    DDC_ISR_LOCK_TYPE semMtiDataPkt;                /* MTi Data Packet spinlock */
    DDC_ISR_FLAG_TYPE semMtiDataPktFlag;            /* MTi Data Packet spinlock flag */

    struct _MTI_DATA_LIST_ENTRY *pMtiDataListEntry[MTI_MAX_NUM_BUF];

    U32BIT u32MtiDataPktHead;                       /* the index of the head in Data Pkt List */
    U32BIT u32MtiDataPktTail;                       /* the index of the tail in Data Pkt List */
    U32BIT u32MtiDataPoolBufferSize;                /* the size of the buffer allocated to contain Data Pkt in each Data List Entry, in 8-bit */
    U32BIT u32MtiDataPoolCount;                     /* the number of entries in Data Packet List */
    BOOLEAN bMtiLookasideListDataInit;              /* whether MTI data list is initialized */

    U8BIT u8MtiDataPoolEventFlag;

    DDC_CALLBACK eMtiDataPoolCallback;
    DDC_EVENT eMtiDataPoolEvent;

    /* MTI CH10 Time Packet */
    DDC_ISR_LOCK_TYPE semMtiTimePkt;                /* Mti Time Packet spinlock */
    DDC_ISR_FLAG_TYPE semMtiTimePktFlag;            /* Mti Time Packet spinlock flag */

    BOOLEAN bMtiTimeDataAvailable;                  /* indicate whether Time Data is available */

    U8BIT u8MtiTimePoolEventFlag;

    DDC_CALLBACK eMtiTimePoolCallback;
    DDC_EVENT eMtiTimePoolEvent;

    struct _MTI_TIME_LIST_ENTRY *pMtiTimeListEntry;
    S8BIT s8MtiTimePacketSeqNumber;

    /* MTi Metrics Information */
    U32BIT u32MtiStackSizeWords;
    U32BIT u32MtiStackPercentFull;
    U32BIT u32MtiStackPercentHigh;
    U32BIT u32MtiStackOverflowCount;

    /* MTI Mode */
    MTI_STATE eMtiState;
    BOOLEAN bMtiDeviceOverflow;                     /* MTI Overflow Interrupt occured */

    U32BIT u32MtiMem32BitStartAddr;                 /* absolute start addr of MTI memory */
    U32BIT u32MtiMem32BitEndAddr;                   /* absolute end addr of MTI memory */

    U16BIT u16MtiPacketChannelId;

    ACEX_MOD_STATE state;
};


/* -------------------------------------------------------------------------- */
/* Chapter 10 Data Packets                                                    */
/* -------------------------------------------------------------------------- */

#pragma pack(1)
/* 24 Byte MTI Chapter 10 Data Packet Header */
typedef struct ACEX_MTI_CH10_DATA_PKT
{
    U16BIT u16PktSyncPattern;
    U16BIT u16ChannelId;
    U32BIT u32PktLength;
    U32BIT u32DataLength;
    U16BIT u16SeqNumHdrVer;
    U16BIT u16DatTypePktFlags;
    U16BIT u16RelativeTimeCntr[3];
    U16BIT u16HeaderChksum;
    U32BIT u32ChnlSpecificData;
    U32BIT u32Pad;                    /* this is needed for the PCIe devices so that u16MsgData is on an 8 byte alignment */
    U16BIT u16MsgData[1];             /* Packet Data From Hardware */

} ACEX_MTI_CH10_DATA_PKT;

#pragma pack()


struct _MTI_DATA_LIST_ENTRY
{
    BUF_STATE State;
    U8BIT bOverflow;
    U8BIT pad;
    U16BIT u16AlignOffset;
    U32BIT u32ErrorCode;
    U32BIT u32MsgCount;                 /* msg cnt of the Pkt recorded in mtSetupDMA                           */
    U32BIT u32WordCount;                /* length of the Pkt recorded in mtSetupDMA, it is actually byte count */
    U32BIT copyOffset;

    DDC_DMA_ADDR mtDMA_Addr;
    size_t mtDMA_Size;
    ACEX_MTI_CH10_DATA_PKT *pDataPkt;
};


/* -------------------------------------------------------------------------- */
/* Chapter 10 Time Packets                                                    */
/* -------------------------------------------------------------------------- */

typedef struct _MTI_CH10_TIME_DATA
{
    U32BIT GlobalTimeLo;
    U32BIT GlobalTimeHi;
    U32BIT RelativeTimeLo;
    U32BIT RelativeTimeHi;

} MTI_CH10_TIME_DATA;

/* <UDL1> */
#pragma pack(1)
/* 24 Byte MTI Chapter 10 Time Packet Header*/
typedef struct _MTI_CH10_TIME_PKT
{
    /* Header, 24 bytes - MTI_CH10_PKT_HEADER_SIZE */
    U16BIT u16PktSyncPattern;
    U16BIT u16ChannelId;
    U32BIT u32PktLength;
    U32BIT u32DataLength;
    U16BIT u16SeqNumHdrVer;
    U16BIT u16DatTypePktFlags;
    U16BIT u16RelativeTimeCntr[3];
    U16BIT u16HeaderChksum;
    U32BIT u32ChnlSpecificData;

    /* Packet Body - filled in by hardware */
    U64BIT u64TimeData;
    
} MTI_CH10_TIME_PKT;

#pragma pack()


struct _MTI_TIME_LIST_ENTRY
{
    BUF_STATE State;
    struct _MTI_CH10_TIME_PKT sMtiTimePkt;
};


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

struct _DMA_TRANSACTION;

extern S16BIT mtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig
);

extern S16BIT mtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig,
    MT_MTI_HW_INFO *pMtiHwInfo
);

extern S16BIT mtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel
);

extern S16BIT mtSetMtiState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void mtStoreTimeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void mtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32mtIntStatus,
    U32BIT u32StartAddress,
    U32BIT u32NumOfWords,
    U32BIT u32NumOfMsgs
);

extern S16BIT mtMtiInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void mtMtiTimeLookasideRemove
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT mtSetStrobeReg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);


extern S32BIT mtGetMtiCh10TimePkt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S32BIT mtGetMtiCh10DataPkt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void mtFreeDataPktListTail
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pCmdInfo
);

extern void mtFreeDataPktList
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel
);

extern S16BIT mtSetRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT mtGetMtiCh10Status
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    MT_MTI_INFO *pMtiCh10Info
);

extern S16BIT mtMtiInterruptConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MT_MTI_CONFIG *pMtiConfig
);

extern S16BIT mtMtiGetMetrics
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    MT_MTI_METRICS *pMtiMetrics
);

extern void mtSetupDMA
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern U8BIT mtDMASGRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _DMA_TRANSACTION *pDMATransaction
);

extern void ddcUdlMtCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#endif /* _DDC_UDL_1553_MT_PRIVATE_H_ */
