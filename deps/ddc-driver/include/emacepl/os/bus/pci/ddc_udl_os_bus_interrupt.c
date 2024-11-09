/*******************************************************************************
 * FILE: ddc_udl_os_bus_interrupt.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide the function prototypes for 
 *  OS specific, generic bus, IRQ hook routines. 
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
#include "os/interface/ddc_udl_os_util_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_interrupt_private.h"
#include "ddc_udl_os_bus_interrupt_private.h"

/* <UDL2> */
#define dbg_print(...) 



/* Device ID for IRQ's.  Note that declaring this in the        */
/* 'ioctl()' method results in a 'memset' module load error.    */
S8BIT szDevName[32] = "acexpci_";

/* Device ID parameter for ISR processing */
/* These are used to indicate, within the ISR, which card issued an interrupt */
U8BIT au8Dev_id[MAX_NUM_DEVICES] =
{
     0,  1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31
};


#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
irqreturn_t ddcUdlOsIsr
(
    int irq,
    void *pCardNumber,
    struct pt_regs *regs
);
#else

/* ========================================================================== */
/* ========================================================================== */
irqreturn_t ddcUdlOsIsr
(
    int irq,
    void *pCardNumber
);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) */

/*******************************************************************************
 * Name:    ddcUdlOsBusIrqHookISR
 *
 * Description:
 *      This function hooks up the Interrupt Service Routine for the PCI bus.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8CardNumber    card number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusIrqHookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
)
{
    unsigned long ulIrqReqFlags;
    unsigned long ulMsiStatus;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
    ulIrqReqFlags = SA_SHIRQ;
#else
    ulIrqReqFlags = IRQF_SHARED;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) */

/*---------------------------------------------------------------------------------
* In some Linux Kernel, MSI can be enabled but no interrupt will be generated.
* When this happens, DDC cards will not work at all. To avoid this problem,
* MSI interrupts are not enabled by default. User has to uncomment the
* DDC_MSI_SUPPORT definition in ddc_udl_os_bus_interrupt_private.h.
*---------------------------------------------------------------------------------*/
#ifdef DDC_MSI_SUPPORT
    ulMsiStatus = pci_enable_msi(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev);
#else
    ulMsiStatus = 1;
#endif

    if (ulMsiStatus) /* MSI_UNAVAILABLE */
    {
        dbg_print(DBG_MODULE_INTERRUPT, DBG_PRIORITY_INFO, "LINE irq installed\n");

        /* Register the IRQ with the kernel as shared slow interrupt */
        if (request_irq(pDeviceContext->ddcOsDevInfo.sBusInfo.u16Irq, ddcUdlOsIsr, ulIrqReqFlags, szDevName, &au8Dev_id[u8CardNumber]))
        {
            dbg_print(DBG_MODULE_INTERRUPT, DBG_PRIORITY_ERROR, "register irq failed\n");
            return DDC_UDL_ERROR__IRQ_REQUEST_FAIL;
        }
    }
    else /* MSI_ENABLED */
    {
        dbg_print(DBG_MODULE_INTERRUPT, DBG_PRIORITY_INFO, "MSI irq installed\n");

        /* Register the IRQ with the kernel with no flags as MSI */
        pDeviceContext->ddcOsDevInfo.sBusInfo.u16Irq = pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->irq;
            
        if (request_irq(pDeviceContext->ddcOsDevInfo.sBusInfo.u16Irq, ddcUdlOsIsr, 0, szDevName, &au8Dev_id[u8CardNumber]))
        {
            dbg_print(DBG_MODULE_INTERRUPT, DBG_PRIORITY_ERROR, "register irq failed\n");
            return DDC_UDL_ERROR__IRQ_REQUEST_FAIL;
        }
    }

    /* set board interrupt mask bit */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsBusIrqUnhookISR
 *
 * Description:
 *      This function unhooks the Interrupt Service Routine
 *
 *
 * In   pDeviceContext  device-specific structure
 * In   u8CardNumber    card number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsBusIrqUnhookISR
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8CardNumber
)
{
    /* Disable/abort DMA transactions and wait for completion */
    switch (pDeviceContext->u16DriverType)
    {
        case ACEX_IO_DRIVER:
        {
            dbg_print(DBG_MODULE_ACEX_DRIVER, DBG_PRIORITY_INFO, "IO device\n");

            break;
        }

        case ACEX_QPRM_DRIVER:
        {
            dbg_print(DBG_MODULE_ACEX_DRIVER, DBG_PRIORITY_INFO, "QPRM\n");

            ddcUdlOsBusAsicDmaWrite(pDeviceContext, REG_QPRM_DMA_CS, QPRIME_DMA_CS_CLEAR_INT | QPRIME_DMA_CS_ABORT);
            ddcUdlOsWaitUs(2000);
            break;
        }

        case ACEX_PCIE_DRIVER:
        case ACEX_DD429_PCIE_DRIVER:
        {
            dbg_print(DBG_MODULE_ACEX_DRIVER, DBG_PRIORITY_INFO, "PCIE\n");

            break;
        }

        case ACEX_DD429_PCI_DRIVER:
        default:
        {
            dbg_print(DBG_MODULE_ACEX_DRIVER, DBG_PRIORITY_INFO, "PCI devices\n");
            ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_CSR, 0x0000000C);
            ddcUdlOsWaitUs(2000);
            break;
        }
    }

    /* Disable the master interrupt signal */
    ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ);

    dbg_print(DBG_MODULE_INTERRUPT, DBG_PRIORITY_INFO, "free irq and destroy workqueue\n");

    free_irq(pDeviceContext->ddcOsDevInfo.sBusInfo.u16Irq, &au8Dev_id[u8CardNumber]);

#ifdef DDC_MSI_SUPPORT
    pci_disable_msi(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev);
#endif

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlOsIsr
 *
 * Description:
 *      This function gets the board level interrupt status and
 *      save them into pDeviceContext->u32BdIntStatus.
 *
 * In   irq
 * In   dev_id      device id
 * In   regs
 * Out  none
 *
 * Returns: IRQ handled condition
 ******************************************************************************/
irqreturn_t ddcUdlOsIsr
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
(
    int irq,
    void *pCardNumber,
    struct pt_regs *regs
)
#else
(
    int irq,
    void *pCardNumber
)
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) */
{
    BOOLEAN bInterruptHandled;

    bInterruptHandled = ddcUdlIsr(*(U8BIT *)pCardNumber);

    if (bInterruptHandled)
    {
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}
