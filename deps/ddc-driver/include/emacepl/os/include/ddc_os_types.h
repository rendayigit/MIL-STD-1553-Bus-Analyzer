/*******************************************************************************
 * FILE: ddc_os_types.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide OS specific definitions for common
 *  types.
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

#ifndef _DDC_OS_TYPES_H_
#define _DDC_OS_TYPES_H_

#define MODULE_NAME                         "acexpci"

#define DDC_SAMPLES_FILE_IO                 1
#define DDC_SAMPLE_USER_INPUT               1
#define PSUMFILE                            1

typedef void                                VOID;
typedef void *                              PVOID;
typedef void *                              LPVOID;

typedef unsigned char                       BOOLEAN;
typedef unsigned char                       BOOL;

typedef char                                CHAR;

typedef signed char                         S8BIT;
typedef unsigned char                       U8BIT;

typedef signed short                        S16BIT;
typedef unsigned short                      U16BIT;

typedef signed int                          S32BIT;
typedef unsigned int                        U32BIT;

typedef signed long long                    S64BIT;
typedef unsigned long long                  U64BIT;

#define UNREFERENCED_PARAMETER(P)
#define DDC_FOPEN                           fopen


/* FILE *, for test logging. */
#ifndef DDC_STDOUT
#define DDC_STDOUT stdout
#endif

/* Make it possible to replace fprintf with another
 * function with the same interface. */
#ifndef DDC_FPRINTF
#define DDC_FPRINTF fprintf
#endif

#define DDC_HANDLE                          int  
#define DDC_LPSECURITY_ATTRIBUTES           void *
#define DDC_LOCAL                           static

#define DDC_DMA_ADDR                        dma_addr_t
#define DDC_EVENT                           wait_queue_head_t

#define DDC_THREAD_HANDLE                   pthread_t *
#define DDC_THREAD_ID                       pthread_t

#define FUNCPTR                             void *

#define DDC_SECURITY_ATTRIBUTES             void *

/* used with MAKE_STATUS() macro */
#define DDC_STATUS_SEVERITY_ERROR           0x3
#define DDC_FACILITY_IO_ERROR_CODE          0x4

#define DDC_OFFSET                          long

/* ioctl semaphore. not used in Linux */
#define DDC_IOCTL_SEMAPHORE                 U32BIT
#define DDC_IOCTL_CREATE_BIN_SEMAPHORE(x)   (*x=0)

#define DDC_PACKED __attribute__((__packed__))

/* Mutex Definitions */
#define DDC_MUTEX                           pthread_mutex_t *
#define DDC_MUTEX_ATTR                      pthread_mutexattr_t
#define DDC_MUTEX_CREATE(mutex, x, y, z)    mutex = ddcMutexCreate(x)
#define DDC_MUTEX_DESTROY(x)                {pthread_mutex_destroy(x); DDC_FREE(x); x = NULL; }
#define DDC_MUTEX_WAIT_FOREVER(x)           pthread_mutex_lock(x)
#define DDC_MUTEX_RELEASE(x)                pthread_mutex_unlock(x)
#define DDC_MUTEX_VALID_CHECK(x)            (TRUE)

#define DDC_CONDITION                       pthread_cond_t
#define DDC_CONDITION_DESTROY(x)            pthread_cond_destroy(&x)
#define DDC_CONDITION_INIT(mutex, x)        pthread_cond_init(&mutex, NULL)
#define DDC_CONDITION_WAIT(cond, mutex)     pthread_cond_wait(&cond, mutex)
#define DDC_CONDITION_SIGNAL(x)             pthread_cond_signal(&x)

#define DDC_VIRT_TO_PHYS(pDeviceContext, virtualAddr, physicalAddr)     *physicalAddr = virt_to_phys(virtualAddr)

#define BYTES_ALLOCATED_CH10DATAPKT         0X10000

#define DEFAULT_ROLLOVER_TIMEOUT            0  

/* Events */
#define DDC_CALLBACK                        U32BIT
#define DDC_INIT_WAITQUEUE_IRQ(call_back, event) \
    init_waitqueue_head(event)
#define DDC_INIT_WAITQUEUE(event) \
    init_waitqueue_head(event)

#define DDC_WAIT_INTERRUPTABLE(event, cond) \
    wait_event_interruptible(event, cond)

#define DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(event, cond, timeout) \
    wait_event_interruptible_timeout(event, cond, msecs_to_jiffies(timeout))

#define DDC_WAKE_UP_INTERRUPTIBLE(call_back, event) \
    wake_up_interruptible(event)

#define DDC_WAKE_UP(event) \
    wake_up_interruptible(event)

/* RTL malloc/free macro */
#ifndef STATIC
   #define DDC_MALLOC(size)                 malloc(size)
   #define DDC_FREE(ptr)                    free(ptr)
#else
   #define DDC_MALLOC(size)                 DDC_Malloc(size)
   #define DDC_FREE(ptr)                    DDC_Free(ptr)
#endif /* STATIC */

#define DDC_UNREFERENCED_PARAMETER(p)       (void)p
#define DDC_PPC                             (defined (PPC) || defined(__powerpc__))
#define DDC_ARM                             (defined(__arm__) || defined(__aarch64__))

/* Device Types */
typedef enum _DDC_PHY_DEV_TYPE
{
    DDC_PHY_DEV_NONE = 0,

    DDC_PHY_DEV_EMAPCI,
    DDC_PHY_DEV_E2MAPCI,
    DDC_PHY_DEV_ACEXPCI,
    DDC_PHY_DEV_ACEXUSB,
    DDC_PHY_DEV_429X

} DDC_PHY_DEV_TYPE;


#define DDC_MAX_DEVICE_NAME_LEN             32

/* Device Information Table */
typedef struct _DDCCM_DEVICE_TABLE
{
    char deviceName[DDC_MAX_DEVICE_NAME_LEN];
    char deviceNameIo[DDC_MAX_DEVICE_NAME_LEN];
    DDC_PHY_DEV_TYPE deviceType;
    unsigned int familyNumber;

    char szModelName[32];
    unsigned char boardModel;
    unsigned int pciBus;
    unsigned int pciDevice;
    unsigned int deviceId;
    unsigned int fwVerMaj;
    unsigned int fwVerMin;
    unsigned int u32RecFwVer;
    unsigned char card;
    unsigned char channel;
    unsigned char bFirmwareMismatch;
    unsigned char bSectorCount;
    char bEepMismatch;
    unsigned int u32RemoteDevice;
    unsigned int u32IpAddr;
    unsigned int u32RemoteLDN;

} DDCCM_DEVICE_TABLE, *PDDCCM_DEVICE_TABLE;


#define DDC_MAX_NUM_DEVICES                         32

typedef struct _DDCCM_DEVICE_TABLES_TYPE
{
    /* device number count (also indicates # of items in the associated device table) */
    U16BIT ddc1553NumDevs;
    U16BIT ddc429NumDevs;

#ifndef LINUX
    /* global device tables  */
    DDCCM_DEVICE_TABLE ddccm1553DeviceTable[DDC_MAX_NUM_DEVICES];
    DDCCM_DEVICE_TABLE ddccm429DeviceTable[DDC_MAX_NUM_DEVICES];
#endif /* not defined LINUX */

} DDCCM_DEVICE_TABLES_TYPE;

enum ddc_dma_data_direction
{
    DDC_DMA_BIDIRECTIONAL = 0,
    DDC_DMA_TO_DEVICE = 1,
    DDC_DMA_FROM_DEVICE = 2,
    DDC_DMA_NONE = 3
};

#define DDC_IRQ_NONE        0
#define DDC_IRQ_HANDLED     1

#if DDC_PPC

#define DDC_INLINE
#define IO_BARRIER iobarrier_rw()
#define IO_SYNC iosync()

#else

#define DDC_INLINE inline
#define IO_SYNC
#define IO_BARRIER

#endif

#define DDC_FILE_NAME_TYPE_MIL_STD_1553     'M'     /* MIL-STD-1553 */
#define DDC_FILE_NAME_TYPE_ARINC_429        'A'     /* ARINC 429    */
#define DDC_FILE_NAME_TYPE_IO               'I'     /* I/O          */



#define DDC_UDL_DEVICE_RESOURCE_NAME_LEN    16

/* ========================================================================== */
/* acexConfiguration Items                                                    */
/* ========================================================================== */
#define TASK_PRIORITY_MIN                   1
#define TASK_PRIORITY_MAX                   254

/* Linux kernels below version 2.6.25 have a maximum of 0x20000 (131072) bytes limit for kmalloc function */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)
#define DDC_UDL_OS_ARINC_RX_HBUF_SIZE       0x0C000
#else
#define DDC_UDL_OS_ARINC_RX_HBUF_SIZE       0x02000
#endif /* Linux kernel >= 2.6.25 */


#endif /* _DDC_OS_TYPES_H_ */
