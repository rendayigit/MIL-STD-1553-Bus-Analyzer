/*******************************************************************************
 * FILE: internal.h
 *
 * DESCRIPTION:
 *
 *  TODO
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

#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* ========================================================================== */
/*                           GLOBAL MACRO DEFINITIONS                         */
/* ========================================================================== */
#define API_MAX                                             3
#define API_EMACE                                           0
#define API_ACEX                                            1
#define API_REMOTE                                          2
#define API_NONE                                            API_MAX

/* Device Status Modes */
#define ACEX_CLOSED                                         0
#define ACEX_OPEN                                           1
#define AIOX_OPEN                                           2

/* ========================================================================== */
/*                            ACE MACRO DEFINITIONS                           */
/* ========================================================================== */

/* ========================================================================== */
/*                        ACEX BOARD MACRO DEFINITIONS                        */
/* ========================================================================== */
#define ACEX_BD_CAPABILITIES_IRIG_RANGE                     0x00000040 /* 06 */
#define ACEX_BD_CAPABILITIES_BUS_COUPLING                   0x00000020 /* 05 */
#define ACEX_BD_CAPABILITIES_VARIABLE_XCVR                  0x00000010 /* 04 */
#define ACEX_BD_CAPABILITIES_TX_IRIG_B                      0x00000008 /* 03 */
#define ACEX_BD_CAPABILITIES_RX_IRIG_B                      0x00000004 /* 02 */
#define ACEX_BD_CAPABILITIES_PARALLEL_FLASH_CTRL            0x00000002 /* 01 */
#define ACEX_BD_CAPABILITIES_FLASH_INFO                     0x00000001 /* 00 */

/* ========================================================================== */
/*                     ACEX UNIFIED MAP MACRO DEFINITIONS                     */
/* ========================================================================== */
#define ACEX_UM_MAX_NUM_COMPONENTS                          20
#define ACEX_UM_MAX_NUM_INSTANCES                           16
#define ACEX_UM_MAX_NUM_DEVICES                             11

/* ========================================================================== */
/*                         ACEX BC MACRO DEFINITIONS                          */
/* ========================================================================== */

/* BC Host Id Definitions */
#define BC_HOST_ID_MSG                                      0
#define BC_HOST_ID_STREAM                                   1
#define BC_HOST_ID_ARRAY                                    2
#define BC_HOST_ID_LP                                       3
#define BC_HOST_ID_HP                                       4

/* BC Maximum Message Index */
#define ACEX_BC_MAX_INDEX_MSG                               2048
#define ACEX_BC_MAX_INDEX_DATA                              2048

/* BC Configuration Constants */
#define ACEX_BC_INIT                                        40
#define ACEX_BC_CONFIG                                      41
#define ACEX_BC_FREE                                        42
#define ACEX_BC_HBUF_INSTALL                                43
#define ACEX_BC_HBUF_UNINSTALL                              44
#define ACEX_BC_GPQ_INIT                                    45
#define ACEX_BC_LPQ_INIT                                    46
#define ACEX_BC_HPQ_INIT                                    47
#define ACEX_BC_REPLAY_INIT                                 48
#define ACEX_BC_MUX_INIT                                    49

/* BC State Macros */
#define ACEX_BC_RESET                                       0
#define ACEX_BC_READY                                       1
#define ACEX_BC_RUN                                         2

/* BC Message Buffer Management Macros */
#define ACEX_BC_MESSAGE_FULL                                0
#define ACEX_BC_MESSAGE_HDR                                 1

/* BC Host Buffer and GpQueue Metric Type Macros*/
#define ACEX_BC_METRIC_HBUF                                 0
#define ACEX_BC_METRIC_GPQ                                  1

/* BC Host Buffer Message Management Macros - These map to ACE_BC_MSGLOC_NEXT_PURGE, ... */
#define ACEX_BC_HBUF_NEXT_PURGE                             0   /* next unread msg, clr stk */
#define ACEX_BC_HBUF_NEXT_NPURGE                            1   /* next unread msg, leave stk */
#define ACEX_BC_HBUF_LATEST_PURGE                           2   /* latest msg, clear stk */
#define ACEX_BC_HBUF_LATEST_NPURGE                          3   /* latest msg, leave stk alone */
#define ACEX_BC_HBUF_FILL_BUFFER                            4   /* fill msg buffer, clear stk */

/* BC Buffer Read Error Definitions - Translated to ACE errors in axdevice.c */
#define ACEX_BC_BUF_STS_SUCCESS                             0
#define ACEX_BC_BUF_STS_INVALID_BUF                         -1

/* BC ASYNC error code */
#define ACEX_BC_ASYNC_STS_SUCCESS                           0
#define ACEX_BC_ASYNC_STS_QUEUE_FULL                        -1
#define ACEX_BC_ASYNC_STS_INVALID_MODE                      -2
#define ACEX_BC_ASYNC_STS_USB_FAIL                          -3
#define ACEX_BC_ASYNC_STS_DATARRAY_FULL                     -4

/* BC Async Queue Type Macros */
#define ACEX_BC_QUEUE_LP                                    0
#define ACEX_BC_QUEUE_HP                                    1

/* BC Queue Size Macros */
#define ACEX_MEMSZ_BC_GPQ_256                               0x0100 /* BC GPQ 256 dwords long */
#define ACEX_MEMSZ_BC_GPQ_64                                0x0040 /* BC GPQ 64 dwords long */
#define ACEX_MEMSZ_BC_LPQ_512                               0x0200 /* BC LPQ 512 dwords long */
#define ACEX_MEMSZ_BC_LPQ_256                               0x0100 /* BC LPQ 256 dwords long */
#define ACEX_MEMSZ_BC_LPQ_128                               0x0080 /* BC LPQ 128 dwords long */
#define ACEX_MEMSZ_BC_LPQ_64                                0x0040 /* BC LPQ 64 dwords long */
#define ACEX_MEMSZ_BC_LPQ_32                                0x0020 /* BC LPQ 32 dwords long */
#define ACEX_MEMSZ_BC_HPQ_512                               0x0200 /* BC HPQ 512 dwords long */
#define ACEX_MEMSZ_BC_HPQ_256                               0x0100 /* BC HPQ 256 dwords long */
#define ACEX_MEMSZ_BC_HPQ_128                               0x0080 /* BC HPQ 128 dwords long */
#define ACEX_MEMSZ_BC_HPQ_64                                0x0040 /* BC HPQ 64 dwords long */
#define ACEX_MEMSZ_BC_HPQ_32                                0x0020 /* BC HPQ 32 dwords long */

/* BC Slot Routine Size Macros */
#define ACEX_MEMSZ_BC_SLOT_PARAM                            0x0006 /* BC Time Slot Manager 6 dwords long */
#define ACEX_MEMSZ_BC_SLOT_MAIN                             0x0030 /* BC Time Slot Manager 48 dwords long */
#define ACEX_MEMSZ_BC_SLOT_RESET                            0x0012 /* BC Time Slot Manager 18 dwords long */
#define ACEX_MEMSZ_BC_SLOT_ITEM                             0x0004 /* BC Time Slot Item Routine 4 dwords long */

/* BC Replay Buffer Size Macros */
#define ACEX_MEMSZ_BC_REPLAY_32K                            0x02000 /* BC Replay Memory 32K Words = 8K Dwords */
#define ACEX_MEMSZ_BC_REPLAY_64K                            0x04000 /* BC Replay Memory 64K Words = 16K Dwords */
#define ACEX_MEMSZ_BC_REPLAY_128K                           0x08000 /* BC Replay Memory 128K Words = 32K Dwords */
#define ACEX_MEMSZ_BC_REPLAY_256K                           0x10000 /* BC Replay Memory 128K Words = 64K Dwords */

/* BC User Interrupt Signal Macros */
#define ACEX_BC_UIRQ_NONE                                   0
#define ACEX_BC_UIRQ0                                       1
#define ACEX_BC_UIRQ1                                       2
#define ACEX_BC_UIRQ2                                       4
#define ACEX_BC_UIRQ3                                       8

/* BC external trigger information */
#define ACEX_BC_EXT_TRIGGER_NOT_AVAIL                       0x00000000
#define ACEX_BC_EXT_TRIGGER_AVAIL                           0x00000001
#define ACEX_BC_EXT_TRIGGER_USING_DIO                       0x00010000
#define ACEX_BC_EXT_TRIGGER_USING_DIRECT_INPUT              0x00020000

/* BC Improvements Entry Bits Masks */
#define ACEX_BC_IMP_NO_AUTO_POST                            0x08000000
#define ACEX_BC_IMP_PACK_DATA                               0x10000000
#define ACEX_BC_IMP_POST_VALID_ONLY                         0x20000000
#define ACEX_BC_IMP_CMD_ONLY                                0x40000000
#define ACEX_BC_IMP_DATA_ONLY                               0x80000000
#define ACEX_BC_IMP_ADDR_MASK                               0x0007FFFF

/* ========================================================================== */
/*                         ACEX (M)RT MACRO DEFINITIONS                       */
/* ========================================================================== */
#define ACEX_ISQ_BA_OFFSET_BITMASK                          0x0003FFC0

#define ACEX_MRT_CLR     0
#define ACEX_MRT_SET     1
#define ACEX_MRT_DISABLE 0
#define ACEX_MRT_ENABLE  1
#define ACEX_MRT_STATUS  2

#define ACEX_MRT_EOM_TT_ENABLE                              0x0200
#define ACEX_MRT_SOM_TT_ENABLE                              0

#define ACE_RT_CMD_SA_MASK                                  0x03e0
#define ACE_RT_CMD_WDCNT_MC_MASK                            0x001f

#define ACEX_RT_OPT_EXTERN_BTWD_IF_BUSY                     0x0400
#define ACEX_RT_OPT_INHIBIT_BTWD_IF_BUSY                    0x0800

#define ACEX_RT_STATUS_INCTRL_MASK                          0x07FF
#define ACEX_RT_STATUS_MASK                                 0x07C0

/* Acex mode code data table locations */
#define ACEX_RT_MCDATA_TX_BITWD_OFFSET_MSW                  0x09 /* located in most significant word */

/* Control operations */
#define ACEX_MRT_HBUF_ENABLE                                0
#define ACEX_MRT_HBUF_DISABLE                               1
#define ACEX_MRT_HBUF_GET                                   2
#define ACEX_MRT_RTCMD_HIGH_PCT_RESET                       3
#define ACEX_MRT_STREAMING_ENABLE_RX                        4
#define ACEX_MRT_STREAMING_ENABLE_TX                        5
#define ACEX_MRT_STREAMING_DISABLE_RX                       6
#define ACEX_MRT_STREAMING_DISABLE_TX                       7
#define ACEX_MRT_DATA_ARRAY_ENABLE                          8
#define ACEX_MRT_DATA_ARRAY_DISABLE                         9
#define ACEX_MRT_DATA_ARRAY_ENABLE_CONTINUOUS               10
#define ACEX_MRT_DATA_ARRAY_DISABLE_INT                     11

#define ACEX_RT_HBUF_MSGS_TO_GET                            100

/* MRT Mode Code Irq Translation Macros */
#define ACE_MRT_MCIRQ_TRNS_VECTOR                           0x00010000 /* TX_DATA */
#define ACE_MRT_MCIRQ_SYNCHRONIZE                           0x00020000 /* Sync with Data */
#define ACE_MRT_MCIRQ_TRNS_LAST_CMD                         0x00040000 /* TX_DATA */
#define ACE_MRT_MCIRQ_TRNS_BIT                              0x00080000 /* TX_DATA */
#define ACE_MRT_MCIRQ_SEL_TRNS_SHUTDWN                      0x00100000 /* (BCST_)RX_DATA */
#define ACE_MRT_MCIRQ_OVR_SEL_TRNS_SHUTDWN                  0x00200000 /* (BCST_)RX_DATA */

/* Configuration Sub-commands */
#define ACEX_MRT_CONFIG                                     1
#define ACEX_MRT_FREE                                       2
#define ACEX_RT_OPEN                                        3
#define ACEX_RT_CONFIG_SET                                  4 /* enable configuration features after open */
#define ACEX_RT_CONFIG_CLR                                  5 /* disable configuration features after open */
#define ACEX_RT_BITWD_RD                                    6
#define ACEX_RT_START                                       7
#define ACEX_RT_STOP                                        8
#define ACEX_RT_CLOSE                                       9
#define ACEX_RT_LATCH_CTRL                                  10
#define ACEX_RT_SOURCE_CTRL                                 11
#define ACEX_MRT_HBUF_CTRL                                  12
#define ACEX_MRT_CMDSTK_STATS                               13
#define ACEX_MRT_CMDSTK_SET_TO_LATEST                       14
#define ACEX_MRT_GET_BRDCST_ADDRS                           15
#define ACEX_RT_STREAM_CTRL                                 16
#define ACEX_RT_START_RESET_CMDSTK                          17
#define ACEX_RT_DATA_ARRAY_CTRL                             18
#define ACEX_MRT_CONFIG_UPDATE                              19 /* this command is used to update mrt global config after
                                                                initial configuration is completed */

#define ACEX_CHAN_INIT                                      200 /* case to clear all given channel resources */

/* MF Phase III: RT holdoff time for dynamic bus control */
#define ACEX_RT_HOLDOFF_TIME_MIN                            40          /* 40 us */
#define ACEX_RT_HOLDOFF_TIME_MAX                            130000      /* 130 ms */

/* Genreal Component Register Definitions */
#define ACEX_REG_GEN_CTRL                                   0x00000000
#define ACEX_REG_GEN_CTRL_PULSE                             0x00000001
#define ACEX_REG_GEN_TT_MSB                                 0x00000002
#define ACEX_REG_GEN_TT_LSB                                 0x00000003
#define ACEX_REG_GEN_TT_LATCHED_MSB                         0x00000004
#define ACEX_REG_GEN_TT_LATCHED_LSB                         0x00000005
#define ACEX_REG_GEN_INT_STATUS                             0x00000006
#define ACEX_REG_GEN_INT_MASK                               0x00000007
#define ACEX_REG_GEN_RAM_SELF_TEST_STATUS                   0x00000008

/* General Component Register Bit Masks */
#define ACEX_GEN_CTRL_RESERVED_MASK_31_20                   0xFFF03000
#define ACEX_GEN_CTRL_EXT_CHA_CHB_LOOP_TEST_ENA             0x00080000
#define ACEX_GEN_CTRL_CHA_TRANS_SLEEP_MODE_ENA              0x00040000
#define ACEX_GEN_CTRL_CHB_TRANS_SLEEP_MODE_ENA              0x00020000
#define ACEX_GEN_CTRL_HOST_MEM_WORD_FLIP_ENABLE             0x00010000

#define ACEX_GEN_CTRL_IRIGB_TIMETAG_ENABLE                  0x00002000
#define ACEX_GEN_CTRL_BC_LOAD_ENABLE                        0x00001000
#define ACEX_GEN_CTRL_SYNC_WITH_DATA_ENABLE                 0x00000800
#define ACEX_GEN_CTRL_SYNC_NO_DATA_ENABLE                   0x00000400
#define ACEX_GEN_CTRL_EXT_TT_COUNT_ENABLE                   0x00000200
#define ACEX_GEN_CTRL_EXT_TT_LOAD_PULSE_ENABLE              0x00000100

#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_MASK                0x000000F0
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_16_BIT              0x00000000
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_17_BIT              0x00000080
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_18_BIT              0x00000100
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_19_BIT              0x00000180
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_20_BIT              0x00000200
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_21_BIT              0x00000280
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_22_BIT              0x00000300
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_48_BIT              0x00000380
#define ACEX_GEN_CTRL_TT_ROLLOVER_POINT_LSHIFT_ALIGN        4

#define ACEX_GENERAL_CTRL_TT_RESOLUTION_MASK                0x0000000F
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_64US                0x00000000
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_32US                0x00000010
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_16US                0x00000020
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_08US                0x00000030
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_04US                0x00000040
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_02US                0x00000050
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_01US                0x00000060
#define ACEX_GENERAL_CTRL_TT_RESOLUTION_BIT_SHIFT_ALIGN     0

/* BC Component Register Definitions */
#define ACEX_REG_BC_INT_MASK                                0x00000000
#define ACEX_REG_BC_CONFIG                                  0x00000001
#define ACEX_REG_BC_START_RESET                             0x00000002
#define ACEX_REG_BC_INST_PTR_READ                           0x00000002
#define ACEX_REG_BC_INT_STS                                 0x00000003
#define ACEX_REG_BC_FRAME_TIME_REMAINING                    0x00000004
#define ACEX_REG_BC_MSG_TIME_REMAINING                      0x00000005
#define ACEX_REG_BC_INIT_INST_PTR                           0x00000006
#define ACEX_REG_BC_COND_CODE                               0x00000007
#define ACEX_REG_BC_GP_FLAG                                 0x00000007
#define ACEX_REG_BC_GPQ                                     0x00000008
#define ACEX_REG_BC_HP_ASYNC_STS                            0x00000009
#define ACEX_REG_BC_HP_ASYNC_NOMP                           0x00000009
#define ACEX_REG_BC_HP_ASYNC_CTRL                           0x0000000A
#define ACEX_REG_BC_TEST_MODE                               0x0000000B
#define ACEX_REG_BC_HOST_ACCESS_CTRL                        0x0000000C
#define ACEX_REG_BC_TEST_NOT_USED                           0x0000000D
#define ACEX_REG_BC_LP_ASYNC_STS                            0x0000000E
#define ACEX_REG_BC_LP_ASYNC_NOMP                           0x0000000E
#define ACEX_REG_BC_LP_ASYNC_CTRL                           0x0000000F

/* MRT Component Register Definitions */
#define ACEX_REG_MRT_GCONFIG                                0x00000000
#define ACEX_REG_MRT_RT_ENABLE                              0x00000001
#define ACEX_REG_MRT_BRDCST_SEL_CTRL                        0x00000002  /* broadcast selected control */
#define ACEX_REG_MRT_CMD_STK_PTR                            0x00000003
#define ACEX_REG_MRT_GBL_DATA_STK_PTR                       0x00000004
#define ACEX_REG_MRT_ISQP_RW                                0x00000008
#define ACEX_REG_MRT_REG_INT_ENABLE_MASK                    0x0000001A
#define ACEX_REG_MRT_CMD_STK_ENTRIES                        0x0000001C
#define ACEX_REG_MRT_RT_1553A_CONFIG                        0x0000001E
#define ACEX_REG_MRT_RESP_TIME_ENABLE                       0x00000026  /* enable response time register */
#define ACEX_REG_MRT_RESP_TIME_FIRST                        0x00000027  /* response time registers - first one */
#define ACEX_REG_MRT_RESP_TIME_LAST                         0x0000002E  /* response time registers - last one */

#define ACEX_REG_MRT_DBC_CTRL                               0x00000030
#define ACEX_REG_MRT_DBC_RT_DISABLED_STATUS                 0x00000032  /* bit-wise for 32 RTs */
#define ACEX_REG_MRT_DBC_RT_TO_ENABLE                       0x00000033  /* bit-wise for 32 RTs */

/* DBC control register bits definitions */
#define ACEX_REG_MRT_DBC_CTRL_ENABLE_RT                     0x00000001  /* to activate RT from BC */
#define ACEX_REG_MRT_DBC_CTRL_DISABLE_RT                    0x00000002  /* to disable RT by RT itself */
#define ACEX_REG_MRT_DBC_CTRL_RT_STATUS                     0x00000004  /* status after a RT is activated */

/* MTI Free Memory Threshold */
#define MT_FREE_MEM_COUNT_THRESHOLD                         0x00000057

/* MTI Component Register Definitions */
#define ACEX_REG_MTI_CONFIG_RW                              0x00000000
#define ACEX_REG_MTI_STROBE_W                               0x00000001
#define ACEX_REG_MTI_STATUS_R                               0x00000001
#define ACEX_REG_MTI_STK_ADDRESS_RW                         0x00000002
#define ACEX_REG_MTI_FREE_MEM_COUNT_RW                      0x00000003
#define ACEX_REG_MTI_INT_ENABLE_RW                          0x00000004
#define ACEX_REG_MTI_INT_SET_NUMBER_OF_WORDS_RW             0x00000005
#define ACEX_REG_MTI_INT_SET_NUMBER_OF_MSGS_RW              0x00000006
#define ACEX_REG_MTI_INT_SET_TIME_INTERVAL_RW               0x00000007
#define ACEX_REG_MTI_INT_QUEUE_STATUS_RW                    0x00000008
#define ACEX_REG_MTI_INT_QUEUE_COUNT_RW                     0x00000009
#define ACEX_REG_MTI_INT_STATUS_RW                          0x0000000A
#define ACEX_REG_MTI_FIRST_MSG_ADDRESS_RW                   0x0000000B
#define ACEX_REG_MTI_TOTAL_LENGTH_RW                        0x0000000C
#define ACEX_REG_MTI_NUMBER_OF_MSGS_RW                      0x0000000D
#define ACEX_REG_MTI_NUMBER_OF_DROPPED_MSGS_RW              0x0000000E
#define ACEX_REG_MTI_TEST0_CONTROL_SIGS_RW                  0x00000010 /* Control Signals */
#define ACEX_REG_MTI_TEST1_CHA_DEC_SIGS_RW                  0x00000011 /* Ch A docoder Signals */
#define ACEX_REG_MTi_TEST1_CHB_DEC_SIGS_RW                  0x00000012 /* Ch B docoder Signals */

/* MTI Component Register Bit Masks */
#define MTI_CONFIG_MODE_MASK                                0x00000000
#define MTI_CONFIG_MODE_MTIE_ENABLE                         0x80000000
#define MTI_CONFIG_MTI_BLOCK_STATUS_ENABLE                  0x40000000
#define MTI_CONFIG_MTI_SELECTIVE_MONITOR_DISABLE            0x20000000
#define MTI_CONFIG_CHA_DISABLE                              0x10000000
#define MTI_CONFIG_CHB_DISABLE                              0x08000000
#define MTI_CONFIG_GAP_CHECK_ENABLE                         0x04000000
#define MTI_CONFIG_BROADCAST_DISABLE                        0x02000000
#define MTI_CONFIG_1553A_MCODES_ENABLE                      0x01000000
#define MTI_CONFIG_OVERRIDE_MC_TR_ERR_ENABLE                0x00800000
#define MTI_CONFIG_BUSY_BIT_NODATA_VALID_FORMAT_ENABLE      0x00400000
#define MTI_CONFIG_EOM_TTAG_ENABLE                          0x00200000
#define MTI_CONFIG_BWE_SWAP_ENABLE                          0x00100000
#define MTI_CONFIG_BUS_SWITCH_EOM_DISABLE                   0x00080000
#define MTI_CONFIG_RESERVED_18                              0x00040000
#define MTI_CONFIG_RESERVED_17                              0x00020000
#define MTI_CONFIG_RESERVED_16                              0x00010000

/* MRT Stream Macros */
#define MAX_STREAMS 4
#define MRT_STREAM_DBLK_RX 0
#define MRT_STREAM_DBLK_TX 1

/* ========================================================================== */
/*                         ACEX MTI MACRO DEFINITIONS                         */
/* ========================================================================== */

#define ACEX_REG_BD_CONFIG                                  0x17

#define BD_CONFIG_MASK_EXT_TRG_SSFLAG_SELECT_4              0x00080000
#define BD_CONFIG_MASK_EXT_TRG_SSFLAG_SELECT_2              0x00040000
#define BD_CONFIG_MASK_EXT_TRG_SSFLAG_SELECT_1              0x00020000
#define BD_CONFIG_MASK_EXT_TRG_SSFLAG_SELECT_0              0x00010000
#define BD_CONFIG_MASK_IRIG_INPUT_RANGE                     0x00000020

#define BD_CONFIG_OFFSET_IRIG_INPUT_RANGE                   5
#define BD_CONFIG_OFFSET_EXT_TRG_SSFLAG_SELECT              16

/* MTI Configuration Constants */
#define ACEX_MTI_CONF   20
#define ACEX_MTI_FREE   21
#define ACEX_MTI_INIT   22

/* MTI Component Write masks */
#define MTI_STROBE_HOST_TRIG_INT                            0x00000008
#define MTI_STROBE_PAUSE_MONITOR                            0x00000004
#define MTI_STROBE_START_RESUME_MONITOR                     0x00000002
#define MTI_STROBE_RESET_MONITOR                            0x00000001

/* MTI Component Read masks */
#define MTI_STATUS_READ_MTIE_ACTIVE                         0x00000010
#define MTI_STATUS_READ_MONITOR_ACTIVE                      0x00000008
#define MTI_STATUS_READ_MONITOR_PAUSED                      0x00000004
#define MTI_STATUS_READ_MONITOR_PAUSE_PENDING               0x00000002
#define MTI_STATUS_READ_MONITOR_ENABLED                     0x00000001

/* The following is a return value for MTi Data and Time packet
 * retrieval if time duration is forever and no data is available */
#define ACEX_MTI_IO_TIMEOUT_PENDING_VALUE                   -100
#define ACEX_MTI_IO_TIMEOUT_PENDING_STATUS                  MAKE_STATUS(STATUS_SEVERITY_ERROR, \
        FACILITY_IO_ERROR_CODE, \
        ACEX_MTI_IO_TIMEOUT_PENDING_VALUE)

/* ========================================================================== */
/*                      ACEX CONFIGURE MACRO DEFINITIONS                      */
/* ========================================================================== */

/* Time Tag Registers */
#if 0
defined (PPC) || defined( __powerpc__ )
#define ACEX_TT_LSB                                         0x00000002 /* timetag lsb read */
#define ACEX_TT_MSB                                         0x00000003 /* timetag msb read - must read first & write last */
#endif /* if 0 */
#define ACEX_TT_MSB                                         0x00000002 /* timetag msb read */
#define ACEX_TT_LSB                                         0x00000003 /* timetag lsb read - must read first & write last */
#define ACEX_PULSE_REG                                      0x00000001 /* Global Control Pulsed Registers */

/* Control Pulse Register Bit-Mask Values */
#define ACEX_CTRL_PULSE_CLEAR_RAM_SELFTEST                  0x00000008
#define ACEX_CTRL_PULSE_START_RAM_SELFTEST                  0x00000004
#define ACEX_CTRL_PULSE_TIMETAG_TEST_CLK                    0x00000002
#define ACEX_CTRL_PULSE_TIMETAG_RESET                       0x00000001

/* Time Tag Rollover Point parameter can be one of the following */
#define ACEX_TT_ROLLOVER_POINT_16_BITS                      0
#define ACEX_TT_ROLLOVER_POINT_17_BITS                      1
#define ACEX_TT_ROLLOVER_POINT_18_BITS                      2
#define ACEX_TT_ROLLOVER_POINT_19_BITS                      3
#define ACEX_TT_ROLLOVER_POINT_20_BITS                      4
#define ACEX_TT_ROLLOVER_POINT_21_BITS                      5
#define ACEX_TT_ROLLOVER_POINT_22_BITS                      6
#define ACEX_TT_ROLLOVER_POINT_48_BITS                      7

/***********************************************************************
 * 1553 MAIN COMPONENT REG_GENERAL_INT_MASK BIT MASKS                  *
 * These bit definitions were place here due to Library requires       *
 * this knowledge.                                                     *
 ***********************************************************************/
#define ACEX_GEN_INT_MASK_RESERVED_MASK_31_07               0xFFFFFF80
#define ACEX_GEN_INT_MASK_IMP_INT_ENABLED                   0x00000040
#define ACEX_GEN_INT_MASK_MT_INT_ENABLED                    0x00000020
#define ACEX_GEN_INT_MASK_RT_INT_ENABLED                    0x00000010
#define ACEX_GEN_INT_MASK_BC_INT_ENABLED                    0x00000008
#define ACEX_GEN_INT_MASK_RAM_SELF_TEST_DONE                0x00000004
#define ACEX_GEN_INT_MASK_RAM_PARITY_DETECTED               0x00000002
#define ACEX_GEN_INT_MASK_TT_ROLLOVER                       0x00000001

/* Coupling / Termination Constant */
#define ACEX_COUPLING_TERMINATION_CAPABILITY                0x04000000

#include "structs.h"
#include "structx.h"
#include "linklist.h"
#include "memmgr.h"
#include "deviceop_common.h"
#include "deviceop.h"
#include "devicex_common.h"
#include "devicex.h"
#include "regmap.h"

/* ========================================================================== */
/*                 EMACE CONFIG INTERNAL FUNCTION PROTOTYPES                  */
/* ========================================================================== */
S16BIT _DECL _coSetInternalIsr
(
    S16BIT DevNum,
    U16BIT (_DECL *funcInternalIsr)
    (
        S16BIT DevNum,
        U32BIT dwIrqStatus
    )
);

S16BIT _DECL _coSetInternalImr
(
    S16BIT DevNum,
    U16BIT bEnable,
    U32BIT dwIMR
);

S16BIT _DECL _coSetExternalImr
(
    S16BIT DevNum,
    U16BIT bEnable,
    U32BIT dwIMR
);

S16BIT _DECL _coSetIrqConfig
(
    S16BIT DevNum,
    U16BIT wLvlOrPulse,
    U16BIT wAutoClear
);

U16BIT _DECL _coBCInternalIsr
(
    S16BIT DevNum,
    U32BIT dwIrqStatus
);

U16BIT _DECL _coMTInternalIsr
(
    S16BIT DevNum,
    U32BIT dwIrqStatus
);

U16BIT _DECL _coRTInternalIsr
(
    S16BIT DevNum,
    U32BIT dwIrqStatus
);

U16BIT _DECL _coRTMTInternalIsr
(
    S16BIT DevNum,
    U32BIT dwIrqStatus
);

U16BIT _DECL _coMRTInternalIsr
(
    S16BIT DevNum,
    U32BIT dwIrqStatus
);

U16BIT _DECL _coDetermineMiniAceDevice
(
    S16BIT DevNum
);

/* ========================================================================== */
/*                   EMACE BC INTERNAL FUNCTION PROTOTYPES                    */
/* ========================================================================== */
S16BIT _DECL _bcSetDefaultConfig
(
    S16BIT DevNum
);

S16BIT _DECL _bcSetGPFState
(
    S16BIT DevNum,
    U16BIT wGPF,
    U16BIT wStateChange
);

/* ========================================================================== */
/*                   EMACE MT INTERNAL FUNCTION PROTOTYPES                    */
/* ========================================================================== */
S16BIT _DECL _mtSetDefaultConfig
(
    S16BIT DevNum
);

S16BIT _DECL _mtLockStackPtr
(
    S16BIT DevNum
);

U16BIT _DECL _mtGetMsgAtAddr
(
    S16BIT DevNum,
    U32BIT dwCmdStkAddr,
    MTSTACK *pStk,
    U16BIT *pBuffer,
    U16BIT bPurge
);

U16BIT _DECL _mtGetMsgAtAddr32
(
    S16BIT DevNum,
    U32BIT dwCmdStkAddr,
    MTSTACK *pStk,
    U32BIT *pBuffer,
    U16BIT bPurge
);

U16BIT _DECL _mtCheckForTrigger
(
    S16BIT DevNum,
    U16BIT *pBuffer
);

/* ========================================================================== */
/*                 EMACE RT(MT) INTERNAL FUNCTION PROTOTYPES                  */
/* ========================================================================== */
S16BIT _DECL _rtSetDefaultConfig
(
    S16BIT DevNum
);

S16BIT _DECL _rtLockStackPtr
(
    S16BIT DevNum
);

S16BIT _DECL _rtmtSetDefaultConfig
(
    S16BIT DevNum
);

S16BIT _DECL _rtmtTmpMTStkToHBuf
(
    S16BIT DevNum
);

S16BIT _DECL _rtmtTmpMTStkToHBuf32
(
    S16BIT DevNum
);

S16BIT _DECL _rtmtMergeHBufs
(
    S16BIT DevNum
);

/* ========================================================================== */
/*                  EMACE TEST INTERNAL FUNCTION PROTOTYPES                   */
/* ========================================================================== */
S16BIT _DECL _tsSetDefaultConfig
(
    S16BIT DevNum
);

/* ========================================================================== */
/*               ACEX UNIFIED MAP INTERNAL FUNCTION PROTOTYPES                */
/* ========================================================================== */
typedef struct _ACEX_UM_COMPONENT_INFO
{
    U32BIT umComponentType;
    S8BIT umName[20];
    U32BIT umComponentRev;
    U32BIT umComponentRegSize;
    U32BIT umRegBaseAddr[ACEX_UM_MAX_NUM_INSTANCES];

} ACEX_UM_COMPONENT_INFO, *PACEX_UM_COMPONENT_INFO;

typedef struct _ACEX_UM_DEVICE_INFO
{
    U32BIT umDevType;
    S8BIT umName[20];
    U32BIT umDevRev;
    U32BIT umDevMemSize;
    U32BIT umDevNumInstances;
    U32BIT umMemBaseAddr[ACEX_UM_MAX_NUM_INSTANCES];
    U32BIT umDevNumComponents;
    ACEX_UM_COMPONENT_INFO umComponentInfo[ACEX_UM_MAX_NUM_COMPONENTS];

} ACEX_UM_DEVICE_INFO, *PACEX_UM_DEVICE_INFO;

typedef struct _ACEX_UM_INFO
{
    S8BIT s8BrdModelNum[10];
    U32BIT u32DataArcNum;
    U32BIT firmwareRelVersion;
    U32BIT firmwareIntVersion;
    U32BIT numDevices;
    ACEX_UM_DEVICE_INFO umDeviceInfo[ACEX_UM_MAX_NUM_DEVICES];

} ACEX_UM_INFO, *PACEX_UM_INFO;

/* ========================================================================== */
/*                 ACEX CONFIG INTERNAL FUNCTION PROTOTYPES                   */
/* ========================================================================== */

typedef struct _ACEX_BOARD_GLOBAL_REG_TYPE
{
    U32BIT u32ModelNum;
    U32BIT u32Capability;
    U32BIT u32DeviceCount[7];
    U32BIT u32FlashStartAddr;
    U32BIT u32FlashNumClusters;
    U32BIT u32FlashClusterSize;
    U32BIT u32Reserved0C;
    U32BIT u32Reserved0D;
    U32BIT u32Reserved0E;
    U32BIT u32ReservedOF;
    U32BIT u32DiscreteIOCtl;
    U32BIT u32Reserved11;
    U32BIT u32Reserved12;
    U32BIT u32Reserved13;
    U32BIT u32IntStatus;
    U32BIT u32IntEn;
    U32BIT u32IntDisStr;
    U32BIT u32Config;
    U32BIT u32ConfigPulses;
    U32BIT u32Status;

} ACEX_BOARD_GLOBAL_REG, *PACEX_BOARD_GLOBAL_REG;

typedef struct _ACEX_BOARD_RESET_TYPE
{
    U32BIT *pu32RegBA; /* ptr to Reset Registers base address */
    U32BIT *pu32RegSize; /* ptr to Reset Register size */

} ACEX_BOARD_RESET;

typedef struct _BRD_MEM_CONFIG_TYPE
{
    U32BIT *pu32RegBA; /* ptr to Reset Registers base address */
    U32BIT *pu32RegSize; /* ptr to Reset Register size */

} ACEX_BOARD_MEM_CONFIG;

typedef struct _ACEX_BOARD_TYPE
{
    ACEX_BOARD_MEM_CONFIG sBdMemConfig;
    U32BIT                *pu32RegBA; /* ptr to 1553 BC Registers base address */
    U32BIT                *pu32RegSize; /* ptr to 1553 Global Register size */
    ACEX_BOARD_GLOBAL_REG sBdReg;
    ACEX_BOARD_RESET sBdReset;

}ACEX_BOARD_TYPE;


/* ========================================================================== */
/*                   ACEX BC INTERNAL FUNCTION PROTOTYPES                     */
/* ========================================================================== */
S16BIT _DECL _bcxFree
(
    S16BIT s16DevNum
);

S16BIT _DECL _bcxDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

/* ========================================================================== */
/*                   ACEX MT INTERNAL FUNCTION PROTOTYPES                     */
/* ========================================================================== */

/* ========================================================================== */
/*                   ACEX MTI INTERNAL FUNCTION PROTOTYPES                    */
/* ========================================================================== */
S16BIT _DECL _mtxSetDefaultConfig
(
    S16BIT s16DevNum
);

S16BIT _DECL _mtxDecodeRawMsg
(
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

/* ========================================================================== */
/*                   ACEX MRT INTERNAL FUNCTION PROTOTYPES                    */
/* ========================================================================== */
_EXTERN S16BIT _DECL _rtxFree
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _rtxRTConfigure
(
    S16BIT DevNum,
    U16BIT wCmdStkSize,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _rtxDataBlkCreate
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT u16DataBlkType,
    U16BIT *pBuffer,
    U16BIT u16BufferSize
);

_EXTERN S16BIT _DECL _rtxDataBlkDelete
(
    S16BIT DevNum,
    S16BIT nDataBlkID
);

_EXTERN S16BIT _DECL _rtxDataBlkWrite
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT u16BufferSize,
    U16BIT u16Offset
);

_EXTERN S16BIT _DECL _rtxDataBlkRead
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pBuffer,
    U16BIT u16BufferSize,
    U16BIT u16Offset
);

_EXTERN S16BIT _DECL _rtxInstallHBuf
(
    S16BIT DevNum,
    U32BIT dwHBufSize
);

_EXTERN S16BIT _DECL _rtxGetHBufMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _rtxGetHBufMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize,
    U32BIT *pdwMsgCount,
    U32BIT *pdwMsgLostStk,
    U32BIT *pdwMsgLostHBuf
);

_EXTERN S16BIT _DECL _rtxGetHBufMetric
(
    S16BIT DevNum,
    HBUFMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _rtxGetHBufMsgCount
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _rtxUninstallHBuf
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _rtxStkToHBuf32
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _rtxDecodeRawMsg
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    MSGSTRUCT *pMsg
);

_EXTERN S16BIT _DECL _rtxAcquireAddress
(
    S16BIT DevNum,
    U16BIT *wRTAddr,
    U16BIT wCANBus
);

_EXTERN S16BIT _DECL _rtxGetAddress
(
    S16BIT DevNum,
    U16BIT *pRTAddress
);

_EXTERN S16BIT _DECL _rtxGetAddrSource
(
    S16BIT DevNum,
    U16BIT *wRTSource
);

_EXTERN S16BIT _DECL _rtxRelatchAddr
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _rtxSetAddress
(
    S16BIT DevNum,
    U16BIT wRTAddress
);

_EXTERN S16BIT _DECL _rtxSetAddrSource
(
    S16BIT DevNum,
    U16BIT wRTSource
);

_EXTERN S16BIT _DECL _rtxSetDefaultConfig
(
    S16BIT DevNum,
    U8BIT u8UseSRTCmdStkSize
);

_EXTERN S16BIT _DECL _rtxGetStkMetric
(
    S16BIT DevNum,
    STKMETRIC *pMetric,
    U16BIT bReset
);

_EXTERN S16BIT _DECL _rtxDataBlkCircBufInfo
(
    S16BIT DevNum,
    S16BIT nDataBlkID,
    U16BIT *pUserRWOffset,
    U16BIT *pAceRWOffset
);

_EXTERN S16BIT _DECL _rtxGetStkMsgsRaw
(
    S16BIT DevNum,
    U16BIT *pBuffer,
    U16BIT wBufferSize
);

_EXTERN S16BIT _DECL _rtxGetStkMsgDecoded
(
    S16BIT DevNum,
    MSGSTRUCT *pMsg,
    U16BIT wMsgLoc
);

_EXTERN S16BIT _DECL _rtxStrMemAlloc
(
    S16BIT DevNum,
    S16BIT *pDBlkID,
    U32BIT *pu32DwdPtr,
    U32BIT u32BufWdSize,
    U8BIT bAlloc
);

_EXTERN S16BIT _DECL _rtxStrDBlkMapToRTSA
(
    S16BIT DevNum,
    ACEX_MRT_STREAM_TYPE *pMrtStream,
    U16BIT u16MsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL _rtxDataStreamReset
(
    S16BIT s16DevNum,
    S8BIT s8RTAddr,
    U8BIT u8Direction
);

_EXTERN void _DECL _rtxShow
(
    S16BIT DevNum
);

_EXTERN void _DECL _rtxIsqShow
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _acexRTInitialize
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL _acexMRTInitialize
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexMRTInitialize
(
    S16BIT s16DevNum
);

_EXTERN S16BIT _DECL acexMRTFree
(
    S16BIT s16DevNum
);

void _rtxDisplayStkMetric
(
    S16BIT DevNum
);

void _rtxClearISQ
(
    S16BIT DevNum
);

/* ========================================================================== */
/*                  ACEX TEST INTERNAL FUNCTION PROTOTYPES                    */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* __INTERNAL_H__ */
