/*******************************************************************************
 * FILE: errordef.h
 *
 * DESCRIPTION:
 *
 *  RTL error definitions header file.
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

#ifndef __ERRORDEF_H__
#define __ERRORDEF_H__

/* Success Errors */
#define ACE_ERR_SUCCESS                             0       /* No error occurred */

#define ACE_ERR_MTI_EOB                             1       /* End of MTi data buffer reached (aceMTIDecodeRawMsg) */

#define ACE_ERR_TIMEOUT                             50      /* A timeout occurred */
#define ACE_ERR_DATA_UNAVAILABLE                    51      /* Requested data unavailable */
#define ACE_ERR_BUFFER_OVERFLOW                     52      /* Data Exceeds Data Buffer Size */
#define ACE_ERR_BUFFER_UNAVAILABLE                  53      /* Data Buffer Unavailable */
#define ACE_ERR_BUFFER_BUSY                         54      /* Data Buffer In Use By Hardware */

/* Success Errors Test */
#define ACE_SUCCESS(Error)   (Error >= 0)

/* Config/Access Errors */
#define ACE_ERR_INVALID_FILE                        -49     /* Bad File path, name, or type */
#define ACE_ERR_INVALID_DEVNUM                      -50     /* Bad Card number */
#define ACE_ERR_INVALID_ACCESS                      -51     /* Bad Access type */
#define ACE_ERR_INVALID_MODE                        -52     /* Bad Mode type */
#define ACE_ERR_INVALID_STATE                       -53     /* Bad device state */
#define ACE_ERR_INVALID_MEMSIZE                     -54     /* Bad memory wrd size */
#define ACE_ERR_INVALID_ADDRESS                     -55     /* Bad mem/reg address */
#define ACE_ERR_INVALID_OS                          -56     /* Bad operating system */
#define ACE_ERR_INVALID_MALLOC                      -57     /* Bad sim/win32 mem alloc */
#define ACE_ERR_INVALID_BUF                         -58     /* Bad buffer */
#define ACE_ERR_INVALID_ADMODE                      -59     /* Bad addressing mode */
#define ACE_ERR_SIMWRITEREG                         -60     /* Bad simulated write reg */
#define ACE_ERR_TIMETAG_RES                         -61     /* Bad time tag resolution */
#define ACE_ERR_RESPTIME                            -62     /* Bad response timeout value*/
#define ACE_ERR_CLOCKIN                             -63     /* Bad clock input value */
#define ACE_ERR_MSGSTRUCT                           -64     /* Bad message structure */
#define ACE_ERR_PARAMETER                           -65     /* Bad parameter */
#define ACE_ERR_INVALID_MODE_OP                     -66     /* Invalid Mode Options */
#define ACE_ERR_METRICS_NOT_ENA                     -67     /* Metrics not enabled */
#define ACE_ERR_NOT_SUPPORTED                       -68     /* Function not supported */
#define ACE_ERR_ISQ_DISABLED                        -69     /* ISQ needs to be enabled for this func. */
#define ACE_ERR_TASK_FAIL                           -70     /* Task priority change failed */
#define ACE_ERR_CANADDR                             -71     /* Invalid CAN Address for BU-6558x cards */
#define ACE_ERR_DIO                                 -72     /* Function should be DIO to perform action not HUB */
#define ACE_ERR_OVERFLOW                            -73     /* An overflow condition occurred */
#define ACE_ERR_INITIALIZED                         -74     /* Channel already initialized */
#define ACE_ERR_INVALID_SIZE                        -75     /* The specified size is outside of the allowable sizes */
#define ACE_ERR_OPERATION                           -76     /* The requested operation failed */
#define ACE_ERR_FREE_RESOURCE                       -77     /* cannot free resource */
#define ACE_ERR_ALLOC_RESOURCE                      -78     /* cannot allocate resource */
#define ACE_ERR_TT_ROLLOVER_POINT                   -79     /* Bad time tag rollover point */
#define ACE_ERR_REG_ACCESS                          -80     /* Unable to access registry */
#define ACE_ERR_INVALID_CARD                        -81     /* Not a valid card */
#define ACE_ERR_DRIVER_OPEN                         -82     /* Unable to open device driver */
#define ACE_ERR_MAPMEM_ACC                          -83     /* Error Accessing mapped memory */
#define ACE_ERR_FUNCTION_DISABLED                   -84     /* Function disabled */

#define ACE_ERR_OBJECT_INUSE                        -90     /* Object cannot be deleted when referenced by another object */

/* Link list errors */
#define ACE_ERR_NODE_NOT_FOUND                      -100    /* Could not find node in DLIST*/
#define ACE_ERR_NODE_MEMBLOCK                       -101    /* Node was not a memory block*/
#define ACE_ERR_NODE_EXISTS                         -102    /* Node ID is already in list */

/* Memory management errors */
#define ACE_ERR_MEMMGR_FAIL                         -150    /* Could not alloc mem block */

/* Test mode errors */
#define ACE_ERR_TEST_BADSTRUCT                      -200    /* Bad test result structure */
#define ACE_ERR_TEST_FILE                           -201    /* Not a valid file */

/* MT mode errors */
#define ACE_ERR_MT_BUFTYPE                          -300    /* Not a valid buffer mode */
#define ACE_ERR_MT_CMDSTK                           -301    /* Not a valid cmd stack size */
#define ACE_ERR_MT_DATASTK                          -302    /* Not a valid data stack size */
#define ACE_ERR_MT_FILTER_RT                        -303    /* Not a valid RT address */
#define ACE_ERR_MT_FILTER_TR                        -304    /* Not a valid TR bit */
#define ACE_ERR_MT_FILTER_SA                        -305    /* Not a valid SA buffer */
#define ACE_ERR_MT_STKLOC                           -306    /* Bad MT stack location */
#define ACE_ERR_MT_MSGLOC                           -307    /* Bad MT msg location */
#define ACE_ERR_MT_HBUFSIZE                         -308    /* Bad HBuf size */
#define ACE_ERR_MT_HBUF                             -309    /* HBuf not allocated */
#define ACE_ERR_RTMT_COMBO_HBUF                     -310    /* RTMT Host Buffer used */
#define ACE_ERR_RTMT_HBUFSIZE                       -311    /* Bad RTMT HBuf size */
#define ACE_ERR_RTMT_HBUF                           -312    /* RTMT HBuf not allocated */
#define ACE_ERR_RTMT_MSGLOC                         -313    /* Bad RTMT msg location */

/* RT mode errors */
#define ACE_ERR_RT_DBLK_EXISTS                      -400    /* data block already exists */
#define ACE_ERR_RT_OBJECT_ALLOC                     -401    /* rt object alloc failed */
#define ACE_ERR_RT_DBLK_ALLOC                       -401    /* data block alloc failed */
#define ACE_ERR_RT_STREAM_ALLOC                     -401    /* data stream item alloc failed */
#define ACE_ERR_RT_DATA_ARRAY_ALLOC                 -401    /* data array item alloc failed */
#define ACE_ERR_RT_DBLK_MAPPED                      -402    /* data block is linked to SA */
#define ACE_ERR_RT_DBLK_NOT_CB                      -403    /* data block is not a circ buf */
#define ACE_ERR_RT_HBUF                             -410    /* HBuf not allocated */
#define ACE_ERR_MRT_CONFIG_FAILURE                  -411
#define ACE_ERR_RT_STREAM_INVALID_SA_MASK           -412    /* illegal SA Mask detected */
#define ACE_ERR_RT_STREAM_USRBUFSIZE                -413    /* user passed a buffer larger than stream MTU size */
#define ACE_ERR_RT_STREAM_DOES_NOT_EXIST            -414    /* requested stream does not exist */
#define ACE_ERR_RT_DATA_ARRAY_DOES_NOT_EXIST        -415
#define ACE_ERR_RT_STREAM_RX_ERROR                  -416
#define ACE_ERR_RT_STREAM_TX_ERROR                  -417
#define ACE_ERR_RT_INVALID_EI_ERROR                 -418
#define ACE_ERR_RT_ADDRESS_PARITY_ERROR             -419    /* TT1554 */

/* RT mode warnings */
#define ACE_WRN_RT_CFG_INVALID                      400     /* Config possibly detrimental to card performance */

/* BC mode errors */
#define ACE_ERR_BC_OBJECT_EXISTS                    -500    /* bc object already exists */
#define ACE_ERR_BC_DBLK_EXISTS                      -500    /* data block already exists */
#define ACE_ERR_BC_MSG_EXISTS                       -500    /* message already exists */
#define ACE_ERR_BC_SLOT_ITEM_EXISTS                 -500    /* slot item already exists */
#define ACE_ERR_BC_STREAM_EXISTS                    -500    /* data stream already exists */
#define ACE_ERR_BC_DATA_ARRAY_EXISTS                -500    /* data array already exists */
#define ACE_ERR_BC_MEMWORD_ITEM_EXISTS              -500    /* memory dword item already exists */
#define ACE_ERR_BC_MEMOBJ_ITEM_EXISTS               -500    /* memory object item already exists */
#define ACE_ERR_BC_OBJECT_ALLOC                     -501    /* bc object alloc failed */
#define ACE_ERR_BC_DBLK_ALLOC                       -501    /* data block alloc failed */
#define ACE_ERR_BC_MSG_ALLOC                        -501    /* message alloc failed */
#define ACE_ERR_BC_SLOT_ITEM_ALLOC                  -501    /* time slot item alloc failed */
#define ACE_ERR_BC_STREAM_ALLOC                     -501    /* data stream item alloc failed */
#define ACE_ERR_BC_DATA_ARRAY_ALLOC                 -501    /* data array item alloc failed */
#define ACE_ERR_BC_MEMWORD_ITEM_ALLOC               -501    /* memory dword item alloc failed */
#define ACE_ERR_BC_MEMOBJ_ITEM_ALLOC                -501    /* memory object item alloc failed */
#define ACE_ERR_BC_DBLK_SIZE                        -502    /* data block size incorrect */
#define ACE_ERR_UNRES_DATABLK                       -503    /* data block not found */
#define ACE_ERR_UNRES_MSGBLK                        -504    /* msg block not found */
#define ACE_ERR_UNRES_FRAME                         -505    /* frame block not found */
#define ACE_ERR_UNRES_OPCODE                        -506    /* opcode block not found */
#define ACE_ERR_UNRES_JUMP                          -507    /* jump address is out of frame */
#define ACE_ERR_FRAME_NOT_MAJOR                     -508    /* Frame is not a major frame */
#define ACE_ERR_NOT_ASYNC_MODE                      -509    /* Not is asynchronous mode */
#define ACE_ERR_UNRES_ASYNC_OP                      -510    /* async opcode block not found */
#define ACE_ERR_UNRES_ASYNC_ID                      -511    /* async msg id not found */
#define ACE_ERR_ASYNC_NOT_EMPTY                     -512    /* async list not empty */
#define ACE_ERR_ASYNC_MSG                           -513    /* Error sending async messages */
#define ACE_ERR_BC_DBLK_INVALID                     -514    /* data block cannot be used in this context */
#define ACE_ERR_BC_DATA_ARRAY_IN_USE                -515    /* data array is in use */
#define ACE_ERR_BC_DATA_ARRAY_MSG_BUSY              -516    /* data array async queue messages in use */
#define ACE_ERR_BC_DATA_ARRAY_COMPLETE              -517    /* data array transmission has completed */
#define ACE_ERR_BC_UNRES_MEMOBJECT                  -518    /* memory object not found */
#define ACE_ERR_BC_MEMOBJECT_INVALID                -519    /* memory object invalid */
#define ACE_ERR_BC_INVALID_EI_ERROR                 -520    /* invalid error injection error type */

/* BC mode warnings */
#define ACE_WRN_BC_OPCODE_INVALID                   500     /* Op Code Could cause detrimental performance */

/* Host buffer errors */
#define ACE_ERR_HBUFSIZE                            -600    /* HBuf size incompatible */
#define ACE_ERR_HBUF                                -601    /* HBuf not allocated */

/* Discrete IO errors */
#define ACE_ERR_DISCRETE                            -650    /* Invalid discrete bit specified */
#define ACE_ERR_DLEVEL                              -651    /* Invalid discrete level specified */

/* Avionic discrete IO errors */
#define ACE_ERR_AVIONIC                             -750    /* Invalid avionicdiscrete bit specified */
#define ACE_ERR_ALEVEL                              -751    /* Invalid discrete level specified */

/* VxWorks errors */
#define ACE_ERR_TOO_MANY_DEVS                       -700    /* max number of devs reached */
#define MTI_ERR_EMPTY_QUEUE                         -701
#define ACE_ERR_MEMMGR_MIN_BLK_SIZE                 -702
#define MTI_ERR_STATE                               -703
#define ACE_ERR_DMA_CHNL_REG                        -704    /* Channel registration error */
#define ACE_ERR_DMA_QCALLOC                         -705    /* queue allocation failure */
#define ACE_ERR_DMA_REMOVE_CHANNEL                  -706    /* error in request to remove channel */
#define ACE_ERR_DMA_BUFFER_LENGTH                   -707    /* Error in reading from device */
#define ACE_ERR_DMA_Q                               -708
#define ACE_ERR_DMA_ZERO_BUFFER                     -709

/* MTI ERRORS */
#define ACE_ERR_MTI_PACKET_INVALID                  -710
#define ACE_ERR_MTI_PACKET_EMPTY                    -711

/* INTEGRITY errors */
#define ACE_RESOURCE_REQ_ERR                        -800    /* Error in connecting to device */
#define ACE_IOWRITE_ERR                             -801    /* Error in writing to device */
#define ACE_IOREAD_ERR                              -802    /* Error in reading from device */
#define ACE_MEM_MAP_ERR                             -803    /* Error mapping/umapping phys to virt */
#define ACE_TASK_SPAWN_ERR                          -804    /* Error in creating and running task */

/* MTI CH10 FILE ERRORS */
#define ACE_ERR_MTI_CH10_FILE_INVALID_MODE          -850    /* invalid file access mode */
#define ACE_ERR_MTI_CH10_FILE_INVALID_STATE         -851    /* invalid file state */
#define ACE_ERR_MTI_CH10_FILE_INVALID_HANDLE        -852    /* invalid file handle */
#define ACE_ERR_MTI_CH10_FILE_DATA_PKT_TOO_SMALL    -853    /* data packet size too small */
#define ACE_ERR_MTI_CH10_FILE_INVALID_PKT           -854    /* invalid packet */
#define ACE_ERR_MTI_CH10_FILE_NO_MORE_PKTS          -855    /* no more packets */

/* socket errors */
#define ACE_ERR_SOCK_CLOSED                         -1001
#define ACE_ERR_SOCK_TIMEOUT                        -1002
#define ACE_ERR_SOCK_ERROR                          -1003
#define ACE_ERR_SOCK_MSG_ALLOC_FAIL                 -1004
#define ACE_ERR_SOCK_MSG_ERROR                      -1005
#define ACE_ERR_SOCK_MSG_NO_SPACE                   -1006
#define ACE_ERR_SOCK_MSG_SEND_FAIL                  -1007
#define ACE_ERR_SOCK_MSG_RECV_FAIL                  -1008
#define ACE_ERR_SOCK_FILE_OPEN_FAIL                 -1009
#define ACE_ERR_SOCK_INVALID                        -1010

/* This function returns information describing a particular error */
_EXTERN S16BIT _DECL aceErrorStr
(
    S16BIT nError,
    char *pBuffer,
    U16BIT wBufSize
);

#endif /* __ERRORDEF_H__ */
