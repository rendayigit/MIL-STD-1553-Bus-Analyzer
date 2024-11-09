/*******************************************************************************
 * FILE: msgop.h
 *
 * DESCRIPTION:
 *
 *  Message structure operations header file.
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

#ifndef __MSGOP_H__
#define __MSGOP_H__

/* the T/R* bit can be defined as follows (wTR parameters) */
#define ACE_RX_CMD  0x0000
#define ACE_TX_CMD  0x0001

/* 1553 message type constants - the values assigned to these constants
 * are from a the below bitmap.
 *
 * Bcst         = RT = ACE_BROADCAST
 * Mode         = SA = ACE_MODE_CODE1 or ACE_MODE_CODE2
 * C2/Data      = RT to RT transfer or Data associated with mode code
 * Tx/~Rx       = bit 10 of Cmd Word 1
 */

#define ACE_BROADCAST               0x001F  /* Broadcast RT address (31) */
#define ACE_MODE_CODE1              0x0000  /* Mode Code Subaddress (0)  */
#define ACE_MODE_CODE2              0x001F  /* Mode Code Subaddress (31) */

                                        /* Bcst   Mode  C2/Data  Tx/~Rx */
#define ACE_MSG_BCTORT              0   /*   0      0       0       0   */
#define ACE_MSG_RTTOBC              1   /*   0      0       0       1   */
#define ACE_MSG_RTTORT              2   /*   0      0       1       0   */
#define ACE_MSG_MODENODATA          5   /*   0      1       0       1   */
#define ACE_MSG_MODEDATARX          6   /*   0      1       1       0   */
#define ACE_MSG_MODEDATATX          7   /*   0      1       1       1   */
#define ACE_MSG_BRDCST              8   /*   1      0       0       0   */
#define ACE_MSG_BRDCSTRTTORT        10  /*   1      0       1       0   */
#define ACE_MSG_BRDCSTMODENODATA    13  /*   1      1       0       1   */
#define ACE_MSG_BRDCSTMODEDATA      14  /*   1      1       1       0   */
#define ACE_MSG_INVALID             15  /*   1      1       1       1   */

/* RAW message sizes */
#define ACE_MSGSIZE_RT              36
#define ACE_MSGSIZE_MT              40
#define ACE_MSGSIZE_BC              42
#define ACE_MSGSIZE_RTMT            42
#define ACE_MSGSIZE_MRT             40

/* Global (used for all modes) Message Structure for decoded 1553 msgs */
typedef struct MSGSTRUCT
{
    U16BIT wType;               /* Contains the msg type (see above) */
    U16BIT wBlkSts;             /* Contains the block status word */
    U16BIT wTimeTag;            /* Time tag of message */
    U16BIT wCmdWrd1;            /* First command word */
    U16BIT wCmdWrd2;            /* Second command word (RT to RT) */
    U16BIT wCmdWrd1Flg;         /* Is command word 1 valid? */
    U16BIT wCmdWrd2Flg;         /* Is command word 2 valid? */
    U16BIT wStsWrd1;            /* First status word */
    U16BIT wStsWrd2;            /* Second status word */
    U16BIT wStsWrd1Flg;         /* Is status word 1 valid? */
    U16BIT wStsWrd2Flg;         /* Is status word 2 valid? */
    U16BIT wWordCount;          /* Number of valid data words */
    U16BIT aDataWrds[32];       /* An array of data words */

    /* The following are only applicable in BC mode */
    U16BIT wBCCtrlWrd;          /* Contains the BC control word */
    U16BIT wBCGapTime;          /* Message gap time word */
    U16BIT wBCLoopBack1;        /* First looped back word */
    U16BIT wTimeTag2;           /* wBCLoopBack2 is redefined as TimeTag2 */
    U16BIT wBCLoopBack1Flg;     /* Is loop back 1 valid? */
    U16BIT wTimeTag3;           /* wBCLoopBack2Flg is redefined as TimeTag3 */
    
}MSGSTRUCT;

_EXTERN S16BIT _DECL aceCmdWordCreate
(
    U16BIT *pCmdWrd,
    U16BIT wRT,
    U16BIT wTR,
    U16BIT wSA,
    U16BIT wWC
);

_EXTERN S16BIT _DECL aceCmdWordParse
(
    U16BIT wCmdWrd,
    U16BIT *pRT,
    U16BIT *pTR,
    U16BIT *pSA,
    U16BIT *pWC
);

_EXTERN char* _DECL aceGetMsgTypeString
(
    U16BIT wMsgType
);

_EXTERN char* _DECL aceGetBSWErrString
(
    U16BIT wMode,
    U16BIT wBlkSts
);

#endif /* ifndef __MSGOP_H__ */
