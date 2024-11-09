/*******************************************************************************
 * FILE: test.h
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

#ifndef __TEST_H__
#define __TEST_H__

/* ========================================================================== */
/*                        FUNCTION POINTER PROTOTYPES                         */
/* ========================================================================== */
typedef struct _API_TEST_FUNCTIONS
{
    S16BIT (_DECL *aceTestRegisters)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestMemory)
    (
        S16BIT DevNum,
        TESTRESULT *pTest,
        U16BIT wValue
    );
    
    S16BIT (_DECL *aceTestIrqs)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestLoopBack)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestProtocol)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestVectors)
    (
        S16BIT DevNum,
        TESTRESULT *pTest,
        char *pFileName
    );
    
    S16BIT (_DECL *aceTestConfigure)
    (
        S16BIT DevNum,
        U32BIT dwOptions
    );
    
    S16BIT (_DECL *aceTestVectorsStatic)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestLoopBackInt)
    (
        S16BIT DevNum,
        TESTRESULT *pTest
    );
    
    S16BIT (_DECL *aceTestEBRLoopBack)
    (
        S16BIT DevNum,
        TESTRESULT *pTest,
        U16BIT wHubNumTx,
        U16BIT wHubNumRx
    );
    
} API_TEST_FUNCTIONS;

/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceTestRegisters
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestMemory
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wValue
);

_EXTERN S16BIT _DECL aceTestIrqs
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestProtocol
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestVectors
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    char *pFileName
);

_EXTERN S16BIT _DECL aceTestConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL aceTestVectorsStatic
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestLoopBackInt
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL aceTestEBRLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wHubNumTx,
    U16BIT wHubNumRx
);

#endif /* ifndef __TEST_H__ */
