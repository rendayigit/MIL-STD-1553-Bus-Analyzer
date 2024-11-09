/*******************************************************************************
 * FILE: ddc_udl_1553_rt_private.h
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

#ifndef _DDC_UDL_1553_RT_PRIVATE_H_
#define _DDC_UDL_1553_RT_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "include/ddc_types.h"
#include "driver_sdk/ddc_udl_sdk.h"


#define MAX_STREAMS                             4
#define MRT_STREAM_DBLK_RX                      0
#define MRT_STREAM_DBLK_TX                      1


#define ACEX_MRT_MAX_DATA_ARRAYS                4

/* Control Operations */
#define ACEX_MRT_HBUF_ENABLE                    0
#define ACEX_MRT_HBUF_DISABLE                   1
#define ACEX_MRT_HBUF_GET                       2
#define ACEX_MRT_RTCMD_HIGH_PCT_RESET           3
#define ACEX_MRT_STREAMING_ENABLE_RX            4
#define ACEX_MRT_STREAMING_ENABLE_TX            5
#define ACEX_MRT_STREAMING_DISABLE_RX           6
#define ACEX_MRT_STREAMING_DISABLE_TX           7
#define ACEX_MRT_DATA_ARRAY_ENABLE              8
#define ACEX_MRT_DATA_ARRAY_DISABLE             9
#define ACEX_MRT_DATA_ARRAY_ENABLE_CONTINUOUS   10
#define ACEX_MRT_DATA_ARRAY_DISABLE_INT         11

#define NUM_RTS                                 32
#define RT_DATA_TFR_IMP_ID                      0x00FE0000  /* RT DATA TFR posts to IMP queue will use this id  */
#define RT_HBUF_IMP_ID                          0x00FF0000  /* RT HBUF posts to IMP queue will use this id      */
            
#define RT_LOOKUP_TABLE_BASE_ADDR               0x140
#define RT_SUB_ADDR_CTL_WORD_OFFSET             0x60
            
#define NUM_RT_DATA_BLOCK_POINTERS              96
            
#define ACEX_MRT_MEMSIZE                        32768       /* 32 KBytes for memory to support 32 RTs in mrt config/mgmt, including Q-prime cards */
#define ACEX_MRT_MEMORY_BA                      0           /* base address in memory space                         */
            
#define RTX_VRT_MEMORY_OFFSET                   0x0100      /* the memory slot length for each Virtual RT (32bit)   */
            
#define RTX_BROADCAST_RT                        31
            
#define RTX_CMDSTK_STAT_UPDATE_RATE             100         /* update stats every 100 Mrt Ints                      */

/* Improvements block fifo command timeout for RtCmdStack mode */
#define RTX_HBUF_IMPCMD_TIMEOUT                 10                                                  /* 2us resolution  1000*2uS = 2 mS timeout          */
            
#define MRT_CMD_STK_MSG_TFR_SZ                  64
#define MRT_CMD_STK_MSG_BYTE_SZ                 92                                                  /* 44 wds 2 wds for length info header              */
#define MRT_CMD_STK_BLK_BYTE_SZ                 (MRT_CMD_STK_MSG_TFR_SZ * MRT_CMD_STK_MSG_BYTE_SZ)  /* 80 bytes per message                             */


enum MRT_STREAM_STATE
{
    MRT_STREAM_RESET = 0,
    MRT_STREAM_PENDING,
    MRT_STREAM_COMPLETE
};


typedef enum _ACEX_DATA_ARRAY_STATE
{
    DA_OFF = 0,
    DA_ON,
    DA_CONTINOUS,
    DA_DONE

} ACEX_DATA_ARRAY_STATE;


/* ========================================================================== */
/* RT Definitions                                                             */
/* ========================================================================== */

typedef struct _ACEX_RT_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16RtAddr;
    U16BIT u16RespTimeOut;
    U32BIT u32Options;              /* RT specific options          */

} ACEX_RT_CONFIG;


typedef struct _ACEX_MRT_CONFIG_TYPE
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16CmdStkType;           /* value to identify stack size */
    U32BIT u32CmdBA;                /* base address of cmd stack    */
    U8BIT u8MrtEnable;
    U16BIT u16SomEom;               /* 0=SOM, 1=EOM                 */
    U16BIT u16GblDataStkType;
    U32BIT u32GblDataStkBA;
    U32BIT u32IntConditions;
    U32BIT u32IsqBA;

} ACEX_MRT_CONFIG_TYPE;


typedef struct _ACEX_RT_ACCESS
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16RtAddr;
    U32BIT u32Data;

} ACEX_RT_ACCESS;


typedef struct _ACEX_RT_HW_INFO
{
    U32BIT u32RtIllegalizationRxTableBA;   /* addr in hw memory                        */
    U32BIT u32RtIllegalizationTxTableBA;
    U32BIT u32SACtrlBA;                    /* addr in hw memory                        */
    U32BIT u32ImpRxLkupTableBA;            /* Imp RxLkup mirror addr in hw memory      */
    U32BIT u32ImpTxLkupTableBA;            /* Imp TxLkup mirror addr in hw memory      */
    U32BIT u32ModeCodeDataTableBA;         /* Mode Code Data Table addr in hw memory   */
    U32BIT u32ConfigurationBA;             /* Configuration field address              */
    U32BIT u32RxLkupTableBA;               /* addr in hw memory                        */
    U32BIT u32TxLkupTableBA;               /* addr in hw memory                        */
    U32BIT u32BusyBitRxLkupTableBA;        /* addr in hw memory                        */
    U32BIT u32BusyBitTxLkupTableBA;        /* addr in hw memory                        */
    U32BIT u32StatusInCtrlTableBA;         /* addr in hw memory                        */
    U32BIT u32ModeCodeSelTxIntTableBA;     /* addr in hw memory                        */
    U32BIT u32ModeCodeSelRxIntTableBA;     /* addr in hw memory                        */
    U32BIT u32DbcHoldoffTimeBA;            /* DBC holdoff time */
    U32BIT u32ImrTrigSelectBA;             /* IMR trigger source selection addr */
    U32BIT u32ImrModeCodeBA;               /* IMR trigger source selection for Mode Codes */

} ACEX_RT_HW_INFO;


/* ========================================================================== */
/* Multi-RT Definitions                                                       */
/* ========================================================================== */

typedef struct _ACEX_1553_MRT_STAT_TYPE
{
    U32BIT u32HighNumCmdStkEntries;
    U32BIT u32NumCmdStkLost;
    U32BIT u32LastNumCmdStkEntries;
    U32BIT u32NumCmdStackOverflows;
    U32BIT u32NumIllegalCmds;
    U32BIT u32NumIsqRollovers;
    U32BIT u32NumCmdStk50Rollovers;
    U32BIT u32NumCircBuf50Rollovers;
    U32BIT u32NumTxTimouts;
    U32BIT u32NumCmdStkRollovers;
    U32BIT u32NumAddrParityErrs;
    U32BIT u32NumCircBufRollovers;
    U32BIT u32NumSACWEoms;
    U32BIT u32NumFmtErrs;
    U32BIT u32NumMCs;
    U32BIT u32NumEoms;

} ACEX_1553_MRT_STAT_TYPE;


typedef struct _VRT_TYPE
{
    U32BIT u32CfgMemBA;                     /* configuration area base address for this RT instance */
    U32BIT u32Config;

    ACEX_RT_HW_INFO sVrtHwInfo;

    ACEX_MOD_STATE state;

} VRT_TYPE;


typedef struct _RTX_REG_TYPE
{
    U32BIT u32RTGConfig;
    U32BIT u32RTEnable;                     /* 1 bit for each Virtual RT */
    U32BIT u32IntMask;
    U32BIT u32CmdStkPtr;
    U32BIT u32GblDataStkPtr;
    U32BIT u32IsqPtr;

    U32BIT u32CmdStkPtrSwIndex;

} RTX_REG_TYPE;


typedef struct _RTX_CMD_STK_TYPE
{
    U32BIT u32RtCmdStkMsgSz;                /* number of 1553 msgs in cmd stack (4dwds per msg) */
    U32BIT u32RtCmdStkDwdSz;                /* number of 32-bit words in cmd stack              */
    U32BIT u32RtCmdStkStatUpdateCounter;    /* used to control when stat update occurs          */
    U32BIT u32CmdStkPtrBA;
    U32BIT u32OverflowNumEntries;           /* log of reported msg count when overflow occurred */

    BOOLEAN bResyncInProgress;

} RTX_CMD_STK_TYPE;

typedef struct _ACEX_MRT_HBUF_TYPE
{
    U8BIT *pu8hbufMemory;
    U32BIT u32HbufNumEntries;
    U32BIT u32CmdRdIndex;
    U32BIT u32HbufNxtRd;
    U32BIT u32HbufLstWr;
    U32BIT u32NumRequested;
    U32BIT u32CmdIndex;                     /* keeps track Current Cmd Index                        */
    U32BIT u32StkMsgCapacity;               /* max number of messages stack can hold                */
    BOOLEAN bImpCmdPending;                 /* TRUE if a pending cmd is on the imp infifo           */
    BOOLEAN bDmaXferPending;                /* TRUE if a DMA transfer is pending                    */
    ACEX_MOD_STATE state;
    U32BIT u32HBufByteSize;
    U32BIT u32MaxTfrCmdSize;                /* maximum number of RtStkCmds transferred at one time  */

} ACEX_MRT_HBUF_TYPE;


typedef struct _ACEX_RT_DATA_ARRAY
{
    ACEX_RT_ACCESS sRtAccess;

    U16BIT u16RtAddr;
    U16BIT u16ID;
    U32BIT u32TxPtrOffsetAddressMRT;        /* 32 bit address of tx pointer         */
    U32BIT u32TxPtrOffsetAddressIMP;        /* 32 bit address of tx pointer         */
    U32BIT u32TxTfrCompleteOffset;          /* 16-bit offset address when complete  */
    U32BIT u32TxTfrStartOffset;             /* 16-bit start offset                  */

    ACEX_DATA_ARRAY_STATE eState;

} ACEX_RT_DATA_ARRAY;


struct _ACEX_1553_RT_TYPE
{
    U32BIT *pu32RegBA;                      /* pointer to 1553 MRT Registers base address                   */
    U32BIT *pu32RegSize;                    /* pointer to 1553 MRT Register size                            */
    U32BIT u32MrtMemSize;                   /* size of memory to hold Virtual RT - specific information     */
    U32BIT u32MrtBA;                        /* start address for Multi-RT's                                 */
    ACEX_MRT_CONFIG_TYPE sRtxGCfg;
    RTX_REG_TYPE sRtxCfgReg;
    ACEX_MOD_STATE state;
    VRT_TYPE sVRT[NUM_RTS];                 /* Virtual RT specific information                              */
    U16BIT u16RtSource;                     /* for SRT compatibility                                        */
    BOOLEAN bMode;                          /* 0 = Single RT, 1 = Multi-RT                                  */
    ACEX_1553_MRT_STAT_TYPE stats;
    ACEX_MRT_HBUF_TYPE sHbuf;
    RTX_CMD_STK_TYPE sCmdStk;

    DDC_ISR_LOCK_TYPE semEventCond;
    DDC_ISR_FLAG_TYPE semEventCondFlag;
    U16BIT u16EventCond;                    /* Event Condition                                              */
    
    DDC_CALLBACK waitqueueCallback;
    DDC_EVENT waitqueueEvent;               /* Event Wait Queue                                             */

    U8BIT *pu8Buf;                          /* for mrtImpReadCmd()/mrtImpReadData(), point to the temporary buffer to transfer read data    */
    U32BIT u32RdByte;                       /* for mrtImpReadCmd()/mrtImpReadData(), store the num of byte read                             */

    enum MRT_STREAM_STATE eStreamState[2];  /* TX = 1, RX = 0                                               */

    ACEX_RT_DATA_ARRAY sDataArrayTable[ACEX_MRT_MAX_DATA_ARRAYS * 32];      /* 32 RT's                      */
    BOOLEAN bDataArrayEnabled;
    BOOLEAN b1553a[NUM_RTS];                /* 1553a mode                                                   */
};


typedef struct _MRT_STREAM_TRANSFER_TYPE
{
    S16BIT nDataBlkID;                      /* DBLK ID                                                      */
    U32BIT u32DwdBufPtr;                    /* address of DBLK in hw                                        */
    U16BIT u16BufWdSize;                    /* DBLK num wds                                                 */

    U32BIT u32UsrBufByteSize;               /* size of user buf in bytes                                    */
    U16BIT *pUBuf;                          /* pointer to user buf                                          */

    U32BIT u32NumSATfr;                     /* running count of number of SA transfers                      */
    U32BIT U32SACount;                      /* running count of number SA transfers completed               */

    S32BIT s32Timeout;                      /* timeout associated w/ transaction -1-forever, 0-don't wait, else wait for amount in mS   */

} MRT_STREAM_TRANSFER_TYPE;


typedef struct _MRT_STREAM_TYPE
{
    U32BIT u32ID;

    MRT_STREAM_TRANSFER_TYPE sDirection[2];     /* MRT_STREAM_DBLK_RX   MRT_STREAM_DBLK_TX                                  */

    U32BIT u32BufWdSegmentSize;                 /* number of words in each SA segment                                       */
    U32BIT u32MTUWd;                            /* maximum transmission unit in wds - hw memory allocated in each direction */

    U8BIT u8RtAddr;
    U32BIT u32SAMask;
    U16BIT u16Direction;                        /* ACE_RT_MSGTYPE_RX    0x0001                                              */
                                                /* ACE_RT_MSGTYPE_TX    0x0002                                              */
    U16BIT u16Chnl;

    /* for driver use only */
    U32BIT u32NumSAGroup;                       /* number of SA in this group                                               */
    void*   pRequest;                           /* handle to request object, used by driver only                            */

} MRT_STREAM_TYPE;


typedef struct _ACEX_MRT_HW_INFO
{
    U32BIT u32ConfigurationBA;  /* addr in hw reg                                                       */
    U32BIT u32GblDStkPtrBA;     /* addr in hw reg                                                       */
    U16BIT u16RtSource;         /* USB INTERNAL ONLY - this is for SRT compatibility                    */
    U16BIT bMode;               /* ACEX_MRT_MODE, ACEX_SRT_MODE                                         */
    U32BIT u32MemBA;            /* memory Base Address - currently used to resolve to offsets when      */
                                /*  writing entries into lookup tables TODO: request hw mod so          */
                                /*  absolute addresses can be used                                      */

} ACEX_MRT_HW_INFO;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern S16BIT mrtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern S16BIT mrtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_MRT_CONFIG_TYPE *psRtxGCfg
);

extern S16BIT mrtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern S16BIT mrtHbufCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *pRtAccess,
    size_t OutputBufferLength,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern void mrtHbufImpPostQueue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void mrtRtCmdStkSync
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void mrtUpdateRtStkStats
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern ACEX_RT_HW_INFO *mrtOpenBrdcst
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_CONFIG_ID *psRtxCfg
);

extern S16BIT mrtInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
);

extern S16BIT mrtInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
);

extern void mrtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntStatus
);

extern S16BIT mrtSetCmdStkPtrToLatest
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void mrtStreamCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *pRtAccess
);

extern void mrtDataArrayCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_DATA_ARRAY *pRtDA
);


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern ACEX_RT_HW_INFO *rtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
);

extern void rtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
);

extern S16BIT rtConfigSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
);

extern S16BIT rtConfigClr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_CONFIG *psRtxCfg
);

extern S16BIT rtInternalBITWdRd
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtxBitWd,
    U16BIT *pu16Data
);

extern void rtStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
);

extern void rtStop
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
);

extern void rtLatchCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
);

extern S16BIT rtSourceCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ACEX_RT_ACCESS *psRtAccess
);

extern S16BIT rtSetNoRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U16BIT u16RtAddr,
    U32BIT u32NRTO
);

extern S16BIT mrtImpReadCmd
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U32BIT u32NumCmd,
    U32BIT u32OutputBufferLen,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT mrtImpReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U16BIT u16RtAddr,
    U16BIT u16RtSA,
    U32BIT u32OutputBufferLen,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern BOOLEAN rtSendStream
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MRT_STREAM_TYPE *pMrtStream,
    void *pData,
    U32BIT *pNumBytes
);

extern BOOLEAN rtReceiveStream
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    MRT_STREAM_TYPE *pMrtStream,
    U32BIT *pNumBytes
);

extern U16BIT mrtReadModeCodeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U8BIT u8RtAddr,
    U16BIT wModeCode
);

extern void mrtWriteModeCodeData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U8BIT u8RtAddr,
    U16BIT wModeCode,
    U16BIT wModeCodeData
);

#endif /* _DDC_UDL_1553_RT_PRIVATE_H_ */
