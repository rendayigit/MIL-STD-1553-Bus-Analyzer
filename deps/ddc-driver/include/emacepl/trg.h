/*******************************************************************************
 * FILE: trg.h
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

#ifndef __TRG_H__
#define __TRG_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_TRG_FUNCTIONS
{
    S16BIT (_DECL *acexTRGReset)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexTRGConfigure)
    (
        S16BIT s16DevNum,
        U16BIT u16TrgId,
        ACEX_TRG_CONFIG sTrgConfig
    );
    
    S16BIT (_DECL *acexTRGEnable)
    (
        S16BIT s16DevNum,
        U16BIT u16TrgId
    );
    
    S16BIT (_DECL *acexTRGDisable)
    (
        S16BIT s16DevNum,
        U16BIT u16TrgId
    );
    
    S16BIT (_DECL *acexTRGEventSelect)
    (
        S16BIT s16DevNum,
        U16BIT u16TrgId,
        U16BIT u16Events,
        U16BIT u16EvtInput
    );
    
    S16BIT (_DECL *acexTRGEventEnable)
    (
        S16BIT s16DevNum,
        U16BIT u16Events
    );
    
    S16BIT (_DECL *acexTRGEventDisable)
    (
        S16BIT s16DevNum,
        U16BIT u16Events
    );
    
    S16BIT (_DECL *acexTRGGetTimeTag)
    (
        S16BIT s16DevNum,
        U64BIT *pu64TimeTagValue
    );
    
    S16BIT (_DECL *acexTRGGetStatus)
    (
        S16BIT s16DevNum,
        U32BIT *pu32TrgStatus,
        U32BIT *pu32IrqStatus
    );
    
} API_TRG_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL acexTRGReset
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexTRGConfigure
(
    S16BIT s16DevNum,
    U16BIT u16TrgId,
    ACEX_TRG_CONFIG sTrgConfig
);

_EXTERN S16BIT _DECL acexTRGEnable
(
    S16BIT s16DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL acexTRGDisable
(
    S16BIT s16DevNum,
    U16BIT u16TrgId
);

_EXTERN S16BIT _DECL acexTRGEventSelect
(
    S16BIT s16DevNum,
    U16BIT u16TrgId,
    U16BIT u16Events,
    U16BIT u16EvtInput
);

_EXTERN S16BIT _DECL acexTRGEventEnable
(
    S16BIT s16DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL acexTRGEventDisable
(
    S16BIT s16DevNum,
    U16BIT u16Events
);

_EXTERN S16BIT _DECL acexTRGGetTimeTag
(
    S16BIT s16DevNum,
    U64BIT *pu64TimeTagValue
);

_EXTERN S16BIT _DECL acexTRGGetStatus
(
    S16BIT s16DevNum,
    U32BIT *pu32TrgStatus,
    U32BIT *pu32IrqStatus
);

#endif /* __TRG_H__ */
