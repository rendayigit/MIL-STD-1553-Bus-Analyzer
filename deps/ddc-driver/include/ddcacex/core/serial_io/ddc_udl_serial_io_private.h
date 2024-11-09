/*******************************************************************************
 * FILE: ddc_udl_serial_io_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support serialIO
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

#ifndef _DDC_UDL_SERIAL_IO_PRIVATE_H_
#define _DDC_UDL_SERIAL_IO_PRIVATE_H_

#include "ddc_udl_serial_io.h"

#include "driver_sdk/ddc_udl_private.h"
#include "core/1553/ddc_udl_1553_common_private.h"
#include "core/arinc429/ddc_udl_arinc429.h"
#include "include/ddc_ioctl.h"
#include "driver_sdk/ddc_udl_sdk.h"


/* Rx host buffer */
#define SERIAL_IO_RX_HBUF_MAX_MSG_SIZE                    1   /* Words in 1 Serial Message */
#define SERIAL_IO_RX_QUEUE_MAX                            640 /* SERIAL Bus shared memory can hold max of 640 messages */
#define SERIAL_IO_RX_HBUF_MAX                             0x8000 * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE

#define SERIAL_IO_INVALID_CHANNEL 255

typedef enum _CAST_SERIAL_PROTOCOL
{
    CAST_PROTOCOL_UART = 0,
    CAST_PROTOCOL_SDLC,
    CAST_PROTOCOL_HDLC

} CAST_SERIAL_PROTOCOL;  /* set the following in the protocol mode reg */


typedef struct _CAST_SERIAL_IO_CHAN_MATRIX
{
    ACEX_429_UART_TYPE serialAsyncUart;     /* Address       */
    ACEX_429_UART_TYPE serialSDLC;          /* Address       */
    ACEX_429_UART_TYPE serialHDLC;          /* Address       */
    CAST_SERIAL_PROTOCOL castSerialMode;

} CAST_SERIAL_IO_CHAN_MATRIX;

typedef struct _CAST_SERIAL_IO_HBUF_TYPE
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
    unsigned long slSerialIOModeFlag;
    /*unsigned long ulIntCount;*/ /* used for interrupt count check */
} CAST_SERIAL_IO_HBUF_TYPE;


typedef struct _CAST_SERIAL_IO_RX_HBUF_TYPE
{
    BOOLEAN bRxFiFoHbufInstalled[NUM_CAST_SIO_CHANNELS];
    CAST_SERIAL_IO_HBUF_TYPE sHBuf[NUM_CAST_SIO_CHANNELS];

} CAST_SERIAL_IO_RX_HBUF_TYPE;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern void serial_ioHandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel,
    U32BIT u32status
);

extern S16BIT serial_ioInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT serial_ioOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
);

extern S16BIT serial_ioClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
);


extern U16BIT serial_ioReadUART
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
);

U16BIT serial_ioReadUART_EXT
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, 
    U8BIT u8Channel, 
    U8BIT *pu8Buff,
    U32BIT u32BufSize
);

#if 0  
extern S32BIT canBusReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pReadData,
    size_t *pOutputBufferLength
);
#endif

#endif /* _DDC_UDL_SERIAL_IO_PRIVATE_H_ */
