#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * TEST.H (TEST MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for TEST.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    modified functions from Rel 1.6 STDACE.C that
 *                            relate to ACE testing and verification; added
 *                            ACE test vector processor
 *
 *   2.1       01-JUN-1995    modified BuVectorTest to pass file pointer
 *                            rather than filename to allows user program
 *                            to open and close the file. also removed
 *                            BU_TEST_FAILED_BADFILE and BU_TEST_CONTINUED
 *                            because they were no longer used.
 *
 *   2.2       01-SEP-1995    protocal to protocol spelling change, most
 *                            noticably the BuProtocolTest() function
 *
 *   2.3       01-JAN-1996    none
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       12-JAN-1997    added Bu_selftest() function which returns Pass/Fail
 *                            along with a character string pointer to the results
 *                            string
 *
 *   4.0       29-NOV-1997    none
 *
 *   5.3.1     11-APR-2003    none
 */


/* Test result structure */
typedef struct BuTest_s {
        U16BIT code;            /* pass or fail code     */
        U16BIT tdata;           /* expected data on fail */
        U16BIT data;            /* actual data on fail   */
        U16BIT addr;            /* address of failure    */
        U16BIT count;           /* test count index      */
} BuTest_t;


/* possible codes returned in test result structure code */
#define TEST_PASSED             0x00
#define TEST_FINISHED           0x02
#define TEST_FAILED_RAM         0x03
#define TEST_FAILED_REGISTER    0x04
#define TEST_FAILED_PROTOCOL    0x05
#define TEST_FAILED_INTERRUPT   0x06
#define TEST_FAILED_MVECTOR     0x08
#define TEST_FAILED_RVECTOR     0x09
#define TEST_FAILED_NOTOPEN     0x0A

/* Health Register - Only Used for BU-65580M1 EBR board */
#define REG_HEALTH_BIT			0x00DD

/* TEST FUNCTION PROTOTYPES -----------------------------------------------*/

/* basic true/false test for ACE */
__BUEXTERN U8BIT __BUDECL BuValid( void);

/* test ram of ACE */
__BUEXTERN void __BUDECL BuRamTest( U16BIT value,BuTest_t*BuTest);

/* test registers of ACE */
__BUEXTERN void __BUDECL BuRegisterTest( BuTest_t*BuTest);

/* test protocol hardware of ACE */
__BUEXTERN void __BUDECL BuProtocolTest( BuTest_t*BuTest);

/* test interrupt capibility of ACE */
__BUEXTERN void __BUDECL BuIrqTest( BuTest_t*BuTest);

/* test ram, registers, protocol, and interrupts of ACE */
__BUEXTERN void __BUDECL BuSelfTest( BuTest_t*BuTest);

/* test ram, registers, protocol, and interrupts of ACE  returns Pass/Fail
   and character string pointer to results string */
__BUEXTERN U16BIT __BUDECL Bu_selftest( char *error_msg);

/* vector test for ace: takes in absolute file name of test vector file */
__BUEXTERN void __BUDECL BuVectorTest( char *filename,BuTest_t*BuTest);

/* convert test type into string */
__BUEXTERN char* __BUDECL BuTestStr(U8BIT code);

/* Internal loopback test */
__BUEXTERN void __BUDECL BuLoopBackTestInt(BuTest_t*BuTest);

/* EBR loopback test */
__BUEXTERN void __BUDECL BuEBRLoopBackTest(BuTest_t*BuTest, U16BIT HubNumTx, U16BIT HubNumRx);

/*
 * END
 * TEST.H (TEST MODULE)
 */
#endif /*TEST_H_INCLUDED */

