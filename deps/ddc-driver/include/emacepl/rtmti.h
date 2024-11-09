/*******************************************************************************
 * FILE: rtmti.h
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

#ifndef __RTMTI_H__
#define __RTMTI_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_RTMTI_FUNCTIONS
{
    S16BIT (_DECL *aceRTMTIConfigure)
    (
        S16BIT DevNum,
        U16BIT wRTCmdStkSize,
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
    
    S16BIT (_DECL *aceRTMTIStart)
    (
        S16BIT DevNum
    );
    
    S16BIT (_DECL *aceRTMTIStop)
    (
        S16BIT DevNum
    );
    
} API_RTMTI_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceRTMTIConfigure
(
    S16BIT DevNum,
    U16BIT wRTCmdStkSize,
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

_EXTERN S16BIT _DECL aceRTMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL aceRTMTIStop
(
    S16BIT DevNum
);

#endif /* ifndef __RTMTI_H__ */
