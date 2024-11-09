/******************************************************************************
 * Data Device Corporation
 *
 * Filename: mtix.h
 *
 *
 * Copyright (c) 2009 Data Device Corporation
 ******************************************************************************
 * Non-Disclosure Statement
 * ------------------------
 * This software is the sole property of Data Device Corporation. All
 * rights, title, ownership, or other interests in the software remain
 * the property of Data Device Corporation. This software may be used
 * in accordance with applicable licenses. Any unauthorized use,
 * duplication, transmission, distribution, or disclosure is expressly
 * forbidden.
 *
 * This non-disclosure statement may not be removed or modified without*
 * prior written consent of Data Device Corporation.
 ******************************************************************************
 * Module Description:
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *
 *  SFP+ Monitor Terminal improvemetns header file for mtix.c.
 *
 *****************************************************************************/

#ifndef __MTIX_H__
#define __MTIX_H__

/* ========================================================================== */
/*                                DEFINITIONS                                  */
/* ========================================================================== */

/* MTI Configure parameter range definitions for ACEX*/
#define ACEX_MTI_BUFFER_COUNT_MIN       4
#define ACEX_MTI_BUFFER_COUNT_MAX       32

#define ACEX_MTI_BUFFER_SIZE_MIN            0x1000
#define ACEX_MTI_BUFFER_SIZE_MAX            0x80000

#define ACEX_MTI_INTERRUPT_TIME_MIN     1        /* 1ms Minimum Interrupt Interval */
#define ACEX_MTI_INTERRUPT_TIME_MAX     65535  /* 6.5535s Maximun Interrupt Interval */

#define ACEX_MTI_INTERRUPT_MSG_MIN      1
#define ACEX_MTI_INTERRUPT_MSG_MAX      1000     /* Based On Max Buffer Size */

#define ACEX_MTI_INTERRUPT_WORDS_MIN        (ACEX_MTI_BUFFER_SIZE_MIN / 2) - 40 /* Subtract 40 Words For Potential In-Process Message */
#define ACEX_MTI_INTERRUPT_WORDS_MAX        (ACEX_MTI_BUFFER_SIZE_MAX / 2) - 40 /* Subtract 40 Words For Potential In-Process Message */

/* Expanded Device Memory Buffer Sizes for SFP */
#define MTIX_DEVBUF_SIZE_1K     0x00400     /* 1024 bytes */
#define MTIX_DEVBUF_SIZE_2K     0x00800     /* 2048 bytes */
#define MTIX_DEVBUF_SIZE_4K     0x01000     /* 4096 bytes */
#define MTIX_DEVBUF_SIZE_8K     0x02000     /* 8092 bytes */
#define MTIX_DEVBUF_SIZE_16K    0x04000     /* 16384 bytes */
#define MTIX_DEVBUF_SIZE_32K    0x08000     /* 32768 bytes */
#define MTIX_DEVBUF_SIZE_2M     0x200000    /* 2097152 bytes */
#define MTIX_DEVBUF_SIZE_4M     0x400000    /* 4194304 bytes */

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */
typedef enum _ACE_MTI_IRIG_RANGE
{
    ACE_MTI_IRIG_RANGE_LOW,     /* 200mV – 2.5V */
    ACE_MTI_IRIG_RANGE_HIGH     /* 2.5V – 10V   */
} ACE_MTI_IRIG_RANGE;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

_EXTERN S16BIT _DECL _acexMTIConfigure
(
    S16BIT s16DevNum,
    U32BIT u32DevBufByteSize,
    U32BIT u32NumBufBlks,
    U32BIT u32BufBlkByteSize,
    BOOLEAN fZeroCopyEnable,
    U32BIT u32IrqDataLen,
    U32BIT u32IrqMsgCnt,
    U16BIT u16IrqTimeInterval,
    U32BIT u32IntConditions,
    U16BIT u16Ch10ChnlId,
    U8BIT u8HdrVer,
    U8BIT u8RelAbsTime,
    U8BIT u8Ch10Checksum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _acexMTISetExternalClk
(
    S16BIT s16DevNum,
    U16BIT u16TTRes,
    U16BIT u16ExtCtrl
);

_EXTERN S16BIT _DECL _acexMTIStop
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIStart
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIInitiateHostIrq
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIInitialize
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIFree
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIGetCh10DataPkt
(
    S16BIT s16DevNum,
    PMTI_CH10_DATA_PKT *ppCh10Pkt,
    S16BIT s16Timeout
);

_EXTERN S16BIT _DECL _acexMTIPause
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIContinue
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMTIGetCh10TimePkt
(
    S16BIT s16DevNum,
    PMTI_CH10_TIME_PKT *ppCh10Pkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL _acexMTICh10TimeFmt
(
    S16BIT s16DevNum,
    U16BIT u16Format
);

_EXTERN S16BIT _DECL _acexMTICh10TimePktEnable
(
    S16BIT s16DevNum,
    BOOLEAN bEnable
);

_EXTERN S16BIT _DECL _acexMTISetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE Range
);

_EXTERN S16BIT _DECL _acexMTIGetCh10TimeRange
(
    S16BIT DevNum,
    ACE_MTI_IRIG_RANGE *pRange
);

_EXTERN S16BIT _DECL _acexMTISetCh10TimePktId
(
    S16BIT DevNum,
    U16BIT u16ChannelId
);

_EXTERN S16BIT _DECL _acexMtiShow
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTISetRespTimeout
(
    S16BIT s16DevNum,
    U32BIT u32Multiple
);

/* MCS 9.3.19 MTI Decode 1553A -- Add DevNum*/
_EXTERN S16BIT _DECL _acexMTIDecodeRawMsgEx
(
	S16BIT s16DevNum,
	PMTI_CH10_DATA_PKT pCh10Pkt,
	U16BIT **ppCurMsg,
	MSGSTRUCT *pDecMsg,
	U32BIT u32Options  /* Future expansion */

);

#endif /* ifndef __MTIX_H__ */
