/*******************************************************************************
 * FILE: structr.h
 *
 * DESCRIPTION:
 *
 *      TODO
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

#ifndef __STRUCT_R_H__
#define __STRUCT_R_H__

#include "os/include/ddc_os_types.h"

#define DDC_REMOTE_DEVICES_MAX_NUM 32

#define DDC_CLIENT_REQ_THREAD_MAX_NUM 10

typedef struct _DDC_CLIENT_REQ_INFO
{
    BOOLEAN bWaiting;                   /* TRUE - the thread is waiting for the response msg */
    U32BIT u32ExpectCmd;                /* The expected msg cmd the thread is waiting for */
    U8BIT  *pRespMsg;                   /* pointer to the location where the thread needs to store the response msg
                                           note that this storage is allocated at the user thread*/

    BOOLEAN bClntReqEvent;              /* flag of client request event mutext */
    DDC_CONDITION condClntReqEvent;     /* client request event condition */
    DDC_MUTEX mutexClntReqEvent;        /* client request event mutext */

}DDC_CLIENT_REQ_INFO, *PDDC_CLIENT_REQ_INFO;

typedef struct _DDC_RT_DATA_ARRAY_INFO
{
    U32BIT u32BufWdSize;  /* store the buffer size information (in words) for current data array in remoteAcexMRTDataArrayCreate()
                          and used later in remoteAcexMRTDataArraySend() */
}DDC_RT_DATA_ARRAY_INFO, *PDDC_RT_DATA_ARRAY_INFO;

typedef struct _DDC_RT_INFO
{
    DDC_RT_DATA_ARRAY_INFO asDataArrayInfo[4];
}DDC_RT_INFO, *PDDC_RT_INFO;

typedef struct _DDC_REMOTE_DEVICE_INFO
{
    char            aRemoteAddr[20];
    S16BIT          s16RemoteDevNum;
    S32BIT          s32RemoteOsType;    /* Informs server type of OS Remote is running on */
    U16BIT          u16ThParam;

    DDC_MUTEX       hClientReqInfoMutex;
    int             nClntSockInfoMutexUseCount;
    DDC_CLIENT_REQ_INFO clntReqInfo[DDC_CLIENT_REQ_THREAD_MAX_NUM];

    DDC_SOCKET      ConnectSocket;
    DDC_THREAD_HANDLE      hRespDispatcherThread;
    DDC_THREAD_ID   u32RespDispatcherThreadID;
    BOOLEAN         bClientMsgDispatcherUp;

    DDC_SOCKET      IrqListenSock;
    DDC_THREAD_HANDLE      hIrqListenThread;
    DDC_THREAD_ID          hIrqListenThreadID;
    
    BOOLEAN         bIrqListenerUp;

    DDC_THREAD_HANDLE      hIrqProcThread;
    DDC_THREAD_ID          hIrqProcThreadID;
    BOOLEAN         bIrqProcUp;
    
    U32BIT         u32InterruptQueHead;  /* Head of interrupt pending queue */
    U32BIT         u32InterruptQueTail;  /* Tail of interrupt pending queue */
    U32BIT         *pu32IrqStatus;       /* Array of interrupt status(s) for IQS */

    BOOLEAN         bIrqStatusEvent;     /* flag of client request event mutext */
    DDC_CONDITION   condIrqStatusEvent;  /* IRQ status Event condition */
    DDC_MUTEX       mutexIrqStatusEvent; /* IRQ status Event mutext */

    void      (_DECL *funcExternalIsr)(S16BIT DevNum, U32BIT dwIrqStatus);

    DDC_RT_INFO asRtInfo[32];
    U32BIT u32IrqListenThreadID;
    U32BIT u32IrqProcThreadID;
}DDC_REMOTE_DEVICE_INFO, *PDDC_REMOTE_DEVICE_INFO;

#endif /* ifndef __STRUCT_R_H__ */
