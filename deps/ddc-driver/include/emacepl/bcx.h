/*****************************************************************************
*       Data Device Corporation
*       105 Wilbur Place
*       Bohemia N.Y. 11716
*
*       AceXtreme 'C' Run Time Library
*
*       Copyright (c) 2008 Data Device Corporation
*       All Rights Reserved.
*
*       Filename: bcx.h
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
*   Description:
*       This is the bcx.h Acex header file.
*
*
******************************************************************************/

#ifndef __BCX_H__
#define __BCX_H__

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */
#define REG_BC_CFG1_RW                  0x01
#define REG_BC_START_RESET_W            0x02
#define REG_BC_INSTRUCTION_PTR_R        0x02
#define REG_BC_INT_STATUS_R             0x03
#define REG_BC_FRM_TIME_REMAINING_R     0x04
#define REG_BC_TIME_CFG_W               0x04
#define REG_BC_MSG_TIME_REMAINING_R     0x05
#define REG_BC_INIT_INSTRUCTION_PTR_RW  0x06
#define REG_BC_COND_CODE_R              0x07
#define REG_BC_GP_FLAG_W                0x07
#define REG_BC_GP_QUEUE_CTRL_RW         0x08
#define REG_BC_ASYNC_HP_STATUS_R        0x09
#define REG_BC_ASYNC_HP_CTRL_RW         0x0A
#define REG_BC_TEST1_RW                 0x0B
#define REG_BC_TEST2_RW                 0x0C
#define REG_BC_TEST3_RW                 0x0D
#define REG_BC_ASYNC_LP_STATUS_R        0x0E
#define REG_BC_ASYNC_LP_CTRL_RW         0x0F
#define REG_BC_IMR_TRG_SEL_RW           0x10
#define REG_BC_DBC_CTRL_RW              0x11
#define REG_BC_CFG_TIME_RW              0x12

#define REG_REPLAY_BUFFER_ADDRESS       0x00
#define REG_REPLAY_BUFFER_LENGTH        0x01
#define REG_REPLAY_XT_DISABLE           0x02
#define REG_REPLAY_RT_DISABLE           0x03
#define REG_REPLAY_CONFIG               0x04
#define REG_REPLAY_IRQ_MASK             0x05
#define REG_REPLAY_CONTROL              0x09
#define REG_REPLAY_IRQ_STATUS           0x0A
#define REG_REPLAY_DEV_STATUS           0x0B
#define REG_REPLAY_CURRENT_ADDR         0x0C

#define CFG_BC_MSG_IN_PROGRESS          0x00000001
#define CFG_BC_FRM_IN_PROGRESS          0x00000002
#define CFG_BC_ENABLED                  0x00000004
#define CFG_BC_DOUBLE_RETRY             0x00000008
#define CFG_BC_EXT_TRIG_ENABLED         0x00000080
#define CFG_BC_VALID_BUSY_BIT           0x00000100
#define CFG_BC_VALID_IF_MSG_ERROR       0x00000200
#define CFG_BC_SECOND_RETRY_ALT_BUS     0x00000400
#define CFG_BC_FIRST_RETRY_ALT_BUS      0x00000800
#define CFG_BC_RETRY_IF_STATUS_SET      0x00001000
#define CFG_BC_RETRY_IF_155A_MSG_ERROR  0x00002000
#define CFG_BC_WATCHDOG_TIMER_ENABLE    0x00004000
#define CFG_BC_MODE_ENABLE              0x00008000
#define CFG_BC_TIMETAG_AT_EOM           0x00010000
#define CFG_BC_ALT_BC_MSG_TIMER         0x00020000
#define CFG_BC_EXP_BC_CTL_WORD          0x00040000
#define CFG_BC_RESP_GAP_CHECK           0x20000000
#define CFG_BC_GPQ_SIZE_64              0x40000000
#define CFG_BC_BCST_STATUS_CHECK        0x80000000

#define SR_BC_START                     0x00000002
#define SR_BC_INTERRUPT_RESET           0x00000004
#define SR_BC_STOP                      0x00000008
#define SR_BC_RESUME                    0x00000010
#define SR_BC_PAUSE                     0x00000020

#define ACEX_BC_CFG_TIME_GAP            0x00000001
#define ACEX_BC_CFG_TIME_1US_FRAME      0x00000002

#define ACEX_BC_MAX_FRAME_TIME_1US      0x00FFFFFF      /* 24 bit max */

#define ACEX_BC_DBC_ENABLE              0x00000001
#define ACEX_BC_DBC_ACTIVATE_BY_RT      0x00000002

#define ACE_BC_MSG2_CTRL_OFFSET         0x0010
#define ACE_BC_MSG2_CMD1_OFFSET         0x0011
#define ACE_BC_MSG2_DATA_PTR_OFFSET     0x0012
#define ACE_BC_MSG2_TT_NEXT_MSG_OFFSET  0x0013
#define ACE_BC_MSG2_TIME_TAG_OFFSET     0x0014
#define ACE_BC_MSG2_BLOCK_STATUS_OFFSET 0x0015
#define ACE_BC_MSG2_LOOP_BACK_OFFSET    0x0016
#define ACE_BC_MSG2_RT_STATUS1_OFFSET   0x0017
#define ACE_BC_MSG2_CMD2_OFFSET         0x0018
#define ACE_BC_MSG2_RT_STATUS2_OFFSET   0x0019

#define ACEX_BC_IMR_IMMEDIATE                   0x80000000
#define ACEX_BC_IMR_BREAK                       0x40000000
#define ACEX_BC_IMR_BLK_DATA_INC(SIZE)          ((SIZE & 0xF) << 25)
#define ACEX_BC_IMR_SET_DISCRETE_1              0x00400000
#define ACEX_BC_IMR_SET_DISCRETE_2              0x00200000
#define ACEX_BC_IMR_SET_DISCRETE_3              0x00100000
#define ACEX_BC_IMR_SET_DISCRETE_4              0x00080000
#define ACEX_BC_IMR_RST_DISCRETE_1              0x00040000
#define ACEX_BC_IMR_RST_DISCRETE_2              0x00020000
#define ACEX_BC_IMR_RST_DISCRETE_3              0x00010000
#define ACEX_BC_IMR_RST_DISCRETE_4              0x00008000
#define ACEX_BC_IMR_WAIT_FOR_INPUT_TRIG         0x00004000
#define ACEX_BC_IMR_NO_RESP_BOTH_BUS            0x00002000
#define ACEX_BC_IMR_RST_SRQ_IN_STATUS           0x00001000
#define ACEX_BC_IMR_SET_SRQ_IN_STATUS           0x00000800
#define ACEX_BC_IMR_RST_TFG_IN_STATUS           0x00000400
#define ACEX_BC_IMR_SET_TFG_IN_STATUS           0x00000200
#define ACEX_BC_IMR_RST_BSY_IN_STATUS           0x00000100
#define ACEX_BC_IMR_SET_BSY_IN_STATUS           0x00000080
#define ACEX_BC_IMR_RETRY_SAME_ALT_REMAIN_ALT   0x00000020
#define ACEX_BC_IMR_RETRY_ALT_REMAIN_ALT        0x00000018
#define ACEX_BC_IMR_RETRY_ALT                   0x00000010
#define ACEX_BC_IMR_RETRY_SAME                  0x00000008
#define ACEX_BC_IMR_EXEC_NEXT_MSG_ONCE          0x00000003
#define ACEX_BC_IMR_SKIP_NEXT_MSG_ONCE          0x00000002
#define ACEX_BC_IMR_SKIP_NEXT_MSG               0x00000001

#define ACEX_BC_IMR_BLK_DATA_SIZE_64            1
#define ACEX_BC_IMR_BLK_DATA_SIZE_128           2
#define ACEX_BC_IMR_BLK_DATA_SIZE_256           3
#define ACEX_BC_IMR_BLK_DATA_SIZE_512           4
#define ACEX_BC_IMR_BLK_DATA_SIZE_1024          5
#define ACEX_BC_IMR_BLK_DATA_SIZE_2048          6
#define ACEX_BC_IMR_BLK_DATA_SIZE_4096          7
#define ACEX_BC_IMR_BLK_DATA_SIZE_8192          8
#define ACEX_BC_IMR_BLK_DATA_SIZE_16384         9
#define ACEX_BC_IMR_BLK_DATA_SIZE_32768         10

#define ACEX_BC_IMR_TRIG_EVENT          0x100

#define ACEX_BC_SYNC_PARAM(CHANNELS, DELAY)      (CHANNELS | DELAY)

#define ACEX_BC_SYNC_CH1                0x010000
#define ACEX_BC_SYNC_CH2                0x020000
#define ACEX_BC_SYNC_CH3                0x040000
#define ACEX_BC_SYNC_CH4                0x080000
#define ACEX_BC_SYNC_CH5                0x100000
#define ACEX_BC_SYNC_CH6                0x200000
#define ACEX_BC_SYNC_CH7                0x400000
#define ACEX_BC_SYNC_CH8                0x800000

#define DISABLE_REPLAY_BC               0x0
#define DISABLE_REPLAY_XT               0x1

#define ACEX_BC_REPLAY_CONFIG_MTR               0x0000
#define ACEX_BC_REPLAY_CONFIG_MTI               0x0001
#define ACEX_BC_REPLAY_CONFIG_SKIP_ERR          0x0002
#define ACEX_BC_REPLAY_CONFIG_TRIG_START_ENA    0x0004
#define ACEX_BC_REPLAY_CONFIG_TRIG_STOP_ENA     0x0008

#define ACEX_BC_REPLAY_IRQ_MASK_50_PCT  0x1
#define ACEX_BC_REPLAY_IRQ_MASK_100_PCT 0x2
#define ACEX_BC_REPLAY_IRQ_MASK_COMPL   0x4

#define ACEX_BC_REPLAY_STATUS_RUN       0x1
#define ACEX_BC_REPLAY_STATUS_PAUSE     0x2
#define ACEX_BC_REPLAY_STATUS_STOP      0x4

#define ACEX_BC_REPLAY_OPT_CHAN_ID_ENA      0x0001
#define ACEX_BC_REPLAY_OPT_TRIG_START_ENA   ACEX_BC_REPLAY_CONFIG_TRIG_START_ENA
#define ACEX_BC_REPLAY_OPT_TRIG_STOP_ENA    ACEX_BC_REPLAY_CONFIG_TRIG_STOP_ENA
#define ACEX_BC_REPLAY_OPT_BUFFER_LEN_32K   0x0010
#define ACEX_BC_REPLAY_OPT_BUFFER_LEN_64K   0x0020
#define ACEX_BC_REPLAY_OPT_BUFFER_LEN_128K  0x0040
#define ACEX_BC_REPLAY_OPT_BUFFER_LEN_256K  0x0080
#define ACEX_BC_REPLAY_OPT_BUFFER_LEN_MASK  0x00F0

#define CTRL_REPLAY_START               0x00000001
#define CTRL_REPLAY_PAUSE               0x00000002
#define CTRL_REPLAY_STOP                0x00000004

#define REPLAY_LOCAL_BUFFER_LEN         0x10000         /* 64K bytes */

#define REPLAY_ALIGNMENT_BYTES          8

#define REPLAY_PARSER_SIZE_SMALLER_THAN_HEADER      1
#define REPLAY_PARSER_SIZE_SMALLER_THAN_PACKET      2

#define ACEX_BC_REPLAY_TERM_BYTE        0xFF
#define ACEX_BC_REPLAY_ALIGN_BYTE       0x77

#define BC_DUAL_DATA_DWORD_OFFSET       16

#define BCX_STREAM_MSG_TIME_BCRT_US     680   /* 34 Words * 20us Per Word */
#define BCX_STREAM_MSG_TIME_RTRT_US     720   /* 36 Words * 20us Per Word */

#define BCX_MSG_WORD_TIME_US      20   /* 20us Per Word */

#define ACEX_STREAM_BCRT          0
#define ACEX_STREAM_RTRT          1

#define ACEX_STREAM_WAIT_TIME_MS  20

#define ACEX_BC_STREAM_LP    0
#define ACEX_BC_STREAM_HP    1

#define ACEX_BC_STREAM_DIR_RECEIVE      0
#define ACEX_BC_STREAM_DIR_SEND         1

#define ACEX_BC_STREAM_MTU   1920  /* Max Transmission Unit == 1920 Bytes */

#define ACEX_BC_ARRAY_TYPE_CONT     0
#define ACEX_BC_ARRAY_TYPE_SNGL     1

#define ACEX_BC_ARRAY_LP    0
#define ACEX_BC_ARRAY_HP    1

#define ACEX_BC_MEMITEM_DATA    0
#define ACEX_BC_MEMITEM_MSG     1
#define ACEX_BC_MEMITEM_DWORD   2

#define BC_TYPE_FRAME    0
#define BC_TYPE_SLOT     1

#define BYTES(COUNT, TYPE)   (COUNT * sizeof(TYPE))
#define WORD_ADDRESS(ADDRESS32, OFFSET16)   ((ADDRESS32 << 1) + OFFSET16)

#define IMP_BC_NO_AUTO_POST     0x08000000
#define IMP_BC_PACK_DATA        0x10000000
#define IMP_BC_POST_VALID_ONLY  0x20000000
#define IMP_BC_CMD_ONLY         0x40000000
#define IMP_BC_DATA_ONLY        0x80000000
#define IMP_BC_ADDR_MASK        0x0007FFFF

#define BC_MSG_IMP_CMD_DATA     1
#define BC_MSG_IMP_DATA_ONLY    2

#define ACEX_OPCODE_PARITY          0x8000
#define ACEX_OPCODE_FIXED_MSG_LEN   0x80000000

#define ACEX_OPCODE_WRITE_OPCODE    0x0001
#define ACEX_OPCODE_WRITE_PARAM     0x0010
#define ACEX_OPCODE_WRITE_MSG       0x0020
#define ACEX_OPCODE_WRITE_FRAME     0x0040

#define ACEX_OPCODE_COND_MASK   0x0000003F
#define ACEX_OPCODE_PARAM_MASK  0xFFFFFFFF

#define BC_OPCODE_OFFSET        0
#define BC_PARAM_OFFSET         1
#define BC_OPCODE_BYTES         8

#define BC_RESP_TIME_BIT_OFFSET 19

#define REG_TYPE_BC             1

#define MEM_TYPE_32             0
#define MEM_TYPE_16             1

#define BC_SOFTWARE_OPCODE      0x8000

#define ACEX_FRAME_MINOR_SIZE   14
#define ACEX_FRAME_MAJOR_DEC    9
#define ACEX_FRAME_MAJOR_SIZE   13
#define ACEX_FRAME_OTHER_SIZE   2

#define ACEX_FRAME_MINOR_LWT_OFFSET  1
#define ACEX_FRAME_MINOR_LFT_OFFSET  3
#define ACEX_FRAME_MINOR_WFV_OFFSET  5

#define ACEX_MSGSIZE_BC_DWORDS  26
#define ACEX_MSGSIZE_BC_BYTES   104

/* BC Queue Size Macros */
#define ACEX_MEMSZ_BC_GPQ_256        0x0100 /* BC GPQ 256 dwords long */
#define ACEX_MEMSZ_BC_GPQ_64         0x0040 /* BC GPQ 64 dwords long */
#define ACEX_MEMSZ_BC_LPQ_512        0x0200 /* BC LPQ 512 dwords long */
#define ACEX_MEMSZ_BC_LPQ_256        0x0100 /* BC LPQ 256 dwords long */
#define ACEX_MEMSZ_BC_LPQ_128        0x0080 /* BC LPQ 128 dwords long */
#define ACEX_MEMSZ_BC_LPQ_64         0x0040 /* BC LPQ 64 dwords long */
#define ACEX_MEMSZ_BC_LPQ_32         0x0020 /* BC LPQ 32 dwords long */
#define ACEX_MEMSZ_BC_HPQ_512        0x0200 /* BC HPQ 512 dwords long */
#define ACEX_MEMSZ_BC_HPQ_256        0x0100 /* BC HPQ 256 dwords long */
#define ACEX_MEMSZ_BC_HPQ_128        0x0080 /* BC HPQ 128 dwords long */
#define ACEX_MEMSZ_BC_HPQ_64         0x0040 /* BC HPQ 64 dwords long */
#define ACEX_MEMSZ_BC_HPQ_32         0x0020 /* BC HPQ 32 dwords long */

/* BC Slot Routine Size Macros */
#define ACEX_MEMSZ_BC_SLOT_PARAM     0x0006 /* BC Time Slot Manager 6 dwords long */
#define ACEX_MEMSZ_BC_SLOT_MAIN      0x0030 /* BC Time Slot Manager 48 dwords long */
#define ACEX_MEMSZ_BC_SLOT_RESET     0x0012 /* BC Time Slot Manager 18 dwords long */
#define ACEX_MEMSZ_BC_SLOT_ITEM      0x0004 /* BC Time Slot Item Routine 4 dwords long */

/* BC Configure wOptions parameter can be a GP Queue option "OR's" with a HP Async Queue option "OR'd" with a LP Async Queue option*/
#define ACEX_BC_GPQ_SZ_256              0x0
#define ACEX_BC_GPQ_SZ_64               0x4
#define ACEX_BC_GPQ_SZ_MASK             0x4

#define ACEX_BC_ASYNCQ_SZ_LP_512        0x0
#define ACEX_BC_ASYNCQ_SZ_LP_256        0x10
#define ACEX_BC_ASYNCQ_SZ_LP_128        0x20
#define ACEX_BC_ASYNCQ_SZ_LP_64         0x30
#define ACEX_BC_ASYNCQ_SZ_LP_32         0x40
#define ACEX_BC_ASYNCQ_SZ_LP_MASK       0xF0

#define ACEX_BC_ASYNCQ_SZ_HP_512        0x0
#define ACEX_BC_ASYNCQ_SZ_HP_256        0x100
#define ACEX_BC_ASYNCQ_SZ_HP_128        0x200
#define ACEX_BC_ASYNCQ_SZ_HP_64         0x300
#define ACEX_BC_ASYNCQ_SZ_HP_32         0x400
#define ACEX_BC_ASYNCQ_SZ_HP_MASK       0xF00

#define ACEX_BC_RESP_GAP_CHECK          0x1000
#define ACEX_BC_BCST_STATUS_CHECK       0x2000

#define ACEX_BC_INACTIVE                0x10000 /* inactive BC */
#define ACEX_BC_TO_ACTIVATE             0x20000 /* to activate the inactive BC by RT */
#define ACEX_BC_INTERMSG_GAP_TIME       0x80000 /* gap time between two messages */

#define ACEX_BC_CHAN_MUX_ENA            0x100000 /* mux 1553 channels */

/* Rate Group Slot Manager Memory Offsets */
#define ACEX_OFFSET_SLOT_COUNT       0x0000
#define ACEX_OFFSET_SLOT_INDEX       0x0001
#define ACEX_OFFSET_SLOT_PTR_BASE    0x0002
#define ACEX_OFFSET_SLOT_PTR_CURRENT 0x0003
#define ACEX_OFFSET_SLOT_CYCLE_COUNT 0x0004
#define ACEX_OFFSET_SLOT_CYCLE_DEC   0x0000

/* BC external trigger information */
#define ACE_BC_EXT_TRIGGER_NOT_AVAIL            0x00000000
#define ACE_BC_EXT_TRIGGER_AVAIL                0x00000001
#define ACE_BC_EXT_TRIGGER_USING_DIO            0x00010000
#define ACE_BC_EXT_TRIGGER_USING_DIRECT_INPUT   0x00020000


typedef struct _GPQ_STAT
{
    U32BIT u32Head;     /* Next unread location of onboard GPQ */
    U32BIT u32Lost;     /* # of possible GPQ entries lost */
    U16BIT u16PctFull;  /* Current Percentage of GPQ used */
    U16BIT u16HighPct;  /* Highest Percentage of GPQ used */
} GPQ_STAT;

typedef struct _GPQ_LIST
{
    U16BIT u16GPQHead;                  /* index to the next unread entry */
    U16BIT u16GPQTail;                  /* index to the next free location */
    U32BIT aGPQ[ACEX_MEMSZ_BC_GPQ_256]; /* the buffered queue, 256 used for both 64 and 256 entry queues */
} GPQ_LIST;

/* BC Async Message Status Information */
typedef struct _ACEX_BC_ASYNC_STATUS
{
    S16BIT s16Status;           /* success or failed */
    U16BIT u16Count;            /* queued message count */
} ACEX_BC_ASYNC_STATUS;

/* BC Async Message Queue Information */

/* ========================================================================== */
/*                            ACEX FUNCTION PROTOTYPES                             */
/* ========================================================================== */
S16BIT _DECL _acexBCConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _acexBCConfigureReplay
(
    S16BIT DevNum,
    U32BIT u32RtDisable,
    BOOLEAN bXtDisable,
    BOOLEAN bBcDisable,
    BOOLEAN bMtrErrDisable,
    U16BIT u16TTRes,
    U16BIT u16ChannelId,
    U32BIT u32Options
);

S16BIT _DECL _acexBCMsgGapTimerEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _acexBCDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL _acexBCDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL _acexBCDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _acexBCDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _acexBCDataBlkRead32
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U32BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _acexBCFrameCreate
(
    S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime,
    U16BIT wFlags
);

_EXTERN S16BIT _DECL _acexBCExFrameCreate
(
    S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U32BIT dwMnrFrmTime,
    U16BIT wFlags
);

_EXTERN S16BIT _DECL _acexBCFrameDelete
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN S16BIT _DECL _acexBCOpCodeCreate
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL _acexBCOpCodeDelete
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL _acexBCMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL _acexBCMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgCreateMode
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

_EXTERN S16BIT _DECL _acexBCMsgCreateBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL _acexBCMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgCreateBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL _acexBCMsgCreate
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

_EXTERN S16BIT _DECL _acexBCMsgModifyBCtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgModifyRTtoBC
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

_EXTERN S16BIT _DECL _acexBCMsgModifyRTtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgModifyMode
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

_EXTERN S16BIT _DECL _acexBCMsgModifyBcst
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

_EXTERN S16BIT _DECL _acexBCMsgModifyBcstRTtoRT
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

_EXTERN S16BIT _DECL _acexBCMsgModifyBcstMode
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

_EXTERN S16BIT _DECL _acexBCMsgModify
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

_EXTERN S16BIT _DECL _acexBCMsgDelete
(
    S16BIT DevNum,
    S16BIT nMsgBlkID
);

_EXTERN S16BIT _DECL _acexBCStart
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    S32BIT lMjrFrmCount
);

_EXTERN S16BIT _DECL _acexBCStartReplay
(
    S16BIT DevNum,
    const CHAR *pFileName,
    S32BIT s32LoopCount
);

_EXTERN S16BIT _DECL _acexBCStartReplayRaw
(
    S16BIT DevNum,
    PVOID  *pReplayData,
    U32BIT u32DataLength
);

_EXTERN S16BIT _DECL _acexBCStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCCreateImageFiles
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL _acexBCSetGPFState
(
    S16BIT DevNum,
    U16BIT wGPF,
    U16BIT wStateChange
);

_EXTERN S16BIT _DECL _acexBCGetConditionCode
(
    S16BIT DevNum,
    U16BIT wConditionCode,
    U16BIT* pCurrentState
);

_EXTERN S16BIT _DECL _acexBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _acexBCGetMsgFromIDRawEx
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL _acexBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _acexBCGetMsgHdrFromIDRawEx
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL _acexBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _acexBCGetMsgFromIDDecodedEx
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL _acexBCDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL _acexBCInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL _acexBCUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCGPQRead
(
    S16BIT DevNum,
    GPQENTRY* pGPQEntry
);

_EXTERN S16BIT _DECL _acexBCGPQGetCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCFrmToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCFrmToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _acexBCGetHBufMsgsRawEx
(
    S16BIT DevNum,
    U16BIT *pBufferEx,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL _acexBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _acexBCGetHBufMsgDecodedEx
(
    S16BIT DevNum,
    U16BIT    *pMsgId,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc,
    U16BIT bBlocking
);

_EXTERN S16BIT _DECL _acexBCSetMsgRetry
(
    S16BIT DevNum,
    U16BIT wNumOfRetries,
    U16BIT wFirstRetryBus,
    U16BIT wSecondRetryBus,
    U16BIT wReserved
);

_EXTERN S16BIT _DECL _acexBCSetWatchDogTimer
(
    S16BIT DevNum,
    U16BIT bEnable,
    U16BIT wTimeOut
);

_EXTERN S16BIT _DECL _acexBCGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _acexBCGetGPQMetric
(
    S16BIT DevNum,
    GPQMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateMode
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateBcst
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL _acexBCAsyncMsgCreateBcstMode
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

_EXTERN S16BIT _DECL _acexBCSendAsyncMsgLP
(
    S16BIT DevNum,
    U16BIT *pMsgLeft,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL _acexBCSendAsyncMsgHP
(
    S16BIT DevNum,
    U16BIT nMsgID,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL _acexBCEmptyAsyncList
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCResetAsyncPtr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCGetAsyncCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexBCDisableMessage
(
    S16BIT DevNum,
    U16BIT wMsgNum,
    BOOLEAN bDisable
);

_EXTERN S16BIT _DECL _acexBCInitialize
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCFree
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCTimeSlotCreate
(
    S16BIT s16DevNum,
    U16BIT u16SlotTime,
    U16BIT u16SlotCount,
    U32BIT u16IrqMask
);

_EXTERN S16BIT _DECL _acexBCTimeSlotItemCreate
(
    S16BIT s16DevNum,
    S16BIT s16SlotItemId,
    S16BIT s16MsgId,
    U16BIT u16SlotStart,
    U16BIT u16SlotInterval
);

_EXTERN S16BIT _DECL _acexBCGetCurrTimeSlot
(
    S16BIT s16DevNum,
    U16BIT *pTimeSlot
);

_EXTERN S16BIT _DECL _acexBCStart
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    S32BIT lMjrFrmCount
);

_EXTERN S16BIT _DECL _acexBCStartEx
(
    S16BIT s16DevNum,
    S16BIT s16MjrFrmID,
    S32BIT s32CycleCount,
    U16BIT u16Type
);

_EXTERN S16BIT _DECL _acexBCDataStreamCreateBCRT
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr,
    U32BIT u32SAMask,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL _acexBCDataStreamCreateRTRT
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16RTAddr1,
    U32BIT u32SAMask1,
    U16BIT u16RTAddr2,
    U32BIT u32SAMask2,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL _acexBCDataStreamDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId
);

_EXTERN S16BIT _DECL _acexBCDataStreamSend
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL _acexBCDataStreamReceive
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U32BIT u32TimeoutMs
);

_EXTERN S16BIT _DECL _acexBCDataArrayCreateBCtoRT
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16RTAddr,
    U16BIT u16RTSubAddr,
    U16BIT u16Transfer,
    U16BIT u16WordCount,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL _acexBCDataArrayDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId
);

_EXTERN S16BIT _DECL _acexBCDataArraySend
(
    S16BIT s16DevNum,
    S16BIT s16DataArrayId,
    U16BIT u16Priority,
    VOID *pBuffer,
    U16BIT u16BufferLen,
    U16BIT u16WordCount
);

_EXTERN S16BIT _DECL _acexBCGetStatus
(
    S16BIT s16DevNum,
    U16BIT  *u16Status
);

_EXTERN S16BIT _DECL _acexBCGetStatusReplay
(
    S16BIT s16DevNum,
    U32BIT  *u32Status
);

_EXTERN S16BIT _DECL _acexBCExtTriggerEnable
(
    S16BIT s16DevNum,
    BOOLEAN bOnStart
);

_EXTERN S16BIT _DECL _acexBCExtTriggerDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCAsyncMsgSendLP
(
    S16BIT s16DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL _acexBCAsyncMsgSendHP
(
    S16BIT s16DevNum,
    U16BIT s16MsgId,
    BOOLEAN bFlushMsg,
    U16BIT *pu16QueueCount
);

_EXTERN S16BIT _DECL _acexBCAsyncQueueInfoLP
(
    S16BIT s16DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL _acexBCAsyncQueueInfoHP
(
    S16BIT s16DevNum,
    ACEX_BC_ASYNC_QUEUE_INFO *psQueueInfo
);

_EXTERN S16BIT _DECL _acexBCMemWrdCreate
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL _acexBCMemWrdDelete
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId
);

_EXTERN S16BIT _DECL _acexBCMemWrdRead
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT *pu32MemWord
);

_EXTERN S16BIT _DECL _acexBCMemWrdWrite
(
    S16BIT s16DevNum,
    S16BIT s16MemWrdId,
    U32BIT u32MemWord
);

_EXTERN S16BIT _DECL _acexBCMemObjCreate
(
    S16BIT s16DevNum,
    S16BIT s16MemObjId,
    S16BIT s16MemItemId,
    U16BIT u16MemItemType,
    U16BIT u16Offset
);

_EXTERN S16BIT _DECL _acexBCMemObjDelete
(
    S16BIT s16DevNum,
    S16BIT s16MemObjId
);

_EXTERN S16BIT _DECL _acexBCSetMsgError
(
    S16BIT s16DevNum,
    S16BIT s16MsgID,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL _acexBCMsgErrorEnable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCMsgErrorDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCSetRespTimeout
(
    S16BIT s16DevNum,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL _acexBCOpCodeRead
(
    S16BIT s16DevNum,
    S16BIT s16OpCodeID,
    U32BIT *pu32OpCode,
    U32BIT *pu32Param
);

_EXTERN S16BIT _DECL _acexBCOpCodeWrite
(
    S16BIT s16DevNum,
    S16BIT s16OpCodeID,
    U16BIT u16Selection,
    U32BIT u32OpCode,
    U32BIT u32Param
);

_EXTERN S16BIT _DECL _acexBCDbcEnable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCDbcDisable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCImrTrigSelect
(
    S16BIT s16DevNum,
    U16BIT u16Select
);

_EXTERN S16BIT _DECL _acexBCPause
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexBCContinue
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _bcxReplay
(
    S16BIT DevNum
);

#endif /* __BCX_H__ */
