/*******************************************************************************
 * FILE: ddc_udl_can_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support CAN Bus
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

#ifndef _DDC_UDL_CAN_PRIVATE_H_
#define _DDC_UDL_CAN_PRIVATE_H_

#include "ddc_udl_can.h"

#include "driver_sdk/ddc_udl_private.h"
#include "core/1553/ddc_udl_1553_common_private.h"
#include "include/ddc_ioctl.h"
#include "driver_sdk/ddc_udl_sdk.h"


#define CAN_BUS_ERR_NONE                            DDC_UDL_ERROR__SUCCESS
#define CAN_BUS_ERROR_BASE                          -300
#define CAN_BUS_INVALID_SPEED                       CAN_BUS_ERROR_BASE
#define CAN_BUS_INVALID_CHANNEL                     (CAN_BUS_ERROR_BASE-1)
#define CAN_BUS_INVALID_STATE_ERR                   (CAN_BUS_ERROR_BASE-2)

#define CAN_BUS_DATA_UNAVAILABLE                    1
#define CAN_BUS_TX_BUSY                             2


/* Maximum number of CAN BUS channels supported */
#define MAX_NUM_CAN_CHANNELS                        16

#define CAN_BUS_CONFIG_OPTION_MEM_OFFSET            0x00
#define CAN_BUS_CONFIG_OPTION_RUN_STATE_OFFSET      0x01
#define CAN_BUS_CONFIG_OPTION_SPEED_OFFSET          0x02
#define CAN_BUS_CONFIG_OPTION_MSG_CNT_INT_OFFSET    0x03
#define CAN_BUS_CONFIG_OPTION_TIMER_INT_OFFSET      0x04
#define CAN_BUS_CONFIG_OPTION_TX_DEBUG_HEADER       0x05
#define CAN_BUS_CONFIG_OPTION_TX_DEBUG_ID           0x06
#define CAN_BUS_CONFIG_OPTION_TX_DEBUG_LENGTH       0x07
#define CAN_BUS_CONFIG_OPTION_TX_DEBUG_WORDS_1_4    0x08
#define CAN_BUS_CONFIG_OPTION_TX_DEBUG_WORDS_5_8    0x09
#define CAN_BUS_CONFIG_OPTION_MONITOR_STATE         0x0A
#define CAN_BUS_CONFIG_OPTION_LOOP_BACK_OFFSET      0x0B
#define CAN_BUS_CONFIG_OPTION_RX_FILTER_OFFSET      0x7F

/* Rx host buffer */
#define CAN_RX_HBUF_MAX_MSG_SIZE                    6   /* DWords in 1 CAN Message */
#define CAN_RX_QUEUE_MAX                            640 /* CAN Bus shared memory can hold max of 640 messages */
#define CAN_RX_HBUF_MAX                             0x8000 * CAN_RX_HBUF_MAX_MSG_SIZE

/*
 * Look in canBusInitialize for FW version change
 * Fixed Memory Size in FPGA only defined for 2 CAN channels
 * so if we have only one the current code will rearrange
 * memory to accommodate. For the 67118 design it's best to
 * assume 2 CAN BUS maximum to keep memory assignments the same.
 */
#define MaxCanBusChannelCount               2

typedef struct _CAN_BUS_CONFIG_STATE
{
    U8BIT u8Speed;
    U8BIT bLoopBack;
    
} CAN_BUS_CONFIG_STATE;


typedef struct _CAN_BUS_INTERRUPT
{
    U32BIT u32MasterStatus;
    U32BIT u32RxStatus[MAX_NUM_CAN_CHANNELS];

} CAN_BUS_INTERRUPT;


typedef struct _CAN_BUS_TYPE
{
    U32BIT *pu32RegBA;  /* ptr to CAN Bus Register base address */
    U32BIT *pu32RegSize; /* ptr to CAN Bus Register size */

    U32BIT *pu32MemSize; /* ptr to CAN Bus memory size */
    U32BIT *pu32MemBA;  /* ptr to CAN Bus memory base address */

    U32BIT u32MemRxBA[MAX_NUM_CAN_CHANNELS];
    U32BIT u32MemRxSize[MAX_NUM_CAN_CHANNELS];

    U32BIT u32MemTxBA[MAX_NUM_CAN_CHANNELS];
    U32BIT u32MemTxSize[MAX_NUM_CAN_CHANNELS];

    U32BIT u32MemRxIndex[MAX_NUM_CAN_CHANNELS];
    U32BIT u32MemTxIndex[MAX_NUM_CAN_CHANNELS];

    U32BIT u32MemConfig[MAX_NUM_CAN_CHANNELS];
    U32BIT u32MemStatus[MAX_NUM_CAN_CHANNELS];

    U32BIT u32FirmwareVersion;

    U32BIT u32FirmwareVersionAddress;

    ACEX_MOD_STATE state[MAX_NUM_CAN_CHANNELS];

} CAN_BUS_TYPE;


typedef struct _CAN_HBUF_TYPE
{
    U8BIT *hMemory;
    U8BIT *hUsbReadBuffer;
    U32BIT u32HbufNumEntries;
    U32BIT u32HbufNextRead;
    U32BIT u32HbufNextWrite;
    BOOLEAN bHbufStoreTimeTag;
    BOOLEAN bHbufOverflow;
    BOOLEAN bHbufQueueOverflow;
    DDC_ISR_LOCK_TYPE hMutex;
    unsigned long slCanModeFlag;

} CAN_HBUF_TYPE;


typedef struct _CAN_RX_HBUF_TYPE
{
    BOOLEAN bRxFiFoHbufInstalled;
    CAN_HBUF_TYPE sHBuf[MAX_NUM_CAN_CHANNELS];

} CAN_RX_HBUF_TYPE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern void canHandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *pu32IntStatus
);

extern S16BIT canBusInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT canBusOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
);

extern S16BIT canBusClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
);

extern S16BIT canBusSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
);

extern S16BIT canBusTransmitData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
);

extern S32BIT canBusReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pReadData,
    size_t *pOutputBufferLength
);

#endif /* _DDC_UDL_CAN_PRIVATE_H_ */
