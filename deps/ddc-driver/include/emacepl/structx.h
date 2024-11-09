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

/*****************************************************************************
*                       Data Device Corporation
*                         105 Wilbur Place
*                        Bohemia N.Y. 11716
*
*       AceXtreme 'C' Run Time Library
*
*       Copyright (c) 2008 Data Device Corporation
*       All Rights Reserved.
*
*       Filename: axstruct.h
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
*       This is the AceX header file.
*
*
******************************************************************************/

#ifndef _ACEX_STRUCT_H
#define _ACEX_STRUCT_H

#define SFP_MT_CFG_REG      0x7

#define SFP_MT_CMDSTK_MASK  0x1800
#define SFP_MT_DATASTK_MASK 0x0700

#define SFP_MRT_GLB_DATA_STK_EN_MASK    0x00000010
#define SFP_MRT_GLB_DATA_STK_TYPE_MASK  0x0000000F

/*  ****************** ACEX Definitions ***************************    */

#define ACEX_DEVICE_ID_STRING_SIZE  256

/*  *******************ACEX MODULE Definitions *********************    */
/*  for now, these are used with the memory manager                     */
#define ACEX_NONE   0
#define ACEX_BC     1
#define ACEX_MRT    2
#define ACEX_UMT    3

/*   ************************ ACEX REMOTE TERMINAL Definitions ************  */

#define ACEX_MAX_RTS            32
#define ACEX_BROADCAST_RT       31

/* CmdStkSize paramater values */
#define ACE_RT_CMDSTK_256       0x0000 /* Cmd Stacks are 256 words */
#define ACE_RT_CMDSTK_512       0x2000 /* Cmd Stacks are 512 words */
#define ACE_RT_CMDSTK_1K        0x4000 /* Cmd Stacks are 1K words */
#define ACE_RT_CMDSTK_2K        0x6000 /* Cmd Stacks are 2K words */

#define ACE_MRT_RESP_TO_18_5_uS 37 /* 37 * 0.5 uS resolution */

#define ACE_OS_WIN9X            1  /* Win95/98 operating system */
#define ACE_OS_WINNT            2  /* Windows NT operating system */
#define ACE_OS_WIN2K            3  /* Windows 2000 operating system */
#define ACE_OS_VXWRKS           16 /* VxWorks embedded OS */

#define MAX_DATA_ARRAYS         4  /* max # data arrays per RT */

#define MRT_STKBUF_MAXCMDS      32
#define MRT_STKBUF_WDSIZE (MRT_STKBUF_MAXCMDS * (ACE_MSGSIZE_RT + 2))   /* buffer 32 Messages Max +2 is for length header*/

#define ACE_API_EMACE           0 /* EMACE      */
#define ACE_API_ACEX            1 /* ACEX       */
#define ACE_API_REMOTE          2 /* Remote     */
#define ACE_API_MAX             3

/* ========================================================================== */
/*                ACEX BC STRUCTURE AND ENUMERATION DEFINITIONS               */
/* ========================================================================== */

/* Rate Group Time Slot Information Structure */
typedef struct _ACEX_BC_SLOTINFO
{
    U32BIT u32FreeMsgTime;      /* Total Message Time Used By Slot */
} ACEX_BC_SLOTINFO;

/* BC Initialization Information Structure */
typedef struct _ACEX_BC_INIT_INFO
{
    ACEX_CONFIG_ID sConfigID;
} ACEX_BC_INIT_INFO;

/* BC Configuration Information Structure */
typedef struct _ACEX_BC_CONFIG_INFO
{
    ACEX_CONFIG_ID sConfigID;
} ACEX_BC_CONFIG_INFO;

/* BC Termination Information Structure */
typedef struct _ACEX_BC_FREE_INFO
{
    ACEX_CONFIG_ID sConfigID;
} ACEX_BC_FREE_INFO;

/* Device Information Member Structure (BC Component) */
typedef struct _ACEX_BC_TYPE
{
    U16BIT u16BcState;                    /* Current BC operational state */
    U32BIT u32Options;                    /* BC Operation Options */
    U16BIT u16MjrFrmTime;                 /* The 'MAJOR' frame time (global) */
    U16BIT u16MjrFlags;                   /* The 'MAJOR' frame flags (global) */
    U16BIT u16RespTimeOut;                /* Message Response Timeout 0.5 uS Resolution */
    U16BIT u16WDTimeOut;                  /* Watchdog timer timeout */
    BOOLEAN bWDEnable;                    /* Watchdog timer enable */
    U16BIT u16AsyncCount;                 /* Counter of Asynch Messages */
    U16BIT u16MsgBlkCount;                /* Counter of Msg Blocks */
    U16BIT u16DataBlkCount;               /* Counter of Data Blocks */
    U16BIT u16SlotCount;                  /* Number of available time slots */
    U16BIT u16SlotTime;                   /* Time slot duration in 100us increments */
    U32BIT u32SlotItemDefAddr;            /* Address of the default time slot item */
    U32BIT u32UsedDataStrSA[32];          /* Bit mask of currently used data stream RT Sub Addresses - One mask for each RT Address */
    U32BIT u32DefaultInstrAddr;           /* Adddress of BC default instruction opcode (HALT);  */
    DLISTNODE        *pDataBlks;          /* DLIST of data blocks */
    DLISTNODE        *pMsgBlks;           /* DLIST of msg blocks */
    DLISTNODE        *pOpCodes;           /* DLIST of opcodes */
    DLISTNODE        *pFrames;            /* DLIST of frames */
    DLISTNODE        *pStreams;           /* DLIST of data streams */
    DLISTNODE        *pDataArrays;        /* DLIST of data arrays */
    DLISTNODE        *pMemWords;          /* DLIST of memory dwords */
    DLISTNODE        *pMemObjects;        /* DLIST of memory objects */
    ALISTNODE        *pAsyncList;         /* DLIST of pri async list */
    FLISTNODE        *pFrameList;         /* DLIST of frame list */
    DLISTNODE        *pSlotParam;         /* Reference to time slot parameter memory */
    DLISTNODE        *pSlotMain;          /* Reference to time slot main routine memory */
    DLISTNODE        *pSlotReset;         /* Reference to time slot reset routine memory */
    DLISTNODE        *pSlotPtr;           /* Reference to time slot pointer memory */
    DLISTNODE        *pSlotItems;         /* DLIST of slot items */
    ACEX_BC_SLOTINFO *pSlotInfo;          /* Reference to time slot management information */
    BOOLEAN bStartByExtTrigger;           /* TRUE if to start BC by Ext Trigger */
    BOOLEAN bHBufInstalled;               /* Host buffer install flag */
    BOOLEAN b1553a;                       /* Use 1553a protocol */
    BOOLEAN bMsgIndexUsed[ACEX_BC_MAX_INDEX_MSG];
    BOOLEAN bDataIndexUsed[ACEX_BC_MAX_INDEX_DATA];
    FILE *            pReplayFile;        /* Replay file pointer */
    FILE *            pReplayFileOut;     /* Replay file pointer */
    U8BIT u8ReplayDataType;               /* Replay file data type */
    U16BIT u16ReplayChannelId;            /* Replay file channel id */
    BOOLEAN bReplayChanIdEnable;          /* Replay file channel id enable */
    S32BIT s32ReplayLoopCount;            /* Replay file loop count */
    MTI_CH10_DATA_PKT *pReplayPacket;     /* Replay file packet buffer pointer */
    U16BIT bMsgIdTbl[ACEX_BC_MAX_INDEX_MSG];
    BOOLEAN bActive;                      /* MF Phase III: Active BC */
    BOOLEAN bReplayMode;                  /* TRUE if BC is currently in Replay Mode */
} ACEX_BC_TYPE;

/* ========================================================================== */
/*               ACEX (M)RT STRUCTURE AND ENUMERATION DEFINITIONS             */
/* ========================================================================== */

/* MRT State Machine Enumeration */
typedef enum _ACEX_MRT_STATES
{
    MRT_CLOSED = 0,
    MRT_OPEN
} ACEX_MRT_STATES;

/* RT Data Array State Structure */
typedef enum _ACEX_RT_DATA_ARRAY_STATE
{
    DA_OFF = 0,
    DA_ON,
    DA_CONTINOUS,
    DA_DONE
} ACEX_RT_DATA_ARRAY_STATE;

/* RT Access Structure */
typedef struct ACEX_RT_ACCESS
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16RtAddr;
    U32BIT u32Data;
}ACEX_RT_ACCESS;

/* RT Data Array Structure */
typedef struct _ACEX_RT_DATA_ARRAY
{
    ACEX_RT_ACCESS sRtAccess;

    U16BIT u16RtAddr;
    U16BIT u16ID;
    U32BIT u32TxPtrOffsetAddressMRT;  /* 32 bit address of tx pointer */
    U32BIT u32TxPtrOffsetAddressIMP;  /* 32 bit address of tx pointer */
    U32BIT u32TxTfrCompleteOffset; /* 16-bit offset address when complete */
    U32BIT u32TxTfrStartOffset;    /* 16-bit start offset */

    ACEX_RT_DATA_ARRAY_STATE eState;
}ACEX_RT_DATA_ARRAY;

/* RT Configuration Structure */
typedef struct _ACEX_RT_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16RtAddr;
    U16BIT u16RespTimeOut;
    U32BIT u32Options;        /* RT specific options */
} ACEX_RT_CONFIG;

/* RT Hardware Information Structure */
typedef struct _ACEX_RT_HW_INFO /* returned from driver */
{
    U32BIT u32RtIllegalizationRxTableBA;  /* addr in hw memory */
    U32BIT u32RtIllegalizationTxTableBA;
    U32BIT u32SACtrlBA;                 /* addr in hw memory */
    U32BIT u32ImpRxLkupTableBA;         /* Imp RxLkup mirror addr in hw memory */
    U32BIT u32ImpTxLkupTableBA;         /* Imp TxLkup mirror addr in hw memory */
    U32BIT u32ModeCodeDataTableBA;      /* Mode Code Data Table addr in hw memory */
    U32BIT u32ConfigurationBA;          /* Configuration field address */
    U32BIT u32RxLkupTableBA;            /* addr in hw memory */
    U32BIT u32TxLkupTableBA;            /* addr in hw memory */
    U32BIT u32BusyBitRxLkupTableBA;     /* addr in hw memory */
    U32BIT u32BusyBitTxLkupTableBA;     /* addr in hw memory */
    U32BIT u32StatusInCtrlTableBA;      /* addr in hw memory */
    U32BIT u32ModeCodeSelTxIntTableBA;  /* addr in hw memory */
    U32BIT u32ModeCodeSelRxIntTableBA;  /* addr in hw memory */
    U32BIT u32DbcHoldoffTimeBA;         /* DBC holdoff time */
    U32BIT u32ImrTrigSelectBA;          /* IMR trigger source selection addr */
    U32BIT u32ImrModeCodeBA;            /* IMR trigger source selection for Mode Codes */
} ACEX_RT_HW_INFO;

/* Data Array Structure */
typedef struct _ACEX_MRT_DATA_ARRAY_TYPE
{
    BOOLEAN bActive;
    BOOLEAN bContinuous;
    U32BIT u32TxBufWdSize;
    U32BIT u32HwBufWdSize;
    U32BIT u32HwOffset16;
    U32BIT u32HwBA16;
    S16BIT s16DBlkID;
    U16BIT u16SA;
} ACEX_MRT_DATA_ARRAY_TYPE;

/* RT Mode Code Structure */
typedef struct _ACEX_RT_MODE_CODE_TYPE
{
    U32BIT u32_TxBitWd_TxLstCmd;
    U32BIT u32TxIrq;
    U32BIT u32RxIrq;
} ACEX_RT_MODE_CODE_TYPE;

/* RT Busy Bit Structure */
typedef struct _ACEX_RT_BB_TABLE_TYPE
{
    U32BIT u32BusyBitTx;
    U32BIT u32BusyBitRx;
}ACEX_RT_BB_TABLE_TYPE;

/* RT Status In Ctrl Structure */
typedef struct _ACEX_RT_STATUS_INCTRL_TABLE_TYPE
{
    U32BIT u32StatusInCtrl;
} ACEX_RT_STATUS_INCTRL_TABLE_TYPE;

/* RT Illegalization Table Structure */
typedef struct _ACEX_RT_ILL_TABLE_TYPE
{
    U32BIT u32IllTx[32];
    U32BIT u32IllRx[32];
}ACEX_RT_ILL_TABLE_TYPE;

/* RT Sub Address Control Structure */
typedef struct ACEX_RT_SACTRL_TABLE_TYPE
{
    U32BIT u32SACtrl[32];
} ACEX_RT_SACTRL_TABLE_TYPE;

/* RT Queue Structure */
typedef struct ACEX_RTX_QUEUE_TYPE
{
    U32BIT u32BA;
    U32BIT u32WdSize;
    U16BIT u16Enabled;
    U32BIT u32Lost;
    U32BIT u32Head;
} ACEX_RTX_QUEUE_TYPE;

/* MRT Command Stack Information Structure */
typedef struct ACEX_MRT_CMD_STK_TYPE
{
    U32BIT dwCmdBase;         /* Start address of CMD stack 16 ->32 */
    U32BIT dwCmdEnd;          /* End address of command stack */
    U16BIT wCmdSize;          /* Size of the command stack in words*/
    U32BIT dwCurrentMsg;      /* Current msg to process on cmd stk */
    U32BIT dwPrevTT;          /* Previous processed Msg's HW TimeTag value */
    U32BIT dwEndMsg;          /* Last msg to be processed */
    U32BIT dwLost;            /* Number of Stack Msgs lost */
    U32BIT dwHighPct;         /* Highest Perecent ever full */
    U32BIT dwPctFull;         /* Current Percent full */
    U32BIT u32NumMsgs;           /* number of messages command stack can hold */
} ACEX_MRT_CMD_STK_TYPE;

/* MRT Data Stream Transfer Information Structure */
typedef struct _ACEX_MRT_STREAM_TRANSFER_TYPE
{
    S16BIT nDataBlkID;   /* DBLK ID */
    U32BIT u32DwdBufPtr; /* address of DBLK in hw */
    U16BIT u16BufWdSize; /* DBLK num wds */

    U32BIT u32UsrBufByteSize; /* size of user buf in bytes */
    U16BIT *pUBuf;            /* pointer to user buf */

    U32BIT u32NumSATfr;   /* running count of number of SA transfers */
    U32BIT U32SACount; /* running count of number SA transfers completed */

    S32BIT s32Timeout; /* timeout associated w/ transaction -1-forever, 0-don't wait, else wait for amount in mS */
} ACEX_MRT_STREAM_TRANSFER_TYPE;

/* MRT Data Stream Information Structure */
typedef struct _ACEX_MRT_STREAM_TYPE
{
    U32BIT u32ID;

    ACEX_MRT_STREAM_TRANSFER_TYPE sDirection[2]; /* MRT_STREAM_DBLK_RX, MRT_STREAM_DBLK_TX */

    U32BIT u32BufWdSegmentSize;    /* number of words in each SA segment */
    U32BIT u32MTUWd;               /* maximum transmition unit in wds - hw memory allocated in each direction */

    S8BIT s8RtAddr;
    U32BIT u32SAMask;
    U16BIT u16Direction;            /*  ACE_RT_MSGTYPE_RX   0x0001 
                                        ACE_RT_MSGTYPE_TX   0x0002 */
    U16BIT u16Chnl;

    /* for driver use only */
    U32BIT u32NumSAGroup;           /* number of SA in this group */
    void* pRequest;                 /* handle to request object, used by driver only */
} ACEX_MRT_STREAM_TYPE;

/* MRT Hardware Information Structure */
typedef struct _ACEX_MRT_HW_INFO    /* Returned from driver */
{
    U32BIT u32ConfigurationBA;  /* Addr in hw reg */
    U32BIT u32GblDStkPtrBA;     /* Dddr in hw reg */
    U16BIT u16RtSource;         /* USB INTERNAL ONLY - this is for SRT compatibility */
    U16BIT bMode;               /* ACEX_MRT_MODE, ACEX_SRT_MODE */
    U32BIT u32MemBA;            /* Memory Base Address - currently used to resolve to offsets when
                                   writing entries into lkup tables TODO: request hw mod so
                                   absolute addresses can be used */
} ACEX_MRT_HW_INFO;

/* Device Information Member Structure (RT Component) */
typedef struct _ACEX_RT_TYPE
{
    U16BIT u16MrtState;                                         /* Current MRT operational state */
    U32BIT dwOptions;                                           /* RT Operation Options            */
    U16BIT wUseAltStatus;                                       /* Use RT in alternate status mode */
    DBLKTYPE sDataBlkInfo[32];                                  /* Data Block Info                 */
    CBUFTYPE asCircBufInfo[32];                                 /* Circular Buffer Info            */
    U16BIT b1553a;                                              /* Use 1553a protocol              */
    ACEX_RT_CONFIG sRtxCfg;
    ACEX_RT_HW_INFO sRtxHwInfo;
    ACEX_RT_MODE_CODE_TYPE sModeCode;
    ACEX_RT_BB_TABLE_TYPE sBbTable;
    ACEX_RT_STATUS_INCTRL_TABLE_TYPE sStatusInCtrlTable;
    ACEX_RT_ILL_TABLE_TYPE sIllTable;
    ACEX_RT_SACTRL_TABLE_TYPE sSACtrl;
    U32BIT u32NumStreams;                                       /* number of active streams */
    ACEX_MRT_STREAM_TYPE sStream[4];                            /* 4 streams allowed per RT */
    U32BIT u32NumDataArrays;                                    /* number of active data arrays */
    ACEX_MRT_DATA_ARRAY_TYPE sDataArray[4];                     /* 4 data array typesa allowed per RT */

    BOOLEAN bActive;                                            /* MF Phase III: active RT */
} ACEX_RT_TYPE;

/* MRT Configuration Information Structure */
typedef struct _ACEX_MRT_CONFIG_TYPE
{
    ACEX_CONFIG_ID sConfigID;
    U16BIT u16CmdStkType;           /* Value to identify stack size */
    U32BIT u32CmdBA;                /* Base address of cmd stack */
    U8BIT u8MrtEnable;
    U16BIT u16SomEom;               /* 0=SOM, 1=EOM */
    U16BIT u16GblDataStkType;
    U32BIT u32GblDataStkBA;
    U32BIT u32IntConditions;
    U32BIT u32IsqBA;
} ACEX_MRT_CONFIG_TYPE;

/* MRT Statistic Information Structure */
typedef struct _ACEX_MRT_STAT_TYPE
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
} ACEX_MRT_STAT_TYPE;

/* Device Information Member Structure (MRT Component) */
typedef struct _ACEX_MRT_TYPE
{
    ACEX_MRT_STATES state;
    S16BIT nGlobalCircID;                           /* ID of global circ buffer */
    S16BIT nGlobalDataStackID;                      /* ID of global data stack */
    ACEX_MRT_CMD_STK_TYPE sStk;                     /* RT Stack information - cmd stack*/
    ACEX_RTX_QUEUE_TYPE sIsq;
    BOOLEAN bBrdcstDisabled;
    HOSTBUFFER sHBuf;                               /* RT Operation Host Buffer */
    ACEX_MRT_CONFIG_TYPE sRtxGCfg;
    ACEX_RT_TYPE asRtx[ACEX_MAX_RTS];               /* RT-Specific info */
    U16BIT u16RespTimeOut;                          /* 0.5 uS resolution */
    DLISTNODE               *pDataBlks;             /* Global Data Stack */
    U16BIT u16Mode;                                 /* derived from ACEX_DEVICE_INFO MODE - either
                                                        ACE_SFP_MODE or ACE_RT_MODE */
    S8BIT s8SrtAddr;                                /* Rt Address support for compatibility mode */
    U16BIT u16StkBuf[MRT_STKBUF_WDSIZE];
    ACEX_MRT_HW_INFO sHwInfo;

    BOOLEAN bStreamEnabled;
    U32BIT u32NumStreams;                           /* number of active streams */

    BOOLEAN bDataArrayEnabled;
    U32BIT u32NumDataArrays;                        /* number of active streams */
} ACEX_MRT_TYPE;

/* ========================================================================== */
/*                 ACEX MTI STRUCTURE AND ENUMERATION DEFINITIONS             */
/* ========================================================================== */

/* MTI Configuration */
typedef struct _ACEX_MTI_CONFIG
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
} ACEX_MTI_CONFIG;

/* MTI Message Information */
typedef struct ACEX_MTI_INFO
{
    U32BIT u32NumOfMsgs;
    U32BIT u32TotalLength;
    U32BIT u32FirstMsgAddress;
} ACEX_MTI_INFO;

/* MTI Hardware Information */
typedef struct ACEX_MTI_HW_INFO
{
    U32BIT u32LookupTable32BitBA;
    U32BIT u32LookupTable32BitSize;
    U32BIT u32MtiMem32BitEndAddr;
    U32BIT u32MtiMem32BitStartAddr;
    U32BIT u32MtiMem32BitStartAddrOffset;
} ACEX_MTI_HW_INFO;

/* Device Information Member Structure (MTI Component) */
typedef struct _ACEX_MTI_TYPE
{
    U16BIT u16MtiState;                          /* Current MTI operational state */

    ACEX_MTI_HW_INFO sMtiHwInfo;  /* Selective Lookup Table info */
    U32BIT u32MemLength;
    BOOLEAN b1553a;
    BOOLEAN bBcstDisable;
    BOOLEAN bMtiErrorMonitorEnable;
    BOOLEAN bMtiReplayMonitorEnable;
    BOOLEAN bMtiCustomDataTypeEnable;
    BOOLEAN bMtiTriggerStartEnable;
    BOOLEAN bMtiTriggerStopEnable;
    ACEX_MTI_CONFIG sMtiConfig;
} ACEX_MTI_TYPE;

/* ========================================================================== */
/*             ACEX CONFIGURE STRUCTURE AND ENUMERATION DEFINITIONS           */
/* ========================================================================== */
/* Device Information Member Structure (1553 Component) */
typedef struct ACEX_1553_COMPONENT
{
    ACEX_BC_TYPE sBcx;
    ACEX_MRT_TYPE sMrtx;            /* multi-rt */
    ACEX_MTI_TYPE sMtix;
    BOOLEAN b1553a;
    U32BIT             *pMemWrdSize;
    U32BIT             *pMemBaseAddr;
    U32BIT dwSysImr;                   /* Internal IMR mask */
    U32BIT dwUsrImr;                   /* External IMR mask */
    MEMBLK_LIST sMemBlkList;
}ACEX_1553_COMPONENT;

/* General Device Information Structure */
typedef struct _ACEX_DEVICE_INFO
{
    DDC_HANDLE hDevice;
    U32BIT u32CardType;
    U16BIT u16State;
    U16BIT u16MetricEna;
    U16BIT u16Access;
    U16BIT u16Card;
    U16BIT u16Channel;
    U16BIT u16Mode;
    U32BIT u32SubRTMask;
    U32BIT u32Slot;
    U32BIT u32Socket;
    U32BIT u32MemLength;
    U32BIT u32MemBA;
    U32BIT u32DeviceCapability;
    U16BIT u16ModeOptions;
    U16BIT u32TTResolution;
    U16BIT u16ISQEnabled;
    U16BIT u16SWMPrimeCheck;
    U32BIT u32MiscInfo;

    U32BIT u32RtOptions;            /* used to hold compatibility options for SRT Mode  */
    U32BIT u32RtCmdStkSize;         /* used to hold compatibility CmdStkSize for SRT Mode  */

    U16BIT bLegacy;

    BOOLEAN bBcDisable;
    BOOLEAN bBcExtTrigger;
    BOOLEAN bMrtDisable;
    BOOLEAN bRtAutoBoot;
    BOOLEAN bTxInhibit;

    U16BIT u16RtAddr;

    /* interrupt handlers */
    U16BIT (_DECL *funcInternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    );
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    );

    void (_DECL * aioExternalIsr)
    (
        S16BIT s16DevNum,
        U32BIT u32IrqStatus
    );

    ACEX_1553_COMPONENT s1553;

    /* MT/MTi usage */
    MTOPERATION         *pMT;         /* Monitor Terminal pointer */
    U16BIT b1553a;                       /* Move from BC, RT, MT Op. Use 1553a protocol */
    U8BIT u8TimeTagRes;                  /* Used to keep track of TT resolution for MT mode TT rollover */

    /* Board Capabilities - SF+, MF, Replay, etc.  */
    U32BIT u32BoardCapabilities;
    U32BIT u32DblkDouble;
    U32BIT u32AcexRemove_BA;
    
    /* VxWorks ddccm information */
    U16BIT		u16DeviceType;
    char        sDeviceName[DDC_MAX_DEVICE_NAME_LEN];
    
} ACEX_DEVICE_INFO;

/* ******************** PUBLIC FUNCTION PROTOTYPES ********************************* */

_EXTERN VOID _DECL aceXPrintDBlkInfo
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN void* _DECL acexDebugGetDevice
(
    U16BIT DevNum
);

#endif /* ifndef _ACEX_STRUCT_H */
