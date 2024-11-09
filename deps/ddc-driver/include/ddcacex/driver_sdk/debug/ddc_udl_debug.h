/*******************************************************************************
 * FILE: ddc_udl_debug.h
 *
 * DESCRIPTION:
 *
 *  DDC Device Driver debug level definitions.
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

#ifndef _DDC_UDL_DEBUG_H_
#define _DDC_UDL_DEBUG_H_

/* ========================================================================== */
/* MODULE DEFINITIONS                                                         */
/* ========================================================================== */
#define DDC_DBG_MODULE_DRIVER       0
#define DDC_DBG_MODULE_MT           1
#define DDC_DBG_MODULE_DEVICE       2
#define DDC_DBG_MODULE_RT           3
#define DDC_DBG_MODULE_IOCTL        4
#define DDC_DBG_MODULE_UM           5
#define DDC_DBG_MODULE_IMP          6
#define DDC_DBG_MODULE_BOARD        7
#define DDC_DBG_MODULE_1553         8
#define DDC_DBG_MODULE_IRIGB        9
#define DDC_DBG_MODULE_INTERRUPT    10
#define DDC_DBG_MODULE_ARINC717     11
#define DDC_DBG_MODULE_CAN          12
#define DDC_DBG_MODULE_DMA          13
#define DDC_DBG_MODULE_ARINC429     14
#define DDC_DBG_MODULE_IODEV        15
#define DDC_DBG_MODULE_DIOTT        16
#define DDC_DBG_MODULE_BUS          17
#define DDC_DBG_MODULE_BC           18
#define DDC_DBG_MODULE_DIO          19
#define DDC_DBG_MODULE_QUEUE        20
#define DDC_DBG_MODULE_OS_LIB       21
#define DDC_DBG_MODULE_1553_TRIGGER 22
#define DDC_DBG_MODULE_AIO          23
#define DDC_DBG_MODULE_FLASH        24
#define DDC_DBG_MODULE_SERAIL_IO    25

#define DDC_DBG_MODULE_MAX          26


/* ========================================================================== */
/* ALL MODULES                                                                */
/* ========================================================================== */
#define DDC_DBG_NONE                                0x00000000
#define DDC_DBG_ALL                                 0xFFFFFFFF




/* ========================================================================== */
/* DDC_DBG_MODULE_DRIVER                                                      */
/* ========================================================================== */
#define DDC_DBG_DRIVER_DRIVER_ENTRY                 0x00000001
#define DDC_DBG_DRIVER_EVT_DEVICE_CONTEXT_CLEANUP   0x00000002
#define DDC_DBG_DRIVER_DRIVER_EXIT                  0x00000003


/* ========================================================================== */
/* DDC_DBG_MODULE_MT                                                          */
/* ========================================================================== */
#define DDC_DBG_MT_INIT_MODE                        0x00000001L
#define DDC_DBG_MT_GET_TIME_PKT                     0x00000002L
#define DDC_DBG_MT_SET_STATE                        0x00000004L
#define DDC_DBG_MT_OPEN                             0x00000008L
#define DDC_DBG_MT_CLOSE                            0x00000010L
#define DDC_DBG_MT_DATA_WORK_ITEM                   0x00000020L
#define DDC_DBG_MT_DATA_WORK_ITEM_1                 0x00000040L
#define DDC_DBG_MT_TIME_WORK_ITEM                   0x00000080L
#define DDC_DBG_MT_DATA_MEM_READ                    0x00000100L
#define DDC_DBG_MT_RESP_TIME_OUT                    0x00000200L
#define DDC_DBG_MT_CH10_DATA_PKT                    0x00000400L
#define DDC_DBG_MT_INT_HANDLER                      0x00000800L
#define DDC_DBG_MT_TIME_LOOOKASIDE_REMOVE           0x00001000L
#define DDC_DBG_MT_SET_STROBE_REGISTER              0x00002000L
#define DDC_DBG_MT_INT_CONFIG                       0x00004000L
#define DDC_DBG_MT_CH10_STATUS                      0x00008000L
#define DDC_DBG_MT_DEVICE_ADD                       0x00010000L
#define DDC_DBG_MT_GET_METRICS                      0x00020000L
#define DDC_DBG_MT_STORE_TIME_DATA                  0x00040000L


/* ========================================================================== */
/* DDC_DBG_MODULE_DEVICE                                                      */
/* ========================================================================== */
#define DDC_DBG_DEVICE_INIT                         0x00000001
#define DDC_DBG_DEVICE_OPEN                         0x00000002
#define DDC_DBG_DEVICE_CLOSE                        0x00000004
#define DDC_DBG_DEVICE_EXIT                         0x00000008
#define DDC_DBG_DEVICE_DO_EXIT                      0x00000010
#define DDC_DBG_INIT_DMA                            0x00000020
#define DDC_DBG_READ_CAPABILITIES                   0x00000040
#define DDC_DBG_DEVICE_INT_HANDLER_ERR              0x00000080
#define DDC_DBG_DEVICE_NOTIFY                       0x00000100
#define DDC_DBG_DEVICE_MISC                         0x00000200


/* ========================================================================== */
/* DDC_DBG_MODULE_RT                                                          */
/* ========================================================================== */
#define DDC_DBG_RT_INITIALIZE                       0x00000001
#define DDC_DBG_RT_INTERRUPT_HANDLER                0x00000002
#define DDC_DBG_RT_CONFIG_CTRL                      0x00000004
#define DDC_DBG_RT_NOT_USED                         0x00000008
#define DDC_DBG_RT_HBUF                             0x00000010
#define DDC_DBG_RT_HBUF_DETAIL                      0x00000020
#define DDC_DBG_RT_RT_CONFIG_CTRL                   0x00000040
#define DDC_DBG_RT_DATA_STREAMING                   0x00000080
#define DDC_DBG_RT_DATA_ARRAY                       0x00000100
#define DDC_DBG_RT_OPEN                             0x00000200
#define DDC_DBG_RT_CMD_STK                          0x00000400
#define DDC_DBG_RT_RT_OPEN                          0x00000800
#define DDC_DBG_RT_IMP                              0x00001000


/* ========================================================================== */
/* DDC_DBG_MODULE_IOCTL                                                       */
/* ========================================================================== */
#define DDC_DBG_IOCTL_CompletionRoutines            0x00000001
#define DDC_DBG_IOCTL_ReadWriteRoutines             0x00000002
#define DDC_DBG_IOCTL_Ioctl_Level1                  0x00000004
#define DDC_DBG_IOCTL_Ioctl_Level2                  0x00000008
#define DDC_DBG_IOCTL_Ioctl_Level3                  0x00000010
#define DDC_DBG_IOCTL_Ioctl_failures                0x00000020
#define DDC_DBG_IOCTL_CntrlRoutines                 0x00000040


/* ========================================================================== */
/* DDC_DBG_MODULE_UM                                                          */
/* ========================================================================== */
#define DDC_DBG_UM_INIT                             0x00000001
#define DDC_DBG_UM_ENDIAN_TEST                      0x00000002


/* ========================================================================== */
/* DDC_DBG_MODULE_IMP - IMPROVEMENTS 1553                                     */
/* ========================================================================== */
#define DDC_DBG_IMP_INT_SET                         0x00000001
#define DDC_DBG_IMP_POST_QUEUE                      0x00000002
#define DDC_DBG_IMP_INITIALIZE                      0x00000004
#define DDC_DBG_IMP_RT_OPEN                         0x00000008
#define DDC_DBG_IMP_RT_INT_HANDLER                  0x00000010
#define DDC_DBG_IMP_RT_CMD_STK_POINTER              0x00000020


/* ========================================================================== */
/* DDC_DBG_MODULE_BOARD                                                       */
/* ========================================================================== */
#define DDC_DBG_BD_INITIALIZE                       0x00000001L
#define DDC_DBG_BD_INT_SET                          0x00000002L
#define DDC_DBG_BD_OPEN                             0x00000004L
#define DDC_DBG_BD_CLOSE                            0x00000008L
#define DDC_DBG_BD_INT_HNDLR                        0x00000010L
#define DDC_DBG_BD_RESET                            0x00000020L
#define DDC_DBG_BD_INT_CLEAR                        0x00000040L
#define DDC_DBG_BD_TEST                             0x00000080L
#define DDC_DBG_BD_RT_AUTO_BOOT_INIT                0x00000100L
#define DDC_DBG_BD_RT_AUTO_BOOT_RESTORE             0x00000200L
#define DDC_DBG_BD_GET_FEATURE_INFO                 0x00000400L
#define DDC_DBG_BD_READ_CAPABILITIES                0x00000800L

#define DDC_DBG_BD_INIT                             (DDC_DBG_BD_INITIALIZE | DDC_DBG_BD_TEST)


/* ========================================================================== */
/* DDC_DBG_MODULE_1553 - GENERAL 1553                                         */
/* ========================================================================== */
#define DDC_DBG_1553_INIT_MODE                      0x00000001
#define DDC_DBG_1553_RAM_PAR_CHK                    0x00000002
#define DDC_DBG_1553_SET_TT_RES                     0x00000004
#define DDC_DBG_1553_TT_RO_PT                       0x00000008
#define DDC_DBG_1553_INT_CLEAR                      0x00000010
#define DDC_DBG_1553_INT_SET                        0x00000020
#define DDC_DBG_1553_MEM_CLEAR                      0x00000040
#define DDC_DBG_1553_SET_IRQ                        0x00000080
#define DDC_DBG_1553_EXT_TT_CNT_CTRL                0x00000100

#define DDC_DBG_1553_INIT                           DDC_DBG_1553_INIT_MODE


/* ========================================================================== */
/* DDC_DBG_MODULE_IRIGB                                                       */
/* ========================================================================== */
#define DDC_DBG_IRIGB_INIT                          0x00000001
#define DDC_DBG_IRIGB_OPEN                          0x00000002
#define DDC_DBG_IRIGB_INT_SET                       0x00000004


/* ========================================================================== */
/* DDC_DBG_MODULE_INTERRUPT                                                   */
/* ========================================================================== */
#define DDC_DBG_INTERRUPT_INT_CREATE                0x00000001
#define DDC_DBG_INTERRUPT_INT_ENABLE                0x00000002
#define DDC_DBG_INTERRUPT_INT_DISABLE               0x00000004
#define DDC_DBG_INTERRUPT_INT_ISR                   0x00000008
#define DDC_DBG_INTERRUPT_INT_DPC                   0x00000010
#define DDC_DBG_INTERRUPT_DPC                       0x00000020
#define DDC_DBG_BC_INT_WORK_ITEM                    0x00000040
#define DDC_DBG_INTERRUPT_INT_ERROR_DPC             0x00000080
#define DDC_DBG_INTERRUPT_INT_CAN_DPC               0x00000100
#define DDC_DBG_INTERRUPT_INT_429_DPC               0x00000200

#define DDC_DBG_INTERRUPT_INIT                      DDC_DBG_INTERRUPT_INT_CREATE | DDC_DBG_INTERRUPT_INT_ERROR_DPC


/* ========================================================================== */
/* DDC_DBG_MODULE_ARINC717                                                    */
/* ========================================================================== */
#define DDC_DBG_ARINC717_INITIALIZE                 0x00000001
#define DDC_DBG_ARINC717_SET_STATE                  0x00000002
#define DDC_DBG_ARINC717_CONFIG                     0x00000004
#define DDC_DBG_ARINC717_INTERRUPT                  0x00000008
#define DDC_DBG_ARINC717_TX_LOAD                    0x00000010
#define DDC_DBG_ARINC717_CLOSE                      0x00000020
#define DDC_DBG_ARINC717_RX_READ                    0x00000040


/* ========================================================================== */
/* DDC_DBG_MODULE_CAN                                                         */
/* ========================================================================== */
#define DDC_DBG_CANBUS_INITIALIZE                   0x00000001
#define DDC_DBG_CANBUS_SET_STATE                    0x00000002
#define DDC_DBG_CANBUS_CONFIG                       0x00000004
#define DDC_DBG_CANBUS_INTERRUPT                    0x00000008
#define DDC_DBG_CANBUS_TRANSMIT                     0x00000010
#define DDC_DBG_CANBUS_CLOSE                        0x00000020
#define DDC_DBG_CANBUS_RECEIVE                      0x00000040
#define DDC_DBG_CANBUS_RX_HBUF_METRICS              0x00000080
#define DDC_DBG_CANBUS_CREATE_RX_HOST_BUFFER        0x00000100
#define DDC_DBG_CANBUS_RX_HOST_BUFFER               0x00000200
#define DDC_DBG_CANBUS_COPY_HOST_BUFFER             0x00000400


/* ========================================================================== */
/* DDC_DBG_MODULE_SERAIL_IO                                                       */
/* ========================================================================== */
#define DDC_DBG_SERIAL_IO_CREATE_RX_HOST_BUFFER     0x00000001
#define DDC_DBG_SERIAL_IO_RX_HOST_BUFFER            0x00000002
#define DDC_DBG_SERIAL_IO_COPY_HOST_BUFFER          0x00000004

/* ========================================================================== */
/* DDC_DBG_MODULE_DMA                                                         */
/* ========================================================================== */
#define DDC_DBG_DMA_INITIALIZE                      0x00000001L
#define DDC_DBG_DMA_EXIT                            0x00000002L
#define DDC_DBG_DMA_START                           0x00000004L
#define DDC_DBG_DMA_EXEC_DMA                        0x00000008L
#define DDC_DBG_DMA_EXECUTE_WORK_ITEM               0x00000010L
#define DDC_DBG_DMA_COMPLETE_WORK_ITEM              0x00000020L
#define DDC_DBG_DMA_PROG_DMA                        0x00000040L
#define DDC_DBG_DMA_QUEUE_INIT                      0x00000080L
#define DDC_DBG_DMA_QUEUE_FREE                      0x00000100L
#define DDC_DBG_DMA_QUEUE_INSERT                    0x00000200L
#define DDC_DBG_DMA_QUEUE_ADD                       0x00000400L
#define DDC_DBG_DMA_GET_NUM_ELEM                    0x00000800L
#define DDC_DBG_DMA_QUEUE_REMOVE                    0x00001000L
#define DDC_DBG_DMA_QUEUE_EXPAND                    0x00002000L
#define DDC_DBG_DMA_QUEUE_START                     0x00004000L
#define DDC_DBG_DMA_EXEC_WORK_ITEM                  0x00008000L
#define DDC_DBG_DMA_COMPLETE_WORK_ITEM_ERROR        0x00010000L
#define DDC_DBG_DMA_RX_FIFO                         0x00020000L
#define DDC_DBG_DMA_TX_FRAME                        0x00040000L
#define DDC_DBG_DMA_VOLTAGE_MON                     0x00080000L
#define DDC_DBG_DMA_DIO_TT                          0x00100000L
#define DDC_DBG_DMA_BC                              0x00200000L
#define DDC_DBG_DMA_RT                              0x00400000L
#define DDC_DBG_DMA_MT                              0x00800000L
#define DDC_DBG_DMA_REPLAY                          0x01000000L
#define DDC_DBG_DMA_717_RX                          0x02000000L
#define DDC_DBG_DMA_717_TX                          0x04000000L
#define DDC_DBG_DMA_CONTINUE_PCIE                   0x08000000L

#define DDC_DBG_DMA_INIT                            DDC_DBG_DMA_COMPLETE_WORK_ITEM_ERROR


/* ========================================================================== */
/* DDC_DBG_MODULE_ARINC429                                                    */
/* ========================================================================== */
#define DDC_DBG_ARINC429_INITIALIZE                 0x00000001
#define DDC_DBG_ARINC429_TRANSLATE_MEMORY_ADDR      0x00000001
#define DDC_DBG_ARINC429_READ_MEMORY                0x00000002
#define DDC_DBG_ARINC429_WRITE_MEMORY               0x00000004
#define DDC_DBG_ARINC429_TRANSLATE_REG_ADDR         0x00000008
#define DDC_DBG_ARINC429_READ_REGISTER              0x00000010
#define DDC_DBG_ARINC429_WRITE_REGISTER             0x00000020
#define DDC_DBG_ARINC429_CAPABILITIES               0x00000040
#define DDC_DBG_ARINC429_COMMAND_PROCESS            0x00000080
#define DDC_DBG_ARINC429_LOOPBACK                   0x00000100
#define DDC_DBG_ARINC429_COMMAND_RX                 0x00000004
#define DDC_DBG_ARINC429_COMMAND_FILTER             0x00000008
#define DDC_DBG_ARINC429_COMMAND_MAILBOX            0x00000010
#define DDC_DBG_ARINC429_COMMAND_FIFO               0x00000020
#define DDC_DBG_ARINC429_COMMAND_GENERAL            0x00000040
#define DDC_DBG_ARINC429_COMMAND_CONTROL            0x00000080
#define DDC_DBG_ARINC429_COMMAND_TX                 0x00000100

#define DDC_DBG_ARINC429_INTERRUPT                  0x00000200
#define DDC_DBG_ARINC429_TIMETAG                    0x00000400
#define DDC_DBG_ARINC429_LOADTXQUEUEMORE            0x00000800
#define DDC_DBG_ARINC429_LOADTXQUEUEONE             0x00001000
#define DDC_DBG_ARINC429_READ_REGISTER_UART         0x00002000
#define DDC_DBG_ARINC429_WRITE_REGISTER_UART        0x00004000
#define DDC_DBG_ARINC429_RX_HOST_BUFFER             0x00008000

#define DDC_DBG_ARINC429_COMMAND_TESTER             0x00010000
#define DDC_DBG_ARINC429_SERIAL_IO                  0x00020000
#define DDC_DBG_ARINC429_GETRXQUEUESTATUS           0x00040000

#define DDC_DBG_ARINC429_REGISTERS                  DDC_DBG_ARINC429_TRANSLATE_REG_ADDR | DDC_DBG_ARINC429_READ_REGISTER | DDC_DBG_ARINC429_WRITE_REGISTER
#define DDC_DBG_ARINC429_MEMORY                     DDC_DBG_ARINC429_TRANSLATE_MEMORY_ADDR | DDC_DBG_ARINC429_READ_MEMORY | DDC_DBG_ARINC429_WRITE_MEMORY


/* ========================================================================== */
/* DDC_DBG_MODULE_IODEV - I/O DEVICE                                          */
/* ========================================================================== */
#define DDC_DBG_IODEVICE_UPDATE_QPRM                0x00000001
#define DDC_DBG_IODEVICE_BINDING_FROM_CPLD          0x00000002
#define DDC_DBG_IODEVICE_BINDING_FROM_QPRM          0x00000004


/* ========================================================================== */
/* DDC_DBG_MODULE_DIOTT                                                       */
/* ========================================================================== */
#define DDC_DBG_DIOTT_INITIALIZE                    0x00000001
#define DDC_DBG_DIOTT_CONFIG                        0x00000002
#define DDC_DBG_DIOTT_INTERRUPT                     0x00000004
#define DDC_DBG_DIOTT_FREE                          0x00000008
#define DDC_DBG_DIOTT_READ                          0x00000010


/* ========================================================================== */
/* DDC_DBG_MODULE_BUS                                                         */
/* ========================================================================== */
#define DDC_DBG_BUS_FLASH_ERASE                     0x00000001
#define DDC_DBG_BUS_REG_READ                        0x00000002
#define DDC_DBG_BUS_REG_READ_BLK                    0x00000004
#define DDC_DBG_BUS_REG_WRITE                       0x00000008
#define DDC_DBG_BUS_REG_WRITE_BLK                   0x00000010
#define DDC_DBG_BUS_MEM_READ                        0x00000040
#define DDC_DBG_BUS_MEM_READ_BLK                    0x00000080
#define DDC_DBG_BUS_MEM_READ_BLK_16                 0x00000100
#define DDC_DBG_BUS_MEM_WRITE                       0x00000200
#define DDC_DBG_BUS_MEM_WRITE_BLK                   0x00000400
#define DDC_DBG_BUS_MEM_WRITE_BLK_16                0x00000800
#define DDC_DBG_BUS_PLX_READ                        0x00001000
#define DDC_DBG_BUS_PLX_WRITE                       0x00002000
#define DDC_DBG_BUS_DMA_READ                        0x00004000
#define DDC_DBG_BUS_DMA_WRITE                       0x00008000
#define DDC_DBG_BUS_FLASH_READ                      0x00010000
#define DDC_DBG_BUS_INIT                            0x00010000
#define DDC_DBG_BUS_MAP_ADDRESS                     0x00020000
#define DDC_DBG_BUS_EXIT                            0x00040000
#define DDC_DBG_BUS_IRQ                             0x00080000


/* ========================================================================== */
/* DDC_DBG_MODULE_BC                                                          */
/* ========================================================================== */
#define DDC_DBG_BC_INTERRUPT_HANDLER                0x00000001
#define DDC_DBG_BC_IMP_INTERRUPT_HANDLER            0x00000002
#define DDC_DBG_BC_IMP_MSG_PROCESS                  0x00000004
#define DDC_DBG_BC_INTERRUPT_SET                    0x00000008
#define DDC_DBG_BC_INTERRUPT_CLEAR                  0x00000010
#define DDC_DBG_BC_IMP_INTERRUPT_SET                0x00000020
#define DDC_DBG_BC_IMP_INTERRUPT_CLEAR              0x00000040
#define DDC_DBG_BC_IMP_START                        0x00000080
#define DDC_DBG_BC_IMP_OPEN                         0x00000100
#define DDC_DBG_BC_REG_E                            0x00000200
#define DDC_DBG_BC_SET_INT_FOR_BUF_OP               0x00000400
#define DDC_DBG_BC_HBUF_ENABLE_ACTION               0x00000800
#define DDC_DBG_BC_HBUF_DISABLE_ACTION              0x00001000
#define DDC_DBG_BC_MSG_BUF_ENABLE_ACTION            0x00002000
#define DDC_DBG_BC_MSG_BUF_FREE                     0x00004000
#define DDC_DBG_BC_TEMP_BUF                         0x00008000
#define DDC_DBG_BC_HBUF                             0x00010000
#define DDC_DBG_BC_METRIC                           0x00020000
#define DDC_DBG_BC_MSG                              0x00040000
#define DDC_DBG_BC_DATA                             0x00080000
#define DDC_DBG_BC_GPQ                              0x00100000
#define DDC_DBG_BC_ASYNC                            0x00200000
#define DDC_DBG_BC_REPLAY                           0x00400000
#define DDC_DBG_BC_OPEN                             0x00800000
#define DDC_DBG_BC_CLOSE                            0x01000000
#define DDC_DBG_BC_INIT                             0x02000000
#define DDC_DBG_BC_CMD_STACK                        0x04000000
#define DDC_DBG_BC_DATA_STACK                       0x08000000
#define DDC_DBG_BC_SET_STATE                        0x10000000


/* ========================================================================== */
/* DDC_DBG_MODULE_DIO                                                         */
/* ========================================================================== */
#define DDC_DBG_DIO_GET_OUTPUT                      0x00000001
#define DDC_DBG_DIO_GET_INPUT                       0x00000002
#define DDC_DBG_DIO_GET_ALL                         0x00000004
#define DDC_DBG_DIO_SET_OUTPUT                      0x00000008
#define DDC_DBG_DIO_SET_DIRECTION                   0x00000010
#define DDC_DBG_DIO_SET_ALL                         0x00000020


/* ========================================================================== */
/* DDC_DBG_MODULE_QUEUE                                                       */
/* ========================================================================== */
#define DDC_DBG_QUEUE_SHOW                          0x00000001
#define DDC_DBG_QUEUE_DESTROY                       0x00000002
#define DDC_DBG_QUEUE_GET_TAIL                      0x00000004
#define DDC_DBG_QUEUE_ADD                           0x00000010
#define DDC_DBG_QUEUE_REMOVE                        0x00000020


/* ========================================================================== */
/* DDC_DBG_MODULE_OS_LIB                                                      */
/* ========================================================================== */
#define DDC_DBG_OS_LIB_GET_TICK_COUNT               0x00000001


/* ========================================================================== */
/* DDC_DBG_MODULE_1553_TRIGGER - 1553 TRIGGER                                 */
/* ========================================================================== */
#define DDC_DBG_1553_TRIGGER_STATUS_WRITE           0x00000001
#define DDC_DBG_1553_TRIGGER_STATUS_READ            0x00000002


/* ========================================================================== */
/* DDC_DBG_MODULE_AIO                                                         */
/* ========================================================================== */
#define DDC_DBG_AIO_GET_OUTPUT                      0x00000001
#define DDC_DBG_AIO_GET_DIRECTION                   0x00000002
#define DDC_DBG_AIO_GET_INPUT                       0x00000004
#define DDC_DBG_AIO_GET_ALL                         0x00000008
#define DDC_DBG_AIO_SET_OUTPUT                      0x00000010
#define DDC_DBG_AIO_SET_DIRECTION                   0x00000020
#define DDC_DBG_AIO_SET_ALL                         0x00000040


/* ========================================================================== */
/* DDC_DBG_MODULE_FLASH                                                       */
/* ========================================================================== */
#define DDC_DBG_FLASH_BLOCK_ERASE                   0x00000001
#define DDC_DBG_FLASH_MEM_READ                      0x00000002
#define DDC_DBG_FLASH_MEM_WRITE                     0x00000004
#define DDC_DBG_FLASH_MEM_WRITE_PROTECTED           0x00000008


#endif  /* _DDC_UDL_DEBUG_H_ */
