/*******************************************************************************
 * FILE: ddc_udl_queueop.c
 *
 * DESCRIPTION:
 *
 *  Queue Operation Routines
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

#include "os/include/ddc_os_types.h"
#include "os/include/ddc_os_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_queueop_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"

#define bzero(buf, size) memset((void*)(buf), 0, (size))

/* -------------------------------------------------------------------------- */
/*  Q routines - NOTE: THERE IS NO QUEUE PROTOECTION BUILT IN TO              */
/*                      THESE ROUTINES.  THIS MUST BE ADDED BY                */
/*                      USER IF NEEDED                                        */
/* -------------------------------------------------------------------------- */

/******************************************************************************
 * Name:    Q__INFO_TYPE_Create
 *
 * Description:
 *      This function creates a queue using a contiguous block of memory.
 *      Using a contiguous block of memory is suitable for software and hard-
 *      ware access (via PCI, for example.
 *
 *      This function uses DMA-SAFE memory so that access by software and
 *      hardware is safe if needed.
 *
 *      The first entry in the queue is actually the queue management
 *      variables, but this is abstracted from the user.
 *
 * In   u32SizeOfEntry
 * In   u32NumQueEntries
 * Out  none
 *
 * Returns: pointer to the queue, or NULL if the queue cannot be created.
 *****************************************************************************/
Q_INFO_TYPE *Q__INFO_TYPE_Create
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32SizeOfEntry,
    U32BIT u32NumQueEntries
)
{
    Q_INFO_TYPE *pHostQ = NULL;

    /* Malloc Q on the host (in bytes) */
    pHostQ = (Q_INFO_TYPE *)DDC_KERNEL_MALLOC(pDeviceContext, (u32SizeOfEntry * u32NumQueEntries) + sizeof(Q_INFO_TYPE));

    /* Verify malloc */
    if (pHostQ == NULL)
    {
        return NULL;
    }

    bzero((char *)pHostQ, ((u32SizeOfEntry * u32NumQueEntries) + sizeof(Q_INFO_TYPE)));

    /* Save parameters in Q_INFO */
    pHostQ->u32QueueSize = u32NumQueEntries;
    pHostQ->u32SizeOfEntry = u32SizeOfEntry;
    pHostQ->u32EntriesInQueue = 0;
    pHostQ->u32MaxEntriesInQueue = 0;
    pHostQ->u32NextRead = 0;
    pHostQ->u32LastWritten = 0;

    /* first area is the Q_INFO_TYPE struct the rest is the actual queue */
    pHostQ->pQ = (void *)(pHostQ + 1);

    /* Return Success */
    return pHostQ;
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Destroy
 *
 * Description:
 *
 * In   psHostQ
 * Out  none
 *
 * Returns: success
 *****************************************************************************/
S16BIT Q__INFO_TYPE_Destroy
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    Q_INFO_TYPE *psHostQ
)
{
    /* if it is already destroyed, just return success for now */
    if (psHostQ == NULL)
    {
        return DDC_UDL_ERROR__SUCCESS;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_DESTROY, "Q__INFO_TYPE_Destroy : psHostQ->u32MaxEntriesInQueue =%d\n", psHostQ->u32MaxEntriesInQueue);

    DDC_KERNEL_FREE(pDeviceContext, psHostQ);

    psHostQ = NULL;

    /* Return Success */
    return (DDC_UDL_ERROR__SUCCESS);
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Get_Tail
 *
 * Description:
 *      Get the tail of the queue.
 *
 * In   psHostQ
 * Out  none
 *
 * Returns: pointer to cq entry (so caller can fill in required fields),
 *          or NULL if entry cannot be added.
 *****************************************************************************/
void *Q__INFO_TYPE_Get_Tail
(
    Q_INFO_TYPE *psHostQ
)
{
    void *pQEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_GET_TAIL, "Q__INFO_TYPE_Get_Tail : psHostQ->u32QueueSize =0x%x\n", psHostQ->u32QueueSize);
    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_GET_TAIL, "Q__INFO_TYPE_Get_Tail : psHostQ->u32EntriesInQueue =0x%x\n", psHostQ->u32EntriesInQueue);

    /* get Free Q entry */
    pQEntry = (void *)(((S8BIT *)psHostQ->pQ) + (psHostQ->u32LastWritten * psHostQ->u32SizeOfEntry));

    return pQEntry;
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Add
 *
 * Description:
 *      Add to the end of the queue.
 *
 * In   psHostQ
 * Out  none
 *
 * Returns: pointer to cq entry (so caller can fill in required fields),
 *          or NULL if entry cannot be added.
 *****************************************************************************/
void *Q__INFO_TYPE_Add
(
    Q_INFO_TYPE *psHostQ
)
{
    void *pQEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_ADD, "Q__INFO_TYPE_Add : psHostQ->u32QueueSize =0x%x\n", psHostQ->u32QueueSize);
    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_ADD, "Q__INFO_TYPE_Add : psHostQ->u32EntriesInQueue =0x%x\n", psHostQ->u32EntriesInQueue);

    if (psHostQ->u32EntriesInQueue >= psHostQ->u32QueueSize)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_ADD, "Q__INFO_TYPE_Add : error condition \n");
        return NULL;
    }

    /* get Free Q entry */
    pQEntry = (void *)(((S8BIT *)psHostQ->pQ) + (psHostQ->u32LastWritten * psHostQ->u32SizeOfEntry));

    if (psHostQ->u32LastWritten == (psHostQ->u32QueueSize - 1))
    {
        psHostQ->u32LastWritten = 0;
    }
    else
    {
        psHostQ->u32LastWritten++;
    }

    psHostQ->u32EntriesInQueue++;

    if (psHostQ->u32EntriesInQueue > psHostQ->u32MaxEntriesInQueue)
    {
        psHostQ->u32MaxEntriesInQueue = psHostQ->u32EntriesInQueue;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_ADD, "Q__INFO_TYPE_Add : psHostQ->u32EntriesInQueue =0x%x\n", psHostQ->u32EntriesInQueue);

    return pQEntry;
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Read
 *
 * Description:
 *      Read the current entry off the queue without removing it.
 *
 * In   psHostQ
 * Out  none
 *
 * Returns: TRUE when data transfer completed
 *****************************************************************************/
void *Q__INFO_TYPE_Read
(
    Q_INFO_TYPE *psHostQ
)
{
    if ((psHostQ->u32NextRead == psHostQ->u32LastWritten) && (psHostQ->u32EntriesInQueue == 0)) /* Q is empty */
    {
        return NULL;
    }

    return (void *)(((S8BIT *)psHostQ->pQ) + (psHostQ->u32NextRead * psHostQ->u32SizeOfEntry));
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Remove
 *
 * Description:
 *      Remove entry from the queue.
 *
 * In   psHostQ
 * Out  none
 *
 * Returns: Null  queue was empty, otherwise a pointer to the entry
 *          removed.  The pointer should not be used by the caller, as the
 *          entry is now free and can be overwritten.
 *****************************************************************************/
void *Q__INFO_TYPE_Remove
(
    Q_INFO_TYPE *psHostQ
)
{
    void *pQEntry;

    if ((psHostQ->u32NextRead == psHostQ->u32LastWritten) && (psHostQ->u32EntriesInQueue == 0)) /* Q is empty */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_REMOVE, "EMPTY Q\n");
        return NULL;
    }

    pQEntry = (void *)(((S8BIT *)psHostQ->pQ) + (psHostQ->u32NextRead * psHostQ->u32SizeOfEntry));

    if (psHostQ->u32NextRead >= (psHostQ->u32QueueSize - 1))
    {
        psHostQ->u32NextRead = 0;
    }
    else
    {
        psHostQ->u32NextRead++;
    }

    psHostQ->u32EntriesInQueue--;

    return (void *)pQEntry;
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Reset
 *
 * Description:
 *
 * In   pHostQ
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void Q__INFO_TYPE_Reset
(
    Q_INFO_TYPE *pHostQ
)
{
    /* Clear parameters in Q_INFO */
    pHostQ->u32NextRead = 0;
    pHostQ->u32LastWritten = 0;
    pHostQ->u32EntriesInQueue = 0;
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_Show
 *
 * Description:
 *
 * In   pHostQ
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void Q__INFO_TYPE_Show
(
    Q_INFO_TYPE *pHostQ
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_SHOW,
        " **** Q Show ****\n"
        " QueueSize:%d   SizeOfEachEntry:%d \n"
        " NumEntriesUsed:%d    NR:%d  NW:%d\n"
        " pHostQ Addr:0x%p   pQ Addr:0x%p\n",
        pHostQ->u32QueueSize,
        pHostQ->u32SizeOfEntry,
        pHostQ->u32EntriesInQueue,
        pHostQ->u32NextRead,
        pHostQ->u32LastWritten,
        pHostQ,
        pHostQ->pQ);
}

/******************************************************************************
 * Name:    Q__INFO_TYPE_EntryShow
 *
 * Description:
 *
 * In   psHostQ
 * In   entry
 * Out  none
 *
 * Returns: none
 *****************************************************************************/
void Q__INFO_TYPE_EntryShow
(
    Q_INFO_TYPE *psHostQ,
    U16BIT entry
)
{
    U32BIT i;
    U32BIT *ptr;

    DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_SHOW, " **** Q Entry Show ****\n" " Entry:%d\n", entry);

    ptr = (U32BIT *)(((S8BIT *)psHostQ->pQ) + (entry * psHostQ->u32SizeOfEntry));

    for (i = 0; i < (psHostQ->u32SizeOfEntry / 4); i++)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_QUEUE, DDC_DBG_QUEUE_SHOW, "field %d: 0x%08x\n", i, *(ptr + i));
    }
}
