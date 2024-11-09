/*******************************************************************************
 * FILE: TODO
 *
 * DESCRIPTION:
 *
 *      TODO
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

#ifndef __CONFIGX_H__
#define __CONFIGX_H__

/* ========================================================================== */
/*                    STRUCTURE AND ENUMERATION DEFINITION                    */
/* ========================================================================== */

/* MTI Metrics */
typedef struct _ACEX_MTI_METRICS
{
    U32BIT u32MtiStkPercentFull;
    U32BIT u32MtiStkPercentHigh;
    U32BIT u32MtiStkOverflowCount;
    U32BIT u32MtiDroppedMsgs;
} ACEX_MTI_METRICS;

/* ========================================================================== */
/*                              MACRO DEFINITIONS                             */
/* ========================================================================== */

/* response time definitions */
#define ACEX_RESP_MAX_VALUE     0x0007
#define ACEX_RESP_BC            0x0001      /* 1553 BC  */
#define ACEX_RESP_MRT           0x0002      /* 1553 MRT */
#define ACEX_RESP_MTI           0x0004      /* 1553 MTI */

/* Response Timeout Definitions - resolution of each bit is 500ns */
#define ACEX_BC_RESP_TIMEOUT_MIN        7       /* Lowest multiple value to obtain 3.5 us */
#define ACEX_BC_RESP_TIMEOUT_MAX        1023    /* Highest multiple value to obtain 511.5 us */
#define ACEX_BC_DEFAULT_RESP_TIMEOUT    36      /* 18us */

#define ACEX_RT_RESP_TIMEOUT_MIN        7       /* Lowest multiple value to obtain 3.5 us */
#define ACEX_RT_RESP_TIMEOUT_MAX        511     /* Highest multiple value to obtain 255.5 us */
#define ACEX_RT_DEFAULT_RESP_TIMEOUT    36      /* 18us */

#define ACEX_MT_RESP_TIMEOUT_MIN        7       /* Lowest multiple value to obtain 3.5 us */
#define ACEX_MT_RESP_TIMEOUT_MAX        255     /* Highest multiple value to obtain 127.5 us */
#define ACEX_MT_DEFAULT_RESP_TIMEOUT    36      /* 18us */

/* Response Time Definitions */
#define ACEX_RESP_TIME_MIN      7           /* Lowest multiple value to obtain   3.5 us */
#define ACEX_RESP_TIME_MAX      60          /* Highest multiple value to obtain 30.0 us */

/* 1553 TX amplitude values*/
#define ACEX_TX_AMPLITUDE_FULL  0x3FF
#define ACEX_TX_AMPLITUDE_HALF  0x1FF
#define ACEX_TX_AMPLITUDE_ZERO  0x000

/* Time Tag Resolution parameter can be one of the following */
#define ACEX_TT_64US            0           /* 64us Time Tag resolution*/
#define ACEX_TT_32US            1           /* 32us Time Tag resolution*/
#define ACEX_TT_16US            2           /* 16us Time Tag resolution*/
#define ACEX_TT_8US             3           /* 8us Time Tag resolution */
#define ACEX_TT_4US             4           /* 4us Time Tag resolution */
#define ACEX_TT_2US             5           /* 2us Time Tag resolution */
#define ACEX_TT_1US             6           /* 1us Time Tag resolution */
#define ACEX_TT_500NS           7           /* 500ns Time Tag resolution */
#define ACEX_TT_100NS           8           /* 100ns Time Tag resolution */
#define ACEX_TT_TEST_CLK        9           /* Test Time Tag clock */
#define ACEX_TT_EXT             10          /* Use external TT clock */

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
_EXTERN U16BIT _DECL _acexGetLibVersion
(
    void
);

_EXTERN U16BIT _DECL _acexGetCoreVersion
(
    void
);

_EXTERN S16BIT _DECL _acexGetSwVersionInfo
(
    PSWVERSIONINFO pSwVersionInfo
);

_EXTERN S16BIT _DECL _acexGetHwVersionInfo
(
    S16BIT DevNum,
    PHWVERSIONINFO pHwVersionInfo
);

_EXTERN S16BIT _DECL _acexInitialize
(
    S16BIT DevNum,
    U16BIT wAccess,
    U16BIT wMode,
    U32BIT dwMemWrdSize,
    U32BIT dwRegAddr,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL _acexFree
(
    S16BIT DevNum
);

_EXTERN U16BIT _DECL _acexRegRead
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL _acexRegWrite
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL _acexRegRead32
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL _acexRegWrite32
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U32BIT dwValue
);

_EXTERN U16BIT _DECL _acexMemRead
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL _acexMemWrite
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL _acexMemRead32
(
    S16BIT DevNum,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL _acexMemWrite32
(
    S16BIT DevNum,
    U32BIT dwMemAddr,
    U32BIT dwValue
);

_EXTERN S16BIT _DECL _acexISQClear
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL _acexISQEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _acexSetTimeTagValue
(
    S16BIT DevNum,
    U16BIT wTTValue
);

_EXTERN S16BIT _DECL _acexGetTimeTagValue
(
    S16BIT DevNum,
    U16BIT* wTTValue
);

_EXTERN S16BIT _DECL _acexSetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT ullTTValue
);

_EXTERN S16BIT _DECL _acexGetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT* ullTTValue
);

_EXTERN S16BIT _DECL _acexResetTimeTag
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexSetMetrics
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _acexInt80Enable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL _acexSetAsyncIsr(S16BIT DevNum, void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    ));
_EXTERN S16BIT _DECL _acexGetCardType
(
    S16BIT DevNum,
    U32BIT *dwCardType
);

_EXTERN S16BIT _DECL _acexSetRespTimeOut
(
    S16BIT s16DevNum,
    U16BIT u16RespTimeOut
);

_EXTERN S16BIT _DECL _acexSetIrqConditions(S16BIT DevNum, U16BIT bEnable, U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    ));
_EXTERN S16BIT _DECL _acexSetRamParityChecking
(
    S16BIT s16DevNum,
    U16BIT u16RamParityEnable
);

_EXTERN S16BIT _DECL _acexGetMemRegInfo
(
    S16BIT DevNum,
    U32BIT *pRegAddr,
    U32BIT *pMemAddr
);

_EXTERN S16BIT _DECL _acexSetClockFreq
(
    S16BIT s16DevNum,
    U16BIT u16ClockIn
);

_EXTERN S16BIT _DECL _acexSetDecoderConfig
(
    S16BIT DevNum,
    U16BIT wDoubleOrSingle,
    U16BIT wExpXingEnable
);

_EXTERN S16BIT _DECL _acexSetIrqConfig
(
    S16BIT DevNum,
    U16BIT wLvlOrPulse,
    U16BIT wAutoClear
);

_EXTERN S16BIT _DECL _acexSetTimeTagRes
(
    S16BIT s16DevNum,
    U16BIT u16TTRes
);

_EXTERN S16BIT _DECL _acexGetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING *pwCoupling,
    CHANNEL_TERMINATION *pwTermination
);

_EXTERN S16BIT _DECL _acexSetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING eCoupling,
    CHANNEL_TERMINATION eTermination
);

_EXTERN S16BIT _DECL _acexGetAmplitude
(
    S16BIT DevNum,
    U32BIT *pu32Amplitude
);

_EXTERN S16BIT _DECL _acexSetAmplitude
(
    S16BIT DevNum,
    U32BIT u32Amplitude
);

_EXTERN S16BIT _DECL _acexMtiGetMetrics
(
    S16BIT s16DevNum,
    ACEX_MTI_METRICS *pMtiMetrics
);

_EXTERN S16BIT _DECL _acexBoardReset
(
    S16BIT s16DevNum,
    U32BIT u32BdComponet
);

_EXTERN S16BIT _DECL _acexGetIRIGTx
(
    S16BIT DevNum,
    PACE_IRIG_TX pstructIRIG
);

_EXTERN S16BIT _DECL _acexSetIRIGTx
(
    S16BIT DevNum,
    ACE_IRIG_TX structIRIG
);

_EXTERN S16BIT _DECL _acexMtiGetMetrics
(
    S16BIT s16DevNum,
    ACEX_MTI_METRICS *pMtiMetrics
);

_EXTERN S16BIT _DECL _acexComponentRegRead
(
    S16BIT s16DevNum,
    U16BIT U16RegAddr,
    U16BIT u16Length,
    U32BIT *pData,
    ACEX_COMPONENT eComponent
);

_EXTERN S16BIT _DECL _acexGetBoardStatus
(
    S16BIT DevNum,
    U32BIT cmd,
    void *pdata
);

_EXTERN S16BIT _DECL _acexConfiguration
(
    S16BIT s16DevNum,
    U32BIT cmd,
    void *pdata
);

/* these initialize the discrete and avionic functions independent of 1553. */
_EXTERN S16BIT _DECL _acexIOInitialize
(
    S16BIT DevNum,
    S16BIT wOptions
);

_EXTERN S16BIT _DECL _acexIOFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexGetChannelCount
(
    S16BIT DevNum,
    CHANCOUNT_t *pChanCount
);

/* EI transmitter shutdown enable/disable functions */
_EXTERN S16BIT _DECL _acexEITxShutdownEnable
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexEITxShutdownDisable
(
    S16BIT s16DevNum
);

_EXTERN U32BIT _DECL _acexRegRead_Ex
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwRegType
);

_EXTERN S32BIT _DECL _acexRegWrite_Ex
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwValue,
    U32BIT dwRegType
);


/* internal debug use only */
_EXTERN S16BIT _DECL _acexDebug
(
    S16BIT s16DevNum,
    U32BIT cmd,
    void *pdata
);

_EXTERN S16BIT _DECL _acexSetBoardFeature
(
    S16BIT s16DevNum,
    U32BIT u32Command,
    void *pData
);
#endif /* __CONFIGX_H__ */
