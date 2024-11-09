/*******************************************************************************
 * FILE: ddc_udl_1553_error_inj_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to support the Error Injection component.
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

#ifndef _DDC_UDL_1553_ERROR_INJ_PRIVATE_H_
#define _DDC_UDL_1553_ERROR_INJ_PRIVATE_H_

#include "os/include/ddc_os_types.h"


typedef struct _ACEX_1553_EI_TYPE
{
    U32BIT *pu32RegBA;                   /* ptr to 1553 EI (error injection) Registers base address */
    U32BIT *pu32RegSize;                 /* ptr to 1553 EI (error injection) Register size */
}ACEX_1553_EI_TYPE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern void errorInj1553Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

#endif /* _DDC_UDL_1553_ERROR_INJ_PRIVATE_H_ */
