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
 *  Structs.h (RTL device configuration/initialization module)
 *
 *
 *
 *  Created 03/21/00 DL
 *
 */

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#ifdef LINUX
#include <pthread.h>
#endif /* ifdef LINUX */

#ifdef VX_WORKS
#include <semLib.h>
#endif /* ifdef VX_WORKS */

#ifdef INTEGRITY
#include <integrity.h>
#endif /* ifdef INTEGRITY */

/********************* LINK LIST STRUCTURES ****************************/
/*  for now, these are used with the memory manager                     */
#define ACEX_NONE   0
#define ACEX_BC     1
#define ACEX_MRT    2
#define ACEX_UMT    3

/* Memory Block Item Structure Prototypes */

/*-----------------------------------------------------------------------
Struct: MEMORYBLOCK

Description:
    This structure contains all information associated with a block of
    memory used by the hardware.
------------------------------------------------------------------------*/
typedef struct MEMORYBLOCK
{
    U32BIT dwAceAddr;           /* Flexcore address location of memblk */
    U32BIT dwSize;              /* Size of memory block (in words) */
    S16BIT nMemType;            /* Free,Perm.,Used */
    U32BIT wBndry;              /* Mem block boundary condition */
    U8BIT u8Module;             /* 0=NONE, 1=BC, 2=RT, 3=MT - assigned by the user
                                   after block descriptor is returned */
} MEMORYBLOCK;

typedef struct HOSTBUFFER
{
    U16BIT *pBase;      /* Pointer to a host buffer for msgs */
    U32BIT dwHead;      /* offset to a next message */
    U32BIT dwTail;      /* offset to next free location */
    U32BIT dwSize;      /* number of words in host buffer */
    U32BIT dwCount;     /* number of msgs in host buffer */
    U32BIT dwLost;      /* HBuf full, possible msg loss */
    U32BIT dwPctFull;   /* Current Percentage of HBuf used */
    U32BIT dwHighPct;   /* Highest Percentage of HBuf used */
} HOSTBUFFER;

/*-----------------------------------------------------------------------
Struct: DATABLOCKITEM

Description:
    This structure contains all information associated with a Data Block
    in memory.
------------------------------------------------------------------------*/
typedef struct DATABLOCKITEM
{
    struct DLISTNODE *pMemNode; /* pointer to memory node */

    U32BIT dwLastRW;            /* the last read or written location */
    U32BIT dwXEQAddr;           /* Address of XEQ opcode*/
    U32BIT dwXEQOpCode;         /* XEQ opcode */
    U32BIT dwXEQMsgAddr;        /* Message address of XEQ opcode */
    U32BIT dwXQFAddr;           /* Address of XQF opcode*/
    U32BIT dwXQFOpCode;         /* XQF opcode */
    U32BIT dwXQFMsgAddr;        /* Message address of XQF opcode */
    U32BIT dwSATx;              /* Each bit represents Tx DBlk mapping */
    U32BIT dwSARx;              /* Each bit represents Rx DBlk mapping */
    U32BIT dwSABcst;            /* Each bit represents Bcst DBlk mapping */
    U32BIT dwWordCount;         /* Number Of 1553 Words In Data Block */
    S8BIT s8RtAddr;             /* Acex RT Address */
    U32BIT u32UseCount;         /* Use counter */
    U16BIT u16Index;            /* Data Block Index Number */
    U32BIT dwLastW;             /* The last written location */
} DATABLOCKITEM;

/*-----------------------------------------------------------------------
Struct: MESSAGEITEM

Description:
    This structure contains all information associated with a Message
    Item in memory.
------------------------------------------------------------------------*/
typedef struct MESSAGEITEM
{
    struct DLISTNODE *pMemNode; /* pointer to memory node */

    S16BIT wFlags;              /* flags description (RT-RT,double buf) */
    S16BIT nDataBlkID;          /* Data Block ID used by message */
    S16BIT nDataBlkID2;         /* Data Block ID 2 used by message */
    U16BIT wMsgTime;            /* number of us needed for avg message */
    U32BIT dwOptions;           /* Msg Options */
    U32BIT u32UseCount;         /* Use counter */
    U16BIT u16Index;            /* Msg Block Index Number */
    U16BIT wBCCtrlWrd1;         /* the 1st control word */
    U16BIT wBCCtrlWrd2;         /* the 2nd control word */
    U16BIT wWordCount;          /* msg word count */
    U32BIT dwXEQAddr;           /* Address of XQF opcode */
    U32BIT dwXEQMsgAddr;        /* Message address of XQF opcode */
    U32BIT dwXQFAddr;           /* Address of XQF opcode */
    U32BIT dwXQFOpCode;         /* XQF opcode - obsolete due to opcode modification requirement */
    U32BIT dwXQFMsgAddr;        /* Message address of XQF opcode */
    BOOLEAN bAsyncFlag;         /* TRUE - async msg */
} MESSAGEITEM;

/*-----------------------------------------------------------------------
Struct: FRAMEITEM

Description:
    This structure contains all information associated with a Frame Item
    in memory.
------------------------------------------------------------------------*/
typedef struct FRAMEITEM
{
    struct  DLISTNODE *pMemNode; /* pointer to memory node */

    U16BIT wFrameType;          /* Type of frame, minor ...major */
    BOOLEAN bFrmTime1us;        /* 1 us frame time if TRUE; 100 us frame time if FALSE  */
    U32BIT dwFrmTime;           /* Frame Time value */
    U16BIT wOpCodeCount;        /* Number of OpCodes in array */
    S16BIT  *paOpCodes;         /* Pointer to array of OpCode IDs */
    U16BIT wFlags;              /* Flag Options for the frame */
    U16BIT wTotalTime;          /* Cummulative time of msgs in Frame */
    U32BIT dwAbsFrmAddr;        /* Absolute address of empty CAL */
} FRAMEITEM;

/*-----------------------------------------------------------------------
Struct: OPCODEITEM

Description:
    This structure contains all information associated with a OpCode,
    software or hardware.
------------------------------------------------------------------------*/
typedef struct OPCODEITEM
{
    U16BIT wOpCodeType;         /* OpCode Type (XEQ,...) */
    U16BIT wCondition;          /* OpCode Condition */
    U32BIT dwParameter1;        /* OpCode Parameter1 */
    U32BIT dwParameter2;        /* OpCode Parameter2 */
    U16BIT nOpCodeCount;        /* Number of OpCodes in array */
    S16BIT *paOpCodes;          /* Pointer to array of OpCode IDs */
    U32BIT dwReserved;          /* Reserved */
    U32BIT dwAceAddr;           /* OpCode Location in ACE Mem */
    U16BIT wAsyncAddr;          /* Address of NOP following this opcode */
    U32BIT u32UseCount;         /* Use counter */
    struct DLISTNODE *pMemNode; /* Pointer to memory node */
} OPCODEITEM;

/*-----------------------------------------------------------------------
Struct: SLOTITEM

Description:
    This structure contains all information associated with a BC Time Slot Item,
    software or hardware.
------------------------------------------------------------------------*/
typedef struct SLOTITEM
{
    U32BIT u32OpCode;       /* OpCode (XEQ, XET, XQF, or XQT) */
    U32BIT u32MsgAddr;      /* Message Address */
    U16BIT u16MsgID;        /* Message ID */
    U32BIT u32SlotStart;    /* Starting Slot Location */
    U32BIT u32SlotInterval; /* Repeat Interval (Slots) */
    U32BIT u32UseCount;     /* Use counter */
    struct DLISTNODE *pMemNode; /* Pointer to memory node */
} SLOTITEM;

/*-----------------------------------------------------------------------
Struct: STREAMITEM

Description:
    This structure contains all information associated with a data stream.
------------------------------------------------------------------------*/
#define STREAMITEM_CTRLBLK_SIZE    480  /* 30 RT-RT Message Control Block Array (30 Blocks * 16 Dwords = 480 Dwords) */

typedef struct STREAMITEM
{
    U16BIT u16StreamType;          /* Type of stream - 0 = BCRT, 1 = RTRT */
    U16BIT u16SubAddrCount;        /* RT Address 1 Sub Addresses */
    U16BIT u16RTAddr1;             /* RT Address 1 */
    U16BIT u16RTAddr2;             /* RT Address 2 */
    U16BIT u16MsgTime;             /* Execution Time Of Message In Microseconds */
    U32BIT u32SubAddrMask1;        /* RT Address 1 Sub Address Mask */
    U32BIT u32SubAddrMask2;        /* RT Address 2 Sub Address Mask */
    U32BIT u32MsgBaseAddr;         /* Base Address of Message Memory Block */
    U32BIT u32DataBaseAddr;        /* Base Address of Data Memory Block */
    U32BIT    *u32CtrlBlockArray;  /* 30 RT-RT Message Control Block Array (30 Blocks * 16 Dwords = 480 Dwords) */
    struct DLISTNODE *pMemNode;    /* pointer to memory node */
} STREAMITEM;

/*-----------------------------------------------------------------------
Struct: ARRAYITEM

Description:
    This structure contains all information associated with a data array.
------------------------------------------------------------------------*/
#define ARRAYITEM_CTRLBLK_SIZE    256 /* Message Control Block Array (8 Dwords + 32 Data Blocks = 256 Dwords) */

typedef struct ARRAYITEM
{
    U16BIT u16RTAddr;              /* RT Address */
    U16BIT u16RTSubAddr;           /* RT Sub Address */
    U16BIT u16Transfer;            /* Type of Array Transfer (0 = Continuous, 1 = Single) */
    U16BIT u16MsgIndex;            /* Index of Next Message */
    U16BIT u16WordIndex;           /* Index of Next Buffer Word */
    U16BIT u16DevWordCount;        /* Device Message Word Count */
    U16BIT u16BufWordCount;        /* User Buffer Word Count */
    U32BIT u32MsgBaseAddr;         /* Base Address of Message Memory Block */
    U32BIT u32DataBaseAddr;        /* Base Address of Data Memory Block */
    U32BIT    *u32CtrlBlkArray;    /* Message Control Block Array (8 Dwords + 32 Data Blocks = 256 Dwords) */
    struct DLISTNODE *pMemNode;    /* pointer to memory node */
} ARRAYITEM;

/*-----------------------------------------------------------------------
Struct: MEMWORDITEM

Description:
    This structure contains all information associated with a memory dword.
------------------------------------------------------------------------*/
typedef struct MEMWORDITEM
{
    S16BIT MemID;                   /* ID of the Memory Word */
    struct DLISTNODE *pMemNode;     /* pointer to memory node */
} MEMWORDITEM;

/*-----------------------------------------------------------------------
Struct: MEMOBJECTITEM

Description:
    This structure contains all information associated with a memory object.
------------------------------------------------------------------------*/
typedef struct MEMOBJECTITEM
{
    U16BIT MemItemType;            /* Type of Memory Item (ACEX_MEMITEM_DATABLOCK, ACEX_MEMITEM_MSG, ACEX_MEMITEM_MEMWORD) */
    S16BIT MemItemID;              /* ID of the referenced Memory Item (Data, Msg, MemWord) */
    U16BIT Offset;                 /* Offset into the specified memory in dwords. (Not used for ACEX_MEMITEM_MEMWORD) */
    struct DLISTNODE *pMemNode;    /* pointer to memory node */
} MEMOBJECTITEM;

/*-----------------------------------------------------------------------
Struct: SLOTINFO

Description:
    This structure contains all information associated with a BC Time Slot.
------------------------------------------------------------------------*/
typedef struct SLOTINFO
{
    U32BIT u32FreeMsgTime;      /* Total Message Time Used By Slot */
} SLOTINFO;

/*-----------------------------------------------------------------------
Struct: DLISTNODE

Description:
    Node for common double linked list functions. All Dlist functions
    assume that any structure passed to it will have this structure as
    it's first element.
------------------------------------------------------------------------*/
typedef struct DLISTNODE
{
    S16BIT nType;               /* Type of Item (MemBlock,DataTable,...)*/
    S16BIT nID;                 /* Generic ID for sorting and searching */
    struct DLISTNODE *pPrev;    /* Next element in the list or NULL */

    struct DLISTNODE *pNext;    /* Previous element in the list or NULL */

    union ITEM_UNION            /* Item in the list  */
    {
        MEMORYBLOCK MemBlock;
        DATABLOCKITEM DataBlock;
        MESSAGEITEM Message;
        FRAMEITEM Frame;
        OPCODEITEM OpCode;
        SLOTITEM Slot;
        STREAMITEM Stream;
        ARRAYITEM DataArray;
        MEMWORDITEM MemWord;
        MEMOBJECTITEM MemObject;
    }U;
} DLISTNODE;

/*-----------------------------------------------------------------------
Struct: ALISTNODE

  Description:
    Node for a double linked list for aysnchronous messages.
------------------------------------------------------------------------*/
typedef struct ALISTNODE
{
    S16BIT nMsgID;              /* Message ID */
    S32BIT ndwMsgAddr;          /* Ace Address of Message */
    S16BIT nOpID;               /* OpCode Associated with Message */
    U16BIT bSent;               /* Did message succesfully get sent */
    struct ALISTNODE *pPrev;    /* Previous element in the list or NULL */

    struct ALISTNODE *pNext;    /* Next element in the list or NULL */
} ALISTNODE;

/*-----------------------------------------------------------------------
Struct: FLISTNODE

  Description:
    Node for a linked list for frame information.
------------------------------------------------------------------------*/
typedef struct FLISTNODE
{
    S16BIT nFrameID;            /* Frame Identifier */
    S32BIT dwAceAddr;           /* Ace Address of the Frame */
    U16BIT wFrameTime;          /* Time the Frame has to execute in 100uS resolution */
    U16BIT wTotalTime;          /* Total Time consumed in Frame */
    struct FLISTNODE *pNext;    /* Next element in the list or NULL */
} FLISTNODE;

/*-----------------------------------------------------------------------
Struct: REGSTATE

Description:
    This structure contains the all registers that can be written to on
    an Enhanced Mini-ACE.
------------------------------------------------------------------------*/
typedef struct REGSTATE
{
    U16BIT wIMR1;
    U16BIT wCFG1;
    U16BIT wCFG2;
    U16BIT wBCRTCtrl;
    U16BIT wTT;
    U16BIT wCFG3;
    U16BIT wCFG4;
    U16BIT wCFG5;
    U16BIT wRTMTDataStkAddr;
    U16BIT wBCFrmTimeRemain;
    U16BIT wBCRTMTMisc;
    U16BIT wCFG6;
    U16BIT wCFG7;
    U16BIT wBCGPF;
    U16BIT wIMR2;
    U16BIT wBCRTMTQP;
    U16BIT wRTBitWrd;
} REGSTATE;

/*************************  DEVICE STRUCTURES ***************************/

/*-----------------------------------------------------------------------
Struct: BCGPQSTRUCT

Description:
    This structure completely describes a buffered general purpose queue.
------------------------------------------------------------------------*/
#define BC_BUF_GPQ_SIZE 0x40

typedef struct BCGPQSTRUCT
{
    U16BIT wGPQHead;                /* index to the next unread entry */
    U16BIT wGPQTail;                /* index to the next free location */
    U16BIT aGPQ[BC_BUF_GPQ_SIZE];   /* the buffered queue */
} BCGPQSTRUCT;

/*-----------------------------------------------------------------------
Struct: BCOPERATION

Description:
    This structure completely describes Bus Controller mode of operation.
------------------------------------------------------------------------*/

typedef struct BCOPERATION
{
    S32BIT lMjrFrmCount;    /* Number of major frames to run, -1 forever*/
    U32BIT dwOptions;       /* BC Operation options */
    U16BIT wMjrFrmTime;     /* The 'MAJOR' frame time (global) */
    U16BIT wMjrFlags;       /* The 'MAJOR' frame flags (global) */
    U16BIT wWDTimeOut;      /* Watchdog timer timeout */
    U16BIT bRunForever;     /* Is the BC to run forever? */
    U32BIT dwAsyncFrmAddr;  /* Async Frame Address */
    U16BIT wAsyncCount;     /* Counter of Asynch Messages */

    GPQ sGPQ;               /* General Purpose Queue */

    BCGPQSTRUCT sGPQUser;   /* buffered user GPQ */
    BCGPQSTRUCT sGPQLib;    /* buffered RTL GPQ */

    HOSTBUFFER sHBuf;       /* BC Operation Host Buffer */

    DLISTNODE *pFrames;     /* DLIST of frames */
    DLISTNODE *pOpCodes;    /* DLIST of opcodes */
    DLISTNODE *pMsgBlks;    /* DLIST of msg blocks */
    DLISTNODE *pDataBlks;   /* DLIST of data blocks */
    DLISTNODE *pAsyncOp;    /* DLIST of async opcodes */
    ALISTNODE *pAsyncList;  /* ALIST of async messages */
    ALISTNODE *pAListTemp;  /* Temp ALIST of async msgs */
    FLISTNODE *pFrameList;  /* FLIST of frames */
} BCOPERATION;

/*-----------------------------------------------------------------------
Struct: RTOPERATION

Description:
    This structure completely describes Remote Terminal mode of operation.
------------------------------------------------------------------------*/
typedef struct RTSTACK
{
    U32BIT dwCmdBase;         /* Start address of CMD stack 16 ->32 */
    U32BIT dwCmdEnd;          /* End address of command stack */
    U16BIT wCmdSize;          /* Size of the command stack */
    U32BIT dwCurrentMsg;      /* Current msg to process on cmd stk */
    U32BIT dwPrevTT;          /* Previous processed Msg's HW TimeTag value */
    U32BIT dwEndMsg;          /* Last msg to be processed */
    U32BIT dwLost;            /* Number of Stack Msgs lost */
    U32BIT dwHighPct;         /* Highest Perecent ever full */
    U32BIT dwPctFull;         /* Current Percent full */
} RTSTACK;

/*-----------------------------------------------------------------------
Struct:  CBUFINFO

Description:
    This structure holds info about RT Circular Buffers.
------------------------------------------------------------------------*/
typedef struct CBUFINFO
{
    U16BIT bEnabled;          /* Is the circular buffer enabled */
    U16BIT nCircBufID;        /* ID of Circular Buffer          */
    U32BIT dwSize;            /* Size of the circular buffer */
    U32BIT dwStartAddr;       /* Start address of the circular buffer */
} CBUFINFO;

/*-----------------------------------------------------------------------
Struct:  CBUFTYPE

Description:
    This structure holds info about the type of RT Circular Buffers.
------------------------------------------------------------------------*/
typedef struct CBUFTYPE
{
    CBUFINFO Transmit; /* Transmit circular buffer info */
    CBUFINFO Receive;  /* Receive circular buffer info */
    CBUFINFO Broadcast; /* Broadcast circular buffer info */
} CBUFTYPE;

/*-----------------------------------------------------------------------
Struct:  DBLKINFO

Description:
    This structure holds info about RT Data Block.
------------------------------------------------------------------------*/
typedef struct DLBKINFO
{
    U16BIT bEnabled;          /* Is the Data Block enabled */
    U16BIT nDataBlkID;        /* ID of Data Block       */
    U32BIT dwSize;            /* Size of the data block */
    U32BIT dwStartAddr;       /* Start address of the data block */
} DBLKINFO;

/*-----------------------------------------------------------------------
Struct:  DBLKTYPE

Description:
    This structure holds info about the type of RT Data Blocks.
------------------------------------------------------------------------*/
typedef struct DBLKTYPE
{
    DBLKINFO Transmit; /* Transmit data block info */
    DBLKINFO Receive;  /* Receive data block info */
    DBLKINFO Broadcast; /* Broadcast data block info */
} DBLKTYPE;

/*-----------------------------------------------------------------------
Struct: RTOPERATION

Description:
    This structure completely describes Remote Terminal mode of operation.
------------------------------------------------------------------------*/
typedef struct RTOPERATION
{
    U32BIT dwOptions;         /* RT Operation Options */
    U16BIT wUseAltStatus;     /* Use RT in alternate status mode */
    S16BIT nGlobalCircID;     /* ID of global circular buffer */

    RTSTACK sStk;             /* RT Stack information */
    HOSTBUFFER sHBuf;         /* RT Operation Host Buffer */

    DLISTNODE *pDataBlks;     /* DLIST of data blocks */

    DBLKTYPE sDataBlkInfo[32];  /* Data Block Info */
    CBUFTYPE asCircBufInfo[32]; /* Circular Buffer Info */
} RTOPERATION;

/*-----------------------------------------------------------------------
Struct: MTSTACK

Description:
    This structure completely describes Monitor Terminal stack.
------------------------------------------------------------------------*/
typedef struct MTSTACK
{
    U32BIT dwCurrentMsg;    /* Next message to be read off of stack */
    U32BIT dwPrevTT;        /* Previous processed Msg's HW TimeTag value */
    U32BIT dwEndMsg;        /* Last msg to be processed */
    U32BIT dwLost;          /* stack lost messages */
    U16BIT wCmdSize;        /* Size of the command stack(s) */
    U32BIT dwCmdBase;       /* First location of command stack 16->32*/
    U32BIT dwCmdInit;       /* Initial start loc for cmd stack 16->32*/
    U32BIT dwCmdEnd;        /* End location of command stack */
    U16BIT wDataSize;       /* Size of the data stack(s) */
    U32BIT dwDataBase;      /* First location of data stack  */
    U32BIT dwDataInit;      /* Initial start loc for data stack */
    U32BIT dwDataEnd;       /* End location of data stack */
    U32BIT dwHighPct;       /* Highest Perecent ever full */
    U32BIT dwPctFull;       /* Current Percent full */
    U32BIT u32TotalSize;    /* Size of the command stack(s) */
}MTSTACK;

/*-----------------------------------------------------------------------
Struct: MTOPERATION

Description:
    This structure completely describes Monitor Terminal mode of
    operation.
------------------------------------------------------------------------*/
typedef struct MTOPERATION
{
    U32BIT dwOptions;            /* holds all register options set on config*/
    U16BIT wStkMode;             /* DOUBLE or SINGLE(only used in emace mode)*/
    MTTRIGGER sTrig;             /* Trigger Capture structure */
    U16BIT wTrgStartState;       /* TRUE if we should use trigger struct*/
    U16BIT wTrgCurState;         /* TRUE if we are currently not triggered */
    U32BIT dwTrgWrdsHeld;        /* Num of words to hold prior to trig*/
    U32BIT dwDummyPurgeAddr;     /* Dummy BSW purge location for USB command stack overflow condition */

    HOSTBUFFER sMTHBuf;          /* MT Host Buffer */
    HOSTBUFFER sRTMTHBuf;        /* RTMT Host Buffer */
    HOSTBUFFER sMTIntHBuf;       /* MT Internal Host Buffer */
    HOSTBUFFER sTmpRTHBuf;       /* RT Temporary Host Buffer */
    HOSTBUFFER sTmpMTHBuf;       /* MT Temporary Host Buffer */

    MTSTACK sStkA;               /* AREA A stacks */
    MTSTACK sStkB;               /* AREA B stacks */
    U32BIT  MT_MEM_SIZE;
} MTOPERATION;

typedef struct MEMBLK_LIST
{
    DLISTNODE *pMemBlkHead;
    DLISTNODE *pMemBlkFree;

    U32BIT u32MemLength;
    U32BIT u32CardType;
    U32BIT u32MemBA;

    DDC_MUTEX hMutex;
} MEMBLK_LIST;

/*-----------------------------------------------------------------------
Struct: DEVICEINFO

Description:
    This structure completely describes a device's state inside of the
    RTL.
------------------------------------------------------------------------*/
typedef struct DEVICEINFO
{
/* OS independent variables */
    U16BIT wPagedDevice;                /* Is this a paged device? 0=NO, 1=YES */
    U16BIT wCard;
    U16BIT wChannel;                    /* For Multi-Channel Cards */
    U16BIT wAccess;                     /* Simulate,Card Access, or User mem. */
    U16BIT wMode;                       /* Mode of operation: BC,RT,MT,TEST */
    U16BIT wModeOptions;                /* Added Mode Options: TT Reset */
    U16BIT wState;                      /* Current access state */
    U16BIT wMemAddrMode;                /* How to access memory from host */
    U16BIT wRegAddrMode;                /* How to access registers from host */
    U16BIT bLegacy;                     /* Do not use Enhanced Mini-ACE features*/
    U16BIT wRTOnly;                     /* Specifies whether card is RT only */
    U32BIT dwRegAddr;                   /* Register base address */
    U32BIT dwCardRegBaseAddr;           /* Card Register window base address */
    U32BIT dwMemAddr;                   /* Memory base address */
    U32BIT dwMemLength;                 /* size of memory (in words) */
    U32BIT dwMiscInfo;                  /* general purpose info place holder */
    U16BIT wISQHead;                    /* Next unread location of onboard GPQ */
    U16BIT wISQLost;                    /* # of possible GPQ entries lost */
    U16BIT wISQCount;                   /* # of interrupts in the ISQ */
    U16BIT wMTStkArea;                  /* Current MT stack area,  A or B */

    MEMBLK_LIST sMemBlkList;            /* Memory Block list that has head and free memory pointers */

    REGSTATE sReg;                      /* State of ACE registers */

    U16BIT b1553a;                      /* move from BC, RT, MT Op. Use 1553a protocol */

    U16BIT wISQEnabled;                 /* Indicates if MRT/RT/MT ISQ is enabled */

    BCOPERATION     *pBC;               /* Bus Controller pointer */

    RTOPERATION     *pRT;               /* Remote Terminal pointer */

    MTOPERATION     *pMT;               /* Monitor Terminal pointer */
    TESTOPERATION   *pTEST;             /* Test Operation */

    U32BIT dwIrqStatus;                 /* ISR #1, #2 passed back from driver */
    U32BIT dwCardType;                  /* The card being accessed */
    U32BIT dwSysImr;                    /* Internal IMR mask */
    U32BIT dwUsrImr;                    /* External IMR mask */
    U16BIT bMetricEna;                  /* Are Performance metrics enabled */
    U16BIT bSWMPrimeCheck;              /* Is SW Check necessary of M' issues */
    U16BIT wCanBusStatus;               /* Can Bus Interrupt Status for EBR */
    U16BIT wEbrMode;                    /* Ebr Channel? */
    U16BIT wNumOfChnls;                 /* Number of channels on the device */
    U16BIT wHubNum;                     /* Hub Port for the EBR device */
    BOOLEAN bBCStartByExtTrigger;       /* BC external trigger enable flag */

    U32BIT u32BoardCapabilities;        /* capability in '96 card */
    BOOLEAN bRtAutoBoot;                /* RT_AUTO_BOOT in '96 card */
    U16BIT u16RtAddr;
    U32BIT u32MemOffset;

    U32BIT u32MtiBufBlkByteSize;        /* record of the MTI pkt buffer size in driver */

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
    void (_DECL * funcCANIsr)
    (
        S16BIT DevNum,
        U16BIT wIrqStatus
    );
    void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    );

    /* VxWorks ddccm information */
    U16BIT u16DeviceNum;
    U16BIT	u16DeviceType;
    unsigned char card;
    unsigned char channel;
    char sDeviceName[DDC_MAX_DEVICE_NAME_LEN];

} DEVICEINFO;

/* MMU_METRICS global variable */
#if 0 /*#ifdef MMU_METRICS */
U32BIT dwALISTNODEcount;
U32BIT dwBCGPQSTRUCTcount;
U32BIT dwBCOPERATIONcount;
U32BIT dwCBUFINFOcount;
U32BIT dwCBUFTYPEcount;
U32BIT dwDATABLOCKITEMcount;
U32BIT dwDLISTNODEcount;
U32BIT dwFLISTNODEcount;
U32BIT dwFRAMEITEMcount;
U32BIT dwHOSTBUFFERcount;
U32BIT dwMEMORYBLOCKcount;
U32BIT dwMESSAGEITEMcount;
U32BIT dwMSGSTRUCTcount;
U32BIT dwMTOPERATIONcount;
U32BIT dwMTSTACKcount;
U32BIT dwOPCODEITEMcount;
U32BIT dwREGSTATEcount;
U32BIT dwRTSTACKcount;
U32BIT dwRTOPERATIONcount;
#endif /* MMU_METRICS */

#endif /* __STRUCTS_H__ */
