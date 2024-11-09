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

#ifndef __RTMTIX_H__
#define __RTMTIX_H__

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                            ACEX FUNCTION PROTOTYPES                             */
/* ========================================================================== */
_EXTERN S16BIT _DECL _acexRTMTIConfigure
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

_EXTERN S16BIT _DECL _acexRTMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTMTIStop
(
    S16BIT DevNum
);

#endif /* ifndef __RTMTIX_H__ */
