/*****************************************************************************
*                       Data Device Corporation
*                         105 Wilbur Place
*                        Bohemia N.Y. 11716
*
*   Filename: axtestop.h
*
*   Copyright (c) 2008 Data Device Corporation
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
*       This is the include file for axtestop.c
*
*
******************************************************************************/

#ifndef __TESTX_H__
#define __TESTX_H__

_EXTERN S16BIT _DECL _acexTestRegisters
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _acexTestMemory
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wValue
);

_EXTERN S16BIT _DECL _acexTestIrqs
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _acexTestLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _acexTestProtocol
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _acexTestVectors
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    char *pFileName
);

_EXTERN S16BIT _DECL _acexTestConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _acexTestVectorsStatic
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

#endif /* ifndef __TESTX_H__ */
