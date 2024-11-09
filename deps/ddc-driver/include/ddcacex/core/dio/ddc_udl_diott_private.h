/*******************************************************************************
 * FILE: ddc_udl_diott_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide function definitions of the
 *  DIO Time Tag module interface.
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

#ifndef _DDC_UDL_DIOTT_PRIVATE_H_
#define _DDC_UDL_DIOTT_PRIVATE_H_

#include "core/1553/ddc_udl_1553_common_private.h"

#define DIOTT_IRQ_STATUS_QUEUE_SIZE                 128


typedef struct _DIO_TT_TYPE
{
    U8BIT *pu8DmaTarget;
    DDC_DMA_ADDR DmaAddr;
    
    U32BIT *pu32RegBA;    /* ptr to Dio Tt registers */
    U32BIT *pu32RegSize;  /* ptr to Dio Tt registers */
    U32BIT *pu32MemSize;  /* ptr to Dio Tt memory size */
    U32BIT *pu32MemBA;    /* ptr to Dio Tt memory base address */
    U32BIT u32Tail;       /* On board timestamp array tail */
    
    ACEX_MOD_STATE state;

    BOOLEAN bIsrEnabled;
    
    DDC_ISR_LOCK_TYPE semDmaEventCond;
    DDC_ISR_FLAG_TYPE semDmaEventCondFlag;
    U16BIT u16DmaCmpltEventCond;
    
    DDC_CALLBACK dmaCmpltWaitqueueCallback;
    DDC_EVENT dmaCmpltWaitqueueEvent;

    DDC_ISR_LOCK_TYPE semIrqEventCond;
    DDC_ISR_FLAG_TYPE semIrqEventCondFlag;
    
    U32BIT sIntStatus[DIOTT_IRQ_STATUS_QUEUE_SIZE];    /* Interrupt status array  */
    U32BIT u32IntQHead;
    U32BIT u32IntQTail;
    U16BIT u32IntQLen; /* IRQ event condition */
    
    DDC_CALLBACK waitqueueIrqCallback;
    DDC_EVENT waitqueueIrqEvent;
    
} DIO_TT_TYPE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT ddcUdlDioTtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlDioTtFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlDioTtCfg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT ddcUdlDioTtCtl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT ddcUdlDioTtRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *pRdData,
    size_t OutputBufferLength
);

#endif /* _DDC_UDL_DIOTT_PRIVATE_H_ */
