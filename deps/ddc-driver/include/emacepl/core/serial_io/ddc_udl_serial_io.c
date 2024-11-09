/*******************************************************************************
 * FILE: ddc_udl_serial_io.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to provide functions to support Serial I/O.
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
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/serial_io/ddc_udl_serial_io_private.h"


#define KERNEL_MEMORY_ALLOC

#ifndef KERNEL_MEMORY_ALLOC
U8BIT u8testver[32768];
#endif

static void serial_ioDeleteRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel);

#ifdef DDC_OVERFLOW
static BOOLEAN serial_ioCheckHostBufferOverflow(U32BIT u32NextRead, U32BIT u32NextWrite, U32BIT u32NumElements);
#endif
static void serial_ioCopyToHostBuffer(U8BIT* pHostBufferMemory, U8BIT* pMemory, U8BIT u32NumMessagesToCopy);

static S16BIT serial_ioCreateRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel);

static U8BIT iOpenChannel[64];


/*-------------------------------------------------------------------------------
   Function:
       serial_ioInitialize

   Description:
        This function Initializes the host buffer of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
---------------------------------------------------------------------------------*/
S16BIT serial_ioInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    memset( iOpenChannel, 0, 64);
    return 0;
}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioOpen

   Description:
        This function open a host buffer on channel of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
---------------------------------------------------------------------------------*/
S16BIT serial_ioOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    S16BIT s16Status = DDC_UDL_ERROR__SUCCESS;
    
    if (u8Channel <= pDeviceContext->u8NumUart)
    {
        if (iOpenChannel[u8Channel] == 0)
        {
            s16Status = serial_ioCreateRxHostBuffer(pDeviceContext, u8Channel);
            iOpenChannel[u8Channel] = 1; 
        }
    }

    return s16Status;
}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioClose

   Description:
        This function closes and removes memory the host buffer of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
---------------------------------------------------------------------------------*/
S16BIT serial_ioClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    /*CAST_SERIAL_IO_HBUF_TYPE *sHbuf;*/   /* used for interrupt count check */
    S16BIT s16Status = DDC_UDL_ERROR__SUCCESS;

    /*sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);
    printk("chan %x IntCount %lx  \n", u8Channel, sHbuf->ulIntCount); */     /* used for interrupt count check */

    serial_ioDeleteRxHostBuffer(pDeviceContext, u8Channel); /* Delete host buffer */
    iOpenChannel[u8Channel] = 0;
    
    return s16Status;
}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioDeleteRxHostBuffer

   Description:
        This function deletes the host buffer of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
---------------------------------------------------------------------------------*/
static void serial_ioDeleteRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    CAST_SERIAL_IO_HBUF_TYPE *sHbuf;

    if ((pDeviceContext->u8NumRS232 > 0) ||
            (pDeviceContext->u8NumRS485 > 0) ||
            (pDeviceContext->u8NumUart > 0))
    {
        sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);
        
        if (sHbuf->hMemory != NULL)
        {
            DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
#ifdef KERNEL_MEMORY_ALLOC
            DDC_KERNEL_FREE(pDeviceContext, sHbuf->hMemory);
#else            
            memset(sHbuf->hMemory, 1, 32768);
#endif            
            sHbuf->hMemory = NULL;
            sHbuf->u32HbufNextRead = 0;
            sHbuf->u32HbufNextWrite = 0;
            sHbuf->u32HbufNumEntries = 0;
            sHbuf->bHbufOverflow = 0;
            sHbuf->bHbufQueueOverflow = 0;
            sHbuf->bHbufStoreTimeTag = FALSE;
            pDeviceContext->sSerialIORxHBuf.bRxFiFoHbufInstalled[u8Channel] = FALSE;
            DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioCreateRxHostBuffer

   Description:
        This function creates a host buffer for an Serial Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
---------------------------------------------------------------------------------*/
static S16BIT serial_ioCreateRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    CAST_SERIAL_IO_HBUF_TYPE *sHbuf;
    
    if ((pDeviceContext->u8NumRS232 > 0) ||
            (pDeviceContext->u8NumRS485 > 0) ||
            (pDeviceContext->u8NumUart > 0))
    {
        sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);

        /* Check if buffer already created */
        if (sHbuf->hMemory == NULL)
        {
            unsigned long u32BufferByteSize;

            u32BufferByteSize = (U32BIT)SERIAL_IO_RX_HBUF_MAX * sizeof(U8BIT);
            /* Create host buffer */
#ifdef KERNEL_MEMORY_ALLOC
            sHbuf->hMemory = DDC_KERNEL_MALLOC(pDeviceContext, u32BufferByteSize);
#else            
            sHbuf->hMemory = u8testver;
#endif
            memset(sHbuf->hMemory, 1, 32768);
            /* initialize INT spin lock */
            DDC_ISR_LOCK_INIT(sHbuf->hMutex);
            DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
            
            if (!sHbuf->hMemory)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_SERAIL_IO, DDC_DBG_SERIAL_IO_CREATE_RX_HOST_BUFFER,
                    "Serial IO - serial_ioCreateRxHostBuffer Memory Create Failure \n");
            }
            else
            {
                sHbuf->u32HbufNextRead = 0;
                sHbuf->u32HbufNextWrite = 0;
                sHbuf->u32HbufNumEntries = 0;
                sHbuf->bHbufOverflow = 0;
                sHbuf->bHbufQueueOverflow = 0;
                sHbuf->bHbufStoreTimeTag = FALSE;
		        /*sHbuf->ulIntCount = 0;*/     /* used for interrupt count check */
             
            }
            pDeviceContext->sSerialIORxHBuf.bRxFiFoHbufInstalled[u8Channel] = TRUE;
            DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
        } /* if(sHbuf->hMemory == NULL) */
    }

    return status;
}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioHandleRxHostBufferInterrupt

   Description:
        This function writes messages to the host buffer of an CAN Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  interruptStatus   - interrupt status structure

   Returns:
      None.
---------------------------------------------------------------------------------*/
void serial_ioHandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel,
    U32BIT u32status
)
{
    U32BIT numMessagesToCopy = 0;
    U16BIT numMessagesToRead = 0;
    U8BIT u8MessagesCount = 0;
    U8BIT  u8Data = 0;
    U8BIT u8ValData = 0;
    U8BIT*  pHostBuffer;
    CAST_SERIAL_IO_HBUF_TYPE *sHbuf;
    DDC_IOCTL_PARAMS sIoCtlParams;
    
    sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);
    
    if (sHbuf->hMemory != NULL)
    {
        pHostBuffer = (U8BIT*)sHbuf->hMemory;

        switch (pDeviceContext->castSerialIOChanMatrix[u8Channel].castSerialMode)
        {
            case CAST_PROTOCOL_SDLC:
            {
                sIoCtlParams.Param2 = 0x68; /* MIO_SDLC_RSTAT*/
                sIoCtlParams.Channel = u8Channel;
                u8ValData = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);                            

                if (u8ValData & 0x08)
                {
                    /* printk("u8Data   ");    */
                    while (u8ValData & 0x04)
                    {
                        numMessagesToCopy = SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;

                        sIoCtlParams.Param2 = 0x74;
                        sIoCtlParams.Channel = u8Channel;
                        u8Data = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                       
                        /*----------------------------------------------------------------------------------------------------*/
                        /* Place data in Hostbuffer */
						DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
                        
						serial_ioCopyToHostBuffer(&(pHostBuffer[sHbuf->u32HbufNextWrite]),
                            &u8Data,
                            (U8BIT)numMessagesToCopy);
                        
                        sHbuf->u32HbufNumEntries += numMessagesToCopy;
                        sHbuf->u32HbufNextWrite += (numMessagesToCopy * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE);
                        if (sHbuf->u32HbufNextWrite >= (U32BIT)SERIAL_IO_RX_HBUF_MAX)
                        {
                            sHbuf->u32HbufNextWrite = 0;
                        }
						
						DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
						
                        u8MessagesCount++;
                       /*----------------------------------------------------------------------------------------------------*/
                        
                       /* Check if messages still remain to be read */
                        sIoCtlParams.Param2 = 0x68; /* MIO_SDLC_RSTAT*/
                        sIoCtlParams.Channel = u8Channel;
                        u8ValData = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);                          
                    } /* while(numMessagesToRead > 0) */
                }
         
                /* Receive enable -  Setting this flag enables the receiver and clears ovr, rcabt, ae, crce and rdn bits. */
                /* The controller clears gren after end of receive operation.*/
               
                sIoCtlParams.Param2 = 0x68;  /* MIO_SDLC_RSTAT*/
                sIoCtlParams.Param3 = 0x02; 
                sIoCtlParams.Channel = u8Channel;
                ARINC429CastSerialIORegWrite(pDeviceContext, &sIoCtlParams);    
                        
                break;
            }
            case CAST_PROTOCOL_HDLC:
            {
                /* Did Frame reception complete*/
                /* Read ISTA reg RME BIT 7 */
                sIoCtlParams.Param2 = 0x20;   /* MIO_HDLC_ISTA */
                sIoCtlParams.Channel = u8Channel;
                u8ValData = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                /*  printk(":  ISTA reg check RME BIT 7 and RPF BIT 6  %x \n",  u8ValData);   */
                /* Can it be read from FIFO */           
                /* Read ISTA reg RPF BIT 6 */

                /* Read Regs  RBCH and RBCL */
                /* how much data in buffer */
                sIoCtlParams.Param2 = 0x2D;   /* MIO_HDLC_RBCH */
                sIoCtlParams.Channel = u8Channel;
                numMessagesToRead = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                 /*  printk(":  RBCH reg count  %x \n",  numMessagesToRead); */
                 
                /* read back size of message */   
                sIoCtlParams.Param2 = 0x25;   /* MIO_HDLC_RBCL */
                sIoCtlParams.Channel = u8Channel;
                numMessagesToRead = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                 /* printk(":  RBCL  reg count %x \n",  numMessagesToRead);   */          

                /*  printk("u8Data   ");     */
                while (numMessagesToRead > 0)
                {
                    numMessagesToCopy = SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;

                    sIoCtlParams.Param2 = 0;
                    sIoCtlParams.Channel = u8Channel;
                    u8Data = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                     /* printk("  %x  ", u8Data);   */  
					DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
					 
                    serial_ioCopyToHostBuffer(&(pHostBuffer[sHbuf->u32HbufNextWrite]),
                        &u8Data,
                        (U8BIT)numMessagesToCopy);
                    
                    sHbuf->u32HbufNumEntries += numMessagesToCopy;
                    sHbuf->u32HbufNextWrite += (numMessagesToCopy * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE);
                    if (sHbuf->u32HbufNextWrite >= (U32BIT)SERIAL_IO_RX_HBUF_MAX)
                    {
                        sHbuf->u32HbufNextWrite = 0;
                    }
					
					DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
					
                    u8MessagesCount++;
                   
                    /* CAN'T use ExReleaseFastMutex at dispatch level must be used at APC or Passive level */
                    /* Check if messages still remain to be read */
                    numMessagesToRead--;
                    
                } /* while(numMessagesToRead > 0) */
                 
                /* Check if Valid Frame was received. */
                sIoCtlParams.Param2 = 0x27;   /* MIO_HDLC_RSTA */
                sIoCtlParams.Channel = u8Channel;

                u8ValData = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
                /* printk("\n serial_ioHandleRxHostBufferInterrupt:  u8ValData %x  \n", u8ValData);     */
                break;
            }
            
            default:
            {            
              	/* used for interrupt count check */
                /*  
                    CAST_IO IP (h16550) DDC's original implementation has a maximum FIFO size of 16. 
                    The new CAST_IO IP (h16750) will support a 256 byte FIFO. 
                     We need to detect which version we are using so we can set the proper numMessagesToRead.
                */
                /* need to check capabilities bit here */            
                if (pDeviceContext->sHwVersionInfo.dwCapabilities2  & HWVER_CAPABILITY2_SERIAL_ENHANCED_FIFO)
                {
                    numMessagesToRead = 256;
                }
                else
                {
                    numMessagesToRead = 16;
                }

                while (numMessagesToRead > 0)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_SERAIL_IO, DDC_DBG_SERIAL_IO_RX_HOST_BUFFER,
                        "Serial - Ch %d Msg Read # %d\n", u8Channel, numMessagesToRead);
            
                    DDC_DBG_PRINT(DDC_DBG_MODULE_SERAIL_IO, DDC_DBG_SERIAL_IO_RX_HOST_BUFFER,
                        "numMessagesToRead %d\n", numMessagesToRead);
                    numMessagesToCopy = SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;

                    /* Read UART here for DATA  - Don't need pu32UsbReadBuffer buffer remove it */
                    sIoCtlParams.Param2 = 5;
                    sIoCtlParams.Channel = u8Channel;
                    u8ValData = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);

                    if (u8ValData & 0x01)
                    {
                        sIoCtlParams.Param2 = 0;
                        sIoCtlParams.Channel = u8Channel;
                        u8Data = (U8BIT)ARINC429CastSerialIORegRead(pDeviceContext, &sIoCtlParams);
						
						DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
                        
						serial_ioCopyToHostBuffer(&(pHostBuffer[sHbuf->u32HbufNextWrite]),
                            &u8Data,
                            (U8BIT)numMessagesToCopy);
						
                        sHbuf->u32HbufNumEntries += numMessagesToCopy;
                        sHbuf->u32HbufNextWrite += (numMessagesToCopy * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE);
                        if (sHbuf->u32HbufNextWrite >= (U32BIT)SERIAL_IO_RX_HBUF_MAX)
                        {
                            sHbuf->u32HbufNextWrite = 0;
                        }
						
						DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
                        u8MessagesCount++;
                    }
                    else
                    {
                        /* no more data available break out of loop */
                        break;
                    }
                    /* CAN'T use ExReleaseFastMutex at dispatch level must be used at APC or Passive level */
                    /* Check if messages still remain to be read */
                    numMessagesToRead--;
                }
                
                break;
            }
        }
    }
    
	DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag); 
    /* Common Code */
    pDeviceContext->u32CastIO_bdIntStatus[1] = (U32BIT)u8MessagesCount;
    pDeviceContext->u32CastIO_bdIntStatus[0] = u32status;

    DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
}

#ifdef DDC_OVERFLOW
/*-------------------------------------------------------------------------------
   Function:
       serial_ioCheckHostBufferOverflow

   Description:
        This function checks, whether the next write sequence would create a
        host buffer overflow.

   Parameters:
      In  u32NextRead    - next read position
      In  u32NextWrite   - next write position
      In  u32NumElements - number of elements to copy

   Returns:
      overflow condition.
---------------------------------------------------------------------------------*/
static BOOLEAN serial_ioCheckHostBufferOverflow
(
    U32BIT u32NextRead,
    U32BIT u32NextWrite,
    U32BIT u32NumElements
)
{
    BOOLEAN result = FALSE;

    
    if (u32NextWrite < u32NextRead)
    {
        result = (BOOLEAN)((u32NextWrite + u32NumElements) >= u32NextRead);
    }
    else
    {
        if ((u32NextWrite + u32NumElements) > (U32BIT)SERIAL_IO_RX_HBUF_MAX)
        {
            result = (BOOLEAN)(((u32NumElements - ((U32BIT)SERIAL_IO_RX_HBUF_MAX - u32NextWrite) - 1)) >= u32NextRead);
        }
    }
    return result;
}
#endif
/*-------------------------------------------------------------------------------
   Function:
       serial_ioReadUART

   Description:
        This function reads messages from the host buffer of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u8Channel         - Channel number

   Returns:
      data.
---------------------------------------------------------------------------------*/
U16BIT serial_ioReadUART(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel)
{
    U8BIT* pMessages;
    CAST_SERIAL_IO_HBUF_TYPE *sHbuf;
    /*size_t numBytesToCopy = 0;*/
    U32BIT u32NumMsgsToRead = 1;
    U8BIT u8Data = 0;
       
    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumRS232 ) ||
        (u8Channel > pDeviceContext->u8NumRS485 ) ||
        (u8Channel > pDeviceContext->u8NumUart ))
    {
        return SERIAL_IO_INVALID_CHANNEL;
    }

    sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);

    DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);    
    
    if (sHbuf->hMemory != NULL)
    {
        if (sHbuf->u32HbufNumEntries > 0)
        {
            if (u32NumMsgsToRead > sHbuf->u32HbufNumEntries)
            {
                u32NumMsgsToRead = sHbuf->u32HbufNumEntries;
            }

            /* do we have to read beyond the end of the buffer (wrapped around messages) */
            if ((sHbuf->u32HbufNextRead + (u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE)) > (U32BIT)SERIAL_IO_RX_HBUF_MAX)
            {
                u32NumMsgsToRead = ((U32BIT)SERIAL_IO_RX_HBUF_MAX - sHbuf->u32HbufNextRead) / SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;
            }

            /* get pointer to the starting address for copying of msgs */
            pMessages = (U8BIT*)sHbuf->hMemory;

            /* calculate the number of bytes to copy */
            /*numBytesToCopy = u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;*/
            /*memcpy(&u8Data, ((U8BIT*)(&(pMessages[sHbuf->u32HbufNextRead]))), numBytesToCopy); */
            u8Data = pMessages[sHbuf->u32HbufNextRead];
          
            sHbuf->u32HbufNextRead += (u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE);
            sHbuf->u32HbufNumEntries -= u32NumMsgsToRead;
            if (sHbuf->u32HbufNextRead >= (U32BIT)SERIAL_IO_RX_HBUF_MAX)
            {
                sHbuf->u32HbufNextRead = 0;
            }
        }
        else
        {
           DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
           return 0x8000;
           
        }
    }

    DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
    
    return (U16BIT)u8Data;

}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioReadUART_EXT

   Description:
        This function reads messages from the host buffer of an Serial IO Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u8Channel         - Channel number

   Returns:
      Status
---------------------------------------------------------------------------------*/
U16BIT serial_ioReadUART_EXT(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel, U8BIT *pu8Buff, U32BIT U32BufSize)
{
    U8BIT* pMessages;
    CAST_SERIAL_IO_HBUF_TYPE *sHbuf;
    /*size_t numBytesToCopy = 0;*/
    U32BIT u32NumMsgsToRead = 1;
    U16BIT x = 0;
    
    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumRS232 ) ||
        (u8Channel > pDeviceContext->u8NumRS485 ) ||
        (u8Channel > pDeviceContext->u8NumUart ))
    {
        return SERIAL_IO_INVALID_CHANNEL;
    }

    sHbuf = &(pDeviceContext->sSerialIORxHBuf.sHBuf[u8Channel]);

    if (sHbuf->hMemory != NULL)
    {
		
		
        while ( x < U32BufSize)
        {
            if (sHbuf->u32HbufNumEntries > 0)
            {
				DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag); 
                if (u32NumMsgsToRead > sHbuf->u32HbufNumEntries)
                {
                    u32NumMsgsToRead = sHbuf->u32HbufNumEntries;
                }

                /* do we have to read beyond the end of the buffer (wrapped around messages) */
                if ((sHbuf->u32HbufNextRead + (u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE)) > (U32BIT)SERIAL_IO_RX_HBUF_MAX)
                {
                    u32NumMsgsToRead = ((U32BIT)SERIAL_IO_RX_HBUF_MAX - sHbuf->u32HbufNextRead) / SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;
                }

                /* get pointer to the starting address for copying of msgs */
                pMessages = (U8BIT*)sHbuf->hMemory;

                /* calculate the number of bytes to copy */
                /*numBytesToCopy = u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE;*/
                /*memcpy(&u8Data, ((U8BIT*)(&(pMessages[sHbuf->u32HbufNextRead]))), numBytesToCopy); */
                pu8Buff[x] = pMessages[sHbuf->u32HbufNextRead];
                
                sHbuf->u32HbufNextRead += (u32NumMsgsToRead * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE);
                sHbuf->u32HbufNumEntries -= u32NumMsgsToRead;
                if (sHbuf->u32HbufNextRead >= (U32BIT)SERIAL_IO_RX_HBUF_MAX)
                {
                    sHbuf->u32HbufNextRead = 0;
                }
				DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slSerialIOModeFlag);
            }
            else
            {

               return x; /*0x8000;*/
               
            }
            x ++;
        }
    }

    return (U16BIT)x; 

}

/*-------------------------------------------------------------------------------
   Function:
       serial_ioCopyToHostBuffer

   Description:
        This function copies the shared memory to the host buffer.

   Parameters:
      In  pHostBufferMemory    - host buffer OS memory
      In  pMemory              - pointer to shared memory
      In  u32NumMessagesToRead - number of messages to copy

   Returns:
      None.
---------------------------------------------------------------------------------*/
static void serial_ioCopyToHostBuffer
(
    U8BIT* pHostBufferMemory,
    U8BIT* pMemory,
    U8BIT u32NumMessagesToCopy
)
{
    U32BIT i;

    DDC_DBG_PRINT(DDC_DBG_MODULE_SERAIL_IO, DDC_DBG_SERIAL_IO_COPY_HOST_BUFFER,
        "Copying %d messages to host buffer\n", u32NumMessagesToCopy);

    for (i = 0; i < (u32NumMessagesToCopy * SERIAL_IO_RX_HBUF_MAX_MSG_SIZE); i++)
    {
        /* Copy Serial IO data */
        pHostBufferMemory[i] = pMemory[i];
    }
}

