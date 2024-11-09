/*******************************************************************************
 * FILE: ddc_udl_os_bus_private.h
 *                    
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific macros and structures
 *  for a generic bus.
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

#ifndef _DDC_UDL_OS_BUS_PRIVATE_H_
#define _DDC_UDL_OS_BUS_PRIVATE_H_

#include <linux/pci.h>
#include "os/include/ddc_os_types.h"


#define DRIVER_NAME     "ACEXPCI"


#define DDC_DMA_MALLOC(pDC, size, addr, mem_rgn) \
    dma_alloc_coherent( \
        &pDC->ddcOsDevInfo.sBusInfo.pPciDev->dev, \
        size, \
        addr, \
        GFP_KERNEL)

#define DDC_DMA_FREE(pDC, size, target, addr, mem_rgn) \
    dma_free_coherent( \
        &pDC->ddcOsDevInfo.sBusInfo.pPciDev->dev, \
        size, \
        target, \
        addr); \
    target = NULL;

#define DDC_MEMORY_REGION__BC_REPLAY_DMA            0
#define DDC_MEMORY_REGION__RT_DMA                   1
#define DDC_MEMORY_REGION__MT_DMA                   2
#define DDC_MEMORY_REGION__DIO_TT_DMA               3
#define DDC_MEMORY_REGION__ARINC_429_RX_DMA         4
#define DDC_MEMORY_REGION__ARINC_429_TX_DMA         5
#define DDC_MEMORY_REGION__ARINC_429_VOLT_MON_DMA   6
#define DDC_MEMORY_REGION__ARINC_717_RX_DMA         7
#define DDC_MEMORY_REGION__ARINC_717_TX_DMA         8

struct DDC_OS_BUS_INFO
{
    struct pci_dev *pPciDev;

    U16BIT u16PciBusNum;
    U16BIT u16PciDevNum;

    /* Registers */
    void *pRegAddr;
    U32BIT u32RegLen;

    /* Memory */
    void *pMemAddr;
    U32BIT u32MemLen;

    /* PLX */
    void *pPlxAddr;
    U32BIT u32PlxLen;

    /* DMA */
    void *pDmaAddr;
    U32BIT u32DmaLen;

    U16BIT u16Irq;
};

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT ddcUdlOsBusInit
(
    void
);

extern void ddcUdlOsBusExit
(
    void
);

extern U32BIT ddcUdlOsBusGetInterruptStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u8RegisterType,
    U32BIT u32BaseAddress,
    U32BIT u32Offset
);

extern S16BIT ddcUdlOsBusPciMapAddress
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8AddressType,
    U8BIT u8BarType
);


void ddcUdlBusPciUnmapAddresses
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

/* ==================================================== */
/* Registers                                            */
/* ==================================================== */

extern S16BIT ddcUdlOsBusRegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData
);

extern S16BIT ddcUdlOsBusRegReadBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Count
);

extern S16BIT ddcUdlOsBusRegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData
);

extern S16BIT ddcUdlOsBusRegWriteBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Count
);

/* ==================================================== */
/* Memory                                               */
/* ==================================================== */

extern S16BIT ddcUdlOsBusMemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Mode
);

extern S16BIT ddcUdlOsBusMemReadBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Count,
    U32BIT u32Mode
);

extern S16BIT ddcUdlOsBusMemReadBlk16
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U16BIT *pu16RdData,
    U32BIT u32Count
);

extern S16BIT ddcUdlOsBusMemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Mode
);

extern S16BIT ddcUdlOsBusMemWriteBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Count,
    U32BIT u32Mode
);

extern S16BIT ddcUdlOsBusMemWriteBlk16
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U16BIT *pu16WrData,
    U32BIT u32WordCount
);

extern void ddcUdlOsBusReadPlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32ByteAddr,
    U32BIT *pu32Data
);

extern void ddcUdlOsBusWritePlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32ByteAddr,
    U32BIT u32Data
);

extern U32BIT ddcUdlOsBusAsicDmaRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr
);

extern void ddcUdlOsBusAsicDmaWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT Value
);

extern S16BIT ddcUdlOsBusPcieDmaWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DmaRegAddr,
    U32BIT *pu32RdData
);

extern DDC_DMA_ADDR ddcUldOsPciDmaMap
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pBufferAddress,
    U32BIT u32ByteCount,
    enum dma_data_direction eDirection
);

extern void ddcUldOsPciDmaUnmap
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_DMA_ADDR dmaAddr,
    U32BIT u32ByteCount,
    enum dma_data_direction eDirection
);

#endif /* _DDC_UDL_OS_BUS_PRIVATE_H_ */
