/*******************************************************************************
 * FILE: ddc_udl_queueop_private.h
 *
 * DESCRIPTION:
 *
 *  This file contains definitions for the queue used by the DMA.
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

#ifndef _DDC_UDL_QUEUEOP_PRIVATE_H_
#define _DDC_UDL_QUEUEOP_PRIVATE_H_

#include "os/include/ddc_os_types.h"

/* Information about each queue */
typedef struct _Q_INFO_TYPE
{
    /* Base address of this queue */
    void *pQ;

    /* Size of completion queue (total # of queue entries). The queue is a circular buffer */
    U32BIT u32QueueSize;
    U32BIT u32SizeOfEntry;       /* size of each entry in the queue */

    U32BIT u32EntriesInQueue;    /* how full the queue is */
    U32BIT u32MaxEntriesInQueue; /* the maximum number of entries in queue */

    U32BIT u32NextRead;          /* (Head) */
    U32BIT u32LastWritten;       /* (Tail) */
} Q_INFO_TYPE;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

extern void *Q__INFO_TYPE_Add
(
    Q_INFO_TYPE *psHostQ
);

extern Q_INFO_TYPE *Q__INFO_TYPE_Create
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32SizeOfEntry,
    U32BIT u32NumQueEntries
);

extern S16BIT Q__INFO_TYPE_Destroy
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    Q_INFO_TYPE *psHostQ
);

extern void *Q__INFO_TYPE_Read
(
    Q_INFO_TYPE *psHostQ
);

extern void *Q__INFO_TYPE_Remove
(
    Q_INFO_TYPE *psHostQ
);

extern void Q__INFO_TYPE_Reset
(
    Q_INFO_TYPE *psHostQ
);

extern void Q__INFO_TYPE_Show
(
    Q_INFO_TYPE *psHostQ
);

extern void Q__INFO_TYPE_EntryShow
(
    Q_INFO_TYPE *psHostQ,
    U16BIT entry
);

#endif /* _DDC_UDL_QUEUEOP_PRIVATE_H_ */
