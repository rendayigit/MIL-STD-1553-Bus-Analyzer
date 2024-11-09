/*******************************************************************************
 * FILE: ddc_udl_um_regmap_private.h
 *
 * DESCRIPTION:
 *
 *  UM ROM Register Masks
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

#ifndef _DDC_UDL_UM_REGMAP_PRIVATE_H_
#define _DDC_UDL_UM_REGMAP_PRIVATE_H_

/* MASK TO MODIFY ALL REGISTER BITS */
#define UM_REG_MASK_SETALL                                      0xFFFFFFFF
#define UM_REG_CLEAR_BITS                                       0x00000000
#define UM_REG_SET_BITS                                         0xFFFFFFFF

/* ========================================================================== */
/* UM_COMPONENTS_ID_CAPABILITIES                                              */
/* ========================================================================== */

#define ACEX_CAPABILITIES_REG_BASE_ADDR                         0x00000000

/* ========================================================================== */
/* UM_COMPONENTS_ID_GLOBAL                                                    */
/* ========================================================================== */

/* ========================================================================== */
/* UM_COMPONENTS_ID_MIL_STD_1553_SF_GLOBAL                                    */
/* ========================================================================== */

#define ACEX_MIL_STD_1553_GLOBAL_RESET                          0x00000000
#define ACEX_MIL_STD_1553_GLOBAL_TT_LO32_REG_ADDR               0x00000001
#define ACEX_MIL_STD_1553_GLOBAL_TT_HI16_REG_ADDR               0x00000002

#define ACEX_MIL_STD_1553_GLOBAL_RESET_ALL                      0x00000001
#define ACEX_MIL_STD_1553_GLOBAL_RESET_BCI                      0x00000002
#define ACEX_MIL_STD_1553_GLOBAL_RESET_MRTI                     0x00000004
#define ACEX_MIL_STD_1553_GLOBAL_RESET_MTIE                     0x00000008
#define ACEX_MIL_STD_1553_GLOBAL_RESET_ALL_IRQ                  0x00000010
#define ACEX_MIL_STD_1553_GLOBAL_RESET_ALL_TT                   0x00000020

/* ========================================================================== */
/* UM_COMPONENTS_ID_MIL_STD_1553_SF_BCI                                       */
/* ========================================================================== */

/* ========================================================================== */
/* UM_COMPONENTS_ID_MIL_STD_1553_SF_MRTI                                      */
/* ========================================================================== */

/* ========================================================================== */
/*UM_COMPONENTS_ID_MIL_STD_1553_SF_MTIE                                       */
/* ========================================================================== */

#define ACEX_1553_MTIE_CONFIGURATION_REG_ADDR                   0x00000000
#define ACEX_1553_MTIE_STROBE_REG_ADDR                          0x00000001
#define ACEX_1553_MTIE_CMD_STACK_PTR_REG_ADDR                   0x00000002
#define ACEX_1553_MTIE_DATA_STACK_PTR_REG_ADDR                  0x00000003
#define ACEX_1553_MTIE_INTRPT_EN_REG_ADDR                       0x00000004
#define ACEX_1553_MTIE_INTRPT_STATUS_REG_ADDR                   0x00000005
#define ACEX_1553_MTIE_IMP_INTRPT_EN_REG_ADDR                   0x00000006
#define ACEX_1553_MTIE_IMP_NUM_WRDS_INTRPT_REG_ADDR             0x00000007
#define ACEX_1553_MTIE_IMP_NUM_MSGS_INTRPT_REG_ADDR             0x00000008
#define ACEX_1553_MTIE_IMP_TIME_INTRVL_INTRPT_REG_ADDR          0x00000009
#define ACEX_1553_MTIE_IMP_INTRPT_QUEUE_STATUS_REG_ADDR         0x0000000A
#define ACEX_1553_MTIE_IMP_INTRPT_QUEUE_CNT_REG_ADDR            0x0000000B
#define ACEX_1553_MTIE_IMP_INTRPT_STATUS_REG_ADDR               0x0000000C
#define ACEX_1553_MTIE_IMP_FRST_MSG_ADDR_REG_ADDR               0x0000000D
#define ACEX_1553_MTIE_IMP_TOT_LEN_REG_ADDR                     0x0000000E
#define ACEX_1553_MTIE_IMP_NUM_MSGS_REG_ADDR                    0x0000000F
#define ACEX_1553_MTIE_IMP_STACK_STRT_ADDR_REG_ADDR             0x00000010
#define ACEX_1553_MTIE_IMP_STACK_SZE_REG_ADDR                   0x00000011

#define ACEX_1553_MTIE_CONFIGURATION_REG_MT_DATASTK_MASK        0x0000000F
#define ACEX_1553_MTIE_CONFIGURATION_REG_MT_CMDSTK_MASK         0x00000030
#define ACEX_1553_MTIE_CONFIGURATION_REG_RESP_TIMEOUT_MASK      0x0000FF00
#define ACEX_1553_MTIE_CONFIGURATION_REG_RESP_TIMEOUT_18US      0x12
#define ACEX_1553_MTIE_CONFIGURATION_REG_RESP_TIMEOUT_22US      0x16
#define ACEX_1553_MTIE_CONFIGURATION_REG_RESP_TIMEOUT_50US      0x32
#define ACEX_1553_MTIE_CONFIGURATION_REG_RESP_TIMEOUT_130US     0xFF
#define ACEX_1553_MTIE_CONFIGURATION_REG_1553A_MC_MASK          0x01000000

#define ACEX_1553_MTIE_STROBE_REG_START_MONITOR                 0x00000001
#define ACEX_1553_MTIE_STROBE_REG_HALT_MONITOR                  0x00000002

/* ========================================================================== */
/* UM_COMPONENTS_ID_MIL_STD_1553_SF_IMP                                       */
/* ========================================================================== */

/* ========================================================================== */
/* UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX                                       */
/* ========================================================================== */
#define ACEX_429_TX_GLOBAL_SCHEDULER_RESOLUTION_REG             0x00000000 /* write only */
#define ACEX_429_TX_GLOBAL_CHANNEL_SELECT_REG                   0x00000001 /* write only */
#define ACEX_429_TX_GLOBAL_SCHEDULER_ENABLE_REG                 0x00000004 /* write only */
#define ACEX_429_TX_GLOBAL_FIFO_REG                             0x00000007 /* write only */
#define ACEX_429_TX_GLOBAL_SCHEDULER_DISABLE_REG                0x00000008 /* write only */
#define ACEX_429_TX_GLOBAL_FIFO_DISABLE_REG                     0x0000000A /* write only */
#define ACEX_429_TX_GLOBAL_SCHEDULER_MESSAGE_QUEUE_LEN_REG      0x0000000B /* write only */
#define ACEX_429_TX_GLOBAL_FIFO_CONTROL_WORD_REG                0x0000000C /* write only */

#define ACEX_429_TX_GLOBAL_TEMP_MESSAGE_FIFO_STATUS_REG         0x00000003 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_0_REG            0x00000004 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_1_REG            0x00000005 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_2_REG            0x00000006 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_3_REG            0x00000007 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_4_REG            0x00000008 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_5_REG            0x00000009 /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_6_REG            0x0000000A /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_7_REG            0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_8_REG            0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_9_REG            0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_10_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_11_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_12_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_13_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_14_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_15_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_16_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_17_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_18_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_19_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_20_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_21_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_22_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_23_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_24_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_25_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_26_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_27_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_28_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_29_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_30_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_31_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_32_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_73_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_34_REG           0x0000000B /* read only */
#define ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_35_REG           0x0000000B /* read only */

/* ========================================================================== */
/* UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX                                       */
/* ========================================================================== */
#define ACEX_429_RX_GLOBAL_GEN_CTRL_REG                         0x00000000
#define ACEX_429_RX_GLOBAL_CTRL_PULSE_REG                       0x00000001
#define ACEX_429_RX_GLOBAL_TT_MSB_REG                           0x00000002 /* read write before LSB */
#define ACEX_429_RX_GLOBAL_TT_LSB_REG                           0x00000003
#define ACEX_429_RX_GLOBAL_LATCHED_REL_TT_MSB_REG               0x00000004
#define ACEX_429_RX_GLOBAL_LATCHED_REL_TT_LSB_REG               0x00000005
#define ACEX_429_RX_GLOBAL_INT_STATUS_REG                       0x00000006
#define ACEX_429_RX_GLOBAL_INT_ENABLE_REG                       0x00000007
#define ACEX_429_RX_GLOBAL_INT_STATUS_2_REG                     0x00000008
#define ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG                     0x00000009

#define ACEX_429_RX_GLOBAL_INT_ENABLE_IRIG_TT_RO_MASK           0x80000000
#define ACEX_429_RX_GLOBAL_INT_ENABLE_MS_TIMER_MASK             0x40000000
#define ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_MASK         0x0000FFFF
#define ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_2_MASK       0x000FFFFF

#define ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_RESOLUTION_MASK                 0x0000000F
#define ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_MASK                   0x000000F0
#define ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_TIMETAG_ENA_MASK                 0x00002000
#define ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_ENHANCED_TIMETAG_SUPPORTED_MASK  0x80000000  /* bit 31 */
#define ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_ENHANCED_TIMETAG_ENA_MASK        0x00010000  /* bit 16 */

#define ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_OFFSET     4

/* ========================================================================== */
/* UM_COMPONENTS_ID_ARINC_429_TX                                              */
/* ========================================================================== */
#define ACEX_429_TX_CONTROL_REG                                 0x00000000
#define ACEX_429_TX_ALT_SPEED_REG                               0x00000001
#define ACEX_429_TX_FRAME_TABLE_REG                             0x00000002
#define ACEX_429_TX_HI_PRIORITY_DATA_INPUT_REG                  0x00000003
#define ACEX_429_TX_LO_PRIORITY_DATA_INPUT_REG                  0x00000004
#define ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_REG                  0x00000005

#define ACEX_429_TX_FRAME_TABLE_REG__START                      0x00000001
#define ACEX_429_TX_FRAME_TABLE_REG__STOP                       0x00000002

#define ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_LO_PRI_FULL_MASK     0x00020000
#define ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_LO_PRI_EMPTY_MASK    0x00010000
#define ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_HI_PRI_FULL_MASK     0x00000002
#define ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_HI_PRI_EMPTY_MASK    0x00000001
#define ACEX_429_TX_HI_ASYNC_FIFO_STATUS_MSG_FREE_COUNT_MASK    0x0000FF80

/* ========================================================================== */
/* UM_COMPONENTS_ID_ARINC_429_RX                                              */
/* ========================================================================== */
#define ACEX_429_RX_INT_ENABLE_REG                              0x00000000
#define ACEX_429_RX_STATUS_REG                                  0x00000001
#define ACEX_429_RX_CONTROL_REG                                 0x00000002
#define ACEX_429_RX_WRAP_AROUND_REG                             0x00000003
#define ACEX_429_RX_WRAP_AROUND_2_REG                           0x00000004
#define ACEX_429_RX_FIFO_MSG_COUNT_REG                          0x00000005
#define ACEX_429_RX_FIFO_HEAD_TAIL_REG                          0x00000006
#define ACEX_429_RX_SAMPLE_RATE_REG                             0x00000007

#define ACEX_429_RX_FIFO_MSG_COUNT__CLEAR_FIFO_POINTERS_MASK    0x20000000

/* ========================================================================== */
/* UM_COMPONENTS_ID_IRIG_B_RX                                                 */
/* ========================================================================== */
#define ACEX_IRIG_B_RX_CONTROL_REG                              0x00000000
#define ACEX_IRIG_B_RX_CONTROL_STROBE_REG                       0x00000001
#define ACEX_IRIG_B_RX_TEST_LOAD_DATA_REG                       0x00000002
#define ACEX_IRIG_B_RX_TIME_STAMP_MSB_REG                       0x00000003
#define ACEX_IRIG_B_RX_TIME_STAMP_LSB_REG                       0x00000004
#define ACEX_IRIG_B_RX_1SEC_TIME_STAMP_MSB_REG                  0x00000005
#define ACEX_IRIG_B_RX_1SEC_TIME_STAMP_LSB_REG                  0x00000006
#define ACEX_IRIG_B_RX_CH0_1SEC_REL_TIME_STAMP_MSB_REG          0x00000007
#define ACEX_IRIG_B_RX_CH0_1SEC_TS_LSB_REG                      0x00000008
#define ACEX_IRIG_B_RX_CH1_1SEC_REL_TIME_STAMP_MSB_REG          0x00000009
#define ACEX_IRIG_B_RX_CH1_1SEC_TS_LSB_REG                      0x0000000A

/* ========================================================================== */
/* UM_COMPONENTS_ID_IRIG_B_TX                                                 */
/* ========================================================================== */
#define IRIGB_TX_CONTROL_REG_1                                  0x000000000
#define IRIGB_TX_CONTROL_REG_2                                  0x000000001

/* ========================================================================== */
/* UM_COMPONENTS_ID_DISCRETE_IO                                               */
/* ========================================================================== */

#endif /* _DDC_UDL_UM_REGMAP_PRIVATE_H_ */
