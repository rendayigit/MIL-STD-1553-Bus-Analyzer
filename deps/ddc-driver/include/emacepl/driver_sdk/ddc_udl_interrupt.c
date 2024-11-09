/*******************************************************************************
 * FILE: ddc_udl_interrupt.c
 *
 * DESCRIPTION:
 *
 *  This module  contains the main interrupt handler routine.
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
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_mt_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_um_regmap_private.h"
#include "driver_sdk/ddc_udl_driver_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "os/interface/ddc_udl_os_interrupt_private.h"
#include "os/interface/ddc_udl_os_worker_private.h"
/* #include "os/bus/ddc_udl_os_bus_private.h" */


#define INTERRUPT_MASK (BD_INT_STATUS_MASK_INT_REQ | \
        BD_INT_STATUS_MASK_TEST_INT | \
        BD_INT_STATUS_MASK_IRIG_1_SEC | \
        BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | \
        BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT | \
        BD_INT_STATUS_MASK_1553_CH | \
        BD_INT_STATUS_MASK_ARINC_0 | \
        BD_INT_STATUS_MASK_UART)

#define DEFAULT_INTERRUPT_MASK ( \
        BD_INT_STATUS_MASK_IRIG_1_SEC | \
        BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | \
        BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT)

#define BC_INT_MASK                 0xFFFFFF00
#define GEN_INT_MASK                0x000000FF
#define MT_INT_MASK                 0xFFFF0000
#define RT_INT_MASK                 0x0000FFFF
#define MTI_NUM_MSGS_INT_MASK       0xFFFFFFFF
#define MTI_LENGTH_INT_MASK         0xFFFFFFFF
#define MTI_START_ADDRESS_INT_MASK  0xFFFFFFFF
#define IMP_INT_MASK                0xFFFFFFFF

/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */
#ifndef DDC_INT_TASK
static void _ddcUdlIntProcessor
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#else
void _ddcUdlIntProcessor
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern Error _ddcUdlHookIntDispatcher
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);
#endif
 

/*******************************************************************************
 * Name:    ddcUdlIsr
 *
 * Description:
 *      This function gets the board level interrupt status and
 *      save them into pDeviceContext->u32BdIntStatus.
 *
 * In   u8DeviceNumber      Device Number
 * Out  none
 *
 * Returns: 0=IRQ Not Handled   1=IRQ Handled
 ******************************************************************************/
BOOLEAN ddcUdlIsr
(
    U8BIT u8DeviceNumber
)
{
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext = NULL;
    U32BIT u32BdIntStatus = 0;
    U32BIT u32BdIntEnable = 0;
    U32BIT u32BdIntEnable2 = 0;

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "ENTER->\n");

    pDeviceContext = ddcUdlGetDeviceContext(u8DeviceNumber);
 
    /*  JMN - Added temporary fix(until FW is fixed) for Interrupt stop problem 
        (RT, BC and Serial Interrupts would randomly stop).
    */
    /* Read the card master interrupt status register */
    u32BdIntEnable = ddcUdlOsBusGetInterruptStatus(
        pDeviceContext,
        UM_DEVICE_ID_BRD_GLOBAL,
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA),
        REG_BD_INT_EN);
        
    /* Read the card master interrupt status register */
    u32BdIntStatus = ddcUdlOsBusGetInterruptStatus(
        pDeviceContext,
        UM_DEVICE_ID_BRD_GLOBAL,
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA),
        REG_BD_INT_STATUS);

    /* 
        JMN - Added temporary fix(until FW is fixed) for Interrupt stop problem 
        (RT, BC and Serial Interrupts would randomly stop).
    */
    /* Read the card master interrupt status register */
    u32BdIntEnable2 = ddcUdlOsBusGetInterruptStatus(
        pDeviceContext,
        UM_DEVICE_ID_BRD_GLOBAL,
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA),
        REG_BD_INT_EN);
        
    if (u32BdIntStatus == 0)
    {
        if(u32BdIntEnable & 0x80000000)
        {
            if(!(u32BdIntEnable2 & 0x80000000))
            {
                u32BdIntStatus = u32BdIntEnable - u32BdIntEnable2;

                DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "INT status modified\n");
            }
        }
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "INT STATUS: 0x%08x\n", u32BdIntStatus);

    /* If this card didn't generate the interrupt, bail */
    if (!(u32BdIntStatus & INTERRUPT_MASK))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "***** INTERRUPT_MASK - No interrupt happened *****\n");
        return DDC_IRQ_NONE;
    }

    /* Check the master bit */
    if (!(u32BdIntStatus & BD_INT_STATUS_MASK_INT_REQ))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "***** BD_INT_STATUS_MASK_INT_REQ - No interrupt happened *****\n");

        return DDC_IRQ_NONE;
    }
    
   
    pDeviceContext->u32BdIntStatus = u32BdIntStatus;

#ifdef DDC_INT_TASK
    _ddcUdlHookIntDispatcher(pDeviceContext);
#else      
    _ddcUdlIntProcessor(pDeviceContext);
#endif
    /* <UDL9> */
    /* ddcScheduleWork(_ddcUdlIntProcessor, data); */

    return DDC_IRQ_HANDLED;
}

/*******************************************************************************
 * Name:    _ddcUdlIntProcessor
 *
 * Description:
 *      This function gets the interrupt status from each component (BC/RT/MT/MTI/IMP)
 *      according to board level interrupt status.
 *
 *      They are stored in an array au32IntStatus[], and at the same time get processed.
 *
 *      Each channel has a bunch of interrupt status information
 *      in au32IntStatus[] as follows:
 *
 *      DWD_01 - D31:08 - Ch1 1553 BC interrupt reg(23:0)
 *               D07:00 - Ch1 1553 general interrupt reg(7:0)
 *      DWD_02 - D31:16 - Ch1 1553 MT interrupt reg(15:0)
 *               D15:00 - Ch1 1553 RT interrupt reg(15:0)
 *      DWD_03 - D31:00 - Ch1 1553 MTI number of messages
 *      DWD_04 - D31:00 - Ch1 1553 MTI length
 *      DWD_05 - D31:00 - Ch1 1553 MTI starting address
 *      DWD_06 - D31:00 - Ch1 1553 improvements interrupt reg(31:0)
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
#ifdef DDC_INT_TASK
void _ddcUdlIntProcessor
#else
static void _ddcUdlIntProcessor
#endif
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT au32IntStatus[ACEX_NUM_DWDS_IN_INTERRUPT]; /* store all interrupt status */

    U32BIT u321553ChIndex = 0;
    U8BIT i = 0;

    struct _ACEX_1553_CHANNEL_TYPE *pCh = NULL;
    U32BIT u32GenIntStatus = 0;
    U32BIT u32RegData = 0;
    U32BIT u32IrqData;
    U32BIT u32ArincIoInterruptType = 0x00000000;
    U32BIT u32AioInterruptStatus = 0x00000000;
    U32BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ENTER->\n");

    /* retrieve board level interrupt status information */
    memset(au32IntStatus, 0, (sizeof(U32BIT) * ACEX_NUM_DWDS_IN_INTERRUPT));
    au32IntStatus[0] = pDeviceContext->u32BdIntStatus;
    
    /* MTi Irig 1 second Time Packet Interrupt */
    if (au32IntStatus[0] & BD_INT_STATUS_MASK_IRIG_1_SEC)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ISR, "***** MTI IRIG 1 Second Time Packet Interrupt Occured *****\n");
        mtStoreTimeData(pDeviceContext);
    }

    /* DMA completion interrupt handling */
    if (au32IntStatus[0] & (BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "DMA CMPLT (%d)\n", pDeviceContext->u16DriverType);

        switch (pDeviceContext->u16DriverType)
        {
            case ACEX_IO_DRIVER:
            {
                /* doing nothing */
                break;
            }

            case ACEX_QPRM_DRIVER:
            {
                /* write DMA reg to clear DMA channel 0 interrupt */
                u32RegData = ddcUdlOsBusAsicDmaRead(pDeviceContext, REG_QPRM_DMA_CS);
                u32RegData |= QPRIME_DMA_CS_CLEAR_INT;
                ddcUdlOsBusAsicDmaWrite(pDeviceContext, REG_QPRM_DMA_CS, u32RegData);

                dmaCmpltHandler(pDeviceContext);
                break;
            }

            case ACEX_PCIE_DRIVER:
            case ACEX_DD429_PCIE_DRIVER:
            {
                /* PCIe DMA */

                if (pDeviceContext->sDMA.queueEntryCurrentTransaction.totalBytesTransfered < pDeviceContext->sDMA.queueEntryCurrentTransaction.ByteCount)
                {
                    /* continue DMA until the whole transaction is completed */
                    dmaContinuePCIe(pDeviceContext);
                }
                else
                {
                    dmaCmpltHandler(pDeviceContext);
                }

                break;
            }

            case ACEX_PCI_DRIVER:
            case ACEX_DD429_PCI_DRIVER:
            default:
            {
                /* DMA uses PLX 9056 */

                /* write PLX reg to clear DMA channel 0 interrupt */
                ddcUdlOsBusReadPlx(pDeviceContext, REG_PLX_DMA0_CSR, &u32RegData);
                u32RegData |= PLX_CS_CLEAR_INT;
                ddcUdlOsBusWritePlx(pDeviceContext, REG_PLX_DMA0_CSR, u32RegData);
                dmaCmpltHandler(pDeviceContext);
                break;
            }
        }
    }

    /* Test Interrupt handling */
    if (au32IntStatus[0] & BD_INT_STATUS_MASK_TEST_INT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "TEST INT Occurred\n");
    }

    /* AIO INTERRUPT */
    if (au32IntStatus[0] & BD_INT_STATUS_MASK_AIO_INTERRUPT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "AIO INT Occurred\n");

        /* read the AIO interrupt status register */
        u32AioInterruptStatus = ddcUdlOsBusGetInterruptStatus(
            pDeviceContext,
            UM_DEVICE_ID_BRD_GLOBAL,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA),
            REG_BD_AIO_INTERRUPT_STATUS);

        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "AIO Irq Status 0x%08x\n", u32AioInterruptStatus);

        /* did the 429 library request this interrupt? */
        if (pDeviceContext->u32IrqAioInterruptMask & u32AioInterruptStatus)
        {
            /* NOTE: 429 library will have to get the AIO interrupt status by calling the GetIntStatus() API */
            /*       this will just send back the indication */
            u32ArincIoInterruptType |= MIO_INT_STATUS_AIO;
        }

        /* loop throuh all 1553 channels to see if any of them requested the AIO interrupt */
        for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
        {
            pCh = pDeviceContext->pChannel1553[i];

            /* did this 1553 channel request this AIO interrupt? */
            if (pCh->u32IrqAioInterruptMask & u32AioInterruptStatus)
            {
                DDC_ISR_LOCK_TAKE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);

                /* add interrupt status into queue if the queue is not full */
                if (pCh->u16IrqEventCond < IRQ_STATUS_QUEUE_SIZE)
                {
                    pCh->pu32IrqStatusQ[pCh->u32IrqStatusQHead].u32Type = DDC_IRQ_TYPE_AIO;
                    pCh->pu32IrqStatusQ[pCh->u32IrqStatusQHead].u32Status = u32AioInterruptStatus;
                    pCh->u32IrqStatusQHead++;

                    /* check for wrap condition */
                    if (pCh->u32IrqStatusQHead == (IRQ_STATUS_QUEUE_SIZE - 1))
                    {
                        pCh->u32IrqStatusQHead = 0;
                    }

                    pCh->u16IrqEventCond++;
                }

                DDC_ISR_LOCK_GIVE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);

                /* wake up the blocked thread so it can process */
                DDC_WAKE_UP_INTERRUPTIBLE(&pCh->waitqueueIrqCallback, &pCh->waitqueueIrqEvent);
            }
        }
    }

    /* ARINC 429/ARINC 717/CAN/UART Interrupt handling*/
    if ((au32IntStatus[0] & (BD_INT_STATUS_MASK_ARINC_717 |
            BD_INT_STATUS_MASK_ARINC_0 |
            BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT |
            BD_INT_STATUS_MASK_CAN_1 |
            BD_INT_STATUS_MASK_CAN_2 |
            BD_INT_STATUS_MASK_UART)) |
        (u32ArincIoInterruptType != 0x00000000))

    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ARINC/UART Interrupt handling, status 0x%08x\n", au32IntStatus[0]);

        DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

        if (pDeviceContext->u32ArincIntQLen < ARINC_IRQ_STATUS_QUEUE_SIZE)
        {
            /* clear status */
            pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus = 0;

            /* check if ARINC 717 Interrupt captured */
            if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_PROG_ARINC_717)
            {
                if (au32IntStatus[0] & BD_INT_STATUS_MASK_ARINC_717)
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ARINC 717 INT Occurred, 0x%08x\n", au32IntStatus[0]);

                    /* Disable interrupt, re-enable them in library */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_DEVICE_ID_ARINC_717_GLOBAL,
                        *(pDeviceContext->sArincGlobal717.pu32RegBA),
                        REG_ARINC_717_GLOBAL_CONFIG);

                    u32RegData &= (~REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE);

                    DDC_REG_WRITE(pDeviceContext,
                        *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_CONFIG,
                        &u32RegData);

                    DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                    /*  take the new interrupt register mapping and make it look like the old */
                    pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus = (au32IntStatus[0] >> 2) & 0x00000100;
                }
            }

            /* check if ARINC 429 Interrupt capture */
            if ((pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_ARINC_429_GLOBAL) ||
                (pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_ARINC_429_GLOBAL_TX_MF))
            {
                if (au32IntStatus[0] & BD_INT_STATUS_MASK_ARINC_0)
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ARINC 429 INT Occurred, 0x%08x\n", au32IntStatus[0]);

                    /* Host Buffer interrupt handling */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX,
                        *(pDeviceContext->sArinc429RxGlobal.pu32RegBA),
                        ACEX_429_RX_GLOBAL_INT_STATUS_REG);

                    ARINC429HandleRxHostBufferInterrupt(pDeviceContext, u32RegData);

                    /* Disable interrupt, re-enable them in library */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX,
                        *(pDeviceContext->sArinc429RxGlobal.pu32RegBA),
                        0);

                    u32RegData &= (~DD429_RX_GLOBAL_MASK__INTERRUPT_ENABLE);

                    DDC_REG_WRITE(pDeviceContext,
                        *(pDeviceContext->sArinc429RxGlobal.pu32RegBA),
                        &u32RegData);

                    DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                    /*  take the new interrupt register mapping and make it look like the old */
                    pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus = (au32IntStatus[0] >> 2) & 0x00000040;

                    if (u32RegData & NEW_IRIG_TT_RO_INT_ENABLE_MASK)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "Time Tag Roll Over\n");
                        pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus |= MIO_INT_STATUS_TTRO;
                    }
                }
            }

            /* check if ARINC VOLT_MON Interrupt capture */
            if (au32IntStatus[0] & BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT)
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ARINC 429 VOLT_MON INT Occurred, 0x%08x\n", au32IntStatus[0]);

                /* clear status register */
                u32RegData = ddcUdlOsBusGetInterruptStatus(
                    pDeviceContext,
                    UM_COMPONENTS_ID_BRD_GLOBAL_VOLT_MON_X8,
                    *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA),
                    BD_VOLT_MON_X8__STATUS_REG);

                DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus |= BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT;
            }

            /* check if UART Interrupt capture */
            if (au32IntStatus[0] & BD_INT_STATUS_MASK_UART)
            {
                  /* Need to check here for Cast IP functionallity. */
                if (pDeviceContext->sMioUart429.pu32RegBA != NULL)
                {
                    /* No Cast IP functionality */
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "UART INT Occurred, 0x%08x\n", au32IntStatus[0]);

					DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
                    /* Disable the Serial Module Interrupt generation */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIO_UART,
                        *(pDeviceContext->sMioUart429.pu32RegBA),
                        (MIO_UART_IO_CTRL & 0x00FF));

                    u32RegData &= (~SER_INT_ENABLE);

                    DDC_REG_WRITE(pDeviceContext,
                        *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_IO_CTRL & 0x00FF),
                        &u32RegData);
						
					DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);						
                }
                else 
                {
                    DDC_IOCTL_PARAMS sIoctlParams;
                    U32BIT u32status = 0;
                    U32BIT u32IntChannel = 0x01;
                    U8BIT i = 0;
                    U32BIT u32StatusData = 0;
                    sIoctlParams.Channel = 0; 
                    sIoctlParams.Param2 = UART_INTERRUPT_STATUS;

					DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
                    /* first find the channel that Interrupted us */
                    u32status = ARINC429CastGetSerialIOConfig(pDeviceContext, &sIoctlParams);
					pDeviceContext->u32CastIO_bdIntStatus[0] = u32status;
					/* Clear hbuf return size Status word */
					pDeviceContext->u32CastIO_bdIntStatus[1] = 0;
					
					
					/* Read MIO_IIR register (Interrupt Identification Register) Addr 2 
					   This will clear the Tx interrupt and provide IIR information(bottom 4 bits 0-3) 
					   to the user to discriminate between TX or RX Interrupts
					*/

                    while( i < 8)
                    {
                        if  ( u32status &  u32IntChannel )                    
                        {
							sIoctlParams.Channel = i;
							sIoctlParams.Param2 = 2;
							u32StatusData = ARINC429CastSerialIORegRead(pDeviceContext, &sIoctlParams);
							pDeviceContext->u32CastIO_bdIntStatus[2] = u32StatusData & 0x000f;
							if (pDeviceContext->u32CastIO_bdIntStatus[2] != 1)
							{
								serial_ioHandleRxHostBufferInterrupt(pDeviceContext, (U8BIT)i, u32status);
							}
							else{
								pDeviceContext->u32CastIO_bdIntStatus[2] = 0;
							}
                        }
                        
                        u32IntChannel =  u32IntChannel  << 1;
                        i++;
                    }
				    DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);	
                }

                DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "Disable Serial Int\n");
                /*  take the new interrupt register mapping and make it look like the old */
                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus |= (au32IntStatus[0] >> 2) & 0x00000080;
            }

            /* CAN Bus Interrupt capture */
            if ((au32IntStatus[0] & BD_INT_STATUS_MASK_CAN_1) ||
                (au32IntStatus[0] & BD_INT_STATUS_MASK_CAN_2))
            {
                DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
                DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "429 - CAN Bus INT Occurred\n");

                canHandleRxHostBufferInterrupt(pDeviceContext, &au32IntStatus[0]);

                DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

#if 0  /* <UDL7> */
                status = WdfIoQueueRetrieveNextRequest(pDeviceContext->IrqQueue429, &IrqRequest);
                WdfRequestRetrieveOutputBuffer(IrqRequest, 0, (void *) &IoBufferCan, NULL);
                pCanBusInterrupt = IoBufferCan;
#endif /* <UDL7> */

                /*  take the new interrupt register mapping and make it look like the old */
                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus |=
                    (au32IntStatus[0] & (BD_INT_STATUS_MASK_CAN_1 | BD_INT_STATUS_MASK_CAN_2));
            }

            /* add the AIO status if set */
            if (u32ArincIoInterruptType != 0x00000000)
            {
                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32MasterStatus |= u32ArincIoInterruptType;
                pDeviceContext->sArincIntStatus[pDeviceContext->u32ArincIntQHead].u32AioStatus = u32AioInterruptStatus;
            }

            /* point to the next INT status location */
            pDeviceContext->u32ArincIntQHead++;
            if (pDeviceContext->u32ArincIntQHead >= (ARINC_IRQ_STATUS_QUEUE_SIZE - 1))
            {
                pDeviceContext->u32ArincIntQHead = 0;
            }

            pDeviceContext->u32ArincIntQLen++;
        }

        DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

        /* Wake up the blocked thread */
        DDC_WAKE_UP_INTERRUPTIBLE(
            &pDeviceContext->waitqueueArincBlockOnIrqCallback,
            &pDeviceContext->waitqueueArincBlockOnIrqEvent);
    }

    /* DIO TT INTERRUPT */
    if (au32IntStatus[0] & BD_INT_STATUS_MASK_DIO_TT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "DIO TT INT Occurred\n");

        DDC_ISR_LOCK_TAKE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

        if (pDeviceContext->sDioTt.u32IntQLen < DIOTT_IRQ_STATUS_QUEUE_SIZE)
        {
            u32RegData = ddcUdlOsBusGetInterruptStatus(
                pDeviceContext,
                UM_COMPONENTS_ID_DIO_TT,
                *(pDeviceContext->sDioTt.pu32RegBA),
                REG_DIO_TT_INT_STATUS);

            /* point to the next INT status location */
            pDeviceContext->sDioTt.u32IntQHead++;

            if (pDeviceContext->sDioTt.u32IntQHead >= (ARINC_IRQ_STATUS_QUEUE_SIZE - 1))
            {
                pDeviceContext->sDioTt.u32IntQHead = 0;
            }

            pDeviceContext->sDioTt.sIntStatus[pDeviceContext->sDioTt.u32IntQHead] = u32RegData;

            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "DIO TT INT: 0x%08X\n", u32RegData);

            pDeviceContext->sDioTt.u32IntQLen++;
        }

        DDC_ISR_LOCK_GIVE(pDeviceContext->sDioTt.semIrqEventCond, pDeviceContext->sDioTt.semIrqEventCondFlag);

        /* Wake up the blocked thread */
        DDC_WAKE_UP_INTERRUPTIBLE(
            &pDeviceContext->sDioTt.waitqueueIrqCallback,
            &pDeviceContext->sDioTt.waitqueueIrqEvent);
    }

    /* 1553 interrupt handling */
    u321553ChIndex = 1; /* set start index to start of Ch 0 int status register set */

    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        if (au32IntStatus[0] & (0x1 << i))
        {
            /* INTERRUPT OCCURRED ON THIS CHANNEL */
            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "CH %d 1553 INT Occurred\n", i);
            pCh = pDeviceContext->pChannel1553[i];

            /* GENERAL INT */
            u32GenIntStatus = ddcUdlOsBusGetInterruptStatus(
                pDeviceContext,
                UM_DEVICE_ID_MIL_STD_1553, /* <UDL8> */
                *(pCh->pu32RegBA),
                REG_GENERAL_INT_STATUS);

            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "General Interrupt Status: 0x%08X\n", (int)u32GenIntStatus);

            if (u32GenIntStatus)
            {
                /* BC INT */
                if (u32GenIntStatus & GENERAL_INT_STATUS_BC_INT_ENABLED)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows BC Int\n");

                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_BCI,
                        *(pCh->sBC.pu32RegBA),
                        REG_BC_INT_STS);

                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC] = u32RegData;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "BC Interrupt Status: 0x%08X\n", (int)u32RegData);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "BC and General IntStatus: 0x%08X\n", \
                        (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC]);
                }

                /* Check for BC Component Interrupts */
                if ((u32GenIntStatus & 0x000000FF) || au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC])
                {
                    bcInterruptHandler(pDeviceContext, (U16BIT)i, au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC], u32GenIntStatus & 0x000000FF);
                }

                /* Check for General 1553 Component Interrupts */
                if (u32GenIntStatus & (
                        GENERAL_INT_STATUS_TT_ROLLOVER | GENERAL_INT_STATUS_RAM_PARITY_DETECTED | GENERAL_INT_STATUS_RAM_SELF_TEST_DONE | GENERAL_INT_STATUS_TRG_INT_ENABLED))
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "General INT Occurred\n");

                    if (u32GenIntStatus & GENERAL_INT_STATUS_TT_ROLLOVER)
                    {
                        pCh->u32IrqEv |= ACE_IMR1_TT_ROVER;
                    }

                    if (u32GenIntStatus & GENERAL_INT_STATUS_RAM_PARITY_DETECTED)
                    {
                        pCh->u32IrqEv |= ACE_IMR1_RAM_PAR_ERR;
                    }

                    if (u32GenIntStatus & GENERAL_INT_STATUS_RAM_SELF_TEST_DONE)
                    {
                        pCh->u32IrqEv |= ACE_IMR2_BIT_COMPLETE;
                    }

                    /* save trigger status and inform user the trigger interrupt*/
                    if (u32GenIntStatus & GENERAL_INT_STATUS_TRG_INT_ENABLED)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows TRIGGER Int\n");

                        trigger1553StatusWrite(pDeviceContext, (U16BIT)i, NULL, NULL);

                        pCh->u32IrqEv |= ACE_IMR2_BIT_TRIGGER;
                    }
                }

                /* MRT INT */
                if (u32GenIntStatus & GENERAL_INT_STATUS_RT_INT_ENABLED)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows RT Int\n");

                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_RTX,
                        *(pCh->sRT.pu32RegBA),
                        REG_MRT_REG_INT_STATUS);

                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT] = u32RegData & 0x0000FFFF;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "MRT Interrupt Status: 0x%08X\n", (int)u32RegData);

                    /* Check for MRT 1553 Component Interrupt */
                    if (u32RegData & RT_INT_MASK)
                    {
                        /* MULTI RT INTERRUPT HANDLER */
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                            "MRT INT Occurred status:0x%08x\n", (unsigned int)(u32RegData & RT_INT_MASK));
                        mrtInterruptHandler(pDeviceContext, i, (u32RegData & RT_INT_MASK));
                    }
                }

                /* MT INT */
                if (u32GenIntStatus & GENERAL_INT_STATUS_MT_INT_ENABLED)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows MT Int, status 0x%08x\n", u32GenIntStatus);

                    /* read the MTI INT QUEUE STATUS first to transfer MTI Interrupt Status, First Message Address,
                                Total Length and Num of Msgs from the FIFO to their associated registers.  */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE,
                        *(pCh->sMT.pu32RegBA),
                        REG_MT_MTI_INT_QUEUE_STATUS_RW);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "CH %d MT INT Q Status: 0x%08x\n", i, (int)u32RegData);

                    /* mt interrupt reg */
                    u32RegData = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE,
                        *(pCh->sMT.pu32RegBA),
                        REG_MT_MTI_INT_STATUS_RW);

                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT] |= (u32RegData & 0x0000FFFF) << 16;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "MT Interrupt Status: 0x%08X\n", (int)u32RegData);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "MT and RT IntStatus: 0x%08X\n", \
                        (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT]);

                    /* mti number of messages */
                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_NUM_MSGS] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE,
                        *(pCh->sMT.pu32RegBA),
                        REG_MT_MTI_NUMBER_OF_MSGS_RW);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                        "MTI Number of Messages: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_NUM_MSGS]);

                    /* mti length */
                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_LENGTH] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE,
                        *(pCh->sMT.pu32RegBA),
                        REG_MT_MTI_TOTAL_LENGTH_RW);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                        "MTI Length: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_LENGTH]);


                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_START_ADDR] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE,
                        *(pCh->sMT.pu32RegBA),
                        REG_MT_MTI_FIRST_MSG_ADDRESS_RW);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                        "MTI Starting Addr: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_START_ADDR]);

                    /* Check for MT 1553 Component Interrupt */
                    if (au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT] & MT_INT_MASK)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "MT INT Occurred\n");

                        /* Set 1553 channel specific interrupt mask */
                        gen1553InterruptClear(pDeviceContext, (U8BIT)i, GENERAL_INT_MASK_MT_INT_ENABLED);

                        /* This is required only during MT IRQ mode, which is set TRUE during library's
                         * call to aceMTInstallHBuf function */
                        if (pCh->sMT.u32BlockOnIrqEnable)
                        {
                            pCh->u32IrqEv |= ACE_IMR2_MTI_INTERRUPT;

                            /* Must also set msg/data rollover indications, if they occurred */
                            if ((au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT] >> 16) & MT_MTI_INT_STATUS_NUMBER_OF_MSGS)
                            {
                                pCh->u32IrqEv |= ACE_IMR2_MT_CSTK_50P_ROVER;
                            }

                            if ((au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT] >> 16) & MT_MTI_INT_STATUS_NUMBER_OF_WORDS)
                            {
                                pCh->u32IrqEv |= ACE_IMR2_MT_DSTK_50P_ROVER;
                            }
                        }

                        mtInterruptHandler(pDeviceContext,
                            i,
                            au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MT_RT],
                            au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_START_ADDR],
                            au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_LENGTH],
                            au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MTI_NUM_MSGS]);

                        /* Re-enable interrupt */
                        gen1553InterruptSet(pDeviceContext, (U8BIT)i, GENERAL_INT_MASK_MT_INT_ENABLED);
                    }
                }

                /* IMP INT   */
                if (u32GenIntStatus & GENERAL_INT_STATUS_BCMRT_IMP_INT_ENABLED)
                {
                    /* For shared IMP module, process one interrupt only */
                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows IMP Int\n");

                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_IMP] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP,
                        *(pCh->sImpBC.pu32RegBA),
                        ACEX_1553_IMP_REG_INT_STATUS);

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                        "IMP Interrupt Status: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_IMP]);

                    /* Check for IMP 1553 Component Interrupt */
                    if (au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_IMP] & IMP_INT_MASK)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "Ch %d IMP INT Occurred status:0x%08X\n",
                            i, (unsigned int)(au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_IMP] & IMP_INT_MASK));

                        /* process BC IMP interrupt if there is any */
                        status = bcImpInterruptHandler(pDeviceContext, (U16BIT)i, au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_IMP]);
                        /* process RT IMP interrupt if there is no BC interrupt */
                        if (status)
                        {
                            impRtInterruptHandler(pDeviceContext, i);
                        }
                    }
                }
                else
                {
                    /* For separated IMP modules, process BC and RT interrupts separately */
                    if (u32GenIntStatus & GENERAL_INT_STATUS_BC_IMP_INT_ENABLED)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows BC IMP Int\n");

                        au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC_IMP] = ddcUdlOsBusGetInterruptStatus(
                            pDeviceContext,
                            UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_BC,
                            *(pCh->sImpBC.pu32RegBA),
                            ACEX_1553_IMP_REG_INT_STATUS);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                            "BC IMP Interrupt Status: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC_IMP]);

                        /* Check for IMP 1553 Component Interrupt */
                        if (au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC_IMP] & IMP_INT_MASK)
                        {
                            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "Ch %d IMP INT Occurred status:0x%08X\n",
                                i, (unsigned int)(au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC_IMP] & IMP_INT_MASK));

                            /* process BC IMP interrupt if there is any */
                            status = bcImpInterruptHandler(pDeviceContext, (U16BIT)i, au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_BC_IMP]);
                        }
                    }

                    if (u32GenIntStatus & GENERAL_INT_STATUS_MRT_IMP_INT_ENABLED)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "1553 GEN INT REG shows MRT IMP Int\n");

                        au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MRT_IMP] = ddcUdlOsBusGetInterruptStatus(
                            pDeviceContext,
                            UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_MRT,
                            *(pCh->sImpRT.pu32RegBA),
                            ACEX_1553_IMP_REG_INT_STATUS);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC,
                            "MRT IMP Interrupt Status: 0x%08X\n", (int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_MRT_IMP]);

                        /* process MRT IMP interrupt if there is any */
                        impRtInterruptHandler(pDeviceContext, i);
                    }
                }

                /* Replay INT */
                if (u32GenIntStatus & GENERAL_INT_STATUS_REPLAY_INT_ENABLED)
                {
                    /* read the Replay component interrupt regsiter */
                    au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_REPLAY_IRQ_STATUS] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_COMPONENTS_ID_MIL_STD_1553_MF_REPLAY,
                        *(pCh->sReplay.pu32RegBA),
                        REG_REPLAY_IRQ_STATUS);

                    if (au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_REPLAY_IRQ_STATUS])
                    {
                        struct _ACEX_1553_REPLAY_TYPE * pReplay = &(pDeviceContext->pChannel1553[i]->sReplay);

                        if (pReplay->bReplayIsrEnabled == TRUE)
                        {
                            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ch%d: Replay IrqStatus 0x%08x, head %d\n",
                                i, (unsigned int)au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_REPLAY_IRQ_STATUS], pReplay->u32IrqStatusQHead);

                            DDC_ISR_LOCK_TAKE(pReplay->semIrqEventCond, pReplay->semIrqEventCondFlag);

                            /* add interrupt status into queue if the queue is not full */
                            if (pReplay->u16IrqEventCond < REPLAY_IRQ_STATUS_QUEUE_SIZE)
                            {
                                pReplay->pu32IrqStatusQ[pReplay->u32IrqStatusQHead] = au32IntStatus[u321553ChIndex + BD_INT_STATUS_OFFSET_REPLAY_IRQ_STATUS];

                                pReplay->u32IrqStatusQHead++;

                                if (pReplay->u32IrqStatusQHead == (REPLAY_IRQ_STATUS_QUEUE_SIZE - 1))
                                {
                                    pReplay->u32IrqStatusQHead = 0;
                                }

                                pReplay->u16IrqEventCond++;
                            }

                            DDC_ISR_LOCK_GIVE(pReplay->semIrqEventCond, pReplay->semIrqEventCondFlag);

                            /* Wake up the blocked thread so it can process */
                            DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "Wake up replay ch %d irq\n", i);

                            DDC_WAKE_UP_INTERRUPTIBLE(&pReplay->waitqueueIrqCallback, &pReplay->waitqueueIrqEvent);
                        }
                    }
                }
            }

            u32IrqData = (pCh->u32IrqUsrEv & pCh->u32IrqEv);

            if (u32IrqData != 0)
            {
                DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
                if (pCh->bIsr1553Enabled == TRUE)
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

                    DDC_ISR_LOCK_TAKE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);

                    pCh->au32UserIrqStatus = u32IrqData;

                    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "ch%d: IrqStatus 0x%08x, head %d\n",
                        i, (unsigned int)pCh->au32UserIrqStatus, pCh->u32IrqStatusQHead);

                    /* add interrupt status into queue if the queue is not full */
                    if (pCh->u16IrqEventCond < IRQ_STATUS_QUEUE_SIZE)
                    {
                        pCh->pu32IrqStatusQ[pCh->u32IrqStatusQHead].u32Status = pCh->au32UserIrqStatus;
                        pCh->pu32IrqStatusQ[pCh->u32IrqStatusQHead].u32Type = DDC_IRQ_TYPE_1553;

                        pCh->u32IrqStatusQHead++;
                        if (pCh->u32IrqStatusQHead == (IRQ_STATUS_QUEUE_SIZE - 1))
                        {
                            pCh->u32IrqStatusQHead = 0;
                        }

                        pCh->u16IrqEventCond++;
                    }

                    DDC_ISR_LOCK_GIVE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);

                    /* Wake up the blocked thread so it can process */
                    DDC_WAKE_UP_INTERRUPTIBLE(&pCh->waitqueueIrqCallback, &pCh->waitqueueIrqEvent);
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
                }
            }

            pCh->u32IrqEv = 0x0; /* Clear out the channel irq's */
        }

        /* update index to point to next channels registers */
        u321553ChIndex = u321553ChIndex + NUM_1553_INT_REGS_PER_CH;
    }

    /* start DMA first if there is DMA req in DMA queue */
    dmaQueueStart(pDeviceContext);

    DDC_ISR_LOCK_TAKE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);
    if (pDeviceContext->sDMA.u16State == ACEX_DMA_IDLE)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        /* rearm with current mask (no DMA Complete Int) */
        ddcUdlBdInterruptSet(pDeviceContext, 0);
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->sDMA.semDmaQ, pDeviceContext->sDMA.semDmaQFlag);

        /* Only Enable DMA Complete Int */
        u32RegData = BD_INT_STATUS_MASK_INT_REQ | BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT | BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT;
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DPC, "WRITE BDINT REG VALUE:%08x\n", u32RegData);

        DDC_REG_WRITE(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_EN,
            &u32RegData);
    }
}

/*******************************************************************************
 * Name:    ddcUdlIsInterruptEnabled
 *
 * Description:
 *      This function checks whether there is already at lease one channel with IsrEnabled,
 *      which means the irq is already requested for this board.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
U8BIT ddcUdlIsInterruptEnabled
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT i, bIntEnabled = 0;

    /* Return number of entities with interrupts enabled */
    for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
    {
        if (pDeviceContext->pChannel1553[i]->bIsr1553Enabled == TRUE)
        {
            bIntEnabled++;
        }
    }

    if (pDeviceContext->bArincSerialIsrEnabled == TRUE)
    {
        bIntEnabled++;
    }

    return bIntEnabled;
}

/*******************************************************************************
 * Name:    irqEnableInterrupt
 *
 * Description:
 *      This function enables 1553 data bus interrupt if it is the first
 *      channel to be enabled.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8ChannelNumber channel number
 * In   u8DeviceNumber  Device Number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT irqEnableInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8DeviceNumber
)
{
    U32BIT u32RegData = 0;
    S16BIT retval = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ENABLE, "ENTER->\n");

    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ENABLE, "IO device does not support interrupt\n");
        return retval;
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    /* Check to see if interrupts are already enabled for this channel */
    if (pDeviceContext->pChannel1553[u8ChannelNumber]->bIsr1553Enabled == TRUE)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ENABLE, "interrupt already enabled\n");

        return DDC_UDL_ERROR__ISR_ALREADY_ENABLED;
    }

    /* if no channel interrupt is enabled,  hook ISR and set board interrupt mask bit */
    if (0 == ddcUdlIsInterruptEnabled(pDeviceContext))
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        /* set board interrupt mask bit */
        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ);

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == FALSE)
        /* hook ISR */
        retval = ddcUdlOsIrqHookISR(pDeviceContext, u8DeviceNumber);
        if (retval != DDC_UDL_ERROR__SUCCESS)
        {
            /* Upon failure return error */
            return retval;
        }
#endif

        DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    }

    /* Flag channel interrupts as enabled */
    pDeviceContext->pChannel1553[u8ChannelNumber]->bIsr1553Enabled = TRUE;

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    /* set interrupt mask bit for this channel */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_1553_0 << u8ChannelNumber);

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_ENABLE, "WRITE BDINT REG VALUE:%08x\n", u32RegData);

    return retval;
}

/*******************************************************************************
 * Name:    irqDisableInterrupt
 *
 * Description:
 *      Disables interrupt
 *
 * In   pDeviceContext      device-specific structure
 * In   u8ChannelNumber     channel number
 * In   u8DeviceNumber      Device Number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT irqDisableInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8ChannelNumber,
    U8BIT u8DeviceNumber
)
{
    struct _ACEX_1553_CHANNEL_TYPE *pCh = NULL;
    S16BIT retval = DDC_UDL_ERROR__SUCCESS;

    pCh = pDeviceContext->pChannel1553[u8ChannelNumber];

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DISABLE, "ENTER->\n");

    if (pDeviceContext->u16DriverType == ACEX_IO_DRIVER)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DISABLE, "IO device does not support interrupt\n");
        return retval;
    }

    /* See if interrupts are enabled for this channel */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    if (pCh->bIsr1553Enabled == FALSE)
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

        /* No. Return error stating such */
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_DISABLE, "interrupts not enabled\n");
        retval = DDC_UDL_ERROR__ISR_NOT_ENABLED;
        return retval;
    }

    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    /* Disable HW interrupt masks*/
    ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_1553_0 << u8ChannelNumber);

    /* Flag chan. interrupt processing as disabled */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    pCh->bIsr1553Enabled = FALSE;

    if (0 == ddcUdlIsInterruptEnabled(pDeviceContext))
    {
        pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn = DEFAULT_INTERRUPT_MASK;
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == FALSE)
        /* unhook ISR*/
        ddcUdlOsIrqUnhookISR(pDeviceContext, u8DeviceNumber);
#endif
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    }

    /* Wake up the blocked thread so it can terminate */
    DDC_ISR_LOCK_TAKE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);
    pCh->au32UserIrqStatus = 0;
    pCh->u16IrqEventCond++;
    DDC_ISR_LOCK_GIVE(pCh->semIrqEventCond, pCh->semIrqEventCondFlag);

    DDC_WAKE_UP(&pCh->waitqueueIrqEvent);

    return retval;
}

/*******************************************************************************
 * Name:    irqEnableInterrupt429
 *
 * Description:
 *      This function enables 429 data bus interrupt if it is the first
 *      channel to be enabled.
 *
 * In   pDeviceContext  device-specific structure
 * In   u8DeviceNumber  Device Number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT irqEnableInterrupt429
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
)
{
    S16BIT retval = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_429_DPC, "ENTER (429)->\n");

    /* Check to see if interrupts are already enabled */
    if (pDeviceContext->bArincSerialIsrEnabled == TRUE)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_429_DPC, "interrupt already enabled\n");
		return DDC_UDL_ERROR__ISR_ALREADY_ENABLED;
    }

    /* if no channel interrupt is enabled,  hook ISR  */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    if (0 == ddcUdlIsInterruptEnabled(pDeviceContext))
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == FALSE)
        /* hook ISR */
        retval = ddcUdlOsIrqHookISR(pDeviceContext, u8DeviceNumber);
        if (retval != DDC_UDL_ERROR__SUCCESS)
        {
            /* Upon failure return error */
            return retval;
        }
#endif
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    }

    pDeviceContext->bArincSerialIsrEnabled = TRUE;
    ARINC429EnableRxInterrupts(pDeviceContext);

    return retval;
}

/*******************************************************************************
 * Name:    irqDisableInterrupt429
 *
 * Description:
 *      Disables 429 interrupt
 *
 * In   pDeviceContext  device-specific structure
 * In   u8DeviceNumber  Device Number
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT irqDisableInterrupt429
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_429_DPC, "ENTER (429)->\n");

    /* See if interrupts are enabled */
    if (pDeviceContext->bArincSerialIsrEnabled == FALSE)
    {
        /* No. Return error stating such */
        DDC_DBG_PRINT(DDC_DBG_MODULE_INTERRUPT, DDC_DBG_INTERRUPT_INT_429_DPC, "429 interrupts not enabled\n");
        return DDC_UDL_ERROR__ISR_NOT_ENABLED;
    }

    /* Flag chan. interrupt processing as disabled */
    ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_ARINC_0);

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    pDeviceContext->bArincSerialIsrEnabled = FALSE;

    if (0 == ddcUdlIsInterruptEnabled(pDeviceContext))
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

#if (DDC_UDL_OS_HOOK_ISR_AT_DRIVER_LOAD == FALSE)
        /* unhook ISR*/
        ddcUdlOsIrqUnhookISR(pDeviceContext, u8DeviceNumber);
#endif
    }
    else
    {
        DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    }

    /* Wake up the blocked thread so it can terminate */
    DDC_ISR_LOCK_TAKE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);
    pDeviceContext->u32ArincIntQLen = 1;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semArincIrqEventCond, pDeviceContext->semArincEventCondFlag);

    DDC_WAKE_UP(&pDeviceContext->waitqueueArincBlockOnIrqEvent);

    return DDC_UDL_ERROR__SUCCESS;
}

