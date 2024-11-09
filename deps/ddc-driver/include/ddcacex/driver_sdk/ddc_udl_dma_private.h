/*******************************************************************************
 * FILE: ddc_udl_dma_private.h
 *
 * DESCRIPTION:
 *
 *  TODO
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

#ifndef _DDC_UDL_DMA_PRIVATE_H_
#define _DDC_UDL_DMA_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "os/include/ddc_os_types_private.h"

/*--------------------------------------------------------------------
  DDC_DMA_ALL is defined in the makefile:
     1 - turn DMA on for all components that can be. 
     0 - turn DMA on/off depending on the individual component's flag
 ---------------------------------------------------------------------*/
/* do not change the following definitions */
#if DDC_DMA_ALL

#define DDC_DMA_BC              0   /* 1- use DMA for BC,     0 - use DDC_BLK_MEM_READ */
                                    /* BC DMA must not be turned on. When it was turned on, 
                                       regression test observed hang in the overnight tests. 
                                       Datastr and dataarray samples were also observed 
                                       receiving 3 extra messages */
#define DDC_DMA_REPLAY          1   /* 1- use DMA for replay,                   0 - use PCI block write */
#define DDC_DMA_RT              1   /* 1- use DMA for RT,                       0 - use DDC_BLK_MEM_READ */
#define DDC_DMA_MT              1   /* 1- use DMA for MT and DIO TT,            0 - use DDC_BLK_MEM_READ */
#define DDC_DMA_429             1   /* 1- use DMA for 429 and Voltage Monitor,  0 - use DDC_BLK_MEM_READ */
#define DDC_DMA_717             1   /* 1- use DMA for 717,                      0 - use DDC_BLK_MEM_READ */

#else

#define DDC_DMA_BC              0
#define DDC_DMA_REPLAY          0
#define DDC_DMA_RT              0
#define DDC_DMA_MT              0
#define DDC_DMA_429             0
#define DDC_DMA_717             0

#endif


#define DMA_ALIGNMENT_BYTES     8


/* write to PCIe DMA register */
#define DDC_DMA_WRITE_PCIE(pDeviceContext, dmaAddr, pValAddr)   \
    ddcUdlOsBusPcieDmaWrite(pDeviceContext,                     \
        dmaAddr,                                                \
        pValAddr)


struct _Q_INFO_TYPE;

struct _DMA_TRANSACTION
{
    U32BIT Channel;
    U32BIT ByteCount;
    U8BIT *BufAddr;              /* addr of the buffer in driver */
    U32BIT StartAddr;           /* absolute addr in device, in byte */

    U16BIT SgListNumEntries;

    U32BIT u32DMACHBaseAddr;    /* the base address of mem blk executing DMA (MTI or IMP), in 8 bit */
    U32BIT u32DMACHMaxAddr;     /* the max address of mem blk executing DMA (MTI or IMP), in 8 bit  */

    U32BIT u32ImpMrtHbufOddWordCount; /* indicates if there is an odd number of words */
    U32BIT u32ImpMrtHbufNumCmds;

    U32BIT u32DMAType;
};


typedef struct _DMA_PARAM_MRT_HBUF
{
    U32BIT u32OddWordCount;     /* indicates if there is an odd number of words */
    U32BIT u32NumCmdXfer;       /* Number of Cmd transfered */

} DMA_PARAM_MRT_HBUF;


typedef struct _DMA_PARAM_BC
{
    U32BIT u32NumMsgXfer;       /* Number of Msg transfered */

} DMA_PARAM_BC;


typedef enum _ACEX_DMA_STATE
{
    ACEX_DMA_IDLE,
    ACEX_DMA_BUSY

} ACEX_DMA_STATE;


typedef struct _DMA_Q_ENTRY
{
    U16BIT Channel;
    U32BIT ByteCount;           /* total transfer bytes */
    U32BIT RequestedByteCount;  /* requested transfer bytes */
    U32BIT StartAddr;           /* absolute addr in device, in byte */
    U8BIT *BufAddr;             /* addr of the buffer in driver */
    U32BIT Direction;           /* direction, 1 - DMA from device; 0 - DMA to device */
    U32BIT totalBytesTransfered;

    U16BIT SgListNumEntries;

    U32BIT u32DMACHBaseAddr;    /* the base address of mem blk executing DMA (MTI or IMP), in 8 bit */
    U32BIT u32DMACHMaxAddr;     /* the max address of mem blk executing DMA (MTI or IMP), in 8 bit */

    U8BIT u8DMAType;
    U32BIT copyOffset;          /* extra num of bytes at the beginning of the DMA mem in order to keep 8 byte alignment */
    U8BIT * userBuf;

    union PARAM_UNION           /* Param Stored for each type */
    {
        DMA_PARAM_MRT_HBUF MrtHbufParam;
        DMA_PARAM_BC BcParam;
    } U;

    U32BIT u32OddAlignment;

} DMA_Q_ENTRY, *PDMA_Q_ENTRY;


typedef enum _DMA_TYPE
{
    DMA_NOT_OCCUR = 0,
    DMA_MT,
    DMA_MRT_HBUF,
    DMA_MRT_CMDDATA,
    DMA_REPLAY,
    DMA_BC,
    DMA_429_RX_FIFO,
    DMA_429_SET_TX_FRAME_CONTROL,
    DMA_429_VOLTAGE_MONITORING,
    DMA_717_RX_FRAME,
    DMA_717_TX_FRAME,
    DMA_MEM_RD,
    DMA_TYPE_DIO_TT

} DMA_TYPE;

typedef struct _DDC_DMA
{
    U32BIT *pu32TacexRegBA;                             /* ptr to DMA Registers base address */
    U32BIT *pu32TacexRegSize;                           /* ptr to DMA Register size */

    BOOLEAN bIntialized;                                /* DMA Queue has been initialized */
    U16BIT u16State;                                    /* DMA Engine State */
    U16BIT u16CurrentChannel;

    U32BIT u32TransferSize;
    U32BIT u32Direction;

    DDC_ISR_LOCK_TYPE semDmaQ;                          /* DMA Queue Semaphore */
    DDC_ISR_FLAG_TYPE semDmaQFlag;                      /* DMA interrupt flag */

    DDC_DMA_ADDR dmaAddr;
    U32BIT *pu32Descriptor;
    struct _Q_INFO_TYPE *pQueuePtr;                             /* Pointer to DMA queue */

    DMA_Q_ENTRY queueEntryCurrentTransaction;

    /* 1553 Replay */
    U8BIT *pu8ReplayDMATarget;
    DDC_DMA_ADDR dmaAddrReplayDMA;
    size_t sizetReplayDMA;

} DDC_DMA;


/* ========================================================================== */
/*                              PLX & QPRM DMA                                */
/* ========================================================================== */

#define DESCRIPTOR_POINTER__DIR_OF_XFER_TO_DEVICE               0x00000000
#define DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE             0x00000008

typedef struct _DMA_TRANSFER_ELEMENT
{
    U32BIT PciAddressLow;
    U32BIT LocalAddress;
    U32BIT TransferSize;
    U32BIT NextDescriptorPointer;
    U32BIT PciAddressHigh;
    U32BIT pad[3];

} DMA_TRANSFER_ELEMENT;


#define DESCRIPTOR_POINTER__ADDR_SHIFT                          (4)
#define DESCRIPTOR_POINTER_ADDR(a)                              (((U32BIT) a) >> DESCRIPTOR_POINTER__ADDR_SHIFT)


/* ========================================================================== */
/*                              PCIe DMA                                      */
/* ========================================================================== */

/* Scatter-Gather implementation using descriptors blocks of 20 bytes - (PLDA EZDMA IP) */
typedef struct _DMA_TRANSFER_ELEMENT_PCIE
{
    U32BIT PciAddressLow;                   /* bits 31:3 - 8-byte alignment - bits 2:0 must be 000 */
    U32BIT PciAddressHigh;                  /* bits 63:32 - if a page is located in the 32-bit addressing space this must be 0 */
    U32BIT TransferSize;                    /* must be in a multiple of 8-bytes */
    U32BIT DmaNextDescriptorPointerLSB;     /* bits 31:2 - 4-byte alignment - bits 1:0 must be 00, with the exception of setting bit 0 to 1 to indicate end of chain */
    U32BIT DmaNextDescriptorPointerMSB;     /* bits 63:32 */

} DMA_TRANSFER_ELEMENT_PCIE;


#define DMA_START_PCIE                                  0x80000000
#define DMA_ABORT_PCIE                                  0x40000000

#define DESCRIPTOR_POINTER_PCIE__END_OF_CHAIN_TRUE      0x00000001
#define DESCRIPTOR_POINTER_PCIE__END_OF_CHAIN_FALSE     0x00000000

#define DESCRIPTOR_POINTER_PCIE__ADDR_SHIFT             (2)
#define DESCRIPTOR_POINTER_PCIE_ADDR(a)                 (((U32BIT) a) >> DESCRIPTOR_POINTER_PCIE__ADDR_SHIFT)

#define DMA_MAX_TRANSFER_SIZE_PCIE                      0x1000  /* PLDA EZDMA IP ONLY SUPPORTS 4K byte Transfers */

/* PCIe DMA Registers */
#define REG_DMA_TO_DEV_LOCAL_ADDR_PCIE                  0x0000  /* Offset in device where the data is located */
#define REG_DMA_TO_DEV_HOST_LSB_ADDR_PCIE               0x0001  /* 32 Address in host where Descriptor is located */
#define REG_DMA_TO_DEV_HOST_MSB_ADDR_PCIE               0x0002  /* MSBs for 64-bit Host Descriptor address */
#define REG_DMA_TO_DEV_XFER_SIZE_PCIE                   0x0003  /* Size in bytes of the DMA transfer */

#define REG_DMA_FROM_DEV_LOCAL_ADDR_PCIE                0x0004  /* Offset in device where the data should go */
#define REG_DMA_FROM_DEV_HOST_LSB_ADDR_PCIE             0x0005  /* 32 Address in host where Descriptor is located */
#define REG_DMA_FROM_DEV_HOST_MSB_ADDR_PCIE             0x0006  /* MSBs for 64-bit Host Descriptor address */
#define REG_DMA_FROM_DEV_XFER_SIZE_PCIE                 0x0007  /* Size in bytes of the DMA transfer */

#define REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_END_PCIE   0x0008  /* Address on device at which to roll over */
#define REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_START_PCIE 0x0009  /* Address of device to rollover too */

#define REG_DMA_TO_DEV_LOCAL_ADDR_ROLLOVER_END_PCIE     0x000A  /* Address on device at which to roll over */
#define REG_DMA_TO_DEV_LOCAL_ADDR_ROLLOVER_START_PCIE   0x000B  /* Address of device to rollover too */


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */

struct _DDC_UDL_DEVICE_CONTEXT;

extern void ddcUdlDmaInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT dmaQueueCreate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32QueueSize
);

extern S16BIT dmaQueueDestroy
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void dmaQueueStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void dmaMtSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32Len,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf
);

extern void dmaImpRtHbufSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumWds,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U32BIT u32NumCmds,
    U8BIT* userBuf
);

extern void dmaImpRtCmdDataSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumWds,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U8BIT* userBuf
);

extern void dmaImpBcSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumByte,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U32BIT u32NumMsgs
);

extern void dmaReplaySetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumByte,
    U32BIT u32StartAddr,
    U8BIT *pu8SrcBuf
);

extern void dmaARINC429RxFifoSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8Buf
);

extern void dmaARINC429SetTxFrameControlSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8Buf
);

extern void dmaArinc429VoltageMonitoringSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT  *pu8Buf
);

extern void  dmaARINC717TxFrameSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT  *pu8Buf
);

extern void dmaARINC717RxFrameSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT  *pu8Buf
);

extern void dmaStartDioTt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8HostDestBuffer
);


extern void dmaContinuePCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void dmaCmpltHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#endif /* _DDC_UDL_DMA_PRIVATE_H_ */
