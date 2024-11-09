/*******************************************************************************
 * FILE: errorinj.h
 *
 * DESCRIPTION:
 *
 *  TODO
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

#ifndef __ERRORINJ_H__
#define __ERRORINJ_H__

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */
/* EI config register base address */
#define ACEX_REG_EI_CONFIG_A_BA             0x00000000
#define ACEX_REG_EI_CONFIG_B_BA             0x00000020
#define ACEX_REG_EI_CONFIG_C_BA             0x00000040

/* EI Component Register Definitions */
#define ACEX_REG_EI_RT_ENABLE               0x00000060
#define ACEX_REG_EI_BC_ENABLE               0x00000061
#define ACEX_REG_EI_TX_SHUTDOWN_DISABLE     0x00000062

/* error value limit */
#ifndef ACEX_VAL_LIMIT
#define ACEX_VAL_LIMIT(y, x, low, high) {y = x; if (y < low) {y = low; } if (y > high) {y = high; }}
#endif /* ifndef ACEX_VAL_LIMIT */

#define ACEX_EI_WORD_SEL_MIN         0
#define ACEX_EI_WORD_SEL_MAX         32

#define ACEX_EI_WORD_COUNT_MIN       -32
#define ACEX_EI_WORD_COUNT_MAX       32

#define ACEX_EI_BIT_COUNT_MIN        -4
#define ACEX_EI_BIT_COUNT_MAX        3

#define ACEX_EI_GAP_TIME_MIN         0
#define ACEX_EI_GAP_TIME_MAX         32

#define ACEX_EI_GLITCH_DUR_MIN       0
#define ACEX_EI_GLITCH_DUR_MAX       60

#define ACEX_EI_GLITCH_LOC_MIN       0
#define ACEX_EI_GLITCH_LOC_MAX       400

#define ACEX_EI_INVERSE_DUR_MIN      0
#define ACEX_EI_INVERSE_DUR_MAX      60

#define ACEX_EI_INVERSE_LOC_MIN      0
#define ACEX_EI_INVERSE_LOC_MAX      400

#define ACEX_EI_RESP_WRONG_ADDR_MIN  0
#define ACEX_EI_RESP_WRONG_ADDR_MAX  31

#define ACEX_EI_RESP_LATE_TIME_MIN   ACEX_RESP_TIME_MIN
#define ACEX_EI_RESP_LATE_TIME_MAX   ACEX_RESP_TIME_MAX

/* error masks */
#define ACEX_EI_WORD_SEL_MASK        0x003F
#define ACEX_EI_WORD_COUNT_MASK      0x003F
#define ACEX_EI_BIT_COUNT_MASK       0x0007
#define ACEX_EI_GAP_TIME_MASK        0x003F
#define ACEX_EI_GLITCH_DUR_MASK      0x003F
#define ACEX_EI_GLITCH_LOC_MASK      0x01FF
#define ACEX_EI_INVERSE_DUR_MASK     0x003F
#define ACEX_EI_INVERSE_LOC_MASK     0x01FF
#define ACEX_EI_SWM_ADDR_MASK        0x001F         /* RT address modification mask */
#define ACEX_EI_SWM_STATUS_MASK      0x07FF         /* RT status modification mask */

/* error types masks */
#define ACEX_EI_MASK_BC              0x000001F
#define ACEX_EI_MASK_MRT             0x000F31F

/* common error types applicable to both BC and MRT */
#define ACEX_EI_NONE                 0x0000000      /* without data */
#define ACEX_EI_WORD_COUNT           0x0000001
#define ACEX_EI_BIT_COUNT            0x0000002
#define ACEX_EI_GLITCH               0x0000004
#define ACEX_EI_INVERSE              0x0000008
#define ACEX_EI_GAP                  0x0000010

/* MRT specific error types */
#define ACEX_EI_NO_RESP_A            0x0000100      /* without data */
#define ACEX_EI_NO_RESP_B            0x0000200      /* without data */

#define ACEX_EI_RESP_LATE            0x0001000
#define ACEX_EI_RESP_WRONG_BUS       0x0002000      /* without data */
#define ACEX_EI_RESP_WRONG_ADDR      0x0004000
#define ACEX_EI_RESP_STATUS_BIT_SET  0x0008000

/* error injection data structure */
typedef struct _ACEX_ERR_INJ
{
    U32BIT u32ErrorType;        /* bit-wise error types */

    /* error data for common errors applicable to BC and MRT  */
    S16BIT s16WordSel;          /* word location */

    S16BIT s16WordCount;        /* word count error*/
    S16BIT s16BitCount;         /* bit count error*/
    S16BIT s16GapTime;          /* gap error*/
    S16BIT s16GlitchLoc;        /* glitch error*/
    S16BIT s16GlitchDur;
    S16BIT s16InverseLoc;       /* inverse error*/
    S16BIT s16InverseDur;

    /* error data for MRT errors only */
    S16BIT s16RespLateTime;     /* late response */
    S16BIT s16RespWrongAddr;    /* response with wrong RT address */
    U16BIT u16StatusBitMask;    /* RT status bit mask */
    U16BIT u16StatusBits;       /* RT status bit modification */
    
} ACEX_ERR_INJ;

/* RT error information */
typedef struct _ACEX_EI_RT
{
    struct _ACEX_EI_RT_REGA
    {
        U32BIT wordSel :    6;  /* word selection */
        U32BIT reserved1 :  2;  /* reserved bits */
        U32BIT wordCount :  6;  /* word count error bits */
        U32BIT reserved2 :  2;  /* reserved bits */
        U32BIT bitCount :   3;  /* bit count error bits */

        U32BIT reserved3 :  1;  /* reserved bits */
        U32BIT swm :        1;  /* enable RT status-word-modify */
        U32BIT noRespA :    1;  /* enable NO RT response on bus A */
        U32BIT noRespB :    1;  /* enable NO RT response on bus B */
        U32BIT respWrong :  1;  /* enable RT response on wrong bus */

        U32BIT gap :        6;  /* gap error bits */
        U32BIT reserved4 :  2;  /* reserved bits */
        
    } sRegA;

    struct _ACEX_EI_RT_REGB
    {
        U32BIT glitchDur :  6;  /* glitch duration */
        U32BIT reserved1 :  1;  /* reserved bits */
        U32BIT glitchLoc :  9;  /* glitch location */
        U32BIT inverseDur : 6;  /* inverse duration */
        U32BIT reserved2 :  1;  /* reserved bits */
        U32BIT inverseLoc : 9;  /* inverse location */
        
    } sRegB;

    struct _ACEX_EI_RT_REGC
    {
        U32BIT statusData : 11; /* status data */
        U32BIT addrData :   5;  /* address data */

        U32BIT statusMask : 11; /* status mask */
        U32BIT addrMask :   5;  /* address mask */
        
    } sRegC;
    
} ACEX_EI_RT;

/* BC error information */
typedef struct _ACEX_EI_BC
{
    struct _ACEX_EI_BC_CTRLBLK
    {
        U32BIT ctrlWrd :    16; /* control word */
        U32BIT hostId :     8;  /* host id used for different features */
        U32BIT gap :        6;  /* gap error bits */
        U32BIT b2 :         1;  /* bit count error bit 2 */
        U32BIT dir :        1;  /* data stream direction */
        
    } u32CBlk;

    struct _ACEX_EI_BC_CMD
    {
        U32BIT cmdWrd :     16; /* command word */
        U32BIT glitchDur :  6;  /* glitch error duration */
        U32BIT glitchLoc :  9;  /* glitch error location */
        U32BIT b1 :         1;  /* bit count error bit 1 */
        
    } u32Cmd;

    struct _ACEX_EI_BC_DBLK
    {
        U32BIT dblkPtr :    19; /* data block pointer */
        U32BIT reserved1 :  1;  /* reserved 1 */
        U32BIT wordSel :    6;  /* word selection */
        U32BIT wordCount :  6;  /* word count error */
        
    } u32DBlk;

    struct _ACEX_EI_BC_TTNM
    {
        U32BIT ttNxtMsg :   16; /* time to next message */
        U32BIT inverseDur : 6;  /* inverse error duration */
        U32BIT inverseLoc : 9;  /* inverse error location */
        U32BIT b0 :         1;  /* bit count error bit 0 */
        
    } u32TTNM;

    U16BIT u16TimeToNxtMsg;
    U16BIT reserved2;

    U32BIT u32TimeTagWordLsb;

    U16BIT u16BlkStsWord;
    U16BIT u16TimeTagWordMsb;

    U16BIT u16LoopbackWord;
    U16BIT u16MsgIdx;

    U16BIT u16StsWord;
    U16BIT u16DataIdx;

    U16BIT u16CmdWord2;
    U16BIT reserved5;

    U16BIT u16StsWord2;
    U16BIT reserved6;
    
} ACEX_EI_BC;

#endif /* __ERRINJ_H__ */
