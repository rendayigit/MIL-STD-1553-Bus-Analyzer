/*******************************************************************************
 * FILE: ddc_udl_dma.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support the
 *  DMA engine.
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
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_dma_private.h"
#include "driver_sdk/ddc_udl_hardware_private.h"
#include "driver_sdk/ddc_udl_um_regmap_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"

#define DEBUG_MTI_CH10_DMA       0

/* ========================================================================== */
/* ========================================================================== */
DDC_LOCAL U8BIT dmaExecEntry
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDMATransaction
);

/* ========================================================================== */
/*                             Q-PRIME DMA                                    */
/* ========================================================================== */
DDC_LOCAL void dmaStartQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

/* ========================================================================== */
/*                               PLX DMA                                      */
/* ========================================================================== */
DDC_LOCAL void dmaStartPlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

/* ========================================================================== */
/*                              PCIe DMA                                      */
/* ========================================================================== */
DDC_LOCAL void dmaStartPCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

DDC_LOCAL U8BIT dmaExecEntryPCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDMATransaction
);


/*******************************************************************************
 * Name:    ddcUdlDmaInit
 *
 * Description:
 *      This function initializes the registers and parameters needed for DMA.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlDmaInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegVal = 0;

    switch (pDeviceContext->u16DriverType)
    {
        case ACEX_IO_DRIVER:
        {
            /* doing nothing */
            break;
        }

        case ACEX_QPRM_DRIVER:
        {
            /* config DMA mode for DDC DMA */
            u32RegVal = QPRIME_DMA_MODE_DONE_INT_ENABLE;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_MODE, &u32RegVal);

            u32RegVal = QPRIME_DMA_INT_CS_INT_ENABLE;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_INT_CS, &u32RegVal);

            break;
        }

        case ACEX_PCIE_DRIVER:
        case ACEX_DD429_PCIE_DRIVER:
        {
            /* PCIe DMA */

            break;
        }

        case ACEX_DD429_PCI_DRIVER:
        default:
        {
            /*DMA using PLX 9056 */

            ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_DMA0_MODE, &u32RegVal);
            u32RegVal |= PLX_DMA_MODE_LOCAL_BUS_DATA_WIDTH_32_BIT |
                PLX_DMA_MODE_READY_INPUT_ENABLE |
                PLX_DMA_MODE_BURST_ENABLE |
                PLX_DMA_MODE_LOCAL_BURST_ENABLE |
                PLX_DMA_MODE_DONE_INT_ENABLE;
            ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_MODE, u32RegVal);
            DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_INITIALIZE, "ddcUdlDmaInit: Write REG_PLX_DMA0_MODE  %X\n", u32RegVal);

            /* Update PLX DMACSR0 */
            ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_DMA0_CSR, &u32RegVal);
            u32RegVal |= 0x00000009; /* Clear powerup DMA indication */
            ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_CSR, u32RegVal);

            /* Update PLX INTCSR */
            ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_INTSCR, &u32RegVal);
            u32RegVal |= PLX_INT_SCR_DMA_CH0_INT_ENABLE;
            if (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER)
            {
                u32RegVal |= PLX_INT_SCR_PCI_INT_ENABLE |
                    PLX_INT_SCR_LOCAL_INT_INPUT_ENABLE |
                    PLX_INT_SCR_LOCAL_INT_OUTPUT_ENABLE;
            }
            ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_INTSCR, u32RegVal);
            DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_INITIALIZE, "ddcUdlDmaInit: Write REG_PLX_INTSCR     %X\n", u32RegVal);

            /* make sure DAC is set to 0 */
            if (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER)
            {
                u32RegVal = PLX_INT_DAC_INIT;
                ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_DAC, u32RegVal);
                DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_INITIALIZE, "ddcUdlDmaInit: Write REG_PLX_DMA0_DAC   %X\n", u32RegVal);
            }

            break;
        }
    }

    /* Enable DMA Complete interrupt bits */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ | BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT);

    /* initialize DMA related parameter */
    pDeviceContext->sDMA.u16State = ACEX_DMA_IDLE;
    pDeviceContext->sDMA.u16CurrentChannel = 0;
    pDeviceContext->sDMA.dmaAddr = 0;
}

/*******************************************************************************
 * Name:    dmaStartPlx
 *
 * Description:
 *      This function configure the registers needed for DMA transfer
 *      and start DMA transfer.
 *
 * In   pDeviceContext  - device-specific structure
 *
 * Returns: none
 ******************************************************************************/
DDC_LOCAL void dmaStartPlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegValue;
    U32BIT ptr = 0;

    /* DMA Channel 0 Mode Register - DMAMODE0 */
    /* Enable Scatter/Gather Mode, Interrupt On Done, and route interrupts to PCI */
    ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_DMA0_MODE, &u32RegValue);
    u32RegValue |= (PLX_DMA_MODE_SG_MODE_ENABLE | PLX_DMA_MODE_DONE_INT_ENABLE |PLX_DMA_MODE_CLEAR_COUNT_MODE);
    ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_MODE, u32RegValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_PLX_DMA0_MODE = 0x%X\n", u32RegValue);


    /* Interrupt Control/Status Register - INTCSR */
    /* Enable PCI interrupts and DMA Channel 0 interrupts */
    ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_INTSCR, &u32RegValue);
    u32RegValue |= (PLX_INT_SCR_PCI_INT_ENABLE | PLX_INT_SCR_DMA_CH0_INT_ENABLE);
    ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_INTSCR, u32RegValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_PLX_INTSCR = 0x%x\n", u32RegValue);


    /* DMA Channel 0 Descriptor Pointer Register - DMADPR0 */
    /* Write the new descriptor address of the DMA_TRANSFER_ELEMENT list */
    DDC_VIRT_TO_PHYS(pDeviceContext, pDeviceContext->sDMA.pu32Descriptor, &ptr);
    ptr = (U32BIT)(DESCRIPTOR_POINTER_ADDR(ptr) << 4);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_PLX_DMADPR0 ADDR = 0x%x\n", ptr);
    ptr |= PLX_DMA_DESC_PTR_PCI_ADDR;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_PLX_DMADPR0 = 0x%x\n", ptr);

    ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMADPR0, ptr);


    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "Starting a DMA operation!\n");


    /* DMA 1 CSR Register - (DMACSR0)                           */
    /*      Start the DMA operation: Set Enable and Start bits  */
    ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_DMA0_CSR, &u32RegValue);
    u32RegValue |= (PLX_DMA_CSR_ENABLE | PLX_DMA_CSR_START);
    ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_CSR, u32RegValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_PLX_DMA0_CSR = 0x%x\n", u32RegValue);
}

/*******************************************************************************
 * Name:    dmaQueueCreate
 *
 * Description:
 *      This function will create a DMA queue with the specified number of entries for a device.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32QueueSize    Number of queue entries
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT dmaQueueCreate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32QueueSize
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_INIT, "ENTER->\n");

    /* check to see that DMA hasn't been allocated */
    if (pDeviceContext->sDMA.bIntialized)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_INIT, "DMA queue already intialized!\n");
        return DDC_UDL_ERROR__DMA_INITIALIZED;
    }
    else
    {
        /* mark device as configured */
        pDeviceContext->sDMA.bIntialized = TRUE;

        /* initialize DMA queue */
        pDeviceContext->sDMA.pQueuePtr = Q__INFO_TYPE_Create(pDeviceContext, sizeof(DMA_Q_ENTRY), u32QueueSize);

        if (pDeviceContext->sDMA.pQueuePtr == NULL)
        {
            pDeviceContext->sDMA.bIntialized = FALSE;
            DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_INIT, "aceDmaCreate : ACE_ERR_QCALLOC!\n");

            return DDC_UDL_ERROR__QUEUE_CREATE;
        }

        DDC_ISR_LOCK_INIT(pDeviceContext->sDMA.semDmaQ);
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    aceDmaDestroy
 *
 * Description:
 *      This function will destroy a DMA queue that has previously been created
 *      for a given logical device number.
 *
 *      State: READY
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT dmaQueueDestroy
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_FREE, "ENTER->\n");

    Q__INFO_TYPE_Destroy(pDeviceContext, pDeviceContext->sDMA.pQueuePtr);

    pDeviceContext->sDMA.bIntialized = FALSE;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    dmaEnQueue
 *
 * Description:
 *      This function enqueue a DMA entry to DMA queue.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT dmaEnQueue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDmaReq
)
{
    DMA_Q_ENTRY *pDmaQueueEntry = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_ADD, "ENTER->\n");

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* Add an entry into tail */
    pDmaQueueEntry = (DMA_Q_ENTRY *) Q__INFO_TYPE_Add(pDeviceContext->sDMA.pQueuePtr);

    if (pDmaQueueEntry == NULL) /* cannot be added to the queue */
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_ADD, "cannot be added to q!\n");
        return DDC_UDL_ERROR__QUEUE_ADD;
    }

    /* fill in entry */
    memcpy(pDmaQueueEntry, pDmaReq, sizeof(DMA_Q_ENTRY));

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    dmaQueueStart
 *
 * Description:
 *      This function checks whether there is DMA requests waiting in DMA Queue.
 *      If there is DMA request and no other DMA is transfering,
 *      start DMA transfer from the first DMA request.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaQueueStart
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DMA_Q_ENTRY *pDmaQueueEntry = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_START, "ENTER->\n");

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    if (pDeviceContext->sDMA.u16State == ACEX_DMA_IDLE)
    {
        pDmaQueueEntry = (DMA_Q_ENTRY *) Q__INFO_TYPE_Read(pDeviceContext->sDMA.pQueuePtr);

        if (pDmaQueueEntry != NULL) /* There is DMA request in the queue */
        {
            memcpy(&pDeviceContext->sDMA.queueEntryCurrentTransaction, pDmaQueueEntry, sizeof(DMA_Q_ENTRY));

            /* set state to busy so no other threads can think we are idle */
            pDeviceContext->sDMA.u16State = ACEX_DMA_BUSY;

            DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

            /* execute DMA */
            switch (pDeviceContext->u16DriverType)
            {
                case ACEX_IO_DRIVER:
                {
                    /* doing nothing */
                    break;
                }

                case ACEX_PCIE_DRIVER:
                case ACEX_DD429_PCIE_DRIVER:
                {
                    dmaExecEntryPCIe(pDeviceContext, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
                    break;
                }

                case ACEX_QPRM_DRIVER:
                case ACEX_DD429_PCI_DRIVER:
                default:
                {
                    dmaExecEntry(pDeviceContext, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
                    break;
                }
            }

            DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        }
    }

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_QUEUE_START, "EXIT->\n");
}

/*******************************************************************************
 * Name:    dmaExecEntry
 *
 * Description:
 *      This function constructs DMA_TRANSFER_ELEMENT according to
 *      the DMA request entry. DMA_TRANSFER_ELEMENT has the information
 *      needed for hardware to execute DMA transfer.
 *
 *      DMA_TRANSFER_ELEMENT                        PCI Memory
 *      +----------------------------------+        +--------------------------------+
 *      | First PCI Address                |------->| First Memory Block to Transfer |
 *      +----------------------------------+        +--------------------------------+
 *      | First Local Address              |---+
 *      +----------------------------------+   |
 *      | First Transfer Size (byte count) |   |    Local Memory
 *      +----------------------------------+   |    +--------------------------------+
 *      | Next Descriptor Pointer      | | |   +--->| First Memory Block to Transfer |
 *      +----------------------------------+        +--------------------------------+
 *      | PCI Address High (not used)      |
 *      +----------------------------------+
 *      | Pad[0]                           |
 *      +----------------------------------+
 *      | Pad[1]                           |
 *      +----------------------------------+
 *      | Pad[2]                           |
 *      +----------------------------------+
 *
 *      The descriptor pointer address is located in bits 31:4. The last 4 bits are used for
 *      DMA information. This makes the address aligned on a 8 byte boundary.
 *
 *      The other bits are defined as follows:
 *
 *      Bit 0 - DMA Channel 0 Descriptor Location
 *              1 = PCI Address space
 *              0 = Local Address space
 *
 *      Bit 1 - DMA Channel 0 End of Chain
 *              1 = End of chain
 *              0 = Not end of chain descriptor (Same as DMA Block mode)
 *
 *      Bit 2 - DMA Channel 0 Interrupt after Terminal Count
 *              1 = Causes an interrupt to be asserted after the terminal count for this descriptor is reached
 *              0 = Disables interrupts from being asserted
 *
 *      Bit 3 - DMA Channel 0 Direction of Transfer
 *              1 = Transfers from the Local Bus to the PCI Bus
 *              0 = Transfers from the PCI Bus to the Local Bus
 *
 * In   pDeviceContext      device-specific structure
 * In   pDMATransaction     the DMA request entry
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
DDC_LOCAL U8BIT dmaExecEntry
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDMATransaction
)
{
    enum ddc_dma_data_direction eDirection;

    DMA_TRANSFER_ELEMENT *dteVirtualAddress;
    U32BIT dtePhysicalAddress = 0;
    U32BIT u32Channel = 0;

#if DDC_PPC
    DMA_TRANSFER_ELEMENT *dteVirtualAddressTemp;
    U32BIT dtePhysicalAddressTemp;
#endif /* DDC_PPC */

    U32BIT u32NextTransferSize = 0;
    U32BIT u32NextPciAddr = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "pDMATransaction->Channel = %d\n", pDMATransaction->Channel);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "pDMATransaction->ByteCount = 0x%x\n", pDMATransaction->ByteCount);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "pDMATransaction->StartAddr = 0x%x\n", pDMATransaction->StartAddr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "pDMATransaction->BufAddr = %p\n", pDMATransaction->BufAddr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "pDMATransaction->SgListNumEntries = %d\n", pDMATransaction->SgListNumEntries);

    /* set the pointer to the first DMA_TRANSFER_ELEMENT    */
    /* for both virtual and physical address references     */
    dteVirtualAddress = (DMA_TRANSFER_ELEMENT *)pDeviceContext->sDMA.pu32Descriptor;

    DDC_VIRT_TO_PHYS(pDeviceContext, dteVirtualAddress, &dtePhysicalAddress);
    dtePhysicalAddress = (U32BIT)(dtePhysicalAddress + (sizeof(DMA_TRANSFER_ELEMENT)));

    /* Translate the System's SCATTER_GATHER_LIST elements  */
    /* into the device's DMA_TRANSFER_ELEMENT elements      */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "constructing DTE!\n");

    u32Channel = pDMATransaction->Channel;

    /* map DMA address */
    if (pDMATransaction->Direction == DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE)
    {
        eDirection = DDC_DMA_FROM_DEVICE;
    }
    else
    {
        eDirection = DDC_DMA_TO_DEVICE;
    }

    pDeviceContext->sDMA.dmaAddr = ddcUldOsPciDmaMap(
        pDeviceContext,
        (void *)(pDMATransaction->BufAddr),
        pDMATransaction->ByteCount,
        eDirection);

    /* construct the DTE (DMA_TRANSFER_ELEMENT) */

    dteVirtualAddress->PciAddressLow = pDeviceContext->sDMA.dmaAddr;

    if (!(dteVirtualAddress->PciAddressLow))
    {
        /* Flag that all buffers are used.  MAP FAILED! */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "DMA Map Fail\n");
        pDeviceContext->pChannel1553[u32Channel]->sMT.bMtiDeviceOverflow = TRUE;    /* <UDL21> */
    }

    /* the LocalAddress is the offset into the RAM          */
    /* from where the data will be read from to written to  */

    dteVirtualAddress->LocalAddress = pDMATransaction->StartAddr;
    dteVirtualAddress->TransferSize = pDMATransaction->ByteCount;

    dteVirtualAddress->NextDescriptorPointer  = PLX_DMA_DESC_PTR_PCI_ADDR;                          /* bit 0 - Descriptor Location */
    /* bit 1 - End of Chain (skip for now - set later) */
    /* bit 2 - Interrupt after Terminal Count - do not set */
    dteVirtualAddress->NextDescriptorPointer |= pDMATransaction->Direction;                         /* bit 3 - Direction of Transfer */
    dteVirtualAddress->NextDescriptorPointer |= (DESCRIPTOR_POINTER_ADDR(dtePhysicalAddress) << 4); /* bits 31:4 - Next Descriptor Address */

    dteVirtualAddress->PciAddressHigh = 0; /* not used by DMA engine */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "[DTE0] PciAddressLow %08X  LocalAddress %08X  TransferSize %5d  Local %08X  NextDescriptorPointer %08x\n",
        dteVirtualAddress->PciAddressLow,
        dteVirtualAddress->LocalAddress,
        dteVirtualAddress->TransferSize,
        dteVirtualAddress->NextDescriptorPointer,
        dteVirtualAddress->PciAddressHigh);

    /* is this transfer going to wrap around? */
    if ((pDMATransaction->StartAddr + pDMATransaction->ByteCount) > pDMATransaction->u32DMACHMaxAddr)
    {
        /* break DMA transfer into 2 transactions */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA,
            DDC_DBG_DMA_EXEC_WORK_ITEM, "buffer wrap occurred! StartAddr: %08x  ByteCount: 0x%x  u32DMACHMaxAddr: 0x%x\n",
            pDMATransaction->StartAddr, pDMATransaction->ByteCount, pDMATransaction->u32DMACHMaxAddr);

        /* save off original xfer size */
        u32NextTransferSize = dteVirtualAddress->TransferSize;

        /* reduce the transfer size of the first element */
        dteVirtualAddress->TransferSize = pDMATransaction->u32DMACHMaxAddr - dteVirtualAddress->LocalAddress;

        /* compute the transfer size of the next element */
        u32NextTransferSize = u32NextTransferSize - dteVirtualAddress->TransferSize;

        /* compute the address of the next transfer - current addr + new transfer size */
        u32NextPciAddr = dteVirtualAddress->PciAddressLow + dteVirtualAddress->TransferSize;


        /* point to the next element */
        dteVirtualAddress++;

        dtePhysicalAddress = (U32BIT)(dtePhysicalAddress + sizeof(DMA_TRANSFER_ELEMENT));

        dteVirtualAddress->PciAddressLow = u32NextPciAddr;
        dteVirtualAddress->LocalAddress = pDMATransaction->u32DMACHBaseAddr; /* wrap to start of buffer */
        dteVirtualAddress->TransferSize = u32NextTransferSize;

        dteVirtualAddress->NextDescriptorPointer  = PLX_DMA_DESC_PTR_PCI_ADDR;                          /* bit 0 - Descriptor Location */
        /* bit 1 - End of Chain (skip for now - set later) */
        /* bit 2 - Interrupt after Terminal Count - do not set */
        dteVirtualAddress->NextDescriptorPointer |= pDMATransaction->Direction;                         /* bit 3 - Direction of Transfer */
        dteVirtualAddress->NextDescriptorPointer |= (DESCRIPTOR_POINTER_ADDR(dtePhysicalAddress) << 4); /* bits 31:4 - Next Descriptor Address */

        dteVirtualAddress->PciAddressHigh = 0;

        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "[DTE1] PciAddressLow %08X  LocalAddress %08X  TransferSize %5d  Local %08X  NextDescriptorPointer %08x\n",
            dteVirtualAddress->PciAddressLow,
            dteVirtualAddress->LocalAddress,
            dteVirtualAddress->TransferSize,
            dteVirtualAddress->NextDescriptorPointer,
            dteVirtualAddress->PciAddressHigh);

        pDMATransaction->SgListNumEntries++;
    }

    /* set the end of chain bit */
    dteVirtualAddress->NextDescriptorPointer |= PLX_DMA_DESC_PTR_END_OF_CHAIN;

#if DDC_PPC
{
    U32BIT i;

    /* only swap for non-ASIC devices */
    if (pDeviceContext->u16DriverType != ACEX_QPRM_DRIVER)
    {
        /* adjust endianess of descriptor block */

        dteVirtualAddressTemp = (DMA_TRANSFER_ELEMENT *)pDeviceContext->sDMA.pu32Descriptor;

        DDC_VIRT_TO_PHYS(pDeviceContext, dteVirtualAddress, &dtePhysicalAddressTemp);
        dtePhysicalAddressTemp = (U32BIT)(dtePhysicalAddressTemp + sizeof(DMA_TRANSFER_ELEMENT));

        for (i = 0; i < pDMATransaction->SgListNumEntries; i++)
        {
            /* adjust endianness of descriptor */
            dteVirtualAddressTemp->PciAddressLow = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->PciAddressLow);

            /* dteVirtualAddressTemp->PciAddressHigh is always 0, so there is no need to swap */
            dteVirtualAddressTemp->TransferSize = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->TransferSize);
            dteVirtualAddressTemp->LocalAddress = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->LocalAddress);
            dteVirtualAddressTemp->NextDescriptorPointer = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->NextDescriptorPointer);

            /*Adjust the next DMA_TRANSFER_ELEMEMT */
            dteVirtualAddressTemp++;
            dtePhysicalAddressTemp += (U32BIT)sizeof(DMA_TRANSFER_ELEMENT);
        }
    }
}
#endif /* DDC_PPC */

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pDeviceContext->sDMA.u16CurrentChannel = (U16BIT)(pDMATransaction->Channel + 1);
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* Start the DMA operation */
    if (pDeviceContext->u16DriverType == ACEX_QPRM_DRIVER)
    {
        dmaStartQPRM(pDeviceContext);
    }
    else
    {
        dmaStartPlx(pDeviceContext);
    }

    return TRUE;
}

/*******************************************************************************
 * Name:    dmaMtSetup
 *
 * Description:
 *      This function setup DMA transfer when Ch10 Packet is available
 *      (MTI interrupt happens). DMA transfer will transfer Ch10 packet
 *      into DataListEntry.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaMtSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32Len,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf
)
{
    struct _ACEX_1553_MT_TYPE *pMT;
    DMA_Q_ENTRY DmaQueueEntry;
    U32BIT u32AlignAddr;
    U32BIT u32AlignLen;

    pMT = &(pDeviceContext->pChannel1553[u8Ch]->sMT);


    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "Chan: %d\n", u8Ch);

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State = BUF_STATE_DMA;
    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    /* Set DMA transaction channel */
    DmaQueueEntry.Channel = u8Ch;

    DmaQueueEntry.StartAddr = u32StartAddr;
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.StartAddr(in bytes): %08x\n", DmaQueueEntry.StartAddr);

    /* buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pu8Buf;
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.BufAddr(in bytes): %p\n", DmaQueueEntry.BufAddr);

    /* DMA from device */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;

    /* transfer byte count */
    DmaQueueEntry.ByteCount = u32Len << 1; /* words to bytes */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.ByteCount: %08x\n", DmaQueueEntry.ByteCount);

    DmaQueueEntry.RequestedByteCount = DmaQueueEntry.ByteCount; /* words to bytes */

    /*Num SGList entries */
    DmaQueueEntry.SgListNumEntries = 1;

    /* transfer address alignment */
    u32AlignAddr = DmaQueueEntry.StartAddr % DMA_ALIGNMENT_BYTES;

    if (!u32AlignAddr)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "even alligned transfer!\n");
        DmaQueueEntry.copyOffset = 0;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "MTI: Adjust Addr, back %d bytes\n", u32AlignAddr);

        DmaQueueEntry.copyOffset = u32AlignAddr;

        /* DMA buffer address */
        DmaQueueEntry.BufAddr = DmaQueueEntry.BufAddr;

        /* DMA SRC addr - Hardware */
        DmaQueueEntry.StartAddr = DmaQueueEntry.StartAddr - DmaQueueEntry.copyOffset;
        DmaQueueEntry.ByteCount = DmaQueueEntry.ByteCount + DmaQueueEntry.copyOffset;

        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.BufAddr: %p\n", DmaQueueEntry.BufAddr);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.StartAddr: %08x\n", DmaQueueEntry.StartAddr);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMATransaction.ByteCount: %08x\n", DmaQueueEntry.ByteCount);

    }

    /* transfer length alignment */
    u32AlignLen = DmaQueueEntry.ByteCount % DMA_ALIGNMENT_BYTES;

    if (u32AlignLen)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "MTI: Adjust length, add %d bytes in the end \n", (DMA_ALIGNMENT_BYTES - u32AlignLen));
        DmaQueueEntry.ByteCount += (DMA_ALIGNMENT_BYTES - u32AlignLen);
    }

    DmaQueueEntry.u32DMACHBaseAddr = pMT->u32MtiMem32BitStartAddr << 2;
    DmaQueueEntry.u32DMACHMaxAddr = pMT->u32MtiMem32BitEndAddr << 2;
    DmaQueueEntry.u8DMAType = DMA_MT;

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaMtCmplt
 *
 * Description:
 *      This function is called when DMA transfer complete.
 *      i.e. Packet has been transfered into DataListEntry
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaMtCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    struct _ACEX_1553_MT_TYPE *pMT;
    U32BIT u32WordCount = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "ENTER->\n");

    pMT = &(pDeviceContext->pChannel1553[u8Ch]->sMT);

    /* Only process a DMA transfer if channel is in RUN state */
    if (pMT->eMtiState != ACEX_MTI_RUN)
    {
        DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
        pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State = BUF_STATE_FREE;
        DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "DMA processing called when card not in RUN state!\n");
        return;
    }

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "ProcessDMA : end \n");

    /* Inform firmware, free to re-use space just read from for future writes.
    * Must write number of words read to the MTI Free Memory Count register */
    u32WordCount = pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->u32WordCount;

    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    DDC_REG_WRITE(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_FREE_MEM_COUNT_RW), &u32WordCount);

    /* Update MTi metrics. Free count register returns amount of free space in word (U16BIT) units */
    DDC_REG_READ(pDeviceContext, ((*(pMT->pu32RegBA)) + REG_MT_MTI_FREE_MEM_COUNT_RW), &u32WordCount);
    pMT->u32MtiStackPercentFull = 100 - ((u32WordCount * 100) / pMT->u32MtiStackSizeWords);

    if (pMT->u32MtiStackPercentHigh < pMT->u32MtiStackPercentFull)
    {
        pMT->u32MtiStackPercentHigh = pMT->u32MtiStackPercentFull;
    }

    DDC_ISR_LOCK_TAKE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);
    pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->copyOffset = pDmaEntry->copyOffset;
    pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->State = BUF_STATE_DATA;

#if DEBUG_MTI_CH10_DMA
{
    ACEX_MTI_CH10_DATA_PKT *pMtiCh10DataPkt = NULL;
    U16BIT *pPacketData = NULL;
    U16BIT *pu16TimeCntr;

    pMtiCh10DataPkt = pMT->pMtiDataListEntry[pMT->u32MtiDataPktHead]->pDataPkt;

    pu16TimeCntr = (U16BIT *) GET_ACEX_MTI_CH10_PKT_TIME_CNTR_POINTER(pMtiCh10DataPkt);
    pPacketData = (U16BIT *) GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER(pMtiCh10DataPkt);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT,
        "SYNC %04x ChID %04x PktLen %08x DataLen %08x SeqNumHdrVer %04x DatTypePktFlags %04x"                                                   \
        "RelTimCntr0 %04x RelTimCntr1 %04x RelTimCntr2 %04x HdrChkSum %04x ChnlSpecificData %08x IntraPktTimeStmp0 %04x IntraPktTimeStmp1 %04x" \
        "IntraPktTimeStmp2 %04x IntraPktTimeStmp3 %04x BlkStatus %04x Gap %04x Len %04x Cmd %04x: %04x %04x\n",                                 \
        pMtiCh10DataPkt->u16PktSyncPattern, pMtiCh10DataPkt->u16ChannelId, pMtiCh10DataPkt->u32PktLength, pMtiCh10DataPkt->u32DataLength,       \
        pMtiCh10DataPkt->u16SeqNumHdrVer, pMtiCh10DataPkt->u16DatTypePktFlags, pu16TimeCntr[0], pu16TimeCntr[1], pu16TimeCntr[2],               \
        pMtiCh10DataPkt->u16HeaderChksum, pMtiCh10DataPkt->u32ChnlSpecificData,                                                                 \
        pPacketData[0], pPacketData[1], pPacketData[2], pPacketData[3], pPacketData[4],                                                         \
        pPacketData[5], pPacketData[6], pPacketData[7], pPacketData[8], pPacketData[9]);
}
#endif /* DEBUG_MTI_CH10_DMA */

    /* Increment Head ptr */
    pMT->u32MtiDataPktHead++;

    /* Roll the head pointer to the beginning if necessary */
    if (pMT->u32MtiDataPktHead == pMT->u32MtiDataPoolCount)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_MT, "MTI Data Pkt List Rolling over to zero.\n");
        pMT->u32MtiDataPktHead = 0;
    }

    /* set wait queue condition flag */
    if (pMT->u8MtiDataPoolEventFlag < 0xff)
    {
        pMT->u8MtiDataPoolEventFlag++;
    }

    DDC_ISR_LOCK_GIVE(pMT->semMtiDataPkt, pMT->semMtiDataPktFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(&pMT->eMtiDataPoolCallback, &pMT->eMtiDataPoolEvent);
}

/*******************************************************************************
 * Name:    dmaImpRtSetup
 *
 * Description:
 *      This function setup DMA transfer when RT messages are available
 *      (IMP interrupt happens). DMA transfer will transfer RT messages
 *      into driver Hbuf.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * In   u32NumCmds      Number of Cmd need to transfer
 * In   u32NumWds       Number of Words need to transfer
 * In   u32StartAddr    Absolute StartAddr in device (in byte)
 * In   pu8Buf          destination buffer address in driver
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaImpRtHbufSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumWds,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U32BIT u32NumCmds,
    U8BIT* userBuf
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    DMA_Q_ENTRY DmaQueueEntry;
    U32BIT u32AlignAddr;
    U32BIT u32AlignLen;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "Chan: %d\n", u8Ch);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "ENTER->\n");

    /* User Buffer Storage */
    DmaQueueEntry.userBuf = userBuf;

    /* channel */
    DmaQueueEntry.Channel = u8Ch;

    /* transfer byte count */
    DmaQueueEntry.ByteCount = u32NumWds << 1; /* words to bytes */

    DmaQueueEntry.RequestedByteCount = DmaQueueEntry.ByteCount;

    /* Start Address */
    DmaQueueEntry.StartAddr = u32StartAddr;

    /* buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pu8Buf;

    /* DMA from device */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;

    /*Num SGList entries */
    DmaQueueEntry.SgListNumEntries = 1;

    /* transfer address alignment */
    u32AlignAddr = DmaQueueEntry.StartAddr % DMA_ALIGNMENT_BYTES;

    if (!u32AlignAddr)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "even alligned transfer!\n");
        DmaQueueEntry.copyOffset = 0;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "RT-Hbuf: Adjust Addr, back %d bytes\n", u32AlignAddr);
        DmaQueueEntry.copyOffset = u32AlignAddr;

        /* DMA destination address -  buffer */
        DmaQueueEntry.BufAddr = DmaQueueEntry.BufAddr;

        /* DMA SRC addr - Hardware */
        DmaQueueEntry.StartAddr = DmaQueueEntry.StartAddr - DmaQueueEntry.copyOffset;
        DmaQueueEntry.ByteCount = DmaQueueEntry.ByteCount + DmaQueueEntry.copyOffset;
    }

    /* transfer length alignment */
    u32AlignLen = DmaQueueEntry.ByteCount % DMA_ALIGNMENT_BYTES;

    if (u32AlignLen)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "RT-Hbuf: Adjust length, add %d bytes in the end \n", (DMA_ALIGNMENT_BYTES - u32AlignLen));
        DmaQueueEntry.ByteCount += (DMA_ALIGNMENT_BYTES - u32AlignLen);
    }

    /* IMP MEM Start Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u32DMACHBaseAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1);

    /* IMP MEM End Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u32DMACHMaxAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1) + (pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD << 2);
    DmaQueueEntry.u8DMAType = DMA_MRT_HBUF;
    DmaQueueEntry.U.MrtHbufParam.u32NumCmdXfer = u32NumCmds;

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaImpRtHbufCmplt
 *
 * Description:
 *      This function is called when Imp RT HBUF DMA transfer complete.
 *      i.e. Messages have been transfered into Driver Hbuf.
 *      Messages are stored at a fixed locations. This makes it easier
 *      to keep track of where a message is located.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaImpRtHbufCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    struct _ACEX_1553_RT_TYPE *pRT = &(pCh->sRT);
#ifdef DDC_MRT_CMD_STK_MSG_BYTESZ
    U8BIT *pUserBuf;
    U8BIT *pDmaData;
    int i;
#endif
    U8BIT *pBuf = 0;
    U8BIT *pBufStart = 0;
    U32BIT u32CurrentBufLoc = 0;
    U32BIT u32LenToEOB = 0;
    U32BIT u32LeftOver = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "Ch %d RT Hbuf DMA Cmplt\n", u8Ch);

    /* update number of available entries once transfer is completed */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

#ifndef  DDC_MRT_CMD_STK_MSG_BYTESZ
    /*
        If DDC_MRT_CMD_STK_MSG_BYTESZ is not defined we
        will utilized method currently defined in VxWorks
        and Linux to copy data from pu8RtDmaTarget to user
        buffer. This will fix crash in completion routine.
    */
    pBuf = (U8BIT *)pDmaEntry->userBuf;

    /* Is Data greater then buffer size? If so then wrap it. */
    if (pRT->sHbuf.u32HbufLstWr >= pRT->sHbuf.u32HBufByteSize)
    {

        /* get pointer to the starting address for copying of msgs */
        pBufStart = (U8BIT*)pRT->sHbuf.pu8hbufMemory;

        /* Find data length to end of buffer, also find overflow data if we have some */
        u32CurrentBufLoc = (U32BIT)((U8BIT*)pBuf - pBufStart);
        u32LenToEOB = (pRT->sHbuf.u32HBufByteSize - u32CurrentBufLoc); /* removed -1; */
        u32LeftOver = pDmaEntry->RequestedByteCount  - u32LenToEOB;    /* was pDmaEntry->ByteCount */

        /* copy data to end of buffer */
        memcpy((void*)pBuf, (void*)pDeviceContext->pu8RtDmaTarget, u32LenToEOB);

        /* if data left over copy to beginning of buffer */
        if (u32LeftOver)
        {
            /* Roll buffer over - use pBufStart */
            memcpy((void*)pBufStart, (U8BIT*)pDeviceContext->pu8RtDmaTarget + u32LenToEOB, u32LeftOver);
            pRT->sHbuf.u32HbufLstWr = u32LeftOver;
        }
        else
        {
            pRT->sHbuf.u32HbufLstWr = 0;
        }
    }
    else
    {
        /* copy data to end of buffer */
        memcpy((void*)pBuf, (void*)pDeviceContext->pu8RtDmaTarget, pDmaEntry->RequestedByteCount);
    }

    pRT->sHbuf.u32HbufNumEntries += pDmaEntry->U.MrtHbufParam.u32NumCmdXfer /*pRT->sHbuf.u32DmaXferCmd*/;

#else
    pUserBuf = pDmaEntry->userBuf;
    pDmaData = pDeviceContext->pu8RtDmaTarget + pDmaEntry->copyOffset;

    /* loop for the number of messages to copy */
    for (i=0; i<pDmaEntry->U.MrtHbufParam.u32NumCmdXfer; i++)
    {
        U32BIT u32BytesToCopy;

        /* determine the number of bytes to copy from the msg field length */
        u32BytesToCopy = (*(U16BIT *)pDmaData * 2) + 4; /* add 4 for msglengtfieldsize */

        memcpy(pUserBuf, pDmaData, u32BytesToCopy);

        /* set the next msg location */
        pUserBuf += MRT_CMD_STK_MSG_BYTE_SZ;

        /* jump to the next msg in the DMA buffer */
        pDmaData += u32BytesToCopy;

        pRT->sHbuf.u32HbufNumEntries++;
        pRT->sHbuf.u32HbufLstWr += MRT_CMD_STK_MSG_BYTE_SZ;

        /* increment the number of messages there are in the host buffer */
        pRT->sHbuf.u32CmdIndex++;

        /* is the index at the end of the host buffer? if so, wrap to beginning */
        if (pRT->sHbuf.u32CmdIndex >= pRT->sHbuf.u32StkMsgCapacity)
        {
            /* wrap back to the beginning */
            pRT->sHbuf.u32HbufLstWr = 0;
            pRT->sHbuf.u32CmdIndex = 0;
            pUserBuf = pRT->sHbuf.pu8hbufMemory;
        }
    }
#endif

    pRT->sHbuf.bDmaXferPending = FALSE;

    if (pDmaEntry->U.MrtHbufParam.u32NumCmdXfer >= pRT->sHbuf.u32MaxTfrCmdSize)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        mrtHbufImpPostQueue(pDeviceContext, u8Ch);
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }
}

/*******************************************************************************
 * Name:    dmaImpRtCmdDataSetup
 *
 * Description:
 *      This function setup DMA transfer when RT messages are available
 *      (IMP interrupt happens). DMA transfer will transfer RT messages
 *      into driver Hbuf.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * In   u32NumCmds      Number of Cmd need to transfer
 * In   u32NumWds       Number of Words need to transfer
 * In   u32StartAddr    Absolute StartAddr in device (in byte)
 * In   pu8Buf          destination buffer address in driver
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaImpRtCmdDataSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumWds,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U8BIT* userBuf
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    DMA_Q_ENTRY DmaQueueEntry;
    U32BIT u32AlignAddr;
    U32BIT u32AlignLen;


    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "Chan: %d\n", u8Ch);

    /* User Buffer Storage */
    DmaQueueEntry.userBuf = userBuf;

    /* channel */
    DmaQueueEntry.Channel = u8Ch;

    /* transfer byte count */
    DmaQueueEntry.ByteCount = u32NumWds << 1; /* words to bytes */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "DMATransaction.ByteCount: %08x\n", DmaQueueEntry.ByteCount);

    DmaQueueEntry.RequestedByteCount = DmaQueueEntry.ByteCount;

    /* Start Address */
    DmaQueueEntry.StartAddr = u32StartAddr;
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "FIRST MSG ADDR(in 16-bits): %08x\n", u32StartAddr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "Chnl Mem Base Addr(in 32-bits): %08x\n", *(pCh->pu32MemBA));
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "DMATransaction.StartAddr(in bytes): %08x\n", DmaQueueEntry.StartAddr);

    /* buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pu8Buf;
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "DMATransaction.BufAddr(in bytes): %p\n", DmaQueueEntry.BufAddr);

    /* DMA from device */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;

    /*Num SGList entries */
    DmaQueueEntry.SgListNumEntries = 1;

    /* xfer address alignment */
    u32AlignAddr = DmaQueueEntry.StartAddr % DMA_ALIGNMENT_BYTES;

    if (!u32AlignAddr)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "even alligned xfer!\n");
        DmaQueueEntry.copyOffset = 0;
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "RT-CMD: Adjust Addr, back %d bytes\n", u32AlignAddr);
        DmaQueueEntry.copyOffset = u32AlignAddr;

        /* DMA destination address -  buffer */
        DmaQueueEntry.BufAddr = DmaQueueEntry.BufAddr;

        /* DMA SRC addr - Hardware */
        DmaQueueEntry.StartAddr = DmaQueueEntry.StartAddr - DmaQueueEntry.copyOffset;
        DmaQueueEntry.ByteCount = DmaQueueEntry.ByteCount + DmaQueueEntry.copyOffset;
    }

    /* xfer length alignment */
    u32AlignLen = DmaQueueEntry.ByteCount % DMA_ALIGNMENT_BYTES;

    if (u32AlignLen)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "RT-CMD: Adjust length, add %d bytes in the end \n", (DMA_ALIGNMENT_BYTES - u32AlignLen));
        DmaQueueEntry.ByteCount += (DMA_ALIGNMENT_BYTES - u32AlignLen);
    }

    DmaQueueEntry.u32DMACHBaseAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1);                             /* IMP MEM Start Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u32DMACHMaxAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1) + (pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD << 2);    /* IMP MEM End Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u8DMAType = DMA_MRT_CMDDATA;

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaImpRtCmdDataCmplt
 *
 * Description:
 *      This function is called when Imp RT Cmd/Data DMA transfer complete.
 *      i.e. Messages have been transfered into a buffer temporarily allocated
 *      in driver. Then, the data in the temporary buffer will be DDC_COPY_TO_USER
 *      to library.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaImpRtCmdDataCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    struct _ACEX_1553_RT_TYPE *pRT = &(pCh->sRT);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RT, "ENTER->\n");

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* the number of words transferred */
    pRT->u32RdByte = pDmaEntry->RequestedByteCount;

    memcpy(pDmaEntry->userBuf, pDeviceContext->pu8RtDmaTarget + pDmaEntry->copyOffset, pDmaEntry->RequestedByteCount);

    /* wake up the waiting process */
    pRT->u16EventCond++;

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(&pRT->waitqueueCallback, &pRT->waitqueueEvent);
}

/*******************************************************************************
 * Name:    dmaImpBcSetup
 *
 * Description:
 *      This function setup DMA transfer when messages are available for BC
 *      (IMP interrupt happens). DMA transfer will transfer messages
 *      into a temporary buf in BC driver.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * In   u32NumByte      Number of Bytes need to transfer
 * In   u32StartAddr    Absolute StartAddr in device (in byte)
 * In   pu8Buf          destination buffer address in driver
 * In   u32NumMsgs      Number of Msgs need to transfer
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaImpBcSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumByte,
    U32BIT u32StartAddr,
    U8BIT *pu8Buf,
    U32BIT u32NumMsgs
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "dmaImpBcSetup :ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "Chan: %d\n", u8Ch);

    /* channel */
    DmaQueueEntry.Channel = u8Ch;

    /* XFER byte count */
    DmaQueueEntry.ByteCount = u32NumByte;

    /* Start Address */
    DmaQueueEntry.StartAddr = u32StartAddr;

    /* buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pu8Buf;

    /* DMA from device */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;

    /*Num SGList entries */
    DmaQueueEntry.SgListNumEntries = 1;

    /* is this xfer even aligned? */
    if (!(DmaQueueEntry.StartAddr % 4))
    {
        /*DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "even alligned xfer!\n");    */
    }
    else
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "odd alligned xfer! Adjust!\n");

        /* DMA destination address -  buffer.  2 bytes back */
        DmaQueueEntry.BufAddr = DmaQueueEntry.BufAddr - 2; /* it will overwrite the mem in driver? by BZ */

        /* DMA SRC addr - Hardware */
        DmaQueueEntry.StartAddr = DmaQueueEntry.StartAddr - 2;
        DmaQueueEntry.ByteCount = DmaQueueEntry.ByteCount + 2;

        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "DMATransaction.BufAddr: %p\n", DmaQueueEntry.BufAddr);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "DMATransaction.StartAddr: %08x\n", DmaQueueEntry.StartAddr);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "DMATransaction.ByteCount: %08x\n", DmaQueueEntry.ByteCount);
    }

    DmaQueueEntry.u32DMACHBaseAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1);                             /* IMP MEM Start Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u32DMACHMaxAddr = (*pCh->pu32MemBA << 2) + (pCh->sImpRT.sImpCfgReg.u32TgtMemBA << 1) + (pCh->sImpRT.sImpCfgReg.u32TgtMemSzDWD << 2);    /* IMP MEM End Addr, 32-bit & 16-bit to 8-bit */
    DmaQueueEntry.u8DMAType = DMA_BC;

    DmaQueueEntry.U.BcParam.u32NumMsgXfer = u32NumMsgs;

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaImpBcCmplt
 *
 * Description:
 *      This function is called when Imp RT HBUF DMA transfer complete.
 *      i.e. Messages have been transfered into Driver Hbuf
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaImpBcCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pCh->sBC);
    U8BIT *pu8Buf = NULL;
    U32BIT u32NumMsgs = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_BC, "Ch %d BC DMA Cmplt\n", u8Ch);

    pu8Buf = (U8BIT *)pDmaEntry->BufAddr;
    u32NumMsgs = pDmaEntry->U.BcParam.u32NumMsgXfer;
    bcImpMsgProcess(pBC, pu8Buf, u32NumMsgs);

    /* re-start IMP */
    bcImpStart(pDeviceContext, (U16BIT)u8Ch);
}

/*******************************************************************************
 * Name:    dmaReplaySetup
 *
 * Description:
 *      This function setup DMA transfer when a block of replay data is ready
 *      to upload to device.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * In   u32NumByte      Number of Bytes need to transfer
 * In   u32StartAddr    Absolute StartAddr in device (in byte)
 * In   pu8SrcBuf       Source buffer address in driver
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaReplaySetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32NumByte,
    U32BIT u32StartAddr,
    U8BIT *pu8SrcBuf
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_REPLAY, "dmaReplauSetup :ENTER->\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_REPLAY, "ch%d: replay 0x%x [%d] bytes\n", u8Ch, u32NumByte, u32NumByte);

    /* copy data into DMA buffer */
    memcpy((char *)pDeviceContext->sDMA.pu8ReplayDMATarget, (char *)pu8SrcBuf, u32NumByte);

    /* set up transaction */
    DmaQueueEntry.Channel = u8Ch;                                           /* channel */
    DmaQueueEntry.ByteCount = u32NumByte;                                   /* XFER byte count */
    DmaQueueEntry.StartAddr = u32StartAddr;                                 /* Start Address */
    DmaQueueEntry.BufAddr = (void *)pDeviceContext->sDMA.pu8ReplayDMATarget; /* src data to DMA to device */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_TO_DEVICE;               /* to device */
    DmaQueueEntry.SgListNumEntries = 1;                                     /* Num SGList entries */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_REPLAY, "ch%d: chBA %08X, rplyBA %08X   rplySz %08X \n", u8Ch, (*pCh->pu32MemBA << 2), pCh->sBC.sReplayBuf.u32BaseAddr, pCh->sBC.sReplayBuf.u32MemSize);

    DmaQueueEntry.u32DMACHBaseAddr = pCh->sBC.sReplayBuf.u32BaseAddr;    /* byte address */
    DmaQueueEntry.u32DMACHMaxAddr = pCh->sBC.sReplayBuf.u32BaseAddr + pCh->sBC.sReplayBuf.u32MemSize;     /* byte address */
    DmaQueueEntry.u8DMAType = DMA_REPLAY;

    DmaQueueEntry.U.BcParam.u32NumMsgXfer = 0;

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaReplayCmplt
 *
 * Description:
 *      This function is called when replay DMA transfer completes.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaReplayCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u8Ch];
    struct _ACEX_1553_BC_TYPE *pBC = &(pCh->sBC);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_REPLAY, "Ch%d: Replay DMA Cmplt\n", u8Ch);

    pBC->sReplayBuf.wDmaCmpltEventCond = 1;
    DDC_WAKE_UP_INTERRUPTIBLE(&pBC->sReplayBuf.waitqueueDmaCmpltCallback, &pBC->sReplayBuf.waitqueueDmaCmpltEvent);
}

/*******************************************************************************
 * Name:    dmaARINC429RxFifoSetup
 *
 * Description:
 *
 *      This function initializes a DMA transfer when 429 RX Fifo data is available.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u32Channel              Channel
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte start address
 * In:  pu8Buf                  Buffer to hold messages
 * Out: none
 ******************************************************************************/
void dmaARINC429RxFifoSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT  *pu8Buf
)
{
    DMA_Q_ENTRY DmaQueueEntry;
    PDMA_Q_ENTRY pDMATransaction = &DmaQueueEntry;
    U32BIT u32MaxAddr;
    U32BIT u32RepeaterInUse = 0x00000000;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RX_FIFO, "Ch%d: RxFifo DMA Setup\n", u8Ch);

    /* setup DMA transaction */
    pDMATransaction->Channel            = u8Ch;                                         /* channel */
    pDMATransaction->ByteCount          = u32DataCountInBytes;                          /* XFER byte count */
    pDMATransaction->RequestedByteCount = u32DataCountInBytes;
    pDMATransaction->StartAddr          = u32ByteDeviceStartAddr;                       /* Start Address */
    pDMATransaction->userBuf            = pu8Buf;                                       /* user buffer address. dest address*/
    pDMATransaction->BufAddr            = pDeviceContext->ARINC429RxDMATarget;   /* DMA buffer */
    pDMATransaction->Direction          = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;             /* from device */
    pDMATransaction->SgListNumEntries   = 1;                                            /* Num SGList entries*/

    pDMATransaction->u32DMACHBaseAddr   = (*(pDeviceContext->pRxChnl429[u8Ch]->pu32MemBA) << 2);        /* byte address */

    /* if the channel is in repeater mode, there will be 4k less memory for storage */
    if (u8Ch <= 31)
    {
        u32RepeaterInUse = pDeviceContext->u32RepeaterActiveCh1to32 & (1 << u8Ch);
    }
    else
    {
        u32RepeaterInUse = pDeviceContext->u32RepeaterActiveCh32to64 & (1 << (u8Ch - 32));
    }

    /* the repeater must be in use AND capable of data pollution */
    if (u32RepeaterInUse && (pDeviceContext->sHwVersionInfo.dwCapabilities2 & HWVER_CAPABILITY2_ARINC_429_REPEATER_DATA_POLLUTION))
    {
        u32MaxAddr = (*(pDeviceContext->pRxChnl429[u8Ch]->pu32MemSize) - DD429_REPEATER_MODE_TABLE_SIZE );
    }
    else
    {
        u32MaxAddr = *(pDeviceContext->pRxChnl429[u8Ch]->pu32MemSize);
    }

    pDMATransaction->u32DMACHMaxAddr    = ((*(pDeviceContext->pRxChnl429[u8Ch]->pu32MemBA) + u32MaxAddr) << 2); /* byte address */
    pDMATransaction->u8DMAType          = DMA_429_RX_FIFO;

    /* alignment check */
    pDMATransaction->copyOffset         = 0;
    pDMATransaction->u32OddAlignment    = FALSE;
    if ((pDMATransaction->ByteCount % DMA_ALIGNMENT_BYTES) != 0)
    {
        /* In DD-40000 devices, FW will update pointers when the last byte is reached.
           This requires us to do a DMA and then an additional 32-bit read to complete
           the transaction if it is not 8-byte aligned. As such, we will DMA one less
           32-bit value and do a direct read on DMA Completion */
        pDMATransaction->ByteCount          =  (U32BIT)(pDMATransaction->ByteCount  - sizeof(U32BIT));
        pDMATransaction->RequestedByteCount  = (U32BIT)(pDMATransaction->RequestedByteCount  - sizeof(U32BIT));
        pDMATransaction->u32OddAlignment     = TRUE;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RX_FIFO, "Arinc Rx 429 ch%d: chBA %08X, startAddr %08X   byteSz %08X \n",
        u8Ch, DmaQueueEntry.u32DMACHBaseAddr, DmaQueueEntry.StartAddr, DmaQueueEntry.ByteCount);

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, pDMATransaction);
}

/*******************************************************************************
 * Name:    dmaARINC429RxFifoCmplt
 *
 * Description:
 *      This function is called when 429 RX Fifo DMA transfer complete.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaARINC429RxFifoCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    U32BIT u32HeadTailRegisterAddress;
    U32BIT u32HeadTailRegisterValue;
    U32BIT u32Tail;
    U32BIT u32Data;
    U32BIT u32DestIndex;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_RX_FIFO, "Ch%d: RxFifo DMA Cmplt\n", u8Ch);

    /* check if we need to do a direct memory read */
    if (pDmaEntry->u32OddAlignment)
    {
    	U32BIT u32Addr;

        /* In DD-40000 devices, FW will update pointers when the last byte is reached.
           This requires us to do a DMA and then an additional 32-bit read to complete
           the transaction if it is not 8-byte aligned. As such, we will DMA one less
           32-bit value and do a direct read on DMA Completion */

        u32HeadTailRegisterAddress = *(pDeviceContext->pRxChnl429[u8Ch]->pu32RegBA) + ACEX_429_RX_FIFO_HEAD_TAIL_REG;
        DDC_REG_READ(pDeviceContext, u32HeadTailRegisterAddress, &u32HeadTailRegisterValue);

        u32Tail = (u32HeadTailRegisterValue & DD429_RX_HEAD_TAIL_MASK__TAIL) >> DD429_RX_HEAD_TAIL_OFFSET__TAIL;
	    u32Addr = (pDmaEntry->u32DMACHBaseAddr >> 2) + u32Tail;

        DDC_MEM_READ(pDeviceContext, u32Addr, &u32Data, ACEX_32_BIT_ACCESS);
    }

    /* copy data to user */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *) pDeviceContext->ARINC429RxDMATarget;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < pDmaEntry->RequestedByteCount / sizeof(U32BIT); byteSwapIndex++)
        {
            pData[byteSwapIndex] = DDC_BYTE_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

    memcpy((void*)pDmaEntry->userBuf, (void*)pDeviceContext->ARINC429RxDMATarget, pDmaEntry->RequestedByteCount);

    if (pDmaEntry->u32OddAlignment)
    {
        U32BIT *pBuffer = (U32BIT *) pDmaEntry->userBuf;

        /* convert # of bytes to 32-bit value index */
        u32DestIndex = (pDmaEntry->RequestedByteCount >> 2);

        pBuffer[u32DestIndex] = u32Data;
    }

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* wake up waiting thread */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);
    pDeviceContext->pRxChnl429[u8Ch]->u16RxFifoEventCond++;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->pRxChnl429[u8Ch]->waitqueueARINC429RxFifoCallback,
        &pDeviceContext->pRxChnl429[u8Ch]->waitqueueARINC429RxFifoEvent);
}

/*******************************************************************************
 * Name:    dmaARINC429SetTxFrameControlSetup
 *
 * Description:
 *
 *      This function initializes a DMA transfer when 429 TX data is available.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u8Ch                    Channel number
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte start address
 * In:  pu8Buf                  point to the data to DMA to device
 * Out: none
 ******************************************************************************/
void dmaARINC429SetTxFrameControlSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8Buf
)
{
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_TX_FRAME, "Ch%d: SetTxFrameControl DMA Setup\n", u8Ch);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *)pu8Buf;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < u32DataCountInBytes / sizeof(U32BIT); byteSwapIndex++)
        {
            pData[byteSwapIndex] = DDC_BYTE_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

    /* copy data into DMA buffer */
    memcpy((char *)pDeviceContext->ARINC429TxDMATarget, (char *)pu8Buf, u32DataCountInBytes);

    /* setup DMA transaction */
    DmaQueueEntry.Channel = u8Ch;                                                    /* channel */
    DmaQueueEntry.ByteCount = u32DataCountInBytes;                                   /* XFER byte count */
    DmaQueueEntry.StartAddr = u32ByteDeviceStartAddr;                                /* Start Address */
    DmaQueueEntry.BufAddr = (void *)pDeviceContext->ARINC429TxDMATarget;             /* DMA buffer */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_TO_DEVICE;                        /* to device */
    DmaQueueEntry.SgListNumEntries = 1;                                              /* Num SGList entries */

    DmaQueueEntry.u32DMACHBaseAddr = (*(pDeviceContext->pTxChnl429[u8Ch]->pu32MemBA) << 2);     /* byte address */
    DmaQueueEntry.u32DMACHMaxAddr = ((*(pDeviceContext->pTxChnl429[u8Ch]->pu32MemBA) +
            *(pDeviceContext->pTxChnl429[u8Ch]->pu32MemSize)) << 2);                            /* byte address */
    DmaQueueEntry.u8DMAType = DMA_429_SET_TX_FRAME_CONTROL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_TX_FRAME, "Arinc 429 Tx ch%d: chBA %08X, startAddr %08X   byteSz %08X \n",
        u8Ch, DmaQueueEntry.u32DMACHBaseAddr, DmaQueueEntry.StartAddr, DmaQueueEntry.ByteCount);

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaARINC429SetTxFrameControlCmplt
 *
 * Description:
 *      This function is called when 429 SetTxFrameControl DMA transfer complete.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaARINC429SetTxFrameControlCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_TX_FRAME, "Ch%d: SetTxFrameControl DMA Cmplt\n", u8Ch);

    /* wake up waiting thread */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);
    pDeviceContext->pTxChnl429[u8Ch]->u16ARINC429SetTxFrameControlEventCond++;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->pTxChnl429[u8Ch]->waitqueueARINC429SetTxFrameControlCallback,
        &pDeviceContext->pTxChnl429[u8Ch]->waitqueueARINC429SetTxFrameControlEvent);
}

/*******************************************************************************
 * Name:    dmaArinc429VoltageMonitoringSetup
 *
 * Description:
 *
 *      This function initializes a DMA transfer when Voltage Monitoring data
 *      is available.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte start address
 * In:  pu8Buf                  Buffer to DMA data from device
 * Out: none
 ******************************************************************************/
void dmaArinc429VoltageMonitoringSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8Buf
)
{
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_VOLTAGE_MON, "Arinc429VoltageMonitoring DMA Setup\n");

    DmaQueueEntry.Channel = 0;                                                       /* channel */
    DmaQueueEntry.ByteCount = u32DataCountInBytes;                                   /* XFER byte count */
    DmaQueueEntry.RequestedByteCount = u32DataCountInBytes;                          /* XFER byte count */
    DmaQueueEntry.StartAddr = u32ByteDeviceStartAddr;                                /* Start Address */
    DmaQueueEntry.userBuf = pu8Buf;                                                  /* user buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pDeviceContext->Arinc429VoltageMonDMATarget;             /* buffer address. dest address */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;                      /* from device */
    DmaQueueEntry.SgListNumEntries = 1;                                              /* Num SGList entries */

    DmaQueueEntry.u32DMACHBaseAddr = (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemBA) << 2);           /* byte address */
    DmaQueueEntry.u32DMACHMaxAddr = ((*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemBA) +
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemSize)) << 2);                                     /* byte address */
    DmaQueueEntry.u8DMAType = DMA_429_VOLTAGE_MONITORING;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_VOLTAGE_MON, "Voltage Mon: chBA %08X, startAddr %08X   byteSz %08X \n",
        DmaQueueEntry.u32DMACHBaseAddr, DmaQueueEntry.StartAddr, DmaQueueEntry.ByteCount);

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaArinc429VoltageMonitoringCmplt
 *
 * Description:
 *      This function is called when Voltage Monitoring DMA transfer complete.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaArinc429VoltageMonitoringCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDmaEntry
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_VOLTAGE_MON, "Arinc429VoltageMonitoring DMA Cmplt\n");

    /* copy data to user */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *) pDeviceContext->Arinc429VoltageMonDMATarget;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < pDmaEntry->RequestedByteCount / sizeof(U32BIT); byteSwapIndex++)
        {
            pData[byteSwapIndex] = DDC_BYTE_ORDER_L(pData[byteSwapIndex]);

            /* the data is stored as 16-bits, but read out as 32-bits, we need to do a word swap now */
            pData[byteSwapIndex] = DDC_WORD_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

    memcpy((void*)pDmaEntry->userBuf, (void*)pDeviceContext->Arinc429VoltageMonDMATarget, pDmaEntry->RequestedByteCount);
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* wake up waiting thread */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);
    pDeviceContext->u16Arinc429VoltageMonitoringEventCond++;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->waitqueueArinc429VoltageMonitoringCallback,
        &pDeviceContext->waitqueueArinc429VoltageMonitoringEvent);
}

/*******************************************************************************
 * Name:    dmaARINC717RxFrameSetup
 *
 * Description:
 *
 *      This function initializes a DMA transfer when 717 RX data is available.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u8Ch                    Channel number
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte start address
 * In:  pu8Buf                  Buffer to DMA into
 * Out: none
 ******************************************************************************/
void dmaARINC717RxFrameSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8Buf
)
{
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_RX, "Ch%d: ARINC 717 Rx Frame DMA Setup\n", u8Ch);

    /* setup DMA transaction */
    DmaQueueEntry.Channel = u8Ch;                                                    /* channel */
    DmaQueueEntry.ByteCount = u32DataCountInBytes;                                   /* XFER byte count */
    DmaQueueEntry.RequestedByteCount = u32DataCountInBytes;
    DmaQueueEntry.StartAddr = u32ByteDeviceStartAddr;                                /* Start Address */
    DmaQueueEntry.userBuf = pu8Buf;                                                  /* user buffer address. dest address */
    DmaQueueEntry.BufAddr = (void *)pDeviceContext->ARINC717RxDMATarget;             /* buffer address. dest address */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE;                      /* from device */
    DmaQueueEntry.SgListNumEntries = 1;                                              /* Num SGList entries */

    DmaQueueEntry.u32DMACHBaseAddr = (*(pDeviceContext->sChannelArinc717[u8Ch].pu32MemBA) << 2);        /* byte address */
    DmaQueueEntry.u32DMACHMaxAddr = ((*(pDeviceContext->sChannelArinc717[u8Ch].pu32MemBA) +
            *(pDeviceContext->sChannelArinc717[u8Ch].pu32MemSize)) << 2);                                  /* byte address */
    DmaQueueEntry.u8DMAType = DMA_717_RX_FRAME;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_RX, "Arinc 717 Rx ch%d: chBA %08X, startAddr %08X   byteSz %08X \n",
        u8Ch, DmaQueueEntry.u32DMACHBaseAddr, DmaQueueEntry.StartAddr, DmaQueueEntry.ByteCount);

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaARINC717RxFrameCmplt
 *
 * Description:
 *      This function is called when 717 RX Frame DMA transfer complete.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel number
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaARINC717RxFrameCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_RX, "Ch%d: ARINC 717 Rx Frame DMA Cmplt\n", u8Ch);

    /* copy data to user */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *) pDeviceContext->ARINC717RxDMATarget;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < pDmaEntry->RequestedByteCount / sizeof(U32BIT); byteSwapIndex++)
        {
            pData[byteSwapIndex] = DDC_BYTE_ORDER_L(pData[byteSwapIndex]);

            /* the data is stored as 16-bits, but read out as 32-bits, we need to do a word swap now */
            pData[byteSwapIndex] = DDC_WORD_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

    memcpy((void*)pDmaEntry->userBuf, (void*)pDeviceContext->ARINC717RxDMATarget, pDmaEntry->RequestedByteCount);
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* wake up waiting thread */
    pDeviceContext->u16ARINC717RxFrameEventCond[u8Ch] = 1;

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->waitqueueARINC717RxFrameCallback[u8Ch],
        &pDeviceContext->waitqueueARINC717RxFrameEvent[u8Ch]);
}

/*******************************************************************************
 * Name:    dmaARINC717TxFrameSetup
 *
 * Description:
 *
 *      This function initializes a DMA transfer when 717 TX data is available.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u8Ch                    Channel number
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte destination address
 * In:  pu8Buf                  point to the data to DMA to device
 * Out: none
 ******************************************************************************/
void dmaARINC717TxFrameSetup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT  *pu8Buf
)
{
    DMA_Q_ENTRY DmaQueueEntry;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_TX, "Ch%d: ARINC 717 Tx Frame DMA Setup\n", u8Ch);

#if DDC_PPC
    {
        U32BIT *pData = (U32BIT *) pu8Buf;
        int byteSwapIndex;

        /* byte-swap data */
        for (byteSwapIndex = 0; byteSwapIndex < u32DataCountInBytes / sizeof(U32BIT); byteSwapIndex++)
        {
            pData[byteSwapIndex] = DDC_BYTE_ORDER_L(pData[byteSwapIndex]);

            /* the data is stored as 16-bits, but written out as 32-bits, we need to do a word swap now */
            pData[byteSwapIndex] = DDC_WORD_ORDER_L(pData[byteSwapIndex]);
        }
    }
#endif /* DDC_PPC */

    /* copy data into DMA buffer */
    memcpy((char *)pDeviceContext->ARINC717TxDMATarget, (char *)pu8Buf, u32DataCountInBytes);

    /* setup DMA transaction */
    DmaQueueEntry.Channel = u8Ch;                                                    /* channel */
    DmaQueueEntry.ByteCount = u32DataCountInBytes;                                   /* XFER byte count */
    DmaQueueEntry.StartAddr = u32ByteDeviceStartAddr;                                /* Start Address */
    DmaQueueEntry.BufAddr = (void *)pDeviceContext->ARINC717TxDMATarget;             /* source address */
    DmaQueueEntry.Direction = DESCRIPTOR_POINTER__DIR_OF_XFER_TO_DEVICE;                        /* to device */
    DmaQueueEntry.SgListNumEntries = 1;                                              /* Num SGList entries */

    DmaQueueEntry.u32DMACHBaseAddr = u32ByteDeviceStartAddr;                                                             /* byte address */
    DmaQueueEntry.u32DMACHMaxAddr = u32ByteDeviceStartAddr + ((*pDeviceContext->sChannelArinc717[u8Ch].pu32MemSize) << 2);       /* byte address */
    DmaQueueEntry.u8DMAType = DMA_717_TX_FRAME;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_TX, "Arinc 717 Tx ch%d: chBA %08X, startAddr %08X   byteSz %08X \n",
        u8Ch, DmaQueueEntry.u32DMACHBaseAddr, DmaQueueEntry.StartAddr, DmaQueueEntry.ByteCount);

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaARINC717TxFrameCmplt
 *
 * Description:
 *      This function is called when 717 ARINC717TxFrame DMA transfer completes.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaARINC717TxFrameCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_717_TX, "Ch%d: SetTxFrameControl DMA Cmplt\n", u8Ch);

    /* wake up waiting thread */
    pDeviceContext->u16ARINC717TxFrameEventCond[u8Ch] = 1;

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->waitqueueARINC717TxFrameCallback[u8Ch],
        &pDeviceContext->waitqueueARINC717TxFrameEvent[u8Ch]);
}

/*******************************************************************************
 * Name:    dmaStartDioTt
 *
 * Description:
 *
 *      This function initializes a DMA transfer when reading the
 *      card level DIO TT memory region.
 *
 * In:  pDeviceContext          Device-specific structure
 * In:  u32Channel              Channel
 * In:  u32DataCountInBytes     Number of bytes to transfer
 * In:  u32ByteDeviceStartAddr  Byte start address
 * In:  pu8HostDestBuffer       Buffer to DMA into
 * Out: none
 ******************************************************************************/
void dmaStartDioTt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DataCountInBytes,
    U32BIT u32ByteDeviceStartAddr,
    U8BIT *pu8HostDestBuffer
)
{
    DMA_Q_ENTRY DmaQueueEntry;

    DmaQueueEntry.u8DMAType = DMA_TYPE_DIO_TT;
    DmaQueueEntry.userBuf = pu8HostDestBuffer;
    DmaQueueEntry.ByteCount = u32DataCountInBytes;

    DmaQueueEntry.StartAddr = u32ByteDeviceStartAddr;
    DmaQueueEntry.u32DMACHBaseAddr = (*(pDeviceContext->sDioTt.pu32MemBA) << 2);
    DmaQueueEntry.u32DMACHMaxAddr = ((*(pDeviceContext->sDioTt.pu32MemBA) + *(pDeviceContext->sDioTt.pu32MemSize)) << 2);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_DIO_TT, "DmaStartDioTt: call dmaQueueAndStart\n");

    /* EnQueue DMA */
    dmaEnQueue(pDeviceContext, &DmaQueueEntry);
}

/*******************************************************************************
 * Name:    dmaDioTtCmplt
 *
 * Description:
 *      This function is called when DIO Time Tag DMA transfer completes.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8Ch            Channel ID
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void dmaDioTtCmplt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch,
    DMA_Q_ENTRY *pDmaEntry
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_DIO_TT, "Ch%d: dmaDioTtCmplt DMA Cmplt\n", u8Ch);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    memcpy(pDmaEntry->userBuf, pDeviceContext->sDioTt.pu8DmaTarget, pDmaEntry->ByteCount);
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    /* wake up the waiting process */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);
    pDeviceContext->sDioTt.u16DmaCmpltEventCond++;
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);

    DDC_WAKE_UP_INTERRUPTIBLE(
        &pDeviceContext->sDioTt.dmaCmpltWaitqueueCallback,
        &pDeviceContext->sDioTt.dmaCmpltWaitqueueEvent);
}

/*******************************************************************************
 * Name:    dmaCmpltHandler
 *
 * Description:
 *      This function is called when DMA interrupt happened.
 *      It checks who is doing DMA and calls the corresponding completion
 *      function for further process.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void dmaCmpltHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    enum ddc_dma_data_direction eDirection;

    U16BIT u16DmaType = 0;
    U8BIT u16DmaChannel = 0; /* changed from U16 since only used as U8 */

    DMA_Q_ENTRY *pDmaQueueEntry = NULL;

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pDmaQueueEntry = (DMA_Q_ENTRY *) Q__INFO_TYPE_Remove(pDeviceContext->sDMA.pQueuePtr);

    if (pDmaQueueEntry == NULL) /* cannot get entry */
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_COMPLETE_WORK_ITEM, "Can't find entry in DMA Q\n");
        return;
    }

    memcpy(&pDeviceContext->sDMA.queueEntryCurrentTransaction, pDmaQueueEntry, sizeof(DMA_Q_ENTRY));

    /* clear that entry */
    memset(pDmaQueueEntry, 0, sizeof(DMA_Q_ENTRY));

    /* clear current DMA channel */
    pDeviceContext->sDMA.u16CurrentChannel = 0;

    u16DmaType = (U16BIT)(pDeviceContext->sDMA.queueEntryCurrentTransaction.u8DMAType & 0x0000ffff);
    u16DmaChannel = (U8BIT)pDeviceContext->sDMA.queueEntryCurrentTransaction.Channel;

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_COMPLETE_WORK_ITEM, "DMA COMPLETE: byte %d\n", pDeviceContext->sDMA.queueEntryCurrentTransaction.RequestedByteCount);

    switch (u16DmaType)
    {
        case DMA_MT:
        {
            dmaMtCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_BC: /* DMA for IMP BC */
        {
            dmaImpBcCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_REPLAY: /* DMA for replay */
        {
            dmaReplayCmplt(pDeviceContext, u16DmaChannel);
            eDirection = DDC_DMA_TO_DEVICE;
            break;
        }

        case DMA_MRT_HBUF: /* DMA for IMP RT */
        {
            dmaImpRtHbufCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_MRT_CMDDATA: /* DMA for IMP RT */
        {
            dmaImpRtCmdDataCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_429_RX_FIFO: /* DMA for 429_RX_FIFO */
        {
            dmaARINC429RxFifoCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_429_SET_TX_FRAME_CONTROL: /* DMA for 429_SET_TX_FRAME_CONTROL */
        {
            dmaARINC429SetTxFrameControlCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_TO_DEVICE;
            break;
        }

        case DMA_429_VOLTAGE_MONITORING: /* DMA for 429_VOLTAGE_MONITORING */
        {
            dmaArinc429VoltageMonitoringCmplt(pDeviceContext, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_717_RX_FRAME: /* DMA for 717_TX_FRAME */
        {
            dmaARINC717RxFrameCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        case DMA_717_TX_FRAME: /* DMA for 717_TX_FRAME */
        {
            dmaARINC717TxFrameCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_TO_DEVICE;
            break;
        }

        case DMA_TYPE_DIO_TT:
        {
            dmaDioTtCmplt(pDeviceContext, u16DmaChannel, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }

        default:
        {
            /* error: doing nothing */
            eDirection = DDC_DMA_FROM_DEVICE;
            break;
        }
    }

    if (pDeviceContext->sDMA.dmaAddr)
    {
        ddcUldOsPciDmaUnmap(
            pDeviceContext,
            pDeviceContext->sDMA.dmaAddr,
            pDeviceContext->sDMA.queueEntryCurrentTransaction.ByteCount,
            eDirection);

        pDeviceContext->sDMA.dmaAddr = 0;
    }

    /* check if there is another DMA entry */
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    pDmaQueueEntry = (DMA_Q_ENTRY *) Q__INFO_TYPE_Read(pDeviceContext->sDMA.pQueuePtr);

    if (pDmaQueueEntry != NULL) /* there is still DMA request waiting in DMA queue, continue processing it */
    {
        memcpy(&pDeviceContext->sDMA.queueEntryCurrentTransaction, pDmaQueueEntry, sizeof(DMA_Q_ENTRY));
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        /* execute DMA */
        switch (pDeviceContext->u16DriverType)
        {
            case ACEX_PCIE_DRIVER:
            case ACEX_DD429_PCIE_DRIVER:
            {
                dmaExecEntryPCIe(pDeviceContext, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
                break;
            }

            default:
            {
                dmaExecEntry(pDeviceContext, &pDeviceContext->sDMA.queueEntryCurrentTransaction);
                break;
            }
        }
    }
    else
    {
        pDeviceContext->sDMA.u16State = ACEX_DMA_IDLE;
        pDeviceContext->sDMA.u16CurrentChannel = 0;

        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    }
}

/* ========================================================================== */
/* ========================================================================== */
/*                           QPRIME - ASIC                                    */
/* ========================================================================== */
/* ========================================================================== */

/*******************************************************************************
 * Name:    dmaStartQPRM
 *
 * Description:
 *      This function configure the registers needed for DMA transfer
 *      and start DMA transfer.
 *
 * In   pDeviceContext  - device-specific structure
 *
 * Returns: none
 ******************************************************************************/
DDC_LOCAL void dmaStartQPRM
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegValue;
    U32BIT ptr;

    /* DMA Mode Register - Enable Scatter/Gather Mode, Interrupt On Done, and route interrupts to PCI */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_MODE, &u32RegValue);
    u32RegValue |= (QPRIME_DMA_MODE_SG_MODE | QPRIME_DMA_MODE_DONE_INT_ENABLE);
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_MODE, &u32RegValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_QPRM_DMA_MODE = 0x%X\n", u32RegValue);

    /* DMA Command/Status Register - Enable PCI interrupts and DMA Channel 1 interrupts */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_CS, &u32RegValue);
    u32RegValue |= QPRIME_DMA_CS_ENABLE;
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_CS, &u32RegValue);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_QPRM_DMA_CS = 0x%x\n", u32RegValue);


    /* DMA Descriptor Pointer Register - Write the base LOGICAL address of the DMA_TRANSFER_ELEMENT list */
    DDC_VIRT_TO_PHYS(pDeviceContext, pDeviceContext->sDMA.pu32Descriptor, &ptr);
    ptr = (U32BIT) DESCRIPTOR_POINTER_ADDR(ptr);
    ptr = (ptr << 4);
    ptr |= PLX_DMA_DESC_PTR_PCI_ADDR;
    /*DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_QPRM_DMA_DSC_PTR ADDR = %x\n",
        (U32BIT)(DESCRIPTOR_POINTER_ADDR(pDeviceContext->sDMA.pu32Descriptor))); */

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_DSC_PTR, &ptr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "write REG_QPRM_DMA_DSC_PTR = 0x%x\n", ptr);


    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "Starting a DMA operation!\n");

    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_CS, &u32RegValue);
    u32RegValue |= (QPRIME_DMA_CS_ENABLE | QPRIME_DMA_CS_START);
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + REG_QPRM_DMA_CS, &u32RegValue);

}


/* ========================================================================== */
/* ========================================================================== */
/*                               PCIe DMA                                     */
/* ========================================================================== */
/* ========================================================================== */

/*******************************************************************************
 * Name:    dmaStartPCIe
 *
 * Description:
 *      This function configure the registers needed for DMA transfer
 *      and start DMA transfer.
 *
 *      The rollover addresses have been previously been written by the
 *      dmaExecEntryPCIe function and do not need to be written again. Only
 *      the new address and size need to be updated.
 *
 * In   pDeviceContext  - device-specific structure
 *
 * Returns: none
 ******************************************************************************/
DDC_LOCAL void dmaStartPCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT ptr = 0;
    U32BIT u32_00000000 = 0x00000000;
    U32BIT u32Size;

    DDC_VIRT_TO_PHYS(pDeviceContext, pDeviceContext->sDMA.pu32Descriptor, &ptr);

    u32Size = pDeviceContext->sDMA.u32TransferSize | DMA_START_PCIE;

    if (pDeviceContext->sDMA.u32Direction == DESCRIPTOR_POINTER__DIR_OF_XFER_TO_DEVICE)
    {
        /* DMA to device */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "DMA to device: write Descriptor Pointer ADDR = 0x%x\n",
              DESCRIPTOR_POINTER_PCIE_ADDR(ptr));
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "Starting DMA to device operation!\n");

        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_HOST_LSB_ADDR_PCIE, &ptr);
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_HOST_MSB_ADDR_PCIE, &u32_00000000);
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_XFER_SIZE_PCIE, &u32Size);
    }
    else
    {
        /* DMA from device */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "DMA from device: write Descriptor Pointer ADDR = 0x%x\n",
            DESCRIPTOR_POINTER_PCIE_ADDR(ptr));
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_START, "Starting DMA from device operation!\n");

        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_HOST_LSB_ADDR_PCIE, &ptr);
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_HOST_MSB_ADDR_PCIE, &u32_00000000);
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_XFER_SIZE_PCIE, &u32Size);
    }
}

/*******************************************************************************
 * Name:    dmaExecEntryPCIe
 *
 * Description:
 *      This function constructs DMA_TRANSFER_ELEMENT according to
 *      the DMA request entry. DMA_TRANSFER_ELEMENT has the information
 *      needed for hardware to execute DMA transfer.
 *
 * Note:
 *      Rollovers are handled by hardware, thus eliminating the need to do a
 *      check for it like the PLX. Therefore we just need to set the start,
 *      end, and current addresses.
 *
 * In   pDeviceContext      device-specific structure
 * In   pDMATransaction     the DMA request entry
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
DDC_LOCAL U8BIT dmaExecEntryPCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DMA_Q_ENTRY *pDMATransaction
)
{
    enum ddc_dma_data_direction eDirection;

    DMA_TRANSFER_ELEMENT_PCIE *dteVirtualAddress;
    U32BIT dtePhysicalAddress = 0;
    U32BIT u32StartAddr;

#if DDC_PPC
    DMA_TRANSFER_ELEMENT_PCIE * dteVirtualAddressTemp;
    U32BIT dtePhysicalAddressTemp;
#endif /* DDC_PPC */

    U32BIT u32_00000000 = 0x00000000;
    U32BIT u32_FFFFFFFF = 0xFFFFFFFF;
    U32BIT u32Channel = 0;
    U32BIT u32TransferSize = 0;
    BOOLEAN bEndofChan = FALSE;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "pDMATransaction->Channel = %d\n", pDMATransaction->Channel);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "pDMATransaction->ByteCount = 0x%x\n", pDMATransaction->ByteCount);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "pDMATransaction->StartAddr = 0x%x\n", pDMATransaction->StartAddr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "pDMATransaction->BufAddr = %p\n", pDMATransaction->BufAddr);
    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "pDMATransaction->SgListNumEntries = %d\n", pDMATransaction->SgListNumEntries);

    /* Setup the pointer to the next DMA_TRANSFER_ELEMENT   */
    /* for both virtual and physical address references.    */
    dteVirtualAddress = (DMA_TRANSFER_ELEMENT_PCIE *)pDeviceContext->sDMA.pu32Descriptor;
    DDC_VIRT_TO_PHYS(pDeviceContext, dteVirtualAddress, &dtePhysicalAddress);
    dtePhysicalAddress = (U32BIT)(dtePhysicalAddress + (sizeof(DMA_TRANSFER_ELEMENT_PCIE)));

    /* Translate the System's SCATTER_GATHER_LIST elements  */
    /* into the device's DMA_TRANSFER_ELEMENT elements      */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "constructing DTE!\n");

    u32Channel = pDMATransaction->Channel;

    /* map DMA address */
    if (pDMATransaction->Direction == DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE)
    {
        eDirection = DDC_DMA_FROM_DEVICE;
    }
    else
    {
        eDirection = DDC_DMA_TO_DEVICE;
    }

    pDeviceContext->sDMA.dmaAddr = ddcUldOsPciDmaMap(
        pDeviceContext,
        (void *)(pDMATransaction->BufAddr),
        pDMATransaction->ByteCount,
        eDirection);

    /* construct the DTE (DMA_TRANSFER_ELEMENT) */

    dteVirtualAddress->PciAddressLow = pDeviceContext->sDMA.dmaAddr;

    if (!(dteVirtualAddress->PciAddressLow))
    {
        /* Flag that all buffers are used.  MAP FAILED! */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "DMA Map Fail\n");
        pDeviceContext->pChannel1553[u32Channel]->sMT.bMtiDeviceOverflow = TRUE;
    }

    u32StartAddr = pDMATransaction->StartAddr;

    if (u32StartAddr >= pDMATransaction->u32DMACHMaxAddr)
    {
        u32StartAddr = pDMATransaction->u32DMACHBaseAddr + (u32StartAddr - pDMATransaction->u32DMACHMaxAddr);
    }

    if (pDMATransaction->Direction == DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE)
    {
        /* DMA from device */
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_PCIE, &u32StartAddr);

        /* set the rollover start and end addresses */
        if ((pDMATransaction->u8DMAType == DMA_MT) ||
            (pDMATransaction->u8DMAType == DMA_429_RX_FIFO))
        {
            /* MTI has rollover */
            DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_START_PCIE, &pDMATransaction->u32DMACHBaseAddr);
            DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_END_PCIE, &pDMATransaction->u32DMACHMaxAddr);
        }
        else
        {
            /* RT Improvements does not roll over */
            DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_START_PCIE, &u32_00000000);
            DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_ROLLOVER_END_PCIE, &u32_FFFFFFFF);
        }
    }
    else
    {
        /* DMA to device */
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_LOCAL_ADDR_PCIE, &u32StartAddr);

        /* no rollovers */
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_LOCAL_ADDR_ROLLOVER_START_PCIE, &u32_00000000);
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_LOCAL_ADDR_ROLLOVER_END_PCIE, &u32_FFFFFFFF);
    }

    /* Transfer up to the maximum allowed (PLDA EZDMA is configured to 4K transfers max) */
    /* dmaContinuePCIe performs additional transfers if required */
    u32TransferSize = pDMATransaction->ByteCount;

    if (u32TransferSize > DMA_MAX_TRANSFER_SIZE_PCIE)
    {
        u32TransferSize = DMA_MAX_TRANSFER_SIZE_PCIE;

        /* since this transfer is <= a page, this will be the only transfer */

        /* indicate the end of chain bit needs to be set */
        bEndofChan = TRUE;
    }

    pDMATransaction->totalBytesTransfered = u32TransferSize;

    dteVirtualAddress->PciAddressHigh = 0;
    dteVirtualAddress->TransferSize = u32TransferSize;
    dteVirtualAddress->DmaNextDescriptorPointerLSB = (DESCRIPTOR_POINTER_PCIE_ADDR(dtePhysicalAddress) << 2);
    dteVirtualAddress->DmaNextDescriptorPointerMSB = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "constructing DTE....built!\n");

    if (bEndofChan)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "last entry!\n");
        dteVirtualAddress->DmaNextDescriptorPointerLSB |= DESCRIPTOR_POINTER_PCIE__END_OF_CHAIN_TRUE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXEC_WORK_ITEM, "[DTE0] PciAddressLow:%08X  TransferSize:%-5d  NextDescriptorPointer:%08X:%08X\n",
        dteVirtualAddress->PciAddressLow,
        dteVirtualAddress->TransferSize,
        dteVirtualAddress->DmaNextDescriptorPointerMSB,
        dteVirtualAddress->DmaNextDescriptorPointerLSB);

#if DDC_PPC
{
    U32BIT i;

    /*adjust endianess of descriptor block*/

    dteVirtualAddressTemp = (DMA_TRANSFER_ELEMENT_PCIE *)pDeviceContext->sDMA.pu32Descriptor;
    DDC_VIRT_TO_PHYS(pDeviceContext, dteVirtualAddress, &dtePhysicalAddressTemp);
    dtePhysicalAddressTemp = (U32BIT)(dtePhysicalAddressTemp + sizeof(DMA_TRANSFER_ELEMENT_PCIE));

    for (i = 0; i < pDMATransaction->SgListNumEntries; i++)
    {
        /* adjust endianness of descriptor */
        dteVirtualAddressTemp->PciAddressLow = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->PciAddressLow);

        /* dteVirtualAddressTemp->PciAddressHigh is always 0, so there is no need to swap */
        dteVirtualAddressTemp->TransferSize = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->TransferSize);
        dteVirtualAddressTemp->DmaNextDescriptorPointerLSB = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->DmaNextDescriptorPointerLSB);
        dteVirtualAddressTemp->DmaNextDescriptorPointerMSB = DDC_BYTE_ORDER_L(dteVirtualAddressTemp->DmaNextDescriptorPointerMSB);

        /*Adjust the next DMA_TRANSFER_ELEMEMT */
        dteVirtualAddressTemp++;
        dtePhysicalAddressTemp += (U32BIT)sizeof(DMA_TRANSFER_ELEMENT_PCIE);
    }
}
#endif /* DDC_PPC */

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    pDeviceContext->sDMA.u16CurrentChannel = (U16BIT)(pDMATransaction->Channel + 1);

    pDeviceContext->sDMA.u32TransferSize = u32TransferSize;
    pDeviceContext->sDMA.u32Direction = pDMATransaction->Direction;

    DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_EXECUTE_WORK_ITEM, "byte total %d,  byte %d\n", pDMATransaction->totalBytesTransfered, pDeviceContext->sDMA.u32TransferSize);

    /* Start the DMA operation */
    dmaStartPCIe(pDeviceContext);

    return TRUE;
}

/* ========================================================================== */
/*
    This function is only called when there is more than 4k of data to
    transfer. The previous transaction was 4k. This transfer may also be 4k.
    If the transfer is less than 4k, this will be the last one.
*/
/* ========================================================================== */
void dmaContinuePCIe
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DMA_TRANSFER_ELEMENT_PCIE *dteVirtualAddress = (DMA_TRANSFER_ELEMENT_PCIE *)pDeviceContext->sDMA.pu32Descriptor;
    DMA_Q_ENTRY *pDMATransaction = &pDeviceContext->sDMA.queueEntryCurrentTransaction;
    U32BIT TransferSize;
    U32BIT LocalAddressStart;
    /*
        To this point, the previous DMA must have transferred DMA_MAX_TRANSFER_SIZE bytes
        As such, user data pointer will be added DMA_MAX_TRANSFER_SIZE
    */

#if DDC_PPC
    /* unswap value */
    dteVirtualAddress->PciAddressLow = DDC_BYTE_ORDER_L(dteVirtualAddress->PciAddressLow);
#endif /* DDC_PPC */

    dteVirtualAddress->PciAddressLow += DMA_MAX_TRANSFER_SIZE_PCIE;

#if DDC_PPC
    /* swap back */
    dteVirtualAddress->PciAddressLow = DDC_BYTE_ORDER_L(dteVirtualAddress->PciAddressLow);
#endif /* DDC_PPC */


    /* transfer all the remaining data */
    TransferSize = pDMATransaction->ByteCount - pDMATransaction->totalBytesTransfered;

    if (TransferSize > DMA_MAX_TRANSFER_SIZE_PCIE)
    {
        /* another max size transfer */
        TransferSize = DMA_MAX_TRANSFER_SIZE_PCIE;
    }

    LocalAddressStart = pDMATransaction->StartAddr + pDMATransaction->totalBytesTransfered;

    if (LocalAddressStart >= pDMATransaction->u32DMACHMaxAddr)
    {
        LocalAddressStart = pDMATransaction->u32DMACHBaseAddr + (LocalAddressStart - pDMATransaction->u32DMACHMaxAddr);
    }

    if (pDMATransaction->Direction == DESCRIPTOR_POINTER__DIR_OF_XFER_FROM_DEVICE)
    {
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_FROM_DEV_LOCAL_ADDR_PCIE, &LocalAddressStart);
    }
    else
    {
        DDC_DMA_WRITE_PCIE(pDeviceContext, REG_DMA_TO_DEV_LOCAL_ADDR_PCIE, &LocalAddressStart);
    }

    pDMATransaction->totalBytesTransfered += TransferSize;

    pDeviceContext->sDMA.u32TransferSize = TransferSize;
    pDeviceContext->sDMA.u32Direction = pDMATransaction->Direction;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DMA, DDC_DBG_DMA_CONTINUE_PCIE, "DMA continue: byte total %d,  byte %d\n", pDMATransaction->totalBytesTransfered, pDeviceContext->sDMA.u32TransferSize);

    dmaStartPCIe(pDeviceContext);
}
