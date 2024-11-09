/*******************************************************************************
 * FILE: ddc_udl_um_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to parse the
 *  Unified Memory Map description and organize the configuration information
 *  into a UM data structure.  The Unified Memory Map description
 *  identifies the configuration of the device.
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

#ifndef _DDC_UDL_UM_PRIVATE_H_
#define _DDC_UDL_UM_PRIVATE_H_

#include "os/include/ddc_os_types.h"

#define DDC_TAG                                             '3551'


#define UM_MAX_NUM_COMPONENTS                               30      /* total component number per device */
#define UM_MAX_NUM_INSTANCES                                36      /* total channel number per FPGA */
#define UM_MAX_NUM_DEVICES                                  14      /* total device number per card */

/* device types */
#define UM_DEVICE_ID_CAPABILITIES                           0x0100
#define UM_DEVICE_ID_BRD_GLOBAL                             0x0200
#define UM_DEVICE_ID_MIL_STD_1553                           0x0300
#define UM_DEVICE_ID_ARINC_429_GLOBAL                       0x0400
#define UM_DEVICE_ID_ARINC_429_TX                           0x0500
#define UM_DEVICE_ID_ARINC_429_RX                           0x0600
#define UM_DEVICE_ID_IRIG_B                                 0x0700
#define UM_DEVICE_ID_MIL_STD_1553_SF                        0x0800
#define UM_DEVICE_ID_MIL_STD_1553_MF                        0x0900
#define UM_DEVICE_ID_MIO_UART                               0x0A00
/* New Serial IO Async/Sync additions */
#define UM_DEVICE_ID_MIO_CAST_GLOBAL_UART                   0x0A10  /* Global control registers for each serial IO channel */
#define UM_DEVICE_ID_MIO_CAST_ASYNC_UART                    0x0A20  /* Only supports rs232,485,422.. Async communications */
#define UM_DEVICE_ID_MIO_CAST_SYNC_ASYNC_UART               0x0A30  /* Supports Async/Sync rs232,485,etc.. and HDLC/SDLC communications */
#define UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF                 0x0B00
#define UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF                 0x0B10
#define UM_DEVICE_ID_ARINC_429_TX_MF                        0x0C00
#define UM_DEVICE_ID_ARINC_429_RX_MF                        0x0D00
#define UM_DEVICE_ID_ARINC_429_TX_SCHEDULE_EXTEND           0x0D10
#define UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF_2               0x0D40
#define UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF_2               0x0D50
#define UM_DEVICE_ID_ARINC_429_TX_MF_2                      0x0D60
#define UM_DEVICE_ID_ARINC_429_RX_MF_2                      0x0D70
#define UM_DEVICE_ID_CAN_BUS                                0x0E00
#define UM_DEVICE_ID_ARINC_717_GLOBAL                       0x0F00
#define UM_DEVICE_ID_ARINC_717_RX_TX                        0x1000
#define UM_DEVICE_ID_DIO_TT                                 0x1100
#define UM_DEVICE_ID_EMBEDDED_ARINC_429_TX_SCHEDULE_EXTEND  0x1210
#define UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_TX           0x1240
#define UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_RX           0x1250
#define UM_DEVICE_ID_EMBEDDED_ARINC_429_TX                  0x1260
#define UM_DEVICE_ID_EMBEDDED_ARINC_429_RX                  0x1270

/* component types */
#define UM_COMPONENTS_ID_CAPABILITIES                       0x0101

#define UM_COMPONENTS_ID_BRD_GLOBAL                         0x0201
#define UM_COMPONENTS_ID_BRD_RESETS                         0x0202
#define UM_COMPONENTS_ID_BRD_MEM_CONFIG                     0x0203
#define UM_COMPONENTS_ID_QPRM_GENERAL                       0x0210
#define UM_COMPONENTS_ID_QPRM_DMA                           0x0211
#define UM_COMPONENTS_ID_QPRM_BIST                          0x0212

#define UM_COMPONENTS_ID_BRD_GLOBAL_VOLT_MON_X8             0x0220

#define UM_COMPONENTS_ID_BRD_GLOBAL_THERMAL_MON             0x0230

#define UM_COMPONENTS_ID_MIL_STD_1553_SF_GLOBAL             0x0301
#define UM_COMPONENTS_ID_MIL_STD_1553_SF_BCI                0x0302
#define UM_COMPONENTS_ID_MIL_STD_1553_SF_RTX                0x0303
#define UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE               0x0304
#define UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP                0x0305

#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX                0x0401
#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX                0x0402

#define UM_COMPONENTS_ID_ARINC_429_TX                       0x0501

#define UM_COMPONENTS_ID_ARINC_429_RX                       0x0601

#define UM_COMPONENTS_ID_IRIG_B_RX                          0x0701
#define UM_COMPONENTS_ID_IRIG_B_TX                          0x0702

#define UM_COMPONENTS_ID_MIL_STD_1553_MF_GLOBAL             0x0901
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_BCI                0x0902
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_RTX                0x0903
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_MTIE               0x0904
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP                0x0905
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_BC             0x0906
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_IMP_MRT            0x0907
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_EI                 0x0908
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_REPLAY             0x0909
#define UM_COMPONENTS_ID_MIL_STD_1553_MF_TRIGGERS           0x090A

#define UM_COMPONENTS_ID_MIO_UART                           0x0A01

/* Global Serial I/O devices 0xA10 */
#define UM_COMPONENTS_ID_MIO_CAST_UART_GLOBAL               0x0A11

/* Serial Async devices only 0xA20 */
#define UM_COMPONENTS_ID_MIO_CAST_UART_SERIAL               0x0A21

/* SERIAL Sync/Async - Async, HDLC & SDLC devices 0xA30 */
#define UM_COMPONENTS_ID_MIO_CAST_UART_HDLC                 0x0A31
#define UM_COMPONENTS_ID_MIO_CAST_UART_SDLC                 0x0A32
#define UM_COMPONENTS_ID_MIO_CAST_UART_ASYNC                0x0A33

#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX_MF             0x0B01
#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX_MF             0x0B11

#define UM_COMPONENTS_ID_ARINC_429_TX_MF                    0x0C01

#define UM_COMPONENTS_ID_ARINC_429_RX_MF                    0x0D01
#define UM_COMPONENTS_ID_ARINC_429_TX_SCHEDULE_EXTEND       0x0D11
#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX_2              0x0D41
#define UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX_2              0x0D51
#define UM_COMPONENTS_ID_ARINC_429_TX_2                     0x0D61
#define UM_COMPONENTS_ID_ARINC_429_RX_2                     0x0D71

#define UM_COMPONENTS_ID_CAN_BUS                            0x0E01

#define UM_COMPONENTS_ID_ARINC_717_PROG_CH_GLOBAL           0x0F01

#define UM_COMPONENTS_ID_ARINC_717_PROG_CH                  0x1001

#define UM_COMPONENTS_ID_DIO_TT                             0x1101

#define UM_COMPONENTS_ID_EMBEDDED_ARINC_429_TX_SCHEDULE_EXTEND  0x1211
#define UM_COMPONENTS_ID_EMBEDDED_ARINC_429_GLOBAL_TX           0x1241
#define UM_COMPONENTS_ID_EMBEDDED_ARINC_429_GLOBAL_RX           0x1251
#define UM_COMPONENTS_ID_EMBEDDED_ARINC_429_TX                  0x1261
#define UM_COMPONENTS_ID_EMBEDDED_ARINC_429_RX                  0x1271

/* ========================================================================== */
/* BOARD DEVICE & COMPONENT MASKS                                             */
/* ========================================================================== */
#define UM_DEVICE_MASK_CAPABILITIES                         0x00000001
#define UM_DEVICE_MASK_BRD_GLOBAL                           0x00000002
#define UM_DEVICE_MASK_MIL_STD_1553                         0x00000004
#define UM_DEVICE_MASK_MIL_STD_1553_SF                      0x00000008
#define UM_DEVICE_MASK_MIL_STD_1553_MF                      0x00000010
#define UM_DEVICE_MASK_ARINC_429_GLOBAL                     0x00000020
#define UM_DEVICE_MASK_ARINC_429_TX                         0x00000040
#define UM_DEVICE_MASK_ARINC_429_RX                         0x00000080
#define UM_DEVICE_MASK_IRIG_B                               0x00000100
#define UM_DEVICE_MASK_MIO_UART                             0x00000200
#define UM_DEVICE_MASK_CAN_BUS                              0x00000400
#define UM_DEVICE_MASK_ARINC_717_GLOBAL                     0x00000800
#define UM_DEVICE_MASK_ARINC_717_RX_TX                      0x00001000
#define UM_DEVICE_MASK_ARINC_429_GLOBAL_TX_MF               0x00002000
#define UM_DEVICE_MASK_ARINC_429_GLOBAL_RX_MF               0x00004000
#define UM_DEVICE_MASK_ARINC_429_TX_MF                      0x00008000
#define UM_DEVICE_MASK_ARINC_429_RX_MF                      0x00010000
#define UM_DEVICE_MASK_DIO_TT                               0x00020000
#define UM_DEVICE_MASK_MIO_CAST_UART_SDLC_HDLC              0x00040000


#define UM_COMPONENTS_MASK_CAPABILITIES                     0x00000001
#define UM_COMPONENTS_MASK_BRD_GLOBAL                       0x00000002
#define UM_COMPONENTS_MASK_BRD_RESETS                       0x00000004
#define UM_COMPONENTS_MASK_BRD_MEM_CONFIG                   0x00000008
#define UM_COMPONENTS_MASK_BRD_GLOBAL_VOLT_MON_X8           0x00000010
#define UM_COMPONENTS_MASK_MIL_STD_1553_GLOBAL              0x00000020
#define UM_COMPONENTS_MASK_MIL_STD_1553_BCI                 0x00000040
#define UM_COMPONENTS_MASK_MIL_STD_1553_RTX                 0x00000080
#define UM_COMPONENTS_MASK_MIL_STD_1553_MTIE                0x00000100
#define UM_COMPONENTS_MASK_MIL_STD_1553_IMP                 0x00000200
#define UM_COMPONENTS_MASK_ARINC_429_GLOBAL_TX              0x00000400
#define UM_COMPONENTS_MASK_ARINC_429_GLOBAL_RX              0x00000800
#define UM_COMPONENTS_MASK_ARINC_429_TX                     0x00001000
#define UM_COMPONENTS_MASK_ARINC_429_RX                     0x00002000
#define UM_COMPONENTS_MASK_IRIG_B_RX                        0x00004000
#define UM_COMPONENTS_MASK_IRIG_B_TX                        0x00008000
#define UM_COMPONENTS_MASK_MIO_UART                         0x00010000
#define UM_COMPONENTS_MASK_MIL_STD_1553_EI                  0x00020000
#define UM_COMPONENTS_MASK_MIL_STD_1553_REPLAY              0x00040000
#define UM_COMPONENTS_MASK_MIL_STD_1553_TRIGGERS            0x00080000
#define UM_COMPONENTS_MASK_CAN_BUS                          0x00100000
#define UM_COMPONENTS_MASK_ARINC_717_GLOBAL                 0x00200000
#define UM_COMPONENTS_MASK_ARINC_717_PROG_CH                0x00400000
#define UM_COMPONENTS_MASK_ARINC_429_GLOBAL_TX_MF           0x00800000
#define UM_COMPONENTS_MASK_ARINC_429_GLOBAL_RX_MF           0x01000000
#define UM_COMPONENTS_MASK_ARINC_429_TX_MF                  0x02000000
#define UM_COMPONENTS_MASK_ARINC_429_RX_MF                  0x04000000
#define UM_COMPONENTS_MASK_QPRM_GENERAL                     0x08000000
#define UM_COMPONENTS_MASK_QPRM_DMA                         0x10000000
#define UM_COMPONENTS_MASK_QPRM_BIST                        0x20000000
#define UM_COMPONENTS_MASK_MIO_CAST_UART_SDLC_HDLC          0x40000000

/**********************************************************************************************************************/

/* ========================================================================== */
/* BOARD COMPONENT REGISTERS    32-BIT ADDRESSABLE(DW)                        */
/* OFFSET TO BOARD REG BASE ADDRESS                                           */
/* ========================================================================== */

#define REG_BD_MODEL_NUMBER                                 0x00000000  /* RD    */
#define REG_BD_CAPBILITIES                                  0x00000001  /* RD    */
#define REG_BD_DEVICE_COUNT_0                               0x00000002  /* RD    */
#define REG_BD_DEVICE_COUNT_1                               0x00000003  /* RD    */
#define REG_BD_DEVICE_COUNT_2                               0x00000004  /* RD    */
#define REG_BD_DEVICE_COUNT_3                               0x00000005  /* RD    */
#define REG_BD_DEVICE_COUNT_4                               0x00000006  /* RD    */
#define REG_BD_DEVICE_COUNT_5                               0x00000007  /* RD    */
#define REG_BD_DEVICE_COUNT_6                               0x00000008  /* RD    */
#define REG_BD_BOOTFLASH_START_ADDR                         0x00000009  /* RD    */
#define REG_BD_BOOTFLASH_CLUSTER_NUM                        0x0000000A  /* RD    */
#define REG_BD_BOOTFLASH_CLUSTER_SIZE                       0x0000000B  /* RD    */
#define REG_BD_FLASH_ADDR                                   0x0000000C  /* RD/WR */
#define REG_BD_FLASH_CONTROL                                0x0000000D  /* RD/WR */
#define REG_BD_FLASH_STATUS                                 0x0000000E  /* RD    */
#define REG_BD_FLASH_CONFIG                                 0x0000000F  /* RD/WR */
#define REG_BD_DIOCTRL                                      0x00000010  /* RD/WR */
#define REG_BD_RESERVED_11                                  0x00000011
#define REG_BD_AVIONICS_IO_CTRL_1                           0x00000012
#define REG_BD_RESERVED_13                                  0x00000013
#define REG_BD_INT_STATUS                                   0x00000014  /* RD    */
#define REG_BD_INT_EN                                       0x00000015  /* RD/WR */
#define REG_BD_INT_DIS_STR                                  0x00000016  /* WR    */
#define REG_BD_CONFIG                                       0x00000017  /* RD/WR */
#define REG_BD_CONFIG_PULSE                                 0x00000018  /* WR    */
#define REG_BD_STATUS                                       0x00000019  /* RD/WR */
#define REG_BD_EXT_TT_LOAD_HIGH                             0x0000001A  /* RD/WR */
#define REG_BD_EXT_TT_LOAD_LOW                              0x0000001B  /* RD/WR */
#define REG_BD_AUTO_INIT_STATUS                             0x0000001C  /* RD    */
#define REG_BD_CH1_2_1553_COUPLING_TERM                     0x0000001D
#define REG_BD_CH1_2_1553_VAR_AMP_XCVR                      0x0000001E
#define REG_BD_DIOCONF_1                                    0x00000020  /* RD/WR */
#define REG_BD_DIOCONF_2                                    0x00000021  /* RD/WR */
#define REG_BD_CHAN_SHARE_MUX                               0x00000024  /* RD/WR */

#define REG_BD_P429_RX_TX                                   0x00000026 /* RD/WR */
#define REG_BD_P429_RX_TX_RES                               0x00000027 /* RD/WR */
#define REG_BD_P429_RX_TX_2                                 0x00000028 /* RD/WR */
#define REG_BD_RESERVED_29                                  0x00000029 /* N/A */
#define REG_BD_P429_RELAY                                   0x0000002A /* RD/WR */
#define REG_BD_P717_RELAY                                   0x0000002B /* RD/WR */

#define REG_BD_P232_RELAY                                   0x0000002E /* RD/WR */

#define REG_BD_429_TIMER_INTERRUPT_CONFIG                   0x00000030
#define REG_BD_DIO_INTERRUPT_CONTROL                        0x00000033 /* R/W   Discrete I/O Interrupt Control */
#define REG_BD_DIO_INTERRUPT_STATUS                         0x00000033 /* R     Discrete I/O Interrupt Status */
#define REG_BD_AIO_INTERRUPT_CONTROL                        0x00000035 /* R/W   Avionic I/O Interrupt Control */
#define REG_BD_AIO_INTERRUPT_STATUS                         0x00000036 /* R     Avionic I/O Interrupt Status */

#define REG_BD_SCRATCHPAD_0                                 0x0000003F

/* ========================================================================== */
/* BOARD COMPONENT CAPABILITIES (1=supported)                                 */
/* ========================================================================== */
#define BD_CAPABILITIES_ENHANCED_SERIAL_IO_FIFO         0x00020000  /* 17 */
#define BD_CAPABILITIES_AIO_INTERRUPT                               0x00010000  /* 16 */
#define BD_CAPABILITIES_DIO_INTERRUPT                               0x00008008  /* 15 */
#define BD_CAPABILITIES_ARINC_429_REPEATER_DATA_POLLUTION_SUPPORTED 0x00004000  /* 14 */
#define BD_CAPABILITIES_ARINC_429_REPEATER_SUPPORTED                0x00002000  /* 13 */

#define BD_CAPABILITIES_REAR_CONNECTOR                              0x00000100  /* 08 */
#define BD_CAPABILITIES_MODEL_NUMBERS                               0x000000FF  /* 07 - 00 */

#define BD_CAPABILITIES_ARINC_429_HI_PRI_FIFO_FREE_COUNTER          0x00001000  /* 12 */
#define BD_CAPABILITIES_TX_INHIBIT                                  0x00000400  /* 10 */
#define BD_CAPABILITIES_TX_INHIBIT_BC_DISABLE                       0x00000100  /* 08 */
#define BD_CAPABILITIES_IRIG_RANGE                                  0x00000040  /* 06 */
#define BD_CAPABILITIES_BUS_COUPLING                                0x00000020  /* 05 */
#define BD_CAPABILITIES_VARIABLE_XCVR                               0x00000010  /* 04 */
#define BD_CAPABILITIES_TX_IRIG_B                                   0x00000008  /* 03 */
#define BD_CAPABILITIES_RX_IRIG_B                                   0x00000004  /* 02 */
#define BD_CAPABILITIES_PARALLEL_FLASH_CTRL                         0x00000002  /* 01 */
#define BD_CAPABILITIES_FLASH_INFO                                  0x00000001  /* 00 */

/* ========================================================================== */
/* BOARD COMPONENT INTERRUPT BIT MASKS                                        */
/*                                                                            */
/* NOTE: THESE IS THE MASTER INTERRUPT STATUS REGISTER BIT MASKS              */
/* ========================================================================== */
#define BD_INT_STATUS_MASK_INT_REQ                          0x80000000  /*31*/
#define BD_INT_STATUS_MASK_TEST_INT                         0x40000000  /*30*/
#define BD_INT_STATUS_MASK_IP_SECURITY_FAIL                 0x20000000  /*29*/
#define BD_INT_STATUS_MASK_FLASH_WRITEPROT                  0x10000000  /*28*/  /* Reserved in PC104+ */
#define BD_INT_STATUS_MASK_USB_ERROR                        0x08000000  /*27*/  /* Reserved in PC104+ */
#define BD_INT_STATUS_MASK_USB_EOM                          0x04000000  /*26*/  /* Reserved in PC104+ */
#define BD_INT_STATUS_MASK_IRIG_1_SEC                       0x02000000  /*25*/
#define BD_INT_STATUS_MASK_DMA_FROM_DEV_CMPLT               0x01000000  /*24*/
#define BD_INT_STATUS_MASK_TTAG_ROLLOVR                     0x01000000  /*24*/  /* USB devices */
                                                                            /*23*/
                                                                            /*22*/
#define BD_INT_STATUS_MASK_DMA_TO_DEV_CMPLT                 0x00200000  /*21*/
#define BD_INT_STATUS_MASK_MF_IP_SECURITY_FAIL2             0x00100000  /*20*/
                                                                        /*19*/
                                                                        /*18*/
                                                                        /*17*/
#define BD_INT_STATUS_MASK_AIO_INTERRUPT                    0x00010000  /*16*/
#define BD_INT_STATUS_MASK_DIO_INTERRUPT                    0x00008000  /*15*/
#define BD_INT_STATUS_MASK_DIO_TT                           0x00004000  /*14*/
#define BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT         0x00002000  /*13*/
#define BD_INT_STATUS_MASK_CAN_2                            0x00001000  /*12*/
#define BD_INT_STATUS_MASK_CAN_1                            0x00000800  /*11*/
#define BD_INT_STATUS_MASK_ARINC_717                        0x00000400  /*10*/
#define BD_INT_STATUS_MASK_UART                             0x00000200  /*09*/
#define BD_INT_STATUS_MASK_ARINC_0                          0x00000100  /*08*/  /* Reserved in PC104+ */
#define BD_INT_STATUS_MASK_1553_7                           0x00000080  /*07*/
#define BD_INT_STATUS_MASK_1553_6                           0x00000040  /*06*/
#define BD_INT_STATUS_MASK_1553_5                           0x00000020  /*05*/
#define BD_INT_STATUS_MASK_1553_4                           0x00000010  /*04*/
#define BD_INT_STATUS_MASK_1553_3                           0x00000008  /*03*/
#define BD_INT_STATUS_MASK_1553_2                           0x00000004  /*02*/
#define BD_INT_STATUS_MASK_1553_1                           0x00000002  /*01*/
#define BD_INT_STATUS_MASK_1553_0                           0x00000001  /*00*/

#define BD_INT_STATUS_MASK_1553_CH                          0x000000FF /*1553 channels*/

#define BD_INT_STATUS_MASK_ALL                              0xFFFFFFFF

/* ========================================================================== */
/* BOARD COMPONENT CONFIGURATION BIT MASKS                                    */
/* ========================================================================== */
#define BD_CONFIG_MASK_IRIG_INPUT_RANGE                     0x00000020
#define BD_CONFIG_MASK_INT_TT_CNT_EN                        0x00000010
#define BD_CONFIG_MASK_EXT_TT_CNT_EN                        0x00000008
#define BD_CONFIG_MASK_EXT_TT_LD_PUL_EN                     0x00000004
#define BD_CONFIG_MASK_EXT_TT_CLK_EN                        0x00000002
#define BD_CONFIG_MASK_CLK_20_OUT_EN                        0x00000001

/* ========================================================================== */
/* BOARD COMPONENT CONFIGURATION PULSE BIT MASKS                              */
/* ========================================================================== */
#define BD_CONFIG_PUL_MASK_EXT_IO_RESET                     0x00000080
#define BD_CONFIG_PUL_MASK_IRIGB_RESET                      0x00000040
#define BD_CONFIG_PUL_MASK_429_RESET                        0x00000020
#define BD_CONFIG_PUL_MASK_1553_1_RESET                     0x00000010
#define BD_CONFIG_PUL_MASK_1553_0_RESET                     0x00000008
#define BD_CONFIG_PULSE_MASK_BD_RESET                       0x00000004
#define BD_CONFIG_PUL_MASK_MSTR_RESET                       0x00000002
#define BD_CONFIG_PUL_MASK_TEST_INT                         0x00000001

#define ACEX_BD_BDSTATUS_MASK_FLASH_WRITE_EN                0x00000002

/* ========================================================================== */
/* BOARD STATUS MASKS                                                         */
/* ========================================================================== */
#define BD_STATUS_MASK_PLL_NOT_LOCKED_CH2                   0x00100000
#define BD_STATUS_MASK_BC_DISABLED_CH2                      0x00080000
#define BD_STATUS_MASK_MRT_DISABLED_CH2                     0x00040000
#define BD_STATUS_MASK_RTBOOT_ENABLED_CH2                   0x00020000
#define BD_STATUS_MASK_SSFLAG_ENABLED_CH2                   0x00010000

#define BD_STATUS_MASK_PLL_NOT_LOCKED_CH1                   0x00001000
#define BD_STATUS_MASK_BC_DISABLED_CH1                      0x00000800
#define BD_STATUS_MASK_MRT_DISABLED_CH1                     0x00000400
#define BD_STATUS_MASK_RTBOOT_ENABLED_CH1                   0x00000200
#define BD_STATUS_MASK_SSFLAG_ENABLED_CH1                   0x00000100

#define BD_STATUS_MASK_FLASH_WRITE_EN                       0x00000002
#define BD_STATUS_MASK_EXT_POWER                            0x00000001

#define ACEX_BD_MASK_ALL                                    0xFFFFFFFF

/* ========================================================================== */
/* BD  COMPONENT INTERRUPT STATUS 32-BIT WORD REGISTER DECODES                */
/* ========================================================================== */
#define BD_INT_STATUS_OFFSET_BC                             0x00000000
#define BD_INT_STATUS_OFFSET_MT_RT                          0x00000001
#define BD_INT_STATUS_OFFSET_MTI_NUM_MSGS                   0x00000002
#define BD_INT_STATUS_OFFSET_MTI_LENGTH                     0x00000003
#define BD_INT_STATUS_OFFSET_MTI_START_ADDR                 0x00000004
#define BD_INT_STATUS_OFFSET_IMP                            0x00000005
#define BD_INT_STATUS_OFFSET_BC_IMP                         0x00000006
#define BD_INT_STATUS_OFFSET_MRT_IMP                        0x00000007
#define BD_INT_STATUS_OFFSET_REPLAY_IRQ_STATUS              0x00000008

#define BD_INT_STATUS_OFFSET_CAN_CHANNEL_1                  0x00000020  /* 32 */
#define BD_INT_STATUS_OFFSET_CAN_CHANNEL_2                  0x00000021  /* 33 */

#define BD_INT_STATUS_DWD_ARINC_GENERAL                     0x0000000D
#define BD_INT_STATUS_DWD_ARINC_REG1_REG0                   0x0000000E
#define BD_INT_STATUS_DWD_ARINC_REG3_REG2                   0x0000000F

/* ========================================================================== */
/* BD RESET COMPONENT REGISTERS        32-BIT ADDRESSABLE(DW)                 */
/* ========================================================================== */
#define REG_BD_RESET_MODULE                                 0x00000000  /* ace to e2mace support */
#define REG_BD_RESET_COMPONENT_SF                           0x00000001  /* added to support SF+ Imp/MT/RT/BC */
#define REG_BD_RESET_COMPONENT_MF                           0x00000002  /* added to support MF Imp RT/BC */
#define REG_BD_RESET_COMPONENT_REPLAY                       0x00000003  /* added to support MF Replay */
#define REG_BD_RESET_COMPONENT_TRG                          0x00000004  /* added to support MF BC Triggering */

/* bit mask to reset entire device - write it to RESET_MODULE */
#define BD_MASTER_RESET                                     0xFFFFFFFF

/* ========================================================================== */
/* BD RESET MODULE BIT MASKS                                                  */
/* ========================================================================== */
#define BD_RESET_DIO_TT                                     0x00001000
#define BD_RESET_CAN_BUS                                    0x00000800
#define BD_RESET_BD                                         0x00000400
#define BD_RESET_IRIGB                                      0x00000200
#define BD_RESET_ARINC429                                   0x00000100
#define BD_RESET_1553_CH7                                   0x00000080
#define BD_RESET_1553_CH6                                   0x00000040
#define BD_RESET_1553_CH5                                   0x00000020
#define BD_RESET_1553_CH4                                   0x00000010
#define BD_RESET_1553_CH3                                   0x00000008
#define BD_RESET_1553_CH2                                   0x00000004
#define BD_RESET_1553_CH1                                   0x00000002
#define BD_RESET_1553_CH0                                   0x00000001

/* ========================================================================== */
/* BD RESET COMPONENT BIT MASKS - Reset Reg 1                                 */
/* ========================================================================== */
#define BD_RESET_1553_CH7_IMP_BCMRT                         0x80000000
#define BD_RESET_1553_CH7_MT                                0x40000000
#define BD_RESET_1553_CH7_RT                                0x20000000
#define BD_RESET_1553_CH7_BC                                0x10000000

#define BD_RESET_1553_CH6_IMP_BCMRT                         0x08000000
#define BD_RESET_1553_CH6_MT                                0x04000000
#define BD_RESET_1553_CH6_RT                                0x02000000
#define BD_RESET_1553_CH6_BC                                0x01000000

#define BD_RESET_1553_CH5_IMP_BCMRT                         0x00800000
#define BD_RESET_1553_CH5_MT                                0x00400000
#define BD_RESET_1553_CH5_RT                                0x00200000
#define BD_RESET_1553_CH5_BC                                0x00100000

#define BD_RESET_1553_CH4_IMP_BCMRT                         0x00080000
#define BD_RESET_1553_CH4_MT                                0x00040000
#define BD_RESET_1553_CH4_RT                                0x00020000
#define BD_RESET_1553_CH4_BC                                0x00010000

#define BD_RESET_1553_CH3_IMP_BCMRT                         0x00008000
#define BD_RESET_1553_CH3_MT                                0x00004000
#define BD_RESET_1553_CH3_RT                                0x00002000
#define BD_RESET_1553_CH3_BC                                0x00001000

#define BD_RESET_1553_CH2_IMP_BCMRT                         0x00000800
#define BD_RESET_1553_CH2_MT                                0x00000400
#define BD_RESET_1553_CH2_RT                                0x00000200
#define BD_RESET_1553_CH2_BC                                0x00000100

#define BD_RESET_1553_CH1_IMP_BCMRT                         0x00000080
#define BD_RESET_1553_CH1_MT                                0x00000040
#define BD_RESET_1553_CH1_RT                                0x00000020
#define BD_RESET_1553_CH1_BC                                0x00000010

#define BD_RESET_1553_CH0_IMP_BCMRT                         0x00000008
#define BD_RESET_1553_CH0_MT                                0x00000004
#define BD_RESET_1553_CH0_RT                                0x00000002
#define BD_RESET_1553_CH0_BC                                0x00000001

/* ========================================================================== */
/* BD RESET COMPONENT BIT MASKS  - Reset Reg 2                                */
/* ========================================================================== */
#define BD_RESET_1553_CH7_IMP_MRT                           0x00008000
#define BD_RESET_1553_CH7_IMP_BC                            0x00004000
#define BD_RESET_1553_CH6_IMP_MRT                           0x00002000
#define BD_RESET_1553_CH6_IMP_BC                            0x00001000
#define BD_RESET_1553_CH5_IMP_MRT                           0x00000800
#define BD_RESET_1553_CH5_IMP_BC                            0x00000400
#define BD_RESET_1553_CH4_IMP_MRT                           0x00000200
#define BD_RESET_1553_CH4_IMP_BC                            0x00000100
#define BD_RESET_1553_CH3_IMP_MRT                           0x00000080
#define BD_RESET_1553_CH3_IMP_BC                            0x00000040
#define BD_RESET_1553_CH2_IMP_MRT                           0x00000020
#define BD_RESET_1553_CH2_IMP_BC                            0x00000010
#define BD_RESET_1553_CH1_IMP_MRT                           0x00000008
#define BD_RESET_1553_CH1_IMP_BC                            0x00000004
#define BD_RESET_1553_CH0_IMP_MRT                           0x00000002
#define BD_RESET_1553_CH0_IMP_BC                            0x00000001

/* ========================================================================== */
/* BD RESET COMPONENT BIT MASKS  - Reset Reg 3                                */
/* ========================================================================== */
#define BD_RESET_1553_CH7_REPLAY                            0x00000100
#define BD_RESET_1553_CH6_REPLAY                            0x00000080
#define BD_RESET_1553_CH5_REPLAY                            0x00000040
#define BD_RESET_1553_CH4_REPLAY                            0x00000020
#define BD_RESET_1553_CH3_REPLAY                            0x00000010
#define BD_RESET_1553_CH2_REPLAY                            0x00000008
#define BD_RESET_1553_CH1_REPLAY                            0x00000004
#define BD_RESET_1553_CH0_REPLAY                            0x00000001

/* ========================================================================== */
/* BD RESET COMPONENT BIT MASKS  - Reset Reg 4                                */
/* ========================================================================== */
#define BD_RESET_1553_CH7_TRIGGER                           0x00000100
#define BD_RESET_1553_CH6_TRIGGER                           0x00000080
#define BD_RESET_1553_CH5_TRIGGER                           0x00000040
#define BD_RESET_1553_CH4_TRIGGER                           0x00000020
#define BD_RESET_1553_CH3_TRIGGER                           0x00000010
#define BD_RESET_1553_CH2_TRIGGER                           0x00000008
#define BD_RESET_1553_CH1_TRIGGER                           0x00000004
#define BD_RESET_1553_CH0_TRIGGER                           0x00000001

/* ========================================================================== */
/* BD VOLTAGE MONITOR X8 COMPONENT                                            */
/* ========================================================================== */
#define BD_VOLT_MON_X8__CONTROL_REG                         0x00000000
#define BD_VOLT_MON_X8__CHANNEL_ENABLE_REG                  0x00000001
#define BD_VOLT_MON_X8__STATUS_REG                          0x00000002
#define BD_VOLT_MON_X8__CHANNEL_STATUS_REG                  0x00000003

#define BD_VOLT_MON_X8__STATUS_SAMPLING_COMPLETE            0x80000000
#define BD_VOLT_MON_X8__STATUS_SAMPLING_IN_PROGRESS         0x40000000

#define BD_VOLT_MON_X8_CONTROL__START_CAPTURE               0x80000000

/* ========================================================================== */
/* 1553 GENERAL COMPONENT REGISTERS    32-BIT ADDRESSABLE(DW)                 */
/* ========================================================================== */
#define REG_GENERAL_CTRL                                    0x00000000
#define REG_GENERAL_CTRL_PULSE                              0x00000001
#define REG_GENERAL_TT_MSB                                  0x00000002
#define REG_GENERAL_TT_LSB                                  0x00000003
#define REG_GENERAL_TT_LATCHED_MSB                          0x00000004
#define REG_GENERAL_TT_LATCHED_LSB                          0x00000005
#define REG_GENERAL_INT_STATUS                              0x00000006
#define REG_GENERAL_INT_MASK                                0x00000007
#define REG_GENERAL_RAM_SELF_TEST_STATUS                    0x00000008
#define REG_GENERAL_RAM_SIZE                                0x00000009
#define REG_GENERAL_TRANSCEIVER_DELAY                       0x0000000A

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_CTRL BIT MASKS                             */
/* ========================================================================== */
#define GENERAL_CTRL_RESERVED_MASK_31_24                    0xFF000000

#define GENERAL_CTRL_CHA_EXTERNAL_TX_INHIBIT                0x00800000
#define GENERAL_CTRL_CHB_EXTERNAL_TX_INHIBIT                0x00400000
#define GENERAL_CTRL_CHA_INTERNAL_TX_INHIBIT                0x00200000
#define GENERAL_CTRL_CHB_INTERNAL_TX_INHIBIT                0x00100000

#define GENERAL_CTRL_RESERVED_MASK_31_20                    0xFFF03000
#define GENERAL_CTRL_EXT_CHA_CHB_LOOP_TEST_ENA              0x00080000
#define GENERAL_CTRL_CHA_TRANS_SLEEP_MODE_ENA               0x00040000
#define GENERAL_CTRL_CHB_TRANS_SLEEP_MODE_ENA               0x00020000
#define GENERAL_CTRL_HOST_MEM_WORD_FLIP_ENABLE              0x00010000

#define GENERAL_CTRL_IRIGB_TIMETAG_ENABLE                   0x00002000
#define GENERAL_CTRL_BC_LOAD_ENABLE                         0x00001000
#define GENERAL_CTRL_SYNC_WITH_DATA_ENABLE                  0x00000800
#define GENERAL_CTRL_SYNC_NO_DATA_ENABLE                    0x00000400
#define GENERAL_CTRL_EXT_TT_COUNT_ENABLE                    0x00000200
#define GENERAL_CTRL_EXT_TT_LOAD_PULSE_ENABLE               0x00000100

#define GENERAL_CTRL_TT_ROLLOVER_POINT_MASK                 0x000000F0
#define GENERAL_CTRL_TT_ROLLOVER_POINT_16_BIT               0x00000000
#define GENERAL_CTRL_TT_ROLLOVER_POINT_17_BIT               0x00000080
#define GENERAL_CTRL_TT_ROLLOVER_POINT_18_BIT               0x00000100
#define GENERAL_CTRL_TT_ROLLOVER_POINT_19_BIT               0x00000180
#define GENERAL_CTRL_TT_ROLLOVER_POINT_20_BIT               0x00000200
#define GENERAL_CTRL_TT_ROLLOVER_POINT_21_BIT               0x00000280
#define GENERAL_CTRL_TT_ROLLOVER_POINT_22_BIT               0x00000300
#define GENERAL_CTRL_TT_ROLLOVER_POINT_48_BIT               0x00000380
#define GENERAL_CTRL_TT_ROLLOVER_POINT_LSHIFT_ALIGN         4

#define GENERAL_CTRL_TT_RESOLUTION_MASK                     0x0000000F
#define GENERAL_CTRL_TT_RESOLUTION_64US                     0x00000000
#define GENERAL_CTRL_TT_RESOLUTION_32US                     0x00000010
#define GENERAL_CTRL_TT_RESOLUTION_16US                     0x00000020
#define GENERAL_CTRL_TT_RESOLUTION_08US                     0x00000030
#define GENERAL_CTRL_TT_RESOLUTION_04US                     0x00000040
#define GENERAL_CTRL_TT_RESOLUTION_02US                     0x00000050
#define GENERAL_CTRL_TT_RESOLUTION_01US                     0x00000060
#define GENERAL_CTRL_TT_RESOLUTION_BIT_SHIFT_ALIGN          0

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_CTRL_PULSE BIT MASKS                       */
/* ========================================================================== */
#define GENERAL_CTRL_PULSE_CLEAR_RAM_SELFTEST               0x00000008
#define GENERAL_CTRL_PULSE_START_RAM_SELFTEST               0x00000004
#define GENERAL_CTRL_PULSE_TIMETAG_TEST_CLK                 0x00000002
#define GENERAL_CTRL_PULSE_TIMETAG_RESET                    0x00000001

#define ACEX_1553_GENCTRL_PULSE_IMP_RESET                   0x00000020
#define ACEX_1553_GENCTRL_PULSE_MT_RESET                    0x00000010
#define ACEX_1553_GENCTRL_PULSE_MRT_RESET                   0x00000008

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_TT_MSB BIT MASKS                           */
/* ========================================================================== */
#define GENERAL_TT_MSB_RESERVED_MASK_31_16                  0xFFFF0000
#define GENERAL_TT_MSB_MASK                                 0x0000FFFF

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_TT_LSB BIT MASKS                           */
/* ========================================================================== */
#define GENERAL_TT_LSB_MASK                                 0xFFFFFFFF

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_TT_LATCHED_MSB BIT MASKS                   */
/* ========================================================================== */
#define GENERAL_TT_MSB_RESERVED_LATCHED_MASK_31_16          0xFFFF0000
#define GENERAL_TT_MSB_LATCHED_MASK                         0x0000FFFF

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_TT_LATCHED_LSB BIT MASKS                   */
/* ========================================================================== */
#define GENERAL_TT_LSB_LATCHED_MASK                         0xFFFFFFFF

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_INT_STATUS BIT MASKS                       */
/* ========================================================================== */
#define GENERAL_INT_STATUS_MASK_31_11                       0xFFFFF800
#define GENERAL_INT_STATUS_TRG_INT_ENABLED                  0x00000400
#define GENERAL_INT_STATUS_REPLAY_INT_ENABLED               0x00000200
#define GENERAL_INT_STATUS_MRT_IMP_INT_ENABLED              0x00000100
#define GENERAL_INT_STATUS_BC_IMP_INT_ENABLED               0x00000080
#define GENERAL_INT_STATUS_BCMRT_IMP_INT_ENABLED            0x00000040
#define GENERAL_INT_STATUS_MT_INT_ENABLED                   0x00000020
#define GENERAL_INT_STATUS_RT_INT_ENABLED                   0x00000010
#define GENERAL_INT_STATUS_BC_INT_ENABLED                   0x00000008
#define GENERAL_INT_STATUS_RAM_SELF_TEST_DONE               0x00000004
#define GENERAL_INT_STATUS_RAM_PARITY_DETECTED              0x00000002
#define GENERAL_INT_STATUS_TT_ROLLOVER                      0x00000001

/* ========================================================================== */
/* 1553 MAIN COMPONENT REG_GENERAL_RAM_SELF_TEST_STATUS BIT MASKS             */
/* ========================================================================== */
#define GENERAL_RAM_SELF_TEST_STATUS_RESERVED_MASK_31_03    0xFFFFFFF8
#define GENERAL_RAM_SELF_TEST_STATUS_DONE                   0x00000004
#define GENERAL_RAM_SELF_TEST_STATUS_IN_PROGRESS            0x00000002
#define GENERAL_RAM_SELF_TEST_STATUS_PASSED                 0x00000001

/* ========================================================================== */
/* 1553 IMPROVEMENTS COMPONENT REGISTERS    32-BIT ADDRESSABLE(DW)            */
/* ========================================================================== */

#define ACEX_1553_IMP_REG_INPUT_DATA_Q_LO                   0x00000000  /* WR    */
#define ACEX_1553_IMP_REG_INPUT_DATA_Q_HI                   0x00000001  /* WR    */
#define ACEX_1553_IMP_REG_IMP_BLK_TRIG                      0x00000002  /* WR    */
#define ACEX_1553_IMP_REG_INT_EN_MASK                       0x00000003  /* RD/WR */
#define ACEX_1553_IMP_REG_INT_NUM_WRDS                      0x00000004  /* RD/WR */
#define ACEX_1553_IMP_REG_INT_NUM_QUEUE_TFR                 0x00000005  /* RD/WR */
#define ACEX_1553_IMP_REG_INT_BLK_TIME                      0x00000006  /* RD/WR */
#define REG_IMP_RT_CMD_MSG_TIMEOUT                          0x00000007  /* RD/WR */
#define ACEX_1553_IMP_REG_TARGET_MEM_BA                     0x00000008  /* RD/WR */
#define ACEX_1553_IMP_REG_TARGET_MEM_SIZE                   0x00000009  /* RD/WR */
#define ACEX_1553_IMP_REG_IGNORE_TXRX_DATA                  0x0000000A  /* RD/WR */
#define ACEX_1553_IMP_REG_OUT_FIFO_STATUS                   0x0000000B  /* RD    */
#define ACEX_1553_IMP_REG_OUTPUT_DATA_Q_LO                  0x0000000C  /* RD    */
#define ACEX_1553_IMP_REG_OUTPUT_DATA_Q_HI                  0x0000000D  /* RD    */
#define ACEX_1553_IMP_REG_INT_STATUS                        0x0000000E  /* RD    */
#define REG_IMP_RT_CMD_STK_PTR                              0x00000010  /* RD    */

#define ACEX_1553_IMP_REG_IN_FIFO_STATUS                    0x0000000F  /* RD    */
#define REG_IMP_RT_CMD_STK_PTR_LST_RD                       0x00000010  /* RD    */
#define REG_IMP_RT_CMD_STK_DWRDS_LEFT                       0x00000011  /* RD    */
#define REG_IMP_RT_GLBL_STK_PTR_LST_RD                      0x00000012  /* RD    */
#define REG_IMP_MT_CMD_STK_PTR_LST_RD                       0x00000013  /* RD    */
#define REG_IMP_REG_MT_CMD_STK_DWRDS_LEFT                   0x00000014  /* RD    */

/* IMP memory size for Q-Prime based devices */
#define ACEX_SF_1553_IMP_MEMSIZE_DWD_QPRM                   0x2000     /* 32 KBytes -  8K DWords */
#define ACEX_MF_1553_IMP_MEMSIZE_DWD_QPRM                   0x4000     /* 64 KBytes - 16K DWords */

/* IMP memory size for other based devices */
#define ACEX_SF_1553_IMP_MEMSIZE_DWD                        0x4000     /* 64 KBytes -  16K DWords */
#define ACEX_MF_1553_IMP_MEMSIZE_DWD                        0x8000     /* 128 KBytes - 32K DWords */

#define REPLAY_MEMSIZE_DWD                                  0x8000     /* 128 KBytes - 32K DWords */

/* ========================================================================== */
/* 1553 IMPROVEMENTS COMPONENT BIT MASKS                                      */
/* ========================================================================== */

#define ACEX_1553_IMP_INT_EN_MASK_HOST_INITIATE             0x00000001
#define ACEX_1553_IMP_INT_EN_MASK_NUM_QUEUE_TFRS            0x00000002
#define ACEX_1553_IMP_INT_EN_MASK_MAX_WDS                   0x00000004
#define ACEX_1553_IMP_INT_EN_MASK_NUM_WDS                   0x00000008
#define ACEX_1553_IMP_INT_EN_MASK_BLOCK_TIME                0x00000010
#define ACEX_1553_IMP_INT_EN_MASK_MTI_TO                    0x00000020
#define ACEX_1553_IMP_INT_EN_MASK_MRTI_TO                   0x00000040

#define ACEX_1553_IMP_BLK_MASK_BLK_TRIGGER                  0x00000001
#define ACEX_1553_IMP_BLK_MASK_HOST_INITIATE                0x00000002

#define IMP_IGNORE_TXRX_WAIT_FOR_EOM                        0x00000001
#define IMP_IGNORE_TXRX_RESET_OUT_FIFO                      0x00000002
#define IMP_IGNORE_TXRX_MRT_WAIT_FOREVER                    0x00000004
#define IMP_IGNORE_TXRX_IMP_INPUT_CTRL                      0x00000008
#define IMP_IGNORE_TXRX_BC_INPUT_CTRL                       0x00000000

#define IMP_OUTFIFO_STATUS_NUM_ENTRIES                      0x000000FF
#define IMP_OUTFIFO_STATUS_EMPTY                            0x00000200
#define IMP_OUTFIFO_STATUS_FULL                             0x00000100

#define ACEX_1553_IMP_OUTFIFO_DQLO_ADDR_START               0x000FFFFF
#define ACEX_1553_IMP_OUTFIFO_DQLO_NUM_CMDS                 0xFFF00000

#define ACEX_1553_IMP_OUTFIFO_DQHI_NUM_WDS                  0x0000FFFF
#define ACEX_1553_IMP_OUTFIFO_DQHI_COND_STK_OVRFLW          0x00010000
#define ACEX_1553_IMP_OUTFIFO_DQHI_COND_NUM_WDS             0x00020000
#define ACEX_1553_IMP_OUTFIFO_DQHI_COND_NUM_CMD             0x00040000
#define ACEX_1553_IMP_OUTFIFO_DQHI_COND_EOM_NOTSET          0x00080000
#define ACEX_1553_IMP_OUTFIFO_DQHI_STKCMD_MOVED             0x00100000
#define ACEX_1553_IMP_OUTFIFO_DQHI_HOST_ID                  0xFF000000

/* ========================================================================== */
/* 1553 MRT COMPONENT REGISTERS  32-BIT ADDRESSABLE(DW)                       */
/* ========================================================================== */
#define REG_MRT_GCONFIG                                     0x00000000
#define REG_MRT_RT_ENABLE                                   0x00000001
#define REG_MRT_BRDCST_SEL_CTRL                             0x00000002  /* broadcast selected control */
#define REG_MRT_CMD_STK_PTR                                 0x00000003
#define REG_MRT_GBL_DATA_STK_PTR                            0x00000004
#define REG_MRT_ISQP_RW                                     0x00000008
#define REG_MRT_REG_INT_ENABLE_MASK                         0x0000001A
#define REG_MRT_REG_INT_STATUS                              0x0000001B
#define REG_MRT_CMD_STK_ENTRIES                             0x0000001C

/* ========================================================================== */
/* 1553 MRT COMPONENT BIT MASKS                                               */
/* ========================================================================== */
#define MRT_GCONFIG_ADDR_SHIFT                              23

#define MRT_GCONFIG_MODULE_EN                               0x40000000
#define MRT_GCONFIG_MRT_MODE_EN                             0x20000000
#define MRT_GCONFIG_SRT_ADDRESS_SOURCE                      0x10000000
#define MRT_GCONFIG_SRT_ADDRESS                             0x0F800000
#define MRT_GCONFIG_SRT_ADDRESS_PARITY                      0x00400000
#define MRT_GCONFIG_SRT_ADDRESS_LATCH_TRK                   0x00200000
#define MRT_GCONFIG_LATCH_SRT_ADDRESS                       0x00100000
#define MRT_GCONFIG_MASK_MODE_CODE_RESET                    0x00080000
#define MRT_GCONFIG_MASK_BRODCAST_DISABLED                  0x00040000
#define MRT_GCONFIG_MASK_RT_CONFIGURED                      0x00020000
#define MRT_GCONFIG_GBL_DSTK_EN                             0x00000010
#define MRT_GCONFIG_INT_STATUS_QUEUE_EN                     0x00000800
#define MRT_GCONFIG_MASK_RT_CMD_STK_SZ                      0x000000E0

#define MRT_INT_ENABLE_MASK_EOM                             0x00000001
#define MRT_INT_ENABLE_MASK_MODE_CODE                       0x00000002
#define MRT_INT_ENABLE_MASK_FORMAT_ERROR                    0x00000004
#define MRT_INT_ENABLE_MASK_SA_CTRLWD_EOM                   0x00000008
#define MRT_INT_ENABLE_MASK_CBUF_ROLLOVER                   0x00000010
#define MRT_INT_ENABLE_MASK_ADDR_PARITY                     0x00000020
#define MRT_INT_ENABLE_MASK_CMD_STK_ROLLOVER                0x00000040
#define MRT_INT_ENABLE_MASK_XMTR_TIMEOUT                    0x00000080
#define MRT_INT_ENABLE_MASK_CBUF_50_ROLLOVER                0x00000100
#define MRT_INT_ENABLE_MASK_CMDSTK_50_ROLLOVER              0x00000200
#define MRT_INT_ENABLE_MASK_ISQ_ROLLOVER                    0x00000400
#define MRT_INT_ENABLE_MASK_ILLEGAL_CMD                     0x00000800
#define MRT_INT_ENABLE_MASK_CMD_STK_OVERFLOW                0x00001000

/* ========================================================================== */
/* 1553 RT COMPONENT CONFIGURATION OFFSETS (MEMORY)  32-BIT ADDRESSABLE(DW)   */
/* ========================================================================== */
#define MEM_RT_ILL_TABLE_BA                                 0x00000000
#define MEM_RT_SA_CTRL_BA                                   0x00000040
#define MEM_RT_IMP_RXDP_TABLE_BA                            0x00000060
#define MEM_RT_IMP_TXDP_TABLE_BA                            0x00000080
#define MEM_RT_MC_DATA_BA                                   0x000000A0
#define MEM_RT_CONFIG_BA                                    0x000000B0
#define MEM_RT_STATUS_INCTRL_TABLE_BA                       0x000000B1
#define MEM_RT_BB_RX_TABLE_BA                               0x000000B2
#define MEM_RT_BB_TX_TABLE_BA                               0x000000B3
#define MEM_RT_MC_SEL_RX_INT_BA                             0x000000B4
#define MEM_RT_MC_SEL_TX_INT_BA                             0x000000B5
#define MEM_RT_NO_RESP_TIMEOUT                              0x000000B6
#define MEM_RT_MC_LAST_CMD_STS                              0x000000B7
#define MEM_RT_MODE_LAST_BITWD_BA                           0x000000B8  /* not public */
#define MEM_RT_DBC_HOLDOFF_TIME_BA                          0x000000B9
#define MEM_RT_IMR_TRIG_SELECT_BA                           0x000000BA
#define MEM_RT_IMR_MODE_CODE_BA                             0x000000BB
#define MEM_RT_RXDP_TABLE_BA                                0x000000C0
#define MEM_RT_TXDP_TABLE_BA                                0x000000E0

/* cmd stack sizes */
#define RT_BSW_OFFSET                                       0           /* Block Status Word */
#define RT_TT_OFFSET                                        1           /* TimeTag */
#define RT_DP_OFFSET                                        2           /* Data Pointer */
#define RT_CMD_OFFSET                                       3           /* Received CMD word */
#define RT_CMD_SIZE                                         4

/* ========================================================================== */
/* 1553 RT CONFIG BIT MASKS (MEMORY)                                          */
/* ========================================================================== */
#define RT_CONFIG_RT_FAIL_FLAG_WRAP_EN                      0x00000001
#define RT_CONFIG_CLR_SERV_REQUEST                          0x00000002
#define RT_CONFIG_RT_HALT                                   0x00000004
#define RT_CONFIG_ENH_TT_SYNC                               0x00000008
#define RT_CONFIG_LD_TT_ON_MODE_SYNC                        0x00000010
#define RT_CONFIG_CLR_TT_ON_MODE_SYNC                       0x00000020
#define RT_CONFIG_DONT_POST_ON_CMD_STK                      0x00000040
#define RT_CONFIG_BRDCST_DIS                                0x00000080
#define RT_CONFIG_ISQ_DIS_VALID_MSG                         0x00000100
#define RT_CONFIG_ISQ_DIS_INVALID_MSG                       0x00000200
#define RT_CONFIG_EXT_BITWD_IF_BUSY                         0x00000400
#define RT_CONFIG_INHIB_BITWD_IF_BUSY                       0x00000800
#define RT_CONFIG_MODE_CMDS_OVERRIDE_BUSY                   0x00001000
#define RT_CONFIG_ALT_STATUS_WD_EN                          0x00002000
#define RT_CONFIG_1553A_ERROR_RESP_EN                       0x00004000
#define RT_CONFIG_1553A_MC_EN                               0x00008000
#define RT_CONFIG_GAP_CHK_EN                                0x00010000
#define RT_CONFIG_BUSY_RX_TFR_DIS                           0x00020000
#define RT_CONFIG_ILL_RX_TFR_DIS                            0x00040000
#define RT_CONFIG_BUSY_LKUP_TBL_EN                          0x00080000
#define RT_CONFIG_ILL_DIS                                   0x00100000
#define RT_CONFIG_RX_DB_EN                                  0x00200000

#define RT_CONFIG_NO_RESP_TIMEOUT_MASK                      0x03FF0000
#define RT_CONFIG_NO_RESP_TIMEOUT_SHIFT                     16

/* ========================================================================== */
/* 1553 BUS CONTROLLER (BC) COMPONENT REGISTERS - 32-BIT ADDRESSABLE(DW)      */
/* ========================================================================== */
#define REG_BC_INT_MASK                                     0x00000000
#define REG_BC_CONFIG                                       0x00000001
#define REG_BC_START_RESET                                  0x00000002
#define REG_BC_INST_PTR_READ                                0x00000002
#define REG_BC_INT_STS                                      0x00000003
#define REG_BC_FRAME_TIME_REMAINING                         0x00000004
#define REG_BC_MSG_TIME_REMAINING                           0x00000005
#define REG_BC_INIT_INST_PTR                                0x00000006
#define REG_BC_COND_CODE                                    0x00000007
#define REG_BC_GP_FLAG                                      0x00000007
#define REG_BC_GPQ                                          0x00000008
#define REG_BC_HP_ASYNC_STS                                 0x00000009
#define REG_BC_HP_ASYNC_NOMP                                0x00000009
#define REG_BC_HP_ASYNC_CTRL                                0x0000000A
#define REG_BC_TEST_MODE                                    0x0000000B
#define REG_BC_HOST_ACCESS_CTRL                             0x0000000C
#define REG_BC_TEST_NOT_USED                                0x0000000D
#define REG_BC_LP_ASYNC_STS                                 0x0000000E
#define REG_BC_LP_ASYNC_NOMP                                0x0000000E
#define REG_BC_LP_ASYNC_CTRL                                0x0000000F

/* ========================================================================== */
/* 1553 BC COMPONENT: REG_BC_INT_MASK REGISTER BITS                           */
/* ========================================================================== */
#define BC_INT_MASK_UIRQ0                                   0x00000200
#define BC_INT_MASK_UIRQ1                                   0x00000400
#define BC_INT_MASK_UIRQ2                                   0x00000800
#define BC_INT_MASK_UIRQ3                                   0x00001000
#define BC_INT_MASK_LP_HALF                                 0x00004000
#define BC_INT_MASK_LP_FULL                                 0x00008000
#define BC_INT_MASK_HP_HALF                                 0x00010000
#define BC_INT_MASK_HP_FULL                                 0x00020000
#define BC_INT_MASK_UIRQ4                                   0x00040000  /* DDC internal use only */
#define BC_INT_MASK_IMP_OVERFLOW                            0x00080000

/* ========================================================================== */
/* 1553 BC COMPONENT: REG_BC_CONFIG REGISTER BITS                             */
/* ========================================================================== */
#define BC_CONFIG_HPQ_ENABLE                                0x00000010
#define BC_CONFIG_LPQ_ENABLE                                0x00000020

/* ========================================================================== */
/* BC CONFIG response time out                                                */
/* ========================================================================== */
/* response timeout */
#define BC_CONFIG_RESP_TIMEOUT_MASK                         0x1FF80000
#define BC_CONFIG_RESP_TIMEOUT_SHIFT                        19

/* ========================================================================== */
/* 1553 REPLAY COMPONENT REGISTERS  32-BIT ADDRESSABLE(DW)                    */
/* ========================================================================== */
#define REG_REPLAY_BUFFER_ADDRESS                           0x00000000
#define REG_REPLAY_BUFFER_LENGTH                            0x00000001
#define REG_REPLAY_BC_DISABLE                               0x00000002
#define REG_REPLAY_RT_DISABLE                               0x00000003
#define REG_REPLAY_CONFIG                                   0x00000004
#define REG_REPLAY_IRQ_MASK                                 0x00000005
#define REG_REPLAY_CONTROL                                  0x00000009
#define REG_REPLAY_IRQ_STATUS                               0x0000000A
#define REG_REPLAY_STATE                                    0x0000000B
#define REG_REPLAY_CURRENT_ADDR                             0x0000000C

/* ========================================================================== */
/* 1553 REPLAY COMPONENT: REG_REPLAY_INT_MASK REGISTER BITS                   */
/* ========================================================================== */
#define REPLAY_INT_MASK_50PCT_MEM                           0x00000001
#define REPLAY_INT_MASK_100PCT_MEM                          0x00000002
#define REPLAY_INT_COMPLETE                                 0x00000004

/* ========================================================================== */
/* 1553 MT COMPONENT REGISTERS - 32-BIT ADDRESSABLE(DW)                       */
/* ========================================================================== */
#define REG_MT_CONFIG_RW                                    0x00000000
#define REG_MT_STROBE_W                                     0x00000001
#define REG_MT_STATUS_R                                     0x00000001
#define REG_MT_MTI_STK_ADDRESS_RW                           0x00000002
#define REG_MT_MTI_FREE_MEM_COUNT_RW                        0x00000003
#define REG_MT_MTI_INT_ENABLE_RW                            0x00000004
#define REG_MT_MTI_INT_SET_NUMBER_OF_WORDS_RW               0x00000005
#define REG_MT_MTI_INT_SET_NUMBER_OF_MSGS_RW                0x00000006
#define REG_MT_MTI_INT_SET_TIME_INTERVAL_RW                 0x00000007
#define REG_MT_MTI_INT_QUEUE_STATUS_RW                      0x00000008
#define REG_MT_MTI_INT_QUEUE_COUNT_RW                       0x00000009
#define REG_MT_MTI_INT_STATUS_RW                            0x0000000A
#define REG_MT_MTI_FIRST_MSG_ADDRESS_RW                     0x0000000B
#define REG_MT_MTI_TOTAL_LENGTH_RW                          0x0000000C
#define REG_MT_MTI_NUMBER_OF_MSGS_RW                        0x0000000D
#define REG_MT_MTI_NUMBER_OF_DROPPED_MSGS_RW                0x0000000E
#define REG_MT_TEST0_CONTROL_SIGS_RW                        0x00000010  /* Control Signals      */
#define REG_MT_TEST1_CHA_DEC_SIGS_RW                        0x00000011  /* Ch A docoder Signals */
#define REG_MT_TEST1_CHB_DEC_SIGS_RW                        0x00000012  /* Ch B docoder Signals */

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_CONFIG_RW BIT MASKS                               */
/* ========================================================================== */
#define MT_CONFIG_MODE_MASK                                0x00000000
#define MT_CONFIG_MODE_MTIE_ENABLE                         0x80000000
#define MT_CONFIG_MTI_BLOCK_STATUS_ENABLE                  0x40000000
#define MT_CONFIG_MTI_SELECTIVE_MONITOR_DISABLE            0x20000000
#define MT_CONFIG_CHA_DISABLE                              0x10000000
#define MT_CONFIG_CHB_DISABLE                              0x08000000
#define MT_CONFIG_GAP_CHECK_ENABLE                         0x04000000
#define MT_CONFIG_BROADCAST_DISABLE                        0x02000000
#define MT_CONFIG_1553A_MCODES_ENABLE                      0x01000000
#define MT_CONFIG_OVERRIDE_MC_TR_ERR_ENABLE                0x00800000
#define MT_CONFIG_BUSY_BIT_NODATA_VALID_FORMAT_ENABLE      0x00400000
#define MT_CONFIG_EOM_TTAG_ENABLE                          0x00200000
#define MT_CONFIG_BWE_SWAP_ENABLE                          0x00100000
#define MT_CONFIG_BUS_SWITCH_EOM_DISABLE                   0x00080000
#define MT_CONFIG_MODE_MTIR_ENABLE                         0x00040000
#define MT_CONFIG_TRIGGER_START_ENABLE                     0x00020000
#define MT_CONFIG_TRIGGER_STOP_ENABLE                      0x00010000

/* Each bit represents 0.5us of timeout resolution                          */
/* (Response Timeout = RT_NO_RESPONSE_TIMEOUT(15:08) X 0.5us). max 128us    */
#define MT_CONFIG_NO_RESP_TIMEOUT_MASK                     0x0000FF00
#define MT_CONFIG_NO_RESP_TIMEOUT_BIT_ALIGN                8
#define MT_CONFIG_NO_RESP_TIMEOUT_DEFAULT                  0x00002500  /* 18.5us */

#define MT_CONFIG_RESERVED_07                              0x00000080
#define MT_CONFIG_RESERVED_06                              0x00000040
#define MT_CONFIG_RESERVED_05                              0x00000020
#define MT_CONFIG_RESERVED_04                              0x00000020

/* Default value is 0 (256 dwords which equates to 128 messages) */
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_MASK              0x0000000F
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_256_DW            0x00000000
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_512_DW            0x00000001
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_1024_DW           0x00000002
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_2048_DW           0x00000003
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_4096_DW           0x00000004
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_8192_DW           0x00000005
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_16384_DW          0x00000006
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_32768_DW          0x00000007
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_65536_DW          0x00000008
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_131072_DW         0x00000009
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_262144_DW         0x0000000A
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_524288_DW         0x0000000B
#define MT_CONFIG_MT_CMD_DATA_STACK_SIZE_1048576_DW        0x0000000C

/* ========================================================================== */
/* 1553  MT COMPONENT REG_MT_STROBE_W/REG_MT_STATUS_R BIT MASKS               */
/* ========================================================================== */
#define MT_STROBE_RESERVED_MASK_31_05                      0xFFFFFFE0

/* Write masks */
#define MT_STROBE_HOST_TRIG_INT                            0x00000008
#define MT_STROBE_PAUSE_MONITOR                            0x00000004
#define MT_STROBE_START_RESUME_MONITOR                     0x00000002
#define MT_STROBE_RESET_MONITOR                            0x00000001

/* Read masks */
#define MT_STATUS_READ_MTIE_ACTIVE                         0x00000010
#define MT_STATUS_READ_MONITOR_ACTIVE                      0x00000008
#define MT_STATUS_READ_MONITOR_PAUSED                      0x00000004
#define MT_STATUS_READ_MONITOR_PAUSE_PENDING               0x00000002
#define MT_STATUS_READ_MONITOR_ENABLED                     0x00000001

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_STK_ADDRESS_RW BIT MASKS                      */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_STK_ADDRESS_RESERVED_MASK_31_18             0xFFFC0000
#define MT_MTI_STK_ADDRESS_MASK                            0x0003FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_ENABLE_RW BIT MASKS                       */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_RESERVED_MASK_MASK31_06                 0xFFFFFC00
#define MT_MTI_INT_ON_ROLL_OVER_ENABLE                     0x00000020
#define MT_MTI_INT_BY_HOST_ENABLE                          0x00000010
#define MT_MTI_INT_ON_TIME_REACHED_BY_MSG_ENABLE           0x00000008
#define MT_MTI_INT_ON_TIME_REACHED_ENABLE                  0x00000004
#define MT_MTI_INT_ON_NUMBER_OF_MSGS_ENABLE                0x00000002
#define MT_MTI_INT_ON_NUMBER_OF_WORDS_ENABLE               0x00000001
#define MT_MTI_INT_DISABLE_ALL                             0x00000000

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_SET_NUMBER_OF_WORDS_RW BIT MASKS          */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_SET_NUMBER_OF_WORDS_MASK_RESERVED_31_18 0xFFFC0000
#define MT_MTI_INT_SET_NUMBER_OF_WORDS_MASK                0x0003FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_SET_NUMBER_OF_MSGS_RW BIT MASKS           */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_SET_NUMBER_OF_MSGS_RESERVED_MASK_31_16  0xFFFF0000
#define MT_MTI_INT_SET_NUMBER_OF_MSGS_MASK                 0x0000FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_SET_TIME_INTERVAL_RW BIT MASKS            */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_SET_TIME_INTERVAL_RESERVED_MASK_31_16   0xFFFF0000
#define MT_MTI_INT_SET_TIME_INTERVAL_MASK                  0x0000FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_QUEUE_STATUS_RW BIT MASKS                 */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_QUEUE_STATUS_RESERVED_MASK_31_16        0xFFFFFFFC
#define MT_MTI_INT_QUEUE_STATUS_FULL                       0x00000002
#define MT_MTI_INT_QUEUE_STATUS_EMPTY                      0x00000001

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_QUEUE_COUNT_RW BIT MASKS                  */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_QUEUE_COUNT_RESERVED_MASK_31_16         0xFFFFFFF0
#define MT_MTI_INT_QUEUE_COUNT_MASK                        0x0000000F

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_INT_STATUS_RW BIT MASKS                       */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_INT_STATUS_RESERVED_MASK_31_06              0xFFFFFFC0
#define MT_MTI_INT_STATUS_ON_OVERFLOW                      0x00000020
#define MT_MTI_INT_STATUS_BY_HOST                          0x00000010
#define MT_MTI_INT_STATUS_TIME_REACHED_BY_MSG              0x00000008
#define MT_MTI_INT_STATUS_TIME_REACHED                     0x00000004
#define MT_MTI_INT_STATUS_NUMBER_OF_MSGS                   0x00000002
#define MT_MTI_INT_STATUS_NUMBER_OF_WORDS                  0x00000001

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_FIRST_MSG_ADDRESS_RW BIT MASKS                */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_FIRST_MSG_ADDRESS_RESERVED_MASK_31_19       0xFFF80000
#define MT_MTI_FIRST_MSG_ADDRESS_MASK                      0x0007FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_TOTAL_LENGTH_RW BIT MASKS                     */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_TOTAL_LENGTH_RESERVED_MASK_31_19            0xFFF80000
#define MT_MTI_TOTAL_LENGTH_MASK                           0x0007FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_NUMBER_OF_MSGS_Rw BIT MASKS                   */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_NUMBER_OF_MSGS_RESERVED_MASK_31_16          0xFFFF0000
#define MT_MTI_NUMBER_OF_MSGS_MASK                         0x0000FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_MTI_NUMBER_OF_DROPPED_MSGS_RW BIT MASKS           */
/* MTi(E) Modes only                                                          */
/* ========================================================================== */
#define MT_MTI_NUMBER_OF_DROPPED_MSGS_RESERVED_MASK_31_16  0xFFFF0000
#define MT_MTI_NUMBER_OF_DROPPED_MSGS_MASK                 0x0000FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_TEST0_CONTROL_SIGS_RW BIT MASKS                   */
/* ========================================================================== */
#define MT_TEST0_CONTROL_SIGS_RESERVED_MASK_31_05          0xFFFFFFE0
#define MT_TEST0_CONTROL_SIGS_CHB_E2MHZ                    0x00000010
#define MT_TEST0_CONTROL_SIGS_CHA_E2MHZ                    0x00000008
#define MT_TEST0_CONTROL_SIGS_TIMEOUT                      0x00000004
#define MT_TEST0_CONTROL_SIGS_MT_TEST_MODE_TIMEOUT_ENABLE  0x00000002
#define MT_TEST0_CONTROL_SIGS_MT_TEST_MODE_ENABLE          0x00000001

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_TEST1_CHA_DEC_SIGS_RW BIT MASKS                   */
/* ========================================================================== */
#define MT_TEST1_CHA_DEC_SIGS_NW                           0x80000000
#define MT_TEST1_CHA_DEC_SIGS_VW                           0x40000000
#define MT_TEST1_CHA_DEC_SIGS_CMD_DATA                     0x20000000
#define MT_TEST1_CHA_DEC_SIGS_GAP                          0x10000000
#define MT_TEST1_CHA_DEC_SIGS_NDB1                         0x08000000
#define MT_TEST1_CHA_DEC_SIGS_PARRERR                      0x04000000
#define MT_TEST1_CHA_DEC_SIGS_MANERR                       0x02000000
#define MT_TEST1_CHA_DEC_SIGS_ACTERR                       0x01000000
#define MT_TEST1_CHA_DEC_SIGS_RESP_TIME_MASK               0x00FF0000
#define MT_TEST1_CHA_DEC_SIGS_RCVD_DATA_MASK               0x0000FFFF

/* ========================================================================== */
/* 1553 MT COMPONENT REG_MT_TEST1_CHB_DEC_SIGS_RW BIT MASKS                   */
/* ========================================================================== */
#define MT_TEST1_CHB_DEC_SIGS_NW                           0x80000000
#define MT_TEST1_CHB_DEC_SIGS_VW                           0x40000000
#define MT_TEST1_CHB_DEC_SIGS_CMD_DATA                     0x20000000
#define MT_TEST1_CHB_DEC_SIGS_GAP                          0x10000000
#define MT_TEST1_CHB_DEC_SIGS_NDB1                         0x08000000
#define MT_TEST1_CHB_DEC_SIGS_PARRERR                      0x04000000
#define MT_TEST1_CHB_DEC_SIGS_MARERR                       0x02000000
#define MT_TEST1_CHB_DEC_SIGS_ACTERR                       0x01000000
#define MT_TEST1_CHB_DEC_SIGS_RESP_TIME_MASK               0x00FF0000
#define MT_TEST1_CHB_DEC_SIGS_RCVD_DATA_MASK               0x0000FFFF

/* ========================================================================== */
/* 1553 BC COMPONENT REGISTER INTERRUPT MASK(DW)                              */
/* ========================================================================== */
#define REG_BC_EOM                                          0x00000001
#define REG_BC_STATUS_SET                                   0x00000002
#define REG_BC_FORMAT_ERROR                                 0x00000004
#define REG_BC_TRAP                                         0x00000008
#define REG_BC_SELECT_EOM                                   0x00000010
#define REG_BC_OPCODE_PARITY_ERROR                          0x00000020
#define REG_BC_GP_QUEUE_ROLLOVER                            0x00000040
#define REG_BC_GP_QUEUE_50P_ROLLOVER                        0x00000080
#define REG_BC_RETRY                                        0x00000100
#define REG_BC_IRQ_0                                        0x00000200
#define REG_BC_IRQ_1                                        0x00000400
#define REG_BC_IRQ_2                                        0x00000800
#define REG_BC_IRQ_3                                        0x00001000
#define REG_BC_CALL_STACK_ERROR                             0x00002000
#define REG_BC_LO_PRI_WRK_QUEUE_HALF_FULL                   0x00004000
#define REG_BC_LO_PRI_WRK_QUEUE_FULL                        0x00008000
#define REG_BC_HI_PRI_WRK_QUEUE_HALF_FULL                   0x00010000
#define REG_BC_HI_PRI_WRK_QUEUE_FULL                        0x00020000
#define REG_BC_IRQ4                                         0x00040000  /* BC internal bit      */
#define REG_BC_IMP_OVERFLOW                                 0x00080000  /* BC IMP internal use  */

/* ========================================================================== */
/* 1553 IRIGB COMPONENT REGISTERS  32-BIT ADDRESSABLE(DW)                     */
/* ========================================================================== */
#define REG_IRIGB_CONTROL_RW                                0x00000000
#define REG_IRIGB_CONTROL_STROBE_W                          0x00000001
#define REG_IRIGB_TIME_STAMP_MSB_RW                         0x00000002
#define REG_IRIGB_TIME_STAMP_LSB_RW                         0x00000003  /* Must read 1st! */
#define REG_IRIGB_LATCHED_1SEC_TIME_STAMP_MSB_RW            0x00000004
#define REG_IRIGB_LATCHED_1SEC_TIME_STAMP_LSB_RW            0x00000005

/* ========================================================================== */
/* 1553 IRIGB COMPONENT REG_IRIGB_CONTROL_RW BIT MASKS                        */
/* ========================================================================== */
#define IRIGB_CONTROL_RESERVED_MASK_31_03                   0xFFFFFFF8
#define IRIGB_CONTROL_1_SEC_TIME_INT_ENABLE                 0x00000004
#define IRIGB_CONTROL_DMY_FORMAT_ENABLE                     0x00000002
#define IRIGB_CONTROL_CLOCK_ENABLE                          0x00000001

#define IRIGB_CONTROL_IRIGB_FORMAT_MASK                     0x0000000E
#define IRIGB_CONTROL_IRIGB_FORMAT_INT_48_BIT               0x00000000

/* ========================================================================== */
/* 1553 IRIGB COMPONENT REG_IRIGB_CONTROL_STROBE_W BIT MASKS                  */
/* ========================================================================== */
#define IRIGB_CONTROL_STROBE_RESERVED_MASK_31_04            0xFFFFE000
#define IRIGB_CONTROL_STROBE_TTAG_RESET_ENABLE              0x00000008
#define IRIGB_CONTROL_STROBE_TEST_WR_16MSBS_ENABLE          0x00000004
#define IRIGB_CONTROL_STROBE_TEST_WR_32LSBS_ENABLE          0x00000002
#define IRIGB_CONTROL_STROBE_TEST_LOAD_ENABLE               0x00000001

/* ========================================================================== */ /* vx329 */
/* QPRIME DMA (Register Offsets)                                              */
/* ========================================================================== */
#define REG_QPRM_DMA_INT_CS                                 0x00000000
#define REG_QPRM_DMA_MODE                                   0x00000001
#define REG_QPRM_DMA_PCI_ADDR                               0x00000002
#define REG_QPRM_DMA_LOC_ADDR                               0x00000003
#define REG_QPRM_DMA_TX_SIZE                                0x00000004
#define REG_QPRM_DMA_CFG                                    0x00000005
#define REG_QPRM_DMA_DSC_PTR                                0x00000006
#define REG_QPRM_DMA_CS                                     0x00000007
#define REG_QPRM_DMA_ABRT_ADDR                              0x00000008

/* ========================================================================== */
/* QPRIME DMA REG_QPRM_DMA_INT_CS Masks                                     */
/* ========================================================================== */
#define QPRIME_DMA_INT_CS_RETRY_TIMEOUT                     0x08000000
#define QPRIME_DMA_INT_CS_ABORT                             0x02000000
#define QPRIME_DMA_INT_CS_INT                               0x00200000
#define QPRIME_DMA_INT_CS_INT_ENABLE                        0x00040000

/* ========================================================================== */
/* QPRIME DMA REG_QPRM_DMA_MODE Masks                                       */
/* ========================================================================== */
#define QPRIME_DMA_MODE_DONE_INT_ENABLE                     0x00000400
#define QPRIME_DMA_MODE_SG_MODE                             0x00000200
#define QPRIME_DMA_MODE_RESET_BITS                          0x00000000

/* ========================================================================== */
/* QPRIME DMA REG_QPRM_DMA_CS Masks                                         */
/* ========================================================================== */
#define QPRIME_DMA_CS_DONE                                  0x00000010
#define QPRIME_DMA_CS_CLEAR_INT                             0x00000008
#define QPRIME_DMA_CS_ABORT                                 0x00000004
#define QPRIME_DMA_CS_START                                 0x00000002
#define QPRIME_DMA_CS_ENABLE                                0x00000001

/* ========================================================================== */
/* DMA Registers for PCIe boards                                              */
/* ========================================================================== */
#define REG_DMA_H_TO_SFP_LOCAL_ADDR                         0x0000      /* Offset in SFP where the data is located          */
#define REG_DMA_H_TO_SFP_HOST_LSB_ADDR                      0x0001      /* 32 Address in host where Descriptor is located   */
#define REG_DMA_H_TO_SFP_HOST_MSB_ADDR                      0x0002      /* MSBs for 64-bit Host Descriptor address          */
#define REG_DMA_H_TO_SFP_XFER_SIZE                          0x0003      /* Size in bytes of the DMA transfer                */

#define REG_DMA_SFP_TO_H_LOCAL_ADDR                         0x0004      /* Offset in SFP where the data should go           */
#define REG_DMA_SFP_TO_H_HOST_LSB_ADDR                      0x0005      /* 32 Address in host where Descriptor is located   */
#define REG_DMA_SFP_TO_H_HOST_MSB_ADDR                      0x0006      /* MSBs for 64-bit Host Descriptor address          */
#define REG_DMA_SFP_TO_H_XFER_SIZE                          0x0007      /* Size in bytes of the DMA transfer                */

#define REG_DMA_X_TO_X_LOCAL_ADDR_ROLLOVER_END              0x0008      /* Address on SFP at which to roll over */
#define REG_DMA_X_TO_X_LOCAL_ADDR_ROLLOVER_START            0x0009      /* Address of SFP to rollover too       */

/* ========================================================================== */
/*             PLX PCI CONFIGURATION REGISTERS                                */
/* ========================================================================== */
#define REG_PLX_PCICLSR                                     0x000C  /* PLX PCI cache line size                          */

/* ========================================================================== */
/*             PLX LOCAL CONFIGURATION REGISTERS                              */
/* ========================================================================== */
#define REG_PLX_MARBR                                       0x0008  /* PLX Mode/DMA Arbitration Register                */
#define REG_PLX_INTSCR                                      0x0068  /* PLX Interrupt Status/Config Register             */
#define REG_PLX_CNTRL                                       0x006C  /* PLX Control Register                             */
#define REG_PLX_DMA0_MODE                                   0x0080  /* PLX DMA channel 0 Mode Register                  */
#define REG_PLX_DMAPADR0                                    0x0084  /* PLX DMA channel 0 PCI Address Register           */
#define REG_PLX_DMALADR0                                    0x0088  /* PLX DMA channel 0 Local Address Register         */
#define REG_PLX_DMASIZ0                                     0x008C  /* PLX DMA channel 0 Transfer Size (Bytes) Register */
#define REG_PLX_DMAPR0                                      0x0090  /* PLX DMA channel 0 mode Register                  */
#define REG_PLX_DMADPR0                                     0x0090  /* PLX DMA channel 0 Descriptpr Pointer Register    */
#define REG_PLX_DMA0_CSR                                    0x00A8  /* PLX DMA channel 0 DMA CSR Register               */
#define REG_PLX_DMA0_DAC                                    0x00B4  /* PLX DMA channel 0 DMA Channel 0 PCI Dual Address Cycles Upper Address */

#define PLX_CS_CLEAR_INT                                    0x00000008

/* ========================================================================== */
/* PLX Mode/DMA Arbitration Register (MARBR)                                  */
/* ========================================================================== */
#define PLX_MARBR_PCI_READ_NO_WRITE_MODE                    0x02000000  /* bit 25 */
#define PLX_MARBR_PCI_READ_WITH_WRITE_FLUSH_MODE            0x04000000  /* bit 26 */

/* ========================================================================== */
/* DMA Mode Register (DMAMODE0)                                               */
/* ========================================================================== */
#define PLX_DMA_MODE_RESET_BITS                             0x00000000  /*             */
#define PLX_DMA_MODE_LOCAL_BUS_DATA_WIDTH_32_BIT            0x00000003  /* bit 1:0     */
#define PLX_DMA_MODE_WAIT_STATES                            0x0000000C  /* bit 5:2  3 wait states   */
#define PLX_DMA_MODE_READY_INPUT_ENABLE                     0x00000040  /* bit 6       */
#define PLX_DMA_MODE_BURST_ENABLE                           0x00000080  /* bit 7       */
#define PLX_DMA_MODE_LOCAL_BURST_ENABLE                     0x00000100  /* bit 8       */
#define PLX_DMA_MODE_SG_MODE_ENABLE                         0x00000200  /* bit 9       */
#define PLX_DMA_MODE_DONE_INT_ENABLE                        0x00000400  /* bit 10      */
#define PLX_DMA_MODE_CLEAR_COUNT_MODE                       0x00010000  /* bit 16      */

/* ========================================================================== */
/* Interrupt Command Status Register (INTSCR)                                 */
/* ========================================================================== */
#define PLX_INT_SCR_PCI_INT_ENABLE                          0x00000100  /* bit 8       */
#define PLX_INT_SCR_LOCAL_INT_INPUT_ENABLE                  0x00000800  /* bit 11      */
#define PLX_INT_SCR_LOCAL_INT_OUTPUT_ENABLE                 0x00010000  /* bit 16      */
#define PLX_INT_SCR_DMA_CH0_INT_ENABLE                      0x00040000  /* bit 18      */

#define PLX_INT_SCR_DISABLE_ALL                             0x00000000

#define PLX_INT_ENABLE                                      (PLX_INT_SCR_DMA_CH0_INT_ENABLE |       \
        PLX_INT_SCR_LOCAL_INT_OUTPUT_ENABLE |  \
        PLX_INT_SCR_LOCAL_INT_INPUT_ENABLE |   \
        PLX_INT_SCR_PCI_INT_ENABLE)

/* ========================================================================== */
/* DMA Descriptor Pointer Register (DMADPR0)                                  */
/* ========================================================================== */
#define PLX_DMA_DESC_PTR_PCI_ADDR                           0x00000001  /* bit 0 */
#define PLX_DMA_DESC_PTR_END_OF_CHAIN                       0x00000002  /* bit 1 */

/* ========================================================================== */
/* DMA Command/Status Register (DMACSR)                                       */
/* ========================================================================== */
#define PLX_DMA_CSR_ENABLE                                  0x00000001  /* bit 0 */
#define PLX_DMA_CSR_START                                   0x00000002  /* bit 1 */
#define PLX_DMA_CSR_ABORT                                   0x00000004  /* bit 2 */
#define PLX_DMA_CSR_CLEAR                                   0x00000008  /* bit 3 */
#define PLX_DMA_CSR_DONE                                    0x00000010  /* bit 4 */

/* ========================================================================== */
/* Dual Address Cycle Upper Address (DAC)                                     */
/* ========================================================================== */
#define PLX_INT_DAC_INIT                                    0x00000000

/* ========================================================================== */
/* 1553 EI COMPONENT REG                                                      */
/* ========================================================================== */
#define ACEX_EI_CONFIG_REG_A_BA                             0x00000000
#define ACEX_EI_CONFIG_REG_B_BA                             0x00000020
#define ACEX_EI_CONFIG_REG_C_BA                             0x00000040

#define ACEX_REG_EI_RT_ENABLE                               0x00000060
#define ACEX_REG_EI_BC_ENABLE                               0x00000061

/* ========================================================================== */
/* ARINC 717 PROGRAMMABLE COMPONENT REGISTERS OFFSETS - 32-BIT ADDRESSABLE(DW)*/
/* ========================================================================== */
#define REG_ARINC_717_GLOBAL_INT_ENABLE                     0x00000000
#define REG_ARINC_717_GLOBAL_INT_STATUS                     0x00000001
#define REG_ARINC_717_GLOBAL_CONFIG                         0x00000002

/* Channel dependent */
#define REG_ARINC_717_CH_INT_ENABLE                         0x00000000
#define REG_ARINC_717_CH_INT_STATUS                         0x00000001
#define REG_ARINC_717_CH_CONFIG                             0x00000002
#define REG_ARINC_717_CH_AUTO_DETECT_STATUS                 0x00000003
#define REG_ARINC_717_CH_TX_FRAME_COUNT                     0x00000004

/* Bit masks */
#define REG_ARINC_717_GLOBAL_INT_CH0_ENABLE                 0x00000001
#define REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE              0x00000001


/* ========================================================================== */
/* DIO TT COMPONENT REGISTERS OFFSETS - 32-BIT ADDRESSABLE(DW)                */
/* Refer to 'REG_GENERAL_0-7' above for common TT component register offsets  */
/* ========================================================================== */
#define REG_DIO_TT_INT_STATUS                               0x00000006
#define REG_DIO_TT_HEAD_POINTER                             0x00000008

/* ========================================================================== */
/* CAN BUS COMPONENT REGISTERS OFFSETS - 32-BIT ADDRESSABLE(DW)               */
/* ========================================================================== */
#if 1
/* <UDL19> */
#define REG_CAN_BUS_CONFIG_CH1_INTERRUPT_TRIGGER            0x00000000
#define REG_CAN_BUS_CONFIG_CH2_INTERRUPT_TRIGGER            0x00000001
#define REG_CAN_BUS_TX_CH1_INTTERRUPT_TRIGGER               0x00000008
#define REG_CAN_BUS_TX_CH2_INTTERRUPT_TRIGGER               0x00000009
#else
#define REG_CAN_BUS_CONFIG_INTERRUPT_TRIGGER                0x00000000
#define REG_CAN_BUS_TX_CH1_INTTERRUPT_TRIGGER               0x00000001
#define REG_CAN_BUS_TX_CH2_INTTERRUPT_TRIGGER               0x00000002
#define REG_CAN_BUS_RX_CH1_INTTERRUPT_TRIGGER               0x00000003
#define REG_CAN_BUS_RX_CH2_INTTERRUPT_TRIGGER               0x00000004
#endif /* 1 */


/* ========================================================================== */
/* ========================================================================== */
/* ========================================================================== */




typedef struct _UM_COMPONENT_INFO
{
    U32BIT umComponentType;
    CHAR umName[32];
    U32BIT umComponentRev;
    U32BIT umComponentRegSize;
    U32BIT umRegBaseAddr[UM_MAX_NUM_INSTANCES];

} UM_COMPONENT_INFO;


typedef struct _UM_DEVICE_INFO
{
    U32BIT umDevType;
    CHAR umName[32];
    U32BIT umDevRev;
    U32BIT umDevMemSize;    /* in 32bit */
    U32BIT umDevNumInstances;
    U32BIT umMemBaseAddr[UM_MAX_NUM_INSTANCES];  /* in 32bit */
    U32BIT umDevNumComponents;
    UM_COMPONENT_INFO umComponentInfo[UM_MAX_NUM_COMPONENTS];

} UM_DEVICE_INFO;


struct _UM_INFO_TYPE
{
    CHAR s8BrdModelNum[10];
    U32BIT u32DataArcNum;
    U32BIT firmwareRelVersion;
    U32BIT firmwareIntVersion;
    U32BIT numDevices;
    UM_DEVICE_INFO umDeviceInfo[UM_MAX_NUM_DEVICES];
};


typedef struct _UM_MILSTD1553_GLOBALREGSTATE
{
    U32BIT resetReg;
    U32BIT wTTLoReg;
    U32BIT wTTHiReg;

} UM_MILSTD1553_GLOBALREGSTATE;


typedef struct _UM_MILSTD1553_BCIREGSTATE
{
    U32BIT u32PlaceHolder; /* dummy var for struct */

} UM_MILSTD1553_BCIREGSTATE;


typedef struct _UM_MILSTD1553_MRTIREGSTATE
{
    U32BIT u32PlaceHolder; /* dummy var for struct */

} UM_MILSTD1553_MRTIREGSTATE;


typedef struct _UM_MILSTD1553_MTIEREGSTATE
{
    U32BIT configurationReg;
    U32BIT strobeReg;
    U32BIT cmdStkPtrReg;
    U32BIT dataStkPtrReg;
    U32BIT intrptEnReg;
    U32BIT intrptStatusReg;
    U32BIT impIntrptEnReg;
    U32BIT impNumWrdsIntrptReg;
    U32BIT impNumMsgsIntrptReg;
    U32BIT impTimeIntrvlIntrptReg;
    U32BIT impIntrptQueueStatusReg;
    U32BIT impIntrptQueueCntReg;
    U32BIT impIntrptStatusReg;
    U32BIT impFrstMsgAddrReg;
    U32BIT impTotLenReg;
    U32BIT impNumMsgsReg;
    U32BIT impStackStrtAddrReg;
    U32BIT impStackSzeReg;

} UM_MILSTD1553_MTIEREGSTATE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT umInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlUmCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#endif /* _DDC_UDL_UM_PRIVATE_H_ */
