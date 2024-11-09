/*******************************************************************************
 * FILE: ddc_udl_os_hook.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide the functions that interface (hook)
 *  into the OS.
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
#include "os/include/ddc_os_private.h"
#include "include/ddc_error_list.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/ddc_udl_interrupt_private.h"
#include "driver_sdk/ddc_udl_ioctl_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_driver_private.h"

#define LINUX_SUCCESS   0

#ifndef EPASSTHROUGH
#define EPASSTHROUGH    -4              /* ioctl not handled by this layer */
#endif /* EPASSTHROUGH */

#define DDC_IOCTL_CALL              0
#define DDC_COMPAT_IOCTL_CALL       1

static int _ddcUdlPreIoctl
(
    struct inode *inode,
    struct file *filp,
    unsigned int ioctl_num,
    unsigned long ioctl_param,
    U8BIT u8IsCompatCall
);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static long _ddcUdlHookUnlockedIoctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
);
static long _ddcUdlHookUnlockedCompatIoctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
);

#else

static int _ddcUdlHookIoctl
(
    struct inode *inode,
    struct file *filp,
    unsigned int ioctl_num,
    unsigned long ioctl_param
);

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */

static int _ddcUdlHookOpen
(
    struct inode *inode,
    struct file *filp
);

static int _ddcUdlHookClose
(
    struct inode *inode,
    struct file *filp
);

/******************************************************************************
 * Name: ddcUdlOsDynamicMemoryInfo
 *
 * Description:
 *      This function prints out the dynamic memory information.
 *
 * Returns: none
 *****************************************************************************/
void ddcUdlOsDynamicMemoryInfo
(
    void
)
{
    /* doing nothing */
}

/* ------------------------------------------------------------ */
/* File Operations Structure                                    */
/*                                                              */
/* Hooks our driver routines to the various device system calls */
/* ------------------------------------------------------------ */
const struct file_operations ddcUdlHookFileOperations =
{
   .owner = THIS_MODULE,

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
   .unlocked_ioctl = _ddcUdlHookUnlockedIoctl,
   .compat_ioctl = _ddcUdlHookUnlockedCompatIoctl,
#else
   .ioctl = _ddcUdlHookIoctl,
#endif

   .open = _ddcUdlHookOpen,
   .release = _ddcUdlHookClose
};


/*******************************************************************************
 * Name:    _ddcUdlHookModuleInit
 *
 * Description:
 *      module initialization routine:
 *      This function is called by the kernel when the driver module is first loaded
 *      (insmod, modprobe) into the running kernel.  We do some type size checking
 *      to ensure that the primitive types are of the dimension that our driver
 *      expects, then register this device as a character device with automatic
 *      major number generation.  Finally, we scan for our cards.
 *
 *      Note that return values are passed back to the module loader, and any value
 *      other than 0 will prevent the module from being loaded.
 *
 * In   none
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static int __init _ddcUdlHookModuleInit
(
    void
)
{
    int result;

    result = ddcUdlInit();

    return result;
}


/*******************************************************************************
 * Name:    _ddcUdlHookModuleExit
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
static void __exit _ddcUdlHookModuleExit
(
    void
)
{
    ddcUdlExit();
}



/******************************************************************************
 * Name:    _ddcUdlPreIoctl
 *
 * Description:
 *      Driver OS specific IOCTL method. It will be called when a 32/64-bit
 *      applications running in a 32/64-bit Kernel.
 *
 * In   inode               Information node to device
 * In   filp                Device file pointer
 * In   wCmd                Command
 * In   dwParam             Command parameter structure
 * In   u8IsCompatCall      set if called from compat_ioctl
 *                          DDC_IOCTL_CALL          0
 *                          DDC_COMPAT_IOCTL_CALL   1
 * Out  none
 *
 * Returns: '0' for success, '>0' success with info., '<0' error
 *****************************************************************************/
static int _ddcUdlPreIoctl
(
    struct inode *inode,
    struct file *filp,
    unsigned int ioctl_num,
    unsigned long ioctl_param,
    U8BIT u8IsCompatCall
)
{
    U32BIT u32DeviceIndex;
    U8BIT  u8CardNumber;
    U8BIT  u8ChannelNumber;
    U32BIT u32InputBufferLength;
    U32BIT u32DataBuffersSize;

    DDC_IOCTL_DATA_BUFFERS_TYPE sIoctlDataBuffers;
    DDC_IOCTL_COMPAT_DATA_BUFFERS_TYPE sIoctlCompatDataBuffers;

    struct _DDC_UDL_DEVICE_CONTEXT  *pDeviceContext = NULL;
    DDC_DEV_HANDLE_TYPE handleType;

    S16BIT s16Result    = 0;
    int    linux_result = LINUX_SUCCESS;

    u32DeviceIndex   = MINOR(inode->i_rdev);
    u8CardNumber    = (U8BIT)(u32DeviceIndex / MAX_NUM_1553_CHANNELS);
    u8ChannelNumber = u32DeviceIndex % MAX_NUM_1553_CHANNELS;
    handleType = DDC_DEV_HANDLE_MIL_STD_1553;

    /* check if the ARINC library is accessing the driver */
    if (u32DeviceIndex >= DDC_MIN_MINOR_ARINC_429 && u32DeviceIndex < DDC_MIN_MINOR_DISCRETE)
    {
        u8CardNumber = u32DeviceIndex - DDC_MIN_MINOR_ARINC_429;
        handleType = DDC_DEV_HANDLE_ARINC_429;
    }

    /* check if the I/0 library is accessing the driver */
    if (u32DeviceIndex >= DDC_MIN_MINOR_DISCRETE)
    {
        u8CardNumber = u32DeviceIndex - DDC_MIN_MINOR_DISCRETE;
        handleType = DDC_DEV_HANDLE_IO;
    }

    /* retrieve IOCTL parameters */
    if (ioctl_param)
    {
        if (u8IsCompatCall)
        {
            s16Result = (S16BIT)DDC_COPY_FROM_USER((char *)&sIoctlCompatDataBuffers, (char *)ioctl_param, sizeof(DDC_IOCTL_COMPAT_DATA_BUFFERS_TYPE));

            u32InputBufferLength = sIoctlCompatDataBuffers.sInBuffer.u32BufferLength;
            u32DataBuffersSize   = (U32BIT)sizeof(DDC_IOCTL_COMPAT_DATA_BUFFERS_TYPE);
        }
        else
        {
            s16Result = (S16BIT)DDC_COPY_FROM_USER((char *)&sIoctlDataBuffers, (char *)ioctl_param, sizeof(DDC_IOCTL_DATA_BUFFERS_TYPE));

            u32InputBufferLength = sIoctlDataBuffers.sInBuffer.u32BufferLength;
            u32DataBuffersSize   = (U32BIT)sizeof(DDC_IOCTL_DATA_BUFFERS_TYPE);
        }

        /* non-zero indicates some data could not be copied */
        if (s16Result != 0)
        {
            return -EFAULT;
        }

        /* set the ioctl params from the compat params.*/
        if (u8IsCompatCall)
        {
            sIoctlDataBuffers.sInBuffer.pDataBuffer                   = (unsigned long )sIoctlCompatDataBuffers.sInBuffer.pDataBuffer;
            sIoctlDataBuffers.sInBuffer.u32BufferLength               = sIoctlCompatDataBuffers.sInBuffer.u32BufferLength;

            sIoctlDataBuffers.sReturnStatus.pDataBuffer               = (unsigned long )sIoctlCompatDataBuffers.sReturnStatus.pDataBuffer;
            sIoctlDataBuffers.sReturnStatus.u32BufferLength           = sIoctlCompatDataBuffers.sReturnStatus.u32BufferLength;

            sIoctlDataBuffers.sOutBufferBytesReturned.pDataBuffer     = (unsigned long )sIoctlCompatDataBuffers.sOutBufferBytesReturned.pDataBuffer;
            sIoctlDataBuffers.sOutBufferBytesReturned.u32BufferLength = sIoctlCompatDataBuffers.sOutBufferBytesReturned.u32BufferLength;

            sIoctlDataBuffers.sInOutBuffer.pDataBuffer                = (unsigned long )sIoctlCompatDataBuffers.sInOutBuffer.pDataBuffer;
            sIoctlDataBuffers.sInOutBuffer.u32BufferLength            = sIoctlCompatDataBuffers.sInOutBuffer.u32BufferLength;
        }

        /* get device context */
        pDeviceContext = ddcUdlGetDeviceContext(u8CardNumber);

        /* execute commands */
        s16Result = ddcUdlIoctl(pDeviceContext, &sIoctlDataBuffers, ioctl_num, u8CardNumber, u8ChannelNumber, u8IsCompatCall, handleType);

        /* set the return value */
        s16Result = DDC_COPY_TO_USER((char *)sIoctlDataBuffers.sReturnStatus.pDataBuffer, (char *)&s16Result, sIoctlDataBuffers.sReturnStatus.u32BufferLength);
        if (s16Result)
        {
            s16Result = DDC_UDL_ERROR__COPY2USER_FAIL;
        }

    }/* if (ioctl_param) */

    /* map UDL error codes to OS specific */
    switch (s16Result)
    {
        case DDC_UDL_ERROR__IOCTL_NOT_IMPLEMENTED:
        {
            /* for ioctl codes that are not handled */
            linux_result = EPASSTHROUGH;
            break;
        }

        default:
        {
            /* return the exsiting result */
            linux_result = s16Result;
            break;
        }
    }

    IO_SYNC;

    return linux_result;
}

/* ========================================================================== */
/* ========================================================================== */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)

static long _ddcUdlHookUnlockedIoctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    long result = 0;

    result = (long)_ddcUdlPreIoctl(file->f_path.dentry->d_inode, file, cmd, arg, DDC_IOCTL_CALL);

    return result;
}

static long _ddcUdlHookUnlockedCompatIoctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    long result = 0;

    result = (long)_ddcUdlPreIoctl(file->f_path.dentry->d_inode, file, cmd, arg, DDC_COMPAT_IOCTL_CALL);

    return result;
}

#else

static int _ddcUdlHookIoctl
(
    struct inode *inode,
    struct file *filp,
    unsigned int ioctl_num,
    unsigned long ioctl_param
)
{
    return _ddcUdlPreIoctl(inode, filp, ioctl_num, ioctl_param, DDC_IOCTL_CALL);
}

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */


/*******************************************************************************
 * Name:    _ddcUdlHookOpen
 *
 * Description:
 *
 *      'open()' method that is called when the driver is opened from the
 *      library.
 *
 * In   inode
 * In   filp
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static int _ddcUdlHookOpen
(
    struct inode *inode,
    struct file *filp
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext = NULL;
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32DevIndx = MINOR(inode->i_rdev);
    U8BIT u8CardNumber = (U8BIT)(u32DevIndx / MAX_NUM_1553_CHANNELS);
    U8BIT u8ChannelNumber = u32DevIndx % MAX_NUM_1553_CHANNELS;
    U8BIT u8CardCount = 0;
    U8BIT u8ChannelType;

    /* check if the ARINC library is making the access */
    if (u32DevIndx >= DDC_MIN_MINOR_ARINC_429 && u32DevIndx < DDC_MIN_MINOR_DISCRETE)
    {
        /* adjust the card number accordingly */
        u8CardNumber = u32DevIndx - DDC_MIN_MINOR_ARINC_429;
        u8ChannelType = DDC_UDL_PROCESS_ARINC_SERIAL;

        /* channel # is only used for 1553, so keep it 0 */
        u8ChannelNumber = 0;
    }
    else if (u32DevIndx >= DDC_MIN_MINOR_DISCRETE) /* check if the discrete library is making the access */
    {
        /* adjust the card number accordingly */
        u8CardNumber = u32DevIndx - DDC_MIN_MINOR_DISCRETE;
        u8ChannelType = DDC_UDL_PROCESS_DISCRETE;

        /* channel # is only used for 1553, so keep it 0 */
        u8ChannelNumber = 0;
    }
    else /* 1553 channel */
    {
        /* 1553 channel numbers are mapped to the beginning channel types */
        u8ChannelType = u8ChannelNumber;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
        "minor %d  u8CardNumber %d  u8ChannelNumber %d\n",
        u32DevIndx, u8CardNumber, u8ChannelNumber);

    u8CardCount = ddcUdlGetDeviceCount();

    /* ensure that we are accessing a valid device (card and channel) */
    if (u8CardNumber >= u8CardCount)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
            "Invalid device u8CardNumber %d u8CardCount %d\n",
            u8CardNumber, u8CardCount);

        return -EBUSY;
    }

    pDeviceContext = ddcUdlGetDeviceContext(u8CardNumber);


    /* exit if it is CPLD IO device */
    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
            "I/O devices are not to be accessed (u8CardNumber %d u8NumberDevicesInstalled %d)\n", u8CardNumber, u8CardCount);

        return DDC_UDL_ERROR__DEVICE_DOES_NOT_EXIST;
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    /* return error if the corresponding channel is already opened */
    if ( ( (u32DevIndx >= DDC_MIN_MINOR_ARINC_429) && (u32DevIndx < DDC_MIN_MINOR_DISCRETE) && pDeviceContext->u8ArincSerialOpen)   ||
         ( (u32DevIndx >= DDC_MIN_MINOR_DISCRETE) && pDeviceContext->u8DiscreteOpen)   ||
         ( (u32DevIndx < DDC_MIN_MINOR_ARINC_429) && pDeviceContext->u8Channel1553Open[u8ChannelNumber]) )
    {
        /* Release Mutex */
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN, "Device Already Opened\n");
        return DDC_UDL_ERROR__DEVICE_ALREADY_OPEN;
    }

    /* Flag channel as open */
    if (u32DevIndx >= DDC_MIN_MINOR_ARINC_429 && u32DevIndx < DDC_MIN_MINOR_DISCRETE)
    {
        pDeviceContext->u8ArincSerialOpen = 1;
    }
    else if (u32DevIndx >= DDC_MIN_MINOR_DISCRETE)
    {
        pDeviceContext->u8DiscreteOpen = 1;
    }
    else
    {
        pDeviceContext->u8Channel1553Open[u8ChannelNumber] = 1;
    }
    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    s16Result = ddcUdlDeviceOpen(u8CardNumber, u8ChannelNumber, u8ChannelType);

    if (s16Result != DDC_UDL_ERROR__SUCCESS)
    {
		DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
				"Failed ddcUdlDeviceOpen with EBUSY\n");
        return -EBUSY;
    }

    if (!try_module_get(THIS_MODULE))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
            "call of try_module_get failed!\n");

        return -EBUSY;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _ddcUdlHookClose
 *
 * Description:
 *      'close()' method:
 *      A 'disable interrupt processing' 'ioctl()' call is also made at this point
 *      to free up any IRQ resources that may not have been freed properly.
 *
 *      Since the release method could be called from user applications, as well as
 *      when a process with a handle to the device is killed, we must clean up as
 *      much as we can and only return failure on situations where the method is
 *      called from a user app.  Lets not return errors indicating bad programming
 *      practices here, (not unmapping, disabling interrupt processing or properly
 *      closing).  The other methods are a good place for this type of thing.
 *
 *      Note that by not properly unmapping and closing a device, it is impossible
 *      to re-open the device within the same process.  With memory windows still
 *      mapped, the release method doesn't actually get run until the process is
 *      terminated.  This is the reason that we need to 'promote good programming
 *      practices'.  Our libraries need the capability to re-open devices within
 *      one process.
 *
 * In   inode
 * In   filp
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static int _ddcUdlHookClose
(
    struct inode *inode,
    struct file *filp
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext = NULL;
    S16BIT s16Result;
    U32BIT u32DevIndx = MINOR(inode->i_rdev);
    U8BIT u8CardNumber = (U8BIT)(u32DevIndx / MAX_NUM_1553_CHANNELS);
    U8BIT u8ChannelNumber = u32DevIndx % MAX_NUM_1553_CHANNELS;
    U8BIT u8CardCount = 0;
    U8BIT u8Index;
    U8BIT bAnyOpen = 1;

    if (u32DevIndx >= DDC_MIN_MINOR_ARINC_429 && u32DevIndx < DDC_MIN_MINOR_DISCRETE)
    {
        u8CardNumber = u32DevIndx - DDC_MIN_MINOR_ARINC_429;
        u8Index = DDC_UDL_PROCESS_ARINC_SERIAL;

        /* channel # is only used for 1553, so keep it 0 */
        u8ChannelNumber = 0;
    }
    else if (u32DevIndx >= DDC_MIN_MINOR_DISCRETE)
    {
        u8CardNumber = u32DevIndx - DDC_MIN_MINOR_DISCRETE;
        u8Index = DDC_UDL_PROCESS_DISCRETE;

        /* channel # is only used for 1553, so keep it 0 */
        u8ChannelNumber = 0;
    }
    else
    {
        u8Index = u8ChannelNumber;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_CLOSE,
        "minor %d  u8CardNumber %d  u8ChannelNumber %d\n",
        u32DevIndx, u8CardNumber, u8ChannelNumber);

    u8CardCount = ddcUdlGetDeviceCount();

    /* ensure that we are accessing a valid device (card and channel) */
    if (u8CardNumber >= u8CardCount)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_CLOSE,
            "Invalid device u8CardNumber %d u8CardCount %d\n",
            u8CardNumber, u8CardCount);

        return DDC_UDL_ERROR__DEVICE_DOES_NOT_EXIST;
    }

    pDeviceContext = ddcUdlGetDeviceContext(u8CardNumber);

    /* exit if it is CPLD IO device */
    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_CLOSE,
            "I/O devices are not to be accessed (u8CardNumber %d u8NumberDevicesInstalled %d)\n", u8CardNumber, u8CardCount);

        return DDC_UDL_ERROR__DEVICE_DOES_NOT_EXIST;
    }

    /* indicate we are busy closing */
    ddcUdlSetDeviceCloseBusyStatus(pDeviceContext, u8Index, TRUE);

    s16Result = ddcUdlDeviceClose(u8CardNumber, u8ChannelNumber, u8Index);

    if (s16Result != DDC_UDL_ERROR__SUCCESS)
    {
		DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
			"Failed ddcUdlDeviceClose with s16Result %x\n", s16Result);
        return s16Result;
    }

	/* clear the corresponding file opened flag */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    if (u32DevIndx >= DDC_MIN_MINOR_ARINC_429 && u32DevIndx < DDC_MIN_MINOR_DISCRETE)
    {
        pDeviceContext->u8ArincSerialOpen = 0;
    }
    else if (u32DevIndx >= DDC_MIN_MINOR_DISCRETE)
    {
        pDeviceContext->u8DiscreteOpen = 0;
    }
    else
    {
        pDeviceContext->u8Channel1553Open[u8ChannelNumber] = 0;
    }

    bAnyOpen = (pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_01] ||    /* Per card */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_02] ||    /* channel  */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_03] ||    /* status   */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_04] ||
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_05] ||    /* Per card */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_06] ||    /* channel  */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_07] ||    /* status   */
                pDeviceContext->u8Channel1553Open[DDC_UDL_PROCESS_1553_CHANNEL_08] ||
                pDeviceContext->u8ArincSerialOpen                                 ||
                pDeviceContext->u8DiscreteOpen);

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    /* If no channel is open, free BC */
    if (bAnyOpen == 0)
    {
		DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DEVICE_OPEN,
			" no channel is open, free BC  bAnyOpen %x\n", bAnyOpen);
        /* free BC memory*/
        bcFree(pDeviceContext);
    }

    /* indicate we are done closing */
    ddcUdlSetDeviceCloseBusyStatus(pDeviceContext, u8Index, FALSE);

    /* Decrement module usage count */
    module_put(THIS_MODULE);

    return DDC_UDL_ERROR__SUCCESS;
}



/* ========================================================================== */
/* ========================================================================== */
/* DO NOT ADD ANY FUNCTIONS BELOW THIS                                        */
/* ========================================================================== */
/* ========================================================================== */

module_init(_ddcUdlHookModuleInit);
module_exit(_ddcUdlHookModuleExit);
