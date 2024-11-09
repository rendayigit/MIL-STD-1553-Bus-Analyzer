/*******************************************************************************
 * FILE: ddc_udl_os_driver.c
 *
 * DESCRIPTION:
 *
 *  This file provides driver specific communication routines
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
 * Copyright (c) 2018 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#include "os/include/ddc_os_types.h"
#include "include/ddc_error_list.h"
#include "include/ddc_device_ids.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_bc_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/ddc_udl_hardware_private.h"
#include "driver_sdk/ddc_udl_iodev_private.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_interrupt_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/include/ddc_os_types_private.h"
#include "bus/ddc_udl_bus_private.h"

MODULE_LICENSE("GPL");

/*******************************************************************************
 * Name:    ddcUdlOsInit
 *
 * Description:
 *      This function should be called from the main driver initialization
 *      routine.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
S16BIT ddcUdlOsInit
(
    void
)
{
    int result;

#if 0 /* DEBUG */
{
    int i;

    /* set all trace levels for all modules */
    for (i=0; i<DDC_DBG_MODULE_MAX; i++)
    {
        ddcUdlDebugSetTraceLevel(i, DDC_DBG_ALL);
    }

    /* clear out individual modules */
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_BUS, DDC_DBG_ALL);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_CntrlRoutines);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1);
}
#endif /* DEBUG */

    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "ENTER\n");

    ddcUdlInitDeviceCount();

    result = ddcUdlBusInit();

    if (result != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT_INFO("ERROR: DDC ACEX driver NOT installed\n");

        return -ENODEV;
    }

    DDC_DBG_PRINT_INFO("Driver installed, %d boards found\n", ddcUdlGetDeviceCount());

    return result;
}

/*******************************************************************************
 * Name:    ddcUdlOsDeviceContexInit
 *
 * Description:
 *      Initialize information in the pDeviceContext structure.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlOsDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    return ddcUdlLateDeviceContexInit(pDeviceContext);
}

/*******************************************************************************
 * Name:    udlOsChannelRegistration
 *
 * Description:
 *      This function performs OS channel registration.
 *
 *      This function should only be called from the ddcUdlDeviceContexInit()
 *      routine.
 *
 * In   pDeviceContext
 * In   u8ChannelNumber
 * In   u8ChannelType
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT udlOsChannelRegistration
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
)
{
    int result = 0;

    if (u8ChannelType == DDC_FILE_NAME_TYPE_MIL_STD_1553)
    {
        /* holds name of device node for this 1553 channel */
        char sDevNode1553[16];
        const size_t sDevNode1553Len = 16;

        /* build device number for this 1553 channel (major is this driver; minor is the 1553 minor offset, plus this card offset, plus this 1553 channel */
        pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber] = (dev_t)(MKDEV(MAJOR(acexpci_major),
                MINOR(DDC_MIN_MINOR_1553 + (pDeviceContext->u8DeviceInstanceCount * MAX_NUM_1553_CHANNELS) + u8ChannelNumber)));

        /* initialize character device structure with same "file_operations" structure as rest of driver */
        cdev_init(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]), &ddcUdlHookFileOperations);
        pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber].owner = THIS_MODULE;

        /* register one (1) character device for this 1553 channel */
        result = cdev_add(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]), pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber], 1);
        if (result < 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device add failure for 1553 channel\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }

        /* build name of device node for this 1553 channel
         * NOTE: Since we need to support "MAX_NUM_DEVICES" (32) devices and since there are only 26 letters in the alphabet,
         *   we start capitalizing letters for the last six devices */
        if (pDeviceContext->u8DeviceInstanceCount < 26)
        {
            snprintf(sDevNode1553, sDevNode1553Len, "%s%c%d", MODULE_NAME, (char)(pDeviceContext->u8DeviceInstanceCount) + 'a', (u8ChannelNumber + 1));
        }
        else if ((pDeviceContext->u8DeviceInstanceCount >= 26) || (pDeviceContext->u8DeviceInstanceCount < MAX_NUM_DEVICES))
        {
            snprintf(sDevNode1553, sDevNode1553Len, "%s%c%d", MODULE_NAME, (char)(pDeviceContext->u8DeviceInstanceCount % 26) + 'A', (u8ChannelNumber + 1));
        }
        else
        {
            /* cannot build device node name that corresponds to a device instance beyond "MAX_NUM_DEVICES" (32) */
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "cannot prepare device node name for this 1553 channel\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }

        /* create character device and register it with sysfs so that it appears as a device node (e.g. /dev/...)
         * NOTE: the device structure corresponding to the PCI device structure for this device is set as the parent
         * NOTE: since there is no macro that can be used for testing which function is appropriate, we look at the kernel version */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26)
        pDeviceContext->ddcOsDevInfo.pDevice1553[u8ChannelNumber] = device_create(
            acexpci_class,                                                     /* pointer to the struct class that this device should be registered to */
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),             /* pointer to the parent struct device of this new device */
            pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber],   /* the dev_t for the char device to be added */
            NULL,                                                              /* the data to be added to the device for callbacks */
            "%s",                                                              /* format string for the device’s name */
            sDevNode1553);                                                     /* 1553 channels are named "MODULE_NAME", followed by a letter of the alphabet, followed by an integer */
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 26)
        /* function name is different for this kernel version */
        pDeviceContext->ddcOsDevInfo.pDevice1553[u8ChannelNumber] = device_create_drvdata(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber],
            NULL,
            "%s",
            sDevNode1553);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17))
        /* no "drvdata" parameter for these kernel versions */
        pDeviceContext->ddcOsDevInfo.pDevice1553[u8ChannelNumber] = device_create(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber],
            "%s",
            sDevNode1553);
#else
#       error "device_create() not available"
#endif

        if (IS_ERR(pDeviceContext->ddcOsDevInfo.pDevice1553[u8ChannelNumber]))
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device registration failure for 1553 channel\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }
    }
    else if (u8ChannelType == DDC_FILE_NAME_TYPE_ARINC_429)
    {
        /* holds name of device node for this 429 device */
        char sDevNode429[16];
        const size_t sDevNode429Len = 16;

        /* build device number for this 429 device (major is this driver; minor is the 429 minor offset, plus this card offset */
        pDeviceContext->ddcOsDevInfo.devMajorMinor429 = (dev_t)(MKDEV(MAJOR(acexpci_major),
                MINOR(DDC_MIN_MINOR_ARINC_429 + pDeviceContext->u8DeviceInstanceCount)));

        /* initialize character device structure with same "file_operations" structure as rest of driver */
        cdev_init(&(pDeviceContext->ddcOsDevInfo.charDevice429), &ddcUdlHookFileOperations);
        pDeviceContext->ddcOsDevInfo.charDevice429.owner = THIS_MODULE;

        /* register one (1) character device for this 429 device */
        result = cdev_add(&(pDeviceContext->ddcOsDevInfo.charDevice429), pDeviceContext->ddcOsDevInfo.devMajorMinor429, 1);
        if (result < 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device add failure for 429 device\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice429));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }

        /* build name of device node for this 429 device
         * NOTE: Since we need to support "MAX_NUM_DEVICES" (32) devices and since there are only 26 letters in the alphabet,
         *   we start capitalizing letters for the last six devices */
        if (pDeviceContext->u8DeviceInstanceCount < 26)
        {
            snprintf(sDevNode429, sDevNode429Len, "%s%c", MODULE_NAME, (char)(pDeviceContext->u8DeviceInstanceCount) + 'a');
        }
        else if ((pDeviceContext->u8DeviceInstanceCount >= 26) || (pDeviceContext->u8DeviceInstanceCount < MAX_NUM_DEVICES))
        {
            snprintf(sDevNode429, sDevNode429Len, "%s%c", MODULE_NAME, (char)(pDeviceContext->u8DeviceInstanceCount % 26) + 'A');
        }
        else
        {
            /* cannot build device node name that corresponds to a device instance beyond "MAX_NUM_DEVICES" (32) */
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "cannot prepare device node name for this 429 device\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice429));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }

        /* create character device and register it with sysfs so that it appears as a device node (e.g. /dev/...)
         * NOTE: the device structure corresponding to the PCI device structure for this device is set as the parent
         * NOTE: since there is no macro that can be used for testing which function is appropriate, we look at the kernel version */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26)
        pDeviceContext->ddcOsDevInfo.pDevice429 = device_create(
            acexpci_class,                                          /* pointer to the struct class that this device should be registered to */
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),  /* pointer to the parent struct device of this new device */
            pDeviceContext->ddcOsDevInfo.devMajorMinor429,          /* the dev_t for the char device to be added */
            NULL,                                                   /* the data to be added to the device for callbacks */
            "%s",                                                   /* format string for the device’s name */
            sDevNode429);                                           /* 429 devices are named "MODULE_NAME", followed by a letter of the alphabet */
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 26)
        /* function name is different for this kernel version */
        pDeviceContext->ddcOsDevInfo.pDevice429 = device_create_drvdata(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinor429,
            NULL,
            "%s",
            sDevNode429);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17))
        /* no "drvdata" parameter for these kernel versions */
        pDeviceContext->ddcOsDevInfo.pDevice429 = device_create(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinor429,
            "%s",
            sDevNode429);
#else
#       error "device_create() not available"
#endif

        if (IS_ERR(pDeviceContext->ddcOsDevInfo.pDevice429))
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device registration failure for 429 device\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice429));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }
    }
    else if (u8ChannelType == DDC_FILE_NAME_TYPE_IO)
    {
        /* build device number for this I/O device (major is this driver; minor is the I/O minor offset, plus this card offset */
        pDeviceContext->ddcOsDevInfo.devMajorMinorIO = (dev_t)(MKDEV(MAJOR(acexpci_major),
                MINOR(DDC_MIN_MINOR_DISCRETE + pDeviceContext->u8DeviceInstanceCount)));

        /* initialize character device structure with same "file_operations" structure as rest of driver */
        cdev_init(&(pDeviceContext->ddcOsDevInfo.charDeviceIO), &ddcUdlHookFileOperations);
        pDeviceContext->ddcOsDevInfo.charDeviceIO.owner = THIS_MODULE;

        /* register one (1) character device for this I/O device */
        result = cdev_add(&(pDeviceContext->ddcOsDevInfo.charDeviceIO), pDeviceContext->ddcOsDevInfo.devMajorMinorIO, 1);
        if (result < 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device add failure for I/O device\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDeviceIO));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }

        /* create character device and register it with sysfs so that it appears as a device node (e.g. /dev/...)
         * NOTE: the device structure corresponding to the PCI device structure for this device is set as the parent
         * NOTE: since there is no macro that can be used for testing which function is appropriate, we look at the kernel version */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26)
        pDeviceContext->ddcOsDevInfo.pDeviceIO = device_create(
            acexpci_class,                                          /* pointer to the struct class that this device should be registered to */
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),  /* pointer to the parent struct device of this new device */
            pDeviceContext->ddcOsDevInfo.devMajorMinorIO,           /* the dev_t for the char device to be added */
            NULL,                                                   /* the data to be added to the device for callbacks */
            "%s%d",                                                 /* format string for the device’s name */
            MODULE_NAME,                                            /* I/O devices are named "MODULE_NAME", followed by an integer */
            (int)(pDeviceContext->u8DeviceInstanceCount) + 1);
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 26)
        /* function name is different for this kernel version */
        pDeviceContext->ddcOsDevInfo.pDeviceIO = device_create_drvdata(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinorIO,
            NULL,
            "%s%d",
            MODULE_NAME,
            (int)(pDeviceContext->u8DeviceInstanceCount) + 1);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17))
        /* no "drvdata" parameter for these kernel versions */
        pDeviceContext->ddcOsDevInfo.pDeviceIO = device_create(
            acexpci_class,
            &(pDeviceContext->ddcOsDevInfo.sBusInfo.pPciDev->dev),
            pDeviceContext->ddcOsDevInfo.devMajorMinorIO,
            "%s%d",
            MODULE_NAME,
            (int)(pDeviceContext->u8DeviceInstanceCount) + 1);
#else
#       error "device_create() not available"
#endif

        if (IS_ERR(pDeviceContext->ddcOsDevInfo.pDeviceIO))
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "character device registration failure for I/O device\n");
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDeviceIO));
            return DDC_UDL_ERROR__DRIVER_INITIALIZATION;
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    udlOsChannelUnregistration
 *
 * Description:
 *      This function performs OS channel unregistration.
 *
 *      This function should only be called from the ddcUdlExit()
 *      routine.
 *
 * In   pDeviceContext
 *
 * Returns: none
 ******************************************************************************/
void udlOsChannelUnregistration
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT u8ChannelNumber = 0;

    /* unregister 1553 channels */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_EXIT, "unregister 1553 dev\n");
    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++)
    {
        if (pDeviceContext->ddcOsDevInfo.pDevice1553[u8ChannelNumber] != NULL)
        {
            /* delete device node for character device */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17)
        	device_destroy(acexpci_class, pDeviceContext->ddcOsDevInfo.devMajorMinor1553[u8ChannelNumber]);
#else
#       	error "device_destroy() not available"
#endif

            /* unregister character device */
            cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice1553[u8ChannelNumber]));
        }
    } /* 1553 channel for loop */

    /* unregister ARINC 429 */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_EXIT, "unregister 429\n");
    if (pDeviceContext->ddcOsDevInfo.pDevice429 != NULL)
    {
        /* delete device node for character device */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17)
        device_destroy(acexpci_class, pDeviceContext->ddcOsDevInfo.devMajorMinor429);
#else
#       error "device_destroy() not available"
#endif

        /* unregister character device */
        cdev_del(&(pDeviceContext->ddcOsDevInfo.charDevice429));
    }

    /* unregister I/O */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_EXIT, "unregister I/O\n");
    if (pDeviceContext->ddcOsDevInfo.pDeviceIO != NULL)
    {
        /* delete device node for character device */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17)
        device_destroy(acexpci_class, pDeviceContext->ddcOsDevInfo.devMajorMinorIO);
#else
#       error "device_destroy() not available"
#endif

        /* unregister character device */
        cdev_del(&(pDeviceContext->ddcOsDevInfo.charDeviceIO));
    }

    return;
}

/*******************************************************************************
 * Name:    ddcUdlOsExit
 *
 * Description:
 *      This function should be called from the main driver exit routine.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlOsExit
(
    void
)
{
    ddcUdlOsBusExit();
}

/*******************************************************************************
 * Name:    ddcUdlOsDeviceAllocate
 *
 * Description:
 *      This function allocates the device context structure item.
 *
 * In   u8DeviceCount   which device to allocate
 * In   pDevice         pointer to a _DDC_UDL_DEVICE_CONTEXT
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlOsDeviceAllocate
(
    U8BIT u8DeviceCount,
    struct _DDC_UDL_DEVICE_CONTEXT **ppDevice
)
{
    *ppDevice = DDC_KERNEL_MALLOC(*ppDevice, sizeof(struct _DDC_UDL_DEVICE_CONTEXT));
}

