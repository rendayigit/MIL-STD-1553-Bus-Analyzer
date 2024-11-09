/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 2006 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  aceop.h (EMACE Translation Layer)
 *
 *
 */

#ifndef __ACEOP_H__
#define __ACEOP_H__

_EXTERN S16BIT _DECL _aceBCOpCodeCreateEx
(
    S16BIT DevNum,
    S16BIT nOpCodeID,
    U16BIT wOpCodeType,
    U16BIT wCondition,
    U32BIT dwParameter1,
    U32BIT dwParameter2,
    U32BIT dwReserved
);

_EXTERN S16BIT _DECL _aceBCOpCodeDeleteEx
(
    S16BIT DevNum,
    S16BIT nOpCodeID
);

_EXTERN S16BIT _DECL _aceBCFrameCreateEx(   S16BIT DevNum,
    S16BIT nFrameID,
    U16BIT wFrameType,
    S16BIT aOpCodeIDs[],
    U16BIT wOpCodeCount,
    U16BIT wMnrFrmTime,
    U16BIT wFlags);

_EXTERN S16BIT _DECL _aceBCFrameDeleteEx
(
    S16BIT DevNum,
    S16BIT nFrameID
);

_EXTERN U16BIT _DECL _aceMemReadEx
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN U32BIT _DECL _aceMemRead32Ex
(
    S16BIT DevNum,
    U16BIT wMemAddr
);

_EXTERN S16BIT _DECL _aceMemWriteEx
(
    S16BIT DevNum,
    U16BIT wMemAddr,
    U16BIT wValue
);

_EXTERN S16BIT _DECL _aceMTGetActiveStk
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTSetActiveStk
(
    S16BIT DevNum,
    U16BIT StkLoc
);

_EXTERN S16BIT _DECL _aceRTDataBlkCreateEx
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wDataBlkType,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL _aceRTDataBlkDeleteEx
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

U32BIT _DECL _dvSetRtDataBlkOffset
(
    S16BIT DevNum,
    U16BIT LookupTableAddr,
    U32BIT DataBlkBaseAddr,
    U16BIT Offset,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL _aceRTDataBlkSetCircBufOffsets
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT Offset
);

_EXTERN S16BIT _DECL _aceRTDataBlkGetAddr
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wBlkType,
    U16BIT wMsgType
);

_EXTERN S16BIT _DECL _aceRTAltStatusEna
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _aceRTBrdcst
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _aceRTIllegal
(
    S16BIT DevNum,
    U16BIT Value
);

_EXTERN S16BIT _DECL _aceSetIrqConditionsEx(    S16BIT DevNum,
    U16BIT bEnable,
    U32BIT dwIrqMask,
    void (_DECL * funcExternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    ));

_EXTERN S16BIT _DECL _aceSetValidBusyBitNoData
(
    S16BIT DevNum,
    U16BIT wValue
);

#endif /* ifndef __ACEOP_H__ */
