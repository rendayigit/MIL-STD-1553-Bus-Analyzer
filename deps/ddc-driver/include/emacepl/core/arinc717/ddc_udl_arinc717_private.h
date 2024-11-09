/*******************************************************************************
 * FILE: ddc_udl_arinc717_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support ARINC 717
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

#ifndef _DDC_UDL_ARINC717_PRIVATE_H_
#define _DDC_UDL_ARINC717_PRIVATE_H_

#include "ddc_udl_arinc717.h"

#define MAX_NUM_717_PROG_CHANNELS                   16

/* ========================================================================== */
/* ========================================================================== */
#define ARINC_717_PROGRAMMABLE_RESET                0x00001000

/* ==================================================== */
/* Growth capability max is 16 channels due to          */
/* limitation of Tx/Rx setting residing in 1 32-bit     */
/* register. A setting of 1 will place channel on       */
/* the bus.                                             */
/* ==================================================== */
#define ARINC_717_PROGRAMMABLE_BUS_ISOLATION_ENABLE 0x00000001

#define ARINC_717_PROGRAMMABLE_GLOBAL_CH_INT_ENABLE 0x00000001

/* ==================================================== */
/* Auto Detect Status Register Constants                */
/* ==================================================== */

/* Auto Detect Speed locked, bit 4 of register */
#define ARINC_717_PROG_AUTO_DETECT_SPEED_LOCKED_SHIFT   0x4
#define ARINC_717_PROG_AUTO_DETECT_SPEED_LOCKED_MASK    0x1

/* Auto Detect Speed value, bit [3:0] of register */
#define ARINC_717_PROG_AUTO_DETECT_SPEED_VALUE_SHIFT    0x0
#define ARINC_717_PROG_AUTO_DETECT_SPEED_VALUE_MASK     0xF

/* ==================================================== */
/* Transmitter Frame Count Register Constants           */
/* ==================================================== */

/* Auto Detect Speed value, bit [3:0] of register.  A value of 0 loops frames forever */
#define ARINC_717_PROG_TX_FRAME_COUNT_SHIFT             0x0
#define ARINC_717_PROG_TX_FRAME_COUNT_MASK              0xF

typedef struct _ARINC_717_PROG_TYPE
{
    U8BIT *pu8MemoryBuf;
    U32BIT *pu32RegBA;      /* ptr to 717 Registers base address */
    U32BIT *pu32RegSize;    /* ptr to 717 Register size */

    U32BIT *pu32MemSize;    /* ptr to 717 memory size */
    U32BIT *pu32MemBA;      /* ptr to 717 memory base address */

    ACEX_MOD_STATE state;

} ARINC_717_PROG_TYPE;


typedef enum _ARINC_717_PROG_CH_STATE
{
    ARINC_717_UNDEFINED,
    ARINC_717_TRANSMITTER,
    ARINC_717_RECEIVER,
    ARINC_717_LOOPBACK    /* channel selftest; loopback unto itself */

} ARINC_717_PROG_CH_STATE;


typedef struct _ARINC_717_PROG_MEM
{
    U8BIT u8SlopeRate;
    U8BIT u8Speed;
    U8BIT u8Protocol;
    U8BIT u8BufMode;
    U8BIT u8RxAutoDetect;
    U8BIT u8ChannelType;
    U8BIT u8WrapAroundMode;
    U16BIT u16FrameCount;

} ARINC_717_PROG_MEM;


typedef struct _ARINC_717_INTERRUPT
{
    U32BIT u32MasterStatus;
    U32BIT u32General717;                 /* 717 general register value */
    U32BIT u32RxStatus[MAX_NUM_717_PROG_CHANNELS];

} ARINC_717_INTERRUPT;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT arinc717Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlARINC717ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT arinc717ProgrammableOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
);

extern S16BIT arinc717ProgrammableClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
);

extern S16BIT arinc717ProgrammableSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig
);

extern S16BIT arinc717Interrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PARINC_717_PROGRMMABLE_CONFIG pConfig,
    U32BIT *pRdData,
    U8BIT bConfig
);

extern S16BIT ARINC717LoadTxQueueData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
);

extern S16BIT ARINC717ReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pRdData,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

#endif /* _DDC_UDL_ARINC717_PRIVATE_H_ */
