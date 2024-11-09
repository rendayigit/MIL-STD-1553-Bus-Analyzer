/*******************************************************************************
 * FILE: config.h
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* ========================================================================== */
/*                             VERSION EXTERNAL                               */
/* ========================================================================== */
extern U32BIT EMACE_RTL_VERSION;
extern U32BIT EMACE_CORE_VERSION;

/* ========================================================================== */
/*                    STRUCTURE AND ENUMERATION DEFINITION                    */
/* ========================================================================== */

#define DDC_CONFIGURATION_CMD__SET_TASK_PRIORITY            0x00000001


/* 1553 Coupling Enumeration */
typedef enum
{
    ACE_COUPLING_TRANSFORMER = 0,
    ACE_COUPLING_DIRECT = 1

} CHANNEL_COUPLING;

/* 1553 Termination Enumeration */
typedef enum
{
    ACE_TERMINATION_NONE = 0,
    ACE_TERMINATION_HALF = 1,
    ACE_TERMINATION_FULL = 3

} CHANNEL_TERMINATION;

/* acexRegRead Types Enumeration */
typedef enum
{
    ACEX_COMPONENT_ABSOLUTE = 0,
    ACEX_COMPONENT_BD,
    ACEX_COMPONENT_BC,
    ACEX_COMPONENT_MRT,
    ACEX_COMPONENT_MTI,
    ACEX_COMPONENT_1553,
    ACEX_COMPONENT_EI,
    ACEX_COMPONENT_REPLAY,
    ACEX_COMPONENT_TRG,
    ACEX_COMPONENT_MAX

} ACEX_COMPONENT;

/* Digital IRIG Transmitter Definitions */
typedef struct _ACE_IRIG_TX
{
    U16BIT u16IRIGBTxSupported;
    U16BIT u16Enable;
    U16BIT u16Seconds;
    U16BIT u16Minutes;
    U16BIT u16Hours;
    U16BIT u16Days;
    U16BIT u16Year;
    U32BIT u32Control;

}ACE_IRIG_TX, *PACE_IRIG_TX;

/* A single entry from the RT/MT Interrupt Status Queue */
typedef struct ISQENTRY
{
    U16BIT wISQHeader;          /* gives information on what data is */
    U16BIT wISQData;            /* the information for the entry */

} ISQENTRY;

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_CONFIG_FUNCTIONS
{
    S16BIT (_DECL *aceInitialize)
    (
        S16BIT DevNum,
        U16BIT wAccess,
        U16BIT wMode,
        U32BIT dwMemWrdSize,
        U32BIT dwRegAddr,
        U32BIT dwMemAddr
    );

    S16BIT (_DECL *aceFree)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceSetAddressMode)
    (
        S16BIT DevNum,
        U16BIT wAddrMode
    );

    S16BIT (_DECL *aceSetClockFreq)
    (
        S16BIT DevNum,
        U16BIT u16ClockIn
    );

    S16BIT (_DECL *aceSetTimeTagValue)
    (
        S16BIT DevNum,
        U16BIT wTTValue
    );

    S16BIT (_DECL *aceSetTimeTagValueEx)
    (
        S16BIT DevNum,
        U64BIT ullTTValue
    );

    S16BIT (_DECL *aceGetTimeTagValue)
    (
        S16BIT DevNum,
        U16BIT *wTTValue
    );

    S16BIT (_DECL *aceGetTimeTagValueEx)
    (
        S16BIT DevNum,
        U64BIT *ullTTValue
    );

    S16BIT (_DECL *aceResetTimeTag)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceSetRespTimeOut)
    (
        S16BIT DevNum,
        U16BIT u16RespTimeOut
    );

    S16BIT (_DECL *aceSetIrqConfig)
    (
        S16BIT DevNum,
        U16BIT wLvlOrPulse,
        U16BIT wAutoClear
    );

    S16BIT (_DECL *aceSetTimeTagRes)
    (
        S16BIT DevNum,
        U16BIT u16TTRes
    );

    S16BIT (_DECL *aceSetIrqConditions)
    (
        S16BIT DevNum,
        U16BIT bEnable,
        U32BIT dwIrqMask,
        void (_DECL * funcExternalIsr)
        (
            S16BIT DevNum,
            U32BIT dwIrqStatus
        )
    );

    S16BIT (_DECL *aceSetDecoderConfig)
    (
        S16BIT DevNum,
        U16BIT wDoubleOrSingle,
        U16BIT wExpXingEnable
    );

    S16BIT (_DECL *aceSetRamParityChecking)
    (
        S16BIT DevNum,
        U16BIT u16RamParityEnable
    );

    U16BIT (_DECL *aceRegRead)
    (
        S16BIT DevNum,
        U16BIT wRegAddr
    );

    S16BIT (_DECL *aceRegWrite)
    (
        S16BIT DevNum,
        U16BIT wRegAddr,
        U16BIT wValue
    );

    U16BIT (_DECL *aceMemRead)
    (
        S16BIT DevNum,
        U16BIT wMemAddr
    );

    S16BIT (_DECL *aceMemWrite)
    (
        S16BIT DevNum,
        U16BIT wMemAddr,
        U16BIT wValue
    );

    S16BIT (_DECL *aceGetMemRegInfo)
    (
        S16BIT DevNum,
        U32BIT *pRegAddr,
        U32BIT *pMemAddr
    );

    S16BIT (_DECL *aceISQClear)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceISQRead)
    (
        S16BIT DevNum,
        ISQENTRY *pISQEntry
    );

    S16BIT (_DECL *aceISQEnable)
    (
        S16BIT DevNum,
        U16BIT bEnable
    );

    S16BIT (_DECL *aceSetMetrics)
    (
        S16BIT DevNum,
        U16BIT bEnable
    );

    U16BIT (_DECL *aceGetLibVersion)
    (
        void
    );

    U16BIT (_DECL *aceGetCoreVersion)
    (
        void
    );

    S16BIT (_DECL *aceGetSwVersionInfo)
    (
        PSWVERSIONINFO pSwVersionInfo
    );

    S16BIT (_DECL *aceGetHwVersionInfo)
    (
        S16BIT DevNum,
        PHWVERSIONINFO pHwVersionInfo
    );

    S16BIT (_DECL *aceInt80Enable)
    (
        S16BIT DevNum,
        U16BIT bEnable
    );

    S16BIT (_DECL *aceSetAsyncIsr)
    (
        S16BIT DevNum,
        void (_DECL * funcAsyncIsr)
        (
            S16BIT DevNum,
            U16BIT wMnrFrmId
        )
    );

    S16BIT (_DECL *aceGetCardType)
    (
        S16BIT DevNum,
        U32BIT *dwCardType
    );

    U32BIT (_DECL *aceRegRead32)
    (
        S16BIT DevNum,
        U16BIT wRegAddr
    );

    S16BIT (_DECL *aceRegWrite32)
    (
        S16BIT DevNum,
        U16BIT wRegAddr,
        U32BIT wValue
    );

    U32BIT (_DECL *aceMemRead32)
    (
        S16BIT DevNum,
        U32BIT wMemAddr
    );

    S16BIT (_DECL *aceMemWrite32)
    (
        S16BIT DevNum,
        U32BIT wMemAddr,
        U32BIT wValue
    );

    S16BIT (_DECL *aceIOInitialize)
    (
        S16BIT DevNum,
        S16BIT wOptions
    );

    S16BIT (_DECL *aceIOFree)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceGetChannelCount)
    (
        S16BIT DevNum,
        CHANCOUNT_t *pChanCount
    );

    S16BIT (_DECL *aceGetIRIGTx)
    (
        S16BIT DevNum,
        PACE_IRIG_TX pstructIRIG
    );

    S16BIT (_DECL *aceSetIRIGTx)
    (
        S16BIT DevNum,
        ACE_IRIG_TX structIRIG
    );

    S16BIT (_DECL *acexGetCoupling)
    (
        S16BIT DevNum,
        CHANNEL_COUPLING *pwCoupling,
        CHANNEL_TERMINATION *pwTermination
    );

    S16BIT (_DECL *acexSetCoupling)
    (
        S16BIT DevNum,
        CHANNEL_COUPLING eCoupling,
        CHANNEL_TERMINATION eTermination
    );

    S16BIT (_DECL *acexGetAmplitude)
    (
        S16BIT DevNum,
        U32BIT *pu32Amplitude
    );

    S16BIT (_DECL *acexSetAmplitude)
    (
        S16BIT DevNum,
        U32BIT u32Amplitude
    );

    S16BIT (_DECL *acexComponentRegRead)
    (
        S16BIT s16DevNum,
        U16BIT u16RegAddrOffset,
        U16BIT u16Length,
        U32BIT *pData,
        ACEX_COMPONENT eComponent
    );

    S16BIT (_DECL *acexEITxShutdownEnable)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *acexEITxShutdownDisable)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceSetHubAddress)
    (
        S16BIT DevNUm,
        U16BIT wHubNum
    );

    U32BIT (_DECL *acexRegRead)
    (
        S16BIT DevNum,
        U32BIT dwRegAddr,
        U32BIT dwRegType
    );

    S32BIT (_DECL *acexRegWrite)
    (
        S16BIT DevNum,
        U32BIT dwRegAddr,
        U32BIT dwValue,
        U32BIT dwRegType
    );

    S16BIT (_DECL *aceSetRemoteAccessDebug)
    (
        S16BIT DevNum
    );

    S16BIT (_DECL *aceRemoteDebugRegRead)
    (
        S16BIT DevNum,
        U32BIT wRegAddr,
        U32BIT dwRegType,
        U32BIT *pData
    );

    S16BIT (_DECL *aceRemoteDebugRegWrite)
    (
        S16BIT DevNum,
        U32BIT u32Address,
        U32BIT u32RegType,
        U32BIT u32Value
    );

    S16BIT (_DECL *aceRemoteDebugMemRead)
    (
        S16BIT DevNum,
        U32BIT u32Address,
        U32BIT *pu32Data
    );

    S16BIT (_DECL *aceRemoteDebugMemWrite)
    (
        S16BIT DevNum,
        U32BIT u32Address,
        U32BIT u32Value
    );

    S16BIT (_DECL *aceRemoteDebugFlashInfo)
    (
        S16BIT DevNum,
        PFLASH_INFO pFlashInfo
    );

    S16BIT (_DECL *aceRemoteDebugFlashRead)
    (
        S16BIT DevNum,
        U32BIT u32Address,
        U32BIT *pu32Data
    );

    S16BIT (_DECL *aceRemoteDebugGetChCount)
    (
        S16BIT DevNum,
        PENHANCED_CAPABILITY_INFO pEnhancedCardCapabilities
    );

    S16BIT (_DECL *aceGetBoardStatus)
    (
        S16BIT DevNum,
        U32BIT cmd,
        void *pdata
    );

    S16BIT (_DECL *acexConfiguration)
    (
        S16BIT s16DevNum, 
        U32BIT cmd, 
        void *pdata
    );
    
    S16BIT (_DECL *aceDebug)
    (
        S16BIT s16DevNum,
        U32BIT cmd,
        void *pdata
    );

    S16BIT (_DECL *aceSetBoardFeature)
    (
        S16BIT s16DevNum,
        U32BIT u32Command,
        void *pData
    );

} API_CONFIG_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN U16BIT _DECL aceGetLibVersion
(
    void
);

_EXTERN U16BIT _DECL aceGetCoreVersion
(
    void
);

_EXTERN S16BIT _DECL aceGetSwVersionInfo
(
    PSWVERSIONINFO pSwVersionInfo
);

_EXTERN S16BIT _DECL aceGetHwVersionInfo
(
    S16BIT DevNum,
    PHWVERSIONINFO pHwVersionInfo
);

_EXTERN S16BIT _DECL aceGetMemRegInfo
(
    S16BIT DevNum,
    U32BIT* pRegAddr,
    U32BIT* pMemAddr
);

_EXTERN S16BIT _DECL aceInitialize
(
    S16BIT DevNum,
    U16BIT wAccess,
    U16BIT wMode,
    U32BIT dwMemWrdSize,
    U32BIT dwRegAddr,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL aceFree
(
    S16BIT DevNum
);

_EXTERN U16BIT _DECL aceRegRead
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL aceRegWrite
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL aceRegRead32
(
    S16BIT DevNum,
    U16BIT wRegister
);

_EXTERN S16BIT _DECL aceRegWrite32
(
    S16BIT DevNum,
    U16BIT wRegAddr,
    U32BIT dwValue
);

_EXTERN U16BIT _DECL aceMemRead
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL aceMemWrite
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN U32BIT _DECL aceMemRead32
(
    S16BIT DevNum,
    U32BIT dwMemAddr
);

_EXTERN S16BIT _DECL aceMemWrite32
(
    S16BIT DevNum,
    U32BIT dwMemAddr,
    U32BIT dwValue
);

_EXTERN S16BIT _DECL aceSetAddressMode
(
    S16BIT DevNum,
    U16BIT wAddrMode
);

_EXTERN S16BIT _DECL aceSetTimeTagRes
(
    S16BIT DevNum,
    U16BIT wTTRes
);

_EXTERN S16BIT _DECL aceSetTimeTagValue
(
    S16BIT DevNum,
    U16BIT wTTValue
);

_EXTERN S16BIT _DECL aceGetTimeTagValue
(
    S16BIT DevNum,
    U16BIT* wTTValue
);

_EXTERN S16BIT _DECL aceSetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT ullTTValue
);

_EXTERN S16BIT _DECL aceGetTimeTagValueEx
(
    S16BIT DevNum,
    U64BIT* ullTTValue
);

_EXTERN S16BIT _DECL aceResetTimeTag
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceSetRespTimeOut
(
    S16BIT DevNum,
    U16BIT wRespTimeOut
);

_EXTERN S16BIT _DECL aceSetClockFreq
(
    S16BIT DevNum,
    U16BIT wClockIn
);

_EXTERN S16BIT _DECL aceSetIrqConfig
(
    S16BIT DevNum,
    U16BIT wLvlOrPulse,
    U16BIT wAutoClear
);

_EXTERN S16BIT _DECL aceSetDecoderConfig
(
    S16BIT DevNum,
    U16BIT wDoubleOrSingle,
    U16BIT wExpXingEnable
);

_EXTERN S16BIT _DECL aceSetIrqConditions
(
    S16BIT DevNum,
    U16BIT bEnable,
    U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    )
);

_EXTERN S16BIT _DECL aceSetRamParityChecking
(
    S16BIT DevNum,
    U16BIT wRamParityEnable
);

_EXTERN S16BIT _DECL aceISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL aceISQClear
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceISQEnable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL aceSetMetrics
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL aceInt80Enable
(
    S16BIT DevNum,
    U16BIT bEnable
);

_EXTERN S16BIT _DECL aceSetAsyncIsr
(
    S16BIT DevNum,
    void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    )
);

_EXTERN S16BIT _DECL aceSetCanIsr
(
    S16BIT DevNum,
    void (_DECL * funcCANIsr)
    (
        S16BIT DevNum,
        U16BIT wIrqStatus
    )
);

_EXTERN S16BIT _DECL aceSetHubAddress
(
    S16BIT DevNum,
    U16BIT wHubNum
);

_EXTERN S16BIT _DECL aceSetAsyncIsr
(
    S16BIT DevNum,
    void (_DECL * funcAsyncIsr)
    (
        S16BIT DevNum,
        U16BIT wMnrFrmId
    )
);

_EXTERN S16BIT _DECL aceGetCardType
(
    S16BIT DevNum,
    U32BIT *dwCardType
);

_EXTERN S16BIT _DECL aceGetBoardStatus
(
    S16BIT DevNum,
    U32BIT cmd,
    void *pdata
);

/* these initialize the discrete and avionic functions independent of 1553. */
_EXTERN S16BIT _DECL aceIOInitialize
(
    S16BIT DevNum,
    S16BIT wOptions
);

_EXTERN S16BIT _DECL aceIOFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceGetChannelCount
(
    S16BIT DevNum,
    CHANCOUNT_t *pChanCount
);

_EXTERN S16BIT _DECL aceGetIRIGTx
(
    S16BIT DevNum,
    PACE_IRIG_TX pstructIRIG
);

_EXTERN S16BIT _DECL aceSetIRIGTx
(
    S16BIT DevNum,
    ACE_IRIG_TX structIRIG
);

_EXTERN S16BIT _DECL acexGetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING *pwCoupling,
    CHANNEL_TERMINATION *pwTermination
);

_EXTERN S16BIT _DECL acexSetCoupling
(
    S16BIT DevNum,
    CHANNEL_COUPLING eCoupling,
    CHANNEL_TERMINATION eTermination
);

_EXTERN S16BIT _DECL acexGetAmplitude
(
    S16BIT DevNum,
    U32BIT *pu32Amplitude
);

_EXTERN S16BIT _DECL acexSetAmplitude
(
    S16BIT DevNum,
    U32BIT u32Amplitude
);

_EXTERN S16BIT _DECL acexComponentRegRead
(
    S16BIT s16DevNum,
    U16BIT u16RegAddrOffset,
    U16BIT u16Length,
    U32BIT *pData,
    ACEX_COMPONENT eComponent
);

/* EI transmitter shut down enable/disable functions */
_EXTERN S16BIT _DECL acexEITxShutdownEnable
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL acexEITxShutdownDisable
(
    S16BIT DevNum
);

/* EBR Board */
_EXTERN S16BIT _DECL aceSetHubAddress
(
    S16BIT DevNum,
    U16BIT wHubNum
);

_EXTERN U32BIT _DECL acexRegRead
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwRegType
);

_EXTERN S32BIT _DECL acexRegWrite
(
    S16BIT DevNum,
    U32BIT dwRegAddr,
    U32BIT dwValue,
    U32BIT dwRegType
);

/* Debug */
_EXTERN S16BIT _DECL aceSetRemoteAccessDebug
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRemoteDebugRegRead
(
    S16BIT DevNum,
    U32BIT wRegAddr,
    U32BIT dwRegType,
    U32BIT *pData
);

_EXTERN S16BIT _DECL aceRemoteDebugRegWrite
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT u32RegType,
    U32BIT u32Value
);

_EXTERN S16BIT _DECL aceRemoteDebugMemRead
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT *pu32Data
);

_EXTERN S16BIT _DECL aceRemoteDebugMemWrite
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT u32Value
);

_EXTERN S16BIT _DECL aceRemoteDebugFlashInfo
(
    S16BIT DevNum,
    PFLASH_INFO pFlashInfo
);

_EXTERN S16BIT _DECL aceRemoteDebugFlashRead
(
    S16BIT DevNum,
    U32BIT u32Address,
    U32BIT *pu32Data
);

_EXTERN S16BIT _DECL aceRemoteDebugGetChCount
(
    S16BIT DevNum,
    PENHANCED_CAPABILITY_INFO pEnhancedCardCapabilities
);

_EXTERN S16BIT _DECL acexConfiguration
(
    S16BIT s16DevNum, 
    U32BIT cmd, 
    void *pdata
);

/* internal debug use only */
_EXTERN S16BIT _DECL aceDebug
(
    S16BIT s16DevNum,
    U32BIT cmd,
    void *pdata
);

_EXTERN S16BIT _DECL aceSetBoardFeature
(
    S16BIT s16DevNum,
    U32BIT u32Command,
    void *pData
);

#endif /* __CONFIG_H__ */
