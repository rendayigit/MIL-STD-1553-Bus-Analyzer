/*******************************************************************************
 * FILE: ddc_udl_1553_bc_private.h
 *
 * DESCRIPTION:
 *
 *  This module provides interface to the Bus Controller (BC) hardware and the
 *  BC Run-Time Library.
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

#ifndef _DDC_UDL_1553_BC_PRIVATE_H_
#define _DDC_UDL_1553_BC_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "os/include/ddc_os_private.h"
#include "os/include/ddc_os_types_private.h"
#include "core/1553/ddc_udl_1553_common_private.h"
#include "include/ddc_ioctl.h"
#include "driver_sdk/ddc_udl_sdk.h"


#define NUM_DATASTR_CHANNELS                    128
#define NUM_DATARRAY_CHANNELS                   128


/* to provide BC IMP msg metric information  */
#define BC_IMP_MSG_METRIC_DBG                   0
#define BC_ASYNC_DEBUG                          0

#define BC_INT_STS_TTAG_ROLLOEVR                0x00000001
#define BC_INT_STS_UIRQX                        (BC_INT_MASK_UIRQ0 | BC_INT_MASK_UIRQ1 | BC_INT_MASK_UIRQ2 | BC_INT_MASK_UIRQ3 | \
        BC_INT_MASK_UIRQ4)
#define BC_INT_STS_LP                           (BC_INT_MASK_LP_HALF | BC_INT_MASK_LP_FULL)
#define BC_INT_STS_HP                           (BC_INT_MASK_HP_HALF | BC_INT_MASK_HP_FULL)
#define BC_INT_STS_ASYNC                        (BC_INT_STS_LP | BC_INT_STS_HP)
#define BC_INT_STS_BC                           (BC_INT_STS_UIRQX | BC_INT_STS_ASYNC | REG_BC_EOM | REG_BC_SELECT_EOM)

#define BC_INT_STS_MASK                         BC_INT_STS_BC

#define ACEX_BC_CLOSE_DELAY_MS                  2       /* ms */
#define ACEX_BC_CLOSE_TIMEOUT_MS                250     /* ms */

/* BC mode flags */
#define BC_MODE_FLAG_ON                         0x00000001
#define BC_MODE_FLAG_INT_BUSY                   0x00000002
#define BC_MODE_FLAG_USER_BUSY                  0x00000004
#define BC_MODE_FLAG_OFF_PENDING                0x00000008
#define BC_MODE_FLAG_INT_UIRQ4                  0x00000010
#define BC_MODE_FLAG_INT_TTAG_ROLLOVER          0x00000100
#define BC_MODE_FLAG_HBUF                       0x00001000
#define BC_MODE_FLAG_MSGBUF                     0x00002000
#define BC_MODE_FLAG_GPQ                        0x00010000
#define BC_MODE_FLAG_LPQ                        0x00020000
#define BC_MODE_FLAG_HPQ                        0x00040000

/* BC INT flags */
#define BC_INT_MASK_UIRQ_SHIFT                  18
#define BC_MODE_FLAG_UIRQ_SHIFT                 4

#define ACEX_BC_UIRQ4                           0x00000001

#define BC_INT_MASK_TT_ROLLOVER_SHIFT           0
#define BC_MODE_FLAG_TT_ROLLOVER_SHIFT          8

#define ACEX_IRQ_TT_ROLLOVER                    0x00000001

/* IMP */
#define BC_IMP_OUTPUT_QUEUE_ENTRY               256
#define BC_IMP_OUTPUT_QUEUE_FULL_MARK           68  /* in percent */

#define BC_IMP_WORD_COUNT_SHIFT                 8   /* IMP INT status, reg E */

/* msg event timeout value */
#define BC_MSG_EVT_TIMEOUT                      500 /* in ms */

/* BC Buf flags */
#define BC_HBUF_FLAG_INIT                       0x00000001
#define BC_MSGBUF_FLAG_INIT                     0x00000002
#define BC_TEMPBUF_FLAG_INIT                    0x00000004

/* BC HBuf size */
#define BC_HBUF_MIN                             0x1000      /* HBuf minimum size - 4K words */
#define BC_HBUF_MAX                             0x400000    /* HBuf maximum size - 4M words */

/* BC IMP cmd and data size in dwords */
#define BC_CMD_STS_DWORDS                       10
#define BC_DATA_DWORDS                          16
#define BC_CMD_DATA_DWORDS                      (BC_CMD_STS_DWORDS + BC_DATA_DWORDS)

/* BC msg cmd and data size in words for HBuf and msgBuf*/
#define BC_MSG_CMDSTS_WORDS                     10
#define BC_MSG_DATA_WORDS                       32
#define BC_MSG_CMD_DATA_WORDS                   (BC_MSG_CMDSTS_WORDS + BC_MSG_DATA_WORDS)

/* BC TEMP BUF size in dwords */
#define BC_TEMPBUF_DWORDS                       (BC_IMP_OUTPUT_QUEUE_ENTRY * BC_CMD_DATA_DWORDS) /* 1024 */

/* BC HBUF read types*/
#define BC_HBUF_READ_NEXT_PURGE                 0       /* next unread msg, increment head index                                    */
#define BC_HBUF_READ_NEXT_NPURGE                1       /* next unread msg, leave head index unchanged                              */
#define BC_HBUF_READ_LATEST_PURGE               2       /* latest msg, set head index to tail index                                 */
#define BC_HBUF_READ_LATEST_NPURGE              3       /* latest msg, leave tail index unchanged                                   */
#define BC_HBUF_READ_FILL_BUFFER                4       /* fill the user buffer from the next unread msg, increment head index,    */
                                                        /* until buffer is full or head index = tail index                          */

/* return message/header/data from buffer */
#define BC_MSG_CTRLSTS                          1       /* return control and status block only */
#define BC_MSG_DATA                             2       /* return data only                     */
#define BC_MSG_ID                               4       /* return msgId       */
#define BC_MSG_FULL                             (BC_MSG_CTRLSTS | BC_MSG_DATA)       /* return full message                  */

/* BC GPQ information */
#define BC_GPQ_ENTRY_DWORDS                     2
#define BC_GPQ_ADDR_MASK                        0x000000FF

/*Async msg work queue bits */
#define BC_ASYNC_DATA_VARIABLE                  0x00000000
#define BC_ASYNC_DATA_FIXED                     0x10000000
#define BC_ASYNC_QUEUE_HALF_FULL                0x40000000
#define BC_ASYNC_QUEUE_FULL                     0x80000000

#define BC_LPQ_MSG_TIME_MASK                    0x00000007
#define BC_LPQ_MSG_TIME_SHIFT                   25

#define BC_ASYNC_CTRLDATA_ALL                   0x00000000
#define BC_ASYNC_CTRLDATA_CTRL                  0x04000000
#define BC_ASYNC_CTRLDATA_DATA                  0x08000000

/* queue size for LPQ and HPQ */
#define BC_QSIZE_BITS_512                       0x0
#define BC_QSIZE_BITS_256                       0x1
#define BC_QSIZE_BITS_128                       0x2
#define BC_QSIZE_BITS_64                        0x3
#define BC_QSIZE_BITS_32                        0x4

/* BC Low Priority Queue information */
#define BC_LPQ_ENTRY_DWORDS                     1

#define BC_LPQ_BA_SHIFT                         7
#define BC_LPQ_RESET                            0x80000000

#define BC_LPQ_MSG_INT_HALF                     0x40000000
#define BC_LPQ_MSG_INT_FULL                     0x80000000
#define BC_LPQ_MSG_ADDR_MASK                    0x0007FFFF
#define BC_LPQ_MSG_DBPO_MASK                    0x00F80000

#define BC_LPQ_MSG_COUNT_MASK                   0x0000003FF

#define BC_LPQ_NOMP_SHIFT                       20

/* BC High Priority Queue information */
#define BC_HPQ_ENTRY_DWORDS                     1

#define BC_HPQ_BA_SHIFT                         7
#define BC_HPQ_RESET                            0x80000000

#define BC_HPQ_MSG_INT_HALF                     0x40000000
#define BC_HPQ_MSG_INT_FULL                     0x80000000
#define BC_HPQ_MSG_ADDR_MASK                    0x0007FFFF
#define BC_HPQ_MSG_DBPO_MASK                    0x00F80000

#define BC_HPQ_MSG_COUNT_MASK                   0x0000003FF

#define BC_HPQ_NOMP_SHIFT                       20

/* Data stream information */
#define BC_DATASTR_ID_MASK                      0xFF
#define BC_DATASTR_MSGNUM_MASK                  0x1FF

#define BC_CTRLWRDS_DATASTR_DIR_MASK            0x80
#define BC_CTRLWRDS_DATASTR_DIR_TX              0x00
#define BC_CTRLWRDS_DATASTR_DIR_RX              0x80
#define BC_BSW_ERROR_FLAG                       0x1000

/* Data array information */
#define BC_DATARRAY_ID_MASK                     0xFF
#define BC_DATARRAY_MSGNUM_MASK                 0x1FF

#define BC_DATARRAY_MSG_MAX                     32
#define BC_DATARRAY_MSG_FULL_MARK               68      /* in percent */

/* Replay buffer sizes */
#define BC_REPLAY_BUFFER_LEN_32K                0x0
#define BC_REPLAY_BUFFER_LEN_64K                0x1
#define BC_REPLAY_BUFFER_LEN_128K               0x2
#define BC_REPLAY_BUFFER_LEN_256K               0x3

/* BC message item offsets */
#define ACE_BC_MSG_CTRL_OFFSET                  0x0000
#define ACE_BC_MSG_CMD1_OFFSET                  0x0001
#define ACE_BC_MSG_DATA_PTR_OFFSET              0x0002
#define ACE_BC_MSG_TT_NEXT_MSG_OFFSET           0x0003
#define ACE_BC_MSG_TIME_TAG_OFFSET              0x0004
#define ACE_BC_MSG_BLOCK_STATUS_OFFSET          0x0005
#define ACE_BC_MSG_LOOP_BACK_OFFSET             0x0006
#define ACE_BC_MSG_RT_STATUS1_OFFSET            0x0007
#define ACE_BC_MSG_CMD2_OFFSET                  0x0008
#define ACE_BC_MSG_RT_STATUS2_OFFSET            0x0009


/* ========================================================================== */
/* ========================================================================== */

typedef struct _ACEX_BC_HBUF
{
    U32BIT u32IntMask;              /* Int mask for BC INT              */
    U32BIT u32ModeFlag;             /* mode flags for BC INT            */

    U32BIT u32GenIntMask;           /* Int mask for BC GEN INT          */
    U32BIT u32GenModeFlag;          /* mode flags for BC GEN INT        */

    U8BIT *hBuf;                    /* HBuf buffer                      */

    U32BIT u32ImpQueueFullMark;     /* INT condition: IMP output queue full mark to prevent overflow in percent     */
    U32BIT u32ImpIntMsgs;           /* INT condition: number of commands (msgs) transferred to the IMP output queue */
    U32BIT u32ImpIntWords;          /* INT condition: number of words transferred to the IMP output queue           */

    U32BIT u32DwordsSize;           /* HBuf size in DWORDS              */
    U32BIT u32NumEntries;           /* HBuf number of entries           */
    U32BIT u32Head;                 /* HBuf head index location (Dwords) to read data from buffer                   */
    U32BIT u32Tail;                 /* HBuf tail index location (Dwords) to put data into buffer                    */

    U32BIT u32MetricEna;            /* metric enable                    */

    U32BIT u32Count;                /* number of msgs in host buffer    */
    U32BIT u32Lost;                 /* HBuf full, possible msg loss     */
    U32BIT u32PctFull;              /* Current Percentage of HBuf used  */
    U32BIT u32HighPct;              /* Highest Percentage of HBuf used  */

} ACEX_BC_HBUF;


typedef struct _ACEX_BC_MSGBUF
{
    U32BIT u32IntMask;              /* Int mask for BC INT          */
    U32BIT u32ModeFlag;             /* mode flags for BC INT        */

    U32BIT u32GenIntMask;           /* Int mask for BC GEN INT      */
    U32BIT u32GenModeFlag;          /* mode flags for BC GEN INT    */

    U8BIT *hBuf;                    /* msgBuf buffer                */

    U32BIT u32MaxMsgIdx;            /* max msgBuf index             */
    U32BIT u32Dwords;               /* total u32ords                */
    U32BIT u32Words;                /* total Words                  */

} ACEX_BC_MSGBUF;


typedef struct _ACEX_BC_DATABUF
{
    U8BIT *hBuf;                    /* dataBuf buffer               */
    U32BIT u32MaxDataIdx;           /* max dataBuf index            */
    U32BIT u32Dwords;               /* total u32ords                */
    U32BIT u32Words;                /* total Words                  */

} ACEX_BC_DATABUF;


typedef struct _ACEX_BC_TEMPBUF
{
    U8BIT *pu8hBuf;                 /* tempBuf buffer               */
    U32BIT u32DWords;               /* 1K u32ords or 4K bytes       */
    U32BIT u32Entries;              /* max. entries in the tempBuf  */

} ACEX_BC_TEMPBUF;


typedef struct _ACEX_BC_GPQ
{
    U32BIT u32BaseAddr;             /* GPQ base address                         */

    U32BIT u32NumEntries;           /* GPQ number of entries                    */
    U32BIT u32DwordsSize;           /* GPQ size in u32ord                       */

    U32BIT u32Head;                 /* GPQ head location to read from           */
    U32BIT u32Tail;                 /* GPQ tail location to write next entry    */

    U32BIT u32Count;                /* # of GPQ entries in the buffer           */
    U32BIT u32Lost;                 /* # of possible GPQ entries lost           */
    U32BIT u32PctFull;              /* Current Percentage of GPQ used           */
    U32BIT u32HighPct;              /* Highest Percentage of GPQ used           */
} ACEX_BC_GPQ;


typedef struct _ACEX_BC_LPQ
{
    U32BIT u32BaseAddr;             /* queue base address                       */
    U32BIT u32Entries;              /* queue entries                            */
    U32BIT u32DwordsSize;           /* queue u32ord size                        */
    U32BIT u32Tail;                 /* queue tail location to write msg         */
    U32BIT u32Count;                /* number of messages queued                */

} ACEX_BC_LPQ;


typedef struct _ACEX_BC_HPQ
{
    U32BIT u32BaseAddr;             /* queue base address                       */
    U32BIT u32Entries;              /* queue entries                            */
    U32BIT u32DwordsSize;           /* queue u32ord size                        */
    U32BIT u32Tail;                 /* queue tail location to write msg         */
    U32BIT u32Count;                /* number of messages queued                */

} ACEX_BC_HPQ;


typedef struct _ACEX_BC_DATASTR
{
    U16BIT u16MsgNum;               /* the expected message number                      */
    U16BIT u16MsgCount;             /* the received message count                       */
    U16BIT u16MsgErrLocation;       /* the location of the first message with errors    */
    U16BIT u16MsgErrBsw;            /* the BSW of the first message with errors         */

} ACEX_BC_DATASTR;


typedef struct _ACEX_BC_DATASTR_CHAN
{
    ACEX_BC_DATASTR sRcv;            /* data stream receiver    */
    ACEX_BC_DATASTR sSnd;            /* data stream sender      */

} ACEX_BC_DATASTR_CHAN;


typedef struct _ACEX_BC_DATA_ARRAY
{
    U16BIT u16MsgToPostMax;         /* the maximum number of messages to post                                       */
    U16BIT u16MsgToPostAvail;       /* the available number of messages to post                                     */
    U16BIT u16MsgErrLocation;       /* the available number of messages to post of the first message with errors    */
    U16BIT u16MsgErrBsw;            /* the BSW of the first message with errors                                     */
    U16BIT u16MsgToPostEmptyMark;

} ACEX_BC_DATA_ARRAY;


typedef struct _ACEX_BC_DATA_ARRAY_CHAN
{
    ACEX_BC_DATA_ARRAY sSnd;         /* data array sender   */

} ACEX_BC_DATA_ARRAY_CHAN;


typedef struct _BC_REPLAY_BUF
{
    U32BIT u32BaseAddr;                 /* memory byte base address         */
    U32BIT u32MemSize;                  /* memory byte size                 */

    U16BIT wDmaCmpltEventCond;          /* DMA complete condition           */

    DDC_CALLBACK waitqueueDmaCmpltCallback;
    DDC_EVENT waitqueueDmaCmpltEvent;   /* DMA complete events wait queue   */

} BC_REPLAY_BUF;


/* BC Buf install settings */
typedef struct _ACEX_BC_BUF_SETTINGS
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32BufferSize;
    U32BIT u32UserIrqMask;
    U32BIT u32DevIrqMask;

} ACEX_BC_BUF_SETTINGS;


typedef struct _ACEX_BC_CTRLWRDS
{
    U16BIT u16CtrlWrd;
    U8BIT u8HostId;
    U8BIT u8Dir; /* Bit 8 - used as direction for Data Stream */

    U16BIT u16CmdWrd;
    U16BIT u16StartAddr1;

    U32BIT u32DataBlkPtr;

    U16BIT u16TimeToNxtMsg;
    U16BIT reserved2;

    U32BIT u32TimeTagWordLsb;

    U16BIT u16BlkStsWord;
    U16BIT u16TimeTagWordMsb;

    U16BIT u16LoopbackWord;
    U16BIT u16MsgIdx;

    U16BIT u16StsWord;
    U16BIT u16DataIdx;

    U16BIT u16CmdWord2;
    U16BIT reserved5;

    U16BIT u16StsWord2;
    U16BIT reserved6;

} ACEX_BC_CTRLWRDS;




struct _ACEX_1553_BC_TYPE
{
    U32BIT *pu32RegBA;                                              /* pointer to 1553 BC Registers base address    */
    U32BIT *pu32RegSize;                                            /* pointer to 1553 BC Register size             */

    U32BIT u32RespTimeout;

    ACEX_MOD_STATE state;           /* BC state */

    U32BIT u32ModeFlag;             /* BC mode flags */
    U32BIT u32BufFlag;              /* Buffer setting flags */

    U32BIT u32IntMask;              /* INT mask */

    U32BIT u32MsgEventMsgIdx;       /* msg event condition - msgIdx */
    U16BIT wMsgEventCond;           /* msg event condition */

    DDC_CALLBACK waitqueueMsgCallback;
    DDC_EVENT waitqueueMsgEvent;    /* msg events wait queue */

    U32BIT u32IrqEvThis;            /* The current user masked IRQ condition */
    U32BIT u32IrqEvLast;            /* The previous user masked IRQ condition */

    U32BIT u32IntCount;             /* INT count */
    U32BIT u32ImpIntCount;          /* IMP INT count */
    U32BIT u32MsgCount;             /* IMP Msg count from regE */
    U32BIT u32MsgCountFromRegB;     /* IMP Msg count from regB */

    U32BIT u32MaxMsg;               /* max Msg count from regE */
    U32BIT u32MaxMsgFromRegB;       /* max Msg count from regB */
    U32BIT u32FullCount;            /* Msg full count */
    S32BIT s32MaxDiff;              /* max Msg count between regB and RegE */
    S32BIT s32MinDiff;              /* min Msg count between regB and RegE */

    U32BIT u32CmdDwords;            /* number of Cmd u32ords */
    U32BIT u32DataDwords;           /* number of data u32ords */
    U32BIT u32CmdDataDwords;        /* number of cmd+data u32ords */

    U32BIT u32CmdWords;             /* number of Cmd words */
    U32BIT u32DataWords;            /* number of data words */
    U32BIT u32CmdDataWords;         /* number of cmd+data words */

    U32BIT u32HBufMsgWords;         /* number of words for HBuf API calls */

    DDC_ISR_LOCK_TYPE slBcMode;     /* BC mode spin lock */
    unsigned long slBcModeFlag;     /* BC mode flag */

    DDC_ISR_LOCK_TYPE slHBuf;       /* HBuf spin lock */
    unsigned long slHBufFlag;       /* HBuf int flag */
    ACEX_BC_HBUF sHBuf;             /* HBuf information */

    DDC_ISR_LOCK_TYPE slMsgBuf;     /* MsgBuf spin lock */
    unsigned long slMsgBufFlag;     /* MsgBuf int flag */
    ACEX_BC_MSGBUF sMsgBuf;         /* msgBuf information */

    DDC_ISR_LOCK_TYPE slDataBuf;    /* DataBuf spin lock */
    unsigned long slDataBufFlag;    /* DataBuf int flag */
    ACEX_BC_DATABUF sDataBuf;       /* DataBuf information */

    ACEX_BC_TEMPBUF sTempBuf;       /* tempBuf used to fetch data from IMP */

    DDC_ISR_LOCK_TYPE slGpq;        /* GPQ spin lock */
    unsigned long slGpqFlag;        /* GPQ int flag */
    ACEX_BC_GPQ sGPQ;               /* GPQ information */

    DDC_ISR_LOCK_TYPE slLpq;        /* LPQ mutex */
    unsigned long slLpqFlag;        /* LPQ int flag */
    ACEX_BC_LPQ sLPQ;               /* LPQ information */
    U16BIT u16LpMsgCount;           /* LP message count */

    DDC_ISR_LOCK_TYPE slHpq;        /* HPQ mutex */
    unsigned long slHpqFlag;        /* HPQ int flag */
    ACEX_BC_HPQ sHPQ;               /* HPQ information */
    U16BIT u16HpMsgCount;           /* HP message count */

    BC_REPLAY_BUF sReplayBuf;       /* Replay memory buffer information */

    DDC_ISR_LOCK_TYPE slDataStr;    /* Data Stream spin lock */
    unsigned long slDataStrFlag;    /* Data Stream int flag */
    ACEX_BC_DATASTR_CHAN sDataStrChan[NUM_DATASTR_CHANNELS];        /* data stream information */

    DDC_ISR_LOCK_TYPE slDataArray;                                  /* Data Array spin lock */
    unsigned long slDataArrayFlag;                                  /* Data Array int flag */
    ACEX_BC_DATA_ARRAY_CHAN sDataArrayChan[NUM_DATARRAY_CHANNELS];  /* data array information */
};

typedef struct _ACEX_1553_REPLAY_TYPE
{
    U32BIT *pu32RegBA;                              /* Pointer to 1553 Replay Registers base address */
    U32BIT *pu32RegSize;                            /* Pointer to 1553 Replay Register size */

    BOOLEAN bReplayIsrEnabled;

    DDC_ISR_LOCK_TYPE semIrqEventCond;
    DDC_ISR_FLAG_TYPE semIrqEventCondFlag;

    U32BIT *pu32IrqStatusQ;                         /* 1553 Replay IRQ status queue */
    U32BIT u32IrqStatusQHead;                       /* 1553 Replay IRQ status queue head */
    U32BIT u32IrqStatusQTail;                       /* 1553 Replay IRQ status queue tail */

    U16BIT u16IrqEventCond;                         /* 1553 Replay IRQ event condition */

    DDC_CALLBACK waitqueueIrqCallback;
    DDC_EVENT waitqueueIrqEvent;                    /* 1553 Replay IRQ events wait queue */ /* IOCTL_BC_REPLAY_BLOCK_ON_IRQ will wait on this queue */

} ACEX_1553_REPLAY_TYPE;


/* -------------------------------------------------------------------------- */
/* BC Async message status information                                        */
/* -------------------------------------------------------------------------- */
typedef struct _ACEX_BC_ASYNC_STATUS
{
    S16BIT s16Status;           /* success or failed    */
    U16BIT u16Count;            /* queued message count */

} ACEX_BC_ASYNC_STATUS;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

struct _DDC_UDL_DEVICE_CONTEXT;


extern void bcInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntStatus,
    U32BIT u32GenIntStatus
);

extern S16BIT bcImpInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32NumEntries
);

extern void bcImpMsgProcess
(
    struct _ACEX_1553_BC_TYPE *pBC,
    U8BIT *pMsg,
    U32BIT u32NumMsgs
);

extern S16BIT bcInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

extern S16BIT bcFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT bcOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcCloseAction
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

extern S16BIT bcClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcSetRespTimeOut
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntMask
);

extern S16BIT bcInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntMask
);

extern void bcSetIrqCondition
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32IntStatus
);

extern S16BIT bcSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcHbufEnable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
);

extern S16BIT bcHbufDisable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pParams
);

extern S16BIT bcGpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcLpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcHpqInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcGetHBufMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcGetMetric
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcSetMsgBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pDfltMsgHeader
);

extern S16BIT bcSeDataBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pDfltData
);

extern S16BIT bcGetMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdMsg
);

extern S16BIT bcGetData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcGetMsgFromId
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcCmdStackRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcDataStackRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcGetDataFromID
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcGetFrameToHBuf
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcPostAsyncMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    void *pRdData
);

extern S16BIT bcGpqClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcGpqGetCount
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcGpqRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U16BIT *pRdData
);

extern S16BIT bcSetDfltMsg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pMsgBlock
);

extern S16BIT bcGetLpAsyncMsgCount
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcImpStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

extern S16BIT bcDataStreamInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcDataStreamCheckCompletion
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcDataArrayInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcDataArrayCheckStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcExtTriggerAvailInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcGetAsyncQueueInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcMuxInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcReplayInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT bcReplayStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT bcReplayDma
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
);

extern S16BIT bcReplayWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
);

extern S16BIT bcReplayRaw
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);


#endif /* _DDC_UDL_1553_BC_PRIVATE_H_ */
