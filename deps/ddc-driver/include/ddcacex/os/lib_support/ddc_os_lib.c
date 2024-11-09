/*******************************************************************************
 * FILE: ddc_os_lib.c
 *
 * DESCRIPTION:
 *
 *  This file defines the common functions used by DDC OS-specific applications
 *  and libraries. The routines contained herein are to be used to determine
 *  the configuration of a device. These functions are not to be used by a
 *  driver.
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
#include <stdlib.h>
#include <stdio.h> /* for FILE */
#include <sys/time.h> /* For 'GetTickCount()' */
#include <sys/ioctl.h>
#include <pthread.h>

#include "os/lib_support/ddc_os_lib.h"
#include "include/ddc_ioctl.h"

/* 
  DDC_REALTIME_EXT_LIB
    
    Updated functions to use clock_gettime with CLOCK_MONOTONIC_RAW 
    which is available only on some Linux kernels,  Similar to CLOCK_MONOTONIC, 
    but provides access to a raw hardware-based time that is not subject to NTP adjustments.

    Added #defined DDC_REALTIME_EXT_LIB for access to new updated functionality. 
    Default is off or 0, set to 1 for access to updated function. 

    When DDC_REALTIME_EXT_LIB is TRUE or 1 with glibc versions 2.16 and below the
    -lrt linking option must be added to the samples and ddccm Makefiles to link real time extension lib.
 
*/
#define DDC_REALTIME_EXT_LIB  0

#if DDC_REALTIME_EXT_LIB
static struct timespec tvEpoc = { 0, 0 }; /* Initialize epoc to zero */
#else
static struct timeval tvEpoc = { 0, 0 }; /* Initialize epoc to zero */
#endif
/*******************************************************************************
 * Name:    _config_file_lookup
 *
 * Description:
 *      This function parses the defined configuration file for the desired
 *      logical device number, and updates all passed in parameters with the
 *      values specified within the file if a matching entry is found. If
 *      no entry is found, all parameters are NULL'ed.
 *
 * In   Card
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
int _config_file_lookup
(
    char *sConfigFile,
    unsigned int dwLogNum,
    unsigned int *pdwBusType,
    unsigned int *pdwDevID,
    unsigned int *pdwBusNum,
    unsigned int *pdwSlotNum,
    unsigned int *pdwChanNum,
    unsigned int *pdwFamily,
    unsigned int *pdwRemoteDev,
    unsigned int *pdwIpAddr,
    unsigned int *pdwRemoteLDN,
    char *sDriverName,
    char *sIODriverName
)
{
    char sTempStr[256];
    unsigned int dwTemp1;
    int dwReturn = 0;
    FILE *hDdcConf;

    /* Open the config file */
    if (!(hDdcConf = fopen(sConfigFile, "r")))
    {
        return DDC_OS_CONFIG_FILE_OPEN_FAIL;
    }

    /* Priming read of first config file entry */
    fscanf(hDdcConf, "%u", &dwTemp1);

    /* While desired logical number is not found... */
    while (dwLogNum != dwTemp1)
    {
        /* Discard the rest of this line        */
        /* If NULL is returned, we are at EOF   */
        if (NULL == fgets(sTempStr, 255, hDdcConf))
        {
            fclose(hDdcConf);
            return DDC_OS_DEVICE_NOT_EXIST;
        }

        /* Parse the next line if present.  Don't error */
        /* check here since the 'fgets()' above will    */
        /* determine if another line is present.        */
        fscanf(hDdcConf, "%u", &dwTemp1);
    }

    /* If we made it to here, we found an entry with the desired logical number */

    /* Read the rest of the line with error checking */
    if ((1 != fscanf(hDdcConf, "%x", pdwBusType)) ||        /* Bus type         */
        (1 != fscanf(hDdcConf, "%x", pdwDevID)) ||          /* Device ID        */
        (1 != fscanf(hDdcConf, "%x", pdwBusNum)) ||         /* Bus number       */
        (1 != fscanf(hDdcConf, "%x", pdwSlotNum)) ||        /* Slot number      */
        (1 != fscanf(hDdcConf, "%x", pdwChanNum)) ||        /* Channel number   */
        (1 != fscanf(hDdcConf, "%x", pdwFamily)) ||         /* Family Type      */
        (1 != fscanf(hDdcConf, "%x", pdwRemoteDev)) ||      /* Remove Device    */
        (1 != fscanf(hDdcConf, "%x", pdwIpAddr)) ||         /* IP Address (hex) */
        (1 != fscanf(hDdcConf, "%x", pdwRemoteLDN)) ||      /* Remote LDN       */
        (1 != fscanf(hDdcConf, "%s", sDriverName)) ||       /* Driver Handle    */
        (1 != fscanf(hDdcConf, "%s", sIODriverName)))       /* IODriver Handle  */
    {
        dwReturn = DDC_OS_CONFIG_FILE_READ_FAIL;
    }

    /* Close the config file */
    fclose(hDdcConf);

    /* If there were any file read errors, NULL out parameters and return error */
    if (dwReturn)
    {
        *pdwBusType = 0;
        *pdwDevID = 0;
        *pdwBusNum = 0;
        *pdwSlotNum = 0;
        *pdwChanNum = 0;
        *pdwFamily = 0;
        *pdwIpAddr = 0;
        *pdwRemoteDev = 0;
        *pdwRemoteLDN = 0;
        *sDriverName = 0;
        *sIODriverName = 0;

        return dwReturn;
    }

    return DDC_SUCCESS;
}

/*******************************************************************************
 * Name:    _epoc
 *
 * Description:
 *      Sets the 'epoc' of use of the library to 'now'. Does nothing for subsequent
 *      calls made from within the same process.  This function MUST be called
 *      before using 'GetTickCount()'. Typically, this could be done from within
 *      a device initialization call. Note, that this routine could block for up to
 *      one second waiting for a rollover.
 *
 * In   none
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void _epoc
(
    void
)
{
#if DDC_REALTIME_EXT_LIB
    long dwRollOver;
#else    
    struct timezone tzCurrent;
    unsigned long dwRollOver;    
#endif
    /* If the epoc has not yet been set, loop until the microsecond     */
    /* field has overflowed in order to bring it to a negligable value  */
    if (!tvEpoc.tv_sec)
    {
#if DDC_REALTIME_EXT_LIB
        do
        {
            dwRollOver = tvEpoc.tv_nsec;
            clock_gettime(CLOCK_MONOTONIC_RAW, &tvEpoc);

        } while (tvEpoc.tv_nsec > dwRollOver);
#else
        do
        {
            dwRollOver = tvEpoc.tv_usec;
            gettimeofday(&tvEpoc, &tzCurrent);
        } while (tvEpoc.tv_usec > dwRollOver);
#endif        
    }
}

/*******************************************************************************
 * Name:    GetTickCount
 *
 * Description:
 *      Returns the number of milliseconds that elapsed since the first call to
 *     '_epoc()'. The roll over of the epoc is 49.7 days, if that matters.
 *     This could be a problem if a process is to exist for that long and a
 *     call to this function is made before and after the rollover for time
 *     comparison purposes. '_epoc()' MUST be called before using this function.
 *
 * In   Card
 * Out  none
 *
 * Returns: double word representing milliseconds since 'epoc'
 ******************************************************************************/
unsigned long GetTickCount
(
    void
)
{
	
	unsigned long ulValue = 0;
	
#if DDC_REALTIME_EXT_LIB
    struct timespec tvCurrent;
#else    
    struct timeval tvCurrent;  /* Keep these local to provide   */
    struct timezone tzCurrent; /* multithreading compatability  */
#endif
    /* If epoc has not been set, print error and return huge value */
    if (!tvEpoc.tv_sec)
    {
        printf("\n<ddclinux>: GetTickCount() called without epoc ");
        printf("initialization.  A call to '_epoc()' must be made");
        printf(" before 'GetTickCount()'.\n");

        return 0xFFFFFFFF;
    }

#if DDC_REALTIME_EXT_LIB
    /* Get the current time of day */
    clock_gettime(CLOCK_MONOTONIC_RAW, &tvCurrent);

    /* Return (seconds*1000) + (microseconds/1000) */
    return ((tvCurrent.tv_sec - tvEpoc.tv_sec) * 1000) + (unsigned long)(tvCurrent.tv_nsec / 1000000);
#else
     /* Get the current time of day */
    gettimeofday(&tvCurrent, &tzCurrent);

    /* Return (seconds*1000) + (microseconds/1000) */
    ulValue = ((tvCurrent.tv_sec - tvEpoc.tv_sec) * 1000) + (unsigned long)(tvCurrent.tv_usec / 1000);
/*  printf("tv_sec %lx tvepoc %lx tv_usec %lx \n",tvCurrent.tv_sec, tvEpoc.tv_sec, tvCurrent.tv_usec);*/
    if (tvCurrent.tv_sec != tvEpoc.tv_sec)
    {
		tvEpoc.tv_sec = 0;	
		/*printf("calling epoc \n");*/
		_epoc();
        /* Get the current time of day */
        gettimeofday(&tvCurrent, &tzCurrent);

		/* Return (seconds*1000) + (microseconds/1000) */
		ulValue = ((tvCurrent.tv_sec - tvEpoc.tv_sec) * 1000) + (unsigned long)(tvCurrent.tv_usec / 1000);
		/*printf("tv_sec %lx tvepoc %lx tv_usec %lx \n",tvCurrent.tv_sec, tvEpoc.tv_sec, tvCurrent.tv_usec);*/
    }    

    return ulValue; 
#endif
}

/*-----------------------------------------------------------------------------
*  Name:    ddcMutexCreate
*
*   Description
*       This function create the mutex on the run.
*
*   In     attr = atributes of the mutex
*   Out    return = the point to the mutex if success; NULL if fail
-----------------------------------------------------------------------------*/
DDC_MUTEX ddcMutexCreate
(
    DDC_MUTEX_ATTR *attr
)
/*---------------------------------------------------------------------------*/
{
    DDC_MUTEX pmutex = NULL;
    int result = 0;

    pmutex = (DDC_MUTEX)malloc(sizeof(pthread_mutex_t));
    if (pmutex == NULL)
    {
        return NULL;
    }

    result = pthread_mutex_init(pmutex, attr);
    if (result)
    {
        free(pmutex);
        return NULL;
    }
    else
    {
        return pmutex;
    }
}

/*******************************************************************************
 * Name:    ddcDeviceIoControl
 *
 * Description:
 * This function send ioctl command to driver with the giving information.
 *
 * In   hDriver                                 DDC_HANDLE to the driver
 * In   pIoctlDev                               pointer to the IOCTL Device
 * In   u32IoControlCode                        IOCTL command
 * In   pDataBuffer_InBuffer                    pointer to the input buffer
 * In   u32BufferLength_InBuffer                input buffer length in bytes
 * In   pDataBuffer_ReturnStatus                pointer to the return status of command
 * In   u32BufferLength_ReturnStatus            return status of command length in bytes
 * In   pDataBuffer_OutBufferBytesReturned      pointer to the # of bytes returned in output buffer
 * In   u32BufferLength_OutBufferBytesReturned  # of bytes returned in output buffer length in bytes
 * In   pDataBuffer_InOutBuffer                 pointer to the output or additional input buffer (optional)
 * In   u32BufferLength_InOutBuffer             output or additional input buffer length in bytes (optional)
 *
 * Returns: TRUE = success, FALSE = failure
 ******************************************************************************/
S16BIT ddcDeviceIoControl
(
    DDC_HANDLE hDriver,
    DDC_IOCTL_DEV *pIoctlDev,
    U32BIT u32IoControlCode,
    void *pDataBuffer_InBuffer,
    U32BIT u32BufferLength_InBuffer,
    void *pDataBuffer_ReturnStatus,
    U32BIT u32BufferLength_ReturnStatus,
    void *pDataBuffer_OutBufferBytesReturned,
    U32BIT u32BufferLength_OutBufferBytesReturned,
    void *pDataBuffer_InOutBuffer,
    U32BIT u32BufferLength_InOutBuffer
)
{
    DDC_IOCTL_DATA_BUFFERS_TYPE sIoctlDataBuffers;
    int status;

    /* make sure the required data items are valid */
    if ((pDataBuffer_InBuffer == NULL) ||
        (pDataBuffer_ReturnStatus == NULL) ||
        (pDataBuffer_OutBufferBytesReturned == NULL) ||
        (u32BufferLength_InBuffer == 0) ||
        (u32BufferLength_ReturnStatus == 0) ||
        (u32BufferLength_OutBufferBytesReturned == 0))
    {
        /* printf("ERROR: ddcDeviceIoControl - NULL or zero length parameter\n"); */
        return FALSE;
    }

    /* if the In/Out buffer is used, make sure the buffer length is valid */
    if (pDataBuffer_InOutBuffer != NULL)
    {
        if (u32BufferLength_InOutBuffer == 0)
        {
            /* printf("ERROR: ddcDeviceIoControl - InOutBuffer NULL or zero length\n"); */
            return FALSE;
        }
    }

    /* fill the DDC_IOCTL_DATA_BUFFERS_TYPE structure */
    sIoctlDataBuffers.sInBuffer.pDataBuffer = (unsigned long)pDataBuffer_InBuffer;
    sIoctlDataBuffers.sInBuffer.u32BufferLength = u32BufferLength_InBuffer;

    sIoctlDataBuffers.sReturnStatus.pDataBuffer = (unsigned long)pDataBuffer_ReturnStatus;
    sIoctlDataBuffers.sReturnStatus.u32BufferLength = u32BufferLength_ReturnStatus;

    sIoctlDataBuffers.sOutBufferBytesReturned.pDataBuffer = (unsigned long)pDataBuffer_OutBufferBytesReturned;
    sIoctlDataBuffers.sOutBufferBytesReturned.u32BufferLength = u32BufferLength_OutBufferBytesReturned;

    sIoctlDataBuffers.sInOutBuffer.pDataBuffer = (unsigned long)pDataBuffer_InOutBuffer;
    sIoctlDataBuffers.sInOutBuffer.u32BufferLength = u32BufferLength_InOutBuffer;

    /* send the ioctl command to driver */
    status = ioctl(hDriver, u32IoControlCode, &sIoctlDataBuffers);

    if (status)
    {
        /* jz--todo -- to return the status ? */
        return FALSE;
    }

    return TRUE;
}


/*******************************************************************************
 * Name:    ddcWaitForThreadTermination
 *
 * Description:
 *      This function waits for a thread to exit.
 *
 * In   ddcThread       thread to wait for
 *
 * Returns: none
 ******************************************************************************/
void ddcWaitForThreadTermination
(
    DDC_THREAD_HANDLE ddcThread
)
{
    pthread_join(*ddcThread, NULL);
}

/*******************************************************************************
 * Name:    ddcWaitForThreadTermination
 *
 * Description:
 *      This function does nothing.
 *
 * In   Void 
 *
 * Returns: FALSE
 ******************************************************************************/
extern S16BIT GetLastError
(
    void
)
{
    return DDC_OS_ERROR;
}

/*******************************************************************************
 * Name:    ddc_bchpwait
 *
 * Description:
 *
 *      BC high priority wait value, only used in _aceBCSendAsyncMsgHP
 *      to wait a specified amount of time between start and end of message.
 *
 * In   None 
 * Out  Wait value
 ******************************************************************************/
int ddc_bchpwait
(
    void
)
{
    return 100;
}

