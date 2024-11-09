/*******************************************************************************
 * FILE: ddc_udl_os_bus_pci.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific functions for the
 *  PCI bus implementation.
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
#include "include/ddc_error_list.h"
#include "include/ddc_device_ids.h"
#include "bus/ddc_udl_bus_private.h"
#include "bus/pci/ddc_udl_bus_pci_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_driver_private.h"

/* Kernel assigns our major number */
dev_t acexpci_major = 0;

/* Kernel device class structure (for organizing devices) */
struct class *acexpci_class = NULL;

/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */

static int _ddcUdlOsBusPciParse
(
    void
);

static int _ddcUdlOsBusPciParseCard
(
    U16BIT u16CardType
);

static void _ddcUdlOsBusPciSetDevInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct pci_dev *pPciDev
);


/*******************************************************************************
 * Name:    ddcUdlOsBusInit
 *
 * Description:
 *      This function performs OS bus specific initialization.
 *
 * In   none
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusInit
(
    void
)
{
    int result;

    /* register range of character device numbers, requesting the next available major number and "DDC_NUM_MINOR" minor numbers, starting with zero (0) */
    result = alloc_chrdev_region(&acexpci_major, 0, DDC_NUM_MINOR, MODULE_NAME);
    if (result < 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_INIT, "driver registration failure - alloc_chrdev_region() returned %d\n", result);
        return (S16BIT) DDC_UDL_ERROR__DRIVER_INITIALIZATION;
    }

    /* create device class structure */
    acexpci_class = class_create(THIS_MODULE, MODULE_NAME);
    if (IS_ERR(acexpci_class))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_INIT, "device class creation failure\n");

        /* unregister range of character device numbers */
        unregister_chrdev_region(acexpci_major, DDC_NUM_MINOR);

        return (S16BIT) DDC_UDL_ERROR__DRIVER_INITIALIZATION;
    }

    /* search for cards and generate the driver level card list */
    result = _ddcUdlOsBusPciParse();

    if (result != DDC_UDL_ERROR__SUCCESS)
    {
        /* destroy device class structure */
        class_destroy(acexpci_class);

        /* unregister range of character device numbers */
        unregister_chrdev_region(acexpci_major, DDC_NUM_MINOR);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_INIT, "_ddcUdlOsBusPciParse() returned %d\n", result);
    }

    return (S16BIT) result;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusGetInterruptStatus
 *
 * Description:
 *  This function gets retrieves an interrupt status register value.
 *
 * In   pDeviceContext  Device Context Pointer
 * In   u32Offset       Interrupt Register Offset
 * In   u32Offset       Interrupt Register Offset
 * Out  none
 *
 * Returns: Interrupt Status Value
 ******************************************************************************/
U32BIT ddcUdlOsBusGetInterruptStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u8RegisterType,
    U32BIT u32BaseAddress,
    U32BIT u32Offset
)
{
    U32BIT u32Value = 0x00000000;

    DDC_UNREFERENCED_PARAMETER(u8RegisterType);

    DDC_REG_READ(pDeviceContext, (u32BaseAddress + u32Offset), &(u32Value));

    return u32Value;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusPciMapAddress
 *
 * Description:
 *  This function takes in an address type and a BAR address type and maps
 *  the PCI BAR address to a kernel address for the particular type. If the
 *  mapping fails, an error is returned.
 *
 * In   pDeviceContext  Device Context Pointer
 * In   u8AddressType
 * In   u8BarType
 * Out  none
 *
 * Returns: Interrupt Status Value
 ******************************************************************************/
S16BIT ddcUdlOsBusPciMapAddress
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8AddressType,
    U8BIT u8BarType
)
{
    struct pci_dev *pPciDev;
    unsigned long  ulPciAddr;
    void **ppAddr;
    U32BIT *pu32Length;

    /* get pointers to the requested address type */
    switch (u8AddressType)
    {
        case DDC_UDL_REGISTER_ADDRESS:
        {
            ppAddr = &pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr;
            pu32Length = &pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen;
            break;
        }

        case DDC_UDL_MEMORY_ADDRESS:
        {
            ppAddr = &pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr;
            pu32Length = &pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen;
            break;
        }

        case DDC_UDL_PLX_ADDRESS:
        {
            ppAddr = &pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr;
            pu32Length = &pDeviceContext->ddcOsDevInfo.sBusInfo.u32PlxLen;
            break;
        }

        case DDC_UDL_DMA_ADDRESS:
        {
            ppAddr = &pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr;
            pu32Length = &pDeviceContext->ddcOsDevInfo.sBusInfo.u32DmaLen;
            break;
        }

        default:
        {
            /* ERROR: invalid address type */
            return DDC_UDL_ERROR__DEVICE_REG_MAP_FAIL;
            break;
        }
    }

    pPciDev = pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev;

    /* get the PCI address from the requested BAR */
    ulPciAddr = pci_resource_start(pPciDev, u8BarType);

    /* get the BAR length */
    *pu32Length = pci_resource_len(pPciDev, u8BarType);

    /* map the PCI address to a kernel address */
    *ppAddr = ioremap(ulPciAddr, *pu32Length);

    if (*ppAddr == NULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MAP_ADDRESS,
            "ERROR: Kernel address mapping failed - AddrType:%d, BarType:%d PciAddr: 0x%08X Len:%d\n",
            u8AddressType, u8BarType, (U32BIT)(ulPciAddr & 0xFFFFFFFF), *pu32Length);

        return DDC_UDL_ERROR__DEVICE_REG_MAP_FAIL;
    }

    /* we can set the length, now that we have a valid kernel address */
    *pu32Length = pci_resource_len(pPciDev, u8BarType);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _ddcUdlOsBusPciParse
 *
 * Description:
 *      This routine searches for all cards in the product family and updates
 *      the driver level card list with the information for each card found.
 *
 * In   nonoe
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static int _ddcUdlOsBusPciParse
(
    void
)
{
    const DEVICE_LIST_TYPE *pSupportedCardList;
    U16BIT result;
    U8BIT u8CardListCount;
    U8BIT u8CardCount;
    U8BIT i;

    pSupportedCardList = ddcUdlGetSupportedDeviceList();
    u8CardListCount = ddcUdlGetSupportedDeviceListCount();

    /* look for PCI/PCIe cards */
    for (i = 0; i < u8CardListCount; i++)
    {
        result = _ddcUdlOsBusPciParseCard(pSupportedCardList[i].deviceId);

        if (result != DDC_UDL_ERROR__SUCCESS)
        {
            return result;
        }
    }

    u8CardCount = ddcUdlGetDeviceCount();

    if (u8CardCount == 0)
    {
        return DDC_UDL_ERROR__NO_CARDS_FOUND;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _ddcUdlOsBusPciParseCard
 *
 * Description:
 *      Here we probe for PCI bus cards matching the passed in device ID, and
 *      initializing the card structure for each.  A total card count is also
 *      updated for each card found.  If too many cards of a particular type
 *      are encountered, an error is returned.
 *
 * In   u16CardType
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static int _ddcUdlOsBusPciParseCard
(
    U16BIT u16CardType
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext;
    struct pci_dev *pPciDev = NULL;
    S16BIT s16Result;
    U8BIT u8CardCount = 0;

    /* loop while finding supported cards */
    while ((pPciDev = pci_get_device(DDC_VENDOR_ID, u16CardType, pPciDev)))
    {
        u8CardCount = ddcUdlGetDeviceCount();

        if (u8CardCount >= MAX_NUM_DEVICES)
        {
            return DDC_UDL_ERROR__TOO_MANY_CARDS;
        }

        /* enable the device */
        if (pci_enable_device(pPciDev))
        {
            return DDC_UDL_ERROR__ENABLE;
        }

        pDeviceContext = ddcUdlCreateDevice();

        if (pDeviceContext == NULL)
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        _ddcUdlOsBusPciSetDevInfo(pDeviceContext, pPciDev);


        s16Result = ddcUdlBusPciInitDevice(pDeviceContext);

        if (s16Result != DDC_UDL_ERROR__SUCCESS)
        {
            return s16Result;
        }


        /* flag card interrupts as disabled and initialize IRQ level */
        pDeviceContext->ddcOsDevInfo.sBusInfo.u16Irq = pPciDev->irq;

        /* make the device a PCI bus master */
        pci_set_master(pPciDev);

        /* initialize device context */
        s16Result = ddcUdlDeviceContexInit(pDeviceContext);

        if (s16Result != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_INIT, "ddcUdlDeviceContexInit() returned %d\n", s16Result);
            return s16Result;
        }
        
        /* Update the total card count */
        ddcUdlIncrementDeviceCount();
	}

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _ddcUdlOsBusPciSetDevInfo
 *
 * Description:
 *      This function sets the PCI device information in the ddcOsDevInfo
 *      structure.
 *
 * In   pDeviceContext
 * In   pPciDev
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
static void _ddcUdlOsBusPciSetDevInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct pci_dev *pPciDev
)
{
    pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev = pPciDev;
    pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum = pPciDev->bus->number;
    pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum = PCI_SLOT(pPciDev->devfn);
    pDeviceContext->u16DeviceID = pPciDev->device;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusExit
 *
 * Description:
 *      This function is called by the kernel when the driver module is unloaded.
 *      The device will be de-registered.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlOsBusExit
(
    void
)
{
    /* destroy device class structure */
    class_destroy(acexpci_class);

    /* unregister range of character device numbers */
    unregister_chrdev_region(acexpci_major, DDC_NUM_MINOR);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_EXIT, "EXIT\n");
}


/* ========================================================================== */
/* ========================================================================== */
static DDC_INLINE U32BIT _ddcUdlOsBusReadU32bit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pAddr
)
{
    if (pDeviceContext->eEndiannessMode == DDC_ENDIANNESS_DO_HW_SWAP)
    {
        /* x86 */
        return (U32BIT)__raw_readl(pAddr);
    }
    else
    {
        /* PPC */
        return (U32BIT)readl(pAddr);
    }
}

/* ========================================================================== */
/* ========================================================================== */
static DDC_INLINE void _ddcUdlOsBusWriteU32bit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Data,
    void *pAddr
)
{
    if (pDeviceContext->eEndiannessMode == DDC_ENDIANNESS_DO_HW_SWAP)
    {
        /* x86 */
        __raw_writel(u32Data, pAddr);
    }
    else
    {
        /* PPC */
        writel(u32Data, pAddr);
    }
}

/* ========================================================================== */
/* ========================================================================== */
static DDC_INLINE U16BIT _ddcUdlOsBusReadU16bit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *addr
)
{
    if (pDeviceContext->eEndiannessMode == DDC_ENDIANNESS_DO_HW_SWAP)
    {
        /* x86 */
        return (U16BIT)__raw_readw(addr);
    }
    else
    {
        /* PPC */
        return (U16BIT)readw(addr);
    }
}

/* ========================================================================== */
/* ========================================================================== */
#if 0 /* not used at this time */
static DDC_INLINE void _ddcUdlOsBusWriteU16bit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Data,
    void *addr
)
{
    if (pDeviceContext->eEndiannessMode == DDC_ENDIANNESS_DO_HW_SWAP)
    {
        /* x86 */
        __raw_writew(u16Data, addr);
    }
    else
    {
        /* PPC */
        writew(u16Data, addr);
    }
}
#endif /* not used at this time */

/*******************************************************************************
 * Name:    ddcUdlOsBusRegRead
 *
 * Description:
 *      This function reads a 32 bit register value
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         register address location
 * Out  pu32RdData       location to store register value
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusRegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData
)
{
    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr) ||
        (u32Addr >= pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    *pu32RdData = _ddcUdlOsBusReadU32bit(pDeviceContext,
        (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr + u32Addr));

    IO_BARRIER;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_REG_READ, "u32Addr:0x%08X  data:0x%08X\n", u32Addr, *pu32RdData);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusRegReadBlk
 *
 * Description:
 *      This function reads a block of 32 bit registers
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         register address location
 * Out  pu32RdData      location to store register value
 * In   u32Count        number of registers to read
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusRegReadBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Count
)
{
    int i = 0;
    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr) ||
        ((u32Addr + (u32Count << 2)) >= pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    for (i = 0; i < u32Count; i++, pu32RdData++)
    {
        *pu32RdData = _ddcUdlOsBusReadU32bit(pDeviceContext,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr + u32Addr + (i * 4)));
    }

    IO_BARRIER;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_REG_READ_BLK,
        "u32Addr:0x%08X  pu32RdData[0]:0x%08X  u32Count: %d\n", u32Addr, pu32RdData[0], u32Count);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusRegWrite
 *
 * Description:
 *      This function writes a 32 bit register value
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         address of register to write
 * In   pu32WrData       pointer to register value
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusRegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData
)
{
    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr) ||
        (u32Addr >= pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    _ddcUdlOsBusWriteU32bit(pDeviceContext,
        *pu32WrData,
        (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr + u32Addr));

    IO_BARRIER;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_REG_WRITE,
        "u32Addr:0x%08X  Data:0x%08X\n", u32Addr, *pu32WrData);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusRegWriteBlk
 *
 * Description:
 *      This function writes a block of 32 bit register values
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         address of register to write
 * In   pu32WrData      pointer to register value
 * In   u32Count        number of registers to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusRegWriteBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Count
)
{
    int i = 0;
    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr) ||
        ((u32Addr + (u32Count << 2)) >= pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_REG_WRITE_BLK,
        "u32Addr:0x%08X  pu32WrData[0]:0x%08X  u32Count: %d\n", u32Addr, pu32WrData[0], u32Count);

    for (i = 0; i < u32Count; i++, pu32WrData++)
    {
        _ddcUdlOsBusWriteU32bit(pDeviceContext, *pu32WrData,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr + u32Addr + (i * 4)));
    }

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemRead
 *
 * Description:
 *      This function reads a 32 bit memory location
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to read
 * Out  pu32RdData       location to store memory value
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Mode
)
{
    U32BIT u32Count = 1;

    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32Count << 2)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    if (u32Mode == ACEX_32_BIT_ACCESS_16_BIT_HW_MEM)
    {
        *pu32RdData = DDC_WORD_ORDER_L(_ddcUdlOsBusReadU32bit(pDeviceContext,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr)));
    }
    else
    {
        *pu32RdData = _ddcUdlOsBusReadU32bit(pDeviceContext,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr));
    }

    IO_BARRIER;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_READ,
        "u32Addr:0x%08X  data:0x%08X\n", u32Addr, *pu32RdData);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemReadBlk
 *
 * Description:
 *      This function reads a 32 bit memory location
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to read
 * Out  pu32RdData      location to store memory values
 * In   u32Count        number of locations to read
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemReadBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32RdData,
    U32BIT u32Count,
    U32BIT u32Mode
)
{
    int i = 0;

    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32Count << 2)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_READ_BLK,
            "DDC_UDL_ERROR__CANT_FIND_DEVICE u32Addr : 0x%x u32Count : %d  MemLen : 0x%x\n",
            u32Addr, u32Count, pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_READ_BLK, "u32Count %d\n", u32Count);

    for (i = 0; i < u32Count; i++, pu32RdData++)
    {
        if (u32Mode == ACEX_32_BIT_ACCESS_16_BIT_HW_MEM)
        {
            *pu32RdData = DDC_WORD_ORDER_L(_ddcUdlOsBusReadU32bit(pDeviceContext,
                (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr + (i * 4))));
        }

        else
        {
            *pu32RdData = _ddcUdlOsBusReadU32bit(pDeviceContext,
                (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr + (i * 4)));
        }
    }

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemReadBlk16
 *
 * Description:
 *      This function reads a block of 16 bit memory values
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to read
 * Out  pu16RdData      location to store memory values
 * In   u32Count        number of locations to read
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemReadBlk16
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U16BIT* pu16RdData,
    U32BIT u32Count
)
{
    int i = 0;

    u32Addr <<= 1;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32Count << 1)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_READ_BLK_16,
            "DDC_UDL_ERROR__CANT_FIND_DEVICE u32Addr : 0x%x u32Count : %d  MemLen : 0x%x\n",
            u32Addr, u32Count, pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_READ_BLK_16, "u32Count %d\n", u32Count);

    for (i = 0; i < u32Count; i++, pu16RdData++)
    {
        *pu16RdData = _ddcUdlOsBusReadU16bit(pDeviceContext,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr + (i * 2)));
    }

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemWrite
 *
 * Description:
 *      This function writes a 32 bit memory value
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to write
 * In   pu32WrData      pointer to value to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Mode
)
{
    U32BIT u32Count = 1;

    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32Count << 1)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_WRITE,
        "u32Addr:0x%08X  data:0x%08X\n", u32Addr, *pu32WrData);

    if (u32Mode == ACEX_32_BIT_ACCESS_16_BIT_HW_MEM)
    {
        _ddcUdlOsBusWriteU32bit(pDeviceContext, DDC_WORD_ORDER_L(*pu32WrData),
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr));
    }
    else
    {
        _ddcUdlOsBusWriteU32bit(pDeviceContext, *pu32WrData,
            (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr));
    }

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemWriteBlk
 *
 * Description:
 *      This function writes a block of 32 bit memory values
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to write
 * In   pu32WrData      pointer to values to write
 * In   u32Count        number of values to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemWriteBlk
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT *pu32WrData,
    U32BIT u32Count,
    U32BIT u32Mode
)
{
    int i = 0;
    u32Addr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32Count << 2)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
            "DDC_UDL_ERROR__CANT_FIND_DEVICE u32Addr : 0x%x u32Count : %d  MemLen : 0x%x\n",
            u32Addr, u32Count, pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_WRITE_BLK,
        "u32Addr:0x%08X  pu32WrData[0]:0x%08X  u32Count: %d\n", u32Addr, pu32WrData[0], u32Count);

    for (i = 0; i < u32Count; i++, pu32WrData++)
    {
        if (u32Mode == ACEX_32_BIT_ACCESS_16_BIT_HW_MEM)
        {
            _ddcUdlOsBusWriteU32bit(pDeviceContext,
                DDC_WORD_ORDER_L(*pu32WrData), (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr + (i * 4)));
        }
        else
        {
            _ddcUdlOsBusWriteU32bit(pDeviceContext, *pu32WrData,
                (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + u32Addr + (i * 4)));
        }
    }

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusMemWriteBlk16
 *
 * Description:
 *
 *  This function writes a block of 16-bit memory values.
 *
 *  It takes care of the case where the 16-bit data falls on an odd boundry.
 *
 * Example: 16-bit address starting at 3, and transferring 6 words.
 *
 *      +----+----+
 *    1 |    |    | 0
 *    3 |XXXX|    | 2
 *    5 |XXXX|XXXX| 4
 *    7 |XXXX|XXXX| 6
 *    9 |    |XXXX| 8
 *      +----+----+
 *
 *  1) read in 32-bit value at address 2
 *  2) Clear out then 'OR' in 16-bit value
 *  3) write data back at address 2
 *
 *  4) write the next 2 32-bit words, for a total of 4 16-bit words
 *
 *  5) read in 32-bit value at address 8
 *  6) Clear out then 'OR' in 16-bit value
 *  7) write data back at address 8
 *
 * In   pDeviceContext  device-specific structure
 * In   u32Addr         location in memory to write
 * In   pu16WdData      pointer to values to write
 * In   u32WordCount    number of values to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusMemWriteBlk16
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U16BIT *pu16WrData,
    U32BIT u32WordCount
)
{
    U32BIT *pu32BlkMemAddr;
    U16BIT *pu16Data;
    U32BIT u32TempData;
    U32BIT u32Start16Addr;
    U32BIT u32End16Addr;
    U32BIT u32Length;
    U32BIT i = 0;

    u32Addr <<= 1;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr) ||
        ((u32Addr + (u32WordCount << 1)) > pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
            "DDC_UDL_ERROR__CANT_FIND_DEVICE u32Addr : 0x%x u32WordCount : %d  MemLen : 0x%x\n",
            u32Addr, u32WordCount, pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    /* set the 16-bit pointer to the data buffer */
    pu16Data = pu16WrData;

    /* flag indicator for initial 16-bit transfer */
    u32Start16Addr = (u32Addr % 4);

    /* flag indicator for trailing 16-bit transfer */
    /* take 32-bit (byte) addr and convert word count to byte count */
    u32End16Addr = ((u32Addr + (u32WordCount * 2)) % 4);

    /* calculate number of 32-bit transfers required */
    u32Length = (u32WordCount / 2);

    /* if we have both a start and end addr (2 16-bit writes), we need to do one less 32-bit xfer */
    if (u32Start16Addr && u32End16Addr)
    {
        u32Length--;
    }

    /* compute the first 32-bit data starting address */
    pu32BlkMemAddr = (U32BIT*)(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr + (u32Addr & 0xFFFFFFFC));

    /* -------------------------------------------- */
    /* first perform a 16-bit transfer, if required */
    /* -------------------------------------------- */
    if (u32Start16Addr)
    {
        /* read the current 32-bit value and mask out our data location */
        u32TempData = DDC_WORD_ORDER_L(_ddcUdlOsBusReadU32bit(pDeviceContext,
            (U8BIT *)pu32BlkMemAddr));

        u32TempData &= 0x0000FFFF;

        /* add our data to the value */
        u32TempData |= (*pu16Data << 16);

        /* write the data back */
        _ddcUdlOsBusWriteU32bit(pDeviceContext,
            DDC_WORD_ORDER_L(u32TempData), (U8BIT *)pu32BlkMemAddr);

        /* update the data and address pointers */
        pu16Data++;
        pu32BlkMemAddr++;
    }

    /* -------------------------------------------- */
    /* write the 32-bit packed data words           */
    /* -------------------------------------------- */
    while (i < u32Length)
    {
        /* assign the 16-bit data words to the 32-bit temporary data */
        u32TempData = ((U32BIT)*pu16Data) & 0x0000FFFF;
        pu16Data++;
        u32TempData |= ((U32BIT)*pu16Data << 16);

        /* Copy data 32-bits at a time and increment pointer */
        _ddcUdlOsBusWriteU32bit(pDeviceContext,
            DDC_WORD_ORDER_L(u32TempData), (U8BIT *)(pu32BlkMemAddr));

        /* update the data and address pointers */
        pu16Data++;
        pu32BlkMemAddr++;

        i++;
    }

    /* --------------------------------------------- */
    /* perform trailing 16-bit transfer, if required */
    /* --------------------------------------------- */
    if (u32End16Addr)
    {
        /* read the last word and mask out our data location */
        u32TempData = DDC_WORD_ORDER_L(_ddcUdlOsBusReadU32bit(pDeviceContext,
            (U8BIT *)(pu32BlkMemAddr)));

        u32TempData &= 0xFFFF0000;

        /* add our data to the value */
        u32TempData |= *pu16Data & 0x0000FFFF;

        /* write the data back */
        _ddcUdlOsBusWriteU32bit(pDeviceContext, DDC_WORD_ORDER_L(u32TempData),
            (U8BIT *)(pu32BlkMemAddr));
    }

    IO_BARRIER;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_MEM_WRITE_BLK_16,
        "u32Addr:0x%08X  pu16WrData[0]:0x%08X  u32WordCount: %d\n", u32Addr, pu16WrData[0], u32WordCount);

    return DDC_UDL_ERROR__SUCCESS;
}


/*============================================================================*/
/*                         PLX BUS ACCESS FUNCTIONS                           */
/*============================================================================*/

/*******************************************************************************
 * Name:    ddcUdlOsBusReadPlx
 *
 * Description:
 *      This function reads a 32 bit PLX register value
 *
 * In   pDeviceContext  device-specific structure
 * In   u32ByteAddr     PLX register address
 * Out  none
 *
 * Returns: PLX register value
 ******************************************************************************/
void ddcUdlOsBusReadPlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32ByteAddr,
    U32BIT *pu32Data
)
{
    *pu32Data = readl((U32BIT *)((U8BIT *)pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr + u32ByteAddr));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_PLX_READ, "u32ByteAddr:0x%X  data:0x%08X\n", u32ByteAddr, *pu32Data);
}

/*******************************************************************************
 * Name:    ddcUdlOsBusWritePlx
 *
 * Description:
 *      This function writes a 32 bit PLX register
 *
 * In   pDeviceContext  device-specific structure
 * In   u32ByteAddr     PLX register address
 * In   u32Data         value to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
void ddcUdlOsBusWritePlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32ByteAddr,
    U32BIT u32Data
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_BUS, DDC_DBG_BUS_PLX_WRITE,
        "u32Addr:0x%X  data:0x%08X\n", u32ByteAddr, u32Data);

    writel(u32Data, (U32BIT *)((U8BIT *)pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr + u32ByteAddr));
}

/*============================================================================*/
/*                        TACEX DMA BUS ACCESS FUNCTIONS                      */
/*============================================================================*/

/*******************************************************************************
 * Name:    ddcUdlOsBusAsicDmaRead
 *
 * Description:
 *      This function reads a 32 bit Dma register value
 *
 * In   pDeviceContext  device-specific structure
 * In   RegAddr         Dma register address
 * Out  none
 *
 * Returns: PLX register value
 ******************************************************************************/
U32BIT ddcUdlOsBusAsicDmaRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr
)
{
    U32BIT u32Data = 0;

    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + u32Addr, &u32Data);
    return u32Data;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusAsicDmaWrite
 *
 * Description:
 *      This function writes a 32 bit Dma register
 *
 * In   pDeviceContext  device-specific structure
 * In   RegAddr         Dma register address
 * In   Value           value to write
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
void ddcUdlOsBusAsicDmaWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Addr,
    U32BIT Value
)
{
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDMA.pu32TacexRegBA) + u32Addr, &Value);
}

/*============================================================================*/
/*                DMA ACCESS FUNCTIONS - PCIE REV B CARDS                     */
/*============================================================================*/

/*----------------------------------------------------------------------------
    Function: ddcUdlOsBusPcieDmaWrite

    Description:
           This writes a 32 bit value to the dma portion of the card.

    Arguments:
            pdc            - pointer to device context
                u32DmaRegAddr  - dword address
                pu32RdData     - pointer to dword buffer to write

    Returns:
            NTSTATUS - Windows status codes
   -----------------------------------------------------------------------------*/
S16BIT ddcUdlOsBusPcieDmaWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32DmaRegAddr,
    U32BIT *pWrData
)
{
    u32DmaRegAddr <<= 2;

    if ((!pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr) ||
        (u32DmaRegAddr >= pDeviceContext->ddcOsDevInfo.sBusInfo.u32DmaLen))
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

#if DDC_PPC

    /* PPC */
    writel(*pWrData, (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr + u32DmaRegAddr));
#else

    /* x86 */
    __raw_writel(*pWrData, (U8BIT *)(pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr + u32DmaRegAddr));
#endif /* DDC_PPC */

    IO_BARRIER;

    return DDC_UDL_ERROR__SUCCESS;
}


/*******************************************************************************
 * Name:    ddcUldOsPciDmaMap
 *
 * Description:
 *      This function maps the buffer for streaming DMA.
 *
 * In   pDeviceContext  device-specific structure
 * In   pBufferAddress
 * In   u32ByteCount    Buffer byte count
 * In   eDirection      DMA direction
 * Out  none
 *
 * Returns: mapped address
 ******************************************************************************/
DDC_DMA_ADDR ddcUldOsPciDmaMap
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    void *pBufferAddress,
    U32BIT u32ByteCount,
    enum dma_data_direction eDirection
)
{
    DDC_DMA_ADDR dmaAddr;

    dmaAddr = dma_map_single(
        &pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev,
        pBufferAddress,
        u32ByteCount,
        eDirection);

    return dmaAddr;
}

/*******************************************************************************
 * Name:    ddcUldOsPciDmaUnmap
 *
 * Description:
 *      This function unmaps the buffer that was used for streaming DMA.
 *
 * In   pDeviceContext  device-specific structure
 * In   dmaAddr         previously mapped DMA address (returned from ddcUldOsPciDmaMap)
 * In   u32ByteCount    Buffer byte count
 * In   eDirection      DMA direction
 * Out  none
 *
 * Returns: mapped address
 ******************************************************************************/
void ddcUldOsPciDmaUnmap
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_DMA_ADDR dmaAddr,
    U32BIT u32ByteCount,
    enum dma_data_direction eDirection
)
{
    dma_unmap_single(
        &pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev,
        dmaAddr,
        u32ByteCount,
        eDirection);
}
