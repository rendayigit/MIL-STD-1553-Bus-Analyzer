/*******************************************************************************
 * FILE: stdemace.h
 *
 * DESCRIPTION:
 *
 *  This header file contains all necessary information to access
 *  type definitions and RTL function prototypes.
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

#ifndef __STDEMACE_H__
#define __STDEMACE_H__

#define FEATURE_DDC_1553_EMACE
#define FEATURE_DDC_1553_ACEX
#define FEATURE_DDC_1553_REMOTE

/* Windows Declarations */
#ifdef WIN32
#include <windows.h>
#define _DECL   WINAPI
#define _EXTERN

/* Non Windows Declarations */
#else
#define _DECL
#define _EXTERN

#ifdef _MSC_VER
#define getvect _dos_getvect
#define setvect _dos_setvect
#define disable _disable
#define enable  _enable
#endif /* ifdef _MSC_VER */

#endif /* ifdef WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RTL is written in 'C' */
#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* UDL includes */
#include "os/include/ddc_os_types.h"
#include "include/ddc_types.h"
#include "driver_sdk/ddc_udl_sdk.h"
#include "include/ddc_ioctl.h"
#include "core/1553/ddc_udl_1553_bc.h"
#include "os/lib_support/ddc_os_lib_util.h"

#define MAX_NUM_OF_DEVICES  32

#define MAX_NUM_OF_MRT      31

/* define device I/O mode constant */
#define ACE_DRIVER_IO   0x80000000

/* define flex device constant */
#define ACE_FLEX_DEVICE 0x40000000

/* define avionics io capabilities constant */
#define ACE_AVIONICS_IO 0x20000000

/* define discrete io capabilites constant */
#define ACE_DISCRETE_IO 0x10000000

/* define AceXtreme device constant */
#define ACE_ACEX_DEVICE 0x08000000

/* define AceXtreme Coupling/Termination capability constant */
#define ACEX_COUPLING_TERMINATION_CAPABILITY 0x04000000

/* cardType Lower 16 - Device ID's */
#define ACE_CARD_5590           0x5590
#define ACE_CARD_7103           0x7103

/* define paging mechinisms */
#define ACE_65553_PAGING       1 /* Indicates that the card is using 65553 paging */
#define ACE_65568_PAGING       2 /* Indicates that the card is using 65568 paging */
#define ACE_6558x_PAGING       3 /* Indicates that the card is using 6558x paging */

/* Include all RTL header files necessary for external compiles */
#include "errordef.h"
#include "msgop.h"

#include "config.h"
#include "configop.h"
#include "configx.h"
#include "errorinj.h"
#include "trgx.h"
#include "trg.h"
#include "mtop.h"
#include "mtiop.h"
#include "rtop.h"
#include "rtx.h"
#include "rt.h"
#include "mrtx.h"
#include "mrt.h"
#include "rtmtop.h"
#include "rtmtx.h"
#include "rtmt.h"
#include "rtmtiop.h"
#include "rtmtix.h"
#include "rtmti.h"
#include "mtop.h"
#include "mtx.h"
#include "mt.h"
#include "mtiop.h"
#include "mtix.h"
#include "mti.h"
#include "bcop.h"
#include "bcx.h"
#include "bc.h"
#include "dioop.h"
#include "diox.h"
#include "dio.h"
#include "aioop.h"
#include "aiox.h"
#include "aio.h"
#include "testop.h"
#include "testx.h"
#include "test.h"
#include "aceop.h"
#include "acex.h"
#include "ace.h"

#include "lib1553_malloc.h"

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef __STDEMACE_H__ */
