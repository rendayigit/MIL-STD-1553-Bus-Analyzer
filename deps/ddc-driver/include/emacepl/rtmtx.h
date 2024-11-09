/*****************************************************************************
*       Data Device Corporation
*       105 Wilbur Place
*       Bohemia N.Y. 11716
*
*       AceXtreme 'C' Run Time Library
*
*       Copyright (c) 2008 Data Device Corporation
*       All Rights Reserved.
*
*       Filename: bcx.h
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
*       This is the rtmtx.h Acex header file.
*
*
******************************************************************************/

#ifndef __RTMTX_H__
#define __RTMTX_H__

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                            ACEX FUNCTION PROTOTYPES                             */
/* ========================================================================== */
_EXTERN S16BIT _DECL _acexRTMTConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
    U16BIT wMTStkType,
    U16BIT wMTCmdStkSize,
    U16BIT wMTDataStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _acexRTMTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTStkToHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwRTMTHBufSize
);

_EXTERN S16BIT _DECL _acexRTMTUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _acexRTMTGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwRTMsgLostStk,
    U32BIT *pdwMTMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _acexRTMTGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

#endif /* ifndef __RTMTX_H__ */
