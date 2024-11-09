/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Software Library
 *
 *          Copyright (c) 2006 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  Config.h (RTL device configuration/initialization module)
 *
 *
 *
 *  Created 03/21/00 DL
 *
 *  Function List:
 *      aceGetLibVersion
 *      aceGetSwVersionInfo
 *      aceGetHwVersionInfo
 *      aceFree
 *      aceInitialize
 *      aceISQRead
 *      aceMemRead
 *      aceMemWrite
 *      aceRegRead
 *      aceRegWrite
 *      aceSetClockFreq
 *      aceSetDecoderConfig
 *      aceSetIrqConditions
 *      aceSetIrqConfig
 *      aceSetTimeTagRes
 *      aceSetTimeTagValue
 *      aceGetTimeTagValue
 *      aceResetTimeTag
 *      aceSetRamParityChecking
 *      aceSetRespTimeOut
 *      aceSetWin32IrqConditions
 */

#ifndef __CONFIGOP_H__
#define __CONFIGOP_H__

/* wAccess can be one of the following */
#define ACE_ACCESS_MAXNUM   3       /* 3 access types supported */
#define ACE_ACCESS_CARD     0       /* ACE card running on W32 OS */
#define ACE_ACCESS_SIM      1       /* simulated memory */
#define ACE_ACCESS_USR      2       /* user defined memory location */

/* wMode parameter can be one of the following */
#define ACE_MODE_MASK       0x0FFF
#define ACE_MODE_MAXNUM     0x0008  /* 7 modes supported */
#define ACE_MODE_TEST       0x0000  /* self test mode */
#define ACE_MODE_BC         0x0001  /* 1553 bus controller mode */
#define ACE_MODE_RT         0x0002  /* 1553 remote terminal mode */
#define ACE_MODE_MT         0x0003  /* 1553 monitor terminal mode */
#define ACE_MODE_RTMT       0x0004  /* 1553 RT with monitoring */
#define ACE_MODE_MTI        0x0006  /* Flexcore MT Improved mode */
#define ACE_MODE_RTMTI      0x0007  /* Flexcore RTMT Improved mode */

/* ACEX Modes - Not all of these are needed, come back to this */
#define ACE_MODE_MRT        0x0008  /* MRT mode */
#define ACE_MODE_MRTMTI     0x000A  /* MRT/MTI Combination mode */
#define ACE_MODE_BCMTI      0x000C  /* BC/MTI Combination mode */

#define ACE_MODE_BCMRT      0x0010  /* BC/MRT Combination mode */
#define ACE_MODE_BCMRTMTI   0x0011  /* BC/MRT/MTI Combination mode */
#define ACE_MODE_ALL        0x0011  /* shortcut of BC/MRT/MTI Combination mode */

/* the following values can be or'd in to the selected mode */
#define ACE_OPTION_MASK     0xF000
#define ACE_NO_TT_RESET     0x1000  /* or'd in value to not reset TT */
#define ACE_ADVANCED_MODE   0x2000  /* or'd in value to run in adv mode */

/* Software states that the hardware can be in */
#define ACE_STATE_RESET     0       /* Not initialized */
#define ACE_STATE_READY     1       /* Initialized in mode of operation*/
#define ACE_STATE_RUN       2       /* ACE mode is running */

/* The different ways to address hardware. A0-A0-> Incrementing the
 * host address by 1 will access next word on the Mini-ACE. A1-A0->
 * Incrementing the host address by 2 will access the next word on
 * the Mini-ACE. A2-A0-> Incrementing the host address by 4 will
 * access the next word on the Mini-ACE.
 */
#define ACE_ADDRMODE_A0_A0  0
#define ACE_ADDRMODE_A1_A0  1
#define ACE_ADDRMODE_A2_A0  2

/* wTTRes parameter can be one of the following */
#define ACE_TT_2US          0       /* 2us Time Tag resolution */
#define ACE_TT_4US          1       /* 4us Time Tag resolution */
#define ACE_TT_8US          2       /* 8us Time Tag resolution */
#define ACE_TT_16US         3       /* 16us Time Tag resolution*/
#define ACE_TT_32US         4       /* 32us Time Tag resolution*/
#define ACE_TT_64US         5       /* 64us Time Tag resolution*/
#define ACE_TT_TEST         6       /* Test Time Tag clock */
#define ACE_TT_EXT          7       /* Use external TT clock */
#define ACE_TT_1US          8       /* 1us Time Tag resolution */
#define ACE_TT_500NS        9       /* 500ns Time Tag resolution */
#define ACE_TT_100NS        10      /* 100ns Time Tag resolution */
#define ACE_TT_TEST_CLK     11      /* Test Time Tag clock */

/* wRespTimeOut parameter can be one of the following */
#define ACE_RESPTIME_18US   0       /* 18.5us before reponse timout*/
#define ACE_RESPTIME_22US   1       /* 22.5us before reponse timout*/
#define ACE_RESPTIME_50US   2       /* 50.5us before reponse timout*/
#define ACE_RESPTIME_130US  3       /* 130us before reponse timout*/
#define ACE_RESPTIME_14US   4       /* 14us before reponse timout */

/* wClockIn parameter can be one of the following */
#define ACE_CLOCK_16MHZ     0       /* 16MHz clock input */
#define ACE_CLOCK_12MHZ     1       /* 12MHz clock input */
#define ACE_CLOCK_20MHZ     2       /* 20MHz clock input */
#define ACE_CLOCK_10MHZ     3       /* 10MHz clock input */

/* wLvlOrPulse parameter can be one of the following */
#define ACE_IRQ_PULSE       0       /* Pulse interrupts */
#define ACE_IRQ_LEVEL       1       /* Level mode interrupts */

/* Interrupt status queue header can be one of the following */
#define ACE_ISQ_MSG_NONMSG          0x0001

/* Message based */
#define ACE_ISQ_TX_TIMEOUT          0x8000
#define ACE_ISQ_ILL_LKUP            0x4000
#define ACE_ISQ_MT_DSTK_50P_ROVR    0x2000
#define ACE_ISQ_MT_DSTK_ROVER       0x1000
#define ACE_ISQ_RT_DSTK_50P_ROVR    0x0800
#define ACE_ISQ_RT_DSTK_ROVER       0x0400
#define ACE_ISQ_MT_CSTK_50P_ROVR    0x0200
#define ACE_ISQ_MT_CSTK_ROVR        0x0100
#define ACE_ISQ_RT_CSTK_50P_ROVR    0x0080
#define ACE_ISQ_RT_CSTK_ROVR        0x0040
#define ACE_ISQ_HNDSHK_FAIL         0x0020
#define ACE_ISQ_FMT_ERR             0x0010
#define ACE_ISQ_MCODE               0x0008
#define ACE_ISQ_SA_EOM              0x0004
#define ACE_ISQ_EOM                 0x0002

/* Non-Message Based */
#define ACE_ISQ_TIME_TAG            0x0010
#define ACE_ISQ_RT_ADDR_PAR         0x0008
#define ACE_ISQ_SELF_TEST           0x0004

/* RT_AUTO_BOOT information */
#define RT_AUTO_BOOT_MEM_OFFSET_WORD    0x6000
#define RT_AUTO_BOOT_MEM_SIZE_WORD      0x1000

/* HW designer doesn't want to change the format */
typedef struct ISQENTRY_MRT
{
    U32BIT dwISQHeader;         /* gives information on what data is */
    U32BIT dwISQData;           /* the information for the entry */
    U32BIT dwMrt;               /* Multi-RT number, drop hi 16bit */
    U32BIT dwPad;               /* all 0's */
} ISQENTRY_MRT;

/*-----------------------------------------------------------------------
Struct: HBUFMETRIC

Description:
    This structure defines valid performance information for a Host Buf
------------------------------------------------------------------------*/
typedef struct HBUFMETRIC
{
    U32BIT dwCount;     /* Number of Msgs in the host buffer */
    U32BIT dwLost;      /* Total number of msgs lost since install */
    U32BIT dwPctFull;   /* Current Percentage of HBuf used */
    U32BIT dwHighPct;   /* Highest Percentage of HBuf used */
} HBUFMETRIC;

/*-----------------------------------------------------------------------
Struct: STKMETRIC

Description:
    This structure defines valid performance information for an RT CmdStk
------------------------------------------------------------------------*/
typedef struct STKMETRIC
{
    U32BIT dwLost;      /* Total number of msgs lost since RT start */
    U32BIT dwPctFull;   /* Current Percentage of Cmd Stk used */
    U32BIT dwHighPct;   /* Highest Percentage of Cmd Stk used */
} STKMETRIC;

/* dwIrqMask parameter can be the following OR'd together */
#define ACE_IMR1_EOM                0x00000001
#define ACE_IMR1_BC_STATUS_SET      0x00000002
#define ACE_IMR1_RT_MODE_CODE       0x00000002
#define ACE_IMR1_MT_PATTERN_TRIG    0x00000002
#define ACE_IMR1_FORMAT_ERR         0x00000004
#define ACE_IMR1_BC_END_OF_FRM      0x00000008
#define ACE_IMR1_BC_MSG_EOM         0x00000010
#define ACE_IMR1_RT_SUBADDR_EOM     0x00000010
#define ACE_IMR1_RT_CIRCBUF_ROVER   0x00000020
#define ACE_IMR1_TT_ROVER           0x00000040
#define ACE_IMR1_RT_ADDR_PAR_ERR    0x00000080
#define ACE_IMR1_BC_RETRY           0x00000100
#define ACE_IMR1_HSHAKE_FAIL        0x00000200
#define ACE_IMR1_MT_DATASTK_ROVER   0x00000400
#define ACE_IMR1_MT_CMDSTK_ROVER    0x00000800
#define ACE_IMR1_BCRT_CMDSTK_ROVER  0x00001000
#define ACE_IMR1_BCRT_TX_TIMEOUT    0x00002000
#define ACE_IMR1_RAM_PAR_ERR        0x00004000
#define ACE_IMR2_BIT_TRIGGER        0x00010000
#define ACE_IMR2_BIT_COMPLETE       0x00020000
#define ACE_IMR2_FLEX_DMA_ENABLE    0x00020000
#define ACE_IMR2_BC_UIRQ0           0x00040000
#define ACE_IMR2_BC_UIRQ1           0x00080000
#define ACE_IMR2_BC_UIRQ2           0x00100000
#define ACE_IMR2_BC_UIRQ3           0x00200000
#define ACE_IMR2_MT_DSTK_50P_ROVER  0x00400000
#define ACE_IMR2_MT_CSTK_50P_ROVER  0x00800000
#define ACE_IMR2_RT_CIRC_50P_ROVER  0x01000000
#define ACE_IMR2_RT_CSTK_50P_ROVER  0x02000000
#define ACE_IMR2_BC_TRAP            0x04000000
#define ACE_IMR2_BC_CALLSTK_ERR     0x08000000
#define ACE_IMR2_GPQ_ISQ_ROVER      0x10000000
#define ACE_IMR2_RT_ILL_CMD         0x20000000
#define ACE_IMR2_BC_OPCODE_PARITY   0x40000000

/* wAutoClear parameter can be one of the following */
#define ACE_IRQ_NO_AUTO_CLR 0       /* No auto clear irqs */
#define ACE_IRQ_AUTO_CLR    1       /* Auto clear irqs */

/* wDoubleOrSingle parameter can be one of the following */
#define ACE_SINGLE_ENDED    0       /* Decode singled ended input */
#define ACE_DOUBLE_ENDED    1       /* Decode double ended input */

/* wExpXingEnable parameter can be one of the following */
#define ACE_DISABLE_EXPANDED_XING 0 /* Sample on single clock edge */
#define ACE_ENABLE_EXPANDED_XING  1 /* Sample on both clock edges */

/* wRamParityEnable parameter can be one of the following */
#define ACE_DISABLE_PARITY_CHECK 0  /* Dont display failures in ISR */
#define ACE_ENABLE_PARITY_CHECK  1  /* Display failures in ISR */

/* EBR port definitions */
#define ACE_EBR_PORT_A          0x00000000
#define ACE_EBR_PORT_B          0x00000001
#define ACE_EBR_PORT_C          0x00000002
#define ACE_EBR_PORT_D          0x00000003
#define ACE_EBR_BROADCAST       0x0000001F

/*---------------------- FUNCTION PROTOYPES -------------------------*/
_EXTERN U16BIT _DECL _aceGetLibVersion
(
    void
);

_EXTERN U16BIT _DECL _aceGetCoreVersion
(
    void
);

_EXTERN S16BIT _DECL _aceGetSwVersionInfo
(
    PSWVERSIONINFO pSwVersionInfo
);

_EXTERN S16BIT _DECL _aceGetHwVersionInfo
(
    S16BIT DevNum,
    PHWVERSIONINFO pHwVersionInfo
);

_EXTERN S16BIT _DECL _aceInitialize
(
    S16BIT DevNum,
    U16BIT wAccess,
    U16BIT wMode,
    U32BIT dwMemWrdSize,
    U32BIT dwRegAddr,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL _aceFree
(
    S16BIT DevNum
);

_EXTERN U16BIT _DECL _aceRegRead
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL _aceRegWrite
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL _aceRegRead32
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL _aceRegWrite32
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U32BIT dwValue
);

_EXTERN U16BIT _DECL _aceMemRead
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL _aceMemWrite
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL _aceMemRead32
(
    S16BIT DevNum,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL _aceMemWrite32
(
    S16BIT DevNum,
    U32BIT dwMemAddr,
    U32BIT dwValue
);

_EXTERN S16BIT _DECL _aceSetTimeTagRes
(
    S16BIT DevNum,
    U16BIT wTTRes
);

_EXTERN S16BIT _DECL _aceSetTimeTagValue
(
    S16BIT DevNum,
    U16BIT wTTValue
);

_EXTERN S16BIT _DECL _aceGetTimeTagValue
(
    S16BIT DevNum,
    U16BIT* wTTValue
);

_EXTERN S16BIT _DECL _aceSetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT ullTTValue
);

_EXTERN S16BIT _DECL _aceGetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT* ullTTValue
);

_EXTERN S16BIT _DECL _aceResetTimeTag
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceSetRespTimeOut
(
    S16BIT DevNum,
    U16BIT wRespTimeOut
);

_EXTERN S16BIT _DECL _aceSetClockFreq
(
    S16BIT DevNum,
    U16BIT wClockIn
);

_EXTERN S16BIT _DECL _aceSetIrqConfig
(
    S16BIT DevNum,
    U16BIT wLvlOrPulse,
    U16BIT wAutoClear
);

_EXTERN S16BIT _DECL _aceSetDecoderConfig
(
    S16BIT DevNum,
    U16BIT wDoubleOrSingle,
    U16BIT wExpXingEnable
);

_EXTERN S16BIT _DECL _aceSetIrqConditions(S16BIT DevNum, U16BIT bEnable, U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    ));
_EXTERN S16BIT _DECL _aceSetRamParityChecking
(
    S16BIT DevNum,
    U16BIT wRamParityEnable
);

_EXTERN S16BIT _DECL _aceISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL _aceISQClear
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceISQEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _aceSetMetrics
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _aceInt80Enable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _aceSetAsyncIsr(S16BIT DevNum, void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    ));
_EXTERN S16BIT _DECL _aceSetAsyncIsr(S16BIT DevNum, void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    ));
_EXTERN S16BIT _DECL _aceGetCardType
(
    S16BIT DevNum,
    U32BIT *dwCardType
);

/* these initialize the discrete and avionic functions independent of 1553. */
_EXTERN S16BIT _DECL _aceIOInitialize
(
    S16BIT DevNum,
    S16BIT wOptions
);

_EXTERN S16BIT _DECL _aceIOFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceGetChannelCount
(
    S16BIT DevNum,
    CHANCOUNT_t *pChanCount
);

/* Digital IRIG Tramsmitter Definitions */

_EXTERN S16BIT _DECL _aceGetIRIGTx
(
    S16BIT DevNum,
    PACE_IRIG_TX pstructIRIG
);

_EXTERN S16BIT _DECL _aceSetIRIGTx
(
    S16BIT DevNum,
    ACE_IRIG_TX structIRIG
);

/* EBR Only */
_EXTERN S16BIT _DECL _aceSetHubAddress
(
    S16BIT DevNum,
    U16BIT wHubNum
);

#endif /* __CONFIGOP_H__ */
