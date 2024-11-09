/*******************************************************************************
 * FILE: ddc_udl_sdk.h
 *
 * DESCRIPTION:
 *
 *  This file is intended to be the repository for information shared between
 *  the kernel mode driver and user space.
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

#ifndef _DDC_UDL_SDK_H_
#define _DDC_UDL_SDK_H_

/* 1553 IOCTL ACCESS TYPES */


/* MRT OPTIONS */
#define MRT_OPTION_RESET_CMDSTK         0x00000001 /* used with acexMRTStart */

/* Device Family Enumeration - This Device Is ACEXUSB */
typedef enum _DDC_FAMILY
{
    DDC_EMACE = 0,
    DDC_E2MACE,
    DDC_ACEX
    
} DDC_FAMILY;


/*  dwIrqMask parameter can be the following OR'd together */
#define ACE_IMR1_EOM                            0x00000001
#define ACE_IMR1_BC_STATUS_SET                  0x00000002
#define ACE_IMR1_RT_MODE_CODE                   0x00000002
#define ACE_IMR1_MT_PATTERN_TRIG                0x00000002
#define ACE_IMR1_FORMAT_ERR                     0x00000004
#define ACE_IMR1_BC_END_OF_FRM                  0x00000008
#define ACE_IMR1_BC_MSG_EOM                     0x00000010
#define ACE_IMR1_RT_SUBADDR_EOM                 0x00000010
#define ACE_IMR1_RT_CIRCBUF_ROVER               0x00000020
#define ACE_IMR1_TT_ROVER                       0x00000040
#define ACE_IMR1_RT_ADDR_PAR_ERR                0x00000080
#define ACE_IMR1_BC_RETRY                       0x00000100
#define ACE_IMR1_HSHAKE_FAIL                    0x00000200
#define ACE_IMR1_MT_DATASTK_ROVER               0x00000400
#define ACE_IMR1_MT_CMDSTK_ROVER                0x00000800
#define ACE_IMR1_BCRT_CMDSTK_ROVER              0x00001000
#define ACE_IMR1_BCRT_TX_TIMEOUT                0x00002000
#define ACE_IMR1_RAM_PAR_ERR                    0x00004000
#define ACE_IMR2_IRIG_INTERRUPT                 0x00008000
#define ACE_IMR2_BIT_TRIGGER                    0x00010000
#define ACE_IMR2_BIT_COMPLETE                   0x00020000
#define ACE_IMR2_FLEX_DMA_ENABLE                0x00020000
#define ACE_IMR2_BC_UIRQ0                       0x00040000
#define ACE_IMR2_BC_UIRQ1                       0x00080000
#define ACE_IMR2_BC_UIRQ2                       0x00100000
#define ACE_IMR2_BC_UIRQ3                       0x00200000
#define ACE_IMR2_MT_DSTK_50P_ROVER              0x00400000
#define ACE_IMR2_MT_CSTK_50P_ROVER              0x00800000
#define ACE_IMR2_RT_CIRC_50P_ROVER              0x01000000
#define ACE_IMR2_RT_CSTK_50P_ROVER              0x02000000
#define ACE_IMR2_BC_TRAP                        0x04000000
#define ACE_IMR2_BC_CALLSTK_ERR                 0x08000000
#define ACE_IMR2_GPQ_ISQ_ROVER                  0x10000000
#define ACE_IMR2_RT_ILL_CMD                     0x20000000
#define ACE_IMR2_BC_OPCODE_PARITY               0x40000000
#define ACE_IMR2_MTI_INTERRUPT                  0x80000000



#define BC_HOST_ID_MSG                          0
#define BC_HOST_ID_STREAM                       1
#define BC_HOST_ID_ARRAY                        2
#define BC_HOST_ID_LP                           3
#define BC_HOST_ID_HP                           4

#define ACEX_BC_MAX_INDEX_MSG                   2048
#define ACEX_BC_MAX_INDEX_DATA                  2048

/* response time definitions */
#define ACEX_RESP_MAX_VALUE                     0x0007
#define ACEX_RESP_BC                            0x0001      /* 1553 BC  */
#define ACEX_RESP_MRT                           0x0002      /* 1553 MRT */
#define ACEX_RESP_MTI                           0x0004      /* 1553 MTI */


/* channel mux option definition */
#define ACEX_BC_CHAN_MUX_ENA                    0x100000    /* mux 1553 channels */

/* Time Tag Registers */
#define ACEX_TT_MSB                             0x00000002 /* timetag msb read */
#define ACEX_TT_LSB                             0x00000003 /* timetag lsb read - must read first & write last   */
#define ACEX_PULSE_REG                          0x00000001 /* Global Control Pulsed Registers                   */

/* Time Tag Resolution */
#define ACEX_TT_64US                            0           /* 64us Time Tag resolution                         */
#define ACEX_TT_32US                            1           /* 32us Time Tag resolution                         */
#define ACEX_TT_16US                            2           /* 16us Time Tag resolution                         */
#define ACEX_TT_8US                             3           /* 8us Time Tag resolution                          */
#define ACEX_TT_4US                             4           /* 4us Time Tag resolution                          */
#define ACEX_TT_2US                             5           /* 2us Time Tag resolution                          */
#define ACEX_TT_1US                             6           /* 1us Time Tag resolution                          */
#define ACEX_TT_500NS                           7           /* 500ns Time Tag resolution                        */
#define ACEX_TT_100NS                           8           /* 100ns Time Tag resolution                        */
#define ACEX_TT_TEST_CLK                        9           /* Test Time Tag clock                              */
#define ACEX_TT_EXT                             10          /* Use external TT clock                            */

/* Time Tag Rollover Point */
#define ACEX_TT_ROLLOVER_POINT_16_BITS          0
#define ACEX_TT_ROLLOVER_POINT_17_BITS          1
#define ACEX_TT_ROLLOVER_POINT_18_BITS          2
#define ACEX_TT_ROLLOVER_POINT_19_BITS          3
#define ACEX_TT_ROLLOVER_POINT_20_BITS          4
#define ACEX_TT_ROLLOVER_POINT_21_BITS          5
#define ACEX_TT_ROLLOVER_POINT_22_BITS          6
#define ACEX_TT_ROLLOVER_POINT_48_BITS          7

/*----------------------------------------------------------------------------*/
/* 1553 MAIN COMPONENT REG_GENERAL_INT_MASK BIT MASKS                         */
/* These bit definitions were place here due to Library requires              */
/* this knowledge.                                                            */
/*----------------------------------------------------------------------------*/
#define GENERAL_INT_MASK_RESERVED_MASK_31_11    0xFFFFF800
#define GENERAL_INT_MASK_TRG_INT_ENABLED        0x00000400
#define GENERAL_INT_MASK_REPLAY_INT_ENABLED     0x00000200
#define GENERAL_INT_MASK_MRT_IMP_INT_ENABLED    0x00000100
#define GENERAL_INT_MASK_BC_IMP_INT_ENABLED     0x00000080
#define GENERAL_INT_MASK_IMP_INT_ENABLED        0x00000040
#define GENERAL_INT_MASK_MT_INT_ENABLED         0x00000020
#define GENERAL_INT_MASK_RT_INT_ENABLED         0x00000010
#define GENERAL_INT_MASK_BC_INT_ENABLED         0x00000008
#define GENERAL_INT_MASK_RAM_SELF_TEST_DONE     0x00000004
#define GENERAL_INT_MASK_RAM_PARITY_DETECTED    0x00000002
#define GENERAL_INT_MASK_TT_ROLLOVER            0x00000001


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
typedef struct _ACEX_CONFIG_ID
{
    U16BIT u16Type;
    U16BIT u16Channel;

} ACEX_CONFIG_ID;

/* BC Message Buffer Management Macros */
#define ACEX_BC_MESSAGE_FULL                    0
#define ACEX_BC_MESSAGE_HDR                     1

/* BC Host Buffer and GpQueue Metric Type Macros*/
#define ACEX_BC_METRIC_HBUF                     0
#define ACEX_BC_METRIC_GPQ                      1

#define ACEX_GETMSG_TYPE                        0x0000FFFF
#define ACEX_GETMSG_BLOCKING                    0x00FF0000
#define ACEX_GETMSG_CHECK_IMP                   0xFF000000

/* BC Host Buffer Message Management Macros - These map to ACE_BC_MSGLOC_NEXT_PURGE, ... */
#define ACEX_BC_HBUF_NEXT_PURGE                 0   /* next unread msg, clr stk     */
#define ACEX_BC_HBUF_NEXT_NPURGE                1   /* next unread msg, leave stk   */
#define ACEX_BC_HBUF_LATEST_PURGE               2   /* latest msg, clear stk        */
#define ACEX_BC_HBUF_LATEST_NPURGE              3   /* latest msg, leave stk alone  */
#define ACEX_BC_HBUF_FILL_BUFFER                4   /* fill msg buffer, clear stk   */

/* BC Buffer Read Error Definitions - Translated to ACE errors in axdevice.c */
#define ACEX_BC_BUF_STS_SUCCESS                 0
#define ACEX_BC_BUF_STS_INVALID_BUF             -1

/* BC ASYNC error code */
#define ACEX_BC_ASYNC_STS_SUCCESS               0
#define ACEX_BC_ASYNC_STS_QUEUE_FULL            -1
#define ACEX_BC_ASYNC_STS_INVALID_MODE          -2
#define ACEX_BC_ASYNC_STS_USB_FAIL              -3
#define ACEX_BC_ASYNC_STS_DATARRAY_FULL         -4

/* BC Async Queue Type Macros */
#define ACEX_BC_QUEUE_LP                        0
#define ACEX_BC_QUEUE_HP                        1

/* BC Queue Size Macros */
#define ACEX_MEMSZ_BC_GPQ_256                   0x0100      /* BC GPQ 256 dwords long                   */
#define ACEX_MEMSZ_BC_GPQ_64                    0x0040      /* BC GPQ 64 dwords long                    */
#define ACEX_MEMSZ_BC_LPQ_512                   0x0200      /* BC LPQ 512 dwords long                   */
#define ACEX_MEMSZ_BC_LPQ_256                   0x0100      /* BC LPQ 256 dwords long                   */
#define ACEX_MEMSZ_BC_LPQ_128                   0x0080      /* BC LPQ 128 dwords long                   */
#define ACEX_MEMSZ_BC_LPQ_64                    0x0040      /* BC LPQ 64 dwords long                    */
#define ACEX_MEMSZ_BC_LPQ_32                    0x0020      /* BC LPQ 32 dwords long                    */
#define ACEX_MEMSZ_BC_HPQ_512                   0x0200      /* BC HPQ 512 dwords long                   */
#define ACEX_MEMSZ_BC_HPQ_256                   0x0100      /* BC HPQ 256 dwords long                   */
#define ACEX_MEMSZ_BC_HPQ_128                   0x0080      /* BC HPQ 128 dwords long                   */
#define ACEX_MEMSZ_BC_HPQ_64                    0x0040      /* BC HPQ 64 dwords long                    */
#define ACEX_MEMSZ_BC_HPQ_32                    0x0020      /* BC HPQ 32 dwords long                    */

/* BC Slot Routine Size Macros              */
#define ACEX_MEMSZ_BC_SLOT_PARAM                0x0006      /* BC Time Slot Manager 6 dwords long       */
#define ACEX_MEMSZ_BC_SLOT_MAIN                 0x0030      /* BC Time Slot Manager 48 dwords long      */
#define ACEX_MEMSZ_BC_SLOT_RESET                0x0012      /* BC Time Slot Manager 18 dwords long      */
#define ACEX_MEMSZ_BC_SLOT_ITEM                 0x0004      /* BC Time Slot Item Routine 4 dwords long  */

/* BC User Interrupt Signal Macros */
#define ACEX_BC_UIRQ_NONE                       0
#define ACEX_BC_UIRQ0                           1
#define ACEX_BC_UIRQ1                           2
#define ACEX_BC_UIRQ2                           4
#define ACEX_BC_UIRQ3                           8

/* BC external trigger information */
#define ACEX_BC_EXT_TRIGGER_NOT_AVAIL           0x00000000
#define ACEX_BC_EXT_TRIGGER_AVAIL               0x00000001
#define ACEX_BC_EXT_TRIGGER_USING_DIO           0x00010000
#define ACEX_BC_EXT_TRIGGER_USING_DIRECT_INPUT  0x00020000


/*----------------------------------------------------------------------------*/
/* aceGetBoardStatus CMD parameters 	                                      */
/*----------------------------------------------------------------------------*/
#define DDC_BOARD_STATUS_CMD_TEMPERATURE 1
#define DDC_BOARD_STATUS_CMD_READBLOCK   2
#define DDC_BOARD_STATUS_CMD_WRITEBLOCK  3

/*----------------------------------------------------------------------------*/
/* This structure describes Thermal Detection parameters                     */
/*----------------------------------------------------------------------------*/
#define DEVICE_TEMPERATURE_ALL                      0x000000FF
#define DEVICE_TEMPERATURE_GENERAL                  0x00000001
#define DEVICE_TEMPERATURE_DEVICE_1                 0x00000002
#define DEVICE_TEMPERATURE_DEVICE_2                 0x00000004
#define DEVICE_TEMPERATURE_DEVICE_3                 0x00000008
#define DEVICE_TEMPERATURE_DEVICE_4                 0x00000010
#define DEVICE_TEMPERATURE_DEVICE_5                 0x00000020
#define DEVICE_TEMPERATURE_DEVICE_6                 0x00000040
#define DEVICE_TEMPERATURE_DEVICE_7                 0x00000080
#define DEVICE_TEMPERATURE_CELCIUS                  0x00000000 * Default Value
#define DEVICE_TEMPERATURE_FAHRENHEIT               0x80000000

#define MAX_TEMPERATURE_DEVICE_NUMBER               8

typedef struct _DEVICE_TEMPERATURE_TYPE
{
    U32BIT u32_Options_Flags;         /* returns Data depending on the flag */
    S16BIT s16_GeneralBoard_temp;     /* returns General board temperature */
    S16BIT s16_Device_1_Temperature;  /* returns Device 1 temperature */
    S16BIT s16_Device_2_Temperature;  /* returns Device 2 temperature */
    S16BIT s16_Device_3_Temperature;  /* returns Device 3 temperature */
    S16BIT s16_Device_4_Temperature;  /* returns Device 4 temperature */
    S16BIT s16_Device_5_Temperature;  /* returns Device 5 temperature */
    S16BIT s16_Device_6_Temperature;  /* returns Device 6 temperature */
    S16BIT s16_Device_7_Temperature;  /* returns Device 7 temperature */
    
} DEVICE_TEMPERATURE_TYPE;


/*----------------------------------------------------------------------------*/
/* Board Features Parameters                                                  */
/*----------------------------------------------------------------------------*/
/* 0:disable external clock     1:enable external clock */
#define DDC_BOARD_FEATURE_CMD_EXTERNAL_CLOCK        1


#define DDC_BOARD_FEATURE_MAX                       DDC_BOARD_FEATURE_CMD_EXTERNAL_CLOCK


#endif /* _DDC_UDL_SDK_H_ */
