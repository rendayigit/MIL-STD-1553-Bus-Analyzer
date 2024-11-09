#ifndef BC_H_INCLUDED
#define BC_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * BC.H (BUS CONTROLLER MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for BC.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    modified functions from rel 1.6 BC.C that
 *                            relate to bus controller operation
 *
 *   2.1       01-JUN-1995    change gaptime to gp in memory block structure
 *
 *                            added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros
 *
 *                            added BuBCWriteMsgNum function to update
 *                            bc message/frame with message structure
 *
 *   2.2       01-SEP-1995    none
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
 *   5.3.1     11-APR-2003 cf none
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* create message BC->RT */
#ifndef _ACELINUX
#define BuBCXBCtoRT(rt,sa,wc,options,data,gaptime,condition)\
        \
        BuBCXMsgEx((options&0xfff8),\
        BuCreateCmdWord(rt,RX_CMD,sa,wc),\
        0,data,gaptime,condition)
#else
	#define BuBCXBCtoRT(rt,sa,wc,options,data,gaptime,condition) BuBCXMsgEx((options&0xfff8),BuCreateCmdWord(rt,RX_CMD,sa,wc),0,data,gaptime,condition)

#endif

/* create message BC<-RT */
#ifndef _ACELINUX
#define BuBCXRTtoBC(rt,sa,wc,options,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8),\
        BuCreateCmdWord(rt,TX_CMD,sa,wc),\
        0,0,gaptime,condition)

#else
#define BuBCXRTtoBC(rt,sa,wc,options,gaptime,condition) BuBCXMsgEx((options&0xfff8),BuCreateCmdWord(rt,TX_CMD,sa,wc),0,0,gaptime,condition)
#endif

/* create message RT<-RT */
#ifndef _ACELINUX
#define BuBCXRTtoRT(rt,sa,wc,rt2,sa2,options,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_RTTORT,\
        BuCreateCmdWord(rt,RX_CMD,sa,wc),\
        BuCreateCmdWord(rt2,TX_CMD,sa2,wc),\
        0,gaptime,condition)
#else
	#define BuBCXRTtoRT(rt,sa,wc,rt2,sa2,options,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_RTTORT,BuCreateCmdWord(rt,RX_CMD,sa,wc),BuCreateCmdWord(rt2,TX_CMD,sa2,wc),0,gaptime,condition)
#endif

/* create message BC->RT(mode no data)*/
#ifndef _ACELINUX
#define BuBCXMode(rt,options,cmd,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_MODE,\
		    BuCreateCmdWord(rt,TX_CMD,0,cmd),\
        0,0,gaptime,condition)
#else
	#define BuBCXMode(rt,options,cmd,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_MODE,BuCreateCmdWord(rt,TX_CMD,0,cmd),0,0,gaptime,condition)
#endif

/* create message BC<-RT(mode tx data)*/
#ifndef _ACELINUX
#define BuBCXModeTx(rt,options,cmd,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_MODE,\
		    (BuCreateCmdWord(rt,TX_CMD,0,cmd)|0x10),\
        0,0,gaptime,condition)
#else
	#define BuBCXModeTx(rt,options,cmd,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_MODE,(BuCreateCmdWord(rt,TX_CMD,0,cmd)|0x10),0,0,gaptime,condition)
#endif

/* create message BC->RT(mode rx data)*/
#ifndef _ACELINUX
#define BuBCXModeRx(rt,options,cmd,dataword,gaptime,condition)\
        \
        BuBCXMsgEx((options&0xfff8)|CW_MODE,\
		    (BuCreateCmdWord(rt,RX_CMD,0,cmd)|0x10),\
        0,dataword,gaptime,condition)
#else
	#define BuBCXModeRx(rt,options,cmd,dataword,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_MODE,(BuCreateCmdWord(rt,RX_CMD,0,cmd)|0x10),0,dataword,gaptime,condition)
#endif

/* create message BC->RTs(broadcast)*/
#ifndef _ACELINUX
#define BuBCXBrdcst(sa,wc,options,data,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_BRDCST,\
		    BuCreateCmdWord(31,RX_CMD,sa,wc),\
        0,data,gaptime,condition)
#else
	#define BuBCXBrdcst(sa,wc,options,data,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_BRDCST,BuCreateCmdWord(31,RX_CMD,sa,wc),0,data,gaptime,condition)
#endif

/* create message RTs<-RT(broadcast)*/
#ifndef _ACELINUX
#define BuBCXBrdcstRTtoRT(sa,wc,rt2,sa2,options,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_RTTORT,\
		    BuCreateCmdWord(31,RX_CMD,sa,wc),\
		    BuCreateCmdWord(rt2,TX_CMD,sa2,wc),\
        0,gaptime,condition)
#else
	#define BuBCXBrdcstRTtoRT(sa,wc,rt2,sa2,options,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_RTTORT,BuCreateCmdWord(31,RX_CMD,sa,wc),BuCreateCmdWord(rt2,TX_CMD,sa2,wc),0,gaptime,condition)
#endif

/* create message BC->RTs(broadcast mode)*/
#ifndef _ACELINUX
#define BuBCXBrdcstMode(options,cmd,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_MODE,\
		    BuCreateCmdWord(31,TX_CMD,0,cmd),\
        0,0,gaptime,condition)
#else
	#define BuBCXBrdcstMode(options,cmd,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_MODE,BuCreateCmdWord(31,TX_CMD,0,cmd),0,0,gaptime,condition)
#endif

/* create message BC->RTs(broadcast mode rx)*/
#ifndef _ACELINUX
#define BuBCXBrdcstModeRx(options,cmd,data,gaptime,condition)\
        \
		    BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_MODE,\
		    (BuCreateCmdWord(31,RX_CMD,0,cmd)|0x10),\
        0,data,gaptime,condition)
#else
	#define BuBCXBrdcstModeRx(options,cmd,data,gaptime,condition) BuBCXMsgEx((options&0xfff8)|CW_BRDCST|CW_MODE,(BuCreateCmdWord(31,RX_CMD,0,cmd)|0x10),0,data,gaptime,condition)
#endif

/* BC TYPE DEFINITIONS AND STRUCTURES -------------------------------------*/

typedef MemBlockHandle BCMsgHandle;

typedef MemBlockType BCMsgType;

typedef struct BCMinorFrmStruct
{
	U32BIT FrameTime;				/* the minor frame time in µs */
	U16BIT flipped;
	U16BIT framecount;				/* number of times the minor frame has been run */
	U16BIT firstEom;
	U16BIT absaddr;					/* LOAD - address of the descriptor stack */
	U16BIT msgcount;				/* number of msgs in this minor frame */
	BCMsgHandle *data;				/* Msg block handles to each message */
	struct BCMinorFrmStruct *next;

    /* New members */
    U16BIT StkArea;                 /* the stack area the minor frame is running in */
    BCMsgHandle *userMsgs;          /* TestTrack #1267: Save user pointers passed in to frame */

	/* LynnSoft Translation Layer Mod */
	/*------------------------------- */
	U16BIT mnrID;
	U16BIT mjrID;
	U16BIT mnrOpCodeCount;
	U16BIT mjrOpCodeCount;
	S16BIT mnrOpCodes[1024];
	S16BIT mjrOpCodes[1024];
} *BCMinorFrmHandle, BCMinorFrmType;

typedef struct BCStruct
{
	BCMinorFrmHandle CurrentFrame;
	BCMinorFrmHandle FrameStart;
	BCMinorFrmHandle NextFrame;
	BCMinorFrmHandle PreviousFrame;
	U16BIT BCStkSize;				/* each stack will be 256 words, 64 messages */
	U32BIT FrameTimeA;				/* FrameTimeA and FrameTimeB are only */
	U32BIT FrameTimeB;				/* used by BuBCLoadMinor and BuBCRunMinor */
	MemBlockHandle CmdStackA;		/* handle to command stack A */
	MemBlockHandle CmdStackB;		/* handle to command stack B */
	MemBlockHandle BCNop;			/* handle to no operation BC message */

	/* New members */
	U16BIT SysIrqMskReg;			/* Internal interrupt mask register */
	U16BIT UsrIrqMskReg;			/* Users interrupt mask register */
	S16BIT ReadStkMode;				/* determines the stack type (BC or MT) */
	U32BIT ReadStkErrs;				/* # of errors occured reading the stack */
	U32BIT ReadWrdCnt;				/* # of words read into buffers */
	S32BIT MjrFrmCount;				/* # of major frames to run, -1 is forever */
	U32BIT MnrFrmCount;				/* # of minor frames in the frame list */
	U32BIT MnrFrmIndex;				/* Index to the currently running minor frame */
	BCMinorFrmHandle MnrFrmList[1024];	/* List of all minor frames to be run */
} BCType, *BCPtr;

/* BC CONSTANTS -----------------------------------------------------------*/

/* t/~r bit in command word */
#define TX_CMD 0x0400
#define RX_CMD 0x0000

/* bc control word */
#define CW_RTTORT          0x0001
#define CW_BRDCST          0x0002
#define CW_MODE            0x0004
#define CW_1553A           0x0008
#define CW_1553B           0x0000
#define CW_EOMINT          0x0010
#define CW_MASKBRDCST      0x0020
#define CW_SELFTEST        0x0040
#define CW_CHANNELA        0x0080
#define CW_CHANNELB        0x0000
#define CW_RETRYENA        0x0100
#define CW_RESERVEDMASK    0X0200
#define CW_TERMFLAGMASK    0x0400
#define CW_FLAGMASK        0x0800
#define CW_BUSYMASK        0x1000
#define CW_REQMASK         0x2000
#define CW_MSGERRMASK      0x4000

/* bc block status word */
#define BC_EOM             0x8000
#define BC_SOM             0x4000
#define BC_CHANNELB        0x2000
#define BC_ERR             0x1000
#define BC_STATSET         0x0800
#define BC_FORMATERR       0x0400
#define BC_RESP            0x0200
#define BC_LOOPFAIL        0X0100
#define BC_MASKSTATSET     0X0080
#define BC_RETRY1          0X0040
#define BC_RETRY0          0X0020
#define BC_GDDATATX        0X0010
#define BC_WRNGSTADNGAP    0X0008
#define BC_WRDCNTERR       0X0004
#define BC_INCSYNC         0X0002
#define BC_INVALIDWD       0X0001

/* gaptime option */
#define BU_BCNOGAP         0x0001

/* message condition */
#define     BU_BCALWAYS         0x0000
#define     BU_BCNTIMES         0x4000
#define     BU_BCEVERYNTHTIME   0x8000
#define     BU_BCUSERCONDITION  0xC000

/* command stack size */
#define     BC_CMD256      0x0000
#define     BC_CMD512      0x2000
#define     BC_CMD1024     0x4000
#define     BC_CMD2048     0x6000

/* number of bc retries */
#define     BU_BCRETRY0    0xffff
#define     BU_BCRETRY1    0x0000
#define     BU_BCRETRY2    0x0008

/* Active Area */
#define     BU_BCAREASTKA    0x0000
#define     BU_BCAREASTKB    0x2000

/* bus to use on retry */
#define     BU_BCRETRYSAME 0x0000
#define     BU_BCRETRYALT  0x0001

/* trigger to run bc */
#define     BC_TRIGNONE    0x0000
#define     BC_TRIGEXT     0x0080
#define     BC_TRIGINT     0x0040

/* frame buffers */
#define     BU_BCFRMBUFA   0x0000
#define     BU_BCFRMBUFB   0x2000

/* desciptor stack pointers and counters */
#define     BC_STKA        0x0100
#define     BC_MSGCNTA     0x0101
#define     BC_INITSTKA    0x0102
#define     BC_INITMSGCNTA 0x0103
#define     BC_STKB        0x0104
#define     BC_MSGCNTB     0x0105
#define     BC_INITSTKB    0x0106
#define     BC_INITMSGCNTB 0x0107

/* run minor macro definitions */
#define     BU_BCSINGLE     0
#define     BU_BCREPEAT     1

#define BuBCGetGapTime(msg) (msg->gp) /* changed [01-JUN-1995] gaptime to gp */
#define BuBCGetMsgAbsAddr(msg) (msg->absaddr)

/* monitor block status word string */
#ifndef _ACELINUX
#define BCBSWErrorString {"STSET ",   \
                          "FORMT ",   \
                          "NORES ",   \
                          "LPTST ",   \
                          "WSADR ",   \
                          "WRDCT ",   \
                          "INSYN ",   \
                          "INVWD "}
#else
#define BCBSWErrorString {"STSET ","FORMT ","NORES ","LPTST ","WSADR ","WRDCT ","INSYN ","INVWD "}
#endif

/* BC function prototypes -------------------------------------------------*/

/* defines trigger source for bc start */
__BUEXTERN BuError_t __BUDECL BuBCTrigger(U16BIT Mask);

/* determines the currently active stack */
__BUEXTERN U16BIT __BUDECL BuBCActiveStk(void);

/* determines if a frame is being processed */
__BUEXTERN U16BIT __BUDECL BuBCIsFrmActive(void);

/* determines if a message is being processed */
__BUEXTERN U16BIT __BUDECL BuBCIsMsgActive(void);

/* enables gap time field in bc stack */
__BUEXTERN BuError_t __BUDECL BuBCGapTime(U8BIT Sel);

/* enables bus controller autorepeat mode */
__BUEXTERN BuError_t __BUDECL BuBCAutoRepeat(U8BIT Sel);

/* enables bus controller expanded control word */
__BUEXTERN BuError_t __BUDECL BuBCExpCntrlWord(U8BIT Sel);

/* sets bc retry control */
__BUEXTERN BuError_t __BUDECL BuBCRetry(U16BIT RetryNumber,
										U16BIT FirstRetryBus,
										U16BIT SecondRetryBus);

/* sets stack area a or b to active */
__BUEXTERN BuError_t __BUDECL BuBCSetActiveStk(U16BIT Stk);

/* sets the time of minor frame  */
__BUEXTERN BuError_t __BUDECL BuBCSetFrameTime(U32BIT FrameTime);

/* These functions are no longer used by bus controller */
__BUEXTERN BuError_t __BUDECL BuBCCmdStk(void);
__BUEXTERN MemBlockHandle __BUDECL BuBCAllocCmdStk(U16BIT size);

/* creates a Nop message on the board */
__BUEXTERN BuError_t __BUDECL BuBCDefNop(void);

/* write frame and messages to descriptor stack */
__BUEXTERN BuError_t __BUDECL BuBCWriteDesc(BCMinorFrmHandle frm,
											U16BIT address);
/* loads minor frame into stack Area */
__BUEXTERN BuError_t __BUDECL BuBCLoadMinor(U16BIT Area,
											BCMinorFrmHandle Frame);
/* runs a minor frame loaded into an Area */
__BUEXTERN BuError_t __BUDECL BuBCRunMinor(U16BIT Area,U16BIT Repeat);

/* creates a minor frame and returns a handle to the frame */
__BUEXTERN BCMinorFrmHandle __BUDECL BuBCXMinorFrm(U32BIT MinorFrameTime,
												   U16BIT numofmsgs,
												   BCMsgHandle *MsgHandles);
/* free minor frame and associated messages (optional) */
__BUEXTERN void __BUDECL BuBCFreeMinor(BCMinorFrmHandle frm,
									   U16BIT FreeMsgs);
/* calculated number of data words from cmdword and ctrlword */
__BUEXTERN U16BIT __BUDECL BuBCCalcDataSize(U16BIT CtrlWord,U16BIT CmdWord1);

/* calculates size of bc message */
__BUEXTERN U16BIT __BUDECL BuBCCalcSize(U16BIT CtrlWord,U16BIT CmdWord1);

/* creates a bc message and returns handle to the message */
__BUEXTERN BCMsgHandle __BUDECL BuBCXMsg(U16BIT CntrlWord,
										 U16BIT CmdWord1,
										 U16BIT CmdWord2,
										 U16BIT *data,
										 U16BIT gaptime,
										 U8BIT condition);
/* converts bc bsw errors into string */
__BUEXTERN char* __BUDECL BuBCBSWErrorStr(U16BIT value);

/* reads a specific message number from a frame and returns */
__BUEXTERN BuError_t __BUDECL BuBCReadMsgNum(BCMinorFrmHandle frm,
											 U16BIT msgnum,
											 MsgType *readmsg);

/* writes data to a specific message number from a frame */
__BUEXTERN BuError_t __BUDECL BuBCWriteMsgNum(BCMinorFrmHandle frm,
											  U16BIT msgnum,
											  MsgType *writemsg);

/* opens bus controller mode */
__BUEXTERN BuError_t __BUDECL BuBCOpen(void);

/* closes bus controller mode and free's data */
__BUEXTERN BuError_t __BUDECL BuBCClose(void);

/* integrated routine to send data to an RT */
__BUEXTERN BuError_t __BUDECL BuBCSendData(U8BIT channel,
										   U8BIT rt,
										   U8BIT sa,
										   U16BIT *Data,
										   U8BIT length);

/* integrated routine to get data from an RT */
__BUEXTERN BuError_t __BUDECL BuBCGetData(U8BIT channel,
										  U8BIT rt,
										  U8BIT sa,
										  U16BIT *Data,
										  U8BIT length);
/* store minor frame to disk */
__BUEXTERN U16BIT __BUDECL BuBCStoreMinor(char *fn,BCMinorFrmHandle frm);

/* recall minor frame from disk */
__BUEXTERN U16BIT __BUDECL BuBCRecallMinor(char *fn,BCMinorFrmHandle *frm);

/* stores messages from frame to user stack */
__BUEXTERN BuError_t __BUDECL BuBCReadStk(BCMinorFrmHandle frm,
										  U16BIT *Buffer,
										  U16BIT *Len,
										  U8BIT Mode);

/* pops message off user stack and returns message structure */
__BUEXTERN BuError_t __BUDECL BuBCDecodeStk(U16BIT **StackPtr,
											U16BIT *StackLen,
											MsgType *msg);

/* Writes RT Address to the Carriage */
__BUEXTERN BuError_t __BUDECL BuBCWriteCanAddr(U16BIT wRTAddr, U16BIT wCANBus);

/*----------------BC EXTENDED FUNCTIONALITY-----------------------*/

/* This functions sets up the BC command stacks [01-MAR-99]*/
__BUEXTERN BuError_t __BUDECL BuBCSetSizeOfCmdStks(U16BIT wSize);

/* creates a bc message and returns handle to the message */
__BUEXTERN BCMsgHandle __BUDECL BuBCXMsgEx(U16BIT wCtrlWrd,
										   U16BIT wCmdWrd1,
										   U16BIT wCmdWrd2,
										   U16BIT *pData,
										   U16BIT wGapTime,
										   U16BIT wCondition);

__BUEXTERN BuError_t __BUDECL BuBCXMajorFrm(U16BIT wNumOfMinorFrms,
								 BCMinorFrmHandle *pMinorFrms);

__BUEXTERN BuError_t __BUDECL BuBCRunMajor(S32BIT lMjrFrmCount,S16BIT wMode);
__BUEXTERN BuError_t __BUDECL BuBCHaltMajor(void);


/* Information functions for use while major frames are running */
__BUEXTERN BuError_t __BUDECL BuBCGetMajorFrmsRemaining(S32BIT *pMjrFrmsRemain);
__BUEXTERN BuError_t __BUDECL BuBCGetMinorFrmIndex(U32BIT *pMnrFrmIndex);


__BUEXTERN BuError_t __BUDECL BuBCReadMsgNumEx(BCMinorFrmHandle hMinorFrm,
											   U16BIT wMsgNum,
											   MsgType *pMsg);

__BUEXTERN BuError_t __BUDECL BuBCReplaceMsg(BCMinorFrmHandle hMinorFrm,
											 U16BIT wMsgNum,
											 BCMsgHandle hMsg);

__BUEXTERN BCMsgHandle __BUDECL BuBCGetNopMsg(void);

__BUEXTERN BuError_t __BUDECL BuBCSetUsrIrqMask(U16BIT bEnable,U16BIT wMask);
__BUEXTERN BuError_t __BUDECL BuBCSetSysIrqMask(U16BIT bEnable,U16BIT wMask);
/*
 * END
 * BC.H (BUS CONTROLLER MODULE)
 */

#endif /*BC_H_INCLUDED */

