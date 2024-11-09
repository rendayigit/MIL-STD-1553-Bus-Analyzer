/*******************************************************************************
 * FILE: ddc_udl_arinc717.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support ARINC 717
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

#ifndef _DDC_UDL_ARINC717_H_
#define _DDC_UDL_ARINC717_H_

/* ========================================================================== */
/* ARINC 717 programmable configuration options                               */
/* ========================================================================== */
#define ARINC_717_PROGRMMABLE_SLOPE_OPT         0x00000001
#define ARINC_717_PROGRMMABLE_WRAP_AROUND_OPT   0x00000002
#define ARINC_717_PROGRMMABLE_STOP_OPT          0x00000004
#define ARINC_717_PROGRMMABLE_AUTO_DETECT_OPT   0x00000008
#define ARINC_717_PROGRMMABLE_RESET_OPT         0x00000010
#define ARINC_717_PROGRMMABLE_BUFFER_MODE_OPT   0x00000020
#define ARINC_717_PROGRMMABLE_PROTOCOL_OPT      0x00000040
#define ARINC_717_PROGRMMABLE_TYPE_OPT          0x00000080
#define ARINC_717_PROGRMMABLE_SPEED_OPT         0x00000200
#define ARINC_717_PROGRMMABLE_FRAME_COUNT_OPT   0x00000400

/* ========================================================================== */
/* ARINC 717 programmable Tx load options                                     */
/* ========================================================================== */
#define ARINC_717_TX_DATA_PRIMARY_BUFFER_OPT    0x00000001
#define ARINC_717_TX_DATA_SECONDARY_BUFFER_OPT  0x00000002

/* ========================================================================== */
/* ARINC 717 interrupts enable/status masks                                   */
/* ========================================================================== */
#define ARINC_717_CH1_INT                           0x00000001
#define ARINC_717_CH2_INT                           0x00000002
#define ARINC_717_PROG_INT_TX_MARKER0_ENA           0x00000001  /* 1st location of of 1st buffer read */
#define ARINC_717_PROG_INT_TX_MARKER1_ENA           0x00000002  /* 1st location of of 2nd buffer read */
#define ARINC_717_PROG_INT_RX_HALF_SUB_FRAME_ENA    0x00010000  /* mid subframe word read */
#define ARINC_717_PROG_INT_RX_SUB_FRAME_ENA         0x00020000  /* next subframe word read */
#define ARINC_717_PROG_INT_RX_50_PC_MEM_ENA         0x00040000  /* 50% of memory written */
#define ARINC_717_PROG_INT_RX_100_PC_MEM_ENA        0x00080000  /* next word at beggining of memory written */
#define ARINC_717_PROG_INT_RX_REC_SYNCED_ENA        0x00100000  /* valid sync word detected when not synced */
#define ARINC_717_PROG_INT_RX_REC_SYNCED_ERR_ENA    0x00200000  /* valid sync word not detected at beginning of frame */
#define ARINC_717_PROG_INT_RX_AUTO_DETECT_LOCK_ENA  0x00400000  /* receiver detected correct speed */
#define ARINC_717_PROG_INT_RX_AUTO_DETECT_LOST_ENA  0x00800000  /* receiver lost lock on correct speed due to bit errors */
#define ARINC_717_PROG_INT_RX_BIT_ERR_DETECTED_ENA  0x01000000  /* Reserved bits */
#define ARINC_717_PROG_INT_NOT_USED_ENA             0x01FF0003  /* Reserved bits */

/* ========================================================================== */
/* ARINC 717 programmable configuration masks                                 */
/* ========================================================================== */
#define ARINC_717_PROG_SLOPE_RATE_MASK              0x0000C000  /* Bits 14 & 15 control waveform slope */
#define ARINC_717_PROG_SLOPE_RATE_65PF_MASK         0x00000000  /* 68 Pf */
#define ARINC_717_PROG_SLOPE_RATE_398PF_MASK        0x00008000  /* 398 Pf */
#define ARINC_717_PROG_SLOPE_RATE_538PF_MASK        0x00004000  /* 538 Pf */
#define ARINC_717_PROG_SLOPE_RATE_868PF_MASK        0x0000C000  /* 868 Pf */

#define ARINC_717_PROG_EXT_WRAP_AROUND_MASK         0x00002000  /* Enables exteranl transmitter/receiver wraparound mode */
#define ARINC_717_PROG_TX_STOP_MASK                 0x00001000  /* Enables exteranl transmitter/receiver wraparound mode */
#define ARINC_717_PROG_RX_AUTO_DETECT_MASK          0x00000800  /* Receiver auto detect transmit speed */
#define ARINC_717_PROG_INT_WRAP_AROUND_MASK         0x00000400  /* Transmitter will transmit data interanally to receiver */
#define ARINC_717_PROG_RESET_MASK                   0x00000200  /* Resets all registers and protocol engines */

#define ARINC_717_PROG_BUFFER_MODE_MASK             0x00000180  /* Buffer mode selection */
#define ARINC_717_PROG_BUFFER_MODE_SINGLE_MASK      0x00000000  /* Singular buffer mode */
#define ARINC_717_PROG_BUFFER_MODE_DOUBLE_MASK      0x00000080  /* Double buffer mode */
#define ARINC_717_PROG_BUFFER_MODE_CIRCULAR_MASK    0x00000100  /* Circular buffer mode. Valid only for Receiver! */

#define ARINC_717_PROG_BPRZ_HBP_SELECT_MASK         0x00000040  /* Tx/rx BPRZ(1) or HBP(0) mode selection */
#define ARINC_717_PROG_CHANNEL_MODE_MASK            0x00000020  /* Tx(1) or Rx(0) mode selection. Bit ignored during loopback! */
#define ARINC_717_PROG_ENABLE_MASK                  0x00000010  /* Enables channel operation; Tx or Rx active */

#define ARINC_717_PROG_SPEED_MASK                   0x0000000F  /* Speed settings */
#define ARINC_717_PROG_SPEED_32WPS_MASK             0x00000000  /* 32 words/s. actual is 64 words/s due to odd word padding */
#define ARINC_717_PROG_SPEED_64WPS_MASK             0x00000001  /* 64 words/s. 12 bits/word */
#define ARINC_717_PROG_SPEED_128WPS_MASK            0x00000002  /* 128 words/s */
#define ARINC_717_PROG_SPEED_256WPS_MASK            0x00000003  /* 256 words/s */
#define ARINC_717_PROG_SPEED_512WPS_MASK            0x00000004  /* 512 words/s */
#define ARINC_717_PROG_SPEED_1024WPS_MASK           0x00000005  /* 1024 words/s */
#define ARINC_717_PROG_SPEED_2048WPS_MASK           0x00000006  /* 2048 words/s */
#define ARINC_717_PROG_SPEED_4096WPS_MASK           0x00000007  /* 4096 words/s */
#define ARINC_717_PROG_SPEED_8192WPS_MASK           0x00000008  /* 8192 words/s */

/* ========================================================================== */
/* ARINC 717 Configuration Register Constants                                 */
/* ========================================================================== */

/* Slope control, bits [15:14] of configuration register */
#define ARINC_717_PROG_TX_SLOPE_CONTROL_SHIFT           14
#define ARINC_717_PROG_TX_SLOPE_CONTROL_MASK            0x3
#define ARINC_717_PROG_TX_SLOPE_CONTROL_68PF            0x0
#define ARINC_717_PROG_TX_SLOPE_CONTROL_398PF           0x1
#define ARINC_717_PROG_TX_SLOPE_CONTROL_538PF           0x2
#define ARINC_717_PROG_TX_SLOPE_CONTROL_868PF           0x3

/* External bit [13] & internal bit [10] wrap around configuration register */
#define ARINC_717_PROG_EXT_WRAP_AROUND                  0x0
#define ARINC_717_PROG_INT_WRAP_AROUND                  0x1
#define ARINC_717_PROG_WRAP_NONE                        0x2

/* Buffer mode, bits [8:7] of configuration register */
#define ARINC_717_PROG_BUF_MODE_SINGLE                  0x0
#define ARINC_717_PROG_BUF_MODE_DOUBLE                  0x1
#define ARINC_717_PROG_BUF_MODE_CIRCULAR                0x2     /* Only valid in receiver mode */
#define ARINC_717_PROG_BUF_MODE_INVALID                 0x3

/* Bi Polar Return to Zero (1) or Harvard Bi Phase (0) mode select, bit [6] of configuration register */
#define ARINC_717_PROG_BPRZ_MASK                        0x0
#define ARINC_717_PROG_HBP_MASK                         0x1

/* Channel enable, bit [4] of configuration register.  Enables send/receive operation */
#define ARINC_717_PROG_CHANNEL_ENABLE_SHIFT             4
#define ARINC_717_PROG_CHANNEL_ENABLE                   0x1

/* Channel speed, bit [3:0] of configuration register */
#define ARINC_717_PROG_SPEED_SHIFT                      0
#define ARINC_717_PROG_SPEED_32_WPS                     0x0     /* 384   bps */
#define ARINC_717_PROG_SPEED_64_WPS                     0x1     /* 768   bps */
#define ARINC_717_PROG_SPEED_128_WPS                    0x2     /* 1536  bps */
#define ARINC_717_PROG_SPEED_256_WPS                    0x3     /* 3072  bps */
#define ARINC_717_PROG_SPEED_512_WPS                    0x4     /* 6144  bps */
#define ARINC_717_PROG_SPEED_1024_WPS                   0x5     /* 12288 bps */
#define ARINC_717_PROG_SPEED_2048_WPS                   0x6     /* 24576 bps */
#define ARINC_717_PROG_SPEED_4096_WPS                   0x7     /* 49152 bps */
#define ARINC_717_PROG_SPEED_8192_WPS                   0x8     /* 98304 bps */
#define ARINC_717_PROG_SPEED_INVALID_WPS                0xFF

/* ========================================================================== */
/* ARINC 717 sync words                                                       */
/* ========================================================================== */
#define ARINC_717_SYNC_WORD_MASK                    0x00000FFF
#define ARINC_717_SYNC_WORD_ONE                     0x00000247 /* Octal 1107 */
#define ARINC_717_SYNC_WORD_TWO                     0x000005B8 /* Octal 2670 */
#define ARINC_717_SYNC_WORD_THREE                   0x00000A47 /* Octal 5107 */
#define ARINC_717_SYNC_WORD_FOUR                    0x00000DB8 /* Octal 6670 */


typedef enum _ARINC_717_STATE
{
    ARINC_717_RESET,
    ARINC_717_READY,
    ARINC_717_RUN,
    ARINC_717_PAUSE

} ARINC_717_STATE;


typedef struct _ARINC_717_PROGRMMABLE_CONFIG
{
    ACEX_CONFIG_ID sConfigID;
    U32BIT u32ConfigOption;
    U8BIT u8Channel;
    U8BIT u8SlopeControl;
    U8BIT u8WrapAroundMode;
    U8BIT bStopTx;
    U8BIT bRxAutoDetect;
    U8BIT bReset;
    U8BIT u8BufferMode;
    U8BIT u8ProtocolType;
    U8BIT u8Type;
    U8BIT u8Speed;
    ARINC_717_STATE eState;         /* Used only during RUN state setting */
    U16BIT u16FrameCount;           /* Frame count for transmitter mode only */
    U8BIT bGetInterrupt;
    U8BIT bEnableInterrupt;
    U32BIT u32Interrupts;

} ARINC_717_PROGRMMABLE_CONFIG, *PARINC_717_PROGRMMABLE_CONFIG;


#endif /* _DDC_UDL_ARINC717_H_ */
