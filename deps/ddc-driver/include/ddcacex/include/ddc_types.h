/*******************************************************************************
 * FILE: ddc_types.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the Global Board Module.
 *
 *  This module contains DDC types and definitions.
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

#ifndef _DDC_TYPES_
#define _DDC_TYPES_

#include "os/include/ddc_os_types.h"   /* OS specific definitions for common types */

#ifndef FALSE
#define FALSE                 0
#endif /* ifndef FALSE */

#ifndef TRUE
#define TRUE                  1
#endif /* ifndef TRUE */

#define DDC_MAX_DEVICE_NAME_LEN     32

#ifndef _DECL
#define _DECL
#endif

/* ========================================================================== */
/* Family Number Macros                                                       */
/* ========================================================================== */

/*
     31  16 15   0
    +------+------+
    | 429  | 1553 |
    +------+------+
*/

#define DDC_SET_429_FAMILY_NUMBER(x, y) \
    { \
        x &= 0x0000FFFF; /* clear out value first */ \
        x |= ((y & 0xFFFF) << 16); \
    }

#define DDC_SET_1553_FAMILY_NUMBER(x, y) \
    { \
        x &= 0xFFFF0000; /* clear out value first */ \
        x |= (y & 0xFFFF); \
    }

#define DDC_GET_429_FAMILY_NUMBER(x) (FAMILY)((x & 0xFFFF0000) >> 16)
#define DDC_GET_1553_FAMILY_NUMBER(x) (FAMILY)(x & 0x0000FFFF)

/* ========================================================================== */
/* ========================================================================== */

typedef enum _FAMILY
{
    EMACE = 0,
    E2MACE,
    ACEX,
    SYNCHRO,
    DDCFAMILY_429X

} FAMILY;


typedef enum
{
    DISC_INPUT = 0,
    DISC_OUTPUT = 1

} DISC_DIR;


typedef enum
{
    DISC_LOW = 0,
    DISC_HIGH = 1

} DISC_LEVEL;


typedef enum
{
    AVIONIC_INPUT = 0,
    AVIONIC_OUTPUT = 1

} AVIONIC_DIRECTION;


typedef enum
{
    AVIONIC_LOW = 0,
    AVIONIC_HIGH = 1

} AVIONIC_LEVEL;


enum
{
    ARINC429_TX_CHANNEL = 0,
    ARINC429_RX_CHANNEL,
    ARINC429_UNDEFINED_CHANNEL
};

enum
{
    ARINC717_TX_CHANNEL = 0,
    ARINC717_RX_CHANNEL,
    ARINC717_UNDEFINED_CHANNEL
};

/* Used to describe the type of handle that is being used to open the
   driver from userspace */
typedef enum
{
    DDC_DEV_HANDLE_NONE = 0,
    DDC_DEV_HANDLE_MIL_STD_1553 = 1 << 0,
    DDC_DEV_HANDLE_ARINC_429 = 1 << 1,
    DDC_DEV_HANDLE_IO = 1 << 2
} DDC_DEV_HANDLE_TYPE;


/* ========================================================================== */
/* ========================================================================== */

/* Software Version Information struct */
typedef struct _SWVERSIONINFO
{
    U32BIT dwRtlVersion;
    U32BIT dwCoreVersion;
    U8BIT szRtlVersion[32];
    U32BIT dwReserved1;
    U32BIT dwReserved2;
    U32BIT dwReserved3;
    U32BIT dwReserved4;

} SWVERSIONINFO, *PSWVERSIONINFO;

/* Hardware Version Information struct */
typedef struct DDC_PACKED _HWVERSIONINFO
{
    U32BIT dwFwVersion;
    U32BIT dwHdlVersion;
    U32BIT dwDriverVersion;
    U32BIT dwSerialNumber;
    U32BIT dwFamilyNumber;
    U32BIT dwModelNumber;
    U8BIT szModelName[32];
    U8BIT szDriverVersion[16];
    U32BIT dwHdlNumber;
    U32BIT dwCapabilities;
    U32BIT dwCapabilities2;
    U32BIT dwReserved4;
    U8BIT szReserved[32];

} HWVERSIONINFO, *PHWVERSIONINFO;

/* Defines used by dwCapabilities */
#define HWVER_CAPABILITY_MF                     0x00000001  /* 0   Multi-Function Capable   */
#define HWVER_CAPABILITY_VAR_VOLT_OUT           0x00000002  /* 1   Variable Voltage Output  */
#define HWVER_CAPABILITY_COUPLING               0x00000004  /* 2   Coupling                 */
#define HWVER_CAPABILITY_TERMINATION            0x00000008  /* 3   Termination              */
#define HWVER_CAPABILITY_REPLAY                 0x00000010  /* 4   Replay                   */
#define HWVER_CAPABILITY_MTI                    0x00000020  /* 5   MTI                      */
#define HWVER_CAPABILITY_MTIE_MEC               0x00000040  /* 6   MTIe (MEC)               */
#define HWVER_CAPABILITY_MTIR_AES               0x00000080  /* 7   MTIr (AES)               */
#define HWVER_CAPABILITY_EXTERNAL_CLOCK_IN      0x00000100  /* 8   External Clock In        */
#define HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT     0x00000200  /* 9   External Clock Out       */
#define HWVER_CAPABILITY_IRIG_IN_ANALOG         0x00000400  /* 10  Analog IRIG Rx (In)      */
#define HWVER_CAPABILITY_IRIG_IN_DIGITAL        0x00000800  /* 11  Digital IRIG Rx (In)     */
#define HWVER_CAPABILITY_IRIG_OUT_DIGITAL       0x00001000  /* 12  Digital IRIG Tx (Out)    */
#define HWVER_CAPABILITY_IRIG_RANGE             0x00002000  /* 13  IRIG Range               */
#define HWVER_CAPABILITY_TX_INHIBIT_BC_DISABLE  0x00004000  /* 14  Tx Inhibit / BC Disable  */
#define HWVER_CAPABILITY_CAN                    0x00008000  /* 15  CAN                      */
#define HWVER_CAPABILITY_PPS                    0x00010000  /* 16  PPS                      */
#define HWVER_CAPABILITY_PROG_ARINC_429         0x00020000  /* 17  Programmable ARINC 429   */
#define HWVER_CAPABILITY_PROG_ARINC_717         0x00040000  /* 18  Programmable ARINC 429   */
#define HWVER_CAPABILITY_PROG_SERIAL            0x00080000  /* 19  Programmable ARINC 429   */
#define HWVER_CAPABILITY_MSI                    0x00100000  /* 20  MSI                      */
#define HWVER_CAPABILITY_BC_DISABLE             0x00200000  /* 22  BC Disable               */
#define HWVER_CAPABILITY_BC_EXT_TRIGGER         0x00400000  /* 21  BC External Trigger      */
#define HWVER_CAPABILITY_MRT_DISABLE            0x00800000  /* 23  MRT Disable              */
#define HWVER_CAPABILITY_RT_AUTO_BOOT           0x01000000  /* 24  RT Auto Boot             */
#define HWVER_CAPABILITY_TX_INHIBIT             0x02000000  /* 25  TX Inhibit               */
#define HWVER_CAPABILITY_429_RELAY              0x04000000  /* 26  429 Relays               */
#define HWVER_CAPABILITY_PROG_VAR_SPEED_429     0x08000000  /* 27  Variable Speed ARINC 429 */
#define HWVER_CAPABILITY_429_TESTER_OPTIONS     0x10000000  /* 28  ARINC 429 Tester Options */
#define HWVER_CAPABILITY_VAR_VOLT_OUT_429       0x20000000  /* 29  429 Variable Volt Output */
#define HWVER_CAPABILITY_MPRM                   0x40000000  /* 30  M-Prime Device           */
#define HWVER_CAPABILITY_QPRM                   0x80000000  /* 31  Q-Prime Device           */

/* Capability Defines to be used with the HWVERSIONINFO structure member dwCapabilities2  */
#define HWVER_CAPABILITY2_ARINC_429_TX_SCHEDULE_EXTEND                  0x00000001  /* 0   Gen-2 TX_SCHEDULE_EXTEND Capable     */
#define HWVER_CAPABILITY2_ARINC_429_HI_PRIORITY_FIFO_FREE_COUNTER       0x00000002  /* 1   Gen-2 TX_FIFO_FREE_COUNTER Capable   */
#define HWVER_CAPABILITY2_IRIG_B_ENHANCED                               0x00000004  /* 2   Enhanced IRIG-B Format Capable       */
#define HWVER_CAPABILITY2_AIO_INTERRUPT                                 0x00000008  /* 3   AIO Interrupt Capable                */
#define HWVER_CAPABILITY2_DIO_INTERRUPT                                 0x00000010  /* 4   DIO Interrupt Capable                */
#define HWVER_CAPABILITY2_ARINC_429_REPEATER                            0x00000020  /* 5   Repeater Capable                     */
#define HWVER_CAPABILITY2_ARINC_429_REPEATER_DATA_POLLUTION             0x00000040  /* 6   Repeater Data Pollution Capable      */
#define HWVER_CAPABILITY2_SERIAL_HBUF                                   0x00000080  /* 7   Serial Host Buffer                   */
#define HWVER_CAPABILITY2_SERIAL_ENHANCED_FIFO              0x00000100  /* 8   Serial Enhanced FIFO Support  (256 Byte FIFO)  */

/* Generic device ID and location info */
typedef struct DDC_PACKED _DEV_INFO
{
    U32BIT dwVendorID;
    U32BIT dwDeviceID;
    U32BIT dwBusHub;
    U32BIT dwSlotPort;
    U32BIT dwChanMemBA;
    U32BIT dwChanMemLen;
    U32BIT u32Capability;

    HWVERSIONINFO sHwVersionInfo;

} DEV_INFO;

/* Channel Count struct */
typedef struct DDC_PACKED _CHANCOUNT
{
    U8BIT bTx;
    U8BIT bRx;
    U8BIT bGroup;
    U8BIT bDiscrete;
    U8BIT bAvionic;
    U8BIT bBoardModel;
    U8BIT b1553;
    U8BIT a429Prog;
    U8BIT bUart;
    U8BIT RS232;
    U8BIT RS485;
    U8BIT CanBus;
    U8BIT a717Rx;
    U8BIT a717Tx;
    U8BIT a717Prog;
    U8BIT bDioTt;

} CHANCOUNT_t, *CHANCOUNT_p;


typedef struct DDC_PACKED _ENHANCED_CAPABILITY_INFO
{
     U8BIT modelNumber;
     U8BIT channelCount1553;
     U8BIT channelCount429Rx;
     U8BIT channelCount429Tx;

     U8BIT channelCount429Prog;
     U8BIT channelCountAIO;
     U8BIT channelCountDIO;
     U8BIT channelCountRS232;

     U8BIT channelCountRS485;
     U8BIT channelCountUart;
     U8BIT channelCountCanBus;
     U8BIT channelCount717Rx;

     U8BIT channelCount717Tx;
     U8BIT channelCount717Prog;
     U8BIT channelCountSyncRS232;
     U8BIT u8Reserved1;

     U8BIT u8Reserved2;
     U8BIT u8Reserved3;
     U8BIT u8Reserved4;
     U8BIT u8Reserved5;

     U8BIT u8Reserved6;
     U8BIT u8Reserved7;
     U8BIT u8Reserved8;
     U8BIT u8deviceNumber;
} ENHANCED_CAPABILITY_INFO, *PENHANCED_CAPABILITY_INFO;


typedef struct DDC_PACKED _DEV_FEATURE_STATUS_INFO
{
    BOOLEAN bBcDisable;
    BOOLEAN bBcExtTrigger;
    BOOLEAN bMrtDisable;
    BOOLEAN bRtAutoBoot;

    BOOLEAN bTxInhibit;
    BOOLEAN bReserved;
    U16BIT  u16RtAddr;

} DEV_FEATURE_STATUS_INFO, *PDEV_FEATURE_STATUS_INFO;


typedef struct DDC_PACKED _FLASH_INFO
{
    U32BIT StartAddress;
    U32BIT SectorCount;
    U32BIT MinSectorSize;

} FLASH_INFO, *PFLASH_INFO;


/* IRQ Status Types */
typedef struct _DDC_IRQ_STATUS_TYPE
{
    U32BIT u32Type;
    U32BIT u32Status;
    U32BIT u32Status2;

} DDC_IRQ_STATUS_TYPE;

typedef struct DDC_PACKED _DEV_HANDLE_INFORMATION
{
    DDC_DEV_HANDLE_TYPE handleType;
    U32BIT Channel;

} DEV_HANDLE_INFORMATION, *PDEV_HANDLE_INFORMATION;


#define DDC_IRQ_TYPE_1553       0
#define DDC_IRQ_TYPE_AIO        1
#define DDC_IRQ_TYPE_DIO        2
#define DDC_IRQ_TYPE_429        3

/* Windows needs multiple driver Handles to support blocking IRQ access to driver */
#define DDC_APP_DRV_HANDLE                     0
#define DDC_IRQ_DRV_HANDLE                      1
#define DDC_MTIDATA_DRV_HANDLE             2
#define DDC_MTITIME_DRV_HANDLE              3
#define DDC_REPLAY_DRV_HANDLE              4

/*-----------------------------------------------------------*/
/* AIO Interrupt Defines */
/*-----------------------------------------------------------*/
#define ACEX_AIO_INTERRUPT_CONFIGURE__ENABLE    0
#define ACEX_AIO_INTERRUPT_CONFIGURE__DISABLE   1
#define ACEX_AIO_INTERRUPT_CONFIGURE__SET       2
#define ACEX_AIO_INTERRUPT_CONFIGURE__CLEAR     3
#define ACEX_AIO_INTERRUPT_CONFIGURE__GET       4

#define DD429_AIO_INTERRUPT_CONFIGURE__ENABLE   5
#define DD429_AIO_INTERRUPT_CONFIGURE__DISABLE  6
#define DD429_AIO_INTERRUPT_CONFIGURE__SET      7
#define DD429_AIO_INTERRUPT_CONFIGURE__CLEAR    8
#define DD429_AIO_INTERRUPT_CONFIGURE__GET      9

/*void (_DECL *isrr)(S16BIT s16DevNum, U32BIT u32IrqStatus);*/

typedef struct _ACEX_IO_CONFIGURE_TYPE
{
    U32BIT u32RisingEdge;
    U32BIT u32FallingEdge;
    void (_DECL *ioExternalIsr)(S16BIT s16DevNum, U32BIT u32IrqStatus);

} ACEX_IO_CONFIGURE_TYPE;

/* ========================================================================== */
/* ========================================================================== */

/*
    DDC Memory Copy
    pd  - dest byte pointer
    ps  - src byte pointer
    len - byte length
*/
#define DDC_MEMCPY(pDest, pSource, len) \
    {                                                           \
        U32BIT i;                                               \
        for (i = 0; i < (len); i++)                             \
        {                                                       \
            *((char*)(pDest) + i) = *((char*)(pSource) + i);    \
        }                                                       \
    }

/* ========================================================================== */
/* Byte Swapping Macros                                                       */
/* ========================================================================== */

#if DDC_PPC

#define DDC_BYTE_ORDER_L(i)             \
       ((((i) & 0xFF000000) >> 24) |    \
        (((i) & 0x00FF0000) >> 8) |     \
        (((i) & 0x0000FF00) << 8) |     \
        (((i) & 0x000000FF) << 24))

#define DDC_BYTE_ORDER_S(i) ( (U16BIT)((((i) & 0xFF00) >> 8) | (((i) & 0x00FF) << 8)))

#define DDC_WORD_ORDER_L(i)             \
    ((((i) & 0x0000FFFF) << 16) |       \
     (((i) & 0xFFFF0000) >> 16))

#else /* DDC_PPC not defined */

#define DDC_BYTE_ORDER_L(i) (i)
#define DDC_WORD_ORDER_L(i) (i)
#define DDC_BYTE_ORDER_S(i) (i)

#endif /* DDC_PPC */


/* ========================================================================== */
/* acexConfiguration Items                                                    */
/* ========================================================================== */
#define TASK__LIBRARY_INTERRUPT                 0
#define TASK__DRIVER_1553_WORKER                1
#define TASK__DRIVER_429_WORKER                 2

typedef struct _TASK_PRIORITY_CONFIG
{
    U32BIT u32Type;         /* Type value identifies Task to configure */
    U32BIT u32Priority;     /* Priority value to be set */

} TASK_PRIORITY_CONFIG;


#define MAX_NUM_DEVICES                     32      /* support up to 32 cards */
#define MAX_NUM_1553_DEVICES                32
#define MAX_NUM_429_DEVICES                 8
#define MAX_NUM_FPGAS                       8
#define MAX_NUM_1553_CHANNELS               8


#endif /* _DDC_TYPES_ */
