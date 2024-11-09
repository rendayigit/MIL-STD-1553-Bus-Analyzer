/*******************************************************************************
 * FILE: ddc_os_lib.h
 *
 * DESCRIPTION:
 *
 *  This file defines the common functions used by DDC Linux applications
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

#ifndef _DDC_OS_LIB_H_
#define _DDC_OS_LIB_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "include/ddc_types.h"
#include "os/include/ddc_os_types.h"

#define CONFIGFILE_1553                         "/usr/lib/ddc/ddc1553ex.cfg"
#define CONFIGFILE_429                          "/usr/lib/ddc/ddc429ex.cfg"

#define DDC_SUCCESS                             0
#define DDC_OS_DEVICE_NOT_EXIST                 -1
#define DDC_OS_CONFIG_FILE_OPEN_FAIL            -2
#define DDC_OS_CONFIG_FILE_READ_FAIL            -3

#define DDC_DEVICE_NOT_EXIST                    (int)-415
#define DDC_DEVICE_NOT_OPEN                     (int)-416
#define DDC_DEVICE_INVALID                      (int)-418
#define DDC_DEVICE_IS_CLOSING                   (int)-419

#define DDC_OS_ERROR                            -999

#define IRQ_STATUS_QUE_LEN                      128     /* Size of interrupt status queue */

typedef struct _OSCONTEXT
{
    DDC_HANDLE hDriverApp;
    pthread_t thIntHandler;   /* Interrupt thread */
    U8BIT bThParam;           /* Thread parameter */
    S16BIT wThRetVal;         /* Dispatch thread return value (heap) */
    
    DDC_IOCTL_SEMAPHORE ioctlDispatchSemaphore;   /* used for synchronization between library and ISR */
    DDC_IOCTL_SEMAPHORE intInterruptProcSemaphore;
} OSCONTEXT;

typedef struct _IOINFO
{
    U16BIT bInitialized;      /* Initialization status */
    OSCONTEXT *pContext;      /* OS specific information */
    U32BIT dwCardType;        /* The card being accessed */
    DDC_HANDLE hDriverLib;    /* Driver handle for library */
    CHANCOUNT_t strChanCount; /* structure that holds number of IO's */

} IOINFO_t, *IOINFO_p;

typedef struct _DDC_IOCTL_DEV
{
    S16BIT s16DevNum;
    DDC_IOCTL_SEMAPHORE semIoctlDispatch;
    DDC_IOCTL_SEMAPHORE semIoctlInterrupt;
} DDC_IOCTL_DEV;

extern int _config_file_lookup
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
);

extern void _epoc
(
    void
);

extern unsigned long GetTickCount
(
    void
);

extern DDC_MUTEX ddcMutexCreate
(
    DDC_MUTEX_ATTR *attr
);

extern S16BIT ddcDeviceIoControl
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
);

extern void ddcWaitForThreadTermination
(
    DDC_THREAD_HANDLE ddcThread
);


extern S16BIT GetLastError
(
    void
);

int ddc_bchpwait
(
    void
);

#endif /* _DDC_OS_LIB_H_ */
