/******************************************************************************
 * Data Device Corporation
 *
 * Filename: mtx.h
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
 *
 *  SF+ Monitor Terminal operations header file
 *
 *****************************************************************************/

#ifndef __MTX_H__
#define __MTX_H__

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

_EXTERN S16BIT _DECL _acexMTGetInfo
(
    S16BIT DevNum,
    MTINFO *pInfo
);

/* This function allows the MT to be reconfigured to the users liking */
_EXTERN S16BIT _DECL _acexMTConfigure
(
    S16BIT DevNum,
    U16BIT wMTStkType,
    U16BIT wCmdStkSize,
    U16BIT wDataStkSize,
    U32BIT dwOptions
);

/* These functions are used to enable/disable MT host buffers */
_EXTERN S16BIT _DECL _acexMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL _acexMTUninstallHBuf
(
    S16BIT DevNum
);

/* These functions are used to enable/disable MT sel. msg monitoring */
_EXTERN S16BIT _DECL _acexMTEnableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexMTDisableRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexMTGetRTFilter
(
    S16BIT DevNum,
    U16BIT wRT,
    U16BIT wTR,
    U32BIT *pSAMask
);

/* MT Run functions */
_EXTERN S16BIT _DECL _acexMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTContinue
(
    S16BIT DevNum
);

/* Onboard Stk functions */
_EXTERN S16BIT _DECL _acexMTGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U16BIT wStkLoc
);

_EXTERN S16BIT _DECL _acexMTGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc,
    U16BIT wStkLoc
);

/* Host buffer functions */
_EXTERN S16BIT _DECL _acexMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _acexMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _acexMTDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL _acexMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTClearHBufTrigger
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTSetHBufTrigger
(
    S16BIT DevNum,
    U16BIT wHBufPercent,
    MTTRIGGER *pTrg
);

_EXTERN S16BIT _DECL _acexMTISQRead
(
    S16BIT DevNum,
    ISQENTRY *pISQEntry
);

_EXTERN S16BIT _DECL _acexMTCreateImageFiles
(
    S16BIT DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL _acexMTSwapStks
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _acexMTGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT wStk,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _mtxFree
(
    S16BIT s16DevNum
);

#endif /* ifndef __MTX_H__ */
