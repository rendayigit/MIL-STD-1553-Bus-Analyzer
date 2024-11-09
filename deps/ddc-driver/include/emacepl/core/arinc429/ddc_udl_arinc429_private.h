/*******************************************************************************
 * FILE: ddc_udl_arinc429_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support ARINC 429
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

#ifndef _DDC_UDL_ARINC429_PRIVATE_H_
#define _DDC_UDL_ARINC429_PRIVATE_H_

#include "driver_sdk/ddc_udl_private.h"


#define MAX_ARINC_429_TX_OFFSET_ENTRIES         1024


typedef struct _ACEX_429_RX_TYPE
{
    U32BIT *pu32RegBA;     /* ptr to 429 Registers base address    */
    U32BIT *pu32RegSize;   /* ptr to 429 Register size             */

    U32BIT *pu32MemSize;   /* ptr to UM memory size                */
    U32BIT *pu32MemBA;     /* ptr to UM memory base address        */
    
    U16BIT u16RxFifoEventCond;  /* DMA complete condition */
    
    DDC_CALLBACK waitqueueARINC429RxFifoCallback;
    DDC_EVENT waitqueueARINC429RxFifoEvent;                /* ARINC429RxFifo wait queue: one for each channel  */
    
    U32BIT u32Arinc429HbufPercentFull;
    U32BIT u32Arinc429HbufPercentHigh;
    U32BIT u32Arinc429HbufOverflowCount;

} ACEX_429_RX_TYPE;


typedef struct _ACEX_429_TX_TYPE
{
    U32BIT *pu32RegBA;     /* ptr to 429 Registers base address    */
    U32BIT *pu32RegSize;   /* ptr to 429 Register size             */

    U32BIT *pu32MemSize;   /* ptr to UM memory size                */
    U32BIT *pu32MemBA;     /* ptr to UM memory base address        */

    DDC_CALLBACK waitqueueARINC429SetTxFrameControlCallback;
    DDC_EVENT waitqueueARINC429SetTxFrameControlEvent;       /* ARINC429SetTxFrameControl wait queue: one for each channel */
    
    U16BIT u16ARINC429SetTxFrameControlEventCond;       /* DMA complete condition */

    S16BIT txOffset[MAX_ARINC_429_TX_OFFSET_ENTRIES];
    
} ACEX_429_TX_TYPE;



/* For devices other than DD-40000, old way of address calculation is used.
   MIO_429_MAX_NUM_RX is to know if the address is for TX */
#define MIO_429_MAX_NUM_RX              16

/*------------------------------------------------------
                BU-67xxx devices
--------------------------------------------------------*/
#define MIO_429_RX_INTEN               0x00000000
#define MIO_429_RX_INTSTAT             0x00000001
#define MIO_429_RX_CTRL                0x00000002
#define MIO_429_RX_WRAP                0x00000003
#define MIO_429_RX_COUNT               0x00000004

#define MIO_429_RX_COUNT_NUM           0x00001FFF
#define MIO_429_RX_COUNT_TT            0x80000000

#define MIO_INT_STATUS_TTRO            0x00004000
#define MIO_INT_STATUS_429             0x00000040
#define MIO_INT_STATUS_SER             0x00000080
#define MIO_INT_STATUS_AIO             0x00010000  /*16*/

#define MIO_FLASH_ADDR                 0x400
#define MIO_FLASH_IN                   0x401
#define MIO_FLASH_OUT                  0x402
#define MIO_CAPABILITIES               0x404
#define MIO_DATA_ARCHIVE               0x405
#define MIO_FPGA_REV                   0x406
#define MIO_FLASH_START                0x409
#define MIO_FLASH_CLUSTERS             0x40A
#define MIO_FLASH_CLUSTERS_SIZE        0x40B
#define MIO_IRIG_CFG_A                 0x40C
#define MIO_TEST_VAL                   0x40D
#define MIO_IRIG_CFG_B                 0x40D
#define MIO_INT_STATUS                 0x414
#define MIO_DISC                       0x41C
#define MIO_AVION                      0x41D
#define MIO_LED_CTRL                   0x425
#define MIO_LED_DATA                   0x426
#define MIO_TEST_MUX_A                 0x428
#define MIO_TT_LSB                     0x43B
#define MIO_TT_MSB                     0x43C

#define MIO_429_TX_CTRL_0              0x530
#define MIO_429_TX_CTRL_1              0x531
#define MIO_429_TX_CTRL_2              0x532
#define MIO_429_TX_CTRL_3              0x533
#define MIO_429_TX_CTRL_4              0x534
#define MIO_429_TX_CTRL_5              0x535
#define MIO_429_TX_CTRL_6              0x536
#define MIO_429_TX_CTRL_7              0x537

#define MIO_429_RX_CTRL_0              0x502
#define MIO_429_RX_CTRL_1              0x505
#define MIO_429_RX_CTRL_2              0x508
#define MIO_429_RX_CTRL_3              0x50B
#define MIO_429_RX_CTRL_4              0x50E
#define MIO_429_RX_CTRL_5              0x511
#define MIO_429_RX_CTRL_6              0x514
#define MIO_429_RX_CTRL_7              0x517
#define MIO_429_RX_CTRL_8              0x51A
#define MIO_429_RX_CTRL_9              0x51D
#define MIO_429_RX_CTRL_10             0x520
#define MIO_429_RX_CTRL_11             0x523
#define MIO_429_RX_CTRL_12             0x526
#define MIO_429_RX_CTRL_13             0x529
#define MIO_429_RX_CTRL_14             0x52C
#define MIO_429_RX_CTRL_15             0x52F

#define MIO_429_RX_INTEN_0             0x500
#define MIO_429_RX_INTEN_1             0x503
#define MIO_429_RX_INTEN_2             0x506
#define MIO_429_RX_INTEN_3             0x509
#define MIO_429_RX_INTEN_4             0x50C
#define MIO_429_RX_INTEN_5             0x50F
#define MIO_429_RX_INTEN_6             0x512
#define MIO_429_RX_INTEN_7             0x515
#define MIO_429_RX_INTEN_8             0x518
#define MIO_429_RX_INTEN_9             0x51B
#define MIO_429_RX_INTEN_10            0x51E
#define MIO_429_RX_INTEN_11            0x521
#define MIO_429_RX_INTEN_12            0x524
#define MIO_429_RX_INTEN_13            0x527
#define MIO_429_RX_INTEN_14            0x52A
#define MIO_429_RX_INTEN_15            0x52D

#define MIO_429_RX_INTSTAT_0           0x501
#define MIO_429_RX_INTSTAT_1           0x504
#define MIO_429_RX_INTSTAT_2           0x507
#define MIO_429_RX_INTSTAT_3           0x50A
#define MIO_429_RX_INTSTAT_4           0x50D
#define MIO_429_RX_INTSTAT_5           0x510
#define MIO_429_RX_INTSTAT_6           0x513
#define MIO_429_RX_INTSTAT_7           0x516
#define MIO_429_RX_INTSTAT_8           0x519
#define MIO_429_RX_INTSTAT_9           0x51C
#define MIO_429_RX_INTSTAT_10          0x51F
#define MIO_429_RX_INTSTAT_11          0x522
#define MIO_429_RX_INTSTAT_12          0x525
#define MIO_429_RX_INTSTAT_13          0x528
#define MIO_429_RX_INTSTAT_14          0x52B
#define MIO_429_RX_INTSTAT_15          0x52E

#define MIO_429_RESET                  0x538

/*------------------------------------------------------------------------------*/
/* Cast Global UART control registers */

/* Read only Capabilities */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_0          0x0000    /* Read only - Capabilities for ch 03 - 00 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_1          0x0001    /* Read only - Capabilities for ch 07 - 04 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_2          0x0002    /* Read only - Capabilities for ch 11 - 08 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_3          0x0003    /* Read only - Capabilities for ch 15 - 12 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_4          0x0004    /* Read only - Capabilities for ch 19 - 16 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_5          0x0005    /* Read only - Capabilities for ch 23 - 20 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_6          0x0006    /* Read only - Capabilities for ch 27 - 24 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_7          0x0007    /* Read only - Capabilities for ch 31 - 28 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_8          0x0008    /* Read only - Capabilities for ch 35 - 32 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_9          0x0009    /* Read only - Capabilities for ch 39 - 36 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_10         0x000A    /* Read only - Capabilities for ch 43 - 40 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_11         0x000B    /* Read only - Capabilities for ch 47 - 44 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_12         0x000C    /* Read only - Capabilities for ch 51 - 48 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_13         0x000D    /* Read only - Capabilities for ch 55 - 52 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_14         0x000E    /* Read only - Capabilities for ch 59 - 56 */
#define MIO_CAST_IO_CAPABILITIES_OFFSET_15         0x000F    /* Read only - Capabilities for ch 63 - 60 */

#define CAST_SERIAL_UART                0x0001
#define CAST_SERIAL_SYNC_ASYNC_UART     0x0007

/* R/W Serial Interrupt Enable */
#define MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_0  0x0010    /* R/W - Serial Interrupt for ch 31 - 00 */
#define MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_1  0x0011    /* R/W - Serial Interrupt for ch 63 - 00 */

/* Read only Serial Interrupt Status */
#define MIO_CAST_SERIAL_INTERRUPT_STATUS_OFFSET_0  0x0012    /* Read only - Serial Interrupt for ch 31 - 00 */
#define MIO_CAST_SERIAL_INTERRUPT_STATUS_OFFSET_1  0x0013    /* Read only - Serial Interrupt for ch 63 - 00 */

/* Reserved 0x14 - 0x1f read only */
/* R/W Protocol mode */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_0            0x0020    /* R/W - PROTOCOL for ch 07 - 00 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_1            0x0021    /* R/W - PROTOCOL for ch 15 - 08 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_2            0x0022    /* R/W - PROTOCOL for ch 23 - 16 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_3            0x0023    /* R/W - PROTOCOL for ch 31 - 24 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_4            0x0024    /* R/W - PROTOCOL for ch 39 - 25 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_5            0x0025    /* R/W - PROTOCOL for ch 47 - 40 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_6            0x0026    /* R/W - PROTOCOL for ch 55 - 48 */
#define MIO_CAST_PROTOCOL_MODE_OFFSET_7            0x0027    /* R/W - PROTOCOL for ch 63 - 56 */

/* R/W Physical mode Offset */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_0            0x0028    /* R/W - PHYSICAL for ch 07 - 00 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_1            0x0029    /* R/W - PHYSICAL for ch 15 - 08 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_2            0x002A    /* R/W - PHYSICAL for ch 23 - 16 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_3            0x002B    /* R/W - PHYSICAL for ch 31 - 24 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_4            0x002C    /* R/W - PHYSICAL for ch 39 - 32 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_5            0x002D    /* R/W - PHYSICAL for ch 47 - 40 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_6            0x002E    /* R/W - PHYSICAL for ch 55 - 48 */
#define MIO_CAST_PHYSICAL_MODE_OFFSET_7            0x002F    /* R/W - PHYSICAL for ch 63 - 56 */

/* R/W Physical mode setting */
#define MIO_CAST_SLEW_CONTROL_FAST                 0x08   /* or in for Fast slew rate    */
#define MIO_CAST_LOOPBACK                          0x00   /* Loops TX to RX              */
#define MIO_CAST_RS485_RS422_HALFDUPLEX            0x01   /* RS485 RS422 Half duplex     */
#define MIO_CAST_MIXED_PROTOCOL_HALFDUPLEX         0x02   /* Mixed Protocol Half duplex  */
#define MIO_CAST_LOW_POWER_4RS323_ACTIVE           0x03   /* Low Power 4RS323 mode       */
#define MIO_CAST_RS232_MODE                        0x04   /* RS232 mode                  */
#define MIO_CAST_RS485_RS422_FULLDUPLEX            0x05   /* RS485 RS422 FULL duplex     */
#define MIO_CAST_MIXED_PROTOCOL_FULLDUPLEX         0x06   /* Mixed Protocol FULL duplex  */
#define MIO_CAST_LOW_POWER_SHUTDOWN                0x07   /* Low Power Shutdow           */

/*------------------------------------------------------------------------------*/

/* Serial UART control registers */
#define MIO_UART_1                                      0x700
#define MIO_UART_2                                      0x710
#define MIO_UART_3                                      0x720
#define MIO_UART_4                                      0x730
#define MIO_UART_IO_CTRL                                0x790
#define MIO_UART_ENA                                    0x02E

/* CAST Serial IO support */
#define UART_PROTOCOL                                   0x8F0
#define UART_PHYSICAL                                   0x8F1
#define UART_INTERRUPT_ENABLE                           0x8F2
#define UART_INTERRUPT_STATUS                           0x8F3
#define UART_CAPABILITIES                               0x8F4

/* Scheduler registers */
#define MIO_SCH_RESOL                                   0x800
#define MIO_SCH_CHAN_SEL                                0x801
#define MIO_SCH_AUTO_ENA                                0x804
#define MIO_SCH_AUTO_PER                                0x805
#define MIO_SCH_AUTO_GAP                                0x806
#define MIO_SCH_TX_FIFO                                 0x807
#define MIO_SCH_CHAN_DIS                                0x808
#define MIO_SCH_STOP                                    0x809
#define MIO_SCH_FIFO_DIS                                0x80A
#define MIO_SCH_Q_LEN                                   0x80B
#define MIO_SCH_FIFO_STAT                               0x803

#define START_RX_REG_ADDRESS                            0x500
#define START_RX_REG_INDEX                              0x00
#define START_TX_REG_ADDRESS                            0x530
#define START_TX_REG_INDEX                              0x30
#define START_RX_FIFO_MEMORY                            MIO_RX_FIFOMB_0
#define START_DMT_MEMORY                                MIO_DMT_0
#define START_SCH_MEMORY                                MIO_SCH_MSG_DATA
#define START_SCH_REG_ADDRESS                           MIO_SCH_RESOL

/* 429 specific addresses */
#define MIO_RX_FIFOMB_0                                 0x00200000
#define MIO_RX_FIFOMB_1                                 0x00200400
#define MIO_RX_FIFOMB_2                                 0x00200800
#define MIO_RX_FIFOMB_3                                 0x00200C00
#define MIO_RX_FIFOMB_4                                 0x00201000
#define MIO_RX_FIFOMB_5                                 0x00201400
#define MIO_RX_FIFOMB_6                                 0x00201800
#define MIO_RX_FIFOMB_7                                 0x00201C00
#define MIO_RX_FIFOMB_8                                 0x00202000
#define MIO_RX_FIFOMB_9                                 0x00202400
#define MIO_RX_FIFOMB_10                                0x00202800
#define MIO_RX_FIFOMB_11                                0x00202C00
#define MIO_RX_FIFOMB_12                                0x00203000
#define MIO_RX_FIFOMB_13                                0x00203400
#define MIO_RX_FIFOMB_14                                0x00203800
#define MIO_RX_FIFOMB_15                                0x00203C00

#define MIO_RX_FIFOMB_LEN                               0x400

/* Data Match Table offsets */
#define MIO_DMT_0                                       0x00206000
#define MIO_DMT_1                                       0x00206020
#define MIO_DMT_2                                       0x00206040
#define MIO_DMT_3                                       0x00206060
#define MIO_DMT_4                                       0x00206080
#define MIO_DMT_5                                       0x002060A0
#define MIO_DMT_6                                       0x002060C0
#define MIO_DMT_7                                       0x002060E0
#define MIO_DMT_8                                       0x00206100
#define MIO_DMT_9                                       0x00206120
#define MIO_DMT_10                                      0x00206140
#define MIO_DMT_11                                      0x00206160
#define MIO_DMT_12                                      0x00206180
#define MIO_DMT_13                                      0x002061A0
#define MIO_DMT_14                                      0x002061C0
#define MIO_DMT_15                                      0x002061E0

#define MIO_DMT_LEN                                     32

/* Scheduler memory */
#define MIO_SCH_MSG_DATA                                0x00204000
#define MIO_SCH_Q_ACTIVE                                0x00204400
#define MIO_SCH_Q_CHAN                                  0x00204600
#define MIO_SCH_Q_PTR                                   0x00204800
#define MIO_SCH_Q_FREQ                                  0x00204A00
#define MIO_SCH_Q_OFST                                  0x00204C00
#define MIO_SCH_Q_RPT                                   0x00204E00

#define MIO_SCH_MSG_MEM_DEPTH                           0x00000400
#define MIO_SCH_Q_DEPTH                                 0x00000040

/* Card reset and interrupt enable masks */
#define CARD_RESET                                      0x00000001
#define ARINC_INT_ENABLE                                0x00000002
#define SER_INT_ENABLE                                  0x80000000

#define OLD_IRIG_ENABLE_MASK                            0x00000001
#define OLD_IRIG_RESET_MASK                             0x00000080
#define OLD_IRIG_FORMAT_MASK                            0x0000000E
#define OLD_IRIG_RESOLUTION_MASK                        0x00000700
#define OLD_IRIG_ROLLOVER_MASK                          0x00003800
#define OLD_IRIG_TT_RO_INT_ENABLE_MASK                  0x00004000

#define NEW_IRIG_ENABLE_MASK                            0x00000001
#define NEW_IRIG_RESET_MASK                             0x00000001
#define NEW_IRIG_FORMAT_MASK                            0x00002000

/* new register actually has 4 bits but flexcore only knows about 3 */
#define NEW_IRIG_RESOLUTION_MASK                        0x00000007
#define NEW_IRIG_ROLLOVER_MASK                          0x00000070
#define NEW_IRIG_TT_RO_INT_ENABLE_MASK                  0x80000000

/* rx control register masks */
#define MIO_RX_CNT_MODE                                 0x00000010
#define MIO_RX_CNT_TT                                   0x00000020
#define MIO_RX_CNT_FIFO_COUNT                           0x01FF8000
#define MIO_RX_CNT_OVERFLOW                             0x40000000
#define MIO_RX_CNT_RESET                                0x00002000
#define MIO_RX_CNT_SPEED                                0x00000200

#define ARINC_429_PROGRAMMABLE_CH1_RX                   0x00000001
#define ARINC_429_PROGRAMMABLE_CH1_TX                   0x00010000
#define ARINC_429_PROGRAMMABLE_MASK                     0x00010001

#define ARINC_429_PROGRAMMABLE_RX_RESET                 0x00002000
#define ARINC_429_PROGRAMMABLE_TX_RESET                 0x00000001

#define ARINC_429_PROGRAMMABLE2_CH1_TX                  0x00000001

#define ARINC_429_PROGRAMMABLE_BUS_ISOLATION_ENABLE     0x00000001

enum  { DISABLE = 0,
        ENABLE,
        ENABLE_ALT};

static const U32BIT LOOPBACK_MASK[MAX_NUM_429_CHANNELS] =
{
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020
};

/*------------------------------------------------------
                DD-40xxx devices
--------------------------------------------------------*/

/* TX Scheduler Memory */
#define DD429_TX_SCHED_MSG_DATA_OFFSET                  0x00000000
#define DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET              0x00000400
#define DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET             0x00000600
#define DD429_TX_SCHED_QUEUE_POINTER_OFFSET             0x00000800
#define DD429_TX_SCHED_QUEUE_FREQ_OFFSET                0x00000A00
#define DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET         0x00000C00
#define DD429_TX_SCHED_QUEUE_REPEAT_OFFSET              0x00000E00
#define DD429_TX_SCHED_MSG_CONTROL_OFFSET               0x00001000

#define DD429_TX_SCHED_QUEUE_DEPTH                      0x00000040
#define DD429_TX_SCHED_MESSAGE_MEM_DEPTH                0x00000400

#define DD429_ACTIVE                                    1
#define DD429_NOT_ACTIVE                                0

#define DD429_SDI_OFFSET                                8
#define DD429_ALT_SDI_OFFSET                            11

#define DD429_ALT_TO_NORMAL_SDI_OFFSET                  3

#define DD429_DATA_MATCH_TABLE_LEN_MIRROR               31      /* size of a DMT per channel local shadow copy 0 based */   

/* 1024 */
#define DD429_TX_SCHED_CONTROL_OFFSET                   0x00000400

/* Option parameters for API function ModifyRepeatedData */
#define ARINC_429_MODIFY_REPEATED_VIA_LABEL             0x0000001
#define ARINC_429_MODIFY_REPEATED_VIA_SDI_LABEL         0x0000002
        
#define DD429_SCHED_DATA_UPDATE_MASK                    0x000000001
#define DD429_SCHED_CONTROL_UPDATE_MASK                 0x000000002
#define DD429_SCHED_FREQUENCY_UPDATE_MASK               0x000000004
#define DD429_SCHED_OFFSET_UPDATE_MASK                  0x000000008
#define DD429_SCHED_ACTIVE_UPDATE_MASK                  0x000000010

#define DD429_REPEATER_MODE_TABLE_SIZE                  0x1000  /* tables start at top 4k of TX memory */

#define DD429_REPEATER__CLEAR_TABLE_OFFSET              0x0000
#define DD429_REPEATER__SET_TABLE_OFFSET                0x0400
#define DD429_REPEATER__FLIP_TABLE_OFFSET               0x0800
#define DD429_REPEATER__REPLACE_TABLE_OFFSET            0x0000
#define DD429_REPEATER__MODE_TABLE_OFFSET               0x0400

#define DD429_MAX_CHANNELS_PER_REGISTER                 32  /* # of channels that are mapped to a 32-bit register */

/* RX Control Register Masks */
#define DD429_RX_CONTROL_MASK__RSVD2                    0x80000000  /* Bit 31       Reserved                            */
#define DD429_RX_CONTROL_MASK__OVERFLOW                 0x40000000  /* Bit 30       Overflow                            */
#define DD429_RX_CONTROL_MASK__ARINC575                 0x20000000  /* Bit 29       ARINC 575       0=429 1=575         */
#define DD429_RX_CONTROL_MASK__FILTER_CODE              0x18000000  /* Bit 28-27    Filter Code     00=filter all, 01=filter data/parity/sdi, 10=filter on data/parity/SSM */
#define DD429_RX_CONTROL_MASK__FILTER_DATA              0x04000000  /* Bit 26       Filter on Data                      */
#define DD429_RX_CONTROL_MASK__FILTER_PARITY_ERR        0x02000000  /* Bit 25       Filter on Parity Error              */
#define DD429_RX_CONTROL_MASK__FIFO_COUNT               0x01FF8000  /* Bit 24-15    FIFO Count                          */
#define DD429_RX_CONTROL_MASK__FIFO_EMPTY               0x00004000  /* Bit 14       FIFO Empty                          */
#define DD429_RX_CONTROL_MASK__RESET                    0x00002000  /* Bit 13       Reset                               */
#define DD429_RX_CONTROL_MASK__DATA_TRANSFER            0x00001000  /* Bit 12       Data Transfer   0=numeric,1=file    */
#define DD429_RX_CONTROL_MASK__BIT_FORMAT               0x00000800  /* Bit 11       Bit Format  0=normal, 1=Alternate   */
#define DD429_RX_CONTROL_MASK__DATA_REPEATER            0x00000400  /* Bit 10       Reserved                            */
#define DD429_RX_CONTROL_MASK__SPEED                    0x00000200  /* Bit  9       Speed   1=low speed, 0=high speed   */
#define DD429_RX_CONTROL_MASK__PARITY                   0x00000180  /* Bit  8-7     Parity  00=odd parity, 01=even parity, 10=no parity, 11=reserved */
#define DD429_RX_CONTROL_MASK__LABEL_AUTO_CLEAR         0x00000040  /* Bit  6       Label Auto Clear                    */
#define DD429_RX_CONTROL_MASK__TIMETAG                  0x00000020  /* Bit  5       TT      1=IRIG/timetag, 0=none      */
#define DD429_RX_CONTROL_MASK__MODE                     0x00000010  /* Bit  4       Mode    0=mailbox mode, 1=fifo mode */
#define DD429_RX_CONTROL_MASK__RX_FULL                  0x0000000C  /* Bit  3-2     RX Full 00=25%, 01=50%, 11=100%     */
#define DD429_RX_CONTROL_MASK__IGNORE_LABEL             0x00000002  /* Bit  1       Ignore Label                        */
#define DD429_RX_CONTROL_MASK__WRAP_AROUND              0x00000001  /* Bit  0       Wrap Around     0=normal, 1=wrap    */
#define DD429_RX_CONTROL_MASK__PLACEHOLDER              0x00000000  /*              Plceholder                          */

/* RX Global Register Masks */
#define DD429_RX_GLOBAL_MASK__INTERRUPT_ENABLE          0x00008000  /* Bit 15       Interrupt Enable                    */
#define DD429_RX_GLOBAL_MASK__RESET                     0x00004000  /* Bit 14       Global Reset                        */

#define DD429_RX_GLOBAL_OFFSET__INTERRUPT_ENABLE        15          /* Bit 15       Interrupt Enable                    */
#define DD429_RX_GLOBAL_OFFSET__RESET                   14          /* Bit 14       Global Reset                        */

/* TX Control Register Masks */
#define DD429_TX_CONTROL_MASK__ENABLE                   0x80000000  /* Bit  31      Enable  0=diable, 1=enable          */
#define DD429_TX_CONTROL_MASK__VARIABLE_AMPLITUDE       0x03FF0000  /* Bit  25-16   TX Variable Amplitude               */
#define DD429_TX_CONTROL_MASK__RESOLUTION               0x00000080  /* Bit  7       TX Schedule Resolution  0 = 1ms, 1 = 1us*/
#define DD429_TX_CONTROL_MASK__ARINC575                 0x00000040  /* Bit  6       ARINC 575                           */
#define DD429_TX_CONTROL_MASK__BIT_FORMAT               0x00000020  /* Bit  5       Bit Format                          */
#define DD429_TX_CONTROL_MASK__SPEED                    0x00000010  /* Bit  4       Speed                               */
#define DD429_TX_CONTROL_MASK__PARITY                   0x0000000C  /* Bit  3-2     Parity  00=odd parity, 01=even parity, 10=no parity, 11=reserved */
#define DD429_TX_CONTROL_MASK__RSVD1                    0x00000002  /* Bit  1       Reserved                            */
#define DD429_TX_CONTROL_MASK__RESET                    0x00000001  /* Bit  0       Reset                               */

/* TX Control Register - Bits 25-16 Variable Amplitude */
#define DD429_TX_VARIABLE_AMPLITUDE_MASK                0x00FF0000
#define DD429_TX_VARIABLE_AMPLITUDE_OFFSET              16

/* TX Control Register - Bit 7 - TX Schedule Resolution */
#define DD429_TX_CONTROL_OFFSET__RESOLUTION             7

/* TX Control Register - Bits 3-2 - Parity */
#define DD429_TX_PARITY_MASK__ODD                       0x00000000
#define DD429_TX_PARITY_MASK__EVEN                      0x00000004
#define DD429_TX_PARITY_MASK__NO                        0x00000008

#define DD429_TX_FIFO_MASK__EMPTY                       0x00000001
#define DD429_TX_FIFO_MASK__FULL                        0x00000002
#define DD429_TX_FIFO_MASK__COUNT                       0x03FF0000
#define DD429_TX_FIFO_OFFSET__COUNT                     16
#define DD429_TX_FIFO_OFFSET__FREE_COUNT                7

/* RX Control Register - Bits 8-7 - Parity */
#define DD429_RX_PARITY_MASK__ODD                       0x00000000
#define DD429_RX_PARITY_MASK__EVEN                      0x00000080
#define DD429_RX_PARITY_MASK__NO                        0x00000100

#define DD429_RX_FIFO_MSG_COUNT_MASK__EMPTY             0x80000000  /* 31 */
#define DD429_RX_FIFO_MSG_COUNT_MASK__OVERFLOW          0x40000000  /* 30 */
#define DD429_RX_FIFO_MSG_COUNT_MASK__MSG_COUNT         0x00003FFF  /* 13 - 0 */

#define DD429_RX_HEAD_TAIL_MASK__HEAD                   0xFFFF0000  /* 31-16 */
#define DD429_RX_HEAD_TAIL_MASK__TAIL                   0x0000FFFF  /* 15-0 */
#define DD429_RX_HEAD_TAIL_OFFSET__HEAD                 16
#define DD429_RX_HEAD_TAIL_OFFSET__TAIL                 0

#define DD429_DATA_MATCH_TABLE_LEN                      32      /* size of a DMT per channel */

#define DATA_MATCH_TABLE__ALL_ENABLED                   0x00000000
#define DATA_MATCH_TABLE__ALL_DISABLED                  0xFFFFFFFF

#define DD429_TX_SCHEDULER_RESOLUTION_1MS               40000

#define DD429_IRIG_MASK__ENABLE                         0x00000001
#define DD429_IRIG_MASK__RESET                          0x00000001
#define DD429_IRIG_MASK__FORMAT                         0x00002000

#define MIN_LABEL_SDI                                   0x0001
#define MAX_LABEL_SDI                                   0x03FF
#define MAX_NUM_LABEL_SDI                               0x0400 /* 1024 label/SID combinations, including invalid ones */
#define DD429_MAX_LABEL                                 0x000000FF
#define DD429_NUM_LABELS                                256

#define ACEX_429_RX_INT_ENABLE__PROTOCOL_WORD           0x00001000
#define ACEX_429_RX_INT_ENABLE__FAIL_WARNING            0x00000800
#define ACEX_429_RX_INT_ENABLE__FUNCTION_TEST           0x00000400
#define ACEX_429_RX_INT_ENABLE__SOLO_WORD               0x00000200
#define ACEX_429_RX_INT_ENABLE__START_OF_TRANSMISSION   0x00000100
#define ACEX_429_RX_INT_ENABLE__NO_DATA_RECEIVED        0x00000080
#define ACEX_429_RX_INT_ENABLE__END_OF_TRANSMISSION     0x00000040
#define ACEX_429_RX_INT_ENABLE__NORMAL_OPERATION        0x00000020
#define ACEX_429_RX_INT_ENABLE__DATA_MATCH              0x00000010
#define ACEX_429_RX_INT_ENABLE__WORD_TYPE               0x00000008
#define ACEX_429_RX_INT_ENABLE__QUARTER_HALF_FULL       0x00000004
#define ACEX_429_RX_INT_ENABLE__PARITY_ERROR            0x00000002
#define ACEX_429_RX_INT_ENABLE__FIFO_OVERFLOW           0x00000001

/* RX Mode Types */
enum
{
    DD429_MAILBOX_MODE = 0,
    DD429_FIFO_MODE
};

/* Channel Speed Types */
enum
{
    DD429_LOW_SPEED = 0,
    DD429_HIGH_SPEED,
    DD429_ARINC575
};

/* Channel Parity Types */
enum
{
    DD429_NO_PARITY = 0,
    DD429_ODD_PARITY,
    DD429_EVEN_PARITY
};

/* Enable Types */
enum
{
    DD429_DISABLE = 0,
    DD429_ENABLE,
    DD429_ENABLE_ALT
};

/* Bit Format Types */
enum
{
    DD429_BITFORMAT_ORIG = 0,
    DD429_BITFORMAT_ALT
};

/* FIFO Status Types */
enum
{
    DD429_FIF0_NOT_EMPTY = 0,
    DD429_FIF0_EMPTY = 1
};

/* Overflow Status Types */
enum
{
    DD429_OVERFLOW_NONE = 0,
    DD429_OVERFLOW_EXISTS = 1
};

/* Time Tag Types */
enum
{
    TT48 = 0,
    IRIG_A,
    IRIG_B,
    IRIG_C,
    IRIG_D,
    IRIG_B_ENHANCED,
    MAX_TTFORMAT
};

/* Channel Types */
enum
{
    CHAN_TYPE_429 = 0,
    CHAN_TYPE_232,
    CHAN_TYPE_485,
    CHAN_TYPE_TT,
    CHAN_TYPE_429_TX,
    CHAN_TYPE_429_RX,
    CHAN_TYPE_717_PROG,
    CHAN_TYPE_717_TX,
    CHAN_TYPE_717_RX,
    CHAN_TYPE_CAN,
    CHAN_TYPE_UART,
    CHAN_TYPE_VOLT_MON,
    CHAN_TYPE_SDLC,
    CHAN_TYPE_HDLC,
    CHAN_TYPE_AIO,
    CHAN_TYPE_UNDEFINED,
    MAX_CHAN_TYPE
};

/* Filter Types */
enum
{
    FILTER_LABEL_SDI = 0,
    FILTER_PARITY_ERR,
    FILTER_STALE_MSG,
    MAX_FILTER
};

typedef struct _ARINC_429_TX_SCHEDULER_DATA
{
    U16BIT u16Channel[DD429_TX_SCHED_MESSAGE_MEM_DEPTH];
    U32BIT u32Data[DD429_TX_SCHED_MESSAGE_MEM_DEPTH]; /* legacy & 1024 */
    U32BIT u32Address[DD429_TX_SCHED_MESSAGE_MEM_DEPTH]; /* legacy */
    U16BIT u16Offset[DD429_TX_SCHED_MESSAGE_MEM_DEPTH]; /* legacy & 1024 */
    
} ARINC_429_TX_SCHEDULER_DATA;

typedef struct _SCHEDULER_DATA
{
    U32BIT u32Channel;
    U16BIT u16Index;
    U32BIT u32Data;
    U32BIT u32Control;
    U16BIT u16Frequency;
    U16BIT u16Offset;
    U8BIT  u8Active;

} SCHEDULER_DATA, *PSCHEDULER_DATA;

typedef struct _ACEX_429HBUF_TYPE
{
    U8BIT *pu8Buffer;
    U32BIT u32HbufNumEntries;
    U32BIT u32HbufNextRead;
    U32BIT u32HbufNextWrite;
    BOOLEAN bHbufStoreTimeTag;
    BOOLEAN bHbufOverflow;
    BOOLEAN bHbufQueueOverflow;
    DDC_ISR_LOCK_TYPE sem429HBuf;
    DDC_ISR_FLAG_TYPE sem429HBufFlag;
    
} ACEX_429HBUF_TYPE;


typedef struct _ACEX_429_RX_HBUF_TYPE
{
    BOOLEAN bRxFiFoHbufInstalled;
    ACEX_429HBUF_TYPE sHBuf[MAX_NUM_429_CHANNELS];
    
}   ACEX_429_RX_HBUF_TYPE;


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;


extern void ARINC429TxSetStateIdle
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ARINC429ChInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlARINC429ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ARINC429HostBufferControl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t           *pBytesReturned,
    U32BIT           *pReadData
);

extern void ARINC429HandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32RxGlobalIS16BIT
);

extern void ARINC429ChFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ARINC429ProcessCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalOutputBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

extern S16BIT ARINC429ProgrammableConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ARINC_429_PROGRMMABLE_CONFIG *pConfig
);

extern void ARINC429SetLegacyLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ARINC429EnableRxInterrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ARINC429DisableRxInterrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);


extern U32BIT ARINC429CastGetSerialIOConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
);

extern U32BIT ARINC429CastSetSerialIOConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
);

extern U32BIT ARINC429CastSerialIORegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
);

extern void ARINC429CastSerialIORegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoWrite
);

extern U32BIT ARINC429CastSerialIOCapChanMatrix
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern U32BIT ARINC429getCastIOMask
(
    int bitpos
);

extern void ARINC429LoadTxQueueOne
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, 
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void ARINC429LoadTxQueueMore
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT * pRdData,
    U32BIT OutputBufferLength
);

extern void ARINC429GetTimeStampConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern void ARINC429SetTimeStampConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

extern U32BIT ARINC429GetLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    S16BIT sRx
);

extern void ARINC429SetLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    S16BIT s16Rx,
    S16BIT s16Tx
);

extern U32BIT ARINC429RegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pUsbIoRead
);

extern void ARINC429RegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pUsbIoRead
);

extern U32BIT ARINC429MemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pUsbIoRead
);

extern void ARINC429MemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pUsbIoRead
);

extern S16BIT ARINC429GetQueueStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Channel,
    size_t *pBytesReturned,
    U32BIT* pReadData
);

extern void ArincDisableIrq
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
);

#endif /* _DDC_UDL_ARINC429_PRIVATE_H_ */
