/*******************************************************************************
 * FILE: ddc_udl_arinc429.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support ARINC 429.
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

#ifndef _DDC_UDL_ARINC429_H_
#define _DDC_UDL_ARINC429_H_

#include "os/include/ddc_os_types.h"

#define MAX_NUM_429_CHANNELS            36

/* ========================================================================== */
/* ARINC 429 programmable configuration options                               */
/* ========================================================================== */
#define ARINC_429_PROGRMMABLE_ENABLE_OPT        0x00000001
#define ARINC_429_PROGRMMABLE_TYPE_OPT          0x00000002
#define ARINC_429_PROGRMMABLE_SPEED_OPT         0x00000008
#define ARINC_429_PROGRMMABLE_PARITY_OPT        0x00000008
#define ARINC_429_PROGRMMABLE_BIT_FORMAT_OPT    0x00000010
#define ARINC_429_PROGRMMABLE_MODE_OPT          0x00000020
#define ARINC_429_PROGRMMABLE_RESET_OPT         0x00000040
#define ARINC_429_PROGRMMABLE_ENABLE_TX_PASSIVE_OPT 0x00000080


typedef struct _ARINC_429_PROGRMMABLE_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32ConfigOption;
    U8BIT u8Channel;
    U8BIT bEnable;
    U8BIT bType;
    U8BIT u8Speed;
    U8BIT u8Parity;
    U8BIT u8BitFormat;
    U8BIT u8Mode;
    U8BIT bReset;

} ARINC_429_PROGRMMABLE_CONFIG, *PARINC_429_PROGRMMABLE_CONFIG;


/* ========================================================================== */
/* Option parameters for API function ModifyRepeatedData                      */
/* ========================================================================== */
#define ARINC_429_MODIFY_REPEATED_VIA_LABEL     0x0000001
#define ARINC_429_MODIFY_REPEATED_VIA_SDI_LABEL 0x0000002


/* ========================================================================== */
/* RX Control Bit Offsets                                                     */
/* ========================================================================== */

#define DD429_RX_CONTROL_BIT_OFFSET_WRAP_AROUND             0
#define DD429_RX_CONTROL_BIT_OFFSET_IGNORE_LABEL            1
#define DD429_RX_CONTROL_BIT_OFFSET_RX_FULL                 2   /* to 3 */
#define DD429_RX_CONTROL_BIT_OFFSET_MODE                    4
#define DD429_RX_CONTROL_BIT_OFFSET_TIMETAG                 5
#define DD429_RX_CONTROL_BIT_OFFSET_LABEL_AUTO_CLEAR        6
#define DD429_RX_CONTROL_BIT_OFFSET_PARITY                  7
#define DD429_RX_CONTROL_BIT_OFFSET_SPEED                   9
#define DD429_RX_CONTROL_BIT_OFFSET_RSVD1                   10
#define DD429_RX_CONTROL_BIT_OFFSET_BIT_FORMAT              11
#define DD429_RX_CONTROL_BIT_OFFSET_DATA_TRANSFER           12
#define DD429_RX_CONTROL_BIT_OFFSET_RESET                   13
#define DD429_RX_CONTROL_BIT_OFFSET_RSVD2                   14
#define DD429_RX_CONTROL_BIT_OFFSET_RSVD3                   15  /*to 24 */
#define DD429_RX_CONTROL_BIT_OFFSET_FILTER_PARITY_ERR       25
#define DD429_RX_CONTROL_BIT_OFFSET_FILTER_DATA             26
#define DD429_RX_CONTROL_BIT_OFFSET_FILTER_CODE             27  /* to 28 */
#define DD429_RX_CONTROL_BIT_OFFSET_ARINC575                29
#define DD429_RX_CONTROL_BIT_OFFSET_RSVD4                   30
#define DD429_RX_CONTROL_BIT_OFFSET_RSVD5                   31
#define DD429_RX_CONTROL_BIT_OFFSET_MAX                     32

/* ========================================================================== */
/* IOCTL_DD429X_COMMAND Sub-Commands                                          */
/*                                                                            */
/* Passed to driver via Param1                                                */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* Command Start Indexes                                                      */
/* -------------------------------------------------------------------------- */
#define DD429_IO_COMMAND__INDEX_START                       0
#define DD429_GENERAL_COMMAND__INDEX_START                  100
#define DD429_CONTROL_COMMAND__INDEX_START                  200
#define DD429_FIFO_COMMAND__INDEX_START                     300
#define DD429_RX_COMMAND__INDEX_START                       400
#define DD429_TX_COMMAND__INDEX_START                       500
#define DD429_RX_CONTROL_COMMAND_GET__INDEX_START           600
#define DD429_RX_CONTROL_COMMAND_SET__INDEX_START           700
#define DD429_FILTER_COMMAND__INDEX_START                   800
#define DD429_MAILBOX_COMMAND__INDEX_START                  900
#define DD429_TESTER_COMMAND__INDEX_START                   1000

/* -------------------------------------------------------------------------- */
/* I/O Commands                                                               */
/* -------------------------------------------------------------------------- */

#define DD429_IO_COMMAND__REG_READ_32                       (DD429_IO_COMMAND__INDEX_START + 0)
#define DD429_IO_COMMAND__REG_WRITE_32                      (DD429_IO_COMMAND__INDEX_START + 1)
#define DD429_IO_COMMAND__MEM_READ_32                       (DD429_IO_COMMAND__INDEX_START + 2)
#define DD429_IO_COMMAND__MEM_WRITE_32                      (DD429_IO_COMMAND__INDEX_START + 3)

#define DD429_IO_COMMAND__INDEX_END                         (DD429_IO_COMMAND__INDEX_START + 3) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* General Commands                                                           */
/* -------------------------------------------------------------------------- */

#define DD429_GENERAL_COMMAND__RESET                        (DD429_GENERAL_COMMAND__INDEX_START + 0)
#define DD429_GENERAL_COMMAND__SET_BIT_FORMAT               (DD429_GENERAL_COMMAND__INDEX_START + 1)
#define DD429_GENERAL_COMMAND__INTERRUPT_ENABLE             (DD429_GENERAL_COMMAND__INDEX_START + 2)
#define DD429_GENERAL_COMMAND__GET_INTERRUPT_STATUS         (DD429_GENERAL_COMMAND__INDEX_START + 3)
#define DD429_GENERAL_COMMAND__RESET_429_PROGRAMMABLE       (DD429_GENERAL_COMMAND__INDEX_START + 4)
#define DD429_GENERAL_COMMAND__CONFIG_INTERRUPT_CONDITIONS  (DD429_GENERAL_COMMAND__INDEX_START + 5)
#define DD429_GENERAL_COMMAND__GET_IRIG_RX_LATCHED_TIME     (DD429_GENERAL_COMMAND__INDEX_START + 6)

#define DD429_GENERAL_COMMAND__INDEX_END                    (DD429_GENERAL_COMMAND__INDEX_START + 6) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* Control Commands                                                           */
/* -------------------------------------------------------------------------- */

#define DD429_CONTROL_COMMAND__SET_AVIONIC_OUTPUT           (DD429_CONTROL_COMMAND__INDEX_START + 0)
#define DD429_CONTROL_COMMAND__GET_AVIONIC_OUTPUT           (DD429_CONTROL_COMMAND__INDEX_START + 1)
#define DD429_CONTROL_COMMAND__SET_AVIONIC_OUTPUT_ENABLE    (DD429_CONTROL_COMMAND__INDEX_START + 2)
#define DD429_CONTROL_COMMAND__GET_AVIONIC_OUTPUT_ENABLE    (DD429_CONTROL_COMMAND__INDEX_START + 3)
#define DD429_CONTROL_COMMAND__GET_AVIONIC_INPUT            (DD429_CONTROL_COMMAND__INDEX_START + 4)
#define DD429_CONTROL_COMMAND__GET_AVIONIC_ALL              (DD429_CONTROL_COMMAND__INDEX_START + 5)
#define DD429_CONTROL_COMMAND__SET_AVIONIC_ALL              (DD429_CONTROL_COMMAND__INDEX_START + 6)
#define DD429_CONTROL_COMMAND__SET_DIO_OUTPUT               (DD429_CONTROL_COMMAND__INDEX_START + 7)
#define DD429_CONTROL_COMMAND__GET_DIO_OUTPUT               (DD429_CONTROL_COMMAND__INDEX_START + 8)
#define DD429_CONTROL_COMMAND__SET_DIO_OUTPUT_ENABLE        (DD429_CONTROL_COMMAND__INDEX_START + 9)
#define DD429_CONTROL_COMMAND__GET_DIO_OUTPUT_ENABLE        (DD429_CONTROL_COMMAND__INDEX_START + 10)
#define DD429_CONTROL_COMMAND__GET_DIO_INPUT                (DD429_CONTROL_COMMAND__INDEX_START + 11)
#define DD429_CONTROL_COMMAND__GET_DIO_ALL                  (DD429_CONTROL_COMMAND__INDEX_START + 12)
#define DD429_CONTROL_COMMAND__SET_DIO_ALL                  (DD429_CONTROL_COMMAND__INDEX_START + 13)

#define DD429_CONTROL_COMMAND__INDEX_END                    (DD429_CONTROL_COMMAND__INDEX_START + 13) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* FIFO Commands                                                              */
/* -------------------------------------------------------------------------- */

#define DD429_FIFO_COMMAND__CLEAR_RX_QUEUE                  (DD429_FIFO_COMMAND__INDEX_START + 0)
#define DD429_FIFO_COMMAND__GET_RX_QUEUE_STATUS             (DD429_FIFO_COMMAND__INDEX_START + 1)
#define DD429_FIFO_COMMAND__READ_RX_QUEUE_IRIG_ONE          (DD429_FIFO_COMMAND__INDEX_START + 2)
#define DD429_FIFO_COMMAND__READ_RX_QUEUE_IRIG_MORE         (DD429_FIFO_COMMAND__INDEX_START + 3)

#define DD429_FIFO_COMMAND__INDEX_END                       (DD429_FIFO_COMMAND__INDEX_START + 3) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* Receiver Commands                                                          */
/* -------------------------------------------------------------------------- */

#define DD429_RX_COMMAND__ENABLE_RX                         (DD429_RX_COMMAND__INDEX_START + 0)
#define DD429_RX_COMMAND__RESET_TIMESTAMP                   (DD429_RX_COMMAND__INDEX_START + 1)
#define DD429_RX_COMMAND__CONFIG_TIMESTAMP                  (DD429_RX_COMMAND__INDEX_START + 2)
#define DD429_RX_COMMAND__GET_TIMESTAMP                     (DD429_RX_COMMAND__INDEX_START + 3)
#define DD429_RX_COMMAND__GET_TIMESTAMP_CONFIG              (DD429_RX_COMMAND__INDEX_START + 4)
#define DD429_RX_COMMAND__GET_LOOPBACK_MAPPING              (DD429_RX_COMMAND__INDEX_START + 5)
#define DD429_RX_COMMAND__SET_LOOPBACK_MAPPING              (DD429_RX_COMMAND__INDEX_START + 6)
#define DD429_RX_COMMAND__SET_INTERRUPT_CONDITION           (DD429_RX_COMMAND__INDEX_START + 7)

#define DD429_RX_COMMAND__INDEX_END                         (DD429_RX_COMMAND__INDEX_START + 7) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* RX Control Register GET Commands                                           */
/*                                                                            */
/* NOTE: These commands must be in the same order as the bit offset values.   */
/*       as these values are also used as indices to dd429RxControlMaskTable  */
/*                                                                            */
/* -------------------------------------------------------------------------- */

#define DD429_RX_CONTROL_COMMAND_GET__WRAP_AROUND           (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_WRAP_AROUND)
#define DD429_RX_CONTROL_COMMAND_GET__IGNORE_LABEL          (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_IGNORE_LABEL)
#define DD429_RX_CONTROL_COMMAND_GET__RX_FULL               (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RX_FULL)
#define DD429_RX_CONTROL_COMMAND_GET__MODE                  (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_MODE)
#define DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED       (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_TIMETAG)
#define DD429_RX_CONTROL_COMMAND_GET__LABEL_AUTO_CLEAR      (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_LABEL_AUTO_CLEAR)
#define DD429_RX_CONTROL_COMMAND_GET__PARITY                (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_PARITY)
#define DD429_RX_CONTROL_COMMAND_GET__SPEED                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_SPEED)
#define DD429_RX_CONTROL_COMMAND_GET__RSVD1                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD1)
#define DD429_RX_CONTROL_COMMAND_GET__BIT_FORMAT            (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_BIT_FORMAT)
#define DD429_RX_CONTROL_COMMAND_GET__DATA_TRANSFER         (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_DATA_TRANSFER)
#define DD429_RX_CONTROL_COMMAND_GET__RESET                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RESET)
#define DD429_RX_CONTROL_COMMAND_GET__RSVD2                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD2)
#define DD429_RX_CONTROL_COMMAND_GET__RSVD3                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD3)
#define DD429_RX_CONTROL_COMMAND_GET__FILTER_PARITY_ERR     (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_PARITY_ERR)
#define DD429_RX_CONTROL_COMMAND_GET__FILTER_DATA           (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_DATA)
#define DD429_RX_CONTROL_COMMAND_GET__FILTER_CODE           (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_CODE)
#define DD429_RX_CONTROL_COMMAND_GET__ARINC575              (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_ARINC575)
#define DD429_RX_CONTROL_COMMAND_GET__RSVD4                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD4)
#define DD429_RX_CONTROL_COMMAND_GET__RSVD5                 (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD5)

                                                            /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */
#define DD429_RX_CONTROL_COMMAND_GET__INDEX_END             (DD429_RX_CONTROL_COMMAND_GET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD5)


/* -------------------------------------------------------------------------- */
/* RX Control Register SET Commands                                           */
/*                                                                            */
/* NOTE: These commands must be in the same order as the bit offset values    */
/*       as these values are also used as indices to dd429RxControlMaskTable  */
/*                                                                            */
/* -------------------------------------------------------------------------- */

#define DD429_RX_CONTROL_COMMAND_SET__WRAP_AROUND           (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_WRAP_AROUND)
#define DD429_RX_CONTROL_COMMAND_SET__IGNORE_LABEL          (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_IGNORE_LABEL)
#define DD429_RX_CONTROL_COMMAND_SET__RX_FULL               (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RX_FULL)
#define DD429_RX_CONTROL_COMMAND_SET__MODE                  (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_MODE)
#define DD429_RX_CONTROL_COMMAND_SET__TIMETAG               (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_TIMETAG)
#define DD429_RX_CONTROL_COMMAND_SET__LABEL_AUTO_CLEAR      (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_LABEL_AUTO_CLEAR)
#define DD429_RX_CONTROL_COMMAND_SET__PARITY                (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_PARITY)
#define DD429_RX_CONTROL_COMMAND_SET__SPEED                 (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_SPEED)
#define DD429_RX_CONTROL_COMMAND_SET__RSVD1                 (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD1)
#define DD429_RX_CONTROL_COMMAND_SET__BIT_FORMAT            (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_BIT_FORMAT)
#define DD429_RX_CONTROL_COMMAND_SET__DATA_TRANSFER         (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_DATA_TRANSFER)
#define DD429_RX_CONTROL_COMMAND_SET__RESET                 (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RESET)
#define DD429_RX_CONTROL_COMMAND_SET__FIFO_EMPTY            (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FIFO_EMPTY)
#define DD429_RX_CONTROL_COMMAND_SET__FIFO_COUNT            (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FIFO_COUNT)
#define DD429_RX_CONTROL_COMMAND_SET__FILTER_PARITY_ERR     (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_PARITY_ERR)
#define DD429_RX_CONTROL_COMMAND_SET__FILTER_DATA           (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_DATA)
#define DD429_RX_CONTROL_COMMAND_SET__FILTER_CODE           (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_FILTER_CODE)
#define DD429_RX_CONTROL_COMMAND_SET__ARINC575              (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_ARINC575)
#define DD429_RX_CONTROL_COMMAND_SET__OVERFLOW              (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_OVERFLOW)
#define DD429_RX_CONTROL_COMMAND_SET__RSVD2                 (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD2)

                                                            /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */
#define DD429_RX_CONTROL_COMMAND_SET__INDEX_END             (DD429_RX_CONTROL_COMMAND_SET__INDEX_START + DD429_RX_CONTROL_BIT_OFFSET_RSVD2)

/* -------------------------------------------------------------------------- */
/* Filter Commands                                                            */
/* -------------------------------------------------------------------------- */

#define DD429_FILTER_COMMAND__ENABLE                        (DD429_FILTER_COMMAND__INDEX_START + 0)
#define DD429_FILTER_COMMAND__CONFIG                        (DD429_FILTER_COMMAND__INDEX_START + 1)
#define DD429_FILTER_COMMAND__ADD                           (DD429_FILTER_COMMAND__INDEX_START + 2)
#define DD429_FILTER_COMMAND__CLEAR                         (DD429_FILTER_COMMAND__INDEX_START + 3)
#define DD429_FILTER_COMMAND__DELETE                        (DD429_FILTER_COMMAND__INDEX_START + 4)
#define DD429_FILTER_COMMAND__GET                           (DD429_FILTER_COMMAND__INDEX_START + 5)
#define DD429_FILTER_COMMAND__GET_ALL                       (DD429_FILTER_COMMAND__INDEX_START + 6)
#define DD429_FILTER_COMMAND__GET_STATUS                    (DD429_FILTER_COMMAND__INDEX_START + 7)
#define DD429_FILTER_COMMAND__GET_NUM_OF_FILTER             (DD429_FILTER_COMMAND__INDEX_START + 8)

#define DD429_FILTER_COMMAND__INDEX_END                     (DD429_FILTER_COMMAND__INDEX_START + 8) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* Mailbox Commands                                                           */
/* -------------------------------------------------------------------------- */

#define DD429_MAILBOX_COMMAND__CLEAR_MAILBOX                (DD429_MAILBOX_COMMAND__INDEX_START + 0)
#define DD429_MAILBOX_COMMAND__GET_MAILBOX                  (DD429_MAILBOX_COMMAND__INDEX_START + 1)
#define DD429_MAILBOX_COMMAND__GET_MAILBOX_STATUS           (DD429_MAILBOX_COMMAND__INDEX_START + 2)
#define DD429_MAILBOX_COMMAND__READ_MAILBOX_IRIG            (DD429_MAILBOX_COMMAND__INDEX_START + 3)

#define DD429_MAILBOX_COMMAND__INDEX_END                    (DD429_MAILBOX_COMMAND__INDEX_START + 3) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* Transmit Commands                                                          */
/* -------------------------------------------------------------------------- */

#define DD429_TX_COMMAND__ENABLE_TX                         (DD429_TX_COMMAND__INDEX_START + 0)
#define DD429_TX_COMMAND__SET_TX_PARITY                     (DD429_TX_COMMAND__INDEX_START + 1)
#define DD429_TX_COMMAND__SET_TX_SPEED                      (DD429_TX_COMMAND__INDEX_START + 2)
#define DD429_TX_COMMAND__GET_TX_QUEUE_STATUS               (DD429_TX_COMMAND__INDEX_START + 3)
#define DD429_TX_COMMAND__LOAD_TX_QUEUE_ONE                 (DD429_TX_COMMAND__INDEX_START + 4)
#define DD429_TX_COMMAND__LOAD_TX_QUEUE_MORE                (DD429_TX_COMMAND__INDEX_START + 5)
#define DD429_TX_COMMAND__CLEAR_REPEATED                    (DD429_TX_COMMAND__INDEX_START + 6)
#define DD429_TX_COMMAND__DEL_REPEATED                      (DD429_TX_COMMAND__INDEX_START + 7)
#define DD429_TX_COMMAND__GET_NUM_OF_REPEATED               (DD429_TX_COMMAND__INDEX_START + 8)
#define DD429_TX_COMMAND__GET_ALL_REPEATED                  (DD429_TX_COMMAND__INDEX_START + 9)
#define DD429_TX_COMMAND__GET_TX_QUEUE_FREE_COUNT           (DD429_TX_COMMAND__INDEX_START + 10)

#define DD429_TX_COMMAND__INDEX_END                         (DD429_TX_COMMAND__INDEX_START + 10) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* -------------------------------------------------------------------------- */
/* Tester Commands                                                            */
/* -------------------------------------------------------------------------- */

#define DD429_TESTER_COMMAND__SET_AMPLITUDE                 (DD429_TESTER_COMMAND__INDEX_START + 0)
#define DD429_TESTER_COMMAND__GET_AMPLITUDE                 (DD429_TESTER_COMMAND__INDEX_START + 1)
#define DD429_TESTER_COMMAND__LOAD_TX_QUEUE_ONE             (DD429_TESTER_COMMAND__INDEX_START + 2)
#define DD429_TESTER_COMMAND__LOAD_TX_QUEUE_MORE            (DD429_TESTER_COMMAND__INDEX_START + 3)
#define DD429_TESTER_COMMAND__SET_RX_VARIABLE_SPEED         (DD429_TESTER_COMMAND__INDEX_START + 4)
#define DD429_TESTER_COMMAND__SET_TX_VARIABLE_SPEED         (DD429_TESTER_COMMAND__INDEX_START + 5)
#define DD429_TESTER_COMMAND__GET_RX_VARIABLE_SPEED         (DD429_TESTER_COMMAND__INDEX_START + 6)
#define DD429_TESTER_COMMAND__GET_TX_VARIABLE_SPEED         (DD429_TESTER_COMMAND__INDEX_START + 7)
#define DD429_TESTER_COMMAND__SET_TX_FRAME_RESOLUTION       (DD429_TESTER_COMMAND__INDEX_START + 8)
#define DD429_TESTER_COMMAND__GET_TX_FRAME_RESOLUTION       (DD429_TESTER_COMMAND__INDEX_START + 9)
#define DD429_TESTER_COMMAND__SET_TX_FRAME_CONTROL          (DD429_TESTER_COMMAND__INDEX_START + 10)
#define DD429_TESTER_COMMAND__SEND_TX_FRAME_ASYNC           (DD429_TESTER_COMMAND__INDEX_START + 11)
#define DD429_TESTER_COMMAND__VOLTAGE_MONITOR_ENABLE        (DD429_TESTER_COMMAND__INDEX_START + 12)
#define DD429_TESTER_COMMAND__VOLTAGE_MONITOR_START         (DD429_TESTER_COMMAND__INDEX_START + 13)
#define DD429_TESTER_COMMAND__VOLTAGE_MONITOR_GET_STATUS    (DD429_TESTER_COMMAND__INDEX_START + 14)
#define DD429_TESTER_COMMAND__VOLTAGE_MONITOR_GET_DATA      (DD429_TESTER_COMMAND__INDEX_START + 15)
#define DD429_TESTER_COMMAND__ADD_REPEATED                  (DD429_TESTER_COMMAND__INDEX_START + 16)
#define DD429_TESTER_COMMAND__GET_REPEATED                  (DD429_TESTER_COMMAND__INDEX_START + 17)
#define DD429_TESTER_COMMAND__MODIFY_REPEATED_DATA          (DD429_TESTER_COMMAND__INDEX_START + 18)
#define DD429_TESTER_COMMAND__MODIFY_REPEATED_DATA_ITEM     (DD429_TESTER_COMMAND__INDEX_START + 19)
#define DD429_TESTER_COMMAND__GET_ALL_REPEATED_ITEM         (DD429_TESTER_COMMAND__INDEX_START + 20)
#define DD429_TESTER_COMMAND__ADD_REPEATED_ITEM             (DD429_TESTER_COMMAND__INDEX_START + 21)
#define DD429_TESTER_COMMAND__CONFIG_REPEATER               (DD429_TESTER_COMMAND__INDEX_START + 22)
#define DD429_TESTER_COMMAND__SET_REPEATER_MODE             (DD429_TESTER_COMMAND__INDEX_START + 23)
#define DD429_TESTER_COMMAND__GET_REPEATER_MODE             (DD429_TESTER_COMMAND__INDEX_START + 24)
#define DD429_TESTER_COMMAND__SERIAL_WRITE_REG              (DD429_TESTER_COMMAND__INDEX_START + 25)
#define DD429_TESTER_COMMAND__SERIAL_READ_REG               (DD429_TESTER_COMMAND__INDEX_START + 26)
#define DD429_TESTER_COMMAND__SERIAL_SET_CONFIG             (DD429_TESTER_COMMAND__INDEX_START + 27)
#define DD429_TESTER_COMMAND__SERIAL_GET_CONFIG             (DD429_TESTER_COMMAND__INDEX_START + 28)
#define DD429_TESTER_COMMAND__SERIAL_READ_REG_EXT           (DD429_TESTER_COMMAND__INDEX_START + 29)
#define DD429_TESTER_COMMAND__INDEX_END                     (DD429_TESTER_COMMAND__INDEX_START + 29) /* MAKE SURE THIS VALUE IS ALWAYS THE SAME AS THE LAST DEFINED ITEM */

/* ========================================================================== */
/* TX Scheduler                                                               */
/* ========================================================================== */
#define DD429_TX_SCHED_QUEUE_DEPTH              0x00000040
#define DD429_TX_SCHED_MESSAGE_MEM_DEPTH        0x00000400

#define DD429_MODIFY_VIA_LABEL                  0x0000001
#define DD429_MODIFY_VIA_SDI_LABEL              0x0000002

/* ========================================================================== */
/* TX Frame                                                                   */
/* ========================================================================== */

#define DD429_TX_CONTROL_WORD__END_OF_MINOR_FRAME_OFFSET        24
#define DD429_TX_CONTROL_WORD__33RD_BIT_OFFSET                  23
#define DD429_TX_CONTROL_WORD__PARITY_ERROR_OFFSET              22
#define DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_OFFSET           16
#define DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_OFFSET  0

#define DD429_TX_CONTROL_WORD__END_OF_MINOR_FRAME_MASK          0x01000000
#define DD429_TX_CONTROL_WORD__33RD_BIT_MASK                    0x00800000
#define DD429_TX_CONTROL_WORD__PARITY_ERROR_MASK                0x00400000
#define DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_MASK             0x003F0000
#define DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_MASK    0x0000FFFF

#define DD429_TX_FRAME_REPEAT_COUNT__INFINITE                   0

#define DD429_TX_FRAME_STOP                                     0
#define DD429_TX_FRAME_START                                    1
#define DD429_TX_FRAME_INIT                                     2

#define DD429_CHANNEL_SPEED_BPS_DEFAULT     0                   /* indcates to use (or is using) the high or low speed setting */

/* DDC INTERNAL USE ONLY (BEGIN) */
#define DD429_TX_FRAME_LOAD                                     3
#define MAX_INT_STATUS_SIZE                                     (sizeof(U32BIT) * 32)

/* DDC INTERNAL USE ONLY (END) */

/* ========================================================================== */
/* Voltage Monitoring                                                         */
/* ========================================================================== */

#define DD429_VOLTAGE_MONITOR_BUFFER_BYTE_SIZE                  0x1000

#define DD429_VOLTAGE_MONITOR_STATUS__SAMPLING_IN_PROGRESS      0x40000000
#define DD429_VOLTAGE_MONITOR_STATUS__SAMPLING_COMPLETE         0x80000000

/* ========================================================================== */
/* ========================================================================== */

#define DD429_TX_VARIABLE_AMPLITUDE_MIN         0x00
#define DD429_TX_VARIABLE_AMPLITUDE_DEFAULT     0xE0
#define DD429_TX_VARIABLE_AMPLITUDE_MAX         0xFF

#define DD429_RX_FIFO_MAILBOX_LENGTH            0x400
#define MAX_LABEL_SDI_FILTERS                   0x400

#define DD429_LABEL_MASK                        0x000000FF
#define DD429_SDI_MASK                          0x00000300
#define DD429_LABEL_SDI_MASK                    0x000003FF

#define DD429_ALT_SDI_MASK                      0x00001800
#define DD429_ALT_LABEL_SDI_MASK                0x000018FF

#define DD429_SDI_OFFSET                        8
#define DD429_ALT_SDI_OFFSET                    11

#define DD429_ALT_TO_NORMAL_SDI_OFFSET          3
/* SCHED 1024 */
#define DD429_TX_SCHED_CONTROL_OFFSET           0x00000400

#define DD429_TX_SCHED_FREQ_SHIFT               16
#define DD429_TX_SCHED_OFFSET_SHIFT             0
#define DD429_TX_SCHED_FREQ_MASK                0xFFFF0000
#define DD429_TX_SCHED_OFFSET_MASK              0x0000FFFF

#define DD429_SCHED_DATA_UPDATE_MASK            0x000000001
#define DD429_SCHED_CONTROL_UPDATE_MASK         0x000000002
#define DD429_SCHED_FREQUENCY_UPDATE_MASK       0x000000004
#define DD429_SCHED_OFFSET_UPDATE_MASK          0x000000008
#define DD429_SCHED_ACTIVE_UPDATE_MASK          0x000000010
/* End of SCHED */

/* ============================================================================ */
/* Data Repeater */
/* ============================================================================ */
#define DD429_REPEATER_UNMAP                    0       /* Remove the repeater map between two channels */
#define DD429_REPEATER_MAP                      1       /* Add a repeater map between two channels */

#define REPEATER_MODE_MASK_SW                   0x000F0000

#define REPEATER_MODE__DISABLE                  0x00000000    /* Disable  data repeater */

#define REPEATER_MODE__REPEAT                   0x00000080    /* Repeat the exact data received */
#define REPEATER_MODE__CLEAR                    0x00000040    /* Clear a portion of the message. A value of 1 in the data field means to clear (AND) one of the received bits */
#define REPEATER_MODE__SET                      0x00000020    /* Set a portion of the message. A value of 1 in the data field means to set (OR) one of the received bits */
#define REPEATER_MODE__FLIP                     0x00000010    /* Flip the data portion of the message. A value of 1 in the data field means to flip (XOR) one of the received bits */
#define REPEATER_MODE__REPLACE                  0x00000008    /* Replace a portion of the message, mutually exclusive to CLEAR, SET, FLIP */

#define REPEATER_OPTION__MSG_MODIFICATION_ANY   0x00000004    /* Modify any portion of the message - modify bits 0-31 */
#define REPEATER_OPTION__MSG_MODIFICATION_DATA  0x00000002    /* Modify the data portion of the message only - see Label/SDI mode for bits modified */
#define REPEATER_OPTION__MSG_MODIFICATION_LABEL 0x00000000    /* Modify the label portion of the message only - see Label/SDI mode for bits modified */
#define REPEATER_OPTION__MODE_LABEL_SDI         0x00000001    /* Label Modification: modify bits 0-9 | Data Modification: modify bits 10-31 | Message Modification: modify bits 0-31 */
#define REPEATER_OPTION__MODE_LABEL_ONLY        0x00000000    /* Label Modification: modify bits 0-7 | Data Modification: modify bits  8-31 | Message Modification: modify bits 0-31 */


#define DD429_ASYNC_PRIORITY_LOW                0
#define DD429_ASYNC_PRIORITY_HIGH               1

/* ========================================================================== */
/* 429 RX Host Buffer                                                         */
/* ========================================================================== */

#define ARINC_RX_HBUF_INSTALL             0
#define ARINC_RX_HBUF_UNINSTALL           1
#define ARINC_RX_HBUF_READ                2
#define ARINC_RX_HBUF_ENABLE              3
#define ARINC_RX_HBUF_DISABLE             4
#define ARINC_HBUF_METRICS                5
#define ARINC_RX_FIFO_MAX_MSG_SIZE        4
#define ARINC_RX_HBUF_MAX_MSG_SIZE        4

#define ARINC_RX_HBUF_MAX                 DDC_UDL_OS_ARINC_RX_HBUF_SIZE * ARINC_RX_HBUF_MAX_MSG_SIZE

#define ARINC_RX_QUEUE_MAX                0x0400
#define ARINC_RX_SHARED_RAM_1             0
#define ARINC_RX_SHARED_RAM_2             1
#define ARINC_RX_SHARED_RAM_MAX           2


typedef struct _RX_HBUF_MESSAGE
{
    U32BIT u32Data;
    U32BIT u32StampHigh;
    U32BIT u32StampLow;

} RX_HBUF_MESSAGE, *PRX_HBUF_MESSAGE;


typedef struct _ARINC_HOST_BUF_METRICS
{
    U32BIT u32429HbufPercentFull;
    U32BIT u32429HbufPercentHigh;
    U32BIT u32429HbufOverflowCount;

} ARINC_HOST_BUF_METRICS, *PARINC_HOST_BUF_METRICS;


typedef struct _DD429TX_FRAME_INFO_TYPE
{
    U32BIT u32NumMinorFrames;
    U8BIT u8PercentageFull;
    U8BIT u8Reserved1;
    U8BIT u8Reserved2;
    U8BIT u8Reserved3;

} DD429TX_FRAME_INFO_TYPE;


typedef struct _DD429_TESTER_OPTIONS_TYPE
{
    U16BIT s16InterWordBitGapError;
    U8BIT u8WordSizeError;
    U8BIT u8ParityError;
    U8BIT u8Bit33;

} DD429_TESTER_OPTIONS_TYPE;


typedef struct _DD429_TX_MINOR_FRAME_PAYLOAD_TYPE
{
    U32BIT u32Data;
    DD429_TESTER_OPTIONS_TYPE sTesterOptions;

} DD429_TX_MINOR_FRAME_PAYLOAD_TYPE;

typedef struct _DD429_REPEATER_MODE_TYPE
{
    U16BIT u16LabelSdi;
    U32BIT u32Mode;
    U32BIT u32ClearData;
    U32BIT u32SetData;
    U32BIT u32FlipData;
    U32BIT u32ReplaceData;
    U32BIT u32Reserved;

} DD429_REPEATER_MODE_TYPE;

/* ========================================================================== */
/* UART                                                                       */
/* ========================================================================== */

typedef struct _ACEX_429_UART_TYPE
{
    U32BIT *pu32RegBA;     /* ptr to 429 Registers base address    */
    U32BIT *pu32RegSize;   /* ptr to 429 Register size             */

    U32BIT *pu32MemSize;   /* ptr to UM memory size                */
    U32BIT *pu32MemBA;     /* ptr to UM memory base address        */

} ACEX_429_UART_TYPE;


/* ========================================================================== */
/* STRUCTURES FOR PASSING DATA TO THE DRIVER                                  */
/* ========================================================================== */

typedef struct _DDC_IRIG_TX_TYPE
{
    U16BIT u16IRIGBTxSupported;
    U16BIT u16Enable;
    U16BIT u16Seconds;
    U16BIT u16Minutes;
    U16BIT u16Hours;
    U16BIT u16Days;
    U16BIT u16Year;
    U32BIT u32Control;

} DDC_IRIG_TX_TYPE, *PDDC_IRIG_TX_TYPE;


/* ========================================================================== */
/* STRUCTURES FOR GETTING DATA FROM THE DRIVER                                */
/* ========================================================================== */

typedef struct _ARINC_INTERRUPT
{
    U32BIT u32MasterStatus;
    U32BIT u32RxStatus[MAX_NUM_429_CHANNELS];
    U32BIT u32AioStatus;

} ARINC_INTERRUPT;


typedef struct _DD429_GET_MAILBOX__OUTPUT_TYPE
{
    U32BIT u32Error;
    S16BIT s16N;
    U16BIT su16LabelSDI[DD429_RX_FIFO_MAILBOX_LENGTH];

} DD429_GET_MAILBOX__OUTPUT_TYPE;


typedef struct DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32RxFIFO_Count;

} DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE;


typedef struct _DD429_READ_DATA_IRIG__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32Data;
    U32BIT u32StampHi;
    U32BIT u32StampLo;

} DD429_READ_DATA_IRIG__OUTPUT_TYPE;


typedef struct _DD429_READ_MAILBOX_IRIG__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32RxMailbox_LabelSdiError;
    U32BIT u32RxMailbox_NewWordReceived;
    U32BIT u32Data;
    U32BIT u32StampHi;
    U32BIT u32StampLo;

} DD429_READ_MAILBOX_IRIG__OUTPUT_TYPE;


typedef struct _DD429_READ_DATA_IRIG_MORE__OUTPUT_TYPE
{
    U32BIT u32Error;
    S16BIT s16TimeTagEnabledValue;
    S16BIT s16FifoCount;
    U32BIT au32Data[0x1000];

} DD429_READ_DATA_IRIG_MORE__OUTPUT_TYPE;


typedef struct _DD429_GET_INTERRUPT_STATUS__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT *pu32Status;

} DD429_GET_INTERRUPT_STATUS__OUTPUT_TYPE;


typedef struct _DD429_GET_TIMESTAMP__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32Lsb;
    U32BIT u32Msb;

} DD429_GET_TIMESTAMP__OUTPUT_TYPE;


typedef struct _DD429_GET_TIME_STAMP_CONFIG__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32Format;
    U32BIT u32Resolution;
    U32BIT u32Rollover;

} DD429_GET_TIME_STAMP_CONFIG__OUTPUT_TYPE;


typedef struct _DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32Command;

} DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE;


typedef struct _DD429_GET_REPEATED__OUTPUT_TYPE
{
    U32BIT u32Error;
    U32BIT u32Data;
    DD429_TESTER_OPTIONS_TYPE sTesterOptions;
    S16BIT s16Frequency;
    S16BIT s16Offset;

    S16BIT s16Found;

} DD429_GET_REPEATED__OUTPUT_TYPE;


typedef struct _DD429_GET_TX_SCHEDULE_RESOLUTION__OUTPUT_TYPE
{
    U32BIT u32Error;
    U8BIT u8Resolution;

} DD429_GET_TX_SCHEDULE_RESOLUTION__OUTPUT_TYPE;


typedef struct _DD429_GET_ALL_REPEATED__OUTPUT_TYPE
{
    U32BIT u32Error;
    S16BIT s16NumFound;
    S16BIT as16Data[DD429_TX_SCHED_QUEUE_DEPTH]; /* assume the the buffer can hold the max # of items in the queue */

} DD429_GET_ALL_REPEATED__OUTPUT_TYPE;


#endif /* _DDC_UDL_ARINC429_H_ */
