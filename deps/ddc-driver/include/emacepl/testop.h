/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 1999 by Data Device Corporation
 *          All Rights Reserved.
 *
 *
 *  Testop.h (RTL test operations header file)
 *
 *
 *
 *  Created 8/26/99 DL
 *
 */

#ifndef __TESTOP_H__
#define __TESTOP_H__

/*-------------------------------------------------------------
Struct: TESTOPERATION

Description:
    This structure completely describes Test mode of operation.
---------------------------------------------------------------*/
typedef struct TESTOPERATION
{
    U32BIT dwOptions;       /* Test Options */
} TESTOPERATION;

/* Test result structure */
typedef struct TESTRESULT
{
    U16BIT wResult;     /* pass or fail code     */
    U16BIT wExpData;    /* expected data on fail */
    U16BIT wActData;    /* actual data on fail   */
    U16BIT wAceAddr;    /* address of failure    */
    U16BIT wCount;      /* test count index      */
} TESTRESULT;

_EXTERN S16BIT _DECL PrintError
(
    U32BIT ErrorCode
);

#define ACE_TEST_DEBUG 0

/* All possible result codes returned in test structure */
#define ACE_TEST_PASSED                 0x00
#define ACE_TEST_NOT_PERFORMED          0x01
#define ACE_TEST_FAILED_RAM             0x02
#define ACE_TEST_FAILED_REGISTER        0x03
#define ACE_TEST_FAILED_PROTOCOL        0x04
#define ACE_TEST_FAILED_INTERRUPT       0x05
#define ACE_TEST_FAILED_MVECTOR         0x06
#define ACE_TEST_FAILED_RVECTOR         0x07
#define ACE_TEST_FAILED_NOTOPEN         0x08
#define ACE_TEST_FAILED_CANEBR          0x09
#define ACE_TEST_FAILED_LOOPBACK        0x0A
#define ACE_TEST_FAILED_LOOPBACK_INT    0x0B
#define ACE_TEST_FAILED_LOOPBACK_EBR    0x0C

/* EBR Loopback definitions */
#define ACE_EBR_LOOPBACK_TX_A           0x0000
#define ACE_EBR_LOOPBACK_TX_B           0x0400
#define ACE_EBR_LOOPBACK_TX_C           0x0800
#define ACE_EBR_LOOPBACK_TX_D           0x0C00

#define ACE_EBR_LOOPBACK_RX_A           0x0000
#define ACE_EBR_LOOPBACK_RX_B           0x0100
#define ACE_EBR_LOOPBACK_RX_C           0x0200
#define ACE_EBR_LOOPBACK_RX_D           0x0300

_EXTERN S16BIT _DECL _aceTestConfigure
(
    S16BIT DevNum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _aceTestRegisters
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _aceTestMemory
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wValue
);

_EXTERN S16BIT _DECL _aceTestProtocol
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _aceTestIrqs
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _aceTestVectors
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    char *pFileName
);

_EXTERN S16BIT _DECL _aceTestVectorsStatic
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _aceTestCanEbrLoop
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S32BIT _DECL _aceTestGetCardTypeFromDevID
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceTestLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _flexTestVectors
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    char *pFileName
);

_EXTERN S16BIT _DECL _aceTestLoopBackInt
(
    S16BIT DevNum,
    TESTRESULT *pTest
);

_EXTERN S16BIT _DECL _aceTestEBRLoopBack
(
    S16BIT DevNum,
    TESTRESULT *pTest,
    U16BIT wHubNumTx,
    U16BIT wHubNumRx
);

#endif /* ifndef __TESTOP_H__ */
