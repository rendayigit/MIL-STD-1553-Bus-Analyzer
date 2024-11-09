/*******************************************************************************
 * FILE: ddc_udl_1553_trigger.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support triggers.
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
#include "os/interface/ddc_udl_os_util_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_flash_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"
#include "core/1553/ddc_udl_1553_trigger_private.h"


/* trigger registers for read*/
#define ACEX_REG_TRG_EVENT_ENABLE_READ          0x00
#define ACEX_REG_TRG_IRQ_STATUS_READ            0x01
#define ACEX_REG_TRG_TT_HI_READ                 0x02
#define ACEX_REG_TRG_TT_LO_READ                 0x03
#define ACEX_REG_TRG_EVENTS_A_READ              0x04
#define ACEX_REG_TRG_EVENTS_B_READ              0x05
#define ACEX_REG_TRG_STATUS_READ                0x06
#define ACEX_REG_TRG_INTERRUPT_MASK_READ        0x07

/* byte address of transceiver delay in flash memory*/
#define ACEX_TRANSCEIVER_DELAY_BA               (0x7F0000 << 1)


/*-------------------------------------------------------------------------------
   Function:
        _trigger1553Reset

   Description:
        This function reset all triggers in the given channel.

   Parameters:
      In pDeviceContext - device-specific structure
      In u16Ch          - 1553 channel number

   Returns:
      none
   ---------------------------------------------------------------------------------*/
static void _trigger1553Reset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32Reset = BD_RESET_1553_CH0 << u16Ch;

    /* Reset 1553 channel */
    ddcUdlBdReset( pDeviceContext, REG_BD_RESET_COMPONENT_TRG, u32Reset);
}

/*-------------------------------------------------------------------------------
   Function:
        trigger1553Initialize

   Description:
        This function initializes trigger component for the given channel.

   Parameters:
      In pDeviceContext - device-specific structure
      In u16Ch          - 1553 channel number

   Returns:
      S16BIT
   ---------------------------------------------------------------------------------*/
S16BIT trigger1553Initialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    ACEX_1553_TRIGGER_TYPE *pTrigger = &(pDeviceContext->pChannel1553[u16Ch]->sTrigger);

    /* initialize TRG module if it is detected */
    if (pTrigger->pu32RegBA)
    {
        /* reset all triggers in the channel */
        _trigger1553Reset( pDeviceContext, u16Ch);

        /* create a buffer to save trigger interrupt status */
        pTrigger->u32StatusEntries = ACEX_TRG_STATUS_ENTRY_NUM;
        pTrigger->u32StatusDwords = ACEX_TRG_STATUS_ENTRY_DWORDS;

        DDC_ISR_LOCK_INIT(pTrigger->slBuf);

        pTrigger->u32Count = 0;
        pTrigger->u32Head = 0;
        pTrigger->u32Tail = 0;
        pTrigger->hBuf = DDC_KERNEL_MALLOC(pDeviceContext, pTrigger->u32StatusDwords << 2);
        if (!pTrigger->hBuf)
        {
            pTrigger->u32StatusEntries = 0;
            pTrigger->u32StatusDwords = 0;
            pTrigger->hBuf = NULL;
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
        trigger1553Free

   Description:
        This function frees trigger status buffer.

   Parameters:
      In pDeviceContext - device-specific structure

   Returns:
      None
   ---------------------------------------------------------------------------------*/
void trigger1553Free
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT u16Ch;
    ACEX_1553_TRIGGER_TYPE *pTrigger;

    for (u16Ch = 0; u16Ch < pDeviceContext->u8Num1553Channels; u16Ch++)
    {
        pTrigger = &(pDeviceContext->pChannel1553[u16Ch]->sTrigger);

        if (pTrigger->pu32RegBA)
        {
            /* get mutex*/
            DDC_ISR_LOCK_TAKE((pTrigger->slBuf), pTrigger->slBufFlag);

            if (pTrigger->hBuf)
            {
                DDC_KERNEL_FREE(pDeviceContext, pTrigger->hBuf);
                pTrigger->hBuf = 0;
            }
            pTrigger->u32Count = 0;
            pTrigger->u32Head = 0;
            pTrigger->u32Tail = 0;

            /* release mutex*/
            DDC_ISR_LOCK_GIVE((pTrigger->slBuf), pTrigger->slBufFlag);
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
        trigger1553Reset

   Description:
        This function reset all triggers in the given channel.

   Parameters:
      In pDeviceContext - device-specific structure
      In  pIoctlParams        - point to the command from user
            Channel:    channel number  0-31
            Param1:     not used
            Param2:     not used
            Param3:     not used
            Param4:     not used

   Returns:
      S16BIT
   ---------------------------------------------------------------------------------*/
S16BIT trigger1553Reset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U16BIT u16Ch = (U16BIT) (pIoctlParams->Channel & 0xFFFF);
    ACEX_1553_TRIGGER_TYPE *pTrigger = &(pDeviceContext->pChannel1553[u16Ch]->sTrigger);

    _trigger1553Reset(pDeviceContext, u16Ch);

    /* get Buf spinlock */
    DDC_ISR_LOCK_TAKE((pTrigger->slBuf), pTrigger->slBufFlag);

    pTrigger->u32Count = 0;
    pTrigger->u32Head = 0;
    pTrigger->u32Tail = 0;

    /* release HBuf mutex */
    DDC_ISR_LOCK_GIVE((pTrigger->slBuf), pTrigger->slBufFlag);

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
      trigger1553StatusWrite

   Description:
        This function saves trigger and IRQ status into buffer. If pu32Status =
        NULL, the function will read from trigger status register directly.

   Parameters:
      In pDeviceContext - device-specific structure
      In u16Ch          - 1553 channel number
      In pu32TrgStatus  - point to trigger status
      In pu32TrgInt     - point to trigger interrupt status

   Returns:
      none
   ---------------------------------------------------------------------------------*/
void trigger1553StatusWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT *pu32TrgStatus,
    U32BIT *pu32TrgInt
)
{
    U32BIT u32data = 0;
    U32BIT *pIrqBuf = NULL;
    ACEX_1553_TRIGGER_TYPE *pTrigger = &(pDeviceContext->pChannel1553[u16Ch]->sTrigger);

    if (!pTrigger)
    {
        return;
    }

    /* get IRQ status buffer pointer */
    pIrqBuf = (U32BIT*)pTrigger->hBuf;

    /* save IRQ status */
    if (pIrqBuf)
    {
        /* get Buf spinlock */
        DDC_ISR_LOCK_TAKE((pTrigger->slBuf), pTrigger->slBufFlag);

        /* save the status if buffer is not full */
        if (pTrigger->u32Count < pTrigger->u32StatusEntries)
        {
            /* If pu32TrgStatus = NULL, read from trigger status register directly */
            if (pu32TrgStatus == NULL)
            {
                /* read and save trigger status */
                DDC_REG_READ(pDeviceContext, (*(pTrigger->pu32RegBA) + ACEX_REG_TRG_STATUS_READ), &u32data);
                *(pIrqBuf + pTrigger->u32Tail) = u32data;
            }
            else
            {
                /* USB provides the information through Interrupt packet */
                *(pIrqBuf + pTrigger->u32Tail) = *pu32TrgStatus;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_1553_TRIGGER, DDC_DBG_1553_TRIGGER_STATUS_WRITE, "%d: Trg status %08X\n", pTrigger->u32Tail, *(pIrqBuf + pTrigger->u32Tail));

            pTrigger->u32Tail++;

            /* If pu32TrgInt = NULL, read from trigger interrupt status register directly */
            if (pu32TrgInt == NULL)
            {
                /* read and save IRQ status */
                DDC_REG_READ(pDeviceContext, (*(pTrigger->pu32RegBA) + ACEX_REG_TRG_IRQ_STATUS_READ), &u32data);
                *(pIrqBuf + pTrigger->u32Tail) = u32data;
            }
            else
            {
                /* USB provides the information through Interrupt packet */
                *(pIrqBuf + pTrigger->u32Tail) = *pu32TrgInt;
            }
            DDC_DBG_PRINT(DDC_DBG_MODULE_1553_TRIGGER, DDC_DBG_1553_TRIGGER_STATUS_WRITE, "%d: Interrupt status %08X\n\n", pTrigger->u32Tail, *(pIrqBuf + pTrigger->u32Tail));

            /* increment the counter */
            pTrigger->u32Count++;

            /* adjust tail index */
            pTrigger->u32Tail++;
            if (pTrigger->u32Tail >= pTrigger->u32StatusDwords)
            {
                pTrigger->u32Tail = 0;
            }
        }

        /* release Buf mutex */
        DDC_ISR_LOCK_GIVE((pTrigger->slBuf), pTrigger->slBufFlag);
    }
}

/*-------------------------------------------------------------------------------
   Function:
      trigger1553StatusRead

   Description:
        This function returns interrupt and trigger status.

   Parameters:
      In  pDeviceContext     - device-specific structure
      In  pIoctlParams             - point to the command from user
            Channel:    channel number  0-31
            Param1:     not used
            Param2:     not used
            Param3:     not used
            Param4:     not used
      Out pBytesReturned     - bytes returned
      Out pRdData            - point to data buffer

   Returns:
      S16BIT
   ---------------------------------------------------------------------------------*/
S16BIT trigger1553StatusRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t          *pBytesReturned,
    U32BIT          *pRdData
)
{
    U32BIT u32Data = 0;

    U32BIT *pIrqBuf = NULL;
    U16BIT u16Ch = (U16BIT) (pIoctlParams->Channel & 0xFFFF);
    ACEX_1553_TRIGGER_TYPE *pTrigger = &(pDeviceContext->pChannel1553[u16Ch]->sTrigger);

    *pBytesReturned = 0;

    /* get IRQ status buffer pointer */
    pIrqBuf = (U32BIT*)pTrigger->hBuf;

    /* save IRQ status */
    if (pIrqBuf)
    {
        /* get Buf spinlock */
        DDC_ISR_LOCK_TAKE((pTrigger->slBuf), pTrigger->slBufFlag);

        /* read the status from buffer there is any */
        if (pTrigger->u32Count)
        {
            /* return the saved status */
            *pRdData = *(pIrqBuf + pTrigger->u32Head);
            DDC_DBG_PRINT(DDC_DBG_MODULE_1553_TRIGGER, DDC_DBG_1553_TRIGGER_STATUS_READ, "%d: Trg status %08X\n", pTrigger->u32Head, *pRdData);
            pTrigger->u32Head++;

            *(pRdData + 1) = *(pIrqBuf + pTrigger->u32Head);
            DDC_DBG_PRINT(DDC_DBG_MODULE_1553_TRIGGER, DDC_DBG_1553_TRIGGER_STATUS_READ, "%d: Trg IRQ    %08X\n", pTrigger->u32Head, *(pRdData + 1));

            *pBytesReturned = 2 * sizeof(U32BIT);

            /* decrement the counter */
            pTrigger->u32Count--;

            /* adjust head index */
            pTrigger->u32Head++;
            if (pTrigger->u32Head >= pTrigger->u32StatusDwords)
            {
                pTrigger->u32Head = 0;
            }
        }
        else /* oterwise, read from registers */
        {
            /* read status registers directly */
            DDC_REG_READ(pDeviceContext, (*(pTrigger->pu32RegBA) + ACEX_REG_TRG_STATUS_READ), &u32Data);
            *pRdData = u32Data;

            /* no interrupt */
            *(pRdData + 1) = 0;
        }

        /* release HBuf mutex */
        DDC_ISR_LOCK_GIVE((pTrigger->slBuf), pTrigger->slBufFlag);
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
      mfTransceiverDelayUpdate

   Description:
      This function updates Transceiver Delay for a given channel.

   Parameters:
      In pDeviceContext - device-specific structure
      In u16Ch          - 1553 channel number

   Returns:
      S16BIT
   ---------------------------------------------------------------------------------*/
S16BIT mfTransceiverDelayUpdate
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch
)
{
    U32BIT u32TransceiverDelayByteAddr;
    U16BIT u16TransceiverDelay;
    DDC_IOCTL_PARAMS sIoctlParams;
    U8BIT retval;
    U32BIT u32Data;

    struct _ACEX_1553_CHANNEL_TYPE *pCh = pDeviceContext->pChannel1553[u16Ch];

    /* update transceiver delay if it is MF card */
    if ((pCh->sImpBC.u32ComponentType != UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP) &&
        (pCh->sImpBC.u32ComponentType != UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP))
    {
        /* read data from flash */
        u32TransceiverDelayByteAddr = ACEX_TRANSCEIVER_DELAY_BA + (u16Ch << 1);

        sIoctlParams.Channel = 0;
        sIoctlParams.Param1 = u32TransceiverDelayByteAddr;   /* Address      */
        sIoctlParams.Param2 = 2;     /* ByteCount    */
        
        retval = flashMemRead(pDeviceContext, (void *) &sIoctlParams, &u16TransceiverDelay);
        
        if (retval)
        {
            return retval;
        }

        /* check if it is an invalid value */
        if (u16TransceiverDelay == 0xFFFF)
        {
            /* doing nothing. It is not an error */
            return DDC_UDL_ERROR__SUCCESS;
        }

        /* write to global register */
        u32Data = 0x00000000 | u16TransceiverDelay;
        return DDC_REG_WRITE(pDeviceContext, *(pCh->pu32RegBA) + REG_GENERAL_TRANSCEIVER_DELAY, (U32BIT *)&u32Data);
    }

    return DDC_UDL_ERROR__SUCCESS;
}
