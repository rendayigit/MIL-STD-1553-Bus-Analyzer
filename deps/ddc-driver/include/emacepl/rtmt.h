/*******************************************************************************
 * FILE: rtmt.h
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

#ifndef __RTMT_H__
#define __RTMT_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_RTMT_FUNCTIONS
{
    S16BIT (_DECL *aceRTMTConfigure)
    (
        S16BIT DevNum,
        U16BIT wRTCmdStkSize,
        U16BIT wMTStkType,
        U16BIT wMTCmdStkSize,
        U16BIT wMTDataStkSize,
        U32BIT dwOptions
    );
    
    S16BIT (_DECL *aceRTMTStart)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTStop)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTStkToHBuf)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTStkToHBuf32)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTInstallHBuf)
    (
        S16BIT DevNum,
        U32BIT dwRTMTHBufSize
    );
    
    S16BIT (_DECL *aceRTMTUninstallHBuf)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTGetHBufMsgCount)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTGetHBufMsgsRaw)
    (
        S16BIT DevNum,
        U16BIT *pBuffer,
        U16BIT wBufferSize,
        U32BIT *pdwMsgCount,
        U32BIT *pdwRTMsgLostStk,
        U32BIT *pdwMTMsgLostStk,
        U32BIT *pdwMsgLostHBuf
    );
    
    S16BIT (_DECL *aceRTMTGetHBufMsgDecoded)
    (
        S16BIT DevNum,
        MSGSTRUCT *pMsg,
        U32BIT *pdwMsgCount,
        U32BIT *pdwRTMsgLostStk,
        U32BIT *pdwMTMsgLostStk,
        U32BIT *pdwMsgLostHBuf,
        U16BIT wMsgLoc
    );
    
    S16BIT (_DECL *aceRTMTGetHBufMetric)
    (
        S16BIT DevNum,
        HBUFMETRIC *pMetric,
        U16BIT bReset
    );
    
} API_RTMT_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceRTMTConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
    U16BIT wMTStkType,
    U16BIT wMTCmdStkSize,
    U16BIT wMTDataStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL aceRTMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwRTMTHBufSize
);

_EXTERN S16BIT _DECL aceRTMTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL aceRTMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL aceRTMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

#endif /* ifndef __RTMT_H__ */
