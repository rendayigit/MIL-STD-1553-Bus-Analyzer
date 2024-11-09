/*******************************************************************************
 * FILE: ddc_os_version.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific Version information.
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
 * Copyright (c) 2020 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef _DDC_OS_VERSION_H_
#define _DDC_OS_VERSION_H_

/* ==================================================== */
/* ==================================================== */
/* Release Version Used in the drivers and libraries    */
/*                                                      */
/* X - Major    Y - Minor    Z - Patch    B - Build     */
/* ==================================================== */
/* ==================================================== */


/* ==================================================== */
/* Macro:   DDC_RELEASE_VERSION_X                       */
/*          DDC_RELEASE_VERSION_Y                       */
/*          DDC_RELEASE_VERSION_Z                       */
/*          DDC_RELEASE_VERSION_P                       */
/*                                                      */
/* Format:  Individual Version Numbers                  */
/* ==================================================== */

/* 
    IMPORTANT:

        * Version numbers are to be in DECIMAL format
        * Do NOT add any text after the version number
 */

#define DDC_RELEASE_VERSION_X       4
#define DDC_RELEASE_VERSION_Y       9
#define DDC_RELEASE_VERSION_Z       5
#define DDC_RELEASE_VERSION_B       1


#endif /* _DDC_OS_VERSION_H_ */
