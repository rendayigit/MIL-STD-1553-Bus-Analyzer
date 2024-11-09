/******************************************************************************
 * Data Device Corporation
 *
 * Filename: acex.h
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
 *  ACEX Translation layer operations header file
 *
 *****************************************************************************/

#ifndef __ACEX_H__
#define __ACEX_H__

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

_EXTERN S16BIT _DECL _acexBCOpCodeCreateEx
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL _acexBCOpCodeDeleteEx
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL _acexBCFrameCreateEx(  S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime,
    U16BIT wFlags);

_EXTERN S16BIT _DECL _acexBCFrameDeleteEx
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN U16BIT _DECL _acexMemReadEx
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN U32BIT _DECL _acexMemRead32Ex
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL _acexMemWriteEx
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN S16BIT _DECL _acexMTGetActiveStk
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexMTSetActiveStk
(
    S16BIT DevNum,
    U16BIT StkLoc
);

_EXTERN S16BIT _DECL _acexRTDataBlkCreateEx
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL _acexRTDataBlkDeleteEx
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

U32BIT _DECL _dvxSetRtDataBlkOffset
(
    S16BIT DevNum,
    U32BIT LookupTableAddr,
    U32BIT DataBlkBaseAddr,
    U16BIT Offset,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL _acexRTDataBlkSetCircBufOffsets
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT Offset
);

_EXTERN S16BIT _DECL _acexRTDataBlkGetAddr
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wBlkType,
    U16BIT wMsgType
);

_EXTERN S16BIT _DECL _acexRTAltStatusEna
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _acexRTBrdcst
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _acexRTIllegal
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _acexSetIrqConditionsEx(   S16BIT DevNum,
    U16BIT bEnable,
    U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    ));

_EXTERN S16BIT _DECL _acexSetValidBusyBitNoData
(
    S16BIT DevNum,
    U16BIT wValue
);                                                                             /* 10/24/11 MCS added to enable the Busy Bit Not Data */

#endif /* ifndef __ACEX_H__ */
