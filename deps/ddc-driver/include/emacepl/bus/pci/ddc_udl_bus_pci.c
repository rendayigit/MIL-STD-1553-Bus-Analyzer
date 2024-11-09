/*******************************************************************************
 * FILE: ddc_udl_bus_pci.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS independent PCI access
 *  routines.
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
#include "include/ddc_error_list.h"
#include "include/ddc_device_ids.h"
#include "bus/ddc_udl_bus_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "bus/pci/ddc_udl_bus_pci_private.h"
#include "os/bus/pci/ddc_udl_os_bus_private.h"

/* Although it is recommended to set FALSE by default, some systems may  
   need to set to TRUE. When this is needed, driver must be re-compiled 
   and re-loaded. */
#define DDC_PLX_RETRY_WRITE_ON_PENDING_READ     FALSE


#define DDC_BAR0    0
#define DDC_BAR1    1
#define DDC_BAR2    2
#define DDC_BAR3    3

/*IMPORTANT - when adding a device make sure you update NumberofSupportedDevices in ddc_udl_bus_private.h */
const DEVICE_LIST_TYPE aSupportedDeviceList[] =
{
    { DDC_DEV_ID_QPRM,          DDC_VENDOR_ID },

    { DDC_DEV_ID_BU67101QX,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67105CX,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67106KX,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67106BKX,    DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67107FM,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67108C,      DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67110FM,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67112Xx,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67118FMX,    DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67118M700,   DDC_VENDOR_ID },	
    { DDC_DEV_ID_BU67118YZX,    DDC_VENDOR_ID },

    { DDC_DEV_ID_BU67206KX,     DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67206BKX,    DDC_VENDOR_ID },
    { DDC_DEV_ID_BU67210FMX,    DDC_VENDOR_ID },

    { DDC_DEV_ID_DD40000K,      DDC_VENDOR_ID },
    { DDC_DEV_ID_DD40100F,      DDC_VENDOR_ID },
    { DDC_DEV_ID_DD40001H,      DDC_VENDOR_ID },
    { DDC_DEV_ID_DD40002M,      DDC_VENDOR_ID }
};
/*IMPORTANT - when adding a device make sure you update NumberofSupportedDevices in ddc_udl_bus_private.h */

/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */
static void _ddcUdlBusPciSetDriverType
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

static S16BIT _ddcUdlBusPciMapAddresses
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

static void _ddcUdlBusPciInitializePlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);


/*******************************************************************************
 * Name:    ddcUdlBusInit
 *
 * Description:
 *      This function performs bus specific initialization.
 *
 * In   none
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBusInit
(
    void
)
{
    return ddcUdlOsBusInit();
}

/*******************************************************************************
 * Name:    ddcUdlBusGetSupportedDeviceList
 *
 * Description:
 *      This function returns a pointer to the supported device list.
 *
 * In   none
 * Out  none
 *
 * Returns: const pointer to supported device list
 ******************************************************************************/
const DEVICE_LIST_TYPE * ddcUdlBusGetSupportedDeviceList
(
    void
)
{
    return aSupportedDeviceList;
}

/*******************************************************************************
 * Name:    ddcUdlBusGetSupportedDeviceListCount
 *
 * Description:
 *      This function returns the number of items in the device list.
 *
 * In   none
 * Out  none
 *
 * Returns: device list count
 ******************************************************************************/
U8BIT ddcUdlBusGetSupportedDeviceListCount
(
    void
)
{
    return (sizeof(aSupportedDeviceList) / sizeof(aSupportedDeviceList[0]));
}

/*******************************************************************************
 * Name:    ddcUdlBusGetLocationInfo
 *
 * Description:
 *      This function returns the bus location of the device.
 *
 * In   pDeviceContext
 * In   pu32Location        PCI Bus or USB Hub Number
 * In   pu32SubLocation     PCI Device or USB Port Number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
void ddcUdlBusGetLocationInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT *pu16Location,
    U16BIT *pu16SubLocation
)
{
    if ((pu16Location == NULL) ||
        (pu16SubLocation == NULL))
    {
        /* ERROR: NULL pointer being passed in */
        return;
    }

    *pu16Location = pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciBusNum;
    *pu16SubLocation = pDeviceContext->ddcOsDevInfo.sBusInfo.u16PciDevNum;
}

/*******************************************************************************
 * Name:    ddcUdlBusPciInitDevice
 *
 * Description:
 *      This function initializes a PCI device.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBusPciInitDevice
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;

    pDeviceContext->pDeviceContextCPLD = NULL;
    pDeviceContext->bBoardLoadCompleted = FALSE;

    _ddcUdlBusPciSetDriverType(pDeviceContext);

    s16Result = _ddcUdlBusPciMapAddresses(pDeviceContext);

    if (s16Result != DDC_UDL_ERROR__SUCCESS)
    {
        return s16Result;
    }

    _ddcUdlBusPciInitializePlx(pDeviceContext);

    return s16Result;
}

/*******************************************************************************
 * Name:    _ddcUdlBusPciSetDriverType
 *
 * Description:
 *      Sets the driver type depending upon the device ID.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void _ddcUdlBusPciSetDriverType
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    /* set driver type */
    switch (pDeviceContext->u16DeviceID)
    {
        case DDC_DEV_ID_BU67112Xx:
        {
            pDeviceContext->u16DriverType = ACEX_IO_DRIVER;
            break;
        }

        case DDC_DEV_ID_QPRM:
        {
            pDeviceContext->u16DriverType = ACEX_QPRM_DRIVER;
            break;
        }

        case DDC_DEV_ID_BU67106BKX:
        case DDC_DEV_ID_BU67206BKX:
        {
            pDeviceContext->u16DriverType = ACEX_PCIE_DRIVER;
            break;
        }

        case DDC_DEV_ID_BU67118YZX:
        case DDC_DEV_ID_DD40000K:
        case DDC_DEV_ID_DD40001H:
        {
            pDeviceContext->u16DriverType = ACEX_DD429_PCIE_DRIVER;
            break;
        }

        case DDC_DEV_ID_BU67118FMX:
        case DDC_DEV_ID_BU67118M700:
        case DDC_DEV_ID_DD40100F:
        case DDC_DEV_ID_DD40002M:
        {
            pDeviceContext->u16DriverType = ACEX_DD429_PCI_DRIVER;
            break;
        }

        default:
        {
            pDeviceContext->u16DriverType = ACEX_PCI_DRIVER;
            break;
        }
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "Driver Type: %d\n", pDeviceContext->u16DriverType);
}

/*******************************************************************************
 * Name:    _ddcUdlBusPciMapAddresses
 *
 * Description:
 *      This function maps the necessary addresses (register, memory, PLX,
 *      and DMA) from the associated BAR addresses to a virtual kernel address.
 *      If any mapping fails, all addresses will be unmapped before returning
 *      an error.
 *
 * In   pDeviceContext
 * Out  affected address and lengths contained in the
 *      pDeviceContext->ddcOsDevInfo.sBusInfo struct
 *
 * Returns: error condition if any mapping fails
 ******************************************************************************/
S16BIT _ddcUdlBusPciMapAddresses
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;

    /* map the addresses according to driver type */
    switch (pDeviceContext->u16DriverType)
    {
        /* ---------------------------- */
        case ACEX_IO_DRIVER:
        /* ---------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "IO device\n");

            /* BAR0 - REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_REGISTER_ADDRESS, DDC_BAR0);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR0 REG addr %p, size 0x%08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen);

            break;
        }

        /* ---------------------------- */
        case ACEX_QPRM_DRIVER:
        /* ---------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "QPRM\n");

            /* BAR0 - MEMORY */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_MEMORY_ADDRESS, DDC_BAR0);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR0 MEM addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

            /* BAR1 - REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_REGISTER_ADDRESS, DDC_BAR1);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR1 REG addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen);

            break;
        }

        /* ---------------------------- */
        case ACEX_PCIE_DRIVER:
        case ACEX_DD429_PCIE_DRIVER:
        /* ---------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "PCIE\n");

            /* BAR0 - DMA REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_DMA_ADDRESS, DDC_BAR0);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR0 DMA addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32DmaLen);

            
            /* BAR1 - MEMORY */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_MEMORY_ADDRESS, DDC_BAR1);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR1 MEM addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);
                      
            /* BAR2 - REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_REGISTER_ADDRESS, DDC_BAR2);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR2 REG addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen);

            break;
        }

        /* ---------------------------- */
        case ACEX_DD429_PCI_DRIVER:
        default:
        /* ---------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "PCI devices\n");

            /* BAR0 - PLX REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_PLX_ADDRESS, DDC_BAR0);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR0 PLX addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32PlxLen);

            /* BAR2 - MEMORY */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_MEMORY_ADDRESS, DDC_BAR2);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     BAR2 MEM addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32MemLen);

            /* BAR3 - REGISTERS */
            s16Result = ddcUdlOsBusPciMapAddress(pDeviceContext, DDC_UDL_REGISTER_ADDRESS, DDC_BAR3);

            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                ddcUdlBusPciUnmapAddresses(pDeviceContext);
                return s16Result;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "     Bar3 REG addr %p, size %08x\n",
                pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr,
                pDeviceContext->ddcOsDevInfo.sBusInfo.u32RegLen);

            break;
        }
    }

    return s16Result;
}

/*******************************************************************************
 * Name:    _ddcUdlBusPciInitializePlx
 *
 * Description:
 *      Initialize the PLX chip.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void _ddcUdlBusPciInitializePlx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT intCSR = 0;
    U32BIT u32RegRd = 0;

    /* make sure there is a valid PLX address first */
    /* as some devices do not contain a PLX chip */
    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr)
    {
        /* read in the register status first */
        ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_INTSCR, &intCSR);

        ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_INTSCR, intCSR | PLX_INT_ENABLE);

        /* some BU67105C boards have wrong PLX configuration */
        if (pDeviceContext->sHwVersionInfo.dwModelNumber == BU67105C_PC104P_ACEX_MODEL_NUMBER)
        {
            ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_MARBR, &u32RegRd);
            u32RegRd &= (~PLX_MARBR_PCI_READ_WITH_WRITE_FLUSH_MODE); /* clear this bit to make sure that writing is forbidden before reading completes */
            ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_MARBR, u32RegRd);
        }

        ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead(pDeviceContext, DDC_PLX_RETRY_WRITE_ON_PENDING_READ);
    }
}

/*******************************************************************************
 * Name:    ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead
 *
 * Description:
 *      PCI Read No Write Mode (PCI Retries for Writes). Enabling PCI Compliance
 *      forces a PCI Retry on writes if a Delayed Read is pending. Disabled allows
 *      writes to occur while a Delayed Read is pending.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    BOOLEAN bEnable
)
{
    U32BIT u32RegRd = 0;

    /* make sure there is a valid PLX address first */
    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr)
    {
        /* read PLX Mode/DMA Arbitration register */
        ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_MARBR, &u32RegRd);
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead: read %08x\n", u32RegRd);

        /* clear the bit first */
        u32RegRd &= ~(PLX_MARBR_PCI_READ_NO_WRITE_MODE);

        /* if enabled, set the bit */
        if (bEnable)
        {
            u32RegRd |= PLX_MARBR_PCI_READ_NO_WRITE_MODE;
        }

        /* set PLX Mode/DMA Arbitration register */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY,
                "ddcUdlBusPciPlxConfigureRetryWriteOnPendingRead: set %08x\n", u32RegRd);
        ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_MARBR, u32RegRd);
    }
}
/*******************************************************************************
 * Name:    ddcUdlBusPciUnmapAddresses
 *
 * Description:
 *  This function will unmap the bus memory spaces that were mapped to
 *  kernel space.
 *
 * In   pDeviceContext      Device Context Pointer
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
void ddcUdlBusPciUnmapAddresses
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr)
    {
        DDC_IO_UNMAP(pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr);
        pDeviceContext->ddcOsDevInfo.sBusInfo.pRegAddr = NULL;
    }

    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr)
    {
        DDC_IO_UNMAP(pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr);
        pDeviceContext->ddcOsDevInfo.sBusInfo.pMemAddr = NULL;
    }

    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr)
    {
        DDC_IO_UNMAP(pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr);
        pDeviceContext->ddcOsDevInfo.sBusInfo.pDmaAddr = NULL;
    }

    if (pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr)
    {
        DDC_IO_UNMAP(pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr);
        pDeviceContext->ddcOsDevInfo.sBusInfo.pPlxAddr = NULL;
    }
}


/* ========================================================================== */
/* PLX EERPOM Support Functions  (adapted from the the PLX SDK)               */
/* ========================================================================== */

/* PLX 9000-series EEPROM definitions */
#define PLX9000_EE_CMD_LEN_46           9       /* Bits in instructions */
#define PLX9000_EE_CMD_LEN_56           11      /* Bits in instructions */
#define PLX9000_EE_CMD_LEN_66           11      /* Bits in instructions */
#define PLX9000_EE_CMD_READ             0x0180  /* 01 1000 0000 */
#define PLX9000_EE_CMD_WRITE            0x0140  /* 01 0100 0000 */
#define PLX9000_EE_CMD_WRITE_ENABLE     0x0130  /* 01 0011 0000 */
#define PLX9000_EE_CMD_WRITE_DISABLE    0x0100  /* 01 0000 0000 */

/* EEPROM status */
typedef enum _PLX_EEPROM_STATUS
{
    PLX_EEPROM_STATUS_NONE = 0,                     /* Not present */
    PLX_EEPROM_STATUS_VALID = 1,                    /* Present with valid data */
    PLX_EEPROM_STATUS_INVALID_DATA = 2,             /* Present w/invalid data or CRC error */
    PLX_EEPROM_STATUS_BLANK = PLX_EEPROM_STATUS_INVALID_DATA,
    PLX_EEPROM_STATUS_CRC_ERROR = PLX_EEPROM_STATUS_INVALID_DATA

} PLX_EEPROM_STATUS;


/* EEPROM Control register offset for 9056 */
#define REG_EEPROM_CTRL     0x6C

static void ddcUdlBusPciPlx9000_EepromSendCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32EepromCommand,
    U8BIT u8DataLengthInBits
);

static void ddcUdlBusPciPlx9000_EepromClock
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);


/******************************************************************************
 *
 * Function   :  ddcUdlBusPci_Plx9000_EepromPresent
 *
 * Description:  Returns the state of the EEPROM as reported by the PLX device
 *
 *****************************************************************************/
void ddcUdlBusPciPlx9000_EepromPresent
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *pu32Status
)
{
    U32BIT u32RegValue;

    /* Get EEPROM status register */
    DDC_PLX_READ(pDeviceContext, REG_EEPROM_CTRL, &u32RegValue);

    if (u32RegValue & (1 << 28))
    {
        *pu32Status = PLX_EEPROM_STATUS_VALID;
    }
    else
    {
        *pu32Status = PLX_EEPROM_STATUS_NONE;
    }
}

/******************************************************************************
 *
 * Function   :  ddcUdlBusPciPlx9000_EepromReadByOffset
 *
 * Description:  Read a 32-bit value from the EEPROM at a specified offset
 *
 *****************************************************************************/
void ddcUdlBusPciPlx9000_EepromReadByOffset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Offset,
    U32BIT *pu32Value
)
{
    S8BIT s8BitPos;
    S8BIT s8CommandShift;
    S8BIT s8CommandLength;
    U16BIT u16Count;
    U32BIT u32RegValue;
    U32BIT u32RegValueTemp;

    /* Setup parameters based on PLX 9056 */
    s8CommandShift  = 2;
    s8CommandLength = PLX9000_EE_CMD_LEN_56;

    /* Send EEPROM read command and offset to EEPROM */
    ddcUdlBusPciPlx9000_EepromSendCommand(
        pDeviceContext,
        (PLX9000_EE_CMD_READ << s8CommandShift) | (u16Offset / 2),
        s8CommandLength);

    /*****************************************************
     * Note: The EEPROM write ouput bit (26) is set here
     *       because it is required before EEPROM read
     *       operations on the 9054.  It does not affect
     *       behavior of non-9054 chips.
     *
     *       The EEDO Input enable bit (31) is required for
     *       some chips.  Since it is a reserved bit in older
     *       chips, there is no harm in setting it for all.
     ****************************************************/

    /* Set EEPROM write output bit */
    DDC_PLX_READ(
        pDeviceContext,
        REG_EEPROM_CTRL,
        &u32RegValue);

    /* Set EEDO Input enable for some PLX chips */
    u32RegValue |= (U32BIT)(1 << 31);

    u32RegValueTemp = u32RegValue | (1 << 26);

    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValueTemp);

    /* Get 32-bit value from EEPROM - one bit at a time */
    for (s8BitPos = 0; s8BitPos < 32; s8BitPos++)
    {
        /* Trigger the EEPROM clock */
        ddcUdlBusPciPlx9000_EepromClock(pDeviceContext);

        /*****************************************************
         * Note: After the EEPROM clock, a delay is sometimes
         *       needed to let the data bit propagate from the
         *       EEPROM to the PLX chip.  If a sleep mechanism
         *       is used, the result is an extremely slow EEPROM
         *       access since the delay resolution is large and
         *       is required for every data bit read.
         *
         *       Rather than using the standard sleep mechanism,
         *       the code, instead, reads the PLX register
         *       multiple times.  This is harmless and provides
         *       enough delay for the EEPROM data to propagate.
         ****************************************************/

        for (u16Count=0; u16Count < 20; u16Count++)
        {
            /* Get the result bit */
            DDC_PLX_READ(
                pDeviceContext,
                REG_EEPROM_CTRL,
                &u32RegValue);
        }

        /* Get bit value and shift into result */
        if (u32RegValue & (1 << 27))
        {
            *pu32Value = (*pu32Value << 1) | 1;
        }
        else
        {
            *pu32Value = (*pu32Value << 1);
        }
    }

    /* Clear EEDO Input enable for some PLX chips */
    u32RegValue &= ~(1 << 31);

    /* Clear Chip Select and all other EEPROM bits */
    u32RegValueTemp = u32RegValue & ~(0xF << 24);

    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValueTemp);
}

/******************************************************************************
 *
 * Function   :  ddcUdlBusPciPlx9000_EepromWriteByOffset
 *
 * Description:  Write a 32-bit value to the EEPROM at a specified offset
 *
 *****************************************************************************/
void ddcUdlBusPciPlx9000_EepromWriteByOffset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Offset,
    U32BIT u32Value
)
{
    S8BIT i;
    S8BIT s8BitPos;
    S8BIT s8CommandShift;
    S8BIT s8CommandLength;
    U16BIT u16EepromValue;
    S32BIT s32Timeout;
    U32BIT u32RegValue;
    U32BIT u32RegValueTemp;

    /* Setup parameters based on EEPROM type 9056 */
    s8CommandShift = 2;
    s8CommandLength = PLX9000_EE_CMD_LEN_56;

    /* Write EEPROM 16-bits at a time */
    for (i=0; i<2; i++)
    {
        /* Set 16-bit value to write */
        if (i == 0)
        {
            u16EepromValue = (U16BIT)(u32Value >> 16);
        }
        else
        {
            u16EepromValue = (U16BIT)u32Value;

            /* Update offset */
            u16Offset = (U16BIT)(u16Offset + sizeof(U16BIT));
        }

        /* Send Write_Enable command to EEPROM */
        ddcUdlBusPciPlx9000_EepromSendCommand(
            pDeviceContext,
            (PLX9000_EE_CMD_WRITE_ENABLE << s8CommandShift),
            s8CommandLength);

        /* Send EEPROM Write command and offset to EEPROM */
        ddcUdlBusPciPlx9000_EepromSendCommand(
            pDeviceContext,
            (PLX9000_EE_CMD_WRITE << s8CommandShift) | (u16Offset / 2),
            s8CommandLength);

        DDC_PLX_READ(
            pDeviceContext,
            REG_EEPROM_CTRL,
            &u32RegValue);

        /* Clear all EEPROM bits */
        u32RegValue &= ~(0xF << 24);

        /* Make sure EEDO Input is disabled for some PLX chips */
        u32RegValue &= ~(1 << 31);

        /* Enable EEPROM Chip Select */
        u32RegValue |= (1 << 25);

        /* Write 16-bit value to EEPROM - one bit at a time */
        for (s8BitPos = 15; s8BitPos >= 0; s8BitPos--)
        {
            /* Get bit value and shift into result */
            if (u16EepromValue & (1 << s8BitPos))
            {
                u32RegValueTemp = u32RegValue | (1 << 26);

                DDC_PLX_WRITE(
                    pDeviceContext,
                    REG_EEPROM_CTRL,
                    u32RegValueTemp);
            }
            else
            {
                DDC_PLX_WRITE(
                    pDeviceContext,
                    REG_EEPROM_CTRL,
                    u32RegValue);
            }

            /* Trigger the EEPROM clock */
            ddcUdlBusPciPlx9000_EepromClock(pDeviceContext);
        }

        /* Deselect Chip */
        u32RegValueTemp = u32RegValue & ~(1 << 25);

        DDC_PLX_WRITE(
            pDeviceContext,
            REG_EEPROM_CTRL,
            u32RegValueTemp);

        /* Re-select Chip */
        u32RegValueTemp = u32RegValue | (1 << 25);

        DDC_PLX_WRITE(
            pDeviceContext,
            REG_EEPROM_CTRL,
            u32RegValueTemp);

        /*****************************************************
         * Note: After the clocking in the last data bit, a
         *       delay is needed to let the EEPROM internally
         *       complete the write operation.  If a sleep
         *       mechanism is used, the result is an extremely
         *       slow EEPROM access since the delay resolution
         *       is too large.
         *
         *       Rather than using the standard sleep mechanism,
         *       the code, instead, reads the PLX register
         *       multiple times.  This is harmless and provides
         *       enough delay for the EEPROM write to complete.
         ****************************************************/

        /* A small delay is needed to let EEPROM complete */
        s32Timeout = 0;

        do
        {
            DDC_PLX_READ(
                pDeviceContext,
                REG_EEPROM_CTRL,
                &u32RegValue);

            s32Timeout++;

        } while (((u32RegValue & (1 << 27)) == 0) && (s32Timeout < 20000));

        /* Send Write_Disable command to EEPROM */
        ddcUdlBusPciPlx9000_EepromSendCommand(
            pDeviceContext,
            PLX9000_EE_CMD_WRITE_DISABLE << s8CommandShift,
            s8CommandLength);

        /* Clear Chip Select and all other EEPROM bits */
        u32RegValueTemp = u32RegValue & ~(0xF << 24);

        DDC_PLX_WRITE(
            pDeviceContext,
            REG_EEPROM_CTRL,
            u32RegValueTemp);
    }
}

/******************************************************************************
 *
 * Function   :  ddcUdlBusPciPlx9000_EepromSendCommand
 *
 * Description:  Sends a Command to the EEPROM
 *
 *****************************************************************************/
static void ddcUdlBusPciPlx9000_EepromSendCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32EepromCommand,
    U8BIT u8DataLengthInBits
)
{
    S8BIT s8BitPos;
    U32BIT u32RegValue;
    U32BIT u32RegValueTemp;

    DDC_PLX_READ(
        pDeviceContext,
        REG_EEPROM_CTRL,
        &u32RegValue);

    /* Clear all EEPROM bits (EECS, EEDI, EEDO, EESK) */
    u32RegValue &= ~(0xF << 24);

    /* Toggle EEPROM's Chip select to get it out of Shift Register Mode */
    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValue);

    /* Enable EEPROM Chip Select (EECS) */
    u32RegValue |= (1 << 25);

    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValue);

    /* Send EEPROM command - one bit at a time */
    for (s8BitPos = (S8BIT)(u8DataLengthInBits-1); s8BitPos >= 0; s8BitPos--)
    {
        /* Check if current bit is 0 or 1 */
        if (u32EepromCommand & (1 << s8BitPos))
        {
            u32RegValueTemp = u32RegValue | (1 << 26);
            DDC_PLX_WRITE(
                pDeviceContext,
                REG_EEPROM_CTRL,
                u32RegValueTemp);
        }
        else
        {
            DDC_PLX_WRITE(
                pDeviceContext,
                REG_EEPROM_CTRL,
                u32RegValue);
        }

        ddcUdlBusPciPlx9000_EepromClock(pDeviceContext);
    }
}

/******************************************************************************
 *
 * Function   :  ddcUdlBusPciPlx9000_EepromClock
 *
 * Description:  Sends the clocking sequence to the EEPROM
 *
 *****************************************************************************/
static void ddcUdlBusPciPlx9000_EepromClock
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S8BIT i;
    U32BIT u32RegValue;
    U32BIT u32RegValueTemp;

    DDC_PLX_READ(
        pDeviceContext,
        REG_EEPROM_CTRL,
        &u32RegValue);

    /* Set EEPROM clock High (EESK) */
    u32RegValueTemp = u32RegValue | (1 << 24);

    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValueTemp);

    /* Need a small delay, perform dummy register reads */
    for (i=0; i<20; i++)
    {
        DDC_PLX_READ(
            pDeviceContext,
            REG_EEPROM_CTRL,
            &u32RegValueTemp);
    }

    /* Set EEPROM clock Low */
    u32RegValueTemp = u32RegValue & ~(1 << 24);

    DDC_PLX_WRITE(
        pDeviceContext,
        REG_EEPROM_CTRL,
        u32RegValueTemp);
}
