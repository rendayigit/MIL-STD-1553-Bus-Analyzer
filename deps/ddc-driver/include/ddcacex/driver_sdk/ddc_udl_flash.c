/*******************************************************************************
 * FILE: ddc_udl_flash.c
 *
 * DESCRIPTION:
 *
 *  This file provides PCI(e)-specific communication routines
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
#include "include/ddc_ioctl.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"

/* Flash OPs */
#define FLASH_OP_READ                   0x00080000  /* Read operation           */
#define FLASH_OP_WRITE                  0x00090000  /* Write operation          */
#define FLASH_OP_ERASE                  0x000A0000  /* Sector erase operation   */
#define FLASH_OP_TERM                   0xFFF7FFFF  /* Terminate operation mask */

/* FLASH status patterns */
#define FLASH_STATUS_DONE               0x00080000  /* Op done                  */
#define FLEXCORE_CAP_REG_INFO_MASK      0x04000000  /* flash info bit           */

#define FLASH_OP_TIMEOUT                8000


/*******************************************************************************
 * Name:    flashMemBlkErase
 *
 * Description:
 *      board debug show routine
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams    IOCTL parameters structure
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
U8BIT flashMemBlkErase
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT dwAddress = (U32BIT)((pIoctlParams->Param1) >> 1); /*DDC_1553_ADDR_SHIFT*/ /* from 8bit addr to 16bit addr */
    U32BIT dwData = 0;
    U8BIT bStatus;

    DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_BLOCK_ERASE, "Erase dwAddress = 0x%x!\n", (int)dwAddress);

    /* Write the address to the FLASH address register */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_ADDR), &dwAddress);

    /* Build the Flash Control / Data In word for write op */
    dwData = FLASH_OP_ERASE;

    /* Write the C/D-In word to the FLASH C/D-In register */
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);

    /* Poll the operation completed bit in C/D-Out register until '1' */
    do
    {
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
    } while (!(dwData & FLASH_STATUS_DONE));

    /* Check for errors from FPGA */
    bStatus = (U8BIT)((dwData & 0x70000) >> 16);
    DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_BLOCK_ERASE, "bStatus = 0x%u!\n", bStatus);

    /* Clear C/D-In perform op bit */
    DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);
    dwData &= FLASH_OP_TERM;
    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);

    /* Poll the operation completed bit in C/D-Out register until '0' */
    do
    {
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
    } while (dwData & FLASH_STATUS_DONE);

    /* If there was a problem, return error */
    if (bStatus)
    {
        return bStatus;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    flashMemRead
 *
 * Description:
 *      This function reads a block of data in Flash memory
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams        IOCTL command structure
 * In   IOBuffer        pointer to buffer for read data
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
U8BIT flashMemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    void *IOBuffer
)
{
    U32BIT dwAddress = (U32BIT)((pIoctlParams->Param1) >> 1);        /* from 8bit addr to 16bit addr */
    U32BIT dwByteCount = (U32BIT)pIoctlParams->Param2;
    U8BIT *pData = (U8BIT *)IOBuffer;
    U32BIT dwData = 0;
    U8BIT bStatus = 0;
    U32BIT dwTimeoutLoop = 0;
    U32BIT i = 0;

    for (i = 0; i < dwByteCount; i += 2)
    {
        /* Write the address to the FLASH address register */
        dwData = dwAddress + i / 2;
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_ADDR), &dwData);

        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Write Reg (Flash Addr) 0x%x dwData =0x%x!\n", (int)(*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_ADDR), (int)dwData);

        /* Build the Flash Control / Data In word for write op */
        dwData = FLASH_OP_READ;

        /* Write the C/D-In word to the FLASH C/D-In register */
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);
        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Write Reg (Flash Ctrl) dwData =0x%x!\n", (int)dwData);

        /* Poll the operation completed bit in C/D-Out register until '1' */
        do
        {
            DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
            dwTimeoutLoop++;
        } while (!(dwData & FLASH_STATUS_DONE) && (dwTimeoutLoop != FLASH_OP_TIMEOUT));

        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Read Reg (Flash Status) dwData =0x%x!\n", (int)dwData);

        if (dwTimeoutLoop == FLASH_OP_TIMEOUT)
        {
            /* fail IOCTL */
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Time Out!\n");
            return (U8BIT)dwData;
        }

        /* Check for error code from FPGA */
        bStatus = (U8BIT)((dwData & 0x70000) >> 16);

        /* Clear C/D-In perform op bit */
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);
        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Read Reg (Flash Ctrl) dwData =0x%x!\n", (int)dwData);
        dwData &= FLASH_OP_TERM;
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);
        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "Write Reg (Flash Ctrl) dwData =0x%x!\n\n", (int)dwData);
        dwTimeoutLoop = 0;

        /* Poll the operation completed bit in C/D-Out register until '0' */
        do
        {
            DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
            dwTimeoutLoop++;
        } while ((dwData & FLASH_STATUS_DONE) && (dwTimeoutLoop != FLASH_OP_TIMEOUT));

        if (dwTimeoutLoop == FLASH_OP_TIMEOUT)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "dwData =0x%x!\n", (int)dwData);
            return bStatus;
        }

        /* If there was a problem, return error */
        if (bStatus)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_READ, "bStatus =0x%x!\n", bStatus);
            return bStatus;
        }

        /* Assign the value at the FLASH address to the passed in pointer */
        pData[i] = (U8BIT)(dwData & 0xFF);
        pData[i + 1] = (U8BIT)((dwData >> 8) & 0xFF);
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    flashMemWrite
 *
 * Description:
 *      flashMemWrite
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams        IOCTL command structure
 * In   IOBuffer        pointer to buffer for read data
 * Out  none
 *
 * Returns: error status
 ******************************************************************************/
U8BIT flashMemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    void *IOBuffer
)
{
    U32BIT dwAddress;
    U32BIT dwByteCount = 0;
    U32BIT dwData = 0;
    U16BIT dwData16 = 0;
    U8BIT *pData;
    U8BIT bStatus = 0;
    U32BIT dwTimeoutLoop = 0;
    U32BIT i = 0;
    U32BIT OpAddr = 0;

    dwAddress = (U32BIT)((pIoctlParams->Param1) >> 1);   /*DDC_1553_ADDR_SHIFT*/ /* from 8bit addr to 16bit addr */
    pData = (U8BIT *)IOBuffer;
    dwByteCount = (U32BIT)pIoctlParams->Param2;

    DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "dwAddress : 0x%x  dwByteCount : 0x%x \n", (int)dwAddress, (int)dwByteCount);

    for (i = 0; i < dwByteCount; i += 2)
    {
        /* Write the address to the FLASH address register */
        OpAddr = dwAddress + i / 2;
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_ADDR), &OpAddr);

        /* Build the Flash Control / Data In word for write op */
        dwData16 = (U16BIT)pData[i] & 0xFF;
        dwData16 |= (U16BIT)(pData[i + 1] << 8);
        dwData = FLASH_OP_WRITE | dwData16;

        /* Write the C/D-In word to the FLASH C/D-In register */
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);

        dwTimeoutLoop = 0;

        /* Poll the operation completed bit in C/D-Out register until '1' */
        do
        {
            DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
            dwTimeoutLoop++;
            
        } while (!(dwData & FLASH_STATUS_DONE) && (dwTimeoutLoop != FLASH_OP_TIMEOUT));

        if (dwTimeoutLoop == FLASH_OP_TIMEOUT)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "operation 1 timed out!\n");
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "dwData = 0x%x\n", (int)dwData);
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "last op addr = 0x%x\n", (int)OpAddr);

            return (U8BIT)dwData;
        }

        /* Check for error code from FPGA */
        bStatus = (U8BIT)((dwData & 0x70000) >> 16);

        /* Clear C/D-In perform op bit */
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);
        dwData &= FLASH_OP_TERM;
        DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_CONTROL), &dwData);

        dwTimeoutLoop = 0;

        /* Poll the operation completed bit in C/D-Out register until '0' */
        do
        {
            DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_FLASH_STATUS), &dwData);
            dwTimeoutLoop++;
        } while ((dwData & FLASH_STATUS_DONE) && (dwTimeoutLoop != FLASH_OP_TIMEOUT));

        if ((dwTimeoutLoop == FLASH_OP_TIMEOUT) || bStatus)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "operation 2 fails!\n");
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "bStatus =0x%x\n", bStatus);
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "dwData =0x%x\n", (int)dwData);
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "last op addr = 0x%x\n", (int)OpAddr);
            return (U8BIT)dwData;
        }

        if ((i / 2) < 10)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "dwData = 0x%x\n", (int)dwData);
            DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE, "last op addr = 0x%x\n", (int)OpAddr);
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    flashMemWriteProtected
 *
 * Description:
 *      Gets the flash write protect status
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: flash write protect status or error status
 ******************************************************************************/
U8BIT flashMemWriteProtected
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT Value = 0;
    U32BIT FlashProtected = 0;
    S16BIT retval = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE_PROTECTED, "ENTER->\n");

    retval = DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_STATUS), &Value);

    if (retval)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE_PROTECTED, "Read Flash Mem Write Protected Error!\n");
        return (U8BIT)retval;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_FLASH, DDC_DBG_FLASH_MEM_WRITE_PROTECTED, "MemWriteProtect 0x%08x\n", Value);

    /* get Flash writable configuration */
    if (Value & ACEX_BD_BDSTATUS_MASK_FLASH_WRITE_EN)
    {
        FlashProtected = TRUE;
    }
    else
    {
        FlashProtected = FALSE;
    }

    return (U8BIT)FlashProtected;
}
