#ifndef RT_H_INCLUDED
#define RT_H_INCLUDED
 /* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * RT.H (REMOTE TERMINAL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for RT.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.1       01-JUN-1995    Converted rel 1.6 rt.c module
 *
 *                            added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros
 *
 *                            added BuRTMONOpen, BuRTMONClose, and
 *                            BuRTMONRun to support RT/MONITOR mode
 *
 *   2.2       01-SEP-1995    extra ) in BuRTModeCode macro was fixed
 *
 *                            changed #define name from BuRTBITExtWord to
 *                            BuRTExtBITWord to keep consistant with
 *                            function call name
 *
 *                            removed BuRTAltStat() since functionality
 *                            is duplicated in BuRTAltStatusEna()
 *
 *                            missing & in BuRTAltStatusRead() macro
 *
 *   2.3       01-JAN-1996    fixed warnings
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       07-JUNE-1996   replaced 'far' with __BUFAR in BuRTReadInactive;
 *                            __BUFAR is defined based upon whether the 
 *                            application is compiled as a 16bit or a WIN32
 *                            application
 *
 *   4.0       29-NOV-1997    none
 *
 *   5.3.1     11-APR-2003    none
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* alternate status word bitmap */
#define RT_AltSta_S00                           0X0002
#define RT_AltSta_S01                           0X0004
#define RT_AltSta_S02                           0X0008
#define RT_AltSta_S03                           0X0010
#define RT_AltSta_S04                           0X0020
#define RT_AltSta_S05                           0X0040
#define RT_AltSta_RTFLAG                        0X0080
#define RT_AltSta_SUBSYS_FLAG                   0X0100
#define RT_AltSta_SRVC_REQST                    0X0200
#define RT_AltSta_BUSY                          0X0400
#define RT_AltSta_DYN_BUS_CTRL                  0X0800

/* assigned mode codes */
#define RT_MODE_DYN_BUS_CTRL                    0X0000
#define RT_MODE_SYNCHRONIZE                     0X0001
#define RT_MODE_TX_STAT_WORD                    0X0002
#define RT_MODE_INIT_SELF_TST                   0X0003
#define RT_MODE_TXS_SHUTDN                      0X0004
#define RT_MODE_OVER_TXS_SHUTDN                 0X0005
#define RT_MODE_INH_TERM_FLAG                   0X0006
#define RT_MODE_OVER_INH_TERM_FLAG              0X0007
#define RT_MODE_RESET_REMOTE_TERM               0X0008
#define RT_MODE_TXS_VECTOR_WORD                 0X0010
#define RT_MODE_SYNCHRONIZE_DATA                0X0011
#define RT_MODE_TX_LAST_COMMAND                 0X0012
#define RT_MODE_TX_BIT_WORD                     0X0013
#define RT_MODE_SEL_TRANS_SHUTDN                0X0014
#define RT_MODE_OVER_SEL_TRANS_SHUTDN           0X0015

/* enhanced mode code data locations */
#define RtEmod_SYNC_WITH_DATA                   0X0111
#define RtEmod_SEL_TXM_SHUT                     0X0114
#define RtEmod_OVER_SEL_TXM_SHUT                0X0115
#define RtEmod_TRANSMIT_VECTOR_WORD             0X0120
#define RtEmod_TRANSMIT_LAST_COMMAND            0X0122
#define RtEmod_TRANSMIT_BIT_WORD                0X0123
#define RtEmod_BCST_SYNC_WITH_DATA              0X0131
#define RtEmod_BCST_SEL_TXM_SHUT                0X0134
#define RtEmod_BCST_OVER_SEL_TXM_SHUT           0X0135

/* rt bit word */
#define RtBitwd_CMD_WRD_CNTENTS_ERR             0X0001
#define RtBitwd_RT_RT_2ND_CMD_WD_ERR            0X0002
#define RtBitwd_RT_RT_NO_RESPONS_ERR            0X0004
#define RtBitwd_RT_RT_GP_SYNC_ADR_ER            0X0008
#define RtBitwd_PAR_MAN_ERR_WD_RXD              0X0010
#define RtBitwd_INCORRECT_SYNC_RXD              0X0020
#define RtBitwd_LOW_WORD_COUNT                  0X0040
#define RtBitwd_HIGH_WORD_COUNT                 0X0080
#define RtBitwd_CHANN_B_CHANN_A                 0X0100
#define RtBitwd_TERMINAL_FLAG_INHD              0X0200
#define RtBitwd_TXTTR_SHUTDOWN_A                0X0400
#define RtBitwd_TXTTR_SHUTDOWN_B                0X0800
#define RtBitwd_HANDSHAKE_FAILURE               0X1000
#define RtBitwd_LOOP_TEST_FAILURE_A             0X2000
#define RtBitwd_LOOP_TEST_FAILURE_B             0X4000
#define RtBitwd_TRANSMITTER_TIMEOUT             0X8000

/* used with sa illegalization tables */
#define ALL                    0xffff
#define TX_CMD                 0x0400
#define RX_CMD                 0x0000

/* rt mode constants */
#define LOOKUP_A               0x0140
#define LOOKUP_B               0x01C0
#define ILLEGALIZATION_TABLE   0x0300
#define STACK_A                0x0000
#define STACK_POINTER_A        0x0100
#define ENH_MODE_TABLE_START   0x0110
#define ENH_MODE_TABLE_END     0x013F
#define ENH_MODE_IRQ_TABLE     0x0108
#define LAST_MESSAGE           0xFFFF
#define RT_ENH_BIT_WORD_ADDR   0x0123
#define ENH_SA_BUSY_TABLE      0x0240

/* types of rt buffers */
#define SINGLE_MESSAGE         0
#define RTBUFFER128            1
#define RTBUFFER256            2
#define RTBUFFER512            3
#define RTBUFFER1024           4
#define RTBUFFER2048           5
#define RTBUFFER4096           6
#define RTBUFFER8192           7
#define DOUBLE_MESSAGE         8

/* buffer types */
#define RECEIVE                0
#define TRANSMIT               1
#define BROADCAST              2
#define BROADCASTTX            3 /*added for BuRTDefMsgLegal*/

/* rx buffer options */
#define SINGLEBUFFER           0
#define DOUBLEBUFFER           1

/* mode command interrupt lookup table entries */
#define RTMIRQ_DYNAMIC_BUS_CONTROL     0X0001
#define RTMIRQ_SYNCHRONIZE             0X0002
#define RTMIRQ_TRANSMIT_STATUS         0X0004
#define RTMIRQ_INITIATE_SELF_TEST      0X0008
#define RTMIRQ_TRANSMITTER_SHUTDOWN    0X0010
#define RTMIRQ_OVERRIDE_TX_SHUTDOWN    0X0020
#define RTMIRQ_INHIBIT_TERMINAL_FLAG   0X0041
#define RTMIRQ_OVERRIDE_INHIBIT_TF     0X0080
#define RTMIRQ_RESET_REMOTE_TERMINAL   0X0100

/* command word type definition */
typedef struct {
  unsigned	WordCount:5,
			SubAddr:5,
			TR:1,
			RTaddr:5;
} CmdWordType;


/* subaddress control word type definition */
typedef struct {
  unsigned BcstMm:3,
	   BcstBuffInt:1,
	   BcstEomInt:1,
	   RxMm:3,
	   RxBuffInt:1,
	   RxEomInt:1,
	   TxMm:3,
	   TxBuffInt:1,
	   TxEomInt:1,
	   RcvBufferType:1;
} SubAddrCtrlWrd;


/* union to convert from structures to 16 bit words */
typedef union {
	U16BIT         word;
	SubAddrCtrlWrd sacw;
	CmdWordType    cmd;
} RTWords;


/* rt uses dynamically allocated memory blocks */
typedef MemBlockHandle RTBlkHandle;
typedef MemBlockType RTBlkType;

/* private data structure for rt mode */
typedef struct RTStruct{
  U16BIT BuRTLastMsg;
} RTType, *RTPtr;


/* RT FUNCTION PROTOTYPES -------------------------------------------------*/

/* allows internal or external BIT word */
__BUEXTERN BuError_t __BUDECL BuRTBITWord(U16BIT sel);

/* sets RT address */
__BUEXTERN BuError_t __BUDECL BuRTAddress(U8BIT RTaddr);

/* converts sacw struct to word */
__BUEXTERN U16BIT __BUDECL sacw2word(SubAddrCtrlWrd*  Sacw);

/* converts word to sacw struct */
__BUEXTERN SubAddrCtrlWrd __BUDECL word2sacw (U16BIT word);

/* defines subaddress using sacw structure */
__BUEXTERN BuError_t __BUDECL BuRTDefSA(U16BIT SubAddr,SubAddrCtrlWrd* Sacw);

/* maps a data block to a subaddress */
__BUEXTERN U16BIT __BUDECL BuRTMapBlk(U16BIT SubAddr,U16BIT t_r,RTBlkHandle BlockHdl, U16BIT Offset);

/* starts combined rt/monitor mode */
__BUEXTERN BuError_t __BUDECL BuRTMONRun(void);

/* starts rt mode */
__BUEXTERN BuError_t __BUDECL BuRTRun(void);

/* allocates memory block for use with subaddress */
__BUEXTERN RTBlkHandle __BUDECL BuRTAllocBlk(U8BIT BlkType);

/* opens rt mode of operation */
__BUEXTERN BuError_t __BUDECL BuRTOpen(void);

/* closes rt mode of operation */
__BUEXTERN BuError_t __BUDECL BuRTClose(void);

/* free's memory block in ACE */
__BUEXTERN BuError_t __BUDECL BuRTFreeBlk(RTBlkHandle BlockHdl);

/* reads message from descriptor stack */
__BUEXTERN U16BIT __BUDECL BuRTReadMsg(U16BIT MessageNum, MsgType *Message);

/* reads inactive buffer in RT rx double buffered mode */
/* modified for WIN32 replaced far with __BUFAR        */ 
__BUEXTERN U16BIT __BUDECL BuRTReadInactive(RTBlkHandle BlockHdl, U16BIT __BUFAR *Buffer);

/* defines message as legal */
__BUEXTERN BuError_t __BUDECL BuRTDefMsgLegal(U16BIT MessType,U16BIT subaddr,U16BIT wc);

/* defines message as illegal */
__BUEXTERN BuError_t __BUDECL BuRTDefMsgIllegal( U16BIT MessType,U16BIT subaddr,U16BIT wc);

__BUEXTERN BuError_t __BUDECL BuRTMONOpen(void);
__BUEXTERN BuError_t __BUDECL BuRTMONClose(void);

#ifdef __BUDEBUG_FUNCTIONS

/* enables/disables alterate status word */
__BUEXTERN BuError_t __BUDECL BuRTAltStatusEna(U16BIT sel);

/* enables/disables busy bit lookup table */
__BUEXTERN BuError_t __BUDECL BuRTBusyTableEna(U16BIT sel);

/* allows selection of external or internal bit word */
__BUEXTERN BuError_t __BUDECL BuRTExtBITWord(U16BIT sel);

/* allows the transmission of the BUSY bit word */
__BUEXTERN BuError_t __BUDECL BuRTBitInhibit(U16BIT sel);

/* enables/disables use of RT address 31 as a broadcast address */
__BUEXTERN BuError_t __BUDECL BuRTBrdcst(U16BIT sel);

/* configures ACE for advanced/normal mode code handling */
__BUEXTERN BuError_t __BUDECL BuRTModeCode(U16BIT Enhanced,U16BIT OverRideEnable);

/* allows for the normal or alternate status word control
   removed [01-SEP-1995] since functionality is duplicated in
   BuRTAltStatusEna()
__BUEXTERN BuError_t __BUDECL BuRTAltStat(U16BIT sel); */

/* allows for valid response with ME bit/no data */
__BUEXTERN BuError_t __BUDECL BuRTMsgErrValid(U16BIT sel);

/* allows for valid response with busy bit/no data */
__BUEXTERN BuError_t __BUDECL BuRTBusyValid(U16BIT sel);

/* determines if an illegal command is stored or discarded */
__BUEXTERN BuError_t __BUDECL BuRTIllegal(U16BIT sel);

/* determines who controls the RTFLAG status bit */
__BUEXTERN BuError_t __BUDECL BuRTFlagWrap(U16BIT sel);

/* controls the terminal flag bit in status words */
__BUEXTERN BuError_t __BUDECL BuRTFlag(U16BIT sel);

/* controls the subsytem request flag in status words */
__BUEXTERN BuError_t __BUDECL BuRTSetSSflag(U16BIT sel);

/* controls the service request bit in status words */
__BUEXTERN BuError_t __BUDECL BuRTSetSvcReq(U16BIT sel);

/* controls the busy bit in status words */
__BUEXTERN BuError_t __BUDECL BuRTSetBusy(U16BIT sel);

/* contrls the DBA bit in status words */
__BUEXTERN BuError_t __BUDECL BuRTSetDba(U16BIT sel);

/* controls rt mode enhanced memory mangament */
__BUEXTERN BuError_t __BUDECL BuRTEnhMM(U16BIT sel);

/* controls enhanced 1553 mode code handling */
__BUEXTERN BuError_t __BUDECL BuRTEnhModeCode(U16BIT sel);

/* controls the separate of broadcast data */
__BUEXTERN BuError_t __BUDECL BuRTSeparateBcst(U16BIT sel);

/* reads enhanced mode code data */
__BUEXTERN U16BIT __BUDECL BuRTReadEnhMCData(U16BIT addr);

/* writes enhanced mode code data */
__BUEXTERN BuError_t __BUDECL BuRTWriteEnhMCData(U16BIT addr,U16BIT data);

/* enables selected mode code interrupt */
__BUEXTERN BuError_t __BUDECL BuRTModeIrqEnable( U8BIT broadcast,U8BIT t_r,
						 U8BIT data,U16BIT map);

/* disables selected mode code interrupt */
__BUEXTERN BuError_t __BUDECL BuRTModeIrqDisable( U8BIT broadcast,U8BIT t_r,
						  U8BIT data,U16BIT map);

/* reads internal or external BIT word */
__BUEXTERN U16BIT __BUDECL BuRTBITRead(void);

/* writes external BIT word */
__BUEXTERN BuError_t __BUDECL BuRTExtBITWrite( U16BIT map);

/* asserts busy bit in status word on subaddress basis */
__BUEXTERN BuError_t __BUDECL BuRTBusyBitEnable( U8BIT broadcast,U8BIT t_r,U8BIT sa);

/* disables assertion of busy bit in status word on subaddress basis */
__BUEXTERN BuError_t __BUDECL BuRTBusyBitDisable( U8BIT broadcast,U8BIT t_r,U8BIT sa);

/* writes alternate status word to ACE */
__BUEXTERN BuError_t __BUDECL BuRTAltStatusWrite( U16BIT map);

/* reads alternate status word from ACE */
__BUEXTERN U16BIT __BUDECL BuRTAltStatusRead(void);

/* stops RT */
__BUEXTERN void __BUDECL BuRTStop(void);

/* Re-Acquire RT Address off CanBus */
__BUEXTERN BuError_t __BUDECL BuAcquireCanRTAddr(U16BIT *wRTAddr, U16BIT wCANBus);

#else

#define BuRTAltStatusEna(sel) BuWriteReg(CONFIG_3,BuReadReg(CONFIG_3)&(0xffdf|(sel<<5)))
#define BuRTBusyTableEna(sel) BuWriteReg(CONFIG_2,BuReadReg(CONFIG_2)&0xdfff|(sel<<13))

/* fixed [01-SEP-1995] changed name from BuRTBITExtWord to BuRTExtBITWord to
 * keep consistant with function call */
#define BuRTExtBITWord(sel) BuWriteReg(CONFIG_4,BuReadReg(CONFIG_4)&0x7fff|(sel<<15))

#define BuRTBitInhibit(sel) BuWriteReg(CONFIG_4,BuReadReg(CONFIG_4)&0xbfff|(sel<<14))
#define BuRTBrdcst(sel) BuWriteReg(CONFIG_5,BuReadReg(CONFIG_5)&0xff7f|(sel<<7))

/* fixed [01-SEP-1995] extra ) in macro */
#define BuRTModeCode(En,Ov) BuWriteReg(CONFIG_3,((BuReadReg(CONFIG_3)&0xffbe)|(En)|(Ov<<6)));

/* removed [01-SEP-1995] since functionality is duplicated in
   BuRTAltStatusEna()
#define BuRTAltStat(sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xffdf)|((sel)<<5)) */
#define BuRTMsgErrValid(sel) BuWriteReg(CONFIG_4,(BuReadReg(CONFIG_4)&0xffbf)|(sel<<6))
#define BuRTBusyValid(sel) BuWriteReg(CONFIG_4,(BuReadReg(CONFIG_4)&0xffdf)|(sel<<5))
#define BuRTIllegal(sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xffef)|(sel<<4))
#define BuRTFlagWrap(sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xfffb)|(sel<<2))
#define BuRTFlag(sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xFF7F)|((!sel)<<7))

#define BuRTSetSSflag(sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xFEFF)|((!sel)<<8))
#define BuRTSetSvcReq(sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xFDFF)|((!sel)<<9))

#define BuRTSetBusy(sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xFBFF)|((!sel)<<10))
#define BuRTSetDba(sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xF7FF)|((!sel)<<11))
/* fixed warning - [01-JAN-1996] */
#define BuRTEnhMM(sel) BuWriteReg(CONFIG_2,(BuReadReg(CONFIG_2)&0xFFFD)|(sel<<1))
#define BuRTEnhModeCode(sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xFFFE)|(sel))
#define BuRTSeparateBcst(sel) BuWriteReg(CONFIG_2,(BuReadReg(CONFIG_2)&0xFFFE)|(sel))
#define BuRTReadEnhMCData(addr) BuReadRam(addr)
#define BuRTWriteEnhMCData(addr,data) BuWriteRam(addr,data)

#define BuRTModeIrqEnable(br,t_r,da,mp)\
	BuWriteRam((ENH_MODE_IRQ_TABLE|(br<<2)|(t_r<<1)|da),\
	(BuReadRam(ENH_MODE_IRQ_TABLE)|(br<<2)|(t_r<<1)|da|mp))

#define BuRTModeIrqDisable(br,t_r,da,mp)\
	BuWriteRam((ENH_MODE_IRQ_TABLE|(br<<2)|(t_r<<1)|da),\
	(BuReadRam(ENH_MODE_IRQ_TABLE)|(br<<2)|(t_r<<1)|da&(~mp)))

#define BuRTExtBITWrite(map) BuWriteRam(RT_ENH_BIT_WORD_ADDR,map)
#define BuRTBITRead() BuReadReg(RT_BIT_WORD)

/* asserts busy bit in status word on subaddress basis */

#define BuRTBusyBitEnable(br,t_r,sa)\
BuWriteRam((ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0)),(1<<sa));

/* disables assertion of busy bit in status word on subaddress basis */

#define BuRTBusyBitDisable(br,t_r,sa)\
	 BuWriteRam((ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0))\
	,BuReadRam((ENH_SA_BUSY_TABLE|(br<<2)|(t_r<<1)|((sa&0x10)?1:0))&(~(1<<sa)));

#define BuRTAltStatusWrite(map) BuWriteReg(CONFIG_1,map&0x0ffe)
/*#define BuRTAltStatusRead() (BuReadReg(CONFIG_1)0x0ffe)
  modified [01-SEP-1995] */
#define BuRTAltStatusRead() (BuReadReg(CONFIG_1)&0x0ffe)
#define BuRTStop() BuWriteReg(CONFIG_1,BuReadReg(CONFIG_1)&0x3fff)

#endif

typedef struct rt_data_block_base_address_type {
	U16BIT address;
	U16BIT size;
} rt_data_block_base_address_type;

/* LynnSoft translation layer mod */
/*------------------------------- */
extern int rt_data_block_base_address_count;
extern rt_data_block_base_address_type rt_data_block_base_address[];

/*
 * END
 * RT.H (REMOTE TERMINAL MODULE)
 */
#endif /*RT_H_INCLUDED */

