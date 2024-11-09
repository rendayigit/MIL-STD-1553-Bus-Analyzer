/*****************************************************************************
*       Data Device Corporation
*       105 Wilbur Place
*       Bohemia N.Y. 11716
*
*       AceXtreme 'C' Run Time Library
*
*       Copyright (c) 2010 Data Device Corporation
*       All Rights Reserved.
*
*       Filename: trgx.h
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
*       This is the trgx.h header file for triggers.
*
*
******************************************************************************/

#ifndef __TRGX_H__
#define __TRGX_H__

/* 18 trigger IDs */
#define ACEX_TRG_IRQ_MASK               0x0003FFFF      /* interrupt mask */
#define ACEX_TRG_AVAIL_MASK             0x0003FFFF      /* bit-wise trigger availability mask*/

/*  bit 0 - 1:  TMTs
    bit 2 - 9:  GPTs */
#define ACEX_TRG_TMT_NUM                1
#define ACEX_TRG_GPT_NUM                8

#define ACEX_TRG_ID_TMT1                0               /* time message trigger 1 */
#define ACEX_TRG_ID_TMT2                1               /* time message trigger 2 */
#define ACEX_TRG_ID_GPT1                2               /* general purpose trigger 1  */
#define ACEX_TRG_ID_GPT2                3               /* general purpose trigger 2  */
#define ACEX_TRG_ID_GPT3                4               /* general purpose trigger 3  */
#define ACEX_TRG_ID_GPT4                5               /* general purpose trigger 4  */
#define ACEX_TRG_ID_GPT5                6               /* general purpose trigger 5  */
#define ACEX_TRG_ID_GPT6                7               /* general purpose trigger 6  */
#define ACEX_TRG_ID_GPT7                8               /* general purpose trigger 7  */
#define ACEX_TRG_ID_GPT8                9               /* general purpose trigger 8  */
#define ACEX_TRG_ID_GPT9                10              /* general purpose trigger 9  */
#define ACEX_TRG_ID_GPT10               11              /* general purpose trigger 10 */
#define ACEX_TRG_ID_GPT11               12              /* general purpose trigger 11 */
#define ACEX_TRG_ID_GPT12               13              /* general purpose trigger 12 */
#define ACEX_TRG_ID_GPT13               14              /* general purpose trigger 13 */
#define ACEX_TRG_ID_GPT14               15              /* general purpose trigger 14 */
#define ACEX_TRG_ID_GPT15               16              /* general purpose trigger 15 */
#define ACEX_TRG_ID_GPT16               17              /* general purpose trigger 16 */

/* 12 trigger events: the order must be aligned with enable register*/
#define ACEX_TRG_EVENT_AVAIL_MASK       0x0FFF          /* bit-wise event availability mask */
#define ACEX_TRG_EVENT_NUM              12

#define ACEX_TRG_EVENT_MTI_MARK_A       0x0001
#define ACEX_TRG_EVENT_MTI_MARK_B       0x0002
#define ACEX_TRG_EVENT_MTI_MARK_C       0x0004
#define ACEX_TRG_EVENT_MTI_MARK_D       0x0008
#define ACEX_TRG_EVENT_MON_START        0x0010
#define ACEX_TRG_EVENT_MON_STOP         0x0020
#define ACEX_TRG_EVENT_REPLAY_START     0x0040
#define ACEX_TRG_EVENT_REPLAY_STOP      0x0080
#define ACEX_TRG_EVENT_BC_IMR_WAIT      0x0100
#define ACEX_TRG_EVENT_RT_IMR_WAIT      0x0200
#define ACEX_TRG_EVENT_TIMETAG_LATCH    0x0400
#define ACEX_TRG_EVENT_INTERRUPT        0x0800

/* 21 trigger inputs */
#define ACEX_TRG_IN_NUM                 21

#define ACEX_TRG_IN_DISABLED            0               /* disable the trigger */
#define ACEX_TRG_IN_START               1               /* start the trigger immediately after it is enabled */
#define ACEX_TRG_IN_DISC                2               /* reserved if TMT; DIO available if GPT  */

#define ACEX_TRG_IN_TMT1                3               /* time message trigger 1 */
#define ACEX_TRG_IN_TMT2                4               /* time message trigger 2 */
#define ACEX_TRG_IN_GPT1                5               /* general purpose trigger 1  */
#define ACEX_TRG_IN_GPT2                6               /* general purpose trigger 2  */
#define ACEX_TRG_IN_GPT3                7               /* general purpose trigger 3  */
#define ACEX_TRG_IN_GPT4                8               /* general purpose trigger 4  */
#define ACEX_TRG_IN_GPT5                9               /* general purpose trigger 5  */
#define ACEX_TRG_IN_GPT6                10              /* general purpose trigger 6  */
#define ACEX_TRG_IN_GPT7                11              /* general purpose trigger 7  */
#define ACEX_TRG_IN_GPT8                12              /* general purpose trigger 8  */
#define ACEX_TRG_IN_GPT9                13              /* general purpose trigger 9  */
#define ACEX_TRG_IN_GPT10               14              /* general purpose trigger 10 */
#define ACEX_TRG_IN_GPT11               15              /* general purpose trigger 11 */
#define ACEX_TRG_IN_GPT12               16              /* general purpose trigger 12 */
#define ACEX_TRG_IN_GPT13               17              /* general purpose trigger 13 */
#define ACEX_TRG_IN_GPT14               18              /* general purpose trigger 14 */
#define ACEX_TRG_IN_GPT15               19              /* general purpose trigger 15 */
#define ACEX_TRG_IN_GPT16               20              /* general purpose trigger 16 */

/* 3 event inputs */
#define ACEX_TRG_EVENT_DISABLE          0               /* disable events */
#define ACEX_TRG_EVENT_SET              1               /* set events */
#define ACEX_TRG_EVENT_TRG              2               /* link events to a trigger */

/* Trigger Mark Masks */
#define ACEX_TRG_EVENT_MTI_MARK_A_MASK  0x1000          /* Trigger Mark A */
#define ACEX_TRG_EVENT_MTI_MARK_B_MASK  0x2000          /* Trigger Mark B */
#define ACEX_TRG_EVENT_MTI_MARK_C_MASK  0x4000          /* Trigger Mark C */
#define ACEX_TRG_EVENT_MTI_MARK_D_MASK  0x8000          /* Trigger Mark D */
#define ACEX_TOTAL_BYTES_MASK           0x0FFF          /* Masks out trigger marks */

/* ========================================================================== */
/*                        TRIGGER USER CONFIGURATUIN                          */
/* ========================================================================== */
/* time-message trigger configuration */
typedef struct _ACEX_TRG_CONFIG_TMT
{
    U8BIT u8InTmtTrg;       /* # - TMT trigger;         0 - not with trigger input   */

    BOOLEAN bSet;           /* 1 - set;                 0 - not to set */

    BOOLEAN bTimeTrg;       /* 1 - use time;            0 - use msg */
    BOOLEAN bInUs;          /* 1 - in us;               0 - in ms */
    BOOLEAN bClrAuto;       /* 1 - auto clr;            0 - not to clr */

    U16BIT u16TMTCount;     /* time or msg count
                                   if time = 1, time value in ms/us depending on us
                                   if time = 0, msg count */
} ACEX_TRG_CONFIG_TMT;

/* general-purpose trigger configuration */
typedef struct _ACEX_TRG_CONFIG_GPT
{
    U8BIT u8InGptTrg;       /* # - GPT trigger;         0 - not with trigger input   */

    BOOLEAN bSet;           /* 1 - set;                 0 - not to set */

    BOOLEAN bClrNotMatched; /* 1 - clr if not matched;  0 - not to clr */
    BOOLEAN bClrAuto;       /* 1 - clr automatically;   0 - not to clr */
    BOOLEAN bClrNewMsg;     /* 1 - clr if it's new msg; 0 - not to clr */

    BOOLEAN bBcCmdEn;       /* 1 - bc cmd;              0 - not bc cmd  */
    BOOLEAN bBcDataEn;      /* 1 - bc data;             0 - not bc data  */
    BOOLEAN bRtStatusEn;    /* 1 - rt status;           0 - not rt status  */
    BOOLEAN bRtDataEn;      /* 1 - rt data;             0 - not rt data  */
    BOOLEAN bBswEn;         /* 1 - bsw;                 0 - not bsw */
    BOOLEAN bBusAEn;        /* 1 - bus A;               0 - not bus A */
    BOOLEAN bBusBEn;        /* 1 - bus B;               0 - not bus A */

    U8BIT u8DataCnt;        /* 0 - 31 words */
    U8BIT u8TrgCnt;         /* 0 - 31 triggers */

    U16BIT u16Mask;         /* msg data mask  */
    U16BIT u16Data;         /* msg data value */
} ACEX_TRG_CONFIG_GPT;

/* trigger configuration information including an union of TMT and GPT data structure */
typedef struct _ACEX_TRG_CONFIG
{
    union ACEX_TRG_CONFIG_UNION
    {
        ACEX_TRG_CONFIG_TMT sTmt;
        ACEX_TRG_CONFIG_GPT sGpt;
    } u;
} ACEX_TRG_CONFIG;

/* Discrete IO trigger configuration */
typedef struct _ACEX_TRG_CONFIG_DISC
{
    BOOLEAN bActiveHI;      /* 0 - active LO;   1 - active HI  */
    BOOLEAN bByTrg;         /* 0 - SW In/Out;   1 - Trg In/out */
    BOOLEAN bBcImr;         /* 0 - triggers;    1 - BC-IMRs, fixed link with DIO 0-3*/
    BOOLEAN bRtImr;         /* 0 - triggers;    1 - RT-IMRs, fixed link with DIO 4-7*/
} ACEX_TRG_CONFIG_DISC;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL _acexTRGReset
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexTRGConfigure
(
    S16BIT s16DevNum,
    U16BIT u16TrgId,
    ACEX_TRG_CONFIG sTrgConfig
);

_EXTERN S16BIT _DECL _acexTRGEnable
(
    S16BIT s16DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL _acexTRGDisable
(
    S16BIT s16DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL _acexTRGEventSelect
(
    S16BIT s16DevNum,
    U16BIT u16TrgId,
    U16BIT u16Events,
    U16BIT u16EvtInput
);

_EXTERN S16BIT _DECL _acexTRGEventEnable
(
    S16BIT s16DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL _acexTRGEventDisable
(
    S16BIT s16DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL _acexTRGGetTimeTag
(
    S16BIT s16DevNum,
    U64BIT *pu64TimeTagValue
);

_EXTERN S16BIT _DECL _acexTRGGetStatus
(
    S16BIT s16DevNum,
    U32BIT *pu32TrgStatus,
    U32BIT *pu32IrqStatus
);

#endif /* __TRGX_H__ */
