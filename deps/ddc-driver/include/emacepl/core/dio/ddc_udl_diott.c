/*******************************************************************************
 * FILE: ddc_udl_diott.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide function to support DIO Time Tag.
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
#include "include/ddc_types.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "driver_sdk/ddc_udl_dma_private.h"
#include "core/dio/ddc_udl_diott.h"

#define TT_EXT_ENA  0x00000200


/*******************************************************************************
 * Name:    ddcUdlDioTtInitialize
 *
 * Description:
 *      This function initializes the DIO TT data structure with the
 *      base register and memory locations for the various components
 *
 * In   pDeviceContext          device-specific structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ddcUdlDioTtInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    U16BIT numDevicesIndex;
    int i;
    int j;
    UM_DEVICE_INFO *pUmDevicePtr;

    memset(&(pDeviceContext->sDioTt), 0, sizeof(DIO_TT_TYPE));

    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        if (pUmDevicePtr->umDevType == UM_DEVICE_ID_DIO_TT)
        {
            for (j = 0; j < pUmDevicePtr->umDevNumComponents; j++)
            {
                switch (pUmDevicePtr->umComponentInfo[j].umComponentType)
                {
                    case UM_COMPONENTS_ID_DIO_TT:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sDioTt.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sDioTt.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                            pDeviceContext->sDioTt.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sDioTt.pu32MemSize = &(pUmDevicePtr->umDevMemSize);
                            pDeviceContext->u8NumDioTt = 1;
                        }

                        break;
                    }

                    default:
                    {
                        /* unknown component */
                        continue;
                    }
                }
            }
        }
    }

    /* Reset the DIO TT component */
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, BD_RESET_DIO_TT);

    /* Enable DIO TT board level interrupt */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_DIO_TT);
#if DDC_DMA_MT
    /* Allocate DMA destination buffer */
    pDeviceContext->sDioTt.pu8DmaTarget = DDC_DMA_MALLOC(
        pDeviceContext,
        (*(pDeviceContext->sDioTt.pu32MemSize) << 2), /* shift by 2 to convert # 32-bit words to bytes */
        &pDeviceContext->sDioTt.DmaAddr,
        DDC_MEMORY_REGION__DIO_TT_DMA);

    if (!pDeviceContext->sDioTt.pu8DmaTarget)
    {
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }
#endif /* DDC_DMA_MT */

    DDC_ISR_LOCK_INIT(pDeviceContext->sDioTt.semDmaEventCond);

    /* This is redundant with 'bdReset()' above */
    pDeviceContext->sDioTt.state = ACEX_MOD_RESET;
    pDeviceContext->sDioTt.bIsrEnabled = FALSE;
    pDeviceContext->sDioTt.u16DmaCmpltEventCond = 0;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlDioTtFree
 *
 * Description:
 *      This function free all the DIO TT resources.
 *
 * In   pDeviceContext          device-specific structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ddcUdlDioTtFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    /* Disable DIO TT board level interrupt */
    ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_DIO_TT);

    /* Reset the DIO TT component */
    pDeviceContext->sDioTt.state = ACEX_MOD_RESET;

    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, BD_RESET_DIO_TT);

#if DDC_DMA_MT
    if (pDeviceContext->sDioTt.pu8DmaTarget != NULL)
    {
        DDC_DMA_FREE(
            pDeviceContext,
            (*pDeviceContext->sDioTt.pu32MemSize << 2),     /* shift by 2 to convert # 32-bit words to bytes */
            pDeviceContext->sDioTt.pu8DmaTarget,
            pDeviceContext->sDioTt.DmaAddr,
            DDC_MEMORY_REGION__DIO_TT_DMA);

        pDeviceContext->sDioTt.pu8DmaTarget = NULL;
    }
#endif /* DDC_DMA_MT */

    pDeviceContext->sDioTt.state = ACEX_MOD_ERROR;
    pDeviceContext->sDioTt.bIsrEnabled = FALSE;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
Description:
    Configures the board level Discrete IO Time Tag feature.

Description:
    Configures the board level Discrete IO Time Tag feature.

State: READY, RUN if aceInitialize used or
       IO Initialized if aceIOInitialzed used.

In    DevNum = number associated with the hardware being accessed.
In    psDioTtCfg = DIO TT configuration option structure pointer:

      u32Dio:
        Bitwise pattern corresponding to discrete
        input signals that will time tag low to high
        and/or high to low transitions.
        The lower eight bits (D0 - D7) enable the feature
        for low to high discrete signal transitions.
        The next eight bits (D8 - D15) enable the feature
        for high to low discrete signal transitions.
        '0x00000001' => rising edge discrete signal 1
        '0x00000002' => rising edge discrete signal 2
        '0x00000003' => rising edge discrete signals 1 and 2
        '0x0000000B' => rising edge discrete signals 1, 2, and 4
        '0x00000102' => rising edge discrete signal 2,
                        and falling edge of discrete signal 1
                               etc.

      u32TtCfg:
        Time tag clock source options:
          TT_RO_16_BIT   Timer rollover points
          TT_RO_17_BIT
          TT_RO_18_BIT
          TT_RO_19_BIT
          TT_RO_20_BIT
          TT_RO_21_BIT
          TT_RO_22_BIT
          TT_RO_48_BIT
          TT_RESO_64US   Internal timer source tick resolution
          TT_RESO_32US
          TT_RESO_16US
          TT_RESO_08US
          TT_RESO_04US
          TT_RESO_02US
          TT_RESO_01US
          TT_RESO_500NS
          TT_RESO_100NS
          TT_TST_CLK     On board register write clock source
          TT_EXT_CLK     External clock source
          TT_IRIGB       IRIG clock source

      u32IntMsk:
        Optional interrupt conditions:
          TT_INT_ENT_OVFL  Card buffer overflow
          TT_INT_ENT_CNT   Preprogrammed time tag entry count reached
          TT_INT_BUF_100   Time tag buffer 100% full
          TT_INT_BUF_75                     75% full
          TT_INT_BUF_50                     50% full
          TT_INT_BUF_25                     25% full
          TT_INT_RO        Timer roll over

Out   return = error condition
 ******************************************************************************/
S16BIT ddcUdlDioTtCfg
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32RegVal = 0;

    /* Allow configuration during initialization and on the fly */
    if ((pDeviceContext->sDioTt.state != ACEX_MOD_RESET) &&
       (pDeviceContext->sDioTt.state != ACEX_MOD_OPEN)    )
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* Check for invalid configuration and interrupt condition bits */
    if ((pIoctlParams->Param2 & ~((U32BIT)TT_CFG_MSK)) || (pIoctlParams->Param3 & ~((U32BIT)TT_INT_MSK)))
    {
        return DDC_UDL_ERROR__CHANNEL;
    }

    /* If the external clock is to be used, configure the board and timer to pass the signal through */
    if ((pIoctlParams->Param2 & 0x0000000F) == TT_EXT_CLK)
    {
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CONFIG, &u32RegVal);
        u32RegVal |= (BD_CONFIG_MASK_EXT_TT_CLK_EN | BD_CONFIG_MASK_EXT_TT_CNT_EN);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CONFIG, &u32RegVal);
        u32RegVal = TT_EXT_ENA;
    }

    /* Shift and merge the discrete enables and timer source options */
    u32RegVal |= DDC_IOCTL_U32((pIoctlParams->Param1 << 16) | pIoctlParams->Param2);
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDioTt.pu32RegBA) + REG_GENERAL_CTRL, &u32RegVal);

    /* Set any interrupt conditions */
    u32RegVal = DDC_IOCTL_U32(pIoctlParams->Param3);
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDioTt.pu32RegBA) + REG_GENERAL_INT_MASK, &u32RegVal);

    /* If all parameters are zero, stop the timer, shut down the logic, and reset the 'tail' pointer to zero */
    if (!pIoctlParams->Param1 && !pIoctlParams->Param2 && !pIoctlParams->Param3)
    {
        u32RegVal = TT_CTL_STOP;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDioTt.pu32RegBA) + REG_GENERAL_CTRL_PULSE, &u32RegVal);
        pDeviceContext->sDioTt.u32Tail = 0;
        pDeviceContext->sDioTt.state = ACEX_MOD_RESET;
        pDeviceContext->sDioTt.bIsrEnabled = FALSE;
    }
    else
    {
        pDeviceContext->sDioTt.state = ACEX_MOD_OPEN;
        pDeviceContext->sDioTt.bIsrEnabled = TRUE;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
Name:   DioTtCtl

Description:
    Controls the board level Discrete IO Time Tag feature.

In    DevNum = number associated with the hardware being accessed.
In    u32Ctl = DIO TT control operation:
        TT_CTL_START    Start the discrete time tag timer
        TT_CTL_STOP     Stop the timer
        TT_CTL_RESET    Reset the timer to zero

Out   return = error condition
 ******************************************************************************/
S16BIT ddcUdlDioTtCtl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32RegVal;
    
    /* The feature needs to be configured prior to issuing commands to it */
    if (pDeviceContext->sDioTt.state != ACEX_MOD_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* Check for invalid bits */
    if (pIoctlParams->Param1 & ~((U32BIT)TT_CTL_MSK))
    {
        return DDC_UDL_ERROR__CHANNEL;
    }

    u32RegVal = DDC_IOCTL_U32(pIoctlParams->Param1);
    
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sDioTt.pu32RegBA) + REG_GENERAL_CTRL_PULSE, &u32RegVal);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlDioTtRead
 *
 * Description:
 *      This function reads the DIO TT memory block from the card
 *
 * In   pDeviceContext          device-specific structure
 * In   pRdData                 pointer to buffer where read data will go
 * In   OutputBufferLength      size of output buffer
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT ddcUdlDioTtRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *pRdData,
    size_t OutputBufferLength
)
{
    U32BIT u32Head = 0;
    U32BIT u32Entries;
    U32BIT u32StartAddr;

    if (pDeviceContext->sDioTt.state != ACEX_MOD_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* Verify the output buffer is not zero length and can store at least one 64 bit entry */
    if ((OutputBufferLength == 0) || (OutputBufferLength < (1 << 3)))
    {
        return DDC_UDL_ERROR__BUFFER_SIZE;
    }

    /* Each DIO TT entry consists of two 32 bit locations, therefore, the head pointer register increments by 2 for each entry */
    /* The head actually points to the next available entry that the logic will populate */
    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sDioTt.pu32RegBA) + REG_DIO_TT_HEAD_POINTER, &u32Head);

    /* Convert the read head pointer value from a 32 bit pointer to an entry index */
    u32Head >>= 1;

    /* The tail is maintained as an entry index */
    /* Determine if data is available to transfer */
    if (pDeviceContext->sDioTt.u32Tail == u32Head)
    {
        return -TRUE;
    }

    /* Convert the tail index entry to bytes and store the address for the DMA operation */
    u32StartAddr = pDeviceContext->sDioTt.u32Tail << 3;

    /* Calculate the number of entries to transfer, taking into consideration */
    /* a circular rollover and the size of the buffer to populate */
    if (u32Head > pDeviceContext->sDioTt.u32Tail)
    {
        u32Entries = u32Head - pDeviceContext->sDioTt.u32Tail;
    }
    else
    {
        u32Entries = ((*(pDeviceContext->sDioTt.pu32MemSize) >> 1) - pDeviceContext->sDioTt.u32Tail) + u32Head;
    }

    if ((OutputBufferLength >> 3) < u32Entries)
    {
        pDeviceContext->sDioTt.u32Tail += (U32BIT)(OutputBufferLength >> 3);
        pDeviceContext->sDioTt.u32Tail &= ((*(pDeviceContext->sDioTt.pu32MemSize) >> 1) - 1);
        u32Entries = (U32BIT)(OutputBufferLength >> 3);
    }
    else
    {
        pDeviceContext->sDioTt.u32Tail = u32Head;
    }
#if DDC_DMA_MT
    DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);
    if (pDeviceContext->sDioTt.u16DmaCmpltEventCond)
    {
        pDeviceContext->sDioTt.u16DmaCmpltEventCond--;
    }
    DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);

    dmaStartDioTt(pDeviceContext,
        (u32Entries << 3),  /* Convert the number of 64-bit entries to byte count */
        ((*(pDeviceContext->sDioTt.pu32MemBA) << 2) + u32StartAddr), /* Start address for DMA transaction */
        pDeviceContext->sDioTt.pu8DmaTarget);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);
    if (pDeviceContext->sDioTt.u16DmaCmpltEventCond == 0)
    {
        int iResult = 0;
        
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);
            
        iResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
            pDeviceContext->sDioTt.dmaCmpltWaitqueueEvent, 
            pDeviceContext->sDioTt.u16DmaCmpltEventCond, 
            1000);
        if (iResult) { }
        
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semDmaEventCond, pDeviceContext->sDioTt.semDmaEventCondFlag);
    }
#else
    /* use direct memory reads */
    {
        DDC_BLK_MEM_READ(pDeviceContext,
            ((*(pDeviceContext->sDioTt.pu32MemBA) << 2) + u32StartAddr),
            (U32BIT *)pDeviceContext->sDioTt.pu8DmaTarget,
            (u32Entries << 3),
            ACEX_32_BIT_ACCESS);
    }
#endif
    return DDC_UDL_ERROR__SUCCESS;
}
