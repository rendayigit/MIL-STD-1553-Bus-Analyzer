/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *          AceXtreme 'C' Run Time Library
 *
 *          Copyright (c) 2008 by Data Device Corporation
 *          All Rights Reserved.
 *
 *          axdioop.c (Discrete IO operations module)
 *
 *  Created 11/13/02 KWH
 *          2008 SF+ support added
 *
 * Programmers Note:
 *  The discrete IO ports associated with a particular logical device
 *  are also associated with up to  four other devices.  These ports
 *  are common to cards that have up to four Enhanced Mini-ACE chips
 *  on them.  This must be taken into consideration when working with
 *  multiple channels on the same card while manipulating the ports.
 */

#ifndef __AXDIOOP_H__
#define __AXDIOOP_H__

#define ACEX_DISC_ACTIVE_HI     0
#define ACEX_DISC_ACTIVE_LO     1

#define ACEX_DISC_SW_CTRL       0
#define ACEX_DISC_TRGIMR_CTRL   1

#define ACEX_DISC_SEL_TRG       0
#define ACEX_DISC_SEL_IMR       1

/* Discrete I/O time tag configuration options common between RTL and Driver. */
/* Time tag clock source options. */
#define TT_RO_16_BIT    0x00000000
#define TT_RO_17_BIT    0x00000010
#define TT_RO_18_BIT    0x00000020
#define TT_RO_19_BIT    0x00000030
#define TT_RO_20_BIT    0x00000040
#define TT_RO_21_BIT    0x00000050
#define TT_RO_22_BIT    0x00000060
#define TT_RO_48_BIT    0x00000070
#define TT_RESO_64US    0x00000000
#define TT_RESO_32US    0x00000001
#define TT_RESO_16US    0x00000002
#define TT_RESO_08US    0x00000003
#define TT_RESO_04US    0x00000004
#define TT_RESO_02US    0x00000005
#define TT_RESO_01US    0x00000006
#define TT_RESO_500NS   0x00000007
#define TT_RESO_100NS   0x00000008
#define TT_TST_CLK      0x00000009
#define TT_EXT_CLK      0x0000000A
#define TT_IRIGB        0x00002000
#define TT_CFG_MSK      TT_RO_16_BIT | \
    TT_RO_17_BIT | \
    TT_RO_18_BIT | \
    TT_RO_19_BIT | \
    TT_RO_20_BIT | \
    TT_RO_21_BIT | \
    TT_RO_22_BIT | \
    TT_RO_48_BIT | \
    TT_RESO_64US | \
    TT_RESO_32US | \
    TT_RESO_16US | \
    TT_RESO_08US | \
    TT_RESO_04US | \
    TT_RESO_02US | \
    TT_RESO_01US | \
    TT_RESO_500NS | \
    TT_RESO_100NS | \
    TT_TST_CLK | \
    TT_EXT_CLK | \
    TT_IRIGB

/* Time tag interrupt conditions. */
#define TT_INT_BUF_OVFL 0x00000040
#define TT_INT_ENT_CNT  0x00000020
#define TT_INT_BUF_100  0x00000010
#define TT_INT_BUF_75   0x00000008
#define TT_INT_BUF_50   0x00000004
#define TT_INT_BUF_25   0x00000002
#define TT_INT_RO       0x00000001
#define TT_INT_MSK      TT_INT_BUF_OVFL | \
    TT_INT_ENT_CNT | \
    TT_INT_BUF_100 | \
    TT_INT_BUF_75 | \
    TT_INT_BUF_50 | \
    TT_INT_BUF_25 | \
    TT_INT_RO

#define TT_CTL_STOP   0x00000008
#define TT_CTL_START  0x00000004
#define TT_CTL_RESET  0x00000001
#define TT_CTL_MSK    TT_CTL_START | \
    TT_CTL_STOP | \
    TT_CTL_RESET

/* DIO TT memory size */
#define DIO_TT_BUF_LEN  0x10000


/* Time tag configuration structure. */
typedef struct _DIO_TT_CFG
{
    U32BIT u32Dio;      /* Discrete input signals to enable.  D15-8 => falling edge.  D7-0 => rising edge */
    U32BIT u32TtCfg;    /* Time tag clock source options */
    U32BIT u32IntMsk;   /* Interrupt conditions */
    U32BIT u32EntCnt;   /* Threshold for entry count interrupt */

} DIO_TT_CFG;


/* discrete configuration informatio for Triggers and IMRs */
typedef struct _ACEX_DISC_CONFIG
{
    U16BIT u16Polarity;      /* polarity in ACEX_DISC_ACTIVE_HI or ACEX_DISC_ACTIVE_LO   */
    U16BIT u16Control;       /* control by ACEX_DISC_SW_CTRL or ACEX_DISC_TRGIMR_CTRL */
    U16BIT u16SelTrgImr;     /* select ACEX_DISC_SEL_TRG or ACEX_DISC_SEL_IMR */
    BOOLEAN bSSFDisable;     /* TRUE if want to disable the SSF in the channel */
} ACEX_DISC_CONFIG;


/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */

/* this get/set once bit at a time. */
_EXTERN S16BIT _DECL _acexGetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _acexGetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _acexGetDiscIn
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _acexSetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_LEVEL Level
);

_EXTERN S16BIT _DECL _acexSetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_DIR Direction
);

/* these get/set all the bits at once. */
_EXTERN S16BIT _DECL _acexGetDiscAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL _acexSetDiscAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

/* additional discrete configuration for triggers and IMRs */
_EXTERN S16BIT _DECL _acexSetDiscConfigure
(
    S16BIT s16DevNum,
    S16BIT s16Discrete,
    ACEX_DISC_CONFIG sDiscConfig
);

_EXTERN S16BIT _DECL _acexClrDiscConfigure
(
    S16BIT s16DevNum
);

/* Discrete IO time tag */
_EXTERN S16BIT _DECL _acexDioTtCfg(S16BIT s16DevNum, DIO_TT_CFG * psDioCfg, void (_DECL * pfDioTtIsr)
    (
        S16BIT DevNum,
        U32BIT u32IntSts
    ));
_EXTERN S16BIT _DECL _acexDioTtCtl
(
    S16BIT s16DevNum,
    U32BIT u32DioTtCtl
);

_EXTERN S16BIT _DECL _acexDioTtRead
(
    S16BIT s16DevNum,
    U8BIT *pu8Data,
    U32BIT *pu32Bytes
);

#endif /* __DIOOP_H__ */
