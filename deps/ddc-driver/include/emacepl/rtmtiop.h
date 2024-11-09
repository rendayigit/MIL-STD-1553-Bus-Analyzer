/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 2007 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  RTMTIop.h (RT,Monitor Terminal IRIG operations header file)
 *
 *
 *
 *  Created 11/29/06 ESK
 *
 */
#ifndef __RTMTIOP_H__
#define __RTMTIOP_H__

#ifdef FEATURE_DDC_1553_EMACE

/**************** MTI FUNCTIONS **************************/
_EXTERN S16BIT _DECL _rtmtiSetDefaultConfig
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceRTMTIStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceRTMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceRTMTIConfigure
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
    U8BIT u8MTIHdrVer,
    U8BIT u8MTIRelAbsTime,
    U8BIT u8MTICh10Checksum,
    U32BIT dwOptions
);

#endif /* FEATURE_DDC_1553_EMACE */

#endif /* ifndef __RTMTIOP_H__ */
