/*******************************************************************************
 * FILE: ddc_udl_can.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support CAN Bus
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

#ifndef _DDC_UDL_CAN_H_
#define _DDC_UDL_CAN_H_

#include "driver_sdk/ddc_udl_sdk.h"

#define CAN_BUS_DWORDS_PER_MSG                      6 /* number of DWords in a CAN message */
#define CAN_BUS_MAX_MSG_COUNT_INT                   500
#define CAN_BUS_MAX_TIMER_MS_INT                    1000
#define CAN_BUS_RX_FILTER_MAX                       32
#define CAN_RX_HEADER_BYTE_COUNT                    16 /* Size in bytes of message count DWORD and Flags DWORD */

#define CAN_BUS_SPEED_MASK                          0xFF  /* Speed settings */
#define CAN_BUS_SPEED_1_MBS                         0x01
#define CAN_BUS_SPEED_800_KBS                       0x02
#define CAN_BUS_SPEED_500_KBS                       0x03
#define CAN_BUS_SPEED_250_KBS                       0x04
#define CAN_BUS_SPEED_125_KBS                       0x05
#define CAN_BUS_SPEED_100_KBS                       0x06
#define CAN_BUS_SPEED_50_KBS                        0x07
#define CAN_BUS_SPEED_40_KBS                        0x08
#define CAN_BUS_SPEED_20_KBS                        0x09
#define CAN_BUS_SPEED_10_KBS                        0x0A
#define CAN_BUS_SPEED_5_KBS                         0x0B

#define CAN_BUS_ID_MASK                             0x1FFFFFFF
#define CAN_BUS_EXT_ID_MASK                         0x0003FFFF
#define CAN_BUS_STD_ID_MASK                         0x1FFC0000
#define CAN_BUS_ID_EXTENDED_ENABLE                  0x80000000

#define CAN_BUS_RX_FILTER_ENABLE                    0x40000000
#define CAN_BUS_RX_FILTER_RESERVED_B29              0x20000000

#define CAN_MSG_PENDING_MASK                        0xFFFF0000
#define CAN_MESSAGE_PENDING                         0x4DDC0000
#define CAN_ERROR_TX_QUEUED_MASK                    0x00004000
#define CAN_ERROR_TX_OVERFLOW_MASK                  0x00002000
#define CAN_ERROR_INT_MASK                          0x00001000
#define CAN_EXT_ID_ENABLE_MASK                      0x00000400
#define CAN_SUB_REMOTE_REQ_MASK                     0x00000200
#define CAN_REMOTE_REQ_MASK                         0x00000100
#define CAN_CH_NUM_MASK                             0x000000FF
#define CAN_MSG_LEN_MASK                            0x0000000F
#define CAN_MSG_TIME_MASK                           0x0000FFFF

#define CAN_MSG_ERROR_RECESSIVE_MASK                0x00008000
#define CAN_MSG_ERROR_DOMINANT_MASK                 0x00004000
#define CAN_MSG_ERROR_ACK_MASK                      0x00002000
#define CAN_MSG_ERROR_CRC_MASK                      0x00001000
#define CAN_MSG_ERROR_FORM_MASK                     0x00000800
#define CAN_MSG_ERROR_STUFFING_MASK                 0x00000400
#define CAN_MSG_ERROR_TX_COUNTER_MASK               0x00000200
#define CAN_MSG_ERROR_RX_COUNTER_MASK               0x00000100
#define CAN_MSG_ERROR_BOFFINT_MASK                  0x00000004
#define CAN_MSG_ERROR_GENERAL_MASK                  0x00000002
#define CAN_MSG_TX_ERROR_CTR_MASK                   0x000000FF
#define CAN_MSG_RX_ERROR_CTR_MASK                   0x0000FF00

/* CAN Bus configuration options place holders, common between RTL and Driver */
#define CAN_BUS_SPEED_OPT                           0x00000001
#define CAN_BUS_RESET_OPT                           0x00000002
#define CAN_BUS_RX_FILTER_OPT                       0x00000004
#define CAN_BUS_RUN_STATE_OPT                       0x00000008
#define CAN_BUS_FIRWARE_VERSION_OPT                 0x00000010
#define CAN_BUS_RX_INTERRUPT_OPT                    0x00000020
#define CAN_BUS_TIMER_OPT                           0x00000040
#define CAN_BUS_MESSAGE_COUNT_INT_OPT               0x00000080
#define CAN_BUS_MONITOR_ONLY_OPT                    0x00000100
#define CAN_BUS_INTERNAL_LOOP_BACK_OPT              0x00000200
#define CAN_BUS_DEBUG_UART_OPT                      0x08000000
#define CAN_BUS_TX_ONE_MSG_CH1                      0x04000000
#define CAN_BUS_TX_ONE_MSG_CH2                      0x02000000
#define CAN_BUS_CONFIG_PENDING                      0x80000000  /* MSB bit reserved for config word pending */

typedef struct _CAN_FIRMWARE_VERSION
{
    U8BIT u8Reserved;
    U8BIT u8VerMajor;
    U8BIT u8VerMinora;
    U8BIT u8VerMinorb;

} CAN_FIRMWARE_VERSION, *PCAN_FIRMWARE_VERSION;


typedef enum _CAN_BUS_RUN_STATE
{
    CAN_BUS_RESET,
    CAN_BUS_READY,
    CAN_BUS_RUN,
    CAN_BUS_PAUSE

} CAN_BUS_RUN_STATE;


typedef struct _CAN_BUS_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32ConfigOption;
    U8BIT u8Channel;
    U8BIT u8Speed;
    U8BIT bInterrupt;
    U16BIT u16TimerValue;
    U16BIT u16MessageCountInt;
    U8BIT u8EnableMonitor;
    U8BIT u8EnableLoopback;
    CAN_BUS_RUN_STATE eState;         /* Used only during RUN state setting */
    U32BIT u32FilterValues[CAN_BUS_RX_FILTER_MAX];

} CAN_BUS_CONFIG, *PCAN_BUS_CONFIG;

typedef struct _CAN_BUS_INT_STATUS
{
    U32BIT u32BdInfoInt;
    U32BIT u32MsgCountStatus;
} CAN_BUS_INT_STATUS, PCAN_BUS_INT_STATUS;


#endif /* _DDC_UDL_CAN_H_ */
