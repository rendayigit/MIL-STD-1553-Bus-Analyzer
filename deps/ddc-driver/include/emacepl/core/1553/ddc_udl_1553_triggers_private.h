/*******************************************************************************
 * FILE: ddc_udl_1553_triggers_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to support the Triggers component.
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

#ifndef _DDC_UDL_1553_TRIGGERS_PRIVATE_H_
#define _DDC_UDL_1553_TRIGGERS_PRIVATE_H_

#include "driver_sdk/ddc_udl_private.h"
#include "include/ddc_ioctl.h"


#define ACEX_TRG_STATUS_ENTRY_NUM       100
#define ACEX_TRG_STATUS_ENTRY_DWORDS    (2 * ACEX_TRG_STATUS_ENTRY_NUM)

typedef struct _ACEX_1553_TRIGGER_TYPE
{
    U32BIT *pu32RegBA;             /* ptr to 1553 Trigger Registers base address */
    U32BIT *pu32RegSize;           /* ptr to 1553 Trigger Register size */

    U32BIT u32StatusEntries;               /* buffer entry number */
    U32BIT u32StatusDwords;                /* buffer u32ords */

    DDC_ISR_LOCK_TYPE slBuf;                /* Buf spin lock                               */
    DDC_ISR_FLAG_TYPE slBufFlag;            /* Buf int flag                                */

    U32BIT u32Count;                       /* counts in buffer */
    U32BIT u32Head;                        /* head index for user to read */
    U32BIT u32Tail;                        /* tail for driver to write */
    U32BIT *hBuf;                  /* trigger Interrupt status buffer */
    
} ACEX_1553_TRIGGER_TYPE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern S16BIT trigger1553Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

extern void trigger1553Free
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT trigger1553Reset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void trigger1553StatusWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT *pu32TrgStatus,
    U32BIT *pu32TrgInt
);

extern S16BIT trigger1553StatusRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern S16BIT mfTransceiverDelayUpdate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
);

#endif /* _DDC_UDL_1553_TRIGGERS_PRIVATE_H_ */
