#ifndef MT_H_INCLUDED
#define MT_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * MT.H (MONITOR MODE MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for MT.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    Converted rel 1.6 to work with BuConf structure
 *
 *   2.1       01-JUN-1995    added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros
 *
 *   2.2       01-SEP-1995    added MTcount and MTerror message and error
 *                            counters into MTType structure as long integers
 *                            (used in BuMTReadStk())
 *
 *   2.3       01-JAN-1996    none
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       04-DEC-1996    none
 *
 *   4.0       29-NOV-1997    none
 *
 *   4.1       05-AUG-1998    added BuMTReadStkTrig() to allow triggering in 
 *                            monitor mode.
 *
 *   5.3.1     11-APR-2003    none
 */

/* MONITOR CONSTANTS ------------------------------------------------------*/

/* used with BuMTEnableRT and BuMTDisableRT to define */
/* selective capture of RT's */
#define MT_ALL          0xffff
#define MT_RX           0x0
#define MT_TX           0x1

/* flag to determine if message left on inactive stack */
#define MT_NOMESSAGE	  0xffff

/* flags to notify that a MT Cmd Stack has overflown */
#define MT_MSGS_AND_MTCMDSTKOVR 0x0002
#define MT_CMDSTKOVRANDCLR 0x0003

/* MT Cmd Stack Overflow Strategies */
#define MT_CMD_STK_CONT 0x0000
#define MT_CMD_STK_CLR  0x0001

/* used with BuMTStkType to define monitor stacks */
#define MT_SINGLE       0
#define MT_DOUBLE       1
#define CMD256          0x0000
#define CMD1024         0x0800
#define CMD4096         0x1000
#define CMD16384        0x1800
#define DATA512         0x0700
#define DATA1024        0x0600
#define DATA2048        0x0500
#define DATA4096        0x0400
#define DATA8192        0x0300
#define DATA16384       0x0200
#define DATA32768       0x0100
#define DATA65536       0x0000

/* monitor stack area constants */
#define MT_STACKA       0x0000
#define MT_STACKB       0x2000
#define CMDA            0x0102
#define DATAA           0x0103
#define CMDB            0x0106
#define DATAB           0x0107
#define SELMTLOOKUP     0x0280

/* monitor control register commands */
#define MT_START        0x0002
#define MT_HALT         0x0040

/* monitor block status word */
#define MT_EOM          0x8000
#define MT_SOM          0x4000
#define MT_CHANNELB     0x2000
#define MT_ERR          0x1000
#define MT_RTTORT       0x0800
#define MT_FORMATERR    0x0400
#define MT_RESP         0x0200
#define MT_GOODDATA     0X0100
#define MT_DATA_ROLL    0X0080
#define MT_WC_ERR       0X0020
#define MT_BADSYNC      0X0010
#define MT_BADWORD      0X0008
#define MT_RTRTGAPERR   0X0004
#define MT_RTTTCMD2ERR  0X0002
#define MT_CMDERR       0X0001

/* monitor block status word string */
#ifdef _ACELINUX
	#define MTBSWErrorString {"FORMT ","NORES ","WDCNT ","INSYN ","INVWD "}            
#else

#define MTBSWErrorString {"FORMT ",                                         \
                          "NORES ",                                         \
                          "WDCNT ",                                         \
                          "INSYN ",                                         \
                          "INVWD "}
#endif                         

/* Trigger Constants */
#define MT_DONT_TRIG		0x0000
#define MT_TRIGFLG_NOT		0x0001
#define MT_TRIGFLG_OR		0x0002
#define MT_DO_TRIG			0x8000



/* MONITOR STRUCTURES -----------------------------------------------------*/

/* struct used to keep track of the locations of the stacks */
typedef struct MTBufferStruct {
     U16BIT CmdStart;        /* where in ram a command stack starts */
     U16BIT CmdEnd;          /* where in ram a command stack ends   */
     U16BIT DataStart;       /* where in ram a data stack starts    */
     U16BIT DataEnd;         /* where in ram a data stack ends      */
     U16BIT InitCmd;         /* init offset into command stack      */
     U16BIT InitData;        /* init offset into data stack         */
     U16BIT MTLastReadMsg;   /* loc of last message read off stack  */
     U16BIT StkPtr;          /* loc of stack pointer in shared RAM  */
     U16BIT DataPtr;         /* loc of data stack ptr in shared RAM */
} MTBufferType;


/* Structures for counters and triggers */
typedef struct MTTriggerStruct {
	U16BIT CmdVal1;		/* trig on this Cmd */
	U16BIT CmdMsk1;		/* Msk these bits when checking for trig */
	U16BIT CmdVal2;		/* trig on this Cmd in RT-RT */
	U16BIT CmdMsk2;		/* Msk these bits when checking for trig */
	U16BIT DataVal;		/* trig on this Data */
	U16BIT DataMsk;		/* Msk these bits when checking for trig */
	U16BIT DataPos;		/* position of Data to look at */
	U16BIT StsVal1;		/* trig on this status */
	U16BIT StsMsk1;		/* Msk these bits when checking for trig */
	U16BIT StsVal2;		/* trig on this status in RT-RT */
	U16BIT StsMsk2;		/* Msk these bits when checking for trig */
	U16BIT ErrVal;		/* trigger on errors in Block Status */
	U16BIT CmdFlgs;		/* flags indicate how trigger should be performed */
	U16BIT DataFlgs;	/* flags indicate how trigger should be performed */
	U16BIT StsFlgs;		/* flags indicate how trigger should be performed */
	U16BIT ErrFlgs;		/* flags indicate how trigger should be performed */
	U16BIT NxtCond;		/* indicates next condition to trigger on */
	U16BIT Count;		/* Number of trig's needed to produce real trigger */
} MTTriggerType;

typedef struct MTCounterStruct {
	U16BIT StkErrors;
	U32BIT MsgCount;
	U32BIT MsgErrors;
	U32BIT RTCount[2][32];
} MTCounterType;

/* structure containing data private to the module */

typedef struct MTStruct {
    /* internal var used to keep track of the size of the cmd stack */
     U16BIT MTCmdSize;
     /* internal var to keep track of the buffering mode */
     U16BIT MTBufferMode;
     /* internal var used to keep track of the data stack size */
     U16BIT MTDataSize;
     /* internal var to be used with ReadStack */
     U16BIT MTActiveStack;
     /* flag to indicated a message left on the inactive stack */
     U16BIT MTMessageOnInactive;
     /* structures to keep track of command stacks */
     MTBufferType MTStkA,MTStkB;
	 /* structure to keep track of trigger conditions */
	 MTTriggerType MTTrig;
	 /* struct to keep track of counts */
	 MTCounterType MTCount;
	 /* internal var to keep track of Cmd Stack overflow strategy */
	 U16BIT MTCmdOvrStrat;

     /* MT module msg and error counters added [01-SEP-1995] */
     U32BIT MTcount;
     U32BIT MTerror;
} MTType,*MTPtr;

/* MONITOR FUNCTION PROTOTYPES --------------------------------------------*/

#ifdef __BUDEBUG_FUNCTIONS

/* returns the active stack */
__BUEXTERN U16BIT __BUDECL BuMTGetActiveStk( void);

/* returns the inactive stack */
__BUEXTERN U16BIT __BUDECL BuMTGetInactiveStk( void);

/* checks to see if a stack is inactive */
__BUEXTERN U16BIT __BUDECL BuMTIsActiveStk( U16BIT stk);

/* returns address of lookup word associated with rt, tr, and sa */
__BUEXTERN U16BIT __BUDECL BuMTLookup( U16BIT rt,U16BIT tr, U16BIT sa);

/* begins capture of messages but does not initialize stack ptrs */
__BUEXTERN BuError_t __BUDECL BuMTContinue( void);

/* puts monitor in idle mode */
__BUEXTERN BuError_t __BUDECL BuMTHalt( void);

#else

#define BuMTGetActiveStk() (BuReadReg(CONFIG_1)&0x2000)
#define BuMTGetInactiveStk() ((BuReadReg(CONFIG_1)&0x2000)^0x2000)
#define BuMTIsActiveStk(stk) ((BuReadReg(CONFIG_1)&0x2000)==stk)
#define BuMTLookup(rt,tr,sa) ((rt*4)+(tr*2)+((sa&0x10)>>4)+SELMTLOOKUP)
#define BuMTContinue() (BuWriteReg(CONTROL,MT_START))
#define BuMTHalt() (BuWriteReg(CONTROL,MT_HALT))

#endif

/* sets the active stack */
__BUEXTERN BuError_t __BUDECL BuMTSetActiveStk( U16BIT stk);

/* swaps inactive and active stacks */
__BUEXTERN BuError_t __BUDECL BuMTSwapStk( void);

/* rt, tr, sa combination to capture */
__BUEXTERN BuError_t __BUDECL BuMTEnableRT( U16BIT rt,
                                            U16BIT tr,
                                            U16BIT sa);

/* rt, tr, sa combination to ignore */
__BUEXTERN BuError_t __BUDECL BuMTDisableRT( U16BIT rt,
                                             U16BIT tr,
                                             U16BIT sa);

/* writes array of 16x128 selective capture table from a buffer */
__BUEXTERN BuError_t __BUDECL BuMTWriteTable( U16BIT *TblPtr);

/* reads array of 16x128 selective capture table to a buffer */
__BUEXTERN BuError_t __BUDECL BuMTReadTable( U16BIT *TblPtr);

/* reads message from memory address */
__BUEXTERN BuError_t __BUDECL BuMTReadMsgAtAddr( MsgType *msg,U16BIT Cmd);

/* converts mt bsw errors into string */
__BUEXTERN char* __BUDECL BuMTBSWErrorStr( U16BIT value);

/* returns message from top of stack if available */
__BUEXTERN U16BIT __BUDECL BuMTReadMsg( MsgType *msg);

/* defines a monitor stack's starting locations */
__BUEXTERN BuError_t __BUDECL BuMTDefStk( U16BIT stack,
                                          U16BIT CmdStart,
                                          U16BIT CmdInitial,
                                          U16BIT DataStart,
                                          U16BIT DataInitial);

/* defines buffering mode and size of the stacks */
__BUEXTERN BuError_t __BUDECL BuMTStkType( U16BIT Type,
                                           U16BIT CmdSize,
                                           U16BIT DataSize);

/* initializes stack ptrs and begins capture of messages */
__BUEXTERN BuError_t __BUDECL BuMTRun( void);

/* must be called at beginning to initialize monitor mode */
__BUEXTERN BuError_t __BUDECL BuMTOpen( void);

/* must be called at end to clear up mode and free data */
__BUEXTERN BuError_t __BUDECL BuMTClose( void);

/* swaps active areas and reads stack into buffer */
__BUEXTERN BuError_t __BUDECL BuMTReadStk( U16BIT *msgarray,
                                           U16BIT *Length);

/* pops message off buffer and returns message structure */
__BUEXTERN BuError_t __BUDECL BuMTDecodeStk( U16BIT **StackPtr,
                                             U16BIT *StackLen,
                                             MsgType *msg);

/* swaps active areas and reads stack into buffer */
__BUEXTERN BuError_t __BUDECL BuMTReadStkTrig(U16BIT *msgarray,U16BIT *Length);

/* sets the strategy for handling MT Cmd Stack Overflows */
__BUEXTERN BuError_t __BUDECL BuMTSetOvrFlwStrategy(U16BIT strategy);

/*
 * END
 * MT.H (MONITOR MODE MODULE)
 */
#endif /*MT_H_INCLUDED */

