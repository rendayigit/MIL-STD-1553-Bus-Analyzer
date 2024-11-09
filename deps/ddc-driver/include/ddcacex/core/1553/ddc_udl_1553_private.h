/*******************************************************************************
 * FILE: ddc_udl_1553_private.h
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

#ifndef _DDC_UDL_1553_PRIVATE_H_
#define _DDC_UDL_1553_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "driver_sdk/ddc_udl_sdk.h"
#include "core/1553/ddc_udl_1553_bc_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "core/1553/ddc_udl_1553_imp_private.h"
#include "core/1553/ddc_udl_1553_error_inj_private.h"
#include "core/1553/ddc_udl_1553_trigger_private.h"


/* ========================================================================== */
/* ========================================================================== */


typedef struct _GEN_1553_REG_TYPE
{
    U32BIT u32IntEn;

} GEN_1553_REG;


typedef struct _ACEX_1553_CHANNEL_TYPE
{
    S16BIT wLogDevNum;

    U16BIT u16ChannelNum;           /* channel # for this instance */
    U32BIT *pu32MemSize;            /* pointer to UM memory size, in 32-bit */
    U32BIT *pu32MemBA;              /* pointer to UM memory base address, in 32-bit */
    U32BIT u32ChannelMemDwdMask;

    U32BIT u32UserMemSizeBytes;     /* memory size available to user to allocate memory blocks */
    U32BIT u32UserMemBA;            /* start address for memory available for user allocation */

    U32BIT *pu32RegSize;            /* pointer to 1553 Global Register size */
    U32BIT *pu32RegBA;              /* pointer to 1553 Global Registers base address */

    U32BIT u32IrqUsrEv;             /* Masked IRQ conditions */
    U32BIT u32IrqEv;                /* Return masked IRQ condition */

    U32BIT u32IrqAioInterruptMask;  /* Masked IRQ conditions */

    BOOLEAN bIsr1553Enabled;        /* whether the interrupt of 1553 channel is enabled */
    U32BIT au32UserIrqStatus;       /* interrupt status */

    DDC_ISR_LOCK_TYPE semIrqEventCond;
    DDC_ISR_FLAG_TYPE semIrqEventCondFlag;

    DDC_IRQ_STATUS_TYPE *pu32IrqStatusQ;    /* 1553 IRQ status queue */
    U32BIT u32IrqStatusQHead;       /* 1553 IRQ status queue head */
    U32BIT u32IrqStatusQTail;       /* 1553 IRQ status queue tail */

    DDC_CALLBACK waitqueueIrqCallback;
    DDC_EVENT waitqueueIrqEvent;        /* 1553 IRQ events wait queue */ /* IOCTL_1553_BLOCK_ON_IRQ will wait on this queue */
    
    U16BIT u16IrqEventCond;             /* 1553 IRQ event condition */

    DDC_EVENT waitqueueBlockOnIrqReadyEvent;    /* IOCTL_1553_BLOCK_ON_IRQ_READY will wait on this queue */

    U16BIT u16BlockOnIrqReadyEventCond;

    GEN_1553_REG sGen1553Reg;

    U32BIT u32RtGConfig;
    BOOLEAN bRtAutoBoot;
    U16BIT u16RtAddr;

    U32BIT u32ImpMemSizeDwds;
    struct _ACEX_1553_BC_TYPE sBC;
    struct _ACEX_1553_RT_TYPE sRT;
    struct _ACEX_1553_MT_TYPE sMT;

    struct _ACEX_1553_IMP_TYPE sImpBC;
    struct _ACEX_1553_IMP_TYPE sImpRT;

    struct _ACEX_1553_EI_TYPE sErrorInj;
    struct _ACEX_1553_REPLAY_TYPE sReplay;
    struct _ACEX_1553_TRIGGER_TYPE sTrigger;

} ACEX_1553_CHANNEL_TYPE;


typedef struct _ACEX_COUPLING
{
    U32BIT coupling;
    U32BIT termination;

} ACEX_COUPLING;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT ddcUdl1553ChannelInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdl1553ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT gen1553SetTimeTagResolution
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT gen1553SetTimeTagRolloverPoint
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT gen1553ExtTTCntCtrl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern U32BIT gen1553ChannelInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern S16BIT gen1553SetRamParityChecking
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT gen1553InterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
);

extern S16BIT gen1553InterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32IntMask
);

extern S16BIT gen1553MemClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32NumDwds,
    U32BIT u32Addr
);

extern S16BIT gen1553SetIRQ
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Mask,
    U16BIT u16Enable,
    U8BIT u8Channel
);

extern S16BIT gen1553CheckMfCapable
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT *pRdData
);

extern void gen1553GetCoupling
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    ACEX_COUPLING *pCoupling
);

extern void gen1553SetCoupling
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void gen1553GetAmplitude
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pAmplitude
);

extern void gen1553SetAmplitude
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

#endif /* _DDC_UDL_1553_PRIVATE_H_ */
