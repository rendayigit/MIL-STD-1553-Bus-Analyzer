/*******************************************************************************
 * FILE: ddc_udl_os_bus_usb.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific USB support funtions.
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


/*******************************************************************************
 * Name:    ddcUldOsBusGetInterruptStatus
 *
 * Description:
 *      This function gets retrieves an interrupt status register value.
 *
 * In   pDeviceContext  Device Context Pointer
 * In   u32Offset       Interrupt Register Offset
 * In   u32Offset       Interrupt Register Offset
 * Out  none
 *
 * Returns: Interrupt Status Value
 ******************************************************************************/
U32BIT ddcUldOsBusGetInterruptStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u8RegisterType,
    U32BIT u32BaseAddress,
    U32BIT u32Offset
)
{
    U32BIT u32Value = 0x00000000;

/* <UDL12> */
    
    for (i = 0; i < SFP_NUM_DWDS_IN_INTERRUPT; i++)
    {
        /* shift bytes into interrupt status fields - bytewise to remove endian issues */
        pDev->u32IntStatus[i] = *(pu8IntStatus + (i * 4)) & 0x00ff;
        pDev->u32IntStatus[i] = pDev->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i * 4) + 1)) << 8);
        pDev->u32IntStatus[i] = pDev->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i * 4) + 2)) << 16);
        pDev->u32IntStatus[i] = pDev->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i * 4) + 3)) << 24);

        dbg_print(DBG_MODULE_ACEX_DRIVER, DBG_PRIORITY_INFO, "%02x %02x %02x %02x,  32BIT:%08x\n",
            *(pu8IntStatus + (i * 4)),
            *(pu8IntStatus + (i * 4) + 1),
            *(pu8IntStatus + (i * 4) + 2),
            *(pu8IntStatus + (i * 4) + 3),
            pDev->u32IntStatus[i]);
    }

/* <UDL13> */
    /* Reception of 4 bytes on this endpoint denotes CAN interrupts for now; until fix in place */
    /* Therefore must fill unitialized area */
    if (NumBytesTransferred == 4)
    {
        pu8IntStatus = WdfMemoryGetBuffer(Buffer, NULL);
        for (i = 1; i < pDeviceContext->u32NumDwdsInInterrupt; i++)
        {
            pu8IntStatus[i] = 0;
        }
    }
    else
    {
        ASSERT(NumBytesTransferred == (size_t)(pDeviceContext->u32NumDwdsInInterrupt * 4));
        pu8IntStatus = WdfMemoryGetBuffer(Buffer, NULL);
    }

    for (i = 0; i < pDeviceContext->u32NumDwdsInInterrupt; i++)
    {
        /* shift bytes into interrupt status fields - bytewise to remove endian issues */
        pDeviceContext->u32IntStatus[i] = *(pu8IntStatus + (i*4)) & 0x00ff;
        pDeviceContext->u32IntStatus[i] = pDeviceContext->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i*4)+1) )<<8) ;
        pDeviceContext->u32IntStatus[i] = pDeviceContext->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i*4)+2))<<16) ;
        pDeviceContext->u32IntStatus[i] = pDeviceContext->u32IntStatus[i] | (((U32BIT)*(pu8IntStatus + (i*4)+3))<<24) ;

        DDC_KDBG_PRINT(DDC_DBG_INTERRUPT_INT_ISR, _DRIVER_NAME_ "%02x %02x %02x %02x,  32BIT:%08x\n",
                     *(pu8IntStatus + (i*4)    ),
                     *(pu8IntStatus + (i*4) + 1),
                     *(pu8IntStatus + (i*4) + 2),
                     *(pu8IntStatus + (i*4) + 3),
                     pDeviceContext->u32IntStatus[i]);

#ifdef INTERRUPT_DEBUG
        KdPrint(( "%02x %02x %02x %02x,  32BIT:%08x\n",
                     *(pu8IntStatus + (i*4)    ),
                     *(pu8IntStatus + (i*4) + 1),
                     *(pu8IntStatus + (i*4) + 2),
                     *(pu8IntStatus + (i*4) + 3),
                     pDeviceContext->u32IntStatus[i]));
#endif
   }


    
    return u32Value;
}

/*******************************************************************************
 * Name:    ddcUldOsBusSetInterruptStatus
 *
 * Description:
 *      This function sets retrieves an interrupt status register value.
 *
 * In   pDeviceContext  Device Context Pointer
 * In   u32Offset       Interrupt Register Offset
 * In   u32Offset       Interrupt Register Offset
 * Out  none
 *
 * Returns: Interrupt Status Value
 ******************************************************************************/
void ddcUldOsBusSetInterruptStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u8RegisterType,
    U32BIT u32BaseAddress,
    U32BIT u32Offset,
    U32BIT u32Value
)
{
    U32BIT u32LocalValue = u32Value;
    

    if (u8RegisterType >= DDC_INTERRUPT_STATUS_TYPE__MAX)
    {
        /* ERROR: Unknown Type */
        return;
    }
    
    DDC_REG_WRITE(pDeviceContext, (u32BaseAddress + u32Offset), &(u32LocalValue);
}