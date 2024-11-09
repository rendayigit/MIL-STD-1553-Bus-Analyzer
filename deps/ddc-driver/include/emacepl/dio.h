/*******************************************************************************
 * FILE: dio.h
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

#ifndef __DIO_H__
#define __DIO_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */

typedef struct _API_DIO_FUNCTIONS
{
    S16BIT (_DECL *aceGetDiscOut)
    (
        S16BIT DevNum,
        S16BIT Discrete
    );

    S16BIT (_DECL *aceGetDiscDir)
    (
        S16BIT DevNum,
        S16BIT Discrete
    );

    S16BIT (_DECL *aceGetDiscIn)
    (
        S16BIT DevNum,
        S16BIT Discrete
    );

    S16BIT (_DECL *aceSetDiscOut)
    (
        S16BIT DevNum,
        S16BIT Discrete,
        DISC_LEVEL Level
    );

    S16BIT (_DECL *aceSetDiscDir)
    (
        S16BIT DevNum,
        S16BIT Discrete,
        DISC_DIR Direction
    );

    S16BIT (_DECL *aceGetDiscAll)
    (
        S16BIT DevNum,
        U16BIT *Directions,
        U16BIT *Levels
    );

    S16BIT (_DECL *aceSetDiscAll)
    (
        S16BIT DevNum,
        U16BIT Directions,
        U16BIT Levels
    );

    S16BIT (_DECL *acexSetDiscConfigure)
    (
        S16BIT s16DevNum,
        S16BIT s16Discrete,
        ACEX_DISC_CONFIG sDiscConfig
    );

    S16BIT (_DECL *acexClrDiscConfigure)
    (
        S16BIT s16DevNum
    );

    S16BIT (_DECL *acexDioTtCfg)
    (
        S16BIT s16DevNum,
        DIO_TT_CFG *psDioCfg,
        void (_DECL * pfDioTtIsr)
        (
            S16BIT DevNum,
            U32BIT u32IntSts
        )
    );

    S16BIT (_DECL *acexDioTtCtl)
    (
        S16BIT s16DevNum,
        U32BIT u32DioTtCtl
    );

    S16BIT (_DECL *acexDioTtRead)
    (
        S16BIT s16DevNum,
        U8BIT *pu8Data,
        U32BIT *pu32Bytes
    );

} API_DIO_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceGetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL aceSetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_DIR Direction
);

_EXTERN S16BIT _DECL aceGetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL aceGetDiscIn
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL aceSetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_LEVEL Level
);

_EXTERN S16BIT _DECL aceGetDiscAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL aceSetDiscAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

_EXTERN S16BIT _DECL acexSetDiscConfigure
(
    S16BIT s16DevNum,
    S16BIT s16Discrete,
    ACEX_DISC_CONFIG sDiscConfig
);

_EXTERN S16BIT _DECL acexClrDiscConfigure
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexDioTtCfg
(
    S16BIT s16DevNum,
    DIO_TT_CFG * psDioCfg,
    void (_DECL * pfDioTtIsr)
    (
        S16BIT DevNum,
        U32BIT u32IntSts
    )
);

_EXTERN S16BIT _DECL acexDioTtCtl
(
    S16BIT s16DevNum,
    U32BIT u32DioTtCtl
);

_EXTERN S16BIT _DECL acexDioTtRead
(
    S16BIT s16DevNum,
    U8BIT *pu8Data,
    U32BIT *pu32Bytes
);

#endif /* ifndef __DIO_H__ */
