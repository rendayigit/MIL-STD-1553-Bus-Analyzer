#ifndef ADVSTDACE_H_INCLUDED
#define ADVSTDACE_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * ACESTDACE.H (ADVANCED ACE FUNCTION PROTOTYPES)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for ADVSTDACE.H (HEADER FILE)
 *
 *
 *   5.3.1     11-APR-2003 cf initial release
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* ACE FUNCTION PROTOTYPES -------------------------------------------------*/

#ifdef __BUDEBUG_FUNCTIONS

/* enables 1553A/1553B mode code handling */
__BUEXTERN BuError_t __BUDECL advBu1553AModeCd(BuConf_p BuConf, U8BIT Sel);

/* enables or disables retries with 1553A/1553B* bit of BC Control Word */
__BUEXTERN BuError_t __BUDECL advBuRetryIfAandME(BuConf_p BuConf, U8BIT Sel);

/* enables or disables retries with bit in Status set condition */
__BUEXTERN BuError_t __BUDECL advBuRetryStatusSet(BuConf_p BuConf,U8BIT Sel);

/* enables or disables word boundaries */
__BUEXTERN BuError_t __BUDECL advBuWordBoundaries(BuConf_p BuConf, U8BIT Sel);

/* enables or disables ram parity checking */
__BUEXTERN BuError_t __BUDECL advBuRamParityCheck(BuConf_p BuConf, U8BIT Sel);

/* setup ACE for 12 or 16 mhz clock */
__BUEXTERN BuError_t __BUDECL advBuClockSel(BuConf_p BuConf, U8BIT Sel);

/* setup ACE for single or double clock edge sampling */
__BUEXTERN BuError_t __BUDECL advBuSamplingSel(BuConf_p BuConf, U8BIT Sel);

/* returns the value of the timetag register */
__BUEXTERN U16BIT __BUDECL advBuReadTimeTag(BuConf_p BuConf);

/* sets the response timeout time for an rt */
__BUEXTERN BuError_t __BUDECL advBuTimeout( BuConf_p BuConf,U16BIT Value);

/* sets the time tag resoultion */
__BUEXTERN BuError_t __BUDECL advBuTimeTagResolution(BuConf_p BuConf, U16BIT Value);

/* stops BC at end of current message */
__BUEXTERN BuError_t __BUDECL advBuStopOnMessage(BuConf_p BuConf);

/* stops BC at end of current frame */
__BUEXTERN BuError_t __BUDECL advBuStopOnFrame(BuConf_p BuConf);

/* increment tt clock by 1 lsb if in test mode */
__BUEXTERN BuError_t __BUDECL advBuTimeTagTest(BuConf_p BuConf);

/* resets time tag register to 0 */
__BUEXTERN BuError_t __BUDECL advBuTimeTagReset(BuConf_p BuConf);

/* start command for BC and MT modes */
__BUEXTERN BuError_t __BUDECL advBuBCMTStart(BuConf_p BuConf);

/* reset ACE, all registers go to 0000h */
__BUEXTERN BuError_t __BUDECL advBuReset(BuConf_p BuConf);

/* stop eom on message error condtion */
__BUEXTERN BuError_t __BUDECL advBuMsgStopOnError(BuConf_p BuConf, U16BIT Sel);

/* stop eof on message error condtion */
__BUEXTERN BuError_t __BUDECL advBuFrameStopOnError(BuConf_p BuConf,U16BIT Sel);

/* stop eom on status set condtion */
__BUEXTERN BuError_t __BUDECL advBuStatusStopOnMsg(BuConf_p BuConf, U16BIT Sel);

/* stop eof on status set condtion */
__BUEXTERN BuError_t __BUDECL advBuStatusStopOnFrame(BuConf_p BuConf, U16BIT Sel);

/* message valid if message error bit an no data */
__BUEXTERN BuError_t __BUDECL advBuValidMENoData(BuConf_p BuConf, U16BIT Sel);

/* message valid if busy bit an no data */
__BUEXTERN BuError_t __BUDECL advBuValidBUSYNoData(BuConf_p BuConf, U16BIT Sel);

/* creates 1553 command word from rt, t/~r, sa, and wc */
/* added BUEXTERN 8-27-96 */
__BUEXTERN U16BIT __BUDECL advBuCreateCmdWord( U16BIT rt,U16BIT tr,U16BIT sa,U16BIT wc);
#else

#define advBu1553AModeCd(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xfffd)|((Sel)?0x0002:0))
#define advBuRetryIfAandME(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_4,(advBuReadReg(BuConf,CONFIG_4)&0xfbff)|((Sel)?0x0400:0))
#define advBuRetryStatusSet(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_4,(advBuReadReg(BuConf,CONFIG_4)&0xfdff)|((Sel)?0x0200:0))
#define advBuWordBoundaries(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_2,(advBuReadReg(BuConf,CONFIG_2)&0xfbff)|((Sel)?0:0x0400))
#define advBuRamParityCheck(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_2,(advBuReadReg(BuConf,CONFIG_2)&0xbfff)|((Sel)?0x4000:0))
#define advBuClockSel(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_5,(advBuReadReg(BuConf,CONFIG_5)&0x7fff)|((Sel)?0x8000:0))
#define advBuSamplingSel(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_5,(advBuReadReg(BuConf,CONFIG_5)&0x3fff)|((Sel)?0x4000:0))
#define advBuReadTimeTag(BuConf) advBuReadReg(BuConf,TIMETAG)
#define advBuTimeout(BuConf,Value) advBuWriteReg(BuConf,CONFIG_5,(advBuReadReg(BuConf,CONFIG_5)&0xf9ff)|Value)
#define advBuTimeTagResolution(BuConf,Value) advBuWriteReg(BuConf,CONFIG_2,(advBuReadReg(BuConf,CONFIG_2)&0xfc7f)|Value)
#define advBuStopOnMessage(BuConf) advBuWriteReg(BuConf,CONTROL,0x0040)
#define advBuStopOnFrame(BuConf) advBuWriteReg(BuConf,CONTROL,0x0020)
#define advBuTimeTagTest(BuConf) advBuWriteReg(BuConf,CONTROL,0x0010)
#define advBuTimeTagReset(BuConf) advBuWriteReg(BuConf,CONTROL,0x0008)
#define advBuBCMTStart(BuConf) advBuWriteReg(BuConf,CONTROL,0x0002)
#define advBuReset(BuConf) advBuWriteReg(BuConf,CONTROL,0x0001)
#define advBuMsgStopOnError(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xefff)|(Sel<<12))
#define advBuFrameStopOnError(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xf7ff)|(Sel<<11))
#define advBuStatusStopOnMsg(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xfbff)|(Sel<<10))
#define advBuStatusStopOnFrame(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xfdff)|(Sel<<9))
#define advBuValidMENoData(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xffbf)|0x0040)
#define advBuValidBUSYNoData(BuConf,Sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xffdf)|0x0020)
/* BuCreateCmdWord modified [01-JAN-1996] to prevent error when setting 
 * word count>31, it was possibly altering the subaddress
 */
#define advBuCreateCmdWord(rt,tr,sa,wc) (((rt<<11)|((tr!=0)<<10)|(sa<<5)|(wc&0x001f)))
#endif

/* enables or disables enhanced ACE features mode */
__BUEXTERN BuError_t __BUDECL advBuEnhancedMode(BuConf_p BuConf, U8BIT Sel);

/* preset ACE to library defaults */
__BUEXTERN BuError_t __BUDECL advBuPreset(BuConf_p BuConf);

/* splits command word into sub-words */
__BUEXTERN void __BUDECL advBuParseCmdWord( U16BIT cmdword,
                                         U16BIT *rt,
                                         U16BIT *tr,
                                         U16BIT *sa,
                                         U16BIT *wc);

/* fills in the DataLength field of MsgType structure */
__BUEXTERN U16BIT __BUDECL advBuGetMsgWordCount( MsgType*msg);

/* fills in the Type field of MsgType structure */
__BUEXTERN U16BIT __BUDECL advBuGetMsgType(BuConf_p BuConf, MsgType*msg,U8BIT Mode);

/* converts message type value to string */
__BUEXTERN char* __BUDECL advBuMsgTypeStr( U8BIT value);

/* converts 1553 cmd word into string */
__BUEXTERN char* __BUDECL advBuCmdStr( U16BIT value);


/*---------------- ACEMEM Prototypes ----------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL advBuFreeMemBlockEx(BuConf_p BuConf,MemBlockHandle hOldBlk,U8BIT cFreeType);
__BUEXTERN MemBlockHandle __BUDECL advBuAllocMemBlockEx(BuConf_p BuConf,
														U8BIT wType,
													    U16BIT wSize,
													    U16BIT wStartAddr,
													    U16BIT wBoundary,
													    U16BIT wCrossOver,
													    MemBlockHandle hStartBlk);

/* closes memory block list and frees all structures */
__BUEXTERN BuError_t __BUDECL advBuCloseBlockList(BuConf_p BuConf);

/* clears memory block list of everyting but protected blocks */
__BUEXTERN BuError_t __BUDECL advBuClearBlockList(BuConf_p BuConf);

/* initializes the memory block list */
__BUEXTERN BuError_t __BUDECL advBuInitBlockList(BuConf_p BuConf);

/* creates the handle for a memory block */
__BUEXTERN MemBlockHandle __BUDECL advCreateMemBlockHandle(void);

/* removes memory block from list */
__BUEXTERN BuError_t __BUDECL advMemBlockRemove(BuConf_p BuConf,MemBlockHandle p);

/* inserts memory block before (at) */
__BUEXTERN BuError_t __BUDECL advMemBlockInsert(BuConf_p BuConf,MemBlockHandle at,MemBlockHandle msg);

/* swaps one memory block with another */
__BUEXTERN BuError_t __BUDECL advSwapMemBlocks(BuConf_p BuConf, MemBlockHandle p,
                                             MemBlockHandle q);

/* creates protected memory block at specific address and size */
__BUEXTERN MemBlockHandle __BUDECL advCreateProtectedMemBlock(BuConf_p BuConf, MemBlockHandle blk,
				                                                    U16BIT addr,
				                                                    U16BIT size);

/* creates permanent memory block at specific address and size */
__BUEXTERN MemBlockHandle __BUDECL advCreatePermanentMemBlock(BuConf_p BuConf, MemBlockHandle blk,
				                                                    U16BIT addr,
				                                                    U16BIT size);

/* allocates memory block within unused memory block (area) */
__BUEXTERN MemBlockHandle __BUDECL advAllocateOnBoard(BuConf_p BuConf, MemBlockHandle area,
                                                    U16BIT size);

/* allocates an off board memory block (host memory) */
__BUEXTERN MemBlockHandle __BUDECL advAllocOffBoard(BuConf_p BuConf, U16BIT size);

/* allocates memory block handle on boundary condition */
__BUEXTERN MemBlockHandle __BUDECL advBuAllocHandleBoundary(BuConf_p BuConf, U16BIT size,
                                                          U16BIT boundary);
/* allocates a memory block and returns the handle */
__BUEXTERN MemBlockHandle __BUDECL advBuAllocHandle(BuConf_p BuConf, U16BIT size);

/* frees an allocated memory block */
__BUEXTERN BuError_t __BUDECL advBuReleaseHandle(BuConf_p BuConf, MemBlockHandle p);

/* finds an unused memory block of >=size with boundry condition and
 * returns a handle [01-JUN-1995 added boundary parameter]
 */
__BUEXTERN MemBlockHandle __BUDECL advBuFindSpace(BuConf_p BuConf, U16BIT size,U16BIT boundary);

/* write data buffer to memory block */
__BUEXTERN BuError_t __BUDECL advBuWriteBlk(BuConf_p BuConf, MemBlockHandle BlockHdl,
                                          U16BIT *DataPtr,U16BIT Size);

/* reads memory block contents into data buffer */
__BUEXTERN BuError_t __BUDECL advBuReadBlk(BuConf_p BuConf, MemBlockHandle BlockHdl,
                                         U16BIT *DataPtr,U16BIT Size);

__BUEXTERN void __BUDECL advBuListDump(BuConf_p BuConf);

__BUEXTERN void __BUDECL advBuLoopBackTestInt(BuConf_p BuConf, BuTest_t*BuTest);

__BUEXTERN void __BUDECL advBuEBRLoopBackTest(BuConf_p BuConf, BuTest_t*BuTest, U16BIT HubNumTx, U16BIT HubNumRx);

/* BC FUNCTION PROTOTYPES -------------------------------------------------*/
/* create message BC->RT */
#ifndef _ACELINUX
#define advBuBCXBCtoRT(BuConf,rt,sa,wc,options,data,gaptime,condition)\
        \
        advBuBCXMsgEx(BuConf,(options&0xfff8),\
        advBuCreateCmdWord(rt,RX_CMD,sa,wc),\
        0,data,gaptime,condition)
#else
	#define advBuBCXBCtoRT(BuConf,rt,sa,wc,options,data,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8),advBuCreateCmdWord(rt,RX_CMD,sa,wc),0,data,gaptime,condition)

#endif

/* create message BC<-RT */
#ifndef _ACELINUX
#define advBuBCXRTtoBC(BuConf,rt,sa,wc,options,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8),\
        advBuCreateCmdWord(rt,TX_CMD,sa,wc),\
        0,0,gaptime,condition)

#else
#define advBuBCXRTtoBC(BuConf,rt,sa,wc,options,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8),advBuCreateCmdWord(rt,TX_CMD,sa,wc),0,0,gaptime,condition)
#endif

/* create message RT<-RT */
#ifndef _ACELINUX
#define advBuBCXRTtoRT(BuConf,rt,sa,wc,rt2,sa2,options,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_RTTORT,\
        advBuCreateCmdWord(rt,RX_CMD,sa,wc),\
        advBuCreateCmdWord(rt2,TX_CMD,sa2,wc),\
        0,gaptime,condition)
#else
	#define advBuBCXRTtoRT(BuConf,rt,sa,wc,rt2,sa2,options,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_RTTORT,BuCreateCmdWord(rt,RX_CMD,sa,wc),advBuCreateCmdWord(rt2,TX_CMD,sa2,wc),0,gaptime,condition)
#endif

/* create message BC->RT(mode no data)*/
#ifndef _ACELINUX
#define advBuBCXMode(BuConf,rt,options,cmd,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,\
		    advBuCreateCmdWord(rt,TX_CMD,0,cmd),\
        0,0,gaptime,condition)
#else
	#define advBuBCXMode(BuConf,rt,options,cmd,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,advBuCreateCmdWord(rt,TX_CMD,0,cmd),0,0,gaptime,condition)
#endif

/* create message BC<-RT(mode tx data)*/
#ifndef _ACELINUX
#define advBuBCXModeTx(BuConf,rt,options,cmd,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,\
		    (advBuCreateCmdWord(rt,TX_CMD,0,cmd)|0x10),\
        0,0,gaptime,condition)
#else
	#define advBuBCXModeTx(BuConf,rt,options,cmd,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,(advBuCreateCmdWord(rt,TX_CMD,0,cmd)|0x10),0,0,gaptime,condition)
#endif

/* create message BC->RT(mode rx data)*/
#ifndef _ACELINUX
#define advBuBCXModeRx(BuConf,rt,options,cmd,dataword,gaptime,condition)\
        \
        advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,\
		    (advBuCreateCmdWord(rt,RX_CMD,0,cmd)|0x10),\
        0,dataword,gaptime,condition)
#else
	#define advBuBCXModeRx(BuConf,rt,options,cmd,dataword,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_MODE,(advBuCreateCmdWord(rt,RX_CMD,0,cmd)|0x10),0,dataword,gaptime,condition)
#endif

/* create message BC->RTs(broadcast)*/
#ifndef _ACELINUX
#define advBuBCXBrdcst(BuConf,sa,wc,options,data,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST,\
		    advBuCreateCmdWord(31,RX_CMD,sa,wc),\
        0,data,gaptime,condition)
#else
	#define advBuBCXBrdcst(BuConf,sa,wc,options,data,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST,advBuCreateCmdWord(31,RX_CMD,sa,wc),0,data,gaptime,condition)
#endif

/* create message RTs<-RT(broadcast)*/
#ifndef _ACELINUX
#define advBuBCXBrdcstRTtoRT(BuConf,sa,wc,rt2,sa2,options,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_RTTORT,\
		    advBuCreateCmdWord(31,RX_CMD,sa,wc),\
		    advBuCreateCmdWord(rt2,TX_CMD,sa2,wc),\
        0,gaptime,condition)
#else
	#define advBuBCXBrdcstRTtoRT(BuConf,sa,wc,rt2,sa2,options,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_RTTORT,advBuCreateCmdWord(31,RX_CMD,sa,wc),BuCreateCmdWord(rt2,TX_CMD,sa2,wc),0,gaptime,condition)
#endif

/* create message BC->RTs(broadcast mode)*/
#ifndef _ACELINUX
#define advBuBCXBrdcstMode(BuConf,options,cmd,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_MODE,\
		    advBuCreateCmdWord(31,TX_CMD,0,cmd),\
        0,0,gaptime,condition)
#else
	#define advBuBCXBrdcstMode(BuConf,options,cmd,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_MODE,advBuCreateCmdWord(31,TX_CMD,0,cmd),0,0,gaptime,condition)
#endif

/* create message BC->RTs(broadcast mode rx)*/
#ifndef _ACELINUX
#define advBuBCXBrdcstModeRx(BuConf,options,cmd,data,gaptime,condition)\
        \
		    advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_MODE,\
		    (advBuCreateCmdWord(31,RX_CMD,0,cmd)|0x10),\
        0,data,gaptime,condition)
#else
	#define advBuBCXBrdcstModeRx(BuConf,options,cmd,data,gaptime,condition) advBuBCXMsgEx(BuConf,(options&0xfff8)|CW_BRDCST|CW_MODE,(advBuCreateCmdWord(31,RX_CMD,0,cmd)|0x10),0,data,gaptime,condition)
#endif

/* defines trigger source for bc start */
__BUEXTERN BuError_t __BUDECL advBuBCTrigger(BuConf_p BuConf,U16BIT Mask);

/* determines the currently active stack */
__BUEXTERN U16BIT __BUDECL advBuBCActiveStk(BuConf_p BuConf);

/* determines if a frame is being processed */
__BUEXTERN U16BIT __BUDECL advBuBCIsFrmActive(BuConf_p BuConf);

/* determines if a message is being processed */
__BUEXTERN U16BIT __BUDECL advBuBCIsMsgActive(BuConf_p BuConf);

/* enables gap time field in bc stack */
__BUEXTERN BuError_t __BUDECL advBuBCGapTime(BuConf_p BuConf,U8BIT Sel);

/* enables bus controller autorepeat mode */
__BUEXTERN BuError_t __BUDECL advBuBCAutoRepeat(BuConf_p BuConf,U8BIT Sel);

/* enables bus controller expanded control word */
__BUEXTERN BuError_t __BUDECL advBuBCExpCntrlWord(BuConf_p BuConf,U8BIT Sel);

/* sets bc retry control */
__BUEXTERN BuError_t __BUDECL advBuBCRetry(BuConf_p BuConf,
										   U16BIT RetryNumber,
										   U16BIT FirstRetryBus,
										   U16BIT SecondRetryBus);

/* sets stack area a or b to active */
__BUEXTERN BuError_t __BUDECL advBuBCSetActiveStk(BuConf_p BuConf,U16BIT Stk);

/* sets the time of minor frame  */
__BUEXTERN BuError_t __BUDECL advBuBCSetFrameTime(BuConf_p BuConf,U32BIT FrameTime);

/* These functions are no longer used by bus controller */
__BUEXTERN BuError_t __BUDECL advBuBCCmdStk(BuConf_p BuConf);
__BUEXTERN MemBlockHandle __BUDECL advBuBCAllocCmdStk(BuConf_p BuConf,U16BIT size);

/* creates a Nop message on the board */
__BUEXTERN BuError_t __BUDECL advBuBCDefNop(BuConf_p BuConf);

/* write frame and messages to descriptor stack */
__BUEXTERN BuError_t __BUDECL advBuBCWriteDesc(BuConf_p BuConf,BCMinorFrmHandle frm,
											U16BIT address);
/* loads minor frame into stack Area */
__BUEXTERN BuError_t __BUDECL advBuBCLoadMinor(BuConf_p BuConf,U16BIT Area,
											BCMinorFrmHandle Frame);
/* runs a minor frame loaded into an Area */
__BUEXTERN BuError_t __BUDECL advBuBCRunMinor(BuConf_p BuConf,U16BIT Area,U16BIT Repeat);

/* creates a minor frame and returns a handle to the frame */
__BUEXTERN BCMinorFrmHandle __BUDECL advBuBCXMinorFrm(U32BIT MinorFrameTime,
												   U16BIT numofmsgs,
												   BCMsgHandle *MsgHandles);
/* free minor frame and associated messages (optional) */
__BUEXTERN void __BUDECL advBuBCFreeMinor(BuConf_p BuConf,BCMinorFrmHandle frm,
									   U16BIT FreeMsgs);
/* calculated number of data words from cmdword and ctrlword */
__BUEXTERN U16BIT __BUDECL advBuBCCalcDataSize(U16BIT CtrlWord,U16BIT CmdWord1);

/* calculates size of bc message */
__BUEXTERN U16BIT __BUDECL advBuBCCalcSize(U16BIT CtrlWord,U16BIT CmdWord1);

/* creates a bc message and returns handle to the message */
__BUEXTERN BCMsgHandle __BUDECL advBuBCXMsg(BuConf_p BuConf,
											U16BIT CntrlWord,
										    U16BIT CmdWord1,
										    U16BIT CmdWord2,
										    U16BIT *data,
										    U16BIT gaptime,
										    U8BIT condition);
/* converts bc bsw errors into string */
__BUEXTERN char* __BUDECL advBuBCBSWErrorStr(U16BIT value);

/* reads a specific message number from a frame and returns */
__BUEXTERN BuError_t __BUDECL advBuBCReadMsgNum(BuConf_p BuConf,
												BCMinorFrmHandle frm,
											    U16BIT msgnum,
											    MsgType *readmsg);

/* writes data to a specific message number from a frame */
__BUEXTERN BuError_t __BUDECL advBuBCWriteMsgNum(BuConf_p BuConf,
												 BCMinorFrmHandle frm,
											     U16BIT msgnum,
											     MsgType *writemsg);

/* opens bus controller mode */
__BUEXTERN BuError_t __BUDECL advBuBCOpen(BuConf_p BuConf);

/* closes bus controller mode and free's data */
__BUEXTERN BuError_t __BUDECL advBuBCClose(BuConf_p BuConf);

/* integrated routine to send data to an RT */
__BUEXTERN BuError_t __BUDECL advBuBCSendData(BuConf_p BuConf,
											  U8BIT channel,
										      U8BIT rt,
										      U8BIT sa,
										      U16BIT *Data,
										      U8BIT length);

/* integrated routine to get data from an RT */
__BUEXTERN BuError_t __BUDECL advBuBCGetData(BuConf_p BuConf,
											 U8BIT channel,
										     U8BIT rt,
										     U8BIT sa,
										     U16BIT *Data,
										     U8BIT length);
/* store minor frame to disk */
__BUEXTERN U16BIT __BUDECL advBuBCStoreMinor(BuConf_p BuConf,char *fn,BCMinorFrmHandle frm);

/* recall minor frame from disk */
__BUEXTERN U16BIT __BUDECL advBuBCRecallMinor(BuConf_p BuConf,char *fn,BCMinorFrmHandle *frm);

/* stores messages from frame to user stack */
__BUEXTERN BuError_t __BUDECL advBuBCReadStk(BuConf_p BuConf,
											 BCMinorFrmHandle frm,
										     U16BIT *Buffer,
										     U16BIT *Len,
										     U8BIT Mode);

/* pops message off user stack and returns message structure */
__BUEXTERN BuError_t __BUDECL advBuBCDecodeStk(BuConf_p BuConf,
											   U16BIT **StackPtr,
											   U16BIT *StackLen,
											   MsgType *msg);


/*----------------BC EXTENDED FUNCTIONALITY-----------------------*/

/* This functions sets up the BC command stacks [01-MAR-99]*/ 
__BUEXTERN BuError_t __BUDECL advBuBCSetSizeOfCmdStks(BuConf_p BuConf,U16BIT wSize);

/* creates a bc message and returns handle to the message */
__BUEXTERN BCMsgHandle __BUDECL advBuBCXMsgEx(BuConf_p BuConf,
											  U16BIT wCtrlWrd,
										      U16BIT wCmdWrd1,
										      U16BIT wCmdWrd2,
										      U16BIT *pData,
										      U16BIT wGapTime,
										      U16BIT wCondition);

__BUEXTERN BuError_t __BUDECL advBuBCXMajorFrm(BuConf_p BuConf,U16BIT wNumOfMinorFrms,
								 BCMinorFrmHandle *pMinorFrms);

__BUEXTERN BuError_t __BUDECL advBuBCRunMajor(BuConf_p BuConf,S32BIT lMjrFrmCount,S16BIT wMode);
__BUEXTERN BuError_t __BUDECL advBuBCHaltMajor(BuConf_p BuConf);


/* Information functions for use while major frames are running */
__BUEXTERN BuError_t __BUDECL advBuBCGetMajorFrmsRemaining(BuConf_p BuConf,S32BIT *pMjrFrmsRemain);
__BUEXTERN BuError_t __BUDECL advBuBCGetMinorFrmIndex(BuConf_p BuConf,U32BIT *pMnrFrmIndex);


__BUEXTERN BuError_t __BUDECL advBuBCReadMsgNumEx(BuConf_p BuConf,
												  BCMinorFrmHandle hMinorFrm,
											      U16BIT wMsgNum,
											      MsgType *pMsg);

__BUEXTERN BuError_t __BUDECL advBuBCReplaceMsg(BCMinorFrmHandle hMinorFrm,
											 U16BIT wMsgNum,
											 BCMsgHandle hMsg);

__BUEXTERN BCMsgHandle __BUDECL advBuBCGetNopMsg(BuConf_p BuConf);

__BUEXTERN BuError_t __BUDECL advBuBCSetUsrIrqMask(BuConf_p BuConf,U16BIT bEnable,U16BIT wMask);
__BUEXTERN BuError_t __BUDECL advBuBCSetSysIrqMask(BuConf_p BuConf,U16BIT bEnable,U16BIT wMask);

/* BUF FUNCTION PROTOTYPES ------------------------------------------------*/

/* allocated BUF module private data */
__BUEXTERN void __BUDECL advBuBufOpen(BuConf_p BuConf);

/* frees BUF module private data */
__BUEXTERN void __BUDECL advBuBufClose(BuConf_p BuConf);

/* creates the desired number of BufferSize word read write buffers */
__BUEXTERN U16BIT __BUDECL advBuCreateRWBuffers(BuConf_p BuConf,
												U16BIT NumBuffers,
											    U32BIT BufferSize,
											    U32BIT BufferFull);

/* frees all the buffers and associated structures */
__BUEXTERN void __BUDECL advBuFreeRWBuffers(BuConf_p BuConf);

/* determines if the current write buffer is full */
__BUEXTERN U16BIT __BUDECL advBuWriteBufFull(BuConf_p BuConf);

/* returns the pointer to the next avaiable write buffer slot */
__BUEXTERN U16BIT* __BUDECL advBuGetWriteBufPtr(BuConf_p BuConf);

/* adds count words to the pointers and counters of write buffer */
__BUEXTERN U16BIT __BUDECL advBuUpdateWriteBuf(BuConf_p BuConf,U32BIT count);

/* assigns Ptr the next avaiable slot to read data from */
__BUEXTERN U16BIT __BUDECL advBuGetReadBufPtr(BuConf_p BuConf,
											  U16BIT **Ptr,
										      U16BIT *Words);

/* resets the read buffer to a clear condition avaiable for writing */
__BUEXTERN U16BIT __BUDECL advBuResetReadBuf(BuConf_p BuConf);


/* CONF FUNCTION PROTOTYPES -----------------------------------------------*/

/* sets the logical device number in a BuConf structure [05-AUG-98] */
__BUEXTERN BuError_t __BUDECL advBuSetCardNum(BuConf_p Conf, U8BIT card_num);

/* retrieves string value from keyword in a .cfg file [30-JULY-97] */
__BUEXTERN char* __BUDECL advBuGetValue(FILE *in, const char *str);

/* dynamically (de)allocates storage for a Conf structure */
__BUEXTERN BuConf_p __BUDECL advBuAllocConf(void);
__BUEXTERN BuError_t __BUDECL advBuDeallocConf(BuConf_p Conf);

/* loads .cfg file into Conf structure and executes BuOpen(Conf) */
/* modified for WIN32, replaced 'far' with __BUFAR */
__BUEXTERN BuError_t __BUDECL advBuInit( char *fn,BuConf_t __BUFAR * Conf);

/* stores active BuConf structure in text file */
__BUEXTERN BuError_t __BUDECL advBuStoreConf(BuConf_p BuConf, char *fn);

/* enables/disables ACE memory (for 65529 card)*/
__BUEXTERN BuError_t __BUDECL advBuRam29(BuConf_p BuConf, U8BIT Sel);

/* enables/disables ACE memory (for 65539 card)*/
__BUEXTERN BuError_t __BUDECL advBuRam39(BuConf_p BuConf, U8BIT Sel);

/* enables/disables ACE memory (for 65568 card)*/
__BUEXTERN BuError_t __BUDECL advBuRam68(BuConf_p BuConf, U8BIT Sel);

/* enables/disables ACE memory (for 65580 card)*/
__BUEXTERN BuError_t __BUDECL advBuRam80(BuConf_p BuConf, U8BIT Sel);

/* opens library and makes Conf active */
/* modified for WIN32, replaced 'far' with __BUFAR */
/* modified for Linux, added Linux path */
__BUEXTERN BuError_t __BUDECL advBuOpen( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL advBuOpenWNT( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL advBuOpenW95( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL advBuOpenLinux( BuConf_t __BUFAR * Conf);

/* returns text revision info on ACE library */
__BUEXTERN char* __BUDECL advBuRev( void);

__BUEXTERN BuError_t __BUDECL advBuDumpMem(BuConf_p BuConf,char *fn);

/* returns number CORE revision info on ACE library */
__BUEXTERN void __BUDECL advBuGetCoreRev( U8BIT *Major,U8BIT *Minor,U8BIT *XMinor);

/* returns number revision info on ACE library */
__BUEXTERN void __BUDECL advBuGetRev( U8BIT *Major,U8BIT *Minor,U8BIT *XMinor,U32BIT *Code);

/* Sets the channel number for PC/104 cards */
__BUEXTERN BuError_t __BUDECL advBuSetChannel(BuConf_p BuConf,U16BIT wChannel);

/* Sets the SouthBridge mode for PC/104 cards */
__BUEXTERN BuError_t __BUDECL advBuSouthBridge(BuConf_p BuConf,U8BIT Sel);

/* set the current hub address of BU-6558xCx device */
__BUEXTERN BuError_t __BUDECL advBuSetEbrHubAddress(BuConf_p BuConf, U8BIT HubPort);

/* CANBus to EBR loop back test */
__BUEXTERN BuError_t __BUDECL advBuCanToEBRLoopTest(BuConf_p BuConf, U8BIT HubPort, U16BIT DataWrite, U16BIT *DataRead);

/* Parametric Checking enable */
__BUEXTERN BuError_t __BUDECL advBuParametricChecking(BuConf_p BuConf, U8BIT Sel);

/* closes library at active Conf */
__BUEXTERN BuError_t __BUDECL advBuClose(BuConf_p BuConf);
__BUEXTERN BuError_t __BUDECL advBuCloseW95(BuConf_p BuConf);
__BUEXTERN BuError_t __BUDECL advBuCloseWNT(BuConf_p BuConf);
__BUEXTERN BuError_t __BUDECL advBuCloseLinux(BuConf_p BuConf);

#ifdef _WIN32
__BUEXTERN U32BIT __BUDECL advWrite39IO_95(BuConf_p BuConf,U16BIT wCtrl);
__BUEXTERN U32BIT __BUDECL advWrite39IO_NT(BuConf_p BuConf,U16BIT wCtrl);

/* opens library and makes Conf active  for Win32 bit*/
__BUEXTERN BuError_t __BUDECL advBuOpen32(BuConf_t *Conf);

/* determines whether the Win32 platform is Windows 95 or NT */
S8BIT __BUDECL advBuWin32Platform(void); /* Void for CVI support*/

__BUEXTERN U16BIT __BUDECL advBuGetPCCardStatus(U8BIT card_num);
#endif

#ifdef _ACELINUX
/*__BUEXTERN U32BIT __BUDECL advWrite39IO_95(BuConf_p BuConf,U16BIT wCtrl);
__BUEXTERN U32BIT __BUDECL advWrite39IO_NT(BuConf_p BuConf,U16BIT wCtrl);*/

/* opens library and makes Conf active  for Win32 bit*/
__BUEXTERN BuError_t __BUDECL advBuOpenLinux(BuConf_t *Conf);

/*__BUEXTERN U16BIT __BUDECL advBuGetPCCardStatus(U8BIT card_num);*/
#endif

/* ERROR FUNCTION PROTOTYPES ----------------------------------------------*/

/* convert error value into string */
__BUEXTERN char* __BUDECL advBuErrorStr(BuError_t BuError);

/* INTERFACE FUNCTION PROTOTYPES ------------------------------------------*/

/* write register,(offset) words */
__BUEXTERN BuError_t __BUDECL advBuWriteReg(BuConf_p BuConf, U16BIT offset,U16BIT data);

/* read register,(offset) words */
__BUEXTERN U16BIT __BUDECL advBuReadReg(BuConf_p BuConf, U16BIT offset);

/* read block from memory into word array Ptr[],(offset,length) words */
/* modified for WIN32, replaced far with __BUFAR                      */   
__BUEXTERN BuError_t __BUDECL advBuBlockRead(BuConf_p BuConf,
											 U16BIT offset,
                                             U16BIT __BUFAR *Ptr,
                                             U16BIT length);

/* read block to memory from word array Ptr[],(offset,length) words */
/* modified for WIN32, replaced far with __BUFAR                    */   
__BUEXTERN BuError_t __BUDECL advBuBlockWrite(BuConf_p BuConf,
											  U16BIT offset,
                                              U16BIT __BUFAR *Ptr,
                                              U16BIT length);

/* fill an area of memory with data,(offset,length) words */
__BUEXTERN BuError_t __BUDECL advBuBlockFill(BuConf_p BuConf,
											 U16BIT offset,
                                             U16BIT data,
                                             U16BIT length);

/* memory copy routine re-written to support paged memory */
BuError_t __BUDECL advBuMemCpy(BuConf_p BuConf, U16BIT __BUFAR* src,U16BIT __BUFAR* dest,U16BIT size);

#ifdef __BUDEBUG_FUNCTIONS
/* write ram,(offset) words */
__BUEXTERN BuError_t __BUDECL advBuWriteRam(BuConf_p BuConf, U16BIT offset,U16BIT data);

/* read ram,(offset) words */
__BUEXTERN U16BIT __BUDECL advBuReadRam(BuConf_p BuConf, U16BIT offset);

#else
 #ifdef __BUDEBUG_ERROR_CHECKING
 /* #define BuWriteRam(ofs,dat) if(BuConf)*(BuConf->BuBaseMem+ofs)=dat
  * fixed 01-SEP-1995 problem with above BuWriteRam() was picking up
  * else statments when macro was before an else. fixed by adding an
  * else at the end
  */
 #define advBuWriteRam(BuConf,ofs,dat) if(BuConf)*(BuConf->BuBaseMem+ofs)=dat; else
 #define advBuReadRam(BuConf,ofs) ((BuConf)?(*(BuConf->BuBaseMem+ofs)):0)
 
 #else
 #define advBuWriteRam(BuConf,ofs,dat) *(BuConf->BuBaseMem+ofs)=dat
 #define advBuReadRam(BuCOnf,ofs) *(BuConf->BuBaseMem+ofs)
 #endif
#endif

/* INTERRUPT FUNCTION PROTOTYPES  -----------------------------------------*/

/* Install interrupt service routine for a given compiler*/
#if defined(__TURBOC__) & !defined(_WIN32)     /* Borland Turbo C */
	BuError_t advBuInstallIsr(BuConf_p BuConf,void interrupt(*Isr)(void));
#endif

#if defined(_MSC_VER) & ! defined(_WIN32)       /* Microsoft C */
	BuError_t advBuInstallIsr(BuConf_p BuConf,void(_interrupt __BUFAR *Isr)(void)); 
#endif

#if defined(_WIN32)								/* Win32 compiler */
 #ifdef __BU65528
	BuError_t advBuInstallIsr(BuConf_p BuConf,BU28_CardInfo_p);   /*65528 support*/
 #else
	BuError_t advBuInstallIsr(BuConf_p BuConf);				/*CVI support*/
 #endif
#endif

#if (defined(_ACELINUX)) || (defined(VX_WORKS))
	BuError_t advBuInstallIsr(BuConf_p BuConf);
	void advBuUninstallIsr(BuConf_p BuConf);
#else
/* Un-install interrupt service routine */
void advBuUninstallIsr(BuConf_p BuConf);
#endif

/* Each IRQ level has a unique service routine vector   */
/* BuSystemIsr is called with the irq level information */
#ifndef _WIN32  /* added for WIN32 */

#ifdef __TURBOC__
void interrupt far advBuSystemIsr_Irq3(void);
void interrupt far advBuSystemIsr_Irq4(void);
void interrupt far advBuSystemIsr_Irq5(void);
void interrupt far advBuSystemIsr_Irq6(void);
void interrupt far advBuSystemIsr_Irq7(void);
void interrupt far advBuSystemIsr_Irq8(void);
void interrupt far advBuSystemIsr_Irq9(void);
void interrupt far advBuSystemIsr_Irq10(void);
void interrupt far advBuSystemIsr_Irq11(void);
void interrupt far advBuSystemIsr_Irq12(void);
void interrupt far advBuSystemIsr_Irq13(void);
void interrupt far advBuSystemIsr_Irq14(void);
void interrupt far advBuSystemIsr_Irq15(void);
#endif

#ifdef _MSC_VER
/* modified for WIN32 replaced far with __BUFAR   */
void _interrupt __BUFAR advBuSystemIsr_Irq3(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq4(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq5(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq6(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq7(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq8(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq9(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq10(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq11(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq12(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq13(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq14(BuConf_p BuConf);
void _interrupt __BUFAR advBuSystemIsr_Irq15(BuConf_p BuConf);
#endif

#endif  


/* system interrupt handler */
#ifndef _WIN32
void advBuSystemIsr(BuConf_p BuConf,int IrqLevel);
#endif


/* installs a hook to a system isr */
BuError_t advBuInstallSysHandler(BuConf_p BuConf,U16BIT(*Handler)(BuConf_p BuConf,U16BIT status));

/* explicitly calls the Usr defined handler */
__BUEXTERN void __BUDECL advBuTestUsrHandler(BuConf_p BuConf, U16BIT value);

/* installs a hook to a user isr */
/* modified for WIN32            */ 
#ifdef _WIN32
__BUEXTERN BuError_t __BUDECL advBuInstallUsrHandler(BuConf_p BuConf, void (__BUFAR __BUDECL *Handler)(BuConf_p BuConf,U16BIT status));  
#else 
__BUEXTERN BuError_t __BUDECL advBuInstallUsrHandler(BuConf_p BuConf, void (__BUFAR *Handler)(BuConf_p BuConf,U16BIT status));  
#endif

/* installs a hook to a usr isr for can interrupts */
#ifdef _WIN32
__BUEXTERN BuError_t __BUDECL advBuInstallCanHandler(BuConf_p BuConf, void(__BUFAR __BUDECL *Handler)(BuConf_p BuConf,U16BIT status));
#else
__BUEXTERN BuError_t __BUDECL advBuInstallCanHandler(BuConf_p BuConf, void(__BUFAR *Handler)(BuConf_p BuConf,U16BIT status));
#endif

/* Writes RT Address to the Carriage */
__BUEXTERN BuError_t __BUDECL advBuBCWriteCanAddr(BuConf_p BuConf,U16BIT wRTAddr, U16BIT wCANBus);

/* Resets the CANbus controller and acquires the serial RT address */
__BUEXTERN BuError_t __BUDECL advBuAcquireCanRTAddr(BuConf_p BuConf, U16BIT *wRTAddr, U16BIT wCANBus);

/* sets user defined pointer for user handler */
__BUEXTERN BuError_t __BUDECL advBuSetUsrPtr(BuConf_p BuConf,void *UsrPtr);

/* gets user defined pointer for user handler */
__BUEXTERN void* __BUDECL advBuGetUsrPtr(BuConf_p BuConf);


#ifdef __BUDEBUG_FUNCTIONS
/* sets auto clear irq on read of irq status register */
__BUEXTERN BuError_t __BUDECL advBuIrqAutoClear(BuConf_p BuConf, U8BIT Sel);

/* sets irq line for level or pulse irq signal */
__BUEXTERN BuError_t __BUDECL advBuIrqType(BuConf_p BuConf, U8BIT Sel);

/* allows the enable of interrupt conditions */
__BUEXTERN BuError_t __BUDECL advBuIrqEnable(BuConf_p BuConf, U16BIT Mask);

/* allows the disable of interrupt conditions */
__BUEXTERN BuError_t __BUDECL advBuIrqDisable(BuConf_p BuConf, U16BIT Mask);

/* get interrupt register status */
__BUEXTERN U16BIT __BUDECL advBuGetIrqStatus( BuConf_p BuConf);

/* resets ace INT* output to a logic 1 */
__BUEXTERN BuError_t __BUDECL advBuIrqReset( BuConf_p BuConf);

#else
#define advBuIrqAutoClear(BuConf,sel) advBuWriteReg(BuConf,CONFIG_2,\
        ((advBuReadReg(BuConf,CONFIG_2)&0xffef)|((sel)?0x0010:0)))
#define advBuIrqType(BuConf,sel) advBuWriteReg(BuConf,CONFIG_2,\
        ((advBuReadReg(BuConf,CONFIG_2)&0xfff7)|((sel)?0x0008:0)))
#define advBuIrqEnable(BuConf,msk) advBuWriteReg(BuConf,INTRPT_MASK,\
        (advBuReadReg(BuConf,INTRPT_MASK)|msk))
#define advBuIrqDisable(BuConf,msk) advBuWriteReg(BuConf,INTRPT_MASK,\
        (advBuReadReg(BuConf,INTRPT_MASK)&(~msk)))
#define advBuGetIrqStatus(BuConf) advBuReadReg(BuConf,INTRPT_STATUS)
#define advBuIrqReset(BuConf) advBuWriteReg(BuConf,CONTROL,0x0004)

#endif

/* MONITOR FUNCTION PROTOTYPES --------------------------------------------*/

#ifdef __BUDEBUG_FUNCTIONS

/* returns the active stack */
__BUEXTERN U16BIT __BUDECL advBuMTGetActiveStk(BuConf_p BuConf);

/* returns the inactive stack */
__BUEXTERN U16BIT __BUDECL advBuMTGetInactiveStk(BuConf_p BuConf);

/* checks to see if a stack is inactive */
__BUEXTERN U16BIT __BUDECL advBuMTIsActiveStk(BuConf_p BuConf, U16BIT stk);

/* returns address of lookup word associated with rt, tr, and sa */
__BUEXTERN U16BIT __BUDECL advBuMTLookup( U16BIT rt,U16BIT tr, U16BIT sa);

/* begins capture of messages but does not initialize stack ptrs */
__BUEXTERN BuError_t __BUDECL advBuMTContinue(BuConf_p BuConf);

/* puts monitor in idle mode */
__BUEXTERN BuError_t __BUDECL advBuMTHalt(BuConf_p BuConf);

#else

#define advBuMTGetActiveStk(BuConf) (advBuReadReg(BuConf,CONFIG_1)&0x2000)
#define advBuMTGetInactiveStk(BuConf) ((advBuReadReg(BuConf,CONFIG_1)&0x2000)^0x2000)
#define advBuMTIsActiveStk(BuConf,stk) ((advBuReadReg(BuConf,CONFIG_1)&0x2000)==stk)
#define advBuMTLookup(rt,tr,sa) ((rt*4)+(tr*2)+((sa&0x10)>>4)+SELMTLOOKUP)
#define advBuMTContinue(BuConf) (advBuWriteReg(BuConf,CONTROL,MT_START))
#define advBuMTHalt(BuConf) (advBuWriteReg(BuConf,CONTROL,MT_HALT))

#endif

/* sets the active stack */
__BUEXTERN BuError_t __BUDECL advBuMTSetActiveStk(BuConf_p BuConf, U16BIT stk);

/* swaps inactive and active stacks */
__BUEXTERN BuError_t __BUDECL advBuMTSwapStk(BuConf_p BuConf);

/* rt, tr, sa combination to capture */
__BUEXTERN BuError_t __BUDECL advBuMTEnableRT(BuConf_p BuConf, U16BIT rt,
                                            U16BIT tr,
                                            U16BIT sa);

/* rt, tr, sa combination to ignore */
__BUEXTERN BuError_t __BUDECL advBuMTDisableRT(BuConf_p BuConf, U16BIT rt,
                                             U16BIT tr,
                                             U16BIT sa);

/* writes array of 16x128 selective capture table from a buffer */
__BUEXTERN BuError_t __BUDECL advBuMTWriteTable(BuConf_p BuConf, U16BIT *TblPtr);

/* reads array of 16x128 selective capture table to a buffer */
__BUEXTERN BuError_t __BUDECL advBuMTReadTable(BuConf_p BuConf, U16BIT *TblPtr);

/* reads message from memory address */
__BUEXTERN BuError_t __BUDECL advBuMTReadMsgAtAddr(BuConf_p BuConf, MsgType *msg,U16BIT Cmd);

/* converts mt bsw errors into string */
__BUEXTERN char* __BUDECL advBuMTBSWErrorStr( U16BIT value);

/* returns message from top of stack if available */
__BUEXTERN U16BIT __BUDECL advBuMTReadMsg(BuConf_p BuConf, MsgType *msg);

/* defines a monitor stack's starting locations */
__BUEXTERN BuError_t __BUDECL advBuMTDefStk(BuConf_p BuConf, U16BIT stack,
                                          U16BIT CmdStart,
                                          U16BIT CmdInitial,
                                          U16BIT DataStart,
                                          U16BIT DataInitial);

/* defines buffering mode and size of the stacks */
__BUEXTERN BuError_t __BUDECL advBuMTStkType(BuConf_p BuConf, U16BIT Type,
                                           U16BIT CmdSize,
                                           U16BIT DataSize);

/* initializes stack ptrs and begins capture of messages */
__BUEXTERN BuError_t __BUDECL advBuMTRun(BuConf_p BuConf);

/* must be called at beginning to initialize monitor mode */
__BUEXTERN BuError_t __BUDECL advBuMTOpen(BuConf_p BuConf);

/* must be called at end to clear up mode and free data */
__BUEXTERN BuError_t __BUDECL advBuMTClose(BuConf_p BuConf);

/* swaps active areas and reads stack into buffer */
__BUEXTERN BuError_t __BUDECL advBuMTReadStk(BuConf_p BuConf, U16BIT *msgarray,
                                           U16BIT *Length);

/* pops message off buffer and returns message structure */
__BUEXTERN BuError_t __BUDECL advBuMTDecodeStk(BuConf_p BuConf, U16BIT **StackPtr,
                                             U16BIT *StackLen,
                                             MsgType *msg);

/* swaps active areas and reads stack into buffer */
__BUEXTERN BuError_t __BUDECL advBuMTReadStkTrig(BuConf_p BuConf,U16BIT *msgarray,U16BIT *Length);

/* sets the strategy for handling MT Cmd Stack Overflows */
__BUEXTERN BuError_t __BUDECL advBuMTSetOvrFlwStrategy(BuConf_p BuConf,U16BIT strategy);


/* RT FUNCTION PROTOTYPES -------------------------------------------------*/

/* allows internal or external BIT word */
__BUEXTERN BuError_t __BUDECL advBuRTBITWord(BuConf_p BuConf,U16BIT sel);

/* sets RT address */
__BUEXTERN BuError_t __BUDECL advBuRTAddress(BuConf_p BuConf,U8BIT RTaddr);

/* defines subaddress using sacw structure */
__BUEXTERN BuError_t __BUDECL advBuRTDefSA(BuConf_p BuConf,U16BIT SubAddr,SubAddrCtrlWrd* Sacw);

/* maps a data block to a subaddress */
__BUEXTERN U16BIT __BUDECL advBuRTMapBlk(BuConf_p BuConf,U16BIT SubAddr,U16BIT t_r,RTBlkHandle BlockHdl, U16BIT Offset);

/* starts combined rt/monitor mode */
__BUEXTERN BuError_t __BUDECL advBuRTMONRun(BuConf_p BuConf);

/* starts rt mode */
__BUEXTERN BuError_t __BUDECL advBuRTRun(BuConf_p BuConf);

/* allocates memory block for use with subaddress */
__BUEXTERN RTBlkHandle __BUDECL advBuRTAllocBlk(BuConf_p BuConf,U8BIT BlkType);

/* opens rt mode of operation */
__BUEXTERN BuError_t __BUDECL advBuRTOpen(BuConf_p BuConf);

/* closes rt mode of operation */
__BUEXTERN BuError_t __BUDECL advBuRTClose(BuConf_p BuConf);

/* free's memory block in ACE */
__BUEXTERN BuError_t __BUDECL advBuRTFreeBlk(BuConf_p BuConf,RTBlkHandle BlockHdl);

/* reads message from descriptor stack */
__BUEXTERN U16BIT __BUDECL advBuRTReadMsg(BuConf_p BuConf,U16BIT MessageNum, MsgType *Message);

/* reads inactive buffer in RT rx double buffered mode */
/* modified for WIN32 replaced far with __BUFAR        */ 
__BUEXTERN U16BIT __BUDECL advBuRTReadInactive(BuConf_p BuConf,RTBlkHandle BlockHdl, U16BIT __BUFAR *Buffer);

/* defines message as legal */
__BUEXTERN BuError_t __BUDECL advBuRTDefMsgLegal(BuConf_p BuConf,U16BIT MessType,U16BIT subaddr,U16BIT wc);

/* defines message as illegal */
__BUEXTERN BuError_t __BUDECL advBuRTDefMsgIllegal(BuConf_p BuConf, U16BIT MessType,U16BIT subaddr,U16BIT wc);

__BUEXTERN BuError_t __BUDECL advBuRTMONOpen(BuConf_p BuConf);
__BUEXTERN BuError_t __BUDECL advBuRTMONClose(BuConf_p BuConf);

/* Discrete IO FUNCTION PROTOTYPES -------------------------------------------------*/

__BUEXTERN BuError_t __BUDECL advBuDioRead(BuConf_p BuConf, U16BIT bPort, U16BIT *wCmd);
__BUEXTERN BuError_t __BUDECL advBuDioWrite(BuConf_p BuConf, U16BIT bPort, U16BIT wCmd);
__BUEXTERN BuError_t __BUDECL advBuDioDirBank(BuConf_p BuConf, U16BIT *wDirection, U8BIT bMask, U16BIT bPort);
__BUEXTERN BuError_t __BUDECL advBuSetDioFunction(BuConf_p BuConf, U16BIT wFunc);
__BUEXTERN BuError_t __BUDECL advBuGetDioFunction(BuConf_p BuConf, U16BIT *wFunc);
__BUEXTERN BuError_t __BUDECL advBuSetDioHubPort(BuConf_p BuConf, U16BIT wPort);
__BUEXTERN BuError_t __BUDECL advBuGetDioHubPort(BuConf_p BuConf, U16BIT *wPort);

#ifdef __BUDEBUG_FUNCTIONS

/* enables/disables alterate status word */
__BUEXTERN BuError_t __BUDECL advBuRTAltStatusEna(BuConf_p BuConf,U16BIT sel);

/* enables/disables busy bit lookup table */
__BUEXTERN BuError_t __BUDECL advBuRTBusyTableEna(BuConf_p BuConf,U16BIT sel);

/* allows selection of external or internal bit word */
__BUEXTERN BuError_t __BUDECL advBuRTExtBITWord(BuConf_p BuConf,U16BIT sel);

/* allows the transmission of the BUSY bit word */
__BUEXTERN BuError_t __BUDECL advBuRTBitInhibit(BuConf_p BuConf,U16BIT sel);

/* enables/disables use of RT address 31 as a broadcast address */
__BUEXTERN BuError_t __BUDECL advBuRTBrdcst(BuConf_p BuConf,U16BIT sel);

/* configures ACE for advanced/normal mode code handling */
__BUEXTERN BuError_t __BUDECL advBuRTModeCode(BuConf_p BuConf,U16BIT Enhanced,U16BIT OverRideEnable);

/* allows for the normal or alternate status word control
   removed [01-SEP-1995] since functionality is duplicated in
   BuRTAltStatusEna()
__BUEXTERN BuError_t __BUDECL BuRTAltStat(U16BIT sel); */

/* allows for valid response with ME bit/no data */
__BUEXTERN BuError_t __BUDECL advBuRTMsgErrValid(BuConf_p BuConf,U16BIT sel);

/* allows for valid response with busy bit/no data */
__BUEXTERN BuError_t __BUDECL advBuRTBusyValid(BuConf_p BuConf,U16BIT sel);

/* determines if an illegal command is stored or discarded */
__BUEXTERN BuError_t __BUDECL advBuRTIllegal(BuConf_p BuConf,U16BIT sel);

/* determines who controls the RTFLAG status bit */
__BUEXTERN BuError_t __BUDECL advBuRTFlagWrap(BuConf_p BuConf,U16BIT sel);

/* controls the terminal flag bit in status words */
__BUEXTERN BuError_t __BUDECL advBuRTFlag(BuConf_p BuConf,U16BIT sel);

/* controls the subsytem request flag in status words */
__BUEXTERN BuError_t __BUDECL advBuRTSetSSflag(BuConf_p BuConf,U16BIT sel);

/* controls the service request bit in status words */
__BUEXTERN BuError_t __BUDECL advBuRTSetSvcReq(BuConf_p BuConf,U16BIT sel);

/* controls the busy bit in status words */
__BUEXTERN BuError_t __BUDECL advBuRTSetBusy(BuConf_p BuConf,U16BIT sel);

/* contrls the DBA bit in status words */
__BUEXTERN BuError_t __BUDECL advBuRTSetDba(BuConf_p BuConf,U16BIT sel);

/* controls rt mode enhanced memory mangament */
__BUEXTERN BuError_t __BUDECL advBuRTEnhMM(BuConf_p BuConf,U16BIT sel);

/* controls enhanced 1553 mode code handling */
__BUEXTERN BuError_t __BUDECL advBuRTEnhModeCode(BuConf_p BuConf,U16BIT sel);

/* controls the separate of broadcast data */
__BUEXTERN BuError_t __BUDECL advBuRTSeparateBcst(BuConf_p BuConf,U16BIT sel);

/* reads enhanced mode code data */
__BUEXTERN U16BIT __BUDECL advBuRTReadEnhMCData(BuConf_p BuConf,U16BIT addr);

/* writes enhanced mode code data */
__BUEXTERN BuError_t __BUDECL advBuRTWriteEnhMCData(BuConf_p BuConf,U16BIT addr,U16BIT data);

/* enables selected mode code interrupt */
__BUEXTERN BuError_t __BUDECL advBuRTModeIrqEnable(BuConf_p BuConf, U8BIT broadcast,U8BIT t_r,
						 U8BIT data,U16BIT map);

/* disables selected mode code interrupt */
__BUEXTERN BuError_t __BUDECL advBuRTModeIrqDisable(BuConf_p BuConf, U8BIT broadcast,U8BIT t_r,
						  U8BIT data,U16BIT map);

/* reads internal or external BIT word */
__BUEXTERN U16BIT __BUDECL advBuRTBITRead(BuConf_p BuConf);

/* writes external BIT word */
__BUEXTERN BuError_t __BUDECL advBuRTExtBITWrite(BuConf_p BuConf, U16BIT map);

/* asserts busy bit in status word on subaddress basis */
__BUEXTERN BuError_t __BUDECL advBuRTBusyBitEnable(BuConf_p BuConf, U8BIT broadcast,U8BIT t_r,U8BIT sa);

/* disables assertion of busy bit in status word on subaddress basis */
__BUEXTERN BuError_t __BUDECL advBuRTBusyBitDisable(BuConf_p BuConf, U8BIT broadcast,U8BIT t_r,U8BIT sa);

/* writes alternate status word to ACE */
__BUEXTERN BuError_t __BUDECL advBuRTAltStatusWrite(BuConf_p BuConf, U16BIT map);

/* reads alternate status word from ACE */
__BUEXTERN U16BIT __BUDECL advBuRTAltStatusRead(BuConf_p BuConf);

/* stops RT */
__BUEXTERN void __BUDECL advBuRTStop(BuConf_p BuConf);


#else

#define advBuRTAltStatusEna(BuConf,sel) advBuWriteReg(BuConf,CONFIG_3,advBuReadReg(BuConf,CONFIG_3)&(0xffdf|(sel<<5)))
#define advBuRTBusyTableEna(BuConf,sel) advBuWriteReg(BuConf,CONFIG_2,advBuReadReg(BuConf,CONFIG_2)&0xdfff|(sel<<13))

/* fixed [01-SEP-1995] changed name from BuRTBITExtWord to BuRTExtBITWord to
 * keep consistant with function call */
#define advBuRTExtBITWord(BuConf,sel) advBuWriteReg(BuConf,CONFIG_4,advBuReadReg(BuConf,CONFIG_4)&0x7fff|(sel<<15))

#define advBuRTBitInhibit(BuConf,sel) advBuWriteReg(BuConf,CONFIG_4,advBuReadReg(BuConf,CONFIG_4)&0xbfff|(sel<<14))
#define advBuRTBrdcst(BuConf,sel) advBuWriteReg(BuConf,CONFIG_5,advBuReadReg(BuConf,CONFIG_5)&0xff7f|(sel<<7))

/* fixed [01-SEP-1995] extra ) in macro */
#define advBuRTModeCode(BuConf,En,Ov) advBuWriteReg(BuConf,CONFIG_3,((advBuReadReg(BuConf,CONFIG_3)&0xffbe)|(En)|(Ov<<6)))

/* removed [01-SEP-1995] since functionality is duplicated in
   BuRTAltStatusEna()
#define BuRTAltStat(sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xffdf)|((sel)<<5)) */
#define advBuRTMsgErrValid(BuConf,sel) advBuWriteReg(BuConf,CONFIG_4,(advBuReadReg(BuConf,CONFIG_4)&0xffbf)|(sel<<6))
#define advBuRTBusyValid(BuConf,sel) advBuWriteReg(BuConf,CONFIG_4,(advBuReadReg(BuConf,CONFIG_4)&0xffdf)|(sel<<5))
#define advBuRTIllegal(BuConf,sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xffef)|(sel<<4))
#define advBuRTFlagWrap(BuConf,sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xfffb)|(sel<<2))
#define advBuRTFlag(BuConf,sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xFF7F)|((!sel)<<7))

#define advBuRTSetSSflag(BuConf,sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xFEFF)|((!sel)<<8))
#define advBuRTSetSvcReq(BuConf,sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xFDFF)|((!sel)<<9))

#define advBuRTSetBusy(BuConf,sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xFBFF)|((!sel)<<10))
#define advBuRTSetDba(BuConf,sel) advBuWriteReg(BuConf,CONFIG_1,(advBuReadReg(BuConf,CONFIG_1)&0xF7FF)|((!sel)<<11))
/* fixed warning - [01-JAN-1996] */
#define advBuRTEnhMM(BuConf,sel) advBuWriteReg(BuConf,CONFIG_2,(advBuReadReg(BuConf,CONFIG_2)&0xFFFD)|(sel<<1))
#define advBuRTEnhModeCode(BuConf,sel) advBuWriteReg(BuConf,CONFIG_3,(advBuReadReg(BuConf,CONFIG_3)&0xFFFE)|(sel))
#define advBuRTSeparateBcst(BuConf,sel) advBuWriteReg(BuConf,CONFIG_2,(advBuReadReg(BuConf,CONFIG_2)&0xFFFE)|(sel))
#define advBuRTReadEnhMCData(BuConf,addr) advBuReadRam(BuConf,addr)
#define advBuRTWriteEnhMCData(BuConf,addr,data) advBuWriteRam(BuConf,addr,data)

#define advBuRTModeIrqEnable(BuConf,br,t_r,da,mp)\
	advBuWriteRam(BuConf,(ENH_MODE_IRQ_TABLE|(br<<2)|(t_r<<1)|da),\
	(advBuReadRam(BuConf,ENH_MODE_IRQ_TABLE)|(br<<2)|(t_r<<1)|da|mp))

#define advBuRTModeIrqDisable(BuConf,br,t_r,da,mp)\
	advBuWriteRam(BuConf,(ENH_MODE_IRQ_TABLE|(br<<2)|(t_r<<1)|da),\
	(advBuReadRam(BuConf,ENH_MODE_IRQ_TABLE)|(br<<2)|(t_r<<1)|da&(~mp)))

#define advBuRTExtBITWrite(BuConf,map) advBuWriteRam(BuConf,RT_ENH_BIT_WORD_ADDR,map)
#define advBuRTBITRead(BuConf) advBuReadReg(BuConf,RT_BIT_WORD)

/* asserts busy bit in status word on subaddress basis */

#define advBuRTBusyBitEnable(BuConf,br,t_r,sa) advBuWriteRam(BuConf,(ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0)),(1<<sa))

/* disables assertion of busy bit in status word on subaddress basis */

#define advBuRTBusyBitDisable(BuConf,br,t_r,sa) advBuWriteRam(BuConf,(ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0)),advBuReadRam(BuConf,(ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0))&(~(1<<sa)))
	

#define advBuRTAltStatusWrite(BuConf,map) advBuWriteReg(BuConf,CONFIG_1,map&0x0ffe)
/*#define BuRTAltStatusRead() (BuReadReg(CONFIG_1)0x0ffe)
  modified [01-SEP-1995] */
#define advBuRTAltStatusRead(BuConf) (advBuReadReg(BuConf,CONFIG_1)&0x0ffe)
#define advBuRTStop(BuConf) advBuWriteReg(BuConf,CONFIG_1,advBuReadReg(BuConf,CONFIG_1)&0x3fff)

#endif


/* TEST FUNCTION PROTOTYPES -----------------------------------------------*/

/* basic true/false test for ACE */
__BUEXTERN U8BIT __BUDECL advBuValid(BuConf_p BuConf);

/* test ram of ACE */
__BUEXTERN void __BUDECL advBuRamTest(BuConf_p BuConf, U16BIT value,BuTest_t*BuTest);

/* test registers of ACE */
__BUEXTERN void __BUDECL advBuRegisterTest(BuConf_p BuConf, BuTest_t*BuTest);

/* test protocol hardware of ACE */
__BUEXTERN void __BUDECL advBuProtocolTest(BuConf_p BuConf, BuTest_t*BuTest);

/* test interrupt capibility of ACE */
__BUEXTERN void __BUDECL advBuIrqTest(BuConf_p BuConf, BuTest_t*BuTest);

/* test ram, registers, protocol, and interrupts of ACE */
__BUEXTERN void __BUDECL advBuSelfTest(BuConf_p BuConf, BuTest_t*BuTest);

/* test ram, registers, protocol, and interrupts of ACE  returns Pass/Fail
   and character string pointer to results string */
__BUEXTERN U16BIT __BUDECL advBu_selftest(BuConf_p BuConf, char *error_msg);

/* vector test for ace: takes in absolute file name of test vector file */
__BUEXTERN void __BUDECL advBuVectorTest(BuConf_p BuConf, char *filename,BuTest_t*BuTest);

/* convert test type into string */
__BUEXTERN char* __BUDECL advBuTestStr(U8BIT code);

__BUEXTERN BuError_t __BUDECL advBuLoopBackTest(BuConf_p BuConf, BuTest_t*BuTest);

#endif /*ADVSTDACE_H_INCLUDED */

