/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 1999 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  MTop.h (Monitor Terminal operations header file)
 *
 *
 *
 *  Created 8/26/99 DL
 *
 */

#ifndef __MTOP_H__
#define __MTOP_H__

/* monitor stack area constants */
#define ACE_MT_STKA                 0x0000
#define ACE_MT_STKB                 0x2000
#define ACE_MT_STK_CMB              0x4000

/* MT block status word bits */
#define ACE_MT_BSW_ERROR            0x163F
#define ACE_MT_BSW_CMDERR           0x0001
#define ACE_MT_BSW_RTRTERR_2CMD     0x0002
#define ACE_MT_BSW_RTRTERR_GSA      0x0004
#define ACE_MT_BSW_INVWRD           0x0008
#define ACE_MT_BSW_BADSYNC          0x0010
#define ACE_MT_BSW_WCERR            0x0020
#define ACE_MT_BSW_DSTK_ROVER       0x0080
#define ACE_MT_BSW_GDATA            0x0100
#define ACE_MT_BSW_NORES            0x0200
#define ACE_MT_BSW_FMTERR           0x0400
#define ACE_MT_BSW_RTRT             0x0800
#define ACE_MT_BSW_ERRFLG           0x1000
#define ACE_MT_BSW_CHNL             0x2000
#define ACE_MT_BSW_SOM              0x4000
#define ACE_MT_BSW_EOM              0x8000
#define ACE_MT_BSW_SUPER            ACE_MT_BSW_ERRFLG

/* wOptions parameter can be any of the following bit "OR's" together */
#define ACE_MTI_OPT_OVR_MBIT        0x00000010  /* T/R*=0,MC MSB=0, ACE will resp   */  
#define ACE_MT_OPT_1553A_MC         0x00000200 /* 1553a mode codes enabled */
#define ACE_MT_OPT_BCST_DIS         0x00000800 /* Broadcast disabled */
#define ACE_MTI_OPT_ERR_MON_ENA     0x00001000 /* MTi Error Monitor mode enable */
#define ACE_MTI_OPT_MEC_MON_ENA     0x00001000 /* MTi Message Error Capture enable (Formerly MTi Error Monitor) */
#define ACE_MTI_OPT_RTBUSY_DISABLE  0x00002000 /* Busy/Illegal bit and data data valid format disable */
#define ACE_MTI_OPT_EOM_TT_ENABLE   0x00004000 /* Enables EOM for TT */
#define ACE_MTI_OPT_AES_MON_ENA     0x00008000 /* MTi Advanced Error Sampling enable */
#define ACE_MTI_OPT_DDC_DATA_TYPE   0x00010000 /* Use ddc defined custom data types for DDC MTI, MEC, and AES packets */
#define ACE_MTI_OPT_DISABLE_BUS_B   0x00020000 /* Disables Bus B monitoring */
#define ACE_MTI_OPT_DISABLE_BUS_A   0x00040000 /* Disables Bus A monitoring */
#define ACE_MTI_OPT_TRIG_START_ENA  0x00080000 /* Enables mti/mtr start triggering */
#define ACE_MTI_OPT_TRIG_STOP_ENA   0x00100000 /* Enables mti/mtr stop triggering */
#define ACE_MTI_OPT_NO_RESTART      0x80000000 /* Change Options without restarting */

/* wMsgLoc parameter can be any one of the following */
#define ACE_MT_MSGLOC_NEXT_PURGE    0   /* next unread msg, clr stk */
#define ACE_MT_MSGLOC_NEXT_NPURGE   1   /* next unread msg, leave stk */
#define ACE_MT_MSGLOC_LATEST_PURGE  2   /* latest msg, clear stk */
#define ACE_MT_MSGLOC_LATEST_NPURGE 3   /* latest msg, leave stk alone */

/* wStkLoc parameter can be any one of the following */
#define ACE_MT_STKLOC_ACTIVE        0   /* Use the active stk */
#define ACE_MT_STKLOC_INACTIVE      1   /* Use the inactive stk */
#define ACE_MT_STKLOC_STKA          2   /* Use stk A */
#define ACE_MT_STKLOC_STKB          3   /* Use stk B */

/* wMTBufferType parameter can be any one of the following */
#define ACE_MT_SINGLESTK    0       /* Use a single stack */
#define ACE_MT_DOUBLESTK    1       /* Use stack A and B */

/* wCmdStkSize parameter can be any one of the following */
#define ACE_MT_CMDSTK_256   0x0000  /* Cmd Stacks are 256 words */
#define ACE_MT_CMDSTK_1K    0x0800  /* Cmd Stacks are 1K words */
#define ACE_MT_CMDSTK_4K    0x1000  /* Cmd Stacks are 4K words */
#define ACE_MT_CMDSTK_16K   0x1800  /* Cmd Stacks are 16K words */

/* wDataStkSize parameter can be any one of the following */
#define ACE_MT_DATASTK_512  0x0700  /* data Stacks are 256 words */
#define ACE_MT_DATASTK_1K   0x0600  /* data Stacks are 1K words */
#define ACE_MT_DATASTK_2K   0x0500  /* data Stacks are 4K words */
#define ACE_MT_DATASTK_4K   0x0400  /* data Stacks are 16K words */
#define ACE_MT_DATASTK_8K   0x0300  /* data Stacks are 16K words */
#define ACE_MT_DATASTK_16K  0x0200  /* data Stacks are 16K words */
#define ACE_MT_DATASTK_32K  0x0100  /* data Stacks are 16K words */

/* Filter parameters can use the following to enable/disable all */
#define ACE_MT_FILTER_ALL   0xFFFF  /* Enable/disable all */
#define ACE_MT_FILTER_RX    0x0000  /* Enable/disable Receive Cmds */
#define ACE_MT_FILTER_TX    0x0001  /* Enable/disable Trasmit Cmds */

/* wSAMask can be the any of the following "OR'd" together */
#define ACE_MT_FILTER_SA_ALL 0xFFFFFFFF
#define ACE_MT_FILTER_SA0   0x00000001
#define ACE_MT_FILTER_SA1   0x00000002
#define ACE_MT_FILTER_SA2   0x00000004
#define ACE_MT_FILTER_SA3   0x00000008
#define ACE_MT_FILTER_SA4   0x00000010
#define ACE_MT_FILTER_SA5   0x00000020
#define ACE_MT_FILTER_SA6   0x00000040
#define ACE_MT_FILTER_SA7   0x00000080
#define ACE_MT_FILTER_SA8   0x00000100
#define ACE_MT_FILTER_SA9   0x00000200
#define ACE_MT_FILTER_SA10  0x00000400
#define ACE_MT_FILTER_SA11  0x00000800
#define ACE_MT_FILTER_SA12  0x00001000
#define ACE_MT_FILTER_SA13  0x00002000
#define ACE_MT_FILTER_SA14  0x00004000
#define ACE_MT_FILTER_SA15  0x00008000
#define ACE_MT_FILTER_SA16  0x00010000
#define ACE_MT_FILTER_SA17  0x00020000
#define ACE_MT_FILTER_SA18  0x00040000
#define ACE_MT_FILTER_SA19  0x00080000
#define ACE_MT_FILTER_SA20  0x00100000
#define ACE_MT_FILTER_SA21  0x00200000
#define ACE_MT_FILTER_SA22  0x00400000
#define ACE_MT_FILTER_SA23  0x00800000
#define ACE_MT_FILTER_SA24  0x01000000
#define ACE_MT_FILTER_SA25  0x02000000
#define ACE_MT_FILTER_SA26  0x04000000
#define ACE_MT_FILTER_SA27  0x08000000
#define ACE_MT_FILTER_SA28  0x10000000
#define ACE_MT_FILTER_SA29  0x20000000
#define ACE_MT_FILTER_SA30  0x40000000
#define ACE_MT_FILTER_SA31  0x80000000

/* wHBufPercent -> Post Trigger percent of buffer held */
#define ACE_MT_TRIG_HBUF_0P     0   /* No msgs will be held prior to trg*/
#define ACE_MT_TRIG_HBUF_25P    1   /* 25% will be held */
#define ACE_MT_TRIG_HBUF_50P    2   /* 50% will be held */
#define ACE_MT_TRIG_HBUF_75P    3   /* 75% will be held */

/* MT Trigger flags */
#define ACE_MT_TRIG_FLG_NONE    0x0000  /* all trigs disabled */
#define ACE_MT_TRIG_FLG_CMD1    0x0001  /* command 1 trig enabled */
#define ACE_MT_TRIG_FLG_CMD2    0x0002  /* command 2 trig enabled */
#define ACE_MT_TRIG_FLG_STS1    0x0004  /* status 1 trig enabled */
#define ACE_MT_TRIG_FLG_STS2    0x0008  /* status 2 trig enabled */
#define ACE_MT_TRIG_FLG_ERR     0x0010  /* error trig enabled */
#define ACE_MT_TRIG_FLG_DATA    0x0020  /* data trig enabled */
#define ACE_MT_TRIG_FLG_ALL     0x8000  /* all trigs must be in msg */
#define ACE_MT_TRIG_FLG_ANY     0x0000  /* any trigs must be in msg */

/* Monitor detection of partial data logging for RT to RT command, where RT is the transmitter */
#define ACE_MTRT_RTRT_SUP_DET   0x00001000

/**************** MT STRUCTURES ************************/

/*-----------------------------------------------------------------------
Struct: MTINFO

Description:
    This structure passes info. about Monitor Terminal to user.
------------------------------------------------------------------------*/
typedef struct MTINFO
{
    U16BIT wStkMode;
    U16BIT wCmdStkSize;
    U16BIT wDataStkSize;
    U16BIT b1553aMCodes;
    U32BIT dwHBufSize;
}MTINFO;

/*-----------------------------------------------------------------------
Struct: MTI_CH10_FILE_HANDLE

Description:
    This structure contains information about a CH10 File.
------------------------------------------------------------------------*/
typedef struct _MTI_CH10_FILE_HANDLE
{
    FILE*   pCh10File;
    S64BIT s64Ch10FileSize;
    U32BIT u32PktLength;
    U8BIT u8FileAccessMode;
} MTI_CH10_FILE_HANDLE, *PMTI_CH10_FILE_HANDLE;

/*-----------------------------------------------------------------------
Struct: MTTRIGGER

Description:
    This structure completely describes Monitor Terminal capture trigger.
------------------------------------------------------------------------*/
typedef struct MTTRIGGER
{
    U16BIT wCmdWrd1;    /* CmdWrd1 value */
    U16BIT wCmdMsk1;    /* CmdWrd1 mask */
    U16BIT wCmdWrd2;    /* CmdWrd2 value */
    U16BIT wCmdMsk2;    /* CmdWrd2 mask */
    U16BIT wStsWrd1;    /* StsWrd1 value */
    U16BIT wStsMsk1;    /* StsWrd1 mask */
    U16BIT wStsWrd2;    /* StsWrd2 value */
    U16BIT wStsMsk2;    /* StsWrd2 mask */
    U16BIT wDataWrd;    /* DataWrd value */
    U16BIT wDataMsk;    /* DataWrd mask */
    U16BIT wDataPos;    /* DataWrd position in message */
    U16BIT wErrWrd;     /* ErrWrd value (errors in Block Status) */
    U16BIT wErrFlg;     /* should all errors be met or only one */

    U16BIT wCount;      /* # of trig's needed to produce real trigger */
    U16BIT wTrigFlags;  /* should all conditions be met or only one */

    U16BIT wNextFlags;  /* Next flags for complex triggering */
    struct MTTRIGGER *pNextTrg; /* indicates next condition to trigger on*/
} MTTRIGGER;

/**************** MT FUNCTIONS **************************/

_EXTERN S16BIT _DECL _aceMTGetInfo
(
    S16BIT DevNum,
    MTINFO *pInfo
);

/* This function allows the MT to be reconfigured to the users liking */
_EXTERN S16BIT _DECL _aceMTConfigure
(
    S16BIT DevNum,
    U16BIT wMTStkType,
    U16BIT wCmdStkSize,
    U16BIT wDataStkSize,
    U32BIT dwOptions
);

/* These functions are used to enable/disable MT host buffers */
_EXTERN S16BIT _DECL _aceMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL _aceMTUninstallHBuf
(
    S16BIT DevNum
);

/* These functions are used to enable/disable MT sel. msg monitoring */
_EXTERN S16BIT _DECL _aceMTEnableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _aceMTDisableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _aceMTGetRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT *pSAMask
);

/* MT Run functions */
_EXTERN S16BIT _DECL _aceMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTContinue
(
    S16BIT DevNum
);

/* Onboard Stk functions */
_EXTERN S16BIT _DECL _aceMTGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wStkLoc
);

_EXTERN S16BIT _DECL _aceMTGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc,
    U16BIT wStkLoc
);

/* Host buffer functions */
_EXTERN S16BIT _DECL _aceMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _aceMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _aceMTDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL _aceMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTClearHBufTrigger
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTSetHBufTrigger
(
    S16BIT DevNum,
    U16BIT wHBufPercent,
    MTTRIGGER *pTrg
);

_EXTERN S16BIT _DECL _aceMTISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL _aceMTCreateImageFiles
(
    S16BIT DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL _aceMTSwapStks
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _aceMTGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT wStk,
    U16BIT bReset
);

#endif /* ifndef __MTOP_H__ */
