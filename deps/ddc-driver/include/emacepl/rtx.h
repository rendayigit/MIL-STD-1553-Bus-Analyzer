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
*       Filename: rtx.h
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
*       This is the rtx.h Acex header file.
*
*
******************************************************************************/

#ifndef __RTX_H__
#define __RTX_H__

/* ========================================================================== */
/*                            ACEX FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL _acexRTConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _acexRTModeCodeIrqEnable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL _acexRTModeCodeIrqDisable
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL _acexRTModeCodeIrqStatus
(
    S16BIT DevNum,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL _acexRTModeCodeReadData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL _acexRTModeCodeWriteData
(
    S16BIT DevNum,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL _acexRTMsgLegalityEnable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL _acexRTMsgLegalityDisable
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL _acexRTMsgLegalityStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwWC_MCMask
);

_EXTERN S16BIT _DECL _acexRTBusyBitsTblSet
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexRTBusyBitsTblClear
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexRTBusyBitsTblStatus
(
    S16BIT DevNum,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL _acexRTStatusBitsSet
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL _acexRTStatusBitsClear
(
    S16BIT DevNum,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL _acexRTStatusBitsStatus
(
    S16BIT DevNum,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL _acexRTDataBlkMapToSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL _acexRTDataBlkUnmapFromSA
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType
);

_EXTERN S16BIT _DECL _acexRTStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTBITWrdRead
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL _acexRTBITWrdWrite
(
    S16BIT DevNum,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL _acexRTCreateImageFiles
(
    S16BIT DevNum,
    char *pszIFile,
    char *pszHFile
);

_EXTERN S16BIT _DECL _acexRTBITWrdConfig
(
    S16BIT DevNum,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

#endif /* ifndef __RTX_H__ */
