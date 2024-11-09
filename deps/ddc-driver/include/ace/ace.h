#ifndef ACE_H_INCLUDED
#define ACE_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * ACE.H (ACE COMMAND/CONTROL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for ACE.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    modified header file STDACE.H from rel 1.6
 *                            to work with rel 2.0 ACE.C functions.
 *
 *   2.1       01-JUN-1995    added BuMsgStopOnError,BuFrameStopOnError,
 *                            BuStatusStopOnMsg,BuValidMENoData,
 *                            BuValidBUSYNoData,BuStatusStopOnFrame functions
 *
 *                            added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros
 *
 *                            CreateCmdWord name changed to BuCreateCmdWord
 *                            and now function or #define macro
 *
 *   2.2       01-SEP-1995    Added BuRetryStatusSet() and BuRetryIfAandME()
 *                            function prototypes/macros to the library
 *
 *   2.3       01-JAN-1996    Added Bu1553AModeCd() function prototype
 *                            and macro to enable 1553A or 1553B handling
 *                            of mode codes.
 *
 *                            Modified BuCreateCmdWord() macro to prevent
 *                            invalid encoding of the command word when
 *                            setting the word count > 31.
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       04-DEC-1996    none
 *
 *   4.0	   29-NOV-1997	  none
 *
 *   5.3.1     11-APR-2003 cf none
 */

/* ACE CONSTANTS -----------------------------------------------------------*/

/* used in the interpretation of the message type */

#define MODESADDRVAL1    0x00
#define MODESADDRVAL2    0x1F
#define BRDCSTRTADDRVAL  0x1F

/* 1553 message type constants - the values assigned to these constants
 * are from a the below bitmap.
 *
 * t/~r       =  bit 10 of cmd word 1
 * mode?      =  saddr = MODESADDRVAL1 or MODESADDRVAL2
 * bcrst?     =  rtaddr = BRDCSTRTADDRVAL
 * 2ndCmd?    =  if there is another cmd word?
 * DataWmode? =  (for mode codes only) is there data?
 */
                                    /* t/~r mode? bcrst? 2ndCmd/DataWmode? */
#define BCTORT                0     /* 0    0     0      0                 */
#define RTTORT                1     /* 0    0     0      1                 */
#define BRDCST                2     /* 0    0     1      0                 */
#define BRDCSTRTTORT          3     /* 0    0     1      1                 */
#define MODEDATARX            5     /* 0    1     0      1                 */
#define BRDCSTMODEDATA        7     /* 0    1     1      1                 */
#define RTTOBC                8     /* 1    0     0      0                 */
#define MODENODATA           12     /* 1    1     0      0                 */
#define MODEDATATX           13     /* 1    1     0      1                 */
#define BRDCSTMODENODATA     14     /* 1    1     1      0                 */
#define INVALID              15     /* 1    1     1      1                 */

/* string associated with message type */
#ifndef _ACELINUX
#define MsgTypeString {"BC to RT      ",                 \
					   "RT to RT      ",                 \
                       "Broadcast     ",                 \
                       "Bcst RT to RT ",                 \
                       "Invalid       ",                 \
                       "Mode Rx Data  ",                 \
                       "Invalid       ",                 \
                       "Bcst Mode Data",                 \
                       "RT to BC      ",                 \
                       "Invalid       ",                 \
                       "Invalid       ",                 \
                       "Invalid       ",                 \
                       "Mode No Data  ",                 \
                       "Mode Tx Data  ",                 \
                       "Bcst Mode     ",                 \
                       "Invalid       "}                 

#else
   #define MsgTypeString {"BC to RT      ","RT to RT      ","Broadcast     ","Bcst RT to RT ","Invalid       ","Mode Rx Data  ","Invalid       ","Bcst Mode Data","RT to BC      ","Invalid       ","Invalid       ","Invalid       ","Mode No Data  ","Mode Tx Data  ","Bcst Mode     ","Invalid       "}                 
#endif

/* parameter for BuTimeout */

#define RESPONSE_185  0x0000  /* 18.5 uS for response timeout */
#define RESPONSE_225  0x0200  /* 22.5 uS for response timeout */
#define RESPONSE_505  0x0400  /* 50.5 uS for response timeout */
#define RESPONSE_130  0x0600  /* 130  uS for response timeout */

#define RESPONSE_004  0x0000  /* 4  uS for EBR response timeout */
#define RESPONSE_005  0x0200  /* 5  uS for EBR response timeout */
#define RESPONSE_010  0x0400  /* 10 uS for EBR response timeout */
#define RESPONSE_026  0x0600  /* 26 uS for EBR response timeout */

/* parameter for BuTimeTagResolution */

#define BU_TIMETAG_2          0x0280  /* 2 uS           */
#define BU_TIMETAG_4          0x0200  /* 4 uS           */
#define BU_TIMETAG_8          0x0180  /* 8 uS           */
#define BU_TIMETAG_16         0x0100  /* 16 uS          */
#define BU_TIMETAG_32         0x0080  /* 32 uS          */
#define BU_TIMETAG_64         0x0000  /* 64 uS          */
#define BU_TIMETAG_TEST       0x0300  /* test clock     */
#define BU_TIMETAG_EXT_CLOCK  0x0380  /* external clock */

/* parameter constant for BuClockSel */

#define CLOCK_16     0
#define CLOCK_12     1

/* parameter constant for BuSamplingSel */

#define SINGLE_EDGE  0
#define DOUBLE_EDGE  1

/* parameter constant for BuGetMsgType */
#define NOmode  -1
#define BCmode  0
#define MTmode  1
#define RTmode  2

/* ACE TYPE DEFINITIONS AND STRUCTURES -------------------------------------*/

/* structure to store a 1553 message */

typedef struct MsgStruct {
     U8BIT Type;
     U8BIT DataLength;
     U8BIT WordCount;
     U8BIT CmdWord1flag;
     U8BIT CmdWord2flag;
     U8BIT Status1flag;
     U8BIT Status2flag;
     U8BIT LoopBack1flag;
     U8BIT LoopBack2flag;
     U16BIT TimeTag;
     U16BIT GapTime;
     U16BIT BlockStatus;
     U16BIT CmdWord1;
     U16BIT CmdWord2;
     U16BIT Status1;
     U16BIT Status2;
     U16BIT LoopBack1;
     U16BIT LoopBack2;
     U16BIT ControlWord;
     U16BIT Data[32];
} MsgType;

/* ACE FUNCTION PROTOTYPES -------------------------------------------------*/

#ifdef __BUDEBUG_FUNCTIONS

/* enables 1553A/1553B mode code handling */
__BUEXTERN BuError_t __BUDECL Bu1553AModeCd( U8BIT Sel);

/* enables or disables retries with 1553A/1553B* bit of BC Control Word */
__BUEXTERN BuError_t __BUDECL BuRetryIfAandME( U8BIT Sel);

/* enables or disables retries with bit in Status set condition */
__BUEXTERN BuError_t __BUDECL BuRetryStatusSet( U8BIT Sel);

/* enables or disables word boundaries */
__BUEXTERN BuError_t __BUDECL BuWordBoundaries( U8BIT Sel);

/* enables or disables ram parity checking */
__BUEXTERN BuError_t __BUDECL BuRamParityCheck( U8BIT Sel);

/* setup ACE for 12 or 16 mhz clock */
__BUEXTERN BuError_t __BUDECL BuClockSel( U8BIT Sel);

/* setup ACE for single or double clock edge sampling */
__BUEXTERN BuError_t __BUDECL BuSamplingSel( U8BIT Sel);

/* returns the value of the timetag register */
__BUEXTERN U16BIT __BUDECL BuReadTimeTag( void);

/* sets the response timeout time for an rt */
__BUEXTERN BuError_t __BUDECL BuTimeout( U16BIT Value);

/* sets the time tag resoultion */
__BUEXTERN BuError_t __BUDECL BuTimeTagResolution( U16BIT Value);

/* stops BC at end of current message */
__BUEXTERN BuError_t __BUDECL BuStopOnMessage( void);

/* stops BC at end of current frame */
__BUEXTERN BuError_t __BUDECL BuStopOnFrame( void);

/* increment tt clock by 1 lsb if in test mode */
__BUEXTERN BuError_t __BUDECL BuTimeTagTest( void);

/* resets time tag register to 0 */
__BUEXTERN BuError_t __BUDECL BuTimeTagReset( void);

/* start command for BC and MT modes */
__BUEXTERN BuError_t __BUDECL BuBCMTStart( void);

/* reset ACE, all registers go to 0000h */
__BUEXTERN BuError_t __BUDECL BuReset( void);

/* stop eom on message error condtion */
__BUEXTERN BuError_t __BUDECL BuMsgStopOnError(U16BIT Sel);

/* stop eof on message error condtion */
__BUEXTERN BuError_t __BUDECL BuFrameStopOnError(U16BIT Sel);

/* stop eom on status set condtion */
__BUEXTERN BuError_t __BUDECL BuStatusStopOnMsg(U16BIT Sel);

/* stop eof on status set condtion */
__BUEXTERN BuError_t __BUDECL BuStatusStopOnFrame(U16BIT Sel);

/* message valid if message error bit an no data */
__BUEXTERN BuError_t __BUDECL BuValidMENoData(U16BIT Sel);

/* message valid if busy bit an no data */
__BUEXTERN BuError_t __BUDECL BuValidBUSYNoData(U16BIT Sel);

/* creates 1553 command word from rt, t/~r, sa, and wc */
/* added BUEXTERN 8-27-96 */
__BUEXTERN U16BIT __BUDECL BuCreateCmdWord( U16BIT rt,U16BIT tr,U16BIT sa,U16BIT wc);
#else

#define Bu1553AModeCd(Sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xfffd)|((Sel)?0x0002:0))
#define BuRetryIfAandME(Sel) BuWriteReg(CONFIG_4,(BuReadReg(CONFIG_4)&0xfbff)|((Sel)?0x0400:0))
#define BuRetryStatusSet(Sel) BuWriteReg(CONFIG_4,(BuReadReg(CONFIG_4)&0xfdff)|((Sel)?0x0200:0))
#define BuWordBoundaries(Sel) BuWriteReg(CONFIG_2,(BuReadReg(CONFIG_2)&0xfbff)|((Sel)?0:0x0400))
#define BuRamParityCheck(Sel) BuWriteReg(CONFIG_2,(BuReadReg(CONFIG_2)&0xbfff)|((Sel)?0x4000:0))
#define BuClockSel(Sel) BuWriteReg(CONFIG_5,(BuReadReg(CONFIG_5)&0x7fff)|((Sel)?0x8000:0))
#define BuSamplingSel(Sel) BuWriteReg(CONFIG_5,(BuReadReg(CONFIG_5)&0x3fff)|((Sel)?0x4000:0))
#define BuReadTimeTag() BuReadReg(TIMETAG)
#define BuTimeout(Value) BuWriteReg(CONFIG_5,(BuReadReg(CONFIG_5)&0xf9ff)|Value)
#define BuTimeTagResolution(Value) BuWriteReg(CONFIG_2,(BuReadReg(CONFIG_2)&0xfc7f)|Value)
#define BuStopOnMessage() BuWriteReg(CONTROL,0x0040)
#define BuStopOnFrame() BuWriteReg(CONTROL,0x0020)
#define BuTimeTagTest() BuWriteReg(CONTROL,0x0010)
#define BuTimeTagReset() BuWriteReg(CONTROL,0x0008)
#define BuBCMTStart() BuWriteReg(CONTROL,0x0002)
#define BuReset() BuWriteReg(CONTROL,0x0001)
#define BuMsgStopOnError(Sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xefff)|(Sel<<12))
#define BuFrameStopOnError(Sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xf7ff)|(Sel<<11))
#define BuStatusStopOnMsg(Sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xfbff)|(Sel<<10))
#define BuStatusStopOnFrame(Sel) BuWriteReg(CONFIG_1,(BuReadReg(CONFIG_1)&0xfdff)|(Sel<<9))
#define BuValidMENoData(Sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xffbf)|0x0040)
#define BuValidBUSYNoData(Sel) BuWriteReg(CONFIG_3,(BuReadReg(CONFIG_3)&0xffdf)|0x0020)
/* BuCreateCmdWord modified [01-JAN-1996] to prevent error when setting 
 * word count>31, it was possibly altering the subaddress
 */
#define BuCreateCmdWord(rt,tr,sa,wc) (((rt<<11)|((tr!=0)<<10)|(sa<<5)|(wc&0x001f)))
#endif

/* enables or disables enhanced ACE features mode */
__BUEXTERN BuError_t __BUDECL BuEnhancedMode( U8BIT Sel);

/* preset ACE to library defaults */
__BUEXTERN BuError_t __BUDECL BuPreset( void);

/* splits command word into sub-words */
__BUEXTERN void __BUDECL BuParseCmdWord( U16BIT cmdword,
                                         U16BIT *rt,
                                         U16BIT *tr,
                                         U16BIT *sa,
                                         U16BIT *wc);

/* fills in the DataLength field of MsgType structure */
__BUEXTERN U16BIT __BUDECL BuGetMsgWordCount( MsgType*msg);

/* fills in the Type field of MsgType structure */
__BUEXTERN U16BIT __BUDECL BuGetMsgType( MsgType*msg,U8BIT Mode);

/* converts message type value to string */
__BUEXTERN char* __BUDECL BuMsgTypeStr( U8BIT value);

/* converts 1553 cmd word into string */
__BUEXTERN char* __BUDECL BuCmdStr( U16BIT value);

/*
 * END
 * ACE.H (ACE COMMAND/CONTROL MODULE)
 */
#endif /*ACE_H_INCLUDED */

