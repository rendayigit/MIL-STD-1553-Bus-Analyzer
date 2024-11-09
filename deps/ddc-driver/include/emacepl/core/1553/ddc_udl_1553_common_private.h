/*******************************************************************************
 * FILE: ddc_udl_1553_common_private.h
 *
 * DESCRIPTION:
 *
 *  General MIL-STD-1553 Definitions.
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

#ifndef _DDC_UDL_1553_COMMON_PRIVATE_H_
#define _DDC_UDL_1553_COMMON_PRIVATE_H_

typedef enum _ACEX_MOD_STATE
{
    ACEX_MOD_RESET = 0,
    ACEX_MOD_CLOSED,
    ACEX_MOD_OPEN,
    ACEX_MOD_ERROR

} ACEX_MOD_STATE;

#endif /* _DDC_UDL_1553_COMMON_PRIVATE_H_ */
