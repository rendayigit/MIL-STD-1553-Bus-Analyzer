/*******************************************************************************
 * FILE: ddc_udl_1553_bc.h
 *
 * DESCRIPTION:
 *
 *  This module provides interface to the Bus Controller (BC) hardware and the
 *  BC Run-Time Library.
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

#ifndef _DDC_UDL_1553_BC_H_
#define _DDC_UDL_1553_BC_H_

#include "os/include/ddc_os_types.h"


/*----------------------------------------------------------------------------*/
/* This structure contains BC General Purpose Queue entry                     */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_GPQ_ENTRY
{
    U32BIT u32GPQHeader;    /* gives information on what data is    */
    U32BIT u32GPQData;      /* the information for the entry        */
    
} ACEX_BC_GPQ_ENTRY;


/*-----------------------------------------------------------------------
Struct:  ACEX_BC_MUX_INFO

Description: This structure contains BC Mux Initialization Information.
------------------------------------------------------------------------*/
typedef struct _ACEX_BC_MUX_INFO
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32MuxEnable;
    
} ACEX_BC_MUX_INFO;


/*----------------------------------------------------------------------------*/
/* This structure contains BC Host Buffer Metrics                             */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_HBUF_METRIC
{
    U32BIT u32Count;   /* number of msgs in host buffer        */
    U32BIT u32Lost;    /* Host buffer overflow, lost msg count */
    U32BIT u32PctFull; /* Current Percentage of HBuf used      */
    U32BIT u32HighPct; /* Highest Percentage of HBuf used      */
    
} ACEX_BC_HBUF_METRIC;


/*----------------------------------------------------------------------------*/
/* This structure contains BC General Purpose Queue Metrics                   */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_GPQ_METRIC
{
    U32BIT u32Lost;    /* GPQ overflow, lost msg count     */
    U16BIT u16PctFull; /* Current Percentage of GPQ used   */
    U16BIT u16HighPct; /* Highest Percentage of GPQ used   */
    
} ACEX_BC_GPQ_METRIC;


/*-----------------------------------------------------------------------
Struct:  ACEX_BC_REPLAY_INFO

Description: This structure contains BC Queue Initialization Information.
------------------------------------------------------------------------*/
typedef struct _ACEX_BC_REPLAY_INFO
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32BaseAddr;
    U32BIT u32MemSize;
    
} ACEX_BC_REPLAY_INFO;


/*-----------------------------------------------------------------------
Struct:  ACEX_BC_REPLAY_STATUS
Description: This structure contains BC Replay status information.
------------------------------------------------------------------------*/
typedef struct _ACEX_BC_REPLAY_STATUS
{
    U32BIT u32BaseAddr;         /* base memory address */
    U32BIT u32MemSize;          /* memory size */
    
} ACEX_BC_REPLAY_STATUS;


/*----------------------------------------------------------------------------*/
/* This structure contains BC data array status Information                   */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_DATA_ARRAY_STATUS
{
    U16BIT u16MsgToPostAvail;   /* the available number of msgs to post             */
    U16BIT u16MsgErrLocation;   /* the location of the first message with errors    */
    U16BIT u16MsgErrBsw;        /* the BSW of the first message with errors         */
    
} ACEX_BC_DATA_ARRAY_STATUS;


/*----------------------------------------------------------------------------*/
/* This structure contains BC data stream status Information                  */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_DATASTR_STATUS
{
    BOOLEAN bComplete;          /* set if the data stream is completed              */
    U16BIT u16MsgErrLocation;   /* the location of the first message with errors    */
    U16BIT u16MsgErrBsw;        /* the BSW of the first message with errors         */
    
} ACEX_BC_DATASTR_STATUS;


/*----------------------------------------------------------------------------*/
/* This structure contains BC Queue Initialization Information                */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_QUEUE_INFO
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32BaseAddr;
    U32BIT u32QueueSize;
    
} ACEX_BC_QUEUE_INFO;


/*----------------------------------------------------------------------------*/
/* This structure contains BC Async message queue information                 */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_ASYNC_QUEUE_INFO
{
    U16BIT u16HwQueueCount;     /* Hardware queue count */
    U16BIT u16QueuedMsgCount;   /* queued message count */
    
} ACEX_BC_ASYNC_QUEUE_INFO;


/*----------------------------------------------------------------------------*/
/* This structure contains BC Host Buffer Uninstall Information               */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_HBUF_UNINSTALL_INFO
{
    ACEX_CONFIG_ID sConfigID;
    
} ACEX_BC_HBUF_UNINSTALL_INFO;


/*----------------------------------------------------------------------------*/
/* This structure contains BC Host Buffer Install Information                 */
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_BC_HBUF_INSTALL_INFO
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32BufferSize;
    
} ACEX_BC_HBUF_INSTALL_INFO;


#endif /* _DDC_UDL_1553_BC_H_ */
