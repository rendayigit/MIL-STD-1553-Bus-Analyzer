/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 2006 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  BCop.h (Bus Controller operations header file)
 *
 *
 *
 *  Created 8/26/99 DL
 *  Updated 8/01/05 ESK Updated for Integrity v4 and v5.
 *
 *  _aceBCConfigure
 *  _aceBCCreateImageFiles
 *  _aceBCDataBlkCreate
 *  _aceBCDataBlkDelete
 *  _aceBCDataBlkRead
 *  _aceBCDataBlkWrite
 *  _aceBCDecodeRawMsg
 *  _aceBCFrameCreate
 *  _aceBCFrameDelete
 *  _aceBCFrmToHBuf
 *  _aceBCGetConditionCode
 *  _aceBCGetHBufMsgDecoded
 *  _aceBCGetHBufMsgsRaw
 *  _aceBCGetMsgFromIDDecoded
 *  _aceBCGetMsgFromIDRaw
 *  _aceBCGPQRead
 *  _aceBCGPQGetCount
 *  _aceBCInstallHBuf
 *  _aceBCMsgCreate
 *  _aceBCMsgDelete
 *  _aceBCMsgCreateBCtoRT
 *  _aceBCMsgCreateRTtoBC
 *  _aceBCMsgCreateRTtoRT
 *  _aceBCMsgCreateMode
 *  _aceBCMsgCreateBcst
 *  _aceBCMsgCreateBcstRTtoRT
 *  _aceBCMsgCreateBcstMode
 *  _aceBCMsgGapTimerEnable
 *  _aceBCOpCodeCreate
 *  _aceBCOpCodeDelete
 *  _aceBCSetGPFState
 *  _aceBCSetMsgRetry
 *  _aceBCSetWatchDogTimer
 *  _aceBCStart
 *  _aceBCStop
 *  _aceBCUninstallHBuf
 *  _aceBCDisableMessage
 *
 */

#ifndef __BCOP_H__
#define __BCOP_H__

#ifdef INTEGRITY
#define BOOL unsigned char
#endif /* ifdef INTEGRITY */

/* BC status macros*/
#define ACE_BC_STATUS_POLLING_COUNT     5
#define ACE_BC_STOP_POLLING_COUNT       5

#define ACE_BC_IDLE               0
#define ACE_BC_BUSY               1

/* BC Configuration Options */

/* Modify Options */
#define ACE_BC_MOD_DBLK1          0x0001
#define ACE_BC_MOD_BCCTRL1        0x0002
#define ACE_BC_MOD_CMDWRD1_1      0x0004
#define ACE_BC_MOD_CMDWRD1_2      0x0008
#define ACE_BC_MOD_GAPTIME1       0x0010
#define ACE_BC_MOD_DBLK2          0x0020
#define ACE_BC_MOD_BCCTRL2        0x0040
#define ACE_BC_MOD_CMDWRD2_1      0x0080
#define ACE_BC_MOD_CMDWRD2_2      0x0100
#define ACE_BC_MOD_GAPTIME2       0x0200

/* Frame Flag Options */
#define ACE_BC_MNRFRM_IRQ_DISABLE 0x0001

/* Retry constants */
#define ACE_RETRY_NONE      0
#define ACE_RETRY_ONCE      1
#define ACE_RETRY_TWICE     2
#define ACE_RETRY_SAME      0
#define ACE_RETRY_ALT       1

/* dwoptions parameter can be any of the following */
#define ACE_BC_ASYNC_LMODE 0x00000001
#define ACE_BC_ASYNC_HMODE 0x00000002
#define ACE_BC_ASYNC_BOTH  0x00000003

/* wMsgOptions parameter can be any of the following bit "OR'd" together*/
#define ACE_MSGOPT_MASK                 0xFFFF0000

#define ACE_MSGOPT_DOUBLE_BUFFER        0x80000000
#define ACE_MSGOPT_STAY_ON_ALT          0x40000000
#define ACE_MSGOPT_MODE_SA31            0x20000000

/*#define ACE_MSGOPT_INTERMSG_GAP_TIME    0x10000000*/

/* wDataBlkSize can be 1-32 or one of the following */
#define ACE_BC_DBLK_SINGLE      32  /* single data block of 32 words */
#define ACE_BC_DBLK_DOUBLE      33  /* double buffered data block 64 */

/* data block size in DWORDS */
#define ACE_BC_DBLK_32                      0x0020
/* Additional multi-function data block sizes for inter-message routine use */
#define ACE_BC_DBLK_64                      0x0040
#define ACE_BC_DBLK_128                     0x0080
#define ACE_BC_DBLK_256                     0x0100
#define ACE_BC_DBLK_512                     0x0200
#define ACE_BC_DBLK_1024                    0x0400
#define ACE_BC_DBLK_2048                    0x0800
#define ACE_BC_DBLK_4096                    0x1000
#define ACE_BC_DBLK_8192                    0x2000
#define ACE_BC_DBLK_16384                   0x4000
#define ACE_BC_DBLK_32768                   0x8000

/* wBCCtrlWrd can be any of the following OR'd together */
#define ACE_BCCTRL_RT_TO_RT                 0x0001
#define ACE_BCCTRL_BCST                     0x0002
#define ACE_BCCTRL_MODE                     0x0004
#define ACE_BCCTRL_1553A                    0x0008
#define ACE_BCCTRL_1553B                    0x0000
#define ACE_BCCTRL_EOM_IRQ                  0x0010
#define ACE_BCCTRL_BCST_MSK                 0x0020
#define ACE_BCCTRL_SELFTST                  0x0040
#define ACE_BCCTRL_CHL_A                    0x0080
#define ACE_BCCTRL_CHL_B                    0x0000
#define ACE_BCCTRL_RETRY_ENA                0x0100
#define ACE_BCCTRL_RES_MSK                  0x0200
#define ACE_BCCTRL_DBC_MSK                  0x0200
#define ACE_BCCTRL_INSTR_MSK                0x0200
#define ACE_BCCTRL_TFLG_MSK                 0x0400
#define ACE_BCCTRL_SSFLG_MSK                0x0800
#define ACE_BCCTRL_SSBSY_MSK                0x1000
#define ACE_BCCTRL_SREQ_MSK                 0x2000
#define ACE_BCCTRL_ME_MSK                   0x4000
#define ACE_BCCTRL_TTTSYNCMODE_MSK          0x8000

/* BC message item offsets */
#define ACE_BC_MSG_CTRL_OFFSET          0x0000
#define ACE_BC_MSG_CMD1_OFFSET          0x0001
#define ACE_BC_MSG_DATA_PTR_OFFSET      0x0002
#define ACE_BC_MSG_TT_NEXT_MSG_OFFSET   0x0003
#define ACE_BC_MSG_TIME_TAG_OFFSET      0x0004
#define ACE_BC_MSG_BLOCK_STATUS_OFFSET  0x0005
#define ACE_BC_MSG_LOOP_BACK_OFFSET     0x0006
#define ACE_BC_MSG_RT_STATUS1_OFFSET    0x0007
#define ACE_BC_MSG_CMD2_OFFSET          0x0008
#define ACE_BC_MSG_RT_STATUS2_OFFSET    0x0009

/* BC block status word bits */
#define ACE_BC_BSW_INVWRD           0x0001
#define ACE_BC_BSW_BADSYNC          0x0002
#define ACE_BC_BSW_WCERR            0x0004
#define ACE_BC_BSW_WRGADDR_NGAP     0x0008
#define ACE_BC_BSW_GDATA            0x0010
#define ACE_BC_BSW_RETRY            0x0060
#define ACE_BC_BSW_MSKED_STS        0x0080
#define ACE_BC_BSW_LPFAIL           0x0100
#define ACE_BC_BSW_NORES            0x0200
#define ACE_BC_BSW_FMTERR           0x0400
#define ACE_BC_BSW_STS_SET          0x0800
#define ACE_BC_BSW_ERRFLG           0x1000
#define ACE_BC_BSW_CHNL             0x2000
#define ACE_BC_BSW_SOM              0x4000
#define ACE_BC_BSW_EOM              0x8000

/* Hardware opcodes */
#define ACE_OPCODE_TRP      0x0000  /* Trap (bad sequence) */
#define ACE_OPCODE_XEQ      0x0001  /* Execute Message */
#define ACE_OPCODE_JMP      0x0002  /* Jump (inst offset within frame) */
#define ACE_OPCODE_CAL      0x0003  /* Call subroutine (frame) */
#define ACE_OPCODE_RTN      0x0004  /* Return from subroutine (frame) */
#define ACE_OPCODE_IRQ      0x0006  /* Irq request (user ISR2 bits set) */
#define ACE_OPCODE_HLT      0x0007  /* Halt the BC */
#define ACE_OPCODE_DLY      0x0008  /* Delay in microseconds */
#define ACE_OPCODE_WFT      0x0009  /* Wait until frm counter = 0 */
#define ACE_OPCODE_CFT      0x000A  /* Compare frm timer to value */
#define ACE_OPCODE_FLG      0x000C  /* Set,clear,toggle 8 GP bits */
#define ACE_OPCODE_LTT      0x000D  /* Load time tag counter */
#define ACE_OPCODE_LFT      0x000E  /* Load frame time register */
#define ACE_OPCODE_SFT      0x000F  /* Start frame time counter */
#define ACE_OPCODE_PTT      0x0010  /* Push tt reg on queue */
#define ACE_OPCODE_PBS      0x0011  /* Push BSW on queue */
#define ACE_OPCODE_PSI      0x0012  /* Push data on queue */
#define ACE_OPCODE_PSM      0x0013  /* Push memory on queue */
#define ACE_OPCODE_WTG      0x0014  /* Wait for external trigger */
#define ACE_OPCODE_XQF      0x0015  /* Execute/Flip message */
#define ACE_OPCODE_HUB      0x0016  /* Hub switching for EBR */

/* Flexcore enhanced OpCodes */
#define ACE_OPCODE_GRP      0x0016  /* Group Condition Codes */
#define ACE_OPCODE_RDM      0x0017  /* Read Memory */
#define ACE_OPCODE_CPM      0x0018  /* Compare Memory */
#define ACE_OPCODE_INC      0x0019  /* Increment Memory */
#define ACE_OPCODE_DEC      0x001A  /* Decrement Memory */
#define ACE_OPCODE_WRM      0x001B  /* Write Memory */
#define ACE_OPCODE_LIA      0x001C  /* Load Immediate Accumulator A */

/* AceX enhanced OpCodes */
#define ACE_OPCODE_WFV      0x001E  /* Wait For Frame Timer Value */
#define ACE_OPCODE_AMT      0x0021  /* Asynchronous Message Queue Trigger */
#define ACE_OPCODE_CMM      0x0022  /* Compare Accumulator A with Accumulator B, using mask bits given in CMM param1  */
#define ACE_OPCODE_LWT      0x0023  /* Load Watchdog timer, param1 = 17-bit value in 100 us  */
#define ACE_OPCODE_LIB      0x0024  /* Load Immediate Accumulator B */

/* Acex Multi-Function OpCodes */
#define ACE_OPCODE_IMR      0x0025  /* Inter-message routine */
#define ACE_OPCODE_PAU      0x0026  /* Pause */
#define ACE_OPCODE_SYN      0x0027  /* Channel Synchronization Opcode */

/* Software opcodes - use negative IDs */
/* 0x8000 to 0x8FFF have 2 hardware opcodes */
/* 0x9000 to 0x9FFF have 3 hardware opcodes */
/* 0xA000 to 0xAFFF have 4 hardware opcodes */
/* 0xB000 to 0xBFFF have 5 hardware opcodes */
/* 0xC000 to 0xCFFF have 6 hardware opcodes */
/* 0xD000 to 0xDFFF have 7 hardware opcodes */
/* 0xE000 to 0xEFFF have 8 hardware opcodes */
/* 0xF000 to 0xFFFF have 9 hardware opcodes */
#define ACE_S_OPCODE_AMSG   0xC000  /* CAL,CMP,RET,XEQ,CLR,RET */
#define ACE_S_OPCODE_IMSG   0xA000  /* XEQ,PSI,PSI,IRQ */

/* wCondition can be any of the following */
#define ACE_XQF_CND_MASK        0x000F  /* XQF cnd test mask */

#define ACE_CNDTST_LT           0x0000  /* set or cleared by CFT */
#define ACE_CNDTST_GT           0x0010  /* set or cleared by CFT */
#define ACE_CNDTST_EQ           0x0001  /* set or cleared by CFT */
#define ACE_CNDTST_NEQ          0x0011  /* set or cleared by CFT */
#define ACE_CNDTST_GP1_1        0x0001  /* executes if GP1 is logic 1 */
#define ACE_CNDTST_GP1_0        0x0011  /* executes if GP1 is logic 0 */
#define ACE_CNDTST_GP2_1        0x0002  /* executes if GP2 is logic 1 */
#define ACE_CNDTST_GP2_0        0x0012  /* executes if GP2 is logic 0 */
#define ACE_CNDTST_GP3_1        0x0003  /* executes if GP3 is logic 1 */
#define ACE_CNDTST_GP3_0        0x0013  /* executes if GP3 is logic 0 */
#define ACE_CNDTST_GP4_1        0x0004  /* executes if GP4 is logic 1 */
#define ACE_CNDTST_GP4_0        0x0014  /* executes if GP4 is logic 0 */
#define ACE_CNDTST_GP5_1        0x0005  /* executes if GP5 is logic 1 */
#define ACE_CNDTST_GP5_0        0x0015  /* executes if GP5 is logic 0 */
#define ACE_CNDTST_GP6_1        0x0006  /* executes if GP6 is logic 1 */
#define ACE_CNDTST_GP6_0        0x0016  /* executes if GP6 is logic 0 */
#define ACE_CNDTST_GP7_1        0x0007  /* executes if GP7 is logic 1 */
#define ACE_CNDTST_GP7_0        0x0017  /* executes if GP8 is logic 0 */
#define ACE_CNDTST_NO_RES       0x0008  /* indicates no response by RT */
#define ACE_CNDTST_RES          0x0018  /* indicates response by RT */
#define ACE_CNDTST_FMT_ERR      0x0009  /* indicates a format err */
#define ACE_CNDTST_NO_FMT_ERR   0x0019  /* indicated a no format err */
#define ACE_CNDTST_GD_XFER      0x000A  /* indicates good dblk xfer */
#define ACE_CNDTST_BAD_XFER     0x001A  /* indicates bad dblk xfer */
#define ACE_CNDTST_MSK_STS_SET  0x000B  /* indicates masked status set */
#define ACE_CNDTST_MSK_STS_CLR  0x001B  /* indicates masked status clred*/
#define ACE_CNDTST_BAD_MSG      0x000C  /* indicates a bad message */
#define ACE_CNDTST_GOOD_MSG     0x001C  /* indicates a good message */
#define ACE_CNDTST_0RETRY       0x001D  /* indicates 0 retries */
#define ACE_CNDTST_1RETRY       0x000D  /* indicates 1 retry  */
#define ACE_CNDTST_2RETRY       0x000E  /* indicates 2 retries*/
#define ACE_CNDTST_ALWAYS       0x000F  /* always run opcode */
#define ACE_CNDTST_NEVER        0x001F  /* never run opcode */
#define ACE_CNDTST_GP8_1        0x0020  /* executes if GP8 is logic 1 */
#define ACE_CNDTST_GP8_0        0x0030  /* executes if GP8 is logic 0 */
#define ACE_CNDTST_GP9_1        0x0021  /* executes if GP9 is logic 1 */
#define ACE_CNDTST_GP9_0        0x0031  /* executes if GP9 is logic 0 */
#define ACE_CNDTST_GP10_1       0x0022  /* executes if GP10 is logic 1 */
#define ACE_CNDTST_GP10_0       0x0032  /* executes if GP10 is logic 0 */
#define ACE_CNDTST_GP11_1       0x0023  /* executes if GP11 is logic 1 */
#define ACE_CNDTST_GP11_0       0x0033  /* executes if GP11 is logic 0 */
#define ACE_CNDTST_GP12_1       0x0024  /* executes if GP12 is logic 1 */
#define ACE_CNDTST_GP12_0       0x0034  /* executes if GP12 is logic 0 */
#define ACE_CNDTST_GP13_1       0x0025  /* executes if GP13 is logic 1 */
#define ACE_CNDTST_GP13_0       0x0035  /* executes if GP13 is logic 0 */
#define ACE_CNDTST_GP14_1       0x0026  /* executes if GP14 is logic 1 */
#define ACE_CNDTST_GP14_0       0x0036  /* executes if GP14 is logic 0 */
#define ACE_CNDTST_GP15_1       0x0027  /* executes if GP15 is logic 1 */
#define ACE_CNDTST_GP15_0       0x0037  /* executes if GP15 is logic 0 */

/* wMsgLoc parameter can be any one of the following */
#define ACE_BC_MSGLOC_NEXT_PURGE    0   /* next unread msg, clr stk */
#define ACE_BC_MSGLOC_NEXT_NPURGE   1   /* next unread msg, leave stk */
#define ACE_BC_MSGLOC_LATEST_PURGE  2   /* latest msg, clear stk */
#define ACE_BC_MSGLOC_LATEST_NPURGE 3   /* latest msg, leave stk alone */

/* wConditionCode can be any one of the following */
#define ACE_CND_LT_GT       0
#define ACE_CND_EQ_NE       1
#define ACE_CND_GPF0        0
#define ACE_CND_GPF1        1
#define ACE_CND_GPF2        2
#define ACE_CND_GPF3        3
#define ACE_CND_GPF4        4
#define ACE_CND_GPF5        5
#define ACE_CND_GPF6        6
#define ACE_CND_GPF7        7
#define ACE_CND_NORES       8
#define ACE_CND_FMT_ERR     9
#define ACE_CND_GD_DATA     10
#define ACE_CND_MSKED_STS   11
#define ACE_CND_BAD_MSG     12
#define ACE_CND_RETRY       13
#define ACE_CND_ALWAYS      15
#define ACE_CND_GPF8        16
#define ACE_CND_GPF9        17
#define ACE_CND_GPF10       18
#define ACE_CND_GPF11       19
#define ACE_CND_GPF12       20
#define ACE_CND_GPF13       21
#define ACE_CND_GPF14       22
#define ACE_CND_GPF15       23

/* wFrameType can be any one of the following */
#define ACE_FRAME_MAJOR     0x0000
#define ACE_FRAME_OTHER     0x0001
#define ACE_FRAME_MINOR     0x0002

/* wGPF can br any one of the following */
#define ACE_GPF0            0x0000
#define ACE_GPF1            0x0001
#define ACE_GPF2            0x0002
#define ACE_GPF3            0x0003
#define ACE_GPF4            0x0004
#define ACE_GPF5            0x0005
#define ACE_GPF6            0x0006
#define ACE_GPF7            0x0007

/* additional 8 bit flags (bit16 - bit23) for 32bit mode */
#define ACE_GPF8            0x0010
#define ACE_GPF9            0x0011
#define ACE_GPF10           0x0012
#define ACE_GPF11           0x0013
#define ACE_GPF12           0x0014
#define ACE_GPF13           0x0015
#define ACE_GPF14           0x0016
#define ACE_GPF15           0x0017

/* wStateChange for GPFs can be one of the following */
#define ACE_GPF_LEAVE       0x0000
#define ACE_GPF_SET         0x0001
#define ACE_GPF_CLEAR       0x0002
#define ACE_GPF_TOGGLE      0x0003

/* General Purpose Queue constants */
#define ACE_GPQ_MJR_EOF         0xFFFF
#define ACE_GPQ_MNR_EOF         0xFFF8
#define ACE_GPQ_MRR_TIME_EXP    0xFFF1
#define ACE_GPQ_ASYNC_EOF       0xFFF0

/* Maximum number of async messages */
#define MAX_NUM_ASYNC_MSGS  0x400

/* Internal BC Msg Modify Options */
#define ACE_BC_MSG_OPT_NONE         0x0
#define ACE_BC_MSG_OPT_HEADER_ONLY  0x1

#define ACE_BC_DUAL_MSG1_OFFSET     0x0000  /* Dual message 1 */
#define ACE_BC_DUAL_MSG2_OFFSET     0x0010  /* Dual message 2 */

/*-----------------------------------------------------------------------
Struct: HBUFMETRIC

Description:
    This structure defines valid performance information for a Host Buf
------------------------------------------------------------------------*/
typedef struct GPQMETRIC
{
    U32BIT dwLost;      /* Total number of msgs lost since install */
    U16BIT wPctFull;    /* Current Percentage of HBuf used */
    U16BIT wHighPct;    /* Highest Percentage of HBuf used */
} GPQMETRIC;

/*-----------------------------------------------------------------------
Struct: GPQ

Description:
    This structure defines a general purpose queue.
------------------------------------------------------------------------*/
typedef struct GPQ
{
    U16BIT wHead;       /* Next unread location of onboard GPQ */
    U32BIT dwLost;      /* # of possible GPQ entries lost */
    U16BIT wPctFull;    /* Current Percentage of GPQ used */
    U16BIT wHighPct;    /* Highest Percentage of GPQ used */
} GPQ;

/* A single entry from the BC General Purpose Queue */
typedef struct GPQENTRY
{
    U16BIT wGPQHeader;          /* gives information on what data is */
    U16BIT wGPQData;            /* the information for the entry */
} GPQENTRY;

/* A single 32-bit entry from the BC General Purpose Queue */
typedef struct GPQENTRY32
{
    U32BIT dwGPQHeader;         /* gives information on what data is */
    U32BIT dwGPQData;           /* the information for the entry */
} GPQENTRY32;

/* BC OpCode information structure */
typedef struct OPCODESTRUCT
{
    S16BIT nOpCodeID;           /* the unique ID of the OpCode */
    U16BIT wOpCodeType;         /* the type of opcode (constant)*/
    U16BIT wParameter;          /* Parameter of opcode */
    U16BIT wCondition;          /* Condition of opcode */
} OPCODESTRUCT;

_EXTERN S16BIT _DECL _aceBCConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _aceBCMsgGapTimerEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _aceBCDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL _aceBCDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL _aceBCDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _aceBCDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _aceBCDataBlkRead32
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U32BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wOffset
);

_EXTERN S16BIT _DECL _aceBCFrameCreate(S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime,
    U16BIT wFlags);

_EXTERN S16BIT _DECL _aceBCFrameDelete
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN S16BIT _DECL _aceBCOpCodeCreate
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL _aceBCOpCodeDelete
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL _aceBCMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL _aceBCMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgCreateMode
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

_EXTERN S16BIT _DECL _aceBCMsgCreateBcst
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wWC,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL _aceBCMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgCreateBcstMode
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    S16BIT nDataBlkID,
    U16BIT wTR,
    U16BIT wModeCmd,
    U16BIT wMsgGapTime,
    U32BIT dwMsgOptions
);

_EXTERN S16BIT _DECL _aceBCMsgCreate
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

_EXTERN S16BIT _DECL _aceBCMsgModifyBCtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgModifyRTtoBC
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

_EXTERN S16BIT _DECL _aceBCMsgModifyRTtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgModifyMode
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

_EXTERN S16BIT _DECL _aceBCMsgModifyBcst
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

_EXTERN S16BIT _DECL _aceBCMsgModifyBcstRTtoRT
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

_EXTERN S16BIT _DECL _aceBCMsgModifyBcstMode
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

_EXTERN S16BIT _DECL _aceBCMsgModify
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

_EXTERN S16BIT _DECL _aceBCMsgDelete
(
    S16BIT DevNum,
    S16BIT nMsgBlkID
);

_EXTERN S16BIT _DECL _aceBCStart
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    S32BIT lMjrFrmCount
);

_EXTERN S16BIT _DECL _aceBCStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCCreateImageFiles
(
    S16BIT DevNum,
    S16BIT nMjrFrmID,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL _aceBCSetGPFState
(
    S16BIT DevNum,
    U16BIT wGPF,
    U16BIT wStateChange
);

_EXTERN S16BIT _DECL _aceBCGetConditionCode
(
    S16BIT DevNum,
    U16BIT wConditionCode,
    U16BIT* pCurrentState
);

_EXTERN S16BIT _DECL _aceBCGetMsgFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _aceBCGetMsgHdrFromIDRaw
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    U16BIT *pBuffer,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _aceBCGetMsgFromIDDecoded
(
    S16BIT DevNum,
    S16BIT nMsgBlkID,
    MSGSTRUCT *pMsg,
    U16BIT bPurge
);

_EXTERN S16BIT _DECL _aceBCDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL _aceBCInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL _aceBCUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCGPQRead
(
    S16BIT DevNum,
    GPQENTRY* pGPQEntry
);

_EXTERN S16BIT _DECL _aceBCGPQGetCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCFrmToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCFrmToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _aceBCGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _aceBCSetMsgRetry
(
    S16BIT DevNum,
    U16BIT wNumOfRetries,
    U16BIT wFirstRetryBus,
    U16BIT wSecondRetryBus,
    U16BIT wReserved
);

_EXTERN S16BIT _DECL _aceBCSetWatchDogTimer
(
    S16BIT DevNum,
    U16BIT bEnable,
    U16BIT wTimeOut
);

_EXTERN S16BIT _DECL _aceBCGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _aceBCGetGPQMetric
(
    S16BIT DevNum,
    GPQMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateBCtoRT
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateRTtoBC
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateRTtoRT
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateMode
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateBcst
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateBcstRTtoRT
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

_EXTERN S16BIT _DECL _aceBCAsyncMsgCreateBcstMode
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

_EXTERN S16BIT _DECL _aceBCSendAsyncMsgLP
(
    S16BIT DevNum,
    U16BIT *pMsgLeft,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL _aceBCSendAsyncMsgHP
(
    S16BIT DevNum,
    U16BIT nMsgID,
    U16BIT wTimeFactor
);

_EXTERN S16BIT _DECL _aceBCEmptyAsyncList
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCResetAsyncPtr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCGetAsyncCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceBCDisableMessage
(
    S16BIT DevNum,
    U16BIT wMsgNum,
    BOOLEAN bDisable
);

_EXTERN S16BIT _DECL _aceBCGetStatus
(
    S16BIT DevNum,
    U16BIT *wStatus
);

_EXTERN S16BIT _DECL _aceBCExtTriggerEnable
(
    S16BIT DevNum,
    BOOLEAN bOnStart
);

_EXTERN S16BIT _DECL _aceBCExtTriggerDisable
(
    S16BIT DevNum
);

#endif /* ifndef __BCOP_H__ */
