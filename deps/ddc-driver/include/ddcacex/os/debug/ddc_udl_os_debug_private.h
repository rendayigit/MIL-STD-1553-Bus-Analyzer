/*******************************************************************************
 * FILE: ddc_udl_os_debug_private.h
 *
 * DESCRIPTION:
 *
 *  This file provides the OS support to for driver debug printing.
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

#ifndef _DDC_UDL_OS_DEBUG_PRIVATE_H_
#define _DDC_UDL_OS_DEBUG_PRIVATE_H_


/* ========================================================================== */
/* DEBUG PRINT MACRO                                                          */
/* ========================================================================== */

/*
    Usage:  DDC_DBG_PRINT(module, mask, fmt, ...))

    module      Module ID
    mask        Debugging mask
    fmt         printf format string
    ...         extra arguments fmt requires (possibly none)
*/


/*
    The way this macro is defined will allow compiler to compile the debug macro
    even when debug printing is disabled
*/

#define DDC_DBG_PRINT_INFO(fmt, ...) \
    printk("[%s %s:%d] " fmt, DRIVER_NAME, __func__, __LINE__, ##__VA_ARGS__);

#define DDC_DBG_PRINT(module, mask, fmt, ...) \
    do \
    { \
        if (DDC_DBG_PRINT_ENABLED) \
        { \
            if (u32DebugTable[module] & mask) \
            { \
                printk("[%s %s:%d] " fmt, DRIVER_NAME, __func__, __LINE__, ##__VA_ARGS__); \
            } \
        } \
    } while (0)
#endif /* _DDC_UDL_OS_DEBUG_PRIVATE_H_ */
