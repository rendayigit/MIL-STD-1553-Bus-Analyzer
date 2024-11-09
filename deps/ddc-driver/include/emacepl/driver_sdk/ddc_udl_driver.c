/*******************************************************************************
 * FILE: ddc_udl_driver.c
 *
 * DESCRIPTION:
 *
 *  This file provides driver initialization/teardown routines.
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
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_bc_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "core/1553/ddc_udl_1553_rt_private.h"
#include "core/can/ddc_udl_can_private.h"
#include "core/serial_io/ddc_udl_serial_io_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_interrupt_private.h"
#include "driver_sdk/ddc_udl_iodev_private.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_driver_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_interrupt_private.h"
#include "os/interface/ddc_udl_os_util_private.h"

#define MAX_DEVICE_CLOSE_BUSY_ITERATIONS            5000

/* list of device context information for each device registered */
static struct _DDC_UDL_DEVICE_CONTEXT *pDevices[MAX_NUM_DEVICES] = { NULL };


/* number of devices installed */
static U8BIT u8NumberDevicesInstalled = 0;

/*******************************************************************************
 * Name:    ddcUdlInitDeviceCount
 *
 * Description:
 *      This function initializes the device count value to zero.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlInitDeviceCount
(
    void
)
{
    u8NumberDevicesInstalled = 0;
}

/*******************************************************************************
 * Name:    ddcUdlIncrementDeviceCount
 *
 * Description:
 *      This function increments the device count value by one each time it is
 *      called.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlIncrementDeviceCount
(
    void
)
{
    u8NumberDevicesInstalled++;
}

/*******************************************************************************
 * Name:    ddcUdlGetDeviceCount
 *
 * Description:
 *      This function returns the device count value.
 *
 * In   none
 * Out  none
 *
 * Returns: device count
 ******************************************************************************/
U8BIT ddcUdlGetDeviceCount
(
    void
)
{
    return u8NumberDevicesInstalled;
}

/*******************************************************************************
 * Name:    ddcUdlGetSupportedDeviceList
 *
 * Description:
 *      This function returns a pointer to the supported device list.
 *
 * In   none
 * Out  none
 *
 * Returns: const pointer to supported device list
 ******************************************************************************/
const DEVICE_LIST_TYPE * ddcUdlGetSupportedDeviceList
(
    void
)
{
    return ddcUdlBusGetSupportedDeviceList();
}

/*******************************************************************************
 * Name:    ddcUdlGetSupportedDeviceListCount
 *
 * Description:
 *      This function returns the number of items in the device list.
 *
 * In   none
 * Out  none
 *
 * Returns: device list count
 ******************************************************************************/
U8BIT ddcUdlGetSupportedDeviceListCount
(
    void
)
{
    return ddcUdlBusGetSupportedDeviceListCount();
}

/*******************************************************************************
 * Name:    ddcUdlCreateDevice
 *
 * Description:
 *      This function will create the device context structure. If the creation
 *      fails, a NULL pointer will be returned.
 *
 * In   none
 * Out  none
 *
 * Returns: pointer to new device context, either valid or NULL
 ******************************************************************************/
struct _DDC_UDL_DEVICE_CONTEXT *ddcUdlCreateDevice
(
    void
)
{
    U8BIT u8DeviceCount = ddcUdlGetDeviceCount();

    ddcUdlOsDeviceAllocate(u8DeviceCount, &pDevices[u8DeviceCount]);

    if (pDevices[u8DeviceCount] != NULL)
    {
        /* initialize members to zero */
        memset(pDevices[u8DeviceCount], 0, sizeof(struct _DDC_UDL_DEVICE_CONTEXT));
    }

    return pDevices[u8DeviceCount];
}

/*******************************************************************************
 * Name:    ddcUdlGetDeviceContext
 *
 * Description:
 *      This function returns the device context associated with a device number.
 *
 * In   u8DeviceNumber
 * Out  none
 *
 * Returns: Device Context Address, NULL on error
 ******************************************************************************/
struct _DDC_UDL_DEVICE_CONTEXT *ddcUdlGetDeviceContext
(
    U8BIT u8DeviceNumber
)
{
    U8BIT u8DeviceCount = ddcUdlGetDeviceCount();

    if (u8DeviceNumber > u8DeviceCount)
    {
        /* ERROR: device number out of range! */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DRIVER, DDC_DBG_DRIVER_DRIVER_ENTRY, "ERROR: device number %d out of range (max %d)\n", u8DeviceNumber, u8DeviceCount);
        return NULL;
    }

    return pDevices[u8DeviceNumber];
}

/*******************************************************************************
 * Name:    ddcUdlDeviceOpen
 *
 * Description:
 *
 *      This function handles all OS independent device open functionality.
 *
 * In   u8DeviceNumber
 * In   u8ChannelNumber
 * Out  u8ChannelType
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
int ddcUdlDeviceOpen
(
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext = NULL;
    U32BIT u32WaitCounter = 0;
    S16BIT s16Result = DDC_UDL_ERROR__SUCCESS;
    BOOLEAN bDeviceCloseBusyState;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_OPEN, "ENTER-> u8DeviceNumber %d u8ChannelNumber %d\n", u8DeviceNumber, u8ChannelNumber);

    /* Ensure that we are accessing a valid device (device and channel) */
    if (u8DeviceNumber >= u8NumberDevicesInstalled)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_OPEN, "Invalid device u8DeviceNumber %d u8NumberDevicesInstalled %d\n",
            u8DeviceNumber, u8NumberDevicesInstalled);

        return DDC_UDL_ERROR__DEVICE_DOES_NOT_EXIST;
    }

    pDeviceContext = ddcUdlGetDeviceContext(u8DeviceNumber);

    /* get the status of the release state */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    bDeviceCloseBusyState = (BOOLEAN)(
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_01] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_02] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_03] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_04] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_05] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_06] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_07] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_08] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_ARINC_SERIAL] ||
        pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_DISCRETE]);

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);


    /* wait for the driver to finish releasing the device before continuing */
    while (bDeviceCloseBusyState == TRUE)
    {
        DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        bDeviceCloseBusyState = (BOOLEAN)(
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_01] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_02] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_03] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_04] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_05] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_06] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_07] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_1553_CHANNEL_08] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_ARINC_SERIAL] ||
            pDeviceContext->bDeviceCloseBusy[DDC_UDL_PROCESS_DISCRETE]);

        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        /* wait 1ms to give the close function time to finish */
        ddcUdlOsWaitMs(1);
        u32WaitCounter++;

        if (u32WaitCounter > MAX_DEVICE_CLOSE_BUSY_ITERATIONS)
        {
            /* do not continue as the driver is stuck trying to close a channel */
            break;
        }
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_OPEN, "Device Opened\n");

    return s16Result;
}

/*******************************************************************************
 * Name:    ddcUdlDeviceClose
 *
 * Description:
 *
 *      This function handles all OS independent device close functionality.
 *
 * In   u8DeviceNumber
 * In   u8ChannelNumber
 * In   u8ChannelType
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
int ddcUdlDeviceClose
(
    U8BIT u8DeviceNumber,
    U8BIT u8ChannelNumber,
    U8BIT u8ChannelType
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext = NULL;

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_CLOSE, "ENTER->DeviceNum %d ChanNum %d\n", u8DeviceNumber, u8ChannelNumber);

    pDeviceContext = ddcUdlGetDeviceContext(u8DeviceNumber);

    switch (u8ChannelType)
    {
        case DDC_UDL_PROCESS_ARINC_SERIAL:
        {
            ArincDisableIrq(pDeviceContext, u8DeviceNumber);

            /* free 429 resources */
            ARINC429ChFree(pDeviceContext);

            break;
        }

        case DDC_UDL_PROCESS_DISCRETE:
        {
            /* nothing to do */
            break;
        }

        /* 1553 Channels */
        default:
        {
            if (pDeviceContext->u8Num1553Channels > 0)
            {
                /* Make a call to disable interrupts in the situation where the user thread was prematurely terminated */
                if (pDeviceContext->pChannel1553[u8ChannelNumber]->bIsr1553Enabled)
                {
                    irqDisableInterrupt(pDeviceContext, u8ChannelNumber, u8DeviceNumber);
                }
            }

            /* free MTI data packet memory */
            mtFreeDataPktList(pDeviceContext, u8ChannelNumber);
        }
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_CLOSE, "Device Closed\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlSetDeviceCloseBusyStatus
 *
 * Description:
 *      Sets/clears the bDeviceCloseBusy status flag for a particular index.
 *
 * In   pDeviceContext
 * In   u8Index     item index
 * In   bBusy       updated busy status
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlSetDeviceCloseBusyStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Index,
    BOOLEAN bBusy
)
{
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    pDeviceContext->bDeviceCloseBusy[u8Index] = bBusy;

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
}


/*******************************************************************************
 * Name:    ddcUdlDeviceContexInit
 *
 * Description:
 *      Initialize some information in pDeviceContext structure and register
 *      the device with the system.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
#if (DDC_UDL_OS_BD_OPEN_AT_DRIVER_LOAD == TRUE)
    U8BIT  u8ChannelNumber = 0;
#endif

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
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_UM, DDC_DBG_ALL);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_MT, DDC_DBG_MT_CH10_DATA_PKT);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_CntrlRoutines);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_IOCTL, DDC_DBG_IOCTL_Ioctl_Level1);
    ddcUdlDebugClearTraceLevel(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE);
}
#endif /* DEBUG */

    pDeviceContext->u8DeviceInstanceCount = ddcUdlGetDeviceCount();

    status = umInitialize(pDeviceContext);
    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "umInitialize fail");
        return status;
    }

    /* update the data archive number (HDL number) since it was read from the UM ROM */
    pDeviceContext->sHwVersionInfo.dwHdlNumber = pDeviceContext->pUmInfo->u32DataArcNum;

    status = ddcUdlBdInfoInitialize(pDeviceContext);
    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ddcUdlBdInfoInitialize fail");
        return status;
    }

    /* initialize I/O only device if find one */
    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ACEX_IO_DRIVER exit now\n");
        status = ioDevOpen(pDeviceContext);
        return status;
    }

#if (DDC_UDL_OS_BD_OPEN_AT_DRIVER_LOAD == TRUE)
    /* open the board component w/interrupts disabled, fill pBoardInfo, sHwVersionInfo structure and channel # info */
    status = ddcUdlBdOpen(pDeviceContext, 0x0000000);
    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "board open fail\n");
        return status;
    }

    /* register 1553 channels */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register 1553\n");
    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++)
    {
        status = udlOsChannelRegistration(pDeviceContext, u8ChannelNumber, DDC_FILE_NAME_TYPE_MIL_STD_1553);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration 1553 Ch %d ERROR: status=%d\n", u8ChannelNumber, status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }

    } /* 1553 channel for loop */

    /* register ARINC 429 */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register 429\n");
    if (pDeviceContext->u8NumProg429RxTx || pDeviceContext->u8NumDed429Tx || pDeviceContext->u8NumDed429Rx
        || pDeviceContext->u8NumProg717 || pDeviceContext->u8NumDed717Tx || pDeviceContext->u8NumDed717Rx 
        || pDeviceContext->u8NumRS232 || pDeviceContext->u8NumRS485 || pDeviceContext->u8NumUart 
        || pDeviceContext->u8NumCanBus)
    {
        status = udlOsChannelRegistration(pDeviceContext, 0, DDC_FILE_NAME_TYPE_ARINC_429);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration 429 ERROR: status=%d\n", status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }
    }

    /* register I/O  - Allow registration for all */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register I/O\n");
    /*if (pDeviceContext->u8NumDiscreteIO || pDeviceContext->u8NumAvionicIO || pDeviceContext->u8NumDioTt)*/
    {
        status = udlOsChannelRegistration(pDeviceContext, 0, DDC_FILE_NAME_TYPE_IO);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration I/O ERROR: status=%d\n", status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }
    }
#endif

#if 0 /* <UDL17> */
    /* register device */
    status = ddcUdlOsBusRegisterDev(pDeviceContext);

    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ddcUdlOsBusRegisterDev failed! , status 0x%x\n", status);
        return status;
    }
#endif /* 0 */


    /* Initialize spinlock for device open/close parameters.
     * Applies to all devices (1553, 429 and I/O) */

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == TRUE)
    /* hook ISR */
    status = ddcUdlOsIrqHookISR(pDeviceContext, pDeviceContext->ddcOsDevInfo.u8DeviceNumber);

    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "hook ISR failed\n");
        return status;
    }
#endif

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlLateDeviceContexInit
 *
 * Description:
 *      Initialize some information in pDeviceContext structure.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlLateDeviceContexInit
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT  u8ChannelNumber;
    U8BIT  i;
    U32BIT u32Reset;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    /* initialize I/O only device if find one */
    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ACEX_IO_DRIVER exit now\n");
        return status;
    }

    DDC_ISR_LOCK_INIT(pDeviceContext->semDeviceOpenEventCond);

    /* initialize device open/close information */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    for (i=0; i < (sizeof(pDeviceContext->u8Channel1553Open)/ sizeof(pDeviceContext->u8Channel1553Open[0])); i++)
    {
        pDeviceContext->u8Channel1553Open[i] = FALSE;
    }

    pDeviceContext->u8ArincSerialOpen = FALSE;
    pDeviceContext->u8DiscreteOpen = FALSE;

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

#if (DDC_UDL_OS_BD_OPEN_AT_DRIVER_LOAD == FALSE)
    /* open the board component w/interrupts disabled, fill pBoardInfo, sHwVersionInfo structure and channel # info */
    status = ddcUdlBdOpen(pDeviceContext, 0x0000000);
    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "board open fail\n");
        return status;
    }

    /* register 1553 channels */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register 1553\n");
    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++)
    {
        status = udlOsChannelRegistration(pDeviceContext, u8ChannelNumber, DDC_FILE_NAME_TYPE_MIL_STD_1553);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration 1553 Ch %d ERROR: status=%d\n", u8ChannelNumber, status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }

    } /* 1553 channel for loop */

    /* register ARINC 429 */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register 429\n");
    if (pDeviceContext->u8NumProg429RxTx || pDeviceContext->u8NumDed429Tx || pDeviceContext->u8NumDed429Rx
        || pDeviceContext->u8NumProg717 || pDeviceContext->u8NumDed717Tx || pDeviceContext->u8NumDed717Rx 
        || pDeviceContext->u8NumRS232 || pDeviceContext->u8NumRS485 || pDeviceContext->u8NumUart 
        || pDeviceContext->u8NumCanBus)
    {
        status = udlOsChannelRegistration(pDeviceContext, 0, DDC_FILE_NAME_TYPE_ARINC_429);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration 429 ERROR: status=%d\n", status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }
    }

    /* register I/O - Allow registration for all */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "register I/O\n");
   /*if (pDeviceContext->u8NumDiscreteIO || pDeviceContext->u8NumAvionicIO || pDeviceContext->u8NumDioTt)*/
    {
        status = udlOsChannelRegistration(pDeviceContext, 0, DDC_FILE_NAME_TYPE_IO);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "udlOsChannelRegistration I/O ERROR: status=%d\n", status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }
    }
#endif

    /* 1553 Channel initialization */
    status = ddcUdl1553ChannelInit(pDeviceContext);

    /* Do not check the status since 429 only devices will not have any 1553 components */

    /* initialize RT Auto Boot information */
    ddcUdlBdRtAutoBootInitialize(pDeviceContext);

    /* Reset all components of the device */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ddcUdlBdReset \n");
    u32Reset = BD_RESET_IRIGB | BD_RESET_ARINC429 | BD_RESET_BD;

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        if (!pDeviceContext->pChannel1553[i]->bRtAutoBoot)
        {
            u32Reset |= (BD_RESET_1553_CH0 << i);
        }
    }

    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, u32Reset);

    /* Initialize spinlock for device open/close parameters.
     * Applies to all devices (1553, 429 and I/O) */

    /* perform channel specific initialization/configuration */
    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++) /* 1553 channel for loop */
    {
        /* initialize interrupt status queue */
        pDeviceContext->pChannel1553[u8ChannelNumber]->pu32IrqStatusQ = DDC_KERNEL_MALLOC(pDeviceContext, (IRQ_STATUS_QUEUE_SIZE *
            sizeof(pDeviceContext->pChannel1553[u8ChannelNumber]->pu32IrqStatusQ[0])));

        if (pDeviceContext->pChannel1553[u8ChannelNumber]->pu32IrqStatusQ == NULL)
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        /* initialize replay interrupt status queue */
        pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.pu32IrqStatusQ = DDC_KERNEL_MALLOC(pDeviceContext, (REPLAY_IRQ_STATUS_QUEUE_SIZE * sizeof(U32BIT)));
        if (pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.pu32IrqStatusQ == NULL)
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.bReplayIsrEnabled = FALSE;

        /* update transcer delay */
        mfTransceiverDelayUpdate(pDeviceContext, u8ChannelNumber);

        /* initialize EI */
        errorInj1553Initialize(pDeviceContext, u8ChannelNumber);

        /*---------------------------------------------------------
           do not change the following initialization sequence
         ----------------------------------------------------------*/
        DDC_ISR_LOCK_INIT(pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.semMtiDataPkt);
        DDC_ISR_LOCK_INIT(pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.semMtiTimePkt);

        /* Initialize MTI Time & Data Lookaside List */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ch%d: mtMtiInit\n", u8ChannelNumber);
        status = mtMtiInit(pDeviceContext, u8ChannelNumber);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "mtMtiInit Ch %d ERROR: status=%d\n", u8ChannelNumber, status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }

        /* initialize Multi-RT module */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ch%d: mrtInitialize\n", u8ChannelNumber);
        pDeviceContext->pChannel1553[u8ChannelNumber]->sRT.state = ACEX_MOD_RESET; /* force to reset since we are loading the device */

        status = mrtInitialize(pDeviceContext, u8ChannelNumber);
        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "mrtInitialize Ch %d ERROR: status=%d\n", u8ChannelNumber, status);
            ddcUdlBdClose(pDeviceContext);
            return status;
        }

        /* initialize and open hardware Improvements Block */
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ch%d: impInitialize\n", u8ChannelNumber);

        impInitialize(pDeviceContext, u8ChannelNumber);

        /*---------------------------------------------------------
           do not change the above initialization sequence
         ----------------------------------------------------------*/
        /* Initialize BC Module after IMP initialized*/
        bcInitialize(pDeviceContext, u8ChannelNumber);

        /* initialize trigger */
        trigger1553Initialize(pDeviceContext, u8ChannelNumber);

    } /* 1553 channel for loop */

    status = irigbOpen(pDeviceContext);
    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "irigbOpen fail");
        return status;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "429 init \n");

    /* Initialize the ARINC 429 engine */
    if ((pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_ARINC_429_GLOBAL) ||
        (pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_ARINC_429_GLOBAL_TX_MF)
        ||(pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_MIO_CAST_UART_SDLC_HDLC))
    {
        /* Initialize the ARINC429 events. Initially there are no events, so is initialized as 0 */
        DDC_ISR_LOCK_INIT(pDeviceContext->semArincIrqEventCond);
        DDC_ISR_LOCK_INIT(pDeviceContext->semArinc429TxState);
        DDC_ISR_LOCK_INIT(pDeviceContext->semArinc429RxFifoEventCond);


        /* Initialize the Voltage Monitoring events */
        DDC_ISR_LOCK_INIT(pDeviceContext->semArinc429VoltageMonitoringEventCond);

        ARINC429TxSetStateIdle(pDeviceContext);

        DDC_INIT_WAITQUEUE_IRQ(
            &pDeviceContext->waitqueueArincBlockOnIrqCallback,
            &pDeviceContext->waitqueueArincBlockOnIrqEvent);

        DDC_INIT_WAITQUEUE(&pDeviceContext->waitqueueArincBlockOnIrqReadyEvent);

        pDeviceContext->b429MemSwByteSwap = FALSE;

        /* Initialize the ARINC 429 channels */
        status = ARINC429ChInitialize(pDeviceContext);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "429 - Failed to get Tx Queue Memory!\n");
            return status;
        }

        serial_ioInitialize(pDeviceContext);

        for (i = 0; i < pDeviceContext->u8NumProg429RxTx; i++)
        {
            DDC_INIT_WAITQUEUE_IRQ(
                &pDeviceContext->pRxChnl429[i]->waitqueueARINC429RxFifoCallback,
                &pDeviceContext->pRxChnl429[i]->waitqueueARINC429RxFifoEvent);

            DDC_INIT_WAITQUEUE_IRQ(
                &pDeviceContext->pTxChnl429[i]->waitqueueARINC429SetTxFrameControlCallback,
                &pDeviceContext->pTxChnl429[i]->waitqueueARINC429SetTxFrameControlEvent);
        }
    }

    /* Initialize the ARINC 717 engine */
    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "init 717 \n");
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_PROG_ARINC_717)
    {
        /* Initialize the ARINC 717 events. Initially there are no events, so is initialized as 0 */
        for (i = 0; i < pDeviceContext->u8NumProg717; i++)
        {
            DDC_INIT_WAITQUEUE_IRQ(
                &pDeviceContext->waitqueueARINC717TxFrameCallback[i],
                &pDeviceContext->waitqueueARINC717TxFrameEvent[i]);

            DDC_INIT_WAITQUEUE_IRQ(
                &pDeviceContext->waitqueueARINC717RxFrameCallback[i],
                &pDeviceContext->waitqueueARINC717RxFrameEvent[i]);
        }

        /* Initialize the ARINC 717 channels */
        status = arinc717Initialize(pDeviceContext);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT,
                "arinc717Initialize ERROR: status:%d\n",status);

            return status;
        }
    }

    /* Initialize the CAN Bus engine */
    /* need to add further checks to see if can bus is really available */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_CAN)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT,
            "canBusInitialize !!\n");

        if (pDeviceContext->u8NumCanBus)
        {
            status = canBusInitialize(pDeviceContext);

            if (status != DDC_UDL_ERROR__SUCCESS)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT,
                    "canBusInitialize ERROR: status:%d\n",status);

                ddcUdlBdClose(pDeviceContext);
                return status;
            }
        }
    }

    /* Initialize the VoltageMonitoring event. Initially there are no events, so is initialized as 0 */
    DDC_INIT_WAITQUEUE_IRQ(
        &pDeviceContext->waitqueueArinc429VoltageMonitoringCallback,
        &pDeviceContext->waitqueueArinc429VoltageMonitoringEvent);

    /* Allocate device level DMA Descriptor (less than 4GB) */
    if (!(pDeviceContext->sDMA.pu32Descriptor = (U32BIT *) DDC_KERNEL_GET_DMA_DESCRIPTOR(pDeviceContext)))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_KERNEL_GET_DMA_DESCRIPTOR fail");
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "sDMA.pu32Descriptor=%p\n",pDeviceContext->sDMA.pu32Descriptor);

    /* Allocate RT DMA buffer */
    pDeviceContext->RtDmaSize = 0x4000;

    /* Allocate RT DMA buffer */
    pDeviceContext->u8RtDmaBusy = 0;
#if DDC_DMA_RT
    pDeviceContext->pu8RtDmaTarget = DDC_DMA_MALLOC(
        pDeviceContext,
        pDeviceContext->RtDmaSize,
        &pDeviceContext->RtDmaAddr,
        DDC_MEMORY_REGION__RT_DMA);

    if (!pDeviceContext->pu8RtDmaTarget)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC fail");
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "pu8RtDmaTarget=%p\n",pDeviceContext->pu8RtDmaTarget);
#endif /* DDC_DMA_RT */

    /* DD-40000 devices only */
    if ((pDeviceContext->u16DriverType == ACEX_DD429_PCIE_DRIVER) ||
        (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER))
    {
        U32BIT u32DmlBytes;

        /* ARINC 429 */
#if DDC_DMA_429
        if (pDeviceContext->pRxChnl429[0]->pu32MemSize)
        {
            u32DmlBytes = (*(pDeviceContext->pRxChnl429[0]->pu32MemSize) << 2); /* since all the 429 channels are the same size, just use the 1st 429 channel */

            /* create ARINC 429 RX FIFO DMA buffer */
            pDeviceContext->ARINC429RxDMA_Size = u32DmlBytes;

            pDeviceContext->ARINC429RxDMATarget = DDC_DMA_MALLOC(
                pDeviceContext,
                pDeviceContext->ARINC429RxDMA_Size,
                &pDeviceContext->ARINC429RxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_RX_DMA);

            if (!pDeviceContext->ARINC429RxDMATarget)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for ARINC429RxDMATarget failed");
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ARINC429RxDMATarget=%p\n",pDeviceContext->ARINC429RxDMATarget);

            /* create ARINC 429 SetTxFrameControl DMA buffer */
            pDeviceContext->ARINC429TxDMA_Size = u32DmlBytes;

            pDeviceContext->ARINC429TxDMATarget = DDC_DMA_MALLOC(
                pDeviceContext,
                pDeviceContext->ARINC429TxDMA_Size,
                &pDeviceContext->ARINC429TxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_TX_DMA);

            if (!pDeviceContext->ARINC429TxDMATarget)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for ARINC429TxDMATarget failed");
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ARINC429TxDMATarget=%p\n",pDeviceContext->ARINC429TxDMATarget);
        }


        /* create Voltage Monitoring DMA buffer */
        if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemSize)
        {
            u32DmlBytes = (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemSize) << 2);

            pDeviceContext->Arinc429VoltageMonDMA_Size = u32DmlBytes;

            pDeviceContext->Arinc429VoltageMonDMATarget = DDC_DMA_MALLOC(
                pDeviceContext,
                pDeviceContext->Arinc429VoltageMonDMA_Size,
                &pDeviceContext->Arinc429VoltageMonDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_VOLT_MON_DMA);

            if (!pDeviceContext->Arinc429VoltageMonDMATarget)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for VoltageMonDMATarget failed");
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "VoltageMonDMATarget=%p, max size 0x%08x bytes\n", pDeviceContext->Arinc429VoltageMonDMATarget, u32DmlBytes);
        }
#endif  /* DDC_DMA_429 */

        /* ARINC 717 */
#if DDC_DMA_717
        if (pDeviceContext->sChannelArinc717[0].pu32MemSize)
        {
            u32DmlBytes = (*(pDeviceContext->sChannelArinc717[0].pu32MemSize) << 2); /* since all the 717 channels are the same size, just use the 1st 717 channel */

            /* create ARINC 717 RX DMA buffer */
            pDeviceContext->ARINC717RxDMA_Size = u32DmlBytes;

            pDeviceContext->ARINC717RxDMATarget = DDC_DMA_MALLOC(
                pDeviceContext,
                pDeviceContext->ARINC717RxDMA_Size,
                &pDeviceContext->ARINC717RxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_717_RX_DMA);

            if (!pDeviceContext->ARINC717RxDMATarget)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for ARINC717RxDMATarget failed");
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ARINC717RxDMATarget=%p\n",pDeviceContext->ARINC717RxDMATarget);

            /* create ARINC 717 buffer */
            for (i=0; i < pDeviceContext->u8NumProg717; i++)
            {
                /* allocate memory buffer */
                pDeviceContext->sChannelArinc717[i].pu8MemoryBuf = DDC_KERNEL_MALLOC(pDeviceContext, u32DmlBytes);

                if (!pDeviceContext->sChannelArinc717[i].pu8MemoryBuf)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for Arinc 717 ch%du8DmaMemory failed", i);
                    return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
                }

                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "Arinc 717 ch%d u8DmaMemory=%p\n",i ,pDeviceContext->sChannelArinc717[i].pu8MemoryBuf);
            }

            /* create ARINC 717 TX DMA buffer */
            pDeviceContext->ARINC717TxDMA_Size = u32DmlBytes;

            pDeviceContext->ARINC717TxDMATarget = DDC_DMA_MALLOC(
                pDeviceContext,
                pDeviceContext->ARINC717TxDMA_Size,
                &pDeviceContext->ARINC717TxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_717_TX_DMA);

            if (!pDeviceContext->ARINC717TxDMATarget)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DDC_DMA_MALLOC for ARINC717TxDMATarget failed");
                return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "ARINC717TxDMATarget=%p\n",pDeviceContext->ARINC717TxDMATarget);
        }
#endif /* DDC_DMA_717 */

    }

    /* DIO Time Tag */
    /* The 'state' member is used as a feature availability and active state flag for all DioTt routines */
    /* Preset the flag to unavailable for the situation where the feature does not exist on a device */
    pDeviceContext->sDioTt.state = ACEX_MOD_ERROR;

    if (pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_DIO_TT)
    {
        status = ddcUdlDioTtInitialize(pDeviceContext);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "DioTtInitilize ERROR: status=%d\n", status);
            ddcUdlBdClose(pDeviceContext);

           return status;
        }
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    memset(pDeviceContext->bDeviceCloseBusy, 0, sizeof(pDeviceContext->bDeviceCloseBusy));
    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++)
    {
        pDeviceContext->pChannel1553[u8ChannelNumber]->bIsr1553Enabled = FALSE;
    }

    for (u8ChannelNumber = 0; u8ChannelNumber < pDeviceContext->u8Num1553Channels; u8ChannelNumber++)
    {
        /* initialize interrupt status queue index */
        pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQHead = 0;
        pDeviceContext->pChannel1553[u8ChannelNumber]->u32IrqStatusQTail = 0;
        pDeviceContext->pChannel1553[u8ChannelNumber]->u16IrqEventCond = 0;
        pDeviceContext->pChannel1553[u8ChannelNumber]->u16BlockOnIrqReadyEventCond = 0;

        DDC_ISR_LOCK_INIT(pDeviceContext->pChannel1553[u8ChannelNumber]->semIrqEventCond);

        DDC_INIT_WAITQUEUE_IRQ(
            &pDeviceContext->pChannel1553[u8ChannelNumber]->waitqueueIrqCallback,
            &pDeviceContext->pChannel1553[u8ChannelNumber]->waitqueueIrqEvent);

        DDC_INIT_WAITQUEUE(&pDeviceContext->pChannel1553[u8ChannelNumber]->waitqueueBlockOnIrqReadyEvent);

        /* initialize replay interrupt status queue index */
        pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQHead = 0;
        pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u32IrqStatusQTail = 0;
        pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.u16IrqEventCond = 0;

        DDC_ISR_LOCK_INIT(pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.semIrqEventCond);
        DDC_INIT_WAITQUEUE_IRQ(
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.waitqueueIrqCallback,
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sReplay.waitqueueIrqEvent);

        /* Initialize MTI data and time packet wait queues */
        pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.u8MtiDataPoolEventFlag = 0;
        DDC_INIT_WAITQUEUE_IRQ(
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.eMtiDataPoolCallback,
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.eMtiDataPoolEvent);

        pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.u8MtiTimePoolEventFlag = 0;
        DDC_INIT_WAITQUEUE_IRQ(
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.eMtiTimePoolCallback,
            &pDeviceContext->pChannel1553[u8ChannelNumber]->sMT.eMtiTimePoolEvent);
    }

    /*status = ddcSfpMemRegConfig(pDeviceContext);*/ /* For SecurityLock */

#if 0 /* RT AUTO BOOT DEBUG code */
    {
        U32BIT u32Data;
        status = DDC_REG_READ(pDeviceContext,0x180,&(u32Data));
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "RT ADDR LATCH - 7: RT GLOBAL Config (%08X)\n", u32Data);
    }
#endif

    /* after reset, restore RT addr if RT Auto BOOT is not ON but SRT addr Latch is ON */
    ddcUdlBdRtAutoBootAddrRestore(pDeviceContext);

    /* create DMA Queue for device */
    status = dmaQueueCreate(pDeviceContext, 512);

    if (status != DDC_UDL_ERROR__SUCCESS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_INIT, "Call of dmaQueueCreate failed!\n");

        return status;
    }

	/* initialize dma */
    ddcUdlDmaInit(pDeviceContext);

/* //todo on error unmap at caller side */
    return DDC_UDL_ERROR__SUCCESS;
}


/*******************************************************************************
 * Name:    ddcUdlInit
 *
 * Description:
 *
 *      This function gets called from the OS driver initialization routine.
 *
 * In   s16DeviceNumber
 *          INIT_ALL_DEVICES    Init for all devices
 *          all other values    Use the s16DeviceNumber parameter as is
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
S16BIT ddcUdlInit
(
    void
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext;
    U16BIT u16Result = DDC_UDL_ERROR__SUCCESS;
    U8BIT u8DeviceNumber = 0;
    U8BIT u8DeviceCount = 0;

    u16Result = ddcUdlOsInit();

    if (u16Result != DDC_UDL_ERROR__SUCCESS)
    {
        return u16Result;
    }

    u8DeviceCount = ddcUdlGetDeviceCount();

    /* initialize device context for all devices */
    for (u8DeviceNumber = 0; u8DeviceNumber < u8DeviceCount; u8DeviceNumber++)
    {
        pDeviceContext = ddcUdlGetDeviceContext(u8DeviceNumber);

        if (ddcUdlOsDeviceContexInit(pDeviceContext))
        {
            return DDC_UDL_ERROR__DEVICE_NOT_FOUND;
        }

        /* device binding for XMC card */
        ioDevBinding(pDeviceContext);
    }

    return u16Result;
}


/*******************************************************************************
 * Name:    ddcUdlExit
 *
 * Description:
 *      module cleanup routine:
 *      This function is called when the driver module is unloaded.
 *      Here we simply de-register the device.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlExit
(
    void
)
{
    int u8DeviceNumber;
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext;

    /* loop through all devices */
    for (u8DeviceNumber = 0; u8DeviceNumber < u8NumberDevicesInstalled; u8DeviceNumber++)
    {
        /* get device context */
        pDeviceContext = ddcUdlGetDeviceContext((U8BIT)u8DeviceNumber);

        if (pDeviceContext)
        {
            /* free components if it is not an I/O only device */
            if (pDeviceContext->u16DriverType != ACEX_IO_DRIVER)
            {
                /* unregister channels as they relate to the OS */
                udlOsChannelUnregistration(pDeviceContext);

                /* free page descriptor */
                DDC_KERNEL_FREE_DMA_DESCRIPTOR((long)pDeviceContext->sDMA.pu32Descriptor);

                if (pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_DIO_TT)
                {
                    ddcUdlDioTtFree(pDeviceContext);
                }

                ddcUdlARINC429ChannelCleanup(pDeviceContext);
                ddcUdlARINC717ChannelCleanup(pDeviceContext);
                ddcUdlUmCleanup(pDeviceContext);
                ddcUdlMtCleanup(pDeviceContext);
                ddcUdl1553ChannelCleanup(pDeviceContext);
                ddcUdlBdClose(pDeviceContext);
                dmaQueueDestroy(pDeviceContext);
            }

            /* Unmap PCI addresses */
            ddcUdlBusPciUnmapAddresses(pDeviceContext);

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == TRUE)
            /* unhook ISR */
            ddcUdlOsIrqUnhookISR(pDeviceContext, (U8BIT)u8DeviceNumber);
#endif

            /* free device context */
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext);
            pDeviceContext = NULL;
        }
    }

    ddcUdlOsExit();

    DDC_DBG_PRINT(DDC_DBG_MODULE_DEVICE, DDC_DBG_DEVICE_EXIT, "ACEX PCI EXIT ======================>>>>>>>>>>\n");

    DDC_DBG_PRINT_INFO("Driver is uninstalled\n");
}
