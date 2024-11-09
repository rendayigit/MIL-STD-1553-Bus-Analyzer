/*******************************************************************************
 * FILE: ddc_udl_private.h
 *
 * DESCRIPTION:
 *
 *  This file contains driver only definitions.
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

#ifndef _DDC_UDL_PRIVATE_H_
#define _DDC_UDL_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "os/include/ddc_os_types_private.h"
#include "include/ddc_types.h"
#include "driver_sdk/ddc_udl_um_private.h"
#include "driver_sdk/ddc_udl_dma_private.h"
#include "driver_sdk/ddc_udl_hardware_private.h"
#include "driver_sdk/ddc_udl_queueop_private.h"
#include "core/irig/ddc_udl_irigb_private.h"
#include "core/can/ddc_udl_can_private.h"
#include "core/serial_io/ddc_udl_serial_io_private.h"
#include "core/arinc429/ddc_udl_arinc429.h"
#include "core/arinc429/ddc_udl_arinc429_private.h"
#include "core/arinc717/ddc_udl_arinc717_private.h"
#include "core/dio/ddc_udl_diott_private.h"


/* ========================================================================== */
/*                            TYPES AND DEFINITIONS                           */
/* ========================================================================== */



#define DDC_UDL_PROCESS_1553_CHANNEL_01     0
#define DDC_UDL_PROCESS_1553_CHANNEL_02     1
#define DDC_UDL_PROCESS_1553_CHANNEL_03     2
#define DDC_UDL_PROCESS_1553_CHANNEL_04     3
#define DDC_UDL_PROCESS_1553_CHANNEL_05     4
#define DDC_UDL_PROCESS_1553_CHANNEL_06     5
#define DDC_UDL_PROCESS_1553_CHANNEL_07     6
#define DDC_UDL_PROCESS_1553_CHANNEL_08     7
#define DDC_UDL_PROCESS_ARINC_SERIAL        8
#define DDC_UDL_PROCESS_DISCRETE            9
#define DDC_UDL_PROCESS_MAX                 10

/* ==================================================== */
/* ==================================================== */

#define ACEX_16_BIT_ACCESS              0
#define ACEX_32_BIT_ACCESS              -1

#define IOCTL_16_BIT_ACCESS             0
#define IOCTL_32_BIT_ACCESS             1


#if DDC_PPC
#   define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -2
#else
#   define ACEX_32_BIT_ACCESS_16_BIT_HW_MEM    -1
#endif /* if defined (PPC) || defined(_powerpc_) */


/* ==================================================== */
/* REGISTERS                                            */
/* ==================================================== */

#define DDC_REG_READ(pDeviceContext, address, pValue)       \
    ddcUdlOsBusRegRead(pDeviceContext,                      \
        address, /* in 32-bit */                            \
        pValue)

/* address & count in 32-bit */
#define DDC_BLK_REG_READ(pDeviceContext, addr, pVal, cnt)   \
    ddcUdlOsBusRegReadBlk(pDeviceContext,                   \
        addr,                                               \
        pVal,                                               \
        cnt)

/* address in 32-bit */
#define DDC_REG_WRITE(pDeviceContext, addr, pVal)           \
    ddcUdlOsBusRegWrite(pDeviceContext,                     \
        addr,                                               \
        pVal)

/* write multiple 32-bit registers - registers must be contiguous, address & count in 32-bit */
#define DDC_BLK_REG_WRITE(pDeviceContext, addr, pVal, cnt)  \
    ddcUdlOsBusRegWriteBlk(pDeviceContext,                  \
        addr,                                               \
        pVal,                                               \
        cnt)

/* read a 32 bit value from PLX space */
#define DDC_PLX_READ(pDeviceContext, addr, pVal)            \
    ddcUdlOsBusReadPlx(pDeviceContext,                      \
       addr,                                                \
       pVal)

/* write 32 bit value to PLX space */
#define DDC_PLX_WRITE(pDeviceContext, addr, pVal)           \
    ddcUdlOsBusWritePlx(pDeviceContext,                     \
        addr,                                               \
        pVal)


#define REG_TYPE_ABSOLUTE_ADDR          0
#define REG_TYPE_BD_OFFSET              1
#define REG_TYPE_BC_OFFSET              2
#define REG_TYPE_MRT_OFFSET             3
#define REG_TYPE_MT_OFFSET              4
#define REG_TYPE_1553_OFFSET            5
#define REG_TYPE_EI_OFFSET              6
#define REG_TYPE_REPLAY_OFFSET          7
#define REG_TYPE_TRG_OFFSET             8


/* ==================================================== */
/*  MEMORY                                              */
/* ==================================================== */

/*
   memaddr - memory address to write to
   pValAddr- pointer to U32BIT var to write from
   type    - 0 = wait to complete, ACEX_USB_INT_ON_COMPLET = usb interrupt on completion
   cnt     - represents number of 32-bit words
   mode    - 32 bit or 32 bit swapped access */

#define DDC_MEM_WRITE(pDeviceContext, addr, pval, mode)             \
    ddcUdlOsBusMemWrite(pDeviceContext,                             \
        addr,                                                       \
        pval,                                                       \
        mode)

/* <UDL19> */
#define DDC_BLK_MEM_WRITE(pDeviceContext, addr, pval, cnt, mode)    \
    ddcUdlOsBusMemWriteBlk(pDeviceContext,                          \
        addr,                                                       \
        pval,                                                       \
        cnt,                                                        \
        mode)

#define DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, addr, pval, mode, cnt)   \
{ \
     U32BIT *pu32WriteData = pval; \
	 int i = 0; \
    \
    if (pDeviceContext->b429MemSwByteSwap) \
    { \
		while(i < cnt) \
		{ \
			pu32WriteData[i] = DDC_BYTE_ORDER_L(pu32WriteData[i]); \
			i++; \
		} \
    } \
	ddcUdlOsBusMemWriteBlk(pDeviceContext,                              \
        addr,                                                           \
        pu32WriteData,                                                  \
        cnt,                                                            \
        mode); \
}

#define DDC_16BIT_BLK_MEM_WRITE(pDeviceContext, addr, pval, cnt)    \
    ddcUdlOsBusMemWriteBlk16(pDeviceContext,                        \
        addr,                                                       \
        pval,                                                       \
        cnt)

#define DDC_MEM_READ(pDeviceContext, addr, pval, mode)              \
    ddcUdlOsBusMemRead(pDeviceContext,                              \
        addr,                                                       \
        pval,                                                       \
        mode)

#define DDC_BLK_MEM_READ(pDeviceContext, addr, pval, cnt, mode)     \
    ddcUdlOsBusMemReadBlk(pDeviceContext,                           \
        addr,                                                       \
        pval,                                                       \
        cnt,                                                        \
        mode)

#define DDC_16BIT_BLK_MEM_READ(pDeviceContext, addr, pval, cnt)     \
    ddcUdlOsBusMemReadBlk16(pDeviceContext,                         \
        addr,                                                       \
        pval,                                                       \
        cnt)

#define DDC_BLK_MEM_READ_IOCTL(pDeviceContext, memaddr, pValAddr, cnt, request) \
{ \
    U32BIT *pu32WriteData = pValAddr; \
	int i = 0; \
	\
	ddcUdlOsBusMemReadBlk(pDeviceContext, memaddr, pValAddr, (cnt/4), 0); \
	if (pDeviceContext->b429MemSwByteSwap) \
	{ \
		 \
		while(i < cnt) \
		{ \
			pu32WriteData[i] = DDC_BYTE_ORDER_L(pu32WriteData[i]); \
			i++; \
		} \
	} \
}

/* ==================================================== */
/*                 COUPLING/TERMINATION                 */
/* ==================================================== */

/* todo - fix macros for mutiple FPGA's */
#define ACEX_CLEAR_CH_COUPLING_TERM(ch)      ~(0x0000003F << (ch * 6))

#define ACEX_GET_CH_COUPLING(ch, data)       ((data >> (ch * 6)) & 0x00000001)
#define ACEX_GET_CH_TERMINATION(ch, data)    ((data >> ((ch * 6) + 1)) & 0x00000003)

#define ACEX_SET_CH_COUPLING(ch, data)       (((data & 0x00000001) << (ch * 6)) | ((data & 0x00000001) << ((ch * 6) + 3)))
#define ACEX_SET_CH_TERMINATION(ch, data)    (((data & 0x00000003) << ((ch * 6) + 1)) | ((data & 0x00000003) << ((ch * 6) + 4)))

/* ==================================================== */
/*                       AMPLITUDE                      */
/* ==================================================== */
/*
    Amplitude BITs
    27:18    1553 Channel_2 Transmit Amplitude
    11:02    1553 Channel_1 Transmit Amplitude
*/
#define ACEX_CLEAR_CH_AMPLITUDE(ch)          ~(0x0000003FF << ((ch * 16) + 2))
#define ACEX_GET_CH_AMPLITUDE(ch, data)      (((data >> ((ch * 16) + 2))) & 0x000003FF)
#define ACEX_SET_CH_AMPLITUDE(ch, data)      (((data & 0x000003FF) << ((ch * 16) + 2)))


/* ==================================================== */
/*                    RT  RT  RT  RT                    */
/* ==================================================== */

#define NUM_RT_SUB_ADDRESSES                    32


/* ==================================================== */
/*                    MT  MT  MT  MT                    */
/* ==================================================== */
#define MTI_MAX_NUM_BUF             32 /*16*/                                            /* The number of MTI pkts in MTIDataList Buffer     */

#define GET_ACEX_MTI_CH10_PKT_MSG_DATA_POINTER(pPkt) \
    (((U16BIT *)pPkt) + (U16BIT)((sizeof(ACEX_MTI_CH10_DATA_PKT) - sizeof(U16BIT)) >> 1))

#define GET_ACEX_MTI_CH10_PKT_TIME_CNTR_POINTER(pPkt) \
    (((U16BIT *)pPkt) + (U16BIT)(((4 * sizeof(U16BIT)) + (2 * sizeof(U32BIT))) >> 1))

#define GET_ACEX_MTI_CH10_TIME_PKT_TIME_CNTR_POINTER(pPkt) \
    (((U16BIT *)pPkt) + (U16BIT)(((4 * sizeof(U16BIT)) + (2 * sizeof(U32BIT))) >> 1))


/* ==================================================== */
/* ARINC 429                                            */
/* ==================================================== */

#define DD429_MAX_NUM_RX                                36
#define DD429_MAX_NUM_TX                                36

/* ==================================================== */
/* ==================================================== */

typedef enum _MTI_STATE
{
    ACEX_MTI_RESET = 0,
    ACEX_MTI_READY,
    ACEX_MTI_RUN

} MTI_STATE;


enum ACEX_DEVICE_STATE
{
    ACEX_CLOSED = 0,
    ACEX_OPEN,
    ACEX_FLASH_ERASE,
    ACEX_FLASH_WRITE
};


typedef enum _DDC_ENDIANNESS_TYPE
{
    DDC_ENDIANNESS_DO_HW_SWAP = 0,
    DDC_ENDIANNESS_DO_SW_SWAP = 1,
    DDC_ENDIANNESS_MAX = 2

} DDC_ENDIANNESS_TYPE;

/* IRQ Status Queue */
#define IRQ_STATUS_QUEUE_SIZE                       128
#define REPLAY_IRQ_STATUS_QUEUE_SIZE                128
#define ARINC_IRQ_STATUS_QUEUE_SIZE                 128


/* ========================================================================== */
/* ========================================================================== */
/* Device Context Structure                                                   */
/* ========================================================================== */
/* ========================================================================== */

struct _DDC_UDL_DEVICE_CONTEXT
{
    /* Board Information */
    struct _ACEXPCI_BOARD_TYPE *pBoardInfo[MAX_NUM_FPGAS];      /* DO NOT ADD ANY ITEMS ABOVE HERE */
    
    U16BIT sBoardInstanceIndex[UM_MAX_NUM_DEVICES + 1];
    U8BIT u8BoardInstanceCount;
    U8BIT u8IrigbInstanceCount;

    U8BIT u8DeviceInstanceCount;                                /* instance count of device usage */

    enum ACEX_DEVICE_STATE eState;
    DDC_ENDIANNESS_TYPE eEndiannessMode;

    struct DDC_OS_DEV_INFO ddcOsDevInfo;

    /* PCI Device Info */
    U16BIT u16DriverType;
    U16BIT u16DeviceID;
    BOOLEAN bBoardLoadCompleted;

    U8BIT u8Channel1553Open[MAX_NUM_1553_CHANNELS]; /* whether the 1553 Channel file is opened */
    U8BIT u8ArincSerialOpen;                        /* whether the ARINC Serial file is opened */
    U8BIT u8DiscreteOpen;                           /* whether the Discrete file is opened     */
    
    HWVERSIONINFO sHwVersionInfo;

#ifdef DDC_INT_TASK    
    /* Interrupt Task */
    Semaphore intSemaphore;                               /* notifies the thread that the an IOCTL needs to be processed */
    CALL intSemaphoreCall; 
    U8BIT requestMutex;
    Task intTask;
#endif
    /* UM-ROM */
    struct _UM_INFO_TYPE *pUmInfo;

    U32BIT u32DeviceMask;
    U32BIT u32ComponentMask;

    /* QPRM and CPLD binding information */
    void *pDeviceContextCPLD;

    BOOLEAN bDeviceCloseBusy[DDC_UDL_PROCESS_MAX];

    DDC_ISR_LOCK_TYPE semDeviceOpenEventCond;       /* Device open spinlock */
    DDC_ISR_FLAG_TYPE semDeviceOpenEventCondFlag;   /* Device open spinlock flag */

    /* ==================================================== */
    /* Channel Counts & Capabilities                        */
    /* ==================================================== */
    U8BIT u8Num1553Channels;
    U8BIT u8NumProg429RxTx;
    U8BIT u8NumDed429Tx;
    U8BIT u8NumDed429Rx;
    U8BIT u8NumProg717;
    U8BIT u8NumDed717Tx;
    U8BIT u8NumDed717Rx;
    U8BIT u8NumDiscreteIO;
    U8BIT u8NumAvionicIO;
    U8BIT u8NumRS232;
    U8BIT u8NumRS485;
    U8BIT u8NumUart;
    U8BIT u8NumDioTt;
    U8BIT u8NumCanBus;

    ENHANCED_CAPABILITY_INFO sEnhancedCapabilityInfo;

    U32BIT u32AvionicIOMask;

    U32BIT u32BdIntStatus;                                  /* board level interrupt status */


    /* ==================================================== */
    /* DMA                                                  */
    /* ==================================================== */
    struct _DDC_DMA sDMA;


    /* ==================================================== */
    /* 1553                                                 */
    /* ==================================================== */
    struct _ACEX_1553_CHANNEL_TYPE *pChannel1553[MAX_NUM_1553_CHANNELS];


    /* ==================================================== */
    /* 1553 - MT                                            */
    /* ==================================================== */

    U16BIT u16MtiChannelCount;


    /* MTI CH10 Data Packet */

    U32BIT u32MtiGlobalTimeDataHi;
    U32BIT u32MtiGlobalTimeDataLo;                                  /* the global time data retrieved from IRIGB_LATCHED_1SEC_TIME_STAMP reg when irig 1sec interrupt occurs */


    /* ==================================================== */
    /* 1553 - RT                                            */
    /* ==================================================== */

    DDC_DMA_ADDR RtDmaAddr;
    size_t RtDmaSize;

    U8BIT u8RtDmaBusy;
    U8BIT *pu8RtDmaTarget;


    /* ==================================================== */
    /* IRIG                                                 */
    /* ==================================================== */
    U16BIT u16IrigPacketChannelId[MAX_NUM_1553_CHANNELS];

    struct _ACEX_1553_IRIGB_TYPE sIrigB_RX[MAX_NUM_1553_CHANNELS];
    struct _ACEX_1553_IRIGB_TYPE sIrigB_TX[MAX_NUM_1553_CHANNELS];

    U8BIT u8NumOfIrigbInstances;


    /* ==================================================== */
    /* ARINC                                                */
    /* ==================================================== */

    DDC_ISR_LOCK_TYPE semArincIrqEventCond;
    DDC_ISR_FLAG_TYPE semArincEventCondFlag;

    ARINC_INTERRUPT sArincIntStatus[ARINC_IRQ_STATUS_QUEUE_SIZE];               /* Interrupt status array  */

    U32BIT u32ArincIntQHead;
    U32BIT u32ArincIntQTail;
    U16BIT u32ArincIntQLen;
    
    DDC_CALLBACK waitqueueArincBlockOnIrqCallback;
    DDC_EVENT waitqueueArincBlockOnIrqEvent;
    
    DDC_EVENT waitqueueArincBlockOnIrqReadyEvent;
    
    U16BIT u16ArincBlockOnIrqReadyEventCond;

    BOOLEAN bArincSerialIsrEnabled;
    
    BOOLEAN b429MemSwByteSwap;

    U32BIT u32IrqAioInterruptMask;  /* AIO Masked IRQ conditions */


    /* ==================================================== */
    /* ARINC 429                                            */
    /* ==================================================== */

    U8BIT u8Arinc429BitFormat;

    U32BIT *p429TxMemory;

    U32BIT u32RepeaterActiveCh1to32;
    U32BIT u32RepeaterActiveCh32to64;

    /* ==================================================== */
    /* ARINC 429 VOLTAGE MONITORING                         */
    /* ==================================================== */

    DDC_ISR_LOCK_TYPE semArinc429VoltageMonitoringEventCond;
    DDC_ISR_FLAG_TYPE semArinc429VoltageMonitoringEventCondFlag;

    U16BIT u16Arinc429VoltageMonitoringEventCond;                               /* DMA complete condition */
    
    DDC_CALLBACK waitqueueArinc429VoltageMonitoringCallback;
    DDC_EVENT waitqueueArinc429VoltageMonitoringEvent;                          /* VoltageMonitoring wait queue: one for the board  */

    DDC_DMA_ADDR Arinc429VoltageMonDMA_Addr;
    size_t Arinc429VoltageMonDMA_Size;
    U8BIT *Arinc429VoltageMonDMATarget;


    /* ==================================================== */
    /* ARINC 429 RX                                         */
    /* ==================================================== */

    struct _ACEX_429_RX_TYPE sArinc429RxGlobal;
    struct _ACEX_429_RX_TYPE *pRxChnl429[MAX_NUM_429_CHANNELS];

    DDC_ISR_LOCK_TYPE semArinc429RxFifoEventCond;
    DDC_ISR_FLAG_TYPE semArinc429RxFifoEventCondFlag;

    ACEX_429_RX_HBUF_TYPE s429RxHBuf;

    DDC_DMA_ADDR ARINC429RxDMA_Addr;
    size_t ARINC429RxDMA_Size;
    U8BIT *ARINC429RxDMATarget;

    /* ==================================================== */
    /* ARINC 429 TX                                         */
    /* ==================================================== */

    U32BIT u32Arinc429TxState;
    DDC_ISR_LOCK_TYPE semArinc429TxState;
    DDC_ISR_FLAG_TYPE semArinc429TxStateFlag;

    struct _ACEX_429_TX_TYPE sArinc429TxGlobal;
    struct _ACEX_429_TX_TYPE *pTxChnl429[MAX_NUM_429_CHANNELS];

    /*ruct _ARINC_429_TX_SCHEDULER_DATA sArinc429TxScheduler;*/

    DDC_ISR_LOCK_TYPE semARINC429SetTxFrameControlEventCond;
    DDC_ISR_FLAG_TYPE semARINC429SetTxFrameControlEventCondFlag;

    DDC_DMA_ADDR ARINC429TxDMA_Addr;
    size_t ARINC429TxDMA_Size;
    U8BIT *ARINC429TxDMATarget;

    ACEX_429_TX_TYPE    sTxChnlExtend429[MAX_NUM_429_CHANNELS]; 
    BOOLEAN             bExtendedScheduler;
    
    union
    {
        ARINC_429_TX_SCHEDULER_DATA   sTxScheduler; /* legacy scheduler */
        U16BIT              u16TxSchedulerOffset[MAX_NUM_429_CHANNELS][DD429_TX_SCHED_MESSAGE_MEM_DEPTH]; /* 1024 scheduler, we need to keep a copy of offset since hardware is actively uses this memory address */
    } U;
    /*U32BIT              u32DataMatchMirror[MAX_NUM_429_CHANNELS][DD429_DATA_MATCH_TABLE_LEN_MIRROR];*/ /* a shadow copy of the data match table used by filters */

    /* ==================================================== */
    /* ARINC 717                                            */
    /* ==================================================== */

    ARINC_717_PROG_TYPE sArincGlobal717;
    ARINC_717_PROG_TYPE sChannelArinc717[MAX_NUM_717_PROG_CHANNELS];
    ARINC_717_STATE eArinc717ProgState[MAX_NUM_717_PROG_CHANNELS];
    ARINC_717_PROG_CH_STATE eArinc717ProgChState[MAX_NUM_717_PROG_CHANNELS];
    ARINC_717_PROG_MEM sArinc717ProgChMemConfig[MAX_NUM_717_PROG_CHANNELS];
    ARINC_717_INTERRUPT sArinc717IntStatus;


    U16BIT u16ARINC717RxFrameEventCond[MAX_NUM_717_PROG_CHANNELS];                  /* DMA complete condition */
    
    DDC_CALLBACK waitqueueARINC717RxFrameCallback[MAX_NUM_717_PROG_CHANNELS];
    DDC_EVENT waitqueueARINC717RxFrameEvent[MAX_NUM_717_PROG_CHANNELS];             /* ARINC717RxFrame wait queue  */

    DDC_DMA_ADDR ARINC717RxDMA_Addr;
    size_t ARINC717RxDMA_Size;
    U8BIT *ARINC717RxDMATarget;

    U16BIT u16ARINC717TxFrameEventCond[MAX_NUM_717_PROG_CHANNELS];                  /* DMA complete condition */
    
    DDC_CALLBACK waitqueueARINC717TxFrameCallback[MAX_NUM_717_PROG_CHANNELS];
    DDC_EVENT waitqueueARINC717TxFrameEvent[MAX_NUM_717_PROG_CHANNELS];             /* ARINC717TxFrame wait queue  */

    DDC_DMA_ADDR ARINC717TxDMA_Addr;
    size_t ARINC717TxDMA_Size;
    U8BIT *ARINC717TxDMATarget;


    /* ==================================================== */
    /* Discrete I/O Time Tag                                */
    /* ==================================================== */
    DIO_TT_TYPE sDioTt;


    /* ==================================================== */
    /* UART                                                 */
    /* ==================================================== */

    ACEX_429_UART_TYPE sMioUart429;

    ACEX_429_UART_TYPE sMioCastUartGlobal429;
    ACEX_429_UART_TYPE sMioCastUart429[NUM_CAST_SIO_CHANNELS];
    ACEX_429_UART_TYPE sMioCastUartAsync429[NUM_CAST_SIO_CHANNELS];
    ACEX_429_UART_TYPE sMioCastUartHDLC429[NUM_CAST_SIO_CHANNELS];
    ACEX_429_UART_TYPE sMioCastUartSDLC429[NUM_CAST_SIO_CHANNELS];

    /* ==================================================== */
    /* CAN BUS                                              */
    /* ==================================================== */

    struct _CAN_BUS_TYPE sCanBus;
    struct _CAN_BUS_CONFIG_STATE sCanBusConfig[MAX_NUM_CAN_CHANNELS];
    CAN_BUS_RUN_STATE eCanBusState[MAX_NUM_CAN_CHANNELS];
    CAN_RX_HBUF_TYPE sCanRxHBuf;
    CAN_BUS_INT_STATUS u32CanBusIntStatus; /*[MAX_NUM_CAN_CHANNELS]; */
    
    U32BIT u32CanHbufPercentFull[MAX_NUM_CAN_CHANNELS];
    U32BIT u32CanHbufPercentHigh[MAX_NUM_CAN_CHANNELS];
    U32BIT u32CanHbufOverflowCount[MAX_NUM_CAN_CHANNELS];

    /* 
	 1st dword is channel interrupted on
	 2nd dword is total bytes added to hbuf 
     3rd dword is MIO_IIR register bits 0 - 3. 
    */
    U32BIT u32CastIO_bdIntStatus[3];    

    CAST_SERIAL_IO_CHAN_MATRIX  castSerialIOChanMatrix[NUM_CAST_SIO_CHANNELS ];
    CAST_SERIAL_IO_RX_HBUF_TYPE sSerialIORxHBuf;
};


/* ========================================================================== */
/* ========================================================================== */

/* driver types used with function overrides */
#define ACEX_PCI_DRIVER                             0   /* for ACEX PCI devices except of other types */
#define ACEX_QPRM_DRIVER                            1   /* for Q-prime based devices */
#define ACEX_PCIE_DRIVER                            2   /* for PCIe devices */
#define ACEX_IO_DRIVER                              3   /* for IO only device */
#define ACEX_DD429_PCIE_DRIVER                      4   /* for 36 channel DD-40000K PCIe ARINC-429 Device */
#define ACEX_DD429_PCI_DRIVER                       5   /* for 36 channel DD-40100F PMC  ARINC-429 Device */
#define NUM_DRIVER_TYPES                            6


#define ACEX_MAX_TFR_BUFSIZE                        16384


/* RT Defines */
#define ACE_MSGSIZE_RT                              36


#define MT_LKUP_TABLE_MEMORY_BYTE_SIZE              0x100           /* memory size, in bytes, to support mt config/mgmt */
#define MT_LKUP_TABLE_MEMORY_BASE_ADDRESS           0x00002000      /* 32 bit base address in memory space */


/* ARINC 429 TX Scheduler Memory */
#define DD429_TX_SCHED_MSG_DATA_OFFSET              0x00000000
#define DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET          0x00000400
#define DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET         0x00000600
#define DD429_TX_SCHED_QUEUE_POINTER_OFFSET         0x00000800
#define DD429_TX_SCHED_QUEUE_FREQ_OFFSET            0x00000A00
#define DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET     0x00000C00
#define DD429_TX_SCHED_QUEUE_REPEAT_OFFSET          0x00000E00
#define DD429_TX_SCHED_MSG_CONTROL_OFFSET           0x00001000

#define DD429_TX_SCHED_QUEUE_DEPTH                  0x00000040
#define DD429_TX_SCHED_MESSAGE_MEM_DEPTH            0x00000400

#define DD429_ACTIVE                                1
#define DD429_NOT_ACTIVE                            0

#define DD429_LABEL_MASK                            0x000000FF
#define DD429_SDI_MASK                              0x00000300
#define DD429_LABEL_SDI_MASK                        0x000003FF

#define DD429_ALT_SDI_MASK                          0x00001800
#define DD429_ALT_LABEL_SDI_MASK                    0x000018FF

#define DD429_SDI_OFFSET                            8
#define DD429_ALT_SDI_OFFSET                        11

#define DD429_ALT_TO_NORMAL_SDI_OFFSET              3

/* ARINC 429 Option parameters for API function ModifyRepeatedData */
#define ARINC_429_MODIFY_REPEATED_VIA_LABEL         0x0000001
#define ARINC_429_MODIFY_REPEATED_VIA_SDI_LABEL     0x0000002


#define NUM_1553_INT_REGS_PER_CH                    9               /* number of 32-bit interrupt status wds per channel */

/* number of interrupt status words (BD INT STATUS + EACH CHANL * CHANL_NUM) */
#define ACEX_NUM_DWDS_IN_INTERRUPT                  (1 + NUM_1553_INT_REGS_PER_CH * MAX_NUM_1553_CHANNELS)



#endif /* _DDC_UDL_PRIVATE_H_ */
