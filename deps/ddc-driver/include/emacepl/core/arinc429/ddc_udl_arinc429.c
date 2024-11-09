/*******************************************************************************
 * FILE: ddc_udl_arinc429.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to provide functions to support ARINC 429.
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

#include "os/include/ddc_os_types.h"
#include "os/include/ddc_os_private.h"
#include "include/ddc_error_list.h"
#include "include/ddc_device_ids.h"
#include "include/ddc_arinc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_interrupt_private.h"
#include "driver_sdk/ddc_udl_um_regmap_private.h"
#include "core/arinc429/ddc_udl_arinc429_private.h"
#include "core/arinc429/ddc_udl_arinc429_private.h"
#include "core/aio/ddc_udl_aio_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/dio/ddc_udl_dio_private.h"


/* ========================================================================== */
/*                          TYPES AND DEFINES                                 */
/* ========================================================================== */

#define DD429_CONTROL_WORD__NO_OPTIONS  0x00000000

#define TX_LOCK_TIMEOUT                 500   /* 500 millisecond timeout */
#define ARINC_429_TX_STATE__IDLE        0
#define ARINC_429_TX_STATE__BUSY        1

#define LOAD_TX_QUEUE_FIFO_TIMEOUT      30    /* 30 millisecond timeout */
#define LOAD_ASYNC_FIFO_TIMEOUT         30    /* 30 millisecond timeout */

#define INVALID_CAST_IO_MASK            0x00000000

#define CAN_STATUS_RETURN_BYTE_COUNT    16

/* some 429 devices do not do HW byte swapping on 429 memory */
#define DD429_MEM_READ(pDeviceContext, addr, pval, mode) \
{ \
    DDC_MEM_READ(pDeviceContext, addr, pval, mode); \
    \
    if (pDeviceContext->b429MemSwByteSwap) \
    { \
        *pval = DDC_BYTE_ORDER_L(*pval); \
    } \
}

#define DD429_MEM_WRITE(pDeviceContext, addr, pval, mode) \
{ \
    U32BIT u32WriteData = *pval; \
    \
    if (pDeviceContext->b429MemSwByteSwap) \
    { \
        u32WriteData = DDC_BYTE_ORDER_L(u32WriteData); \
    } \
    \
    DDC_MEM_WRITE(pDeviceContext, addr, &u32WriteData, mode); \
}

#define DD429_MEM_READ_TX_GLOBAL(pdev, offset, pdata) \
{ \
    DDC_MEM_READ(pDeviceContext, *(pDeviceContext->sArinc429TxGlobal.pu32MemBA) + offset, pdata, ACEX_32_BIT_ACCESS); \
    if (pDeviceContext->b429MemSwByteSwap) \
    { \
        *pdata = DDC_BYTE_ORDER_L(*pdata); \
    } \
}

#define DD429_MEM_WRITE_TX_GLOBAL(pdev, offset, data) \
{ \
    U32BIT u32MemWriteTxGlobalData = data; \
    if (pDeviceContext->b429MemSwByteSwap) \
    { \
        u32MemWriteTxGlobalData = DDC_BYTE_ORDER_L(u32MemWriteTxGlobalData); \
    } \
    DDC_MEM_WRITE(pDeviceContext, *(pDeviceContext->sArinc429TxGlobal.pu32MemBA) + offset, &u32MemWriteTxGlobalData, ACEX_32_BIT_ACCESS); \
}

/*
    Data Match Table - Used for filters

    Values inside the boxes represent which LABEL/SDI is being accessed

         Bit           Bit              Bit               Bit
          31            0                9 8 7             0
   Addr +----+--------+----+            +---+---------------+
      0 | 31 |        |  0 |            |SDI|     LABEL     |
        +----+--------+----+            +---+---------------+
      1 | 63 |        | 32 |
        +----+--------+----+            Label/SDI = 10 bits
        |    |        |    |
        |    |        |    |
        +----+--------+----+
     30 | 991|        | 960|
        +----+--------+----+
     31 |1023|        | 992|
        +----+--------+----+
 */

#define FILTER_LABEL_SDI_ADDRESS_MASK       0x000003E0  /* upper 5 bits */
#define FILTER_LABEL_SDI_ADDRESS(x)         ((x & FILTER_LABEL_SDI_ADDRESS_MASK) >> 5)

#define FILTER_LABEL_SDI_BIT_MASK           0x0000001F  /* lower 5 bits */
#define FILTER_LABEL_SDI_BIT(x)             (x & FILTER_LABEL_SDI_BIT_MASK)

/* variable speed table defines */
#define DD429_VARIABLE_SPEED__LOWER_TABLE_BPS_START     500
#define DD429_VARIABLE_SPEED__LOWER_TABLE_BPS_END       50000
#define DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_START     50000
#define DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_END       200000
#define DD429_VARIABLE_SPEED__LOWER_TABLE_INDEX_START   5
#define DD429_VARIABLE_SPEED__LOWER_TABLE_INDEX_END     500
#define DD429_VARIABLE_SPEED__UPPER_TABLE_INDEX_START   500
#define DD429_VARIABLE_SPEED__UPPER_TABLE_INDEX_END     650

#define DD429_VARIABLE_SPEED__LOWER_TABLE_STEP_SIZE     100
#define DD429_VARIABLE_SPEED__UPPER_TABLE_STEP_SIZE     1000

#define DD429_VARIABLE_SPEED__TABLE_INDEX_DEFAULT       0
#define DD429_VARIABLE_SPEED__LOWER_TABLE_INDEX_START   5
#define DD429_VARIABLE_SPEED__UPPPER_TABLE_INDEX_START  500
#define DD429_VARIABLE_SPEED__BPS_DEFAULT               0

#define ARINC_429_PROGRAMMABLE_RX_RESET                 0x00002000
#define ARINC_429_PROGRAMMABLE_TX_RESET                 0x00000001

#define DD429_RX_HOST_BUFFER_INT_ENABLE                 0x80000000
#define DD429_RX_HOST_BUFFER_INTERVAL_100MS             0x00000064

/* ========================================================================== */
/*                           LOCAL VARIABLES                                  */
/* ========================================================================== */

static const U32BIT dd429RxControlMaskTable[] =
{
    DD429_RX_CONTROL_MASK__WRAP_AROUND,         /* Bit  0       Wrap Around     0=normal, 1=wrap    */
    DD429_RX_CONTROL_MASK__IGNORE_LABEL,        /* Bit  1       Ignore Label                        */
    DD429_RX_CONTROL_MASK__RX_FULL,             /* Bit  2-3     RX Full 00=25%, 01=50%, 11=100%     */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit  3       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__MODE,                /* Bit  4       Mode    0=mailbox mode, 1=fifo mode */
    DD429_RX_CONTROL_MASK__TIMETAG,             /* Bit  5       Timetag 1=IRIG/timetag, 0=none      */
    DD429_RX_CONTROL_MASK__LABEL_AUTO_CLEAR,    /* Bit  6       Label Auto Clear                    */
    DD429_RX_CONTROL_MASK__PARITY,              /* Bit  7-8     Parity  00=odd parity, 01=even parity, 10=no parity, 11=reserved */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit  8       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__SPEED,               /* Bit  9       Speed   1=low speed, 0=high speed   */
    DD429_RX_CONTROL_MASK__DATA_REPEATER,       /* Bit 10       Data Repeater 0=disabled, 1=enabled */
    DD429_RX_CONTROL_MASK__BIT_FORMAT,          /* Bit 11       Bit Format  0=normal, 1=Alternate   */
    DD429_RX_CONTROL_MASK__DATA_TRANSFER,       /* Bit 12       Data Transfer   0=numeric,1=file    */
    DD429_RX_CONTROL_MASK__RESET,               /* Bit 13       Reset                               */
    DD429_RX_CONTROL_MASK__FIFO_EMPTY,          /* Bit 14       FIFO Empty                          */
    DD429_RX_CONTROL_MASK__FIFO_COUNT,          /* Bit 24-15    FIFO Count                          */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 16       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 17       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 18       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 19       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 20       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 21       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 22       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 23       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 24       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__FILTER_PARITY_ERR,   /* Bit 25       Filter on Parity Error              */
    DD429_RX_CONTROL_MASK__FILTER_DATA,         /* Bit 26       Filter on Data                      */
    DD429_RX_CONTROL_MASK__FILTER_CODE,         /* Bit 27-28    Filter Code     00=filter all, 01=filter data/parity/sdi, 10=filter on data/parity/SSM */
    DD429_RX_CONTROL_MASK__PLACEHOLDER,         /* Bit 28       (Placeholder)                       */
    DD429_RX_CONTROL_MASK__ARINC575,            /* Bit 29       ARINC 575       0=429 1=575         */
    DD429_RX_CONTROL_MASK__OVERFLOW,            /* Bit 30       Overflow                            */
    DD429_RX_CONTROL_MASK__RSVD2                /* Bit 31       Reserved                            */
};

/* ============================================================================ */

static const U32BIT au32DD429_DataMatchTable_BitTable[DD429_DATA_MATCH_TABLE_LEN] =
{
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000
};

static const U32BIT LOOPBACK_MAPPING_MASK[DD429_MAX_NUM_TX] =
{
    /* TX Channel */
    0x00000001, /* 1 */ /* ACEX_429_RX_WRAP_AROUND_REG */
    0x00000002, /* 2 */
    0x00000004, /* 3 */
    0x00000008, /* 4 */
    0x00000010, /* 5 */
    0x00000020, /* 6 */
    0x00000040, /* 7 */
    0x00000080, /* 8 */
    0x00000100, /* 9 */
    0x00000200, /* 10 */
    0x00000400, /* 11 */
    0x00000800, /* 12 */
    0x00001000, /* 13 */
    0x00002000, /* 14 */
    0x00004000, /* 15 */
    0x00008000, /* 16 */
    0x00010000, /* 17 */
    0x00020000, /* 18 */
    0x00040000, /* 19 */
    0x00080000, /* 20 */
    0x00100000, /* 21 */
    0x00200000, /* 22 */
    0x00400000, /* 23 */
    0x00800000, /* 24 */
    0x01000000, /* 25 */
    0x02000000, /* 26 */
    0x04000000, /* 27 */
    0x08000000, /* 28 */
    0x10000000, /* 29 */
    0x20000000, /* 30 */
    0x40000000, /* 31 */
    0x80000000, /* 32 */
    0x00000001, /* 33 */ /* ACEX_429_RX_WRAP_AROUND_2_REG */
    0x00000002, /* 34 */
    0x00000004, /* 35 */
    0x00000008, /* 36 */
};

/* ============================================================================ */
/* ============================================================================ */
static U32BIT _RxControlCommandGetValue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32Command
);

static U32BIT _ConvertTesterOptionsToControlWord
(
    DD429_TESTER_OPTIONS_TYPE *psTesterOptions
);

static U32BIT _ConvertSpeedToTableIndex
(
    U32BIT *pu32Speed
);

static U32BIT _ConvertTableIndexToSpeed
(
    U32BIT u32TableIndex
);

static void _ARINC429SetLoopbackMapping
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Rx,
    U32BIT u32Tx,
    U8BIT u8Clear
);

static U32BIT _ARINC429GetLoopbackMapping
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Rx
);

static S16BIT _ARINC429SendAsync
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32TxData,
    U32BIT u32Control
);

/* ------------------------------------ */
/* COMMAND PROCESSING FUNCTIONS         */
/* ------------------------------------ */
static S16BIT _ARINC429GeneralCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429ControlCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429FifoCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429RxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429TxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429RxControlCommandSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
);

static S16BIT _ARINC429RxControlCommandGet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429FilterCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429MailboxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

static S16BIT _ARINC429TesterCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
);

/* ------------------------------------ */
/* 429 RX HOST BUFFER FUNCTIONS         */
/* ------------------------------------ */
static void ARINC429OnRxHostBufferChannelEnabled(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel);

static void ARINC429OnRxHostBufferChannelSpeedChanged(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel);

static void ARINC429ConfigInternalRxInterrupt(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel);

static size_t ARINC429ReadRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel, U32BIT u32NumMsgsToRead, U32BIT* pReadData);

static void ARINC429CreateRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel);

static void ARINC429DeleteRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel);

static void ARINC429InstallRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext);

static void ARINC429UninstallRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, BOOLEAN disposing);

static BOOLEAN ARINC429CheckHostBufferOverflow(U32BIT u32NextRead, U32BIT u32NextWrite, U32BIT u32NumElements);

static void ARINC429CopyFifoToHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U32BIT u16Channel, U32BIT* pHostBufferMemory, U32BIT u32NumMessagesToCopy, U32BIT u32FifoMessageSize);

/* ============================================================================ */
/* ============================================================================ */

/*******************************************************************************
 * Name:    ARINC429TxSetStateIdle
 *
 * Description:
 *
 *      This function is sets the 429 TX state to idle. See function
 *      _ARINC429TxSetStateBusy() for more information on usage.
 *
 * In:  pDeviceContext          device context
 * Out: - none -
 ******************************************************************************/
void ARINC429TxSetStateIdle
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429TxState, pDeviceContext->semArinc429TxStateFlag);
    pDeviceContext->u32Arinc429TxState = ARINC_429_TX_STATE__IDLE;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429TxState, pDeviceContext->semArinc429TxStateFlag);
}

/*******************************************************************************
 * Name:    _ARINC429TxSetStateBusy
 *
 * Description:
 *
 *      This function is used as a locking mechinisim to prevent multi-threaded
 *      writing of the 429 TX Global Registers. In order to write to a 429 TX
 *      Global Register, one must write to the select register first.
 *
 *      The function checks to see if the state is idle. If is it not idle,
 *      we wait until there is a timeout. If a timeout occurs we return
 *      an error, otherwise we set the state to busy.
 *
 * In:  pDeviceContext          device context
 * Out: return                  error condition
 ******************************************************************************/
static S16BIT _ARINC429TxSetStateBusy
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32ClockTimeEnd;
    U32BIT u32ClockTimeNow;
    U32BIT u32Arinc429TxState;

    u32ClockTimeEnd = ddcUdlOsGetClockMs() + TX_LOCK_TIMEOUT;

    do
    {
        DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429TxState, pDeviceContext->semArinc429TxStateFlag);

        u32Arinc429TxState = pDeviceContext->u32Arinc429TxState;

        if (u32Arinc429TxState == ARINC_429_TX_STATE__IDLE)
        {
            pDeviceContext->u32Arinc429TxState = ARINC_429_TX_STATE__BUSY;
            DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429TxState, pDeviceContext->semArinc429TxStateFlag);
            return DDC_UDL_ERROR__SUCCESS;
        }

        DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429TxState, pDeviceContext->semArinc429TxStateFlag);

        /* check if timeout is reached */
        u32ClockTimeNow = ddcUdlOsGetClockMs();

        if (u32ClockTimeNow >= u32ClockTimeEnd)
        {
            break;
        }
    } while (u32Arinc429TxState != ARINC_429_TX_STATE__IDLE);

    return DDC_UDL_ERROR__TIMEOUT;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429ChInitialize

   Description:
        This function initializes all the ARINC 429 data structures with the
        base register and memory locations for the various ARINC 429 components.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      DDC_UDL_ERROR__SUCCESS if ok
   ---------------------------------------------------------------------------------*/
S16BIT ARINC429ChInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    U16BIT numDevicesIndex;
    size_t i, j;
    UM_DEVICE_INFO       *pUmDevicePtr;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - NumProg:%d\n", pDeviceContext->u8NumProg429RxTx);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - NumTx:%d\n", pDeviceContext->u8NumDed429Tx);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - NumRx:%d\n", pDeviceContext->u8NumDed429Rx);

    /* allocate memory for ARINC 429 channel structs */
    for (i = 0; i < MAX_NUM_429_CHANNELS; i++)
    {
        pDeviceContext->pRxChnl429[i] = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(ACEX_429_RX_TYPE));
        pDeviceContext->pTxChnl429[i] = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(ACEX_429_TX_TYPE));

        if ((pDeviceContext->pRxChnl429[i] == NULL) ||
            (pDeviceContext->pTxChnl429[i] == NULL))
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }
    }

    /* initialize all members to zero */
    for (i = 0; i < MAX_NUM_429_CHANNELS; i++)
    {
        memset(pDeviceContext->pRxChnl429[i], 0, sizeof(ACEX_429_RX_TYPE));
        memset(pDeviceContext->pTxChnl429[i], 0, sizeof(ACEX_429_TX_TYPE));
        memset(&(pDeviceContext->sTxChnlExtend429[i]), 0, sizeof(ACEX_429_TX_TYPE));
    }

    pDeviceContext->p429TxMemory = DDC_KERNEL_MALLOC(pDeviceContext, 1024 * sizeof(U32BIT));

    memset(&(pDeviceContext->sArinc429RxGlobal), 0, sizeof(ACEX_429_RX_TYPE));
    memset(&(pDeviceContext->sArinc429TxGlobal), 0, sizeof(ACEX_429_TX_TYPE));
    memset(&(pDeviceContext->U.sTxScheduler), 0, sizeof(ARINC_429_TX_SCHEDULER_DATA));


    pDeviceContext->u8Arinc429BitFormat = DD429_BITFORMAT_ORIG;

    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE,  "DeviceIndex:%d, DeviceType: 0x%04X, NumberInstances: %d, NumComponents: %d\n",
            (int)numDevicesIndex, pUmDevicePtr->umDevType, pUmDevicePtr->umDevNumInstances, pUmDevicePtr->umDevNumComponents);

        if ((pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_TX)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_RX)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_GLOBAL)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIO_UART)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_TX_MF)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_RX_MF)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIO_CAST_GLOBAL_UART)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIO_CAST_ASYNC_UART)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_MIO_CAST_SYNC_ASYNC_UART)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_TX_SCHEDULE_EXTEND)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_GLOBAL_TX_MF_2)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_GLOBAL_RX_MF_2)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_TX_MF_2)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_ARINC_429_RX_MF_2)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_EMBEDDED_ARINC_429_TX_SCHEDULE_EXTEND)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_TX)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_EMBEDDED_ARINC_429_GLOBAL_RX)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_EMBEDDED_ARINC_429_TX)
            || (pUmDevicePtr->umDevType == UM_DEVICE_ID_EMBEDDED_ARINC_429_RX))
        {
            for (j = 0; j < pUmDevicePtr->umDevNumComponents; j++)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - Component Type ID: %08X\n",
                    pUmDevicePtr->umComponentInfo[j].umComponentType);

                switch (pUmDevicePtr->umComponentInfo[j].umComponentType)
                {
                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX:
                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX_MF:
                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_TX_2:
                    case UM_COMPONENTS_ID_EMBEDDED_ARINC_429_GLOBAL_TX:
                    {
                        pDeviceContext->sArinc429TxGlobal.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[0]);
                        pDeviceContext->sArinc429TxGlobal.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                        pDeviceContext->sArinc429TxGlobal.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[0]);
                        pDeviceContext->sArinc429TxGlobal.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE,  "429 - Global Register TX: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArinc429TxGlobal.pu32RegBA),
                            *(pDeviceContext->sArinc429TxGlobal.pu32RegSize));

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - Global Memory TX: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArinc429TxGlobal.pu32MemBA),
                            *(pDeviceContext->sArinc429TxGlobal.pu32MemSize));
                        break;
                    }

                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX:
                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX_MF:
                    case UM_COMPONENTS_ID_ARINC_429_GLOBAL_RX_2:
                    case UM_COMPONENTS_ID_EMBEDDED_ARINC_429_GLOBAL_RX:
                    {
                        pDeviceContext->sArinc429RxGlobal.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[0]);
                        pDeviceContext->sArinc429RxGlobal.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                        pDeviceContext->sArinc429RxGlobal.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[0]);
                        pDeviceContext->sArinc429RxGlobal.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - Global RX: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArinc429RxGlobal.pu32RegBA),
                            *(pDeviceContext->sArinc429RxGlobal.pu32RegSize));

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - Global Memory RX: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sArinc429RxGlobal.pu32MemBA),
                            *(pDeviceContext->sArinc429RxGlobal.pu32MemSize));
                        break;
                    }

                    case UM_COMPONENTS_ID_ARINC_429_TX:
                    case UM_COMPONENTS_ID_ARINC_429_TX_MF:
                    case UM_COMPONENTS_ID_ARINC_429_TX_2:
                    case UM_COMPONENTS_ID_EMBEDDED_ARINC_429_TX:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->pTxChnl429[i]->pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->pTxChnl429[i]->pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->pTxChnl429[i]->pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->pTxChnl429[i]->pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - TX: Register BA:%08X  Size:%d\n",
                                *(pDeviceContext->pTxChnl429[i]->pu32RegBA),
                                *(pDeviceContext->pTxChnl429[i]->pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - TX: Memory BA:%08X  Size:%d\n",
                                *(pDeviceContext->pTxChnl429[i]->pu32MemBA),
                                *(pDeviceContext->pTxChnl429[i]->pu32MemSize));
                        }
                        break;
                    }

                    case UM_COMPONENTS_ID_ARINC_429_TX_SCHEDULE_EXTEND:
                    case UM_COMPONENTS_ID_EMBEDDED_ARINC_429_TX_SCHEDULE_EXTEND:
                    {
                        U32BIT u32RegisterValue = 0;

                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sTxChnlExtend429[i].pu32RegBA   = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sTxChnlExtend429[i].pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sTxChnlExtend429[i].pu32MemBA   = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sTxChnlExtend429[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429,DDC_DBG_ARINC429_INITIALIZE, "429 - TX Extended: Register BA:%08X  Size:%d\n",
                                        *(pDeviceContext->sTxChnlExtend429[i].pu32RegBA),
                                        *(pDeviceContext->sTxChnlExtend429[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429,DDC_DBG_ARINC429_INITIALIZE, "429 - TX Extended: Memory BA:%08X  Size:%d\n",
                                        *(pDeviceContext->sTxChnlExtend429[i].pu32MemBA),
                                        *(pDeviceContext->sTxChnlExtend429[i].pu32MemSize));
                        }

                        pDeviceContext->bExtendedScheduler = TRUE;

                        /*
                            Add HWVER_CAPABILITY_ARINC_429_HI_PRIORITY_FIFO_FREE_COUNTER - There is a new
                            capability bit (offset 0x01h of BRD_GLOBAL_GENERAL_REG_BASE_ADDR_INST_0, bit 12),
                            that when set, indicates read back capability of the hi priority fifo data available counter.
                        */
                        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CAPBILITIES, &u32RegisterValue);

                        if (u32RegisterValue & BD_CAPABILITIES_ARINC_429_HI_PRI_FIFO_FREE_COUNTER)
                        {
                            pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_ARINC_429_HI_PRIORITY_FIFO_FREE_COUNTER;
                        }

                        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_ARINC_429_TX_SCHEDULE_EXTEND;
                        break;
                    }

                    case UM_COMPONENTS_ID_ARINC_429_RX:
                    case UM_COMPONENTS_ID_ARINC_429_RX_MF:
                    case UM_COMPONENTS_ID_ARINC_429_RX_2:
                    case UM_COMPONENTS_ID_EMBEDDED_ARINC_429_RX:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->pRxChnl429[i]->pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->pRxChnl429[i]->pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->pRxChnl429[i]->pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->pRxChnl429[i]->pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - RX: Register BA:%08X  Size:%d\n",
                                *(pDeviceContext->pRxChnl429[i]->pu32RegBA),
                                *(pDeviceContext->pRxChnl429[i]->pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - RX: Memory BA:%08X  Size:%d\n",
                                *(pDeviceContext->pRxChnl429[i]->pu32MemBA),
                                *(pDeviceContext->pRxChnl429[i]->pu32MemSize));
                        }
                        break;
                    }

                    case UM_COMPONENTS_ID_MIO_UART:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sMioUart429.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sMioUart429.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sMioUart429.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sMioUart429.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - UART: Register BA:%08X  Size:%d\n",
                                *(pDeviceContext->sMioUart429.pu32RegBA),
                                *(pDeviceContext->sMioUart429.pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - UART: Memory BA:%08X  Size:%d\n",
                                *(pDeviceContext->sMioUart429.pu32MemBA),
                                *(pDeviceContext->sMioUart429.pu32MemSize));
                        }
                        break;
                    }
                    case UM_COMPONENTS_ID_MIO_CAST_UART_GLOBAL:
                    {
                        pDeviceContext->sMioCastUartGlobal429.pu32RegBA =
                            &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[0]);
                        pDeviceContext->sMioCastUartGlobal429.pu32RegSize =
                            &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                        pDeviceContext->sMioCastUartGlobal429.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[0]);
                        pDeviceContext->sMioCastUartGlobal429.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST GLOBAL UART: Register BA:%08X  Size:%d\n",
                                *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA),
                                *(pDeviceContext->sMioCastUartGlobal429.pu32RegSize));

                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST GLOBAL UART: Memory BA:%08X  Size:%d\n",
                                *(pDeviceContext->sMioCastUartGlobal429.pu32MemBA),
                                *(pDeviceContext->sMioCastUartGlobal429.pu32MemSize));
                        break;
                    }

                    case UM_COMPONENTS_ID_MIO_CAST_UART_SERIAL:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sMioCastUart429[i].pu32RegBA =
                                &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sMioCastUart429[i].pu32RegSize =
                                &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sMioCastUart429[i].pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sMioCastUart429[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST UART: Register BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUart429[i].pu32RegBA),
                                    *(pDeviceContext->sMioCastUart429[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST UART: Memory BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUart429[i].pu32MemBA),
                                    *(pDeviceContext->sMioCastUart429[i].pu32MemSize));
                        }
                        break;
                    }

                    case UM_COMPONENTS_ID_MIO_CAST_UART_ASYNC:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sMioCastUartAsync429[i].pu32RegBA =
                                &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sMioCastUartAsync429[i].pu32RegSize =
                                &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sMioCastUartAsync429[i].pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sMioCastUartAsync429[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST Async UART: Register BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartAsync429[i].pu32RegBA),
                                    *(pDeviceContext->sMioCastUartAsync429[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST Async UART: Memory BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartAsync429[i].pu32MemBA),
                                    *(pDeviceContext->sMioCastUartAsync429[i].pu32MemSize));
                        }
                        break;
                    }

                    case UM_COMPONENTS_ID_MIO_CAST_UART_HDLC:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sMioCastUartHDLC429[i].pu32RegBA =
                                &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sMioCastUartHDLC429[i].pu32RegSize =
                                &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sMioCastUartHDLC429[i].pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sMioCastUartHDLC429[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST HDLC UART: Register BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartHDLC429[i].pu32RegBA),
                                    *(pDeviceContext->sMioCastUartHDLC429[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST HDLC UART: Memory BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartHDLC429[i].pu32MemBA),
                                    *(pDeviceContext->sMioCastUartHDLC429[i].pu32MemSize));
                        }
                        break;
                    }

                    case UM_COMPONENTS_ID_MIO_CAST_UART_SDLC:
                    {
                        for (i = 0; i < pUmDevicePtr->umDevNumInstances; i++)
                        {
                            pDeviceContext->sMioCastUartSDLC429[i].pu32RegBA =
                                &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[i]);
                            pDeviceContext->sMioCastUartSDLC429[i].pu32RegSize =
                                &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);

                            pDeviceContext->sMioCastUartSDLC429[i].pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[i]);
                            pDeviceContext->sMioCastUartSDLC429[i].pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST SDLC UART: Register BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartSDLC429[i].pu32RegBA),
                                    *(pDeviceContext->sMioCastUartSDLC429[i].pu32RegSize));

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "429 - CAST SDLC UART: Memory BA:%08X  Size:%d\n",
                                    *(pDeviceContext->sMioCastUartSDLC429[i].pu32MemBA),
                                    *(pDeviceContext->sMioCastUartSDLC429[i].pu32MemSize));
                        }
                        break;
                    }

                    default:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INITIALIZE, "Not a 429 Component\n");
                        break;
                    }
                } /* switch */
            } /* for components */
        } /* if umDevType */
    } /* for numDevicesIndex */

    if (pDeviceContext->sMioCastUartGlobal429.pu32RegBA != NULL)
    {
        ARINC429CastSerialIOCapChanMatrix(pDeviceContext);
    }

    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) || (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX))
    {
        U32BIT u32RegisterValue;

        /* Ensure all 429 channels disconnected from Bus */
        if (pDeviceContext->u8NumProg429RxTx > 0)
        {
            u32RegisterValue = 0x00000000;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RELAY, &u32RegisterValue);

            /* Default all 429 programmable channels to disabled */
            u32RegisterValue = 0x00000000;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX, &u32RegisterValue);
        }

        /*
            the 118 card will do HW byte swapping for 1553, it will set the eEndiannessMode to HW,
            but 429 needs SW byte swapping. The DD-40xxx cards set eEndiannessMode to SW and does
            the byte swapping at the memory read level.
        */
        pDeviceContext->b429MemSwByteSwap = TRUE;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlARINC429ChannelCleanup
 *
 * Description:
 *      This function performs all necessary 429 channel cleanup.
 *
 * In   pDeviceContext
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlARINC429ChannelCleanup
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U8BIT i;

    if (pDeviceContext->p429TxMemory)
    {
        DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->p429TxMemory);
        pDeviceContext->p429TxMemory = NULL;
    }

    /* DD-40000 compatible devices only */
    if ((pDeviceContext->u16DriverType == ACEX_DD429_PCIE_DRIVER) ||
        (pDeviceContext->u16DriverType == ACEX_DD429_PCI_DRIVER))
    {
#if DDC_DMA_429
        /* free ARINC 429 RX FIFO DMA buffer */
        if (pDeviceContext->ARINC429RxDMATarget)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pDeviceContext->ARINC429RxDMA_Size,
                pDeviceContext->ARINC429RxDMATarget,
                pDeviceContext->ARINC429RxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_RX_DMA);

            pDeviceContext->ARINC429RxDMATarget = NULL;
        }

        /* free ARINC 429 SetTxFrameControl DMA buffer */
        if (pDeviceContext->ARINC429TxDMATarget)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pDeviceContext->ARINC429TxDMA_Size,
                pDeviceContext->ARINC429TxDMATarget,
                pDeviceContext->ARINC429TxDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_TX_DMA);

            pDeviceContext->ARINC429TxDMATarget = NULL;
        }


        if (pDeviceContext->Arinc429VoltageMonDMATarget)
        {
            DDC_DMA_FREE(
                pDeviceContext,
                pDeviceContext->Arinc429VoltageMonDMA_Size,
                pDeviceContext->Arinc429VoltageMonDMATarget,
                pDeviceContext->Arinc429VoltageMonDMA_Addr,
                DDC_MEMORY_REGION__ARINC_429_VOLT_MON_DMA);

            pDeviceContext->Arinc429VoltageMonDMATarget = NULL;
        }
#endif /* DDC_DMA_429 */
    }

    for (i = 0; i < MAX_NUM_429_CHANNELS; i++)
    {
        if (pDeviceContext->pRxChnl429[i])
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pRxChnl429[i]);
            pDeviceContext->pRxChnl429[i] = NULL;
        }

        if (pDeviceContext->pTxChnl429[i])
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pTxChnl429[i]);
            pDeviceContext->pTxChnl429[i] = NULL;
        }
    }
}

/*===================================================================================
                        Existing Functions in 3.3.1
   =====================================================================================*/

/*-------------------------------------------------------------------------------
   Function:
       ARINC429SetTimeStampConfig

   Description:
        This function sets the time tag configuration

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pIoctlParams         - will contain the new time tag configuration

   Returns:
     none.
   ---------------------------------------------------------------------------------*/
void ARINC429SetTimeStampConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32TempRegisterValue = 0;
    U32BIT u32Format;
    U32BIT u32Resolution;
    U32BIT u32Rollover;

    u32Format = DDC_IOCTL_U32(pIoctlParams->Param1);
    u32Rollover = DDC_IOCTL_U32(pIoctlParams->Param2);
    u32Resolution = DDC_IOCTL_U32(pIoctlParams->Param3);

    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);
    u32TempRegisterValue |= 0x00000001;
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_TIMETAG,  "429 - IRIG Config ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);

    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
    u32TempRegisterValue &= 0xFFFFDF00;
    u32TempRegisterValue |= (u32Resolution & 0x0000000F);
    u32TempRegisterValue |= ((u32Rollover << 4) & 0x000000F0);

    /* 0 = 48 bit timer. IRIG B is the only other time stamp format supported */
    if (u32Format != 0)
    {
        u32TempRegisterValue |= 0x00002000;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_TIMETAG,  "429 - ARINC429SetTimeStampConfig Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetTimeStampConfig

   Description:
        This function gets the time tag configuration

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pIoctlParams         - will contain the new time tag configuration

   Returns:
     none.
   ---------------------------------------------------------------------------------*/
void ARINC429GetTimeStampConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32TempRegisterValue = 0;
    U32BIT u32Format;
    U32BIT u32Resolution;
    U32BIT u32Rollover;

    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_TIMETAG,  "429 - ARINC429GetTimeStampConfig Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG,  u32TempRegisterValue);

    u32Resolution = (u32TempRegisterValue & 0x0000000F);
    u32Rollover = ((u32TempRegisterValue >> 4) & 0x0000000F);

    if ((u32TempRegisterValue & 0x00002000) != 0)
    {
        u32Format = 2;
    }
    else
    {
        u32Format = 0;
    }

    pIoctlParams->Param1 = u32Format;
    pIoctlParams->Param2 = u32Resolution;
    pIoctlParams->Param3 = u32Rollover;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetLegacyLoopback

   Description:
       For potential future use.

   Parameters:

   Returns:
    0
   ---------------------------------------------------------------------------------*/
#if 0 /* for future use */
DDC_LOCAL U32BIT ARINC429GetLegacyLoopback
(
    void
)
{
    return 0;
}

#endif /* for future use */

/*-------------------------------------------------------------------------------
   Function:
       ARINC429SetLegacyLoopback

   Description:
        This function initializes the Rx to loopback to the Tx as it was done for
        flexcore. For backward compatibility.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429SetLegacyLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_LOOPBACK, "429 - ARINC429SetLegacyLoopback \n");

    if ((pDeviceContext->u8NumDed429Rx == 8) && (pDeviceContext->u8NumDed429Tx == 4))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
    }
    if ((pDeviceContext->u8NumDed429Rx == 12) && (pDeviceContext->u8NumDed429Tx == 4))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 9, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 10, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 11, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
        ARINC429SetLoopback(pDeviceContext, 12, 4);
    }
    else if ((pDeviceContext->u8NumDed429Rx == 16) && (pDeviceContext->u8NumDed429Tx == 4))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 9, 1);
        ARINC429SetLoopback(pDeviceContext, 10, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 11, 2);
        ARINC429SetLoopback(pDeviceContext, 12, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 13, 3);
        ARINC429SetLoopback(pDeviceContext, 14, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
        ARINC429SetLoopback(pDeviceContext, 15, 4);
        ARINC429SetLoopback(pDeviceContext, 16, 4);
    }
    else if ((pDeviceContext->u8NumDed429Rx == 16) && (pDeviceContext->u8NumDed429Tx == 8))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
        ARINC429SetLoopback(pDeviceContext, 9, 5);
        ARINC429SetLoopback(pDeviceContext, 10, 5);
        ARINC429SetLoopback(pDeviceContext, 11, 6);
        ARINC429SetLoopback(pDeviceContext, 12, 6);
        ARINC429SetLoopback(pDeviceContext, 13, 7);
        ARINC429SetLoopback(pDeviceContext, 14, 7);
        ARINC429SetLoopback(pDeviceContext, 15, 8);
        ARINC429SetLoopback(pDeviceContext, 16, 8);
    }
    else if ((pDeviceContext->u8NumDed429Rx == 8) && (pDeviceContext->u8NumDed429Tx == 6))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 4);
        ARINC429SetLoopback(pDeviceContext, 7, 5);
        ARINC429SetLoopback(pDeviceContext, 8, 6);
    }
    else if ((pDeviceContext->u8NumDed429Rx == 12) && (pDeviceContext->u8NumDed429Tx == 6))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
        ARINC429SetLoopback(pDeviceContext, 9, 5);
        ARINC429SetLoopback(pDeviceContext, 10, 5);
        ARINC429SetLoopback(pDeviceContext, 11, 6);
        ARINC429SetLoopback(pDeviceContext, 12, 6);
    }
    else if ((pDeviceContext->u8NumDed429Rx == 16) && (pDeviceContext->u8NumDed429Tx == 6))
    {
        ARINC429SetLoopback(pDeviceContext, 1, 1);
        ARINC429SetLoopback(pDeviceContext, 2, 1);
        ARINC429SetLoopback(pDeviceContext, 13, 1);
        ARINC429SetLoopback(pDeviceContext, 3, 2);
        ARINC429SetLoopback(pDeviceContext, 4, 2);
        ARINC429SetLoopback(pDeviceContext, 14, 2);
        ARINC429SetLoopback(pDeviceContext, 5, 3);
        ARINC429SetLoopback(pDeviceContext, 6, 3);
        ARINC429SetLoopback(pDeviceContext, 15, 3);
        ARINC429SetLoopback(pDeviceContext, 7, 4);
        ARINC429SetLoopback(pDeviceContext, 8, 4);
        ARINC429SetLoopback(pDeviceContext, 16, 4);
        ARINC429SetLoopback(pDeviceContext, 9, 5);
        ARINC429SetLoopback(pDeviceContext, 10, 5);
        ARINC429SetLoopback(pDeviceContext, 11, 6);
        ARINC429SetLoopback(pDeviceContext, 12, 6);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429SetLoopback

   Description:
        This function programs the receiver to which transmitter it will be connected
        to when in internal loopback.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  s16Rx             - the receiver number
      In  s16Tx             - the transmitter number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429SetLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    S16BIT s16Rx,
    S16BIT s16Tx
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32Value;

    u32RegisterAddress = *(pDeviceContext->pRxChnl429[s16Rx-1]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
    u32Value = LOOPBACK_MASK[s16Tx - 1];
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_LOOPBACK, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", u32RegisterAddress, u32Value);
    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32Value);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetLoopback

   Description:
        This function returns the transmitter number that the receiver is
        programmed to be connected to when in internal loopback.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  s16Rx             - the receiver number

   Returns:
      The transmitter number.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429GetLoopback
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    S16BIT s16Rx
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32Value = 0;

    u32RegisterAddress = *(pDeviceContext->pRxChnl429[s16Rx-1]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32Value);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_LOOPBACK, "429 - ARINC429RegRead Address: 0x%08X, Value: 0x%08X\n", u32RegisterAddress, u32Value);

    return u32Value;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetMemoryAddress

   Description:
        This function takes a flexcore 429 memory address and translates it to
        a SFP memory address.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      SFP memory address
   ---------------------------------------------------------------------------------*/
DDC_LOCAL U32BIT ARINC429GetMemoryAddress
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIo
)
{
    U32BIT u32MemoryAddress;
    U32BIT u32MemoryOffset;
    U8BIT u16ChannelIndex;

    /* NOTE: base address of RX Global and TX Global are the same. they both point to ARNIC_429_GLOBAL */
    if (pPciIo->Param1 < START_SCH_MEMORY)
    {
        /* Rx FIFO/Mailbox address */
        u16ChannelIndex = (U8BIT)((pPciIo->Param1 - START_RX_FIFO_MEMORY) / MIO_RX_FIFOMB_LEN);
        u32MemoryOffset = (DDC_IOCTL_U32(pPciIo->Param1) - MIO_RX_FIFOMB_0) % MIO_RX_FIFOMB_LEN;
        u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16ChannelIndex]->pu32MemBA) + u32MemoryOffset;
    }
    else if (pPciIo->Param1 < START_DMT_MEMORY)
    {
        /* Tx scheduler address */
        u32MemoryOffset = DDC_IOCTL_U32(pPciIo->Param1) - START_SCH_MEMORY;
        u32MemoryAddress = *(pDeviceContext->sArinc429TxGlobal.pu32MemBA) + u32MemoryOffset;
    }
    else
    {
        /* DMT address */
        u32MemoryOffset = DDC_IOCTL_U32(pPciIo->Param1) - START_DMT_MEMORY;
        u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + u32MemoryOffset + 0x00002000;
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_TRANSLATE_MEMORY_ADDR, "429 - ARINC429GetMemoryAddress:In 0x%08X, Out 0x%08X\n", pPciIo->Param1, u32MemoryAddress);
    return u32MemoryAddress;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetRegisterAddress

   Description:
        This function takes a flexcore 429 register address and translates it to
        a SFP memory address.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      SFP register address
   ---------------------------------------------------------------------------------*/
DDC_LOCAL U32BIT ARINC429GetRegisterAddress
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIo
)
{
    U32BIT u32RegisterAddress = 0;
    U8BIT u16ChannelIndex;
    U8BIT u8RegisterOffset;

    if ((pPciIo->Param1 >= MIO_UART_1) && (pPciIo->Param1 <= MIO_UART_IO_CTRL))
    {
        if ((pDeviceContext->u8NumRS232 > 0) ||
            (pDeviceContext->u8NumRS485 > 0) ||
            (pDeviceContext->u8NumUart > 0))
        {
            u16ChannelIndex = (U8BIT)((pPciIo->Param1 - START_RX_REG_ADDRESS) / 3);
            u8RegisterOffset = (U8BIT)(pPciIo->Param1 - START_SCH_REG_ADDRESS); /* offset */
            u32RegisterAddress = *(pDeviceContext->sMioUart429.pu32RegBA) + u8RegisterOffset;
        }
    }
    else if (pPciIo->Param1 >= START_SCH_REG_ADDRESS)
    {
        /* Tx scheduler register */
        u8RegisterOffset = (U8BIT)(pPciIo->Param1 - START_SCH_REG_ADDRESS); /* offset */
        u32RegisterAddress += *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + u8RegisterOffset;
    }
    else
    {
        /* Rx or Tx control register */
        u16ChannelIndex = (U8BIT)((pPciIo->Param1 - START_RX_REG_ADDRESS) / 3);
        u8RegisterOffset = (U8BIT)((pPciIo->Param1 - START_RX_REG_ADDRESS) % 3);
        if (u16ChannelIndex >= MIO_429_MAX_NUM_RX)
        {
            /* transmit channel */
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[(U8BIT)(pPciIo->Param1 - START_TX_REG_INDEX)]->pu32RegBA);
        }
        else
        {
            /* receive channel */
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16ChannelIndex]->pu32RegBA) + u8RegisterOffset;
        }
    }
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_TRANSLATE_REG_ADDR, "429 - ARINC429GetRegisterAddress:In 0x%08X, Out 0x%08X\n", pPciIo->Param1, u32RegisterAddress);
    return u32RegisterAddress;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429ReadIRIGConfigRegister

   Description:
        This function reads the various registers that contain time tag
        configuration information and puts it into the flexcore format.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      Time tag configuration information in flexcore format.
   ---------------------------------------------------------------------------------*/
DDC_LOCAL U32BIT ARINC429ReadIRIGConfigRegister
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32TempRegisterValue;
    U32BIT u32RegisterValue;
    U32BIT u32ResetBit;
    U32BIT u32EnableBit;
    U32BIT u32IntEnableBit;
    U32BIT u32Format;
    U32BIT u32Resolution;
    U32BIT u32Rollover;

    /* get enable bit */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32EnableBit);
    u32EnableBit &= NEW_IRIG_ENABLE_MASK; /* same bit position as old register */

    /* get TT format */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
    if (u32TempRegisterValue & NEW_IRIG_FORMAT_MASK)
    {
        u32Format = 0x04;
    }
    else
    {
        u32Format = 0x00;
    }

    /* get TT resloution */
    u32Resolution = (u32TempRegisterValue & NEW_IRIG_RESOLUTION_MASK) << 8;

    /* get TT rollover */
    u32Rollover = (u32TempRegisterValue & NEW_IRIG_ROLLOVER_MASK) << 7;

    /* get reset bit. probably always 0 */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_CTRL_PULSE_REG, &u32TempRegisterValue);
    u32ResetBit = (u32TempRegisterValue & NEW_IRIG_RESET_MASK) << 7;

    /* get TT rollover interrupt enable */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);
    u32IntEnableBit = (u32TempRegisterValue & NEW_IRIG_TT_RO_INT_ENABLE_MASK) >> 17;

    /* put it all together */
    u32RegisterValue = u32EnableBit | u32Format | u32Resolution | u32Rollover | u32ResetBit | u32IntEnableBit;

    return u32RegisterValue;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429WriteIRIGConfigRegister

   Description:
        This function takes a value formatted in the flexcore format for the
        time tag configuration and sets the corresponding bits in the SFP
        time tag configurartion registers.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32NewValue       - new value for the time tag configuration

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
DDC_LOCAL void ARINC429WriteIRIGConfigRegister
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32NewValue
)
{
    U32BIT u32TempRegisterValue;
    U32BIT u32ResetBit;
    U32BIT u32EnableBit;
    U32BIT u32IntEnableBit;
#if 0

    /* this functionality has been moved to ARINC429SetTimeStampConfig */
    U32BIT u32Format;
    U32BIT u32Resolution;
    U32BIT u32Rollover;
#endif
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER,  "429 - IRIG Config Value: 0x%08X\n",  u32NewValue);

    /* write irig enable value */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);
    u32EnableBit = u32NewValue & OLD_IRIG_ENABLE_MASK;
    u32TempRegisterValue &= ~NEW_IRIG_ENABLE_MASK;
    u32TempRegisterValue |= u32EnableBit;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER,  "429 - IRIG Config ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW,  u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);
#if 0

    /* this functionality has been moved to ARINC429SetTimeStampConfig */
    /* write TT, resolution, rolloever, format */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
    u32TempRegisterValue &= ~(NEW_IRIG_FORMAT_MASK | NEW_IRIG_RESOLUTION_MASK | NEW_IRIG_ROLLOVER_MASK);

    if ((ulNewValue & OLD_IRIG_FORMAT_MASK) == 0x04)
    {
        ulFormat = NEW_IRIG_FORMAT_MASK;
    }
    else
    {
        ulFormat = 0x00;
    }

    u32Resolution = (u32NewValue & OLD_IRIG_RESOLUTION_MASK) >> 8;
    u32Rollover = (u32NewValue & OLD_IRIG_ROLLOVER_MASK) >> 7;
    u32TempRegisterValue |= (u32Format | u32Resolution | u32Rollover);

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - IRIG Config ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG,  u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
#endif

    /* TT reset */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_CTRL_PULSE_REG, &u32TempRegisterValue);
    u32ResetBit = (u32NewValue & OLD_IRIG_RESET_MASK) >> 7;
    u32TempRegisterValue &= ~NEW_IRIG_RESET_MASK;
    u32TempRegisterValue |= u32ResetBit;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER,  "429 - IRIG Config ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_CTRL_PULSE_REG, u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_CTRL_PULSE_REG, &u32TempRegisterValue);

    /* get TT rollover interrupt status */
    DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);
    u32IntEnableBit = (u32NewValue & OLD_IRIG_TT_RO_INT_ENABLE_MASK) << 17;
    u32TempRegisterValue &= ~NEW_IRIG_TT_RO_INT_ENABLE_MASK;
    u32TempRegisterValue |= u32IntEnableBit;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER,  "429 - IRIG Config ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, u32TempRegisterValue);
    DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);
}


/*
    Function:
        getMask

    Description:
        Returns a bitmask to be used when setting the Cast
        serial IO physical mode register. This avoids leaving
        old bits laying around. And makes sure we don't destroy
        good bits!
 */
U32BIT ARINC429getCastIOMask
(
    int bitpos
)
{
    U32BIT u32Mask = INVALID_CAST_IO_MASK;

    switch (bitpos)
    {
        case 0:
        {
            u32Mask = 0xfffffff0;
            break;
        }

        case 4:
        {
            u32Mask = 0xffffff0f;
            break;
        }

        case 8:
        {
            u32Mask = 0xfffff0ff;
            break;
        }

        case 12:
        {
            u32Mask = 0xffff0fff;
            break;
        }

        case 16:
        {
            u32Mask = 0xfff0ffff;
            break;
        }

        case 20:
        {
            u32Mask = 0xff0fffff;
            break;
        }

        case 24:
        {
            u32Mask = 0xf0ffffff;
            break;
        }

        case 28:
        {
            u32Mask = 0x0fffffff;
            break;
        }

        default:
            /* ERROR */
            break;
    }

    return u32Mask;
}

U32BIT isThisAPairedTranceiverDevice
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    switch (pDeviceContext->u16DeviceID)
    {
        case DDC_DEV_ID_BU67118YZX:
        case DDC_DEV_ID_BU67118FMX:
		case DDC_DEV_ID_BU67118M700:
        {
            return 0x01;
        }

        default:
        {
            return 0;
        }
    }
}

#define Max_CAP_OFFSETS             16
#define CHANNELS_PER_CAP_OFFSETS     4
/*-------------------------------------------------------------------------------
   Function:
       CastSerialIOCapChanMatrix

   Description:
        Because of the nature of the CAST serial IO IP and how this driver
        handles logical to physical devices, there is a need to reorganize the
        the serial IO Board addresses in order to make the Logical and
        physical match up according to it's proper usage.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
       0 - not currently useful.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429CastSerialIOCapChanMatrix
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    int x = 0;
    int i = 0;
    U32BIT u32TempRegValue = 0;
    U32BIT u32Temp = 0;
    U32BIT u32DeviceTypeTemp = 0;
    U32BIT uartasyncIndex = 0;
    U32BIT SDLCasyncIndex = 0;
    U32BIT HDLCasyncIndex = 0;
    U32BIT uartIndex = 0;
    U32BIT u32Index = 0;

    /*
        Find all channel capabilities and place
        the corresponding address in the array.
     */
    for (i=0; i < 64; i++)
    {
        memset(&pDeviceContext->castSerialIOChanMatrix[i], 0 , sizeof(CAST_SERIAL_IO_CHAN_MATRIX));
    }

    for (i=0;i<Max_CAP_OFFSETS;i++)
    {
        /* Get Capabilities for each channel */
        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + i, &u32TempRegValue);

        for (x=0; x < CHANNELS_PER_CAP_OFFSETS; x++)
        {
            u32DeviceTypeTemp =  ((u32TempRegValue >> (x * 8)) & 0x000000FF);
            u32Temp = (u32DeviceTypeTemp & 0x0000000f);
            u32DeviceTypeTemp >>= 4;
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "ARINC429CastSerialIOCapChanMatrix - u32Temp: %x u32TempRegValue %x \n", u32Temp, u32TempRegValue );
            u32Index = x + (CHANNELS_PER_CAP_OFFSETS * i);

            switch (u32Temp)
            {
                case CAST_UART_ONLY:
                {
                    if (u32DeviceTypeTemp == CAST_SERIAL_UART)
                    {
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA = pDeviceContext->sMioCastUart429[uartIndex].pu32RegBA;
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegSize = pDeviceContext->sMioCastUart429[uartIndex].pu32RegSize;
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "CAST_SERIAL_UART - reg BA %p, index: %d, uartIndex: %d\n",
                                    (pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA), u32Index, uartIndex);
                        uartIndex++;
                    }
                    else
                    {
                        /* set Async channel to the proper index */
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA = pDeviceContext->sMioCastUartAsync429[uartasyncIndex].pu32RegBA;
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegSize = pDeviceContext->sMioCastUartAsync429[uartasyncIndex].pu32RegSize;
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "CAST_SERIAL_SYNC_ASYNC_UART CAST_SERIAL_UART - reg BA %p, index: %d, uartasyncIndex: %d\n",
                                    (pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA), u32Index, uartasyncIndex);
                        uartasyncIndex++;
                    }

                    break;
                }

                case CAST_SDLC_ONLY:
                case CAST_HDLC_ONLY:
                case CAST_HDLC_SDLC:
                case CAST_HDLC_UART:
                case CAST_SDCL_UART:
                case CAST_HDLC_SDLC_UART:
                {
                    /* set Async channel to the proper index */
                    if ((u32Temp == CAST_HDLC_UART) || (u32Temp == CAST_SDCL_UART) || (u32Temp == CAST_HDLC_SDLC_UART))
                    {
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA = pDeviceContext->sMioCastUartAsync429[uartasyncIndex].pu32RegBA;
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegSize = pDeviceContext->sMioCastUartAsync429[uartasyncIndex].pu32RegSize;
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "ARINC429CastSerialIOCapChanMatrix -  BA %p index: %d, uartasyncIndex: %d\n",
                                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA, u32Index, uartasyncIndex);
                        uartasyncIndex++;
                    }
                    /* set SDLC channel to the proper index */
                    if ((u32Temp == CAST_SDLC_ONLY) || (u32Temp == CAST_SDCL_UART) || (u32Temp == CAST_HDLC_SDLC_UART) || (u32Temp == CAST_HDLC_SDLC))
                    {
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialSDLC.pu32RegBA = pDeviceContext->sMioCastUartSDLC429[SDLCasyncIndex].pu32RegBA;
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialSDLC.pu32RegSize = pDeviceContext->sMioCastUartSDLC429[SDLCasyncIndex].pu32RegSize;
                        SDLCasyncIndex++;
                    }

                    /* set HDLC channel to the proper index */
                    if ((u32Temp == CAST_HDLC_ONLY) || (u32Temp == CAST_HDLC_UART) || (u32Temp == CAST_HDLC_SDLC_UART) || (u32Temp == CAST_HDLC_SDLC))
                    {
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialHDLC.pu32RegBA = pDeviceContext->sMioCastUartHDLC429[HDLCasyncIndex].pu32RegBA;
                        pDeviceContext->castSerialIOChanMatrix[u32Index].serialHDLC.pu32RegSize = pDeviceContext->sMioCastUartHDLC429[HDLCasyncIndex].pu32RegSize;
                        HDLCasyncIndex++;
                    }

                    break;
                }

                default:
                {
                    /* set Undefined UART channels to null */
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegBA =  NULL;
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialAsyncUart.pu32RegSize = 0;
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "ARINC429CastSerialIOCapChanMatrix - reg BA null, index: %d\n", u32Index);

                    /* set Undefined SDLC channel to null */
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialSDLC.pu32RegBA = NULL;
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialSDLC.pu32RegSize = 0;

                    /* set Undefined HDLC channel to null  */
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialHDLC.pu32RegBA = NULL;
                    pDeviceContext->castSerialIOChanMatrix[u32Index].serialHDLC.pu32RegSize = 0;

                    break;
                }
            }

            /* Default each channel to CAST_PROTOCOL_UART mode */
            pDeviceContext->castSerialIOChanMatrix[u32Index].castSerialMode = CAST_PROTOCOL_UART;
        }
    }

    return 0;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CastGetSerialIOConfig

   Description:
       This function sets the configuration information in the global reg. Need to
       support 64 SIO channels.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A register contents.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429CastGetSerialIOConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
)
{
    U32BIT u32TempOFFSET = 0;
    U32BIT bitPostiion = 0;
    U32BIT u16Channel = pPciIoRead->Channel;
    U32BIT u32RegisterValue = 0;
    U32BIT u32tempChan = 0;

    if (UART_PROTOCOL == pPciIoRead->Param2)
    {
        if ((pPciIoRead->Channel > 7) && (pPciIoRead->Channel < 16))
        {
            bitPostiion = u16Channel - 8;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_1;
        }
        else if ((pPciIoRead->Channel > 15) && (pPciIoRead->Channel < 24))
        {
            bitPostiion = u16Channel - 16;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_2;
        }
        else if ((pPciIoRead->Channel > 23) && (pPciIoRead->Channel < 32))
        {
            bitPostiion = u16Channel - 24;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_3;
        }
        else if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 40))
        {
            bitPostiion = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_4;
        }
        else if ((pPciIoRead->Channel > 39) && (pPciIoRead->Channel < 48))
        {
            bitPostiion = u16Channel - 40;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_5;
        }
        else if ((pPciIoRead->Channel > 47) && (pPciIoRead->Channel < 56))
        {
            bitPostiion = u16Channel - 48;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_6;
        }
        else if ((pPciIoRead->Channel > 55) && (pPciIoRead->Channel < 64))
        {
            bitPostiion = u16Channel - 56;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_7;
        }
        else
        {
            bitPostiion = u16Channel;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_0;
        }

        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);

        u32RegisterValue >>= (bitPostiion * 4);

        /* clear the bits we need to write first */
        u32RegisterValue &= 0x0000000f;
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastGetSerialIOConfig - UART_PROTOCOL: u32RegisterValue %x u16Channel %x\n", u32RegisterValue, u16Channel);
    }
    else if (UART_INTERRUPT_ENABLE == pPciIoRead->Param2)
    {
        if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 64))
        {
            bitPostiion = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_1;
        }
        else
        {
            bitPostiion = u16Channel;
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_0;
        }

        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);
       /*u32RegisterValue >>= bitPostiion;
       u32RegisterValue &= 0x00000001;*/
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastGetSerialIOConfig - UART_INTERRUPT_ENABLE: u32RegisterValue %x u16Channel %x\n", u32RegisterValue, u16Channel);
    }
    else if (UART_INTERRUPT_STATUS == pPciIoRead->Param2)
    {
        if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 64))
        {
            bitPostiion = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_STATUS_OFFSET_1;
        }
        else
        {
            bitPostiion = u16Channel;
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_STATUS_OFFSET_0;
        }

        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);
       /*u32RegisterValue >>= bitPostiion;
       u32RegisterValue &= 0x00000001;*/
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastGetSerialIOConfig - UART_INTERRUPT_STATUS: u32RegisterValue %x u16Channel %x\n", u32RegisterValue, u16Channel);
    }
    else if (UART_CAPABILITIES == pPciIoRead->Param2)
    {
        if ((pPciIoRead->Channel > 3) && (pPciIoRead->Channel < 8))
        {
            bitPostiion = u16Channel - 4;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_1;
        }
        else if ((pPciIoRead->Channel > 7) && (pPciIoRead->Channel < 12))
        {
            bitPostiion = u16Channel - 8;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_2;
        }
        else if ((pPciIoRead->Channel > 11) && (pPciIoRead->Channel < 16))
        {
            bitPostiion = u16Channel - 12;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_3;
        }
        else if ((pPciIoRead->Channel > 15) && (pPciIoRead->Channel < 20))
        {
            bitPostiion = u16Channel - 16;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_4;
        }
        else if ((pPciIoRead->Channel > 19) && (pPciIoRead->Channel < 24))
        {
            bitPostiion = u16Channel - 20;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_5;
        }
        else if ((pPciIoRead->Channel > 23) && (pPciIoRead->Channel < 28))
        {
            bitPostiion = u16Channel - 24;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_6;
        }
        else if ((pPciIoRead->Channel > 27) && (pPciIoRead->Channel < 32))
        {
            bitPostiion = u16Channel - 28;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_7;
        }
        else if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 36))
        {
            bitPostiion = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_8;
        }
        else if ((pPciIoRead->Channel > 35) && (pPciIoRead->Channel < 40))
        {
            bitPostiion = u16Channel - 36;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_9;
        }
        else if ((pPciIoRead->Channel > 39) && (pPciIoRead->Channel < 44))
        {
            bitPostiion = u16Channel - 40;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_10;
        }
        else if ((pPciIoRead->Channel > 43) && (pPciIoRead->Channel < 48))
        {
            bitPostiion = u16Channel - 44;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_11;
        }
        else if ((pPciIoRead->Channel > 47) && (pPciIoRead->Channel < 52))
        {
            bitPostiion = u16Channel - 48;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_12;
        }
        else if ((pPciIoRead->Channel > 51) && (pPciIoRead->Channel < 56))
        {
            bitPostiion = u16Channel - 52;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_13;
        }
        else if ((pPciIoRead->Channel > 55) && (pPciIoRead->Channel < 60))
        {
            bitPostiion = u16Channel - 56;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_14;
        }
        else if ((pPciIoRead->Channel > 59) && (pPciIoRead->Channel < 64))
        {
            bitPostiion = u16Channel - 60;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_15;
        }
        else
        {
            bitPostiion = u16Channel;
            u32TempOFFSET = MIO_CAST_IO_CAPABILITIES_OFFSET_0;
        }

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);
        u32RegisterValue >>= (bitPostiion * 8);
        u32RegisterValue &= 0x000000ff;
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastGetSerialIOConfig - UART_CAPABILITIES: Value: 0x%08X\n",u32RegisterValue );
    }
    else if (UART_PHYSICAL == pPciIoRead->Param2)
    {
        if ((pPciIoRead->Channel > 7) && (pPciIoRead->Channel < 16))
        {
            u32tempChan = u16Channel - 8;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_1;
        }
        else if ((pPciIoRead->Channel > 15) && (pPciIoRead->Channel < 24))
        {
            u32tempChan = u16Channel - 16;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_2;
        }
        else if ((pPciIoRead->Channel > 23) && (pPciIoRead->Channel < 32))
        {
            u32tempChan = u16Channel - 24;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_3;
        }
        else if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 40))
        {
            u32tempChan = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_4;
        }
        else if ((pPciIoRead->Channel > 39) && (pPciIoRead->Channel < 48))
        {
            u32tempChan = u16Channel - 40;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_5;
        }
        else if ((pPciIoRead->Channel > 47) && (pPciIoRead->Channel < 56))
        {
            u32tempChan = u16Channel - 48;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_6;
        }
        else if ((pPciIoRead->Channel > 55) && (pPciIoRead->Channel < 64))
        {
            u32tempChan = u16Channel - 56;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_7;
        }
        else
        {
            u32tempChan = u16Channel;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_0;
        }

#if 0   /*
            Not needed all devices should be able to read each physical mode per channel,
            regardless of channel pairing.
        */

        /* set the bits in the correct position depending on the channel */
        /* Must do this because BU67118 Physical registers are paired due to hw transceiver pairs */
        if (u32tempChan % 2)
        {
            u32tempChan -= 1;
        }
#endif

        bitPostiion = (u32tempChan * 4);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);
        /* shift bits down to LSB and clear out all other bits and return value */
        u32RegisterValue >>= bitPostiion;
        u32RegisterValue &= 0x0000000f;
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastGetSerialIOConfig - UART_PHYSICAL: Value: 0x%08X\n", u32RegisterValue);
    }

    return u32RegisterValue;
}

/* R/W Protocol mode */

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CastSetSerialIOConfig

   Description:
       This function sets the configuration information in the global reg.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A register contents.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429CastSetSerialIOConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32TempRegValue = 0;
    U32BIT u32TempReg = 0;
    U32BIT u32TempOFFSET = 0;
    U32BIT bitPostiion = 0;
    U32BIT u16Channel = pIoctlParams->Channel;
    U32BIT u32tempChan = 0;
    U32BIT u32PairedTranceivers = isThisAPairedTranceiverDevice(pDeviceContext);

    if (UART_PROTOCOL == pIoctlParams->Param2)
    {
        if ((pIoctlParams->Channel > 7) && (pIoctlParams->Channel < 16))
        {
            bitPostiion = u16Channel - 8;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_1;
        }
        else if ((pIoctlParams->Channel > 15) && (pIoctlParams->Channel < 24))
        {
            bitPostiion = u16Channel - 16;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_2;
        }
        else if ((pIoctlParams->Channel > 23) && (pIoctlParams->Channel < 32))
        {
            bitPostiion = u16Channel - 24;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_3;
        }
        else if ((pIoctlParams->Channel > 31) && (pIoctlParams->Channel < 40))
        {
            bitPostiion = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_4;
        }
        else if ((pIoctlParams->Channel > 39) && (pIoctlParams->Channel < 48))
        {
            bitPostiion = u16Channel - 40;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_5;
        }
        else if ((pIoctlParams->Channel > 47) && (pIoctlParams->Channel < 56))
        {
            bitPostiion = u16Channel - 48;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_6;
        }
        else if ((pIoctlParams->Channel > 55) && (pIoctlParams->Channel < 64))
        {
            bitPostiion = u16Channel - 56;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_7;
        }
        else
        {
            bitPostiion = u16Channel;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_0;
        }

        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);

        /* make sure all bits are cleared except for what we need */
        u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << (bitPostiion * 4));

        /* clear the bits we need to write first */
        u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion * 4));
        u32TempRegValue  |= u32TempReg;
        DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSetSerialIOConfig UART_PROTOCOL - UARTu32TempRegValue %x, u32TempReg: %x u16Channel %x\n", u32TempRegValue, u32TempReg, u16Channel);

        switch (0x0000000f & pIoctlParams->Param3)
        {
            case CAST_PROTOCOL_SDLC:
            {
                pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode = CAST_PROTOCOL_SDLC;
                break;
            }

            case CAST_PROTOCOL_HDLC:
            {
                pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode = CAST_PROTOCOL_HDLC;
                break;
            }
            default:
            {
                pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode = CAST_PROTOCOL_UART;
                break;
            }
        }
    }
    else if (UART_INTERRUPT_ENABLE == pIoctlParams->Param2)
    {
        /*
            This code makes the assumption that the value passed in
            is set in the correct bit order.
        */
        if ((pIoctlParams->Channel > 31) && (pIoctlParams->Channel < 64))
        {
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_1;
        }
        else
        {
            u32TempOFFSET = MIO_CAST_SERIAL_INTERRUPT_ENABLE_OFFSET_0;
        }

        /* first read the enable */
        DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
        u32TempRegValue |= DDC_IOCTL_U32(pIoctlParams->Param3);
        /* then write the value */
        DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
    }
    else if (UART_PHYSICAL == pIoctlParams->Param2)
    {
        /* Write the Physical Mode to the global registers */
        if ((pIoctlParams->Channel > 7) && (pIoctlParams->Channel < 16))
        {
            u32tempChan = u16Channel - 8;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_1;
        }
        else if ((pIoctlParams->Channel > 15) && (pIoctlParams->Channel < 24))
        {
            u32tempChan = u16Channel - 16;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_2;
        }
        else if ((pIoctlParams->Channel > 23) && (pIoctlParams->Channel < 32))
        {
            u32tempChan = u16Channel - 24;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_3;
        }
        else if ((pIoctlParams->Channel > 31) && (pIoctlParams->Channel < 40))
        {
            u32tempChan = u16Channel - 32;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_4;
        }
        else if ((pIoctlParams->Channel > 39) && (pIoctlParams->Channel < 48))
        {
            u32tempChan = u16Channel - 40;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_5;
        }
        else if ((pIoctlParams->Channel > 47) && (pIoctlParams->Channel < 56))
        {
            u32tempChan = u16Channel - 48;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_6;
        }
        else if ((pIoctlParams->Channel > 55) && (pIoctlParams->Channel < 64))
        {
            u32tempChan = u16Channel - 56;
            u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_7;
        }
        else
        {
            u32tempChan = u16Channel;
            u32TempOFFSET = MIO_CAST_PROTOCOL_MODE_OFFSET_0;
        }

        /*
           To help make life more difficult the BU67118 HW uses paired tranceivers.
           In order to accomodate those paired transceivers we need to write the same
           values to both physical channels.  i.e. ch 0 and ch 1 have same values.
        */
        if (u32PairedTranceivers) /* NEED TO UPDATE THIS - bu67118 transceiver pairing */
        {
            /* Assure we have the lower channel first */
            if (u32tempChan % 2)
            {
                u32tempChan -= 1;
            }

            DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);

            /* Calculate the lower channel # first */
            bitPostiion = (u32tempChan * 4);
            u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);
            u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
            u32TempRegValue  |= u32TempReg;

            /* Calculate the upper channel # */
            bitPostiion += 4;
            u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);
            u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
            u32TempRegValue  |= u32TempReg;

            DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSetSerialIOConfig UART_PHYSICAL- UARTu32TempRegValue %x, u32TempReg: %x\n", u32TempRegValue, u32TempReg);
        }
        else
        {
            bitPostiion = (u32tempChan * 4);
            u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);

            DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);

            /* clear the bits we need to write first */
            /* call ARINC429getCastIOMask so we don't destroy other channel bits */
            u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
            u32TempRegValue  |= u32TempReg;
            DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSetSerialIOConfig UART_PHYSICAL- UARTu32TempRegValue %x, u32TempReg: %x\n", u32TempRegValue, u32TempReg);
        }
    }

    return 0;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CastSerialIORegRead

   Description:
       This function Reads the UART and Gloabal registers.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A register contents.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429CastSerialIORegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
)
{
    U32BIT u32RegisterValue;
    U32BIT u32TempOFFSET = 0;
    U32BIT u16Channel = pPciIoRead->Channel;
    U32BIT bitPostiion = 0;
    U32BIT u32tempChan = 0;

    u32RegisterValue = 0;

    if ((pDeviceContext->u8NumRS232 > 0) ||
        (pDeviceContext->u8NumRS485 > 0) ||
        (pDeviceContext->u8NumUart > 0))
    {
        if (pPciIoRead->Param2 == MIO_UART_IO_CTRL)
        {
            /* Read the Physical Mode from the global registers */
            if ((pPciIoRead->Channel > 7) && (pPciIoRead->Channel < 16))
            {
                u32tempChan = u16Channel - 8;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_1;
            }
            else if ((pPciIoRead->Channel > 15) && (pPciIoRead->Channel < 24))
            {
                u32tempChan = u16Channel - 16;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_2;
            }
            else if ((pPciIoRead->Channel > 23) && (pPciIoRead->Channel < 32))
            {
                u32tempChan = u16Channel - 24;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_3;
            }
            else if ((pPciIoRead->Channel > 31) && (pPciIoRead->Channel < 40))
            {
                u32tempChan = u16Channel - 32;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_4;
            }
            else if ((pPciIoRead->Channel > 39) && (pPciIoRead->Channel < 48))
            {
                u32tempChan = u16Channel - 40;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_5;
            }
            else if ((pPciIoRead->Channel > 47) && (pPciIoRead->Channel < 56))
            {
                u32tempChan = u16Channel - 48;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_6;
            }
            else if ((pPciIoRead->Channel > 55) && (pPciIoRead->Channel < 64)) {
                u32tempChan = u16Channel - 56;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_7;
            }
            else
            {
                u32tempChan = u16Channel;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_0;
            }

#if 0
            /*
                Not needed all devices should be able to read each physical mode per channel,
                regardless of channel pairing.
            */

            /* set the bits in the correct position depending on the channel */
            /* Must do this because BU67118 Physical registers are paired due to hw transceiver pairs */
            if (u32tempChan % 2)
            {
                u32tempChan -= 1;
            }
#endif
            bitPostiion = (u32tempChan * 4);

            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32RegisterValue);

            /* shift bits down to LSB and clear out all other bits and return value */
            u32RegisterValue >>= bitPostiion;
            u32RegisterValue &= 0x0000000f;
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSerialIORegRead MIO_UART_IO_CTRL - UART_PHYSICAL: Value: 0x%08X u16Channel %x\n",u32RegisterValue, u16Channel);

        }
        else
        {
            /* first check to see how this channel is configured */
            switch (pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode)
            {
                case CAST_PROTOCOL_SDLC:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA != NULL)
                    {
                        DDC_REG_READ(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA) + DDC_IOCTL_U32(pPciIoRead->Param2), &u32RegisterValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSerialIORegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA) + (pPciIoRead->Param2), u32RegisterValue);
                    }

                    break;
                }

                case CAST_PROTOCOL_HDLC:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA != NULL)
                    {
                        DDC_REG_READ(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA) + DDC_IOCTL_U32(pPciIoRead->Param2), &u32RegisterValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA) + (pPciIoRead->Param2), u32RegisterValue);
                    }

                    break;
                }

                default:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA != NULL)
                    {
                        DDC_REG_READ(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA) + DDC_IOCTL_U32(pPciIoRead->Param2 & 0x00FF), &u32RegisterValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA) + (pPciIoRead->Param2 & 0x00FF), u32RegisterValue);
                    }
                    break;
                }
            }
        }
    }

    return u32RegisterValue;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CastSerialIORegWrite

   Description:
        This function writes the UART and Gloabal registers.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      void
   ---------------------------------------------------------------------------------*/
void ARINC429CastSerialIORegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32TempRegValue = 0;
    U32BIT u32TempReg = 0;
    U32BIT u32TempOFFSET = 0;
    U32BIT bitPostiion = 0;
    U32BIT u16Channel = pIoctlParams->Channel;
    U32BIT u32tempChan = 0;
    U32BIT u32PairedTranceivers = isThisAPairedTranceiverDevice(pDeviceContext);

    if ((pDeviceContext->u8NumRS232 > 0) ||
        (pDeviceContext->u8NumRS485 > 0) ||
        (pDeviceContext->u8NumUart > 0))
    {
        if (pIoctlParams->Param2 == MIO_UART_IO_CTRL)
        {
            /* Write the Physical Mode from the global registers */
            if ((pIoctlParams->Channel > 7) && (pIoctlParams->Channel < 16))
            {
                u32tempChan = u16Channel - 8;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_1;
            }
            else if ((pIoctlParams->Channel > 15) && (pIoctlParams->Channel < 24))
            {
                u32tempChan = u16Channel - 16;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_2;
            }
            else if ((pIoctlParams->Channel > 23) && (pIoctlParams->Channel < 32))
            {
                u32tempChan = u16Channel - 24;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_3;
            }
            else if ((pIoctlParams->Channel > 31) && (pIoctlParams->Channel < 40))
            {
                u32tempChan = u16Channel - 32;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_4;
            }
            else if ((pIoctlParams->Channel > 39) && (pIoctlParams->Channel < 48))
            {
                u32tempChan = u16Channel - 40;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_5;
            }
            else if ((pIoctlParams->Channel > 47) && (pIoctlParams->Channel < 56))
            {
                u32tempChan = u16Channel - 48;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_6;
            }
            else if ((pIoctlParams->Channel > 55) && (pIoctlParams->Channel < 64))
            {
                u32tempChan = u16Channel - 56;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_7;
            }
            else
            {
                u32tempChan = u16Channel;
                u32TempOFFSET = MIO_CAST_PHYSICAL_MODE_OFFSET_0;
            }

            /*
               To help make life more difficult the BU67118 HW uses paired tranceivers.
               In order to accomodate those paired transceivers we need to write the same
               values to both physical channels. i.e. ch 0 and ch 1 have same values.
            */

            if (u32PairedTranceivers) /* NEED TO UPDATE THIS - bu67118 transceiver pairing */
            {
                /* Assure we have the lower channel first */
                if (u32tempChan % 2)
                {
                    u32tempChan -= 1;
                }

                DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);

                /* Calculate the lower channel # first */
                bitPostiion = (u32tempChan * 4);
                u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);
                u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
                u32TempRegValue  |= u32TempReg;

                /* Calculate the upper channel # */
                bitPostiion += 4;
                u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);
                u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
                u32TempRegValue  |= u32TempReg;

                DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSerialIORegWrite UART_PHYSICAL - UARTu32TempRegValue %x, u32TempReg: %x\n", u32TempRegValue, u32TempReg);
            }
            else
            {
                bitPostiion = (u32tempChan * 4);
                u32TempReg = ((0x0000000f & DDC_IOCTL_U32(pIoctlParams->Param3)) << bitPostiion);

                DDC_REG_READ(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);

                /* clear the bits we need to write first */
                /* call ARINC429getCastIOMask so we don't destroy other channel bits */
                u32TempRegValue &= (ARINC429getCastIOMask(bitPostiion));
                u32TempRegValue  |= u32TempReg;
                DDC_REG_WRITE(pDeviceContext,  *(pDeviceContext->sMioCastUartGlobal429.pu32RegBA) + u32TempOFFSET, &u32TempRegValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429CastSerialIORegWrite UART_PHYSICAL- UARTu32TempRegValue %x, u32TempReg: %x\n", u32TempRegValue, u32TempReg);
            }

            /*pDeviceContext->castSerialIOChanMatrix[u32Index].castSerialMode = CAST_PROTOCOL_UART;*/
        }
        else
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - CAST_PROTOCOL_UART - castSerialMode: %x u16Channel %d  p2 %x p3 %x\n", pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode, u16Channel, pIoctlParams->Param2, pIoctlParams->Param3);

        /* first check to see how this channel is configured */
            switch (pDeviceContext->castSerialIOChanMatrix[u16Channel].castSerialMode)
            {
                case CAST_PROTOCOL_SDLC:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA != NULL)
                    {
                        u32TempRegValue = DDC_IOCTL_U32(pIoctlParams->Param3);
                        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA) + DDC_IOCTL_U32(pIoctlParams->Param2 & 0x00FF), &u32TempRegValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429RegWrite - SDLC: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialSDLC.pu32RegBA) + (pIoctlParams->Param2 & 0x00FF), pIoctlParams->Param3);
                    }

                    break;
                }

                case CAST_PROTOCOL_HDLC:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA != NULL)
                    {
                        u32TempRegValue = DDC_IOCTL_U32(pIoctlParams->Param3);
                        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA) + DDC_IOCTL_U32(pIoctlParams->Param2 & 0x00FF), &u32TempRegValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - ARINC429RegWrite - HDLC: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialHDLC.pu32RegBA) + (pIoctlParams->Param2 & 0x00FF), pIoctlParams->Param3);
                    }

                    break;
                }

                default:
                {
                    if (pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA != NULL)
                    {
                        u32TempRegValue = DDC_IOCTL_U32(pIoctlParams->Param3);
                        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA) + DDC_IOCTL_U32(pIoctlParams->Param2 & 0x00FF), &u32TempRegValue);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_SERIAL_IO, "429 - CAST_PROTOCOL_UART - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->castSerialIOChanMatrix[u16Channel].serialAsyncUart.pu32RegBA) + (pIoctlParams->Param2 & 0x00FF), pIoctlParams->Param3);
                    }

                    break;
                }
            }
        }
    }
}


/*-------------------------------------------------------------------------------
   Function:
       ARINC429RegRead

   Description:
        This function takes a flexcore register address, translates it to SFP,
        and reads the register.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A register contents.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429RegRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
)
{
    U32BIT u32RegisterValue;
    U32BIT u32RegisterAddress;
    u32RegisterValue = 0;

    if ((pPciIoRead->Param1 >= MIO_UART_1) && (pPciIoRead->Param1 <= MIO_UART_IO_CTRL))
    {
        if ((pDeviceContext->u8NumRS232 > 0) ||
            (pDeviceContext->u8NumRS485 > 0) ||
            (pDeviceContext->u8NumUart > 0))
        {
            switch (pPciIoRead->Param1)
            {
                case MIO_UART_1:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_1 & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_1 & 0x00FF), u32RegisterValue);
                    break;
                }

                case MIO_UART_2:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_2 & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_2 & 0x00FF), u32RegisterValue);
                    break;
                }

                case MIO_UART_3:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_3 & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_3 & 0x00FF), u32RegisterValue);
                    break;
                }

                case MIO_UART_4:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_4 & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_4 & 0x00FF), u32RegisterValue);
                    break;
                }

                case MIO_UART_IO_CTRL:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_IO_CTRL & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_IO_CTRL & 0x00FF), u32RegisterValue);
                    break;
                }

                default:
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + DDC_IOCTL_U32(pPciIoRead->Param1 & 0x00FF), &u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (pPciIoRead->Param1 & 0x00FF), u32RegisterValue);
                    break;
                }
            }
        }
        return u32RegisterValue;
    }

    if (((pPciIoRead->Param1 >= MIO_FLASH_ADDR) && (pPciIoRead->Param1 < MIO_429_RX_INTEN_0)) ||
        (pPciIoRead->Param1 == MIO_429_RESET))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead Board Address: 0x%08X\n", pPciIoRead->Param1);

        /* board level registers */
        switch (pPciIoRead->Param1)
        {
            case MIO_CAPABILITIES:
            {
                u32RegisterValue = ddcUdlBdReadCapabilities(pDeviceContext);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Capibilities = 0x%08X\n", u32RegisterValue);
                break;
            }

            case MIO_IRIG_CFG_A:
            {
                u32RegisterValue = ARINC429ReadIRIGConfigRegister(pDeviceContext);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sIrigB_RX->pu32RegBA), u32RegisterValue);
                break;
            }

            case MIO_DISC:
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL, &u32RegisterValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, 0x%08X\n", *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL, u32RegisterValue);
                break;
            }

            case MIO_AVION:
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1, &u32RegisterValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1, u32RegisterValue);
                break;
            }

            case MIO_TT_LSB:
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_LSB_REG, &u32RegisterValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_LSB_REG, u32RegisterValue);
                break;
            }

            case MIO_TT_MSB:
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_MSB_REG, &u32RegisterValue);
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_MSB_REG, u32RegisterValue);
                break;
            }

            case MIO_429_RESET:
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegisterValue);
                u32RegisterValue >>= 14;
                u32RegisterValue &= 0x3;
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, u32RegisterValue);
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else
    {
        /* ARINC 429 Registers */
        u32RegisterAddress = ARINC429GetRegisterAddress(pDeviceContext, pPciIoRead);
        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_REGISTER,  "429 - ARINC429RegRead: Address: 0x%08X, Value: 0x%08X\n", u32RegisterAddress, u32RegisterValue);
    }

    /* KdPrint(("ARINC 429 ARINC429RegRead:0x%08X\n", ulRegisterValue)); */

    return u32RegisterValue;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429RegWrite

   Description:
        This function takes a flexcore register address, translates it to SFP,
        and writes the register.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A register contents.
   ---------------------------------------------------------------------------------*/
void ARINC429RegWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32RegisterValue;
    U32BIT u32RegisterValueBefore;

    u32RegisterValue = (U32BIT)pIoctlParams->Param2;

    if ((pIoctlParams->Param1 >= MIO_UART_1) && (pIoctlParams->Param1 <= MIO_UART_IO_CTRL))
    {
        if ((pDeviceContext->u8NumRS232 > 0) ||
            (pDeviceContext->u8NumRS485 > 0) ||
            (pDeviceContext->u8NumUart > 0))
        {
            switch (pIoctlParams->Param1)
            {
                case MIO_UART_1:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_1 & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_1 & 0x00FF), pIoctlParams->Param2);
                    break;
                }

                case MIO_UART_2:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_2 & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_2 & 0x00FF), pIoctlParams->Param2);
                    break;
                }

                case MIO_UART_3:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_3 & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_3 & 0x00FF), pIoctlParams->Param2);
                    break;
                }

                case MIO_UART_4:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_4 & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_4 & 0x00FF), pIoctlParams->Param2);
                    break;
                }

                case MIO_UART_IO_CTRL:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_IO_CTRL & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (MIO_UART_IO_CTRL & 0x00FF), pIoctlParams->Param2);
                    break;
                }

                default:
                {
                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sMioUart429.pu32RegBA) + DDC_IOCTL_U32(pIoctlParams->Param1 & 0x00FF), (U32BIT *)&u32RegisterValue);
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite - UART: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sMioUart429.pu32RegBA) + (pIoctlParams->Param1 & 0x00FF), pIoctlParams->Param2);
                    break;
                }
            }
        }
    }
    else if (((pIoctlParams->Param1 >= MIO_FLASH_ADDR) && (pIoctlParams->Param1 < START_RX_REG_ADDRESS)) ||
        (pIoctlParams->Param1 == MIO_429_RESET))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Board Address: 0x%08X\n", pIoctlParams->Param1);

        /* board level registers */
        switch (pIoctlParams->Param1)
        {
            case MIO_CAPABILITIES:
            {
                /* Read only Register */
                break;
            }

            case MIO_IRIG_CFG_A:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, pIoctlParams->Param2);
                ARINC429WriteIRIGConfigRegister(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param2));
                break;
            }

            case MIO_DISC:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL, pIoctlParams->Param2);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DIOCTRL, (U32BIT *)&u32RegisterValue);
                break;
            }

            case MIO_AVION:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1,  pIoctlParams->Param2);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AVIONICS_IO_CTRL_1, (U32BIT *)&u32RegisterValue);
                break;
            }

            case MIO_TT_LSB:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n",  *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_LSB_REG,  pIoctlParams->Param2);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_LSB_REG, (U32BIT *)&u32RegisterValue);
                break;
            }

            case MIO_TT_MSB:
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_MSB_REG,  pIoctlParams->Param2);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_MSB_REG, (U32BIT *)&u32RegisterValue);
                break;
            }

            case MIO_429_RESET:
            {
                /* read, modify, write to register */
                u32RegisterValue = DDC_IOCTL_U32(pIoctlParams->Param2);
                u32RegisterValue <<= 14;
                u32RegisterValue &= 0x0000C000;
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegisterValueBefore);
                u32RegisterValueBefore &= 0xFFFF3FFF;
                u32RegisterValueBefore |= u32RegisterValue;
                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 Reset - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, u32RegisterValueBefore);
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegisterValueBefore);
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else
    {
        /* ARINC 429 Registers */
        u32RegisterAddress = ARINC429GetRegisterAddress(pDeviceContext, pIoctlParams);
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_REGISTER, "429 - ARINC429RegWrite Address: 0x%08X, Value: 0x%08X\n", u32RegisterAddress,  pIoctlParams->Param2);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, (U32BIT *)&u32RegisterValue);

        /* Check whether a control command to enable a receive channel has been sent */
        if ((pIoctlParams->Param1 >= MIO_429_RX_CTRL_0) && (pIoctlParams->Param1 <= MIO_429_RX_CTRL_15))
        {
            U32BIT mask = 0x00002000;                 /* reset rx bit*/

            /* (Current register == Rx Control Register) && (Reset Rx bit == 0)*/
            if ((((pIoctlParams->Param1 - START_RX_REG_ADDRESS) % 3) == 2) && (((~pIoctlParams->Param2) & mask) == mask))
            {
                /* Enabele internal interrupts to fill the internal buffer with incoming messages */
                ARINC429ConfigInternalRxInterrupt(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param1 - START_RX_REG_ADDRESS) / 3);
            }
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429MemRead

   Description:
        This function takes a flexcore memory address, translates it to SFP,
        and reads the memory location.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      A memory location contents.
   ---------------------------------------------------------------------------------*/
U32BIT ARINC429MemRead
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pPciIoRead
)
{
    U32BIT u32MemoryAddress;
    U32BIT u32Data = 0;

    u32MemoryAddress = ARINC429GetMemoryAddress(pDeviceContext, pPciIoRead);
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_MEMORY, "429 - ARINC429MemRead: Address: 0x%08X, Value: ", u32MemoryAddress);
    DDC_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);
    /* NOTE: PPC byte swap is done in IOCTL */
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_READ_MEMORY, "0x%08X\n", u32Data);

    return u32Data;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429MemWrite

   Description:
        This function takes a flexcore memory address, translates it to SFP,
        and writes the memory location.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pPciIo            - point to IO command from user

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429MemWrite
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32MemoryAddress;
    U32BIT u32RegisterValue;

    u32MemoryAddress = ARINC429GetMemoryAddress(pDeviceContext, pIoctlParams);
    u32RegisterValue = (U32BIT)pIoctlParams->Param2;
    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_WRITE_MEMORY, "429 - ARINC429MemWrite Address: 0x%08X, Value: 0x%08X\n", u32MemoryAddress, pIoctlParams->Param2);
    /* NOTE: PPC byte swap is done in IOCTL */
    DDC_MEM_WRITE(pDeviceContext, u32MemoryAddress, (U32BIT *)(&u32RegisterValue), ACEX_32_BIT_ACCESS);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429EnableRxInterrupts

   Description:
        This function turns on the interrupts for ARINC 429

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429EnableRxInterrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegData;

    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M))
    {
        U8BIT u16Channel;

        /* DD-40xxx devices */

        /* enable on interrupts on all channels */
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);
        u32RegData |= ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);
        u32RegData |= ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_2_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);

        for (u16Channel = 0; u16Channel < pDeviceContext->u8NumProg429RxTx; u16Channel++)
        {
            pDeviceContext->pTxChnl429[u16Channel]->u16ARINC429SetTxFrameControlEventCond = 0;
            pDeviceContext->pRxChnl429[u16Channel]->u16RxFifoEventCond = 0;
        }

        pDeviceContext->u16Arinc429VoltageMonitoringEventCond = 0;
    }
    else
    {
        /* SFP/MF devices */

        /* enable Arinc interrupt */
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_EN, &u32RegData);
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INTERRUPT, "ARINC429EnableRxInterrupts: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_EN, u32RegData);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegData);
        u32RegData |= 0x00008000;
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INTERRUPT, "ARINC429EnableRxInterrupts: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, u32RegData);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegData);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);
        u32RegData |= ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_MASK; /* was 0x000000FF*/
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INTERRUPT, "ARINC429EnableRxInterrupts: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, u32RegData);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);
        u32RegData |= ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_2_MASK;
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_INTERRUPT, "ARINC429EnableRxInterrupts: Address: 0x%08X, Value: 0x%08X\n", *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, u32RegData);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);
    }

    /* set interrupt enable register */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_ARINC_0);

    /* enable master interrupt */
    ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_INT_REQ);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429DisableRxInterrupts

   Description:
        This function turns off the interrupts for ARINC 429

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429DisableRxInterrupts
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegData = 0;

    /* Disable Arinc interrupt */
    ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_ARINC_0);

    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M))
    {
        /* DD-40xxx devices */

        /* disable on interrupts on all channels */
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);
        u32RegData &= ~ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);
        u32RegData &= ~ACEX_429_RX_GLOBAL_INT_ENABLE_ALL_CHANNELS_2_MASK;
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_2_REG, &u32RegData);
    }
    else
    {
        /* SFP/MF devices */

        /* clear Arinc RX interrupts */
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegData);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegData);
    }
}

/*===================================================================================
                    New Functions in 3.3.3, ported for DD-40000 devices
   =====================================================================================*/

/*******************************************************************************
 * Name:    ARINC429ProcessCommand
 *
 * Description:
 *
 *      This function parses the command.
 *
 * In:  pDeviceContext      device-specific structure
 * In:  pIoctlParams        ->Channel   Channel to operate on (N/A for board commands)
 *                          ->Param1    Command
 *                          ->Param2    Command specific value
 *                          ->Param3    Command specific value
 *                          ->Param4    Command specific value
 * In:  pLocalInOutBuffer   pointer to dynamic allocated output data
 * In:  OutputBufferLength  length of output buffer in bytes
 * Out: pBytesReturned      pointer to number of bytes retuned in the output buffer
 * In:  pwdfRequest         pointer to WDF request object
 * Out: pbRequestPending    pointer to the request pending status
 * Out: error value
 ******************************************************************************/
S16BIT ARINC429ProcessCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u32Command = DDC_IOCTL_U32(pIoctlParams->Param1);

    /* default to no bytes returned */
    *pBytesReturned = 0;

    if ((u32Command >= DD429_GENERAL_COMMAND__INDEX_START) &&
        (u32Command <= DD429_GENERAL_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - General Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429GeneralCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_CONTROL_COMMAND__INDEX_START) &&
        (u32Command <= DD429_CONTROL_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - Control Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429ControlCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_FIFO_COMMAND__INDEX_START) &&
        (u32Command <= DD429_FIFO_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - FIFO Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429FifoCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_RX_COMMAND__INDEX_START) &&
        (u32Command <= DD429_RX_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - RX Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429RxCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_TX_COMMAND__INDEX_START) &&
        (u32Command <= DD429_TX_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - Tx Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429TxCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_RX_CONTROL_COMMAND_SET__INDEX_START) &&
        (u32Command <= DD429_RX_CONTROL_COMMAND_SET__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - RX Control Command Set %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429RxControlCommandSet(pDeviceContext, pIoctlParams);
    }
    else if ((u32Command >= DD429_RX_CONTROL_COMMAND_GET__INDEX_START) &&
        (u32Command <= DD429_RX_CONTROL_COMMAND_GET__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - RX Control Command Get %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429RxControlCommandGet(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_FILTER_COMMAND__INDEX_START) &&
        (u32Command <= DD429_FILTER_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - Filter Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429FilterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_MAILBOX_COMMAND__INDEX_START) &&
        (u32Command <= DD429_MAILBOX_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - mailbox Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429MailboxCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else if ((u32Command >= DD429_TESTER_COMMAND__INDEX_START) &&
        (u32Command <= DD429_TESTER_COMMAND__INDEX_END))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - tester Command %d\n", pIoctlParams->Channel, pIoctlParams->Param1);
        status = _ARINC429TesterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
    }
    else
    {
        /* ERROR: unknown command */
        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_PROCESS, "429 - ARINC429ProcessCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);

        status = ERR_FEATURE_NOT_SUPPORTED;
    }

    return status;
}

/* ============================================================================ */
/* ============================================================================ */

/*******************************************************************************
 * Name:    _RxControlCommandGetValue
 *
 * Description:
 *
 *      This function retrieves the RX control command value.
 *
 * In:  pDeviceContext
 * In:  u16Channel
 * In:  u32Command
 * Out: return              value
 ******************************************************************************/
static U32BIT _RxControlCommandGetValue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32Command
)
{
    DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE sRxControlCommandGetOutput;
    DDC_IOCTL_PARAMS tempIoctlParams;
    size_t bytesReturned = 0; /* not used */

    /* initialize value */
    sRxControlCommandGetOutput.u32Command = 0;

    tempIoctlParams.Channel = u16Channel;
    tempIoctlParams.Param1 = u32Command;
    _ARINC429RxControlCommandGet(pDeviceContext, &tempIoctlParams, (U32BIT *) &sRxControlCommandGetOutput, sizeof(&sRxControlCommandGetOutput), &bytesReturned);

    return sRxControlCommandGetOutput.u32Command;
}

/*******************************************************************************
 * Name:    _ConvertTesterOptionsToControlWord
 *
 * Description:
 *
 *      This function converts a tester options structure into a 32-bit register
 *      value.
 *
 * In:  psTesterOptions     pointer to tester values
 * Out: return              converted value
 ******************************************************************************/
static U32BIT _ConvertTesterOptionsToControlWord
(
    DD429_TESTER_OPTIONS_TYPE *psTesterOptions
)
{
    U32BIT u32TesterOptions = 0x00000000;

    u32TesterOptions |= (psTesterOptions->s16InterWordBitGapError << DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_OFFSET) & DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_MASK;
    u32TesterOptions |= (psTesterOptions->u8WordSizeError << DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_OFFSET) & DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_MASK;
    u32TesterOptions |= (psTesterOptions->u8ParityError << DD429_TX_CONTROL_WORD__PARITY_ERROR_OFFSET) & DD429_TX_CONTROL_WORD__PARITY_ERROR_MASK;
    u32TesterOptions |= (psTesterOptions->u8Bit33 << DD429_TX_CONTROL_WORD__33RD_BIT_OFFSET) & DD429_TX_CONTROL_WORD__33RD_BIT_MASK;

    return u32TesterOptions;
}

/*******************************************************************************
 * Name:    _ConvertControlWordToTesterOptions
 *
 * Description:
 *
 *      This function converts a 32-bit control word to a tester options structue.
 *
 * In:  psTesterOptions     pointer to tester values
 * Out: return              converted value
 ******************************************************************************/
static void _ConvertControlWordToTesterOptions
(
    U32BIT u32ControlWord,
    DD429_TESTER_OPTIONS_TYPE *psTesterOptions
)
{
    if (psTesterOptions != NULL)
    {
        psTesterOptions->s16InterWordBitGapError = (U16BIT)((u32ControlWord & DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_MASK) >> DD429_TX_CONTROL_WORD__INTER_WORD_BIT_GAP_ERROR_OFFSET);
        psTesterOptions->u8WordSizeError = (U8BIT)((u32ControlWord & DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_MASK) >> DD429_TX_CONTROL_WORD__WORD_SIZE_ERROR_OFFSET);
        psTesterOptions->u8ParityError = (U8BIT)((u32ControlWord & DD429_TX_CONTROL_WORD__PARITY_ERROR_MASK) >> DD429_TX_CONTROL_WORD__PARITY_ERROR_OFFSET);
        psTesterOptions->u8Bit33 = (U8BIT)((u32ControlWord & DD429_TX_CONTROL_WORD__33RD_BIT_MASK) >> DD429_TX_CONTROL_WORD__33RD_BIT_OFFSET);
    }
}

/*******************************************************************************
 * Name:    _ConvertSpeedToTableIndex
 *
 * Description:
 *
 *      This function converts a speed into a 32-bit table index value and
 *      returns the actual speed used.
 *
 * In:  pu32Speed       Input speed
 * Out: pu32Speed       Actual speed used
 * Out: return          Table index
 ******************************************************************************/
static U32BIT _ConvertSpeedToTableIndex
(
    U32BIT *pu32Speed
)
{
    U32BIT u32TableIndex;
    U32BIT u32ClosestestSpeed = *pu32Speed;

    if (*pu32Speed == DD429_CHANNEL_SPEED_BPS_DEFAULT)
    {
        u32TableIndex = DD429_VARIABLE_SPEED__TABLE_INDEX_DEFAULT;

        u32ClosestestSpeed = 0;
    }
    else if (*pu32Speed <= DD429_VARIABLE_SPEED__LOWER_TABLE_BPS_END)
    {
        /* see if the speed is too low */
        if (*pu32Speed < DD429_VARIABLE_SPEED__LOWER_TABLE_BPS_START)
        {
            u32ClosestestSpeed = DD429_VARIABLE_SPEED__LOWER_TABLE_BPS_START;
        }
        else
        {
            /* round up to the nearest hundred */
            *pu32Speed += 50;
            u32ClosestestSpeed = (U32BIT) (*pu32Speed / 100);
            u32ClosestestSpeed *= 100;
        }

        u32TableIndex = u32ClosestestSpeed / DD429_VARIABLE_SPEED__LOWER_TABLE_STEP_SIZE;
    }
    else
    {
        /* see if the speed is too high */
        if (*pu32Speed > DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_END)
        {
            u32ClosestestSpeed = DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_END;
        }
        else
        {
            /* round up to the nearest thousand */
            *pu32Speed += 500;
            u32ClosestestSpeed = (U32BIT) (*pu32Speed / 1000);
            u32ClosestestSpeed *= 1000;
        }

        u32TableIndex = ((u32ClosestestSpeed - DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_START) / DD429_VARIABLE_SPEED__UPPER_TABLE_STEP_SIZE)
            + DD429_VARIABLE_SPEED__UPPPER_TABLE_INDEX_START;
    }

    *pu32Speed = u32ClosestestSpeed;

    return u32TableIndex;
}

/*******************************************************************************
 * Name:    _ConvertTableIndexToSpeed
 *
 * Description:
 *
 *      This function converts a 32-bit table index value into a speed value.
 *
 * In:  u32TableIndex   Table index
 * Out: speed
 ******************************************************************************/
static U32BIT _ConvertTableIndexToSpeed
(
    U32BIT u32TableIndex
)
{
    U32BIT u32Speed;

    if (u32TableIndex < DD429_VARIABLE_SPEED__LOWER_TABLE_INDEX_START)
    {
        u32Speed = DD429_VARIABLE_SPEED__BPS_DEFAULT;
    }
    else if (u32TableIndex <= DD429_VARIABLE_SPEED__LOWER_TABLE_INDEX_END)
    {
        u32Speed = u32TableIndex * DD429_VARIABLE_SPEED__LOWER_TABLE_STEP_SIZE;
    }
    else if (u32TableIndex <= DD429_VARIABLE_SPEED__UPPER_TABLE_INDEX_END)
    {
        u32Speed = ((u32TableIndex - DD429_VARIABLE_SPEED__UPPPER_TABLE_INDEX_START) * DD429_VARIABLE_SPEED__UPPER_TABLE_STEP_SIZE) + DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_START;
    }
    else
    {
        u32Speed = ((DD429_VARIABLE_SPEED__UPPER_TABLE_INDEX_END - DD429_VARIABLE_SPEED__UPPPER_TABLE_INDEX_START) * DD429_VARIABLE_SPEED__UPPER_TABLE_STEP_SIZE) + DD429_VARIABLE_SPEED__UPPER_TABLE_BPS_START;
    }

    return u32Speed;
}

/*******************************************************************************
 * Name:    _ARINC429SetLoopbackMapping
 *
 * Description:
 *
 *      This function programs the receiver to which transmitter it will be connected.
 *      to when in internal loopback.
 *
 * In:  pDeviceContext      device-specific structure
 * In:  u32Rx               receiver number (0 based)
 * In:  u32Tx               transmitter number (0 based)
 * In:  u8Clear             1=clear mapping setting
 * Out: none
 ******************************************************************************/
static void _ARINC429SetLoopbackMapping
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Rx,
    U32BIT u32Tx,
    U8BIT u8Clear
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32Value;

    if (u32Tx < DD429_MAX_CHANNELS_PER_REGISTER)
    {
        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u32Rx]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
    }
    else
    {
        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u32Rx]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_2_REG;
    }

    if (u8Clear == TRUE)
    {
        u32Value = 0x00000000;
    }
    else
    {
        /* set bit mask of TX channel to wrap around from */
        u32Value = LOOPBACK_MAPPING_MASK[u32Tx];
    }

    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32Value);
}

/*******************************************************************************
 * Name:    _ARINC429GetLoopbackMapping
 *
 * Description:
 *
 *      This function returns the transmitter number that the receiver is
 *      programmed to be connected to when in internal loopback.
 *
 * In:  pDeviceContext      device-specific structure
 * In:  u32Rx               the receiver number (0 based)
 * Out: returns transmitter number
 ******************************************************************************/
static U32BIT _ARINC429GetLoopbackMapping
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Rx
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32Value = 0;
    U32BIT u16ChannelOffset = 0;
    U32BIT u32TxChannel = 0;

    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u32Rx]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32Value);

    /* if the TX is not mapped in this register, try the other */
    if (u32Value == 0x00000000)
    {
        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u32Rx]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_2_REG;
        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32Value);
        u16ChannelOffset = DD429_MAX_CHANNELS_PER_REGISTER;
    }

    if (u32Value != 0x00000000)
    {
        /* convert - bit position (1 based) is TX channel # */
        while (u32Value)
        {
            u32TxChannel++;
            u32Value >>= 1;
        }
    }

    return (u32TxChannel + u16ChannelOffset);
}

/*******************************************************************************
 * Name:    _ARINC429SendAsync
 *
 * Description:
 *
 *      This function loads one 32-bit TX data word. The data word will not be
 *      loaded if the board temp FIFO or the TX channel FIFO are full after a
 *      specified amount of time
 *
 * In:  pDeviceContext          device context
 * In:  u32TxData               data word to transmit
 * In:  u32Control              control word options
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429SendAsync
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32TxData,
    U32BIT u32Control
)
{
    U32BIT u32RegisterAddress;
    U32BIT u32ClockTimeEnd;
    U32BIT u32ClockTimeNow;

    U32BIT u32RegisterValue = 0;
    S16BIT s16Result;

    s16Result = _ARINC429TxSetStateBusy(pDeviceContext);
    if (s16Result != DDC_UDL_ERROR__SUCCESS)
    {
        return s16Result;
    }

    u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

    if (u32RegisterValue & DD429_TX_CONTROL_MASK__RESET)
    {
        /* ERROR: channel is in reset */
        ARINC429TxSetStateIdle(pDeviceContext);
        return ERR_ENABLE;
    }

    if (!pDeviceContext->bExtendedScheduler)
    {
        u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_CHANNEL_SELECT_REG);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u16Channel);

        /* loop while board FIFO is full */
        u32ClockTimeEnd = ddcUdlOsGetClockMs() + LOAD_TX_QUEUE_FIFO_TIMEOUT;
        do
        {
            /* read the FIFO status register */
            u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_TEMP_MESSAGE_FIFO_STATUS_REG);
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /*check if timeout is reached*/
            u32ClockTimeNow = ddcUdlOsGetClockMs();
            if (u32ClockTimeNow >= u32ClockTimeEnd)
            {
                ARINC429TxSetStateIdle(pDeviceContext);
                return DDC_UDL_ERROR__TIMEOUT;
            }
        } while (u32RegisterValue & DD429_TX_FIFO_MASK__FULL);

        /* loop while the TX channels FIFO is FULL */
        u32ClockTimeEnd = ddcUdlOsGetClockMs() + LOAD_TX_QUEUE_FIFO_TIMEOUT;
        do
        {
            /* read the message FIFO status register */
            u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_0_REG + u16Channel);
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /*check if timeout is reached*/
            u32ClockTimeNow = ddcUdlOsGetClockMs();
            if (u32ClockTimeNow >= u32ClockTimeEnd)
            {
                ARINC429TxSetStateIdle(pDeviceContext);
                return DDC_UDL_ERROR__TIMEOUT;
            }
        } while (u32RegisterValue & DD429_TX_FIFO_MASK__FULL);

        /* must write control word first */
        u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_FIFO_CONTROL_WORD_REG);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32Control);

        /* write data */
        u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_FIFO_REG);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TxData);
    }
    else
    {
        /* loop while FIFO is full */
        u32ClockTimeEnd = ddcUdlOsGetClockMs() + LOAD_ASYNC_FIFO_TIMEOUT;
        do
        {
            /* read the FIFO status register */
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + (ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_REG);
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /*check if timeout is reached*/
            u32ClockTimeNow = ddcUdlOsGetClockMs();
            if (u32ClockTimeNow >= u32ClockTimeEnd)
            {
                break;
            }
        } while (u32RegisterValue & ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_HI_PRI_FULL_MASK);

        /* write the data */
        u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_HI_PRIORITY_DATA_INPUT_REG;
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TxData);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32Control);
    }

    ARINC429TxSetStateIdle(pDeviceContext);

    return DDC_UDL_ERROR__SUCCESS;
}

/* ============================================================================ */
/* ============================================================================ */
/* COMMAND PROCESSING FUNCTIONS                                                 */
/* ============================================================================ */
/* ============================================================================ */

/*******************************************************************************
 * Name:    _ARINC429RxCommand
 *
 * Description:
 *
 *      This function processes DD429_RX_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429RxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    U32BIT u32TempRegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U32BIT u16Channel = pIoctlParams->Channel;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_RX, "429 - _ARINC429RxCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__ENABLE_RX:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;
            ARINC_429_PROGRMMABLE_CONFIG sConfig;
            U32BIT u32EnableValue = DDC_IOCTL_U32(pIoctlParams->Param2);
            U32BIT u32PassiveValue = DDC_IOCTL_U32(pIoctlParams->Param3);

            sConfig.u32ConfigOption = ARINC_429_PROGRMMABLE_TYPE_OPT;
            sConfig.bEnable = (U8BIT)u32EnableValue;
            sConfig.sConfigID.u16Channel = (U16BIT)u16Channel;

            if (u32PassiveValue == FALSE)
            {
                sConfig.u32ConfigOption |= ARINC_429_PROGRMMABLE_ENABLE_OPT;
            }

            /* If disabling channel, force channel to Undefined */
            if (u32EnableValue == FALSE)
            {
                sConfig.bType = ARINC429_UNDEFINED_CHANNEL;
            }
            else
            {
                sConfig.bType = ARINC429_RX_CHANNEL;
            }

            /* put RX into reset */
            tempIoctlParams.Channel = u16Channel;
            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__RESET;
            tempIoctlParams.Param2 = DD429_ENABLE;
            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            if (u32EnableValue)
            {
                /* take RX out of reset */
                tempIoctlParams.Channel = u16Channel;
                tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__RESET;
                tempIoctlParams.Param2 = DD429_DISABLE;
                _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

                /* need to set type and connection to bus */
                status = ARINC429ProgrammableConfig(pDeviceContext, &sConfig);
                if (status)
                {
                    ARINC429OnRxHostBufferChannelEnabled(pDeviceContext, u16Channel);
                    break;
                }
            }
            else
            {
                /* need to disconnect channel from bus */
                status = ARINC429ProgrammableConfig(pDeviceContext, &sConfig);
                if (status)
                {
                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__RESET_TIMESTAMP:
        /* ---------------------------------------------------- */
        {
            U32BIT u32RegisterAddress;

            u32RegisterAddress = *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_CTRL_PULSE_REG;

            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);
            u32TempRegisterValue |= DD429_IRIG_MASK__RESET;
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);

            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);
            u32TempRegisterValue &= ~(DD429_IRIG_MASK__RESET);
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__CONFIG_TIMESTAMP:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Format = pIoctlParams->Param2;
            U32BIT u32Rollover = pIoctlParams->Param3;
            U32BIT u32Resolution = pIoctlParams->Param4;
            S32BIT *pResult = (S32BIT *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*pResult))
            {
                *pResult = ERR_SUCCESS;
                *pBytesReturned = sizeof(*pResult);
            }

            if  (u32Format == IRIG_B_ENHANCED)
            {
                /* check if this device supports IRIG-B enhanced format */
                if ((pDeviceContext->sHwVersionInfo.dwCapabilities2 & HWVER_CAPABILITY2_IRIG_B_ENHANCED) == 0)
                {
                    if (OutputBufferLength >= sizeof(*pResult))
                    {
                        *pResult = ERR_FEATURE_NOT_SUPPORTED;
                        *pBytesReturned = sizeof(*pResult);
                    }
                    break;
                }
            }

            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);
            u32TempRegisterValue |= IRIGB_CONTROL_CLOCK_ENABLE;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);

            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);

            /* clear out existing values */
            u32TempRegisterValue &= ~(ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_RESOLUTION_MASK |
                ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_MASK |
                ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_TIMETAG_ENA_MASK);

            /* set the Timetag Resolution */
            u32TempRegisterValue |= (u32Resolution & 0x0000000F);

            /* set the Timetag Rollover */
            u32TempRegisterValue |= ((u32Rollover << ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_OFFSET) & ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_MASK);

            /*
             *  Set the Timetag Format
             *  0 = Internal relative 48 bit timer
             *  2 = IRIG-B timetag
             *  5 = IRIG-B Enhanced timetag
             */
            if (u32Format != 0)
            {
                /* clear out IRIG-B Enhanced value first */
                u32TempRegisterValue &= ~(ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_ENHANCED_TIMETAG_ENA_MASK);

                if (u32Format == IRIG_B)
                {
                    u32TempRegisterValue |= ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_TIMETAG_ENA_MASK;
                }
                else if (u32Format == IRIG_B_ENHANCED)
                {
                    u32TempRegisterValue |= ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_ENHANCED_TIMETAG_ENA_MASK;
                }
            }

            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__GET_TIMESTAMP:
        /* ---------------------------------------------------- */
        {
            DD429_GET_TIMESTAMP__OUTPUT_TYPE *psTimeStampValue = (DD429_GET_TIMESTAMP__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*psTimeStampValue))
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_LSB_REG, &psTimeStampValue->u32Lsb);
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_TT_MSB_REG, &psTimeStampValue->u32Msb);

                *pBytesReturned = sizeof(*psTimeStampValue);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__GET_TIMESTAMP_CONFIG:
        /* ---------------------------------------------------- */
        {
            DD429_GET_TIME_STAMP_CONFIG__OUTPUT_TYPE *psGetTimeStampConfigOutput = (DD429_GET_TIME_STAMP_CONFIG__OUTPUT_TYPE *) pLocalInOutBuffer;

            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);

            if (OutputBufferLength >= sizeof(*psGetTimeStampConfigOutput))
            {
                psGetTimeStampConfigOutput->u32Resolution = (u32TempRegisterValue & ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_RESOLUTION_MASK);
                psGetTimeStampConfigOutput->u32Rollover = ((u32TempRegisterValue & ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_MASK) >> ACEX_429_RX_GLOBAL_GEN_CTRL_TIMETAG_ROLLOVER_OFFSET);

                if ((u32TempRegisterValue & ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_TIMETAG_ENA_MASK) != 0)
                {
                    psGetTimeStampConfigOutput->u32Format = IRIG_B;
                }
                else
                {
                    psGetTimeStampConfigOutput->u32Format = TT48;
                }

                *pBytesReturned = sizeof(*psGetTimeStampConfigOutput);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__GET_LOOPBACK_MAPPING:
        /* ---------------------------------------------------- */
        {
            *pLocalInOutBuffer = _ARINC429GetLoopbackMapping(pDeviceContext, pIoctlParams->Channel);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__SET_LOOPBACK_MAPPING:
        /* ---------------------------------------------------- */
        {
            _ARINC429SetLoopbackMapping(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param2), DDC_IOCTL_U32(pIoctlParams->Param3), FALSE);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_COMMAND__SET_INTERRUPT_CONDITION:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Type = DDC_IOCTL_U32(pIoctlParams->Param2);
            U32BIT u32Condition = DDC_IOCTL_U32(pIoctlParams->Param3);
            U32BIT u32RegisterAddress;

            switch (u32Type)
            {
                /* --------------------- */
                case CHAN_TYPE_429:
                /* --------------------- */
                {
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_INT_ENABLE_REG;

                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);

                    if (u32Condition > 0)
                    {
                        u32TempRegisterValue |= u32Condition;
                    }
                    else
                    {
                        u32TempRegisterValue = 0; /* clear out conditions */
                    }

                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_SDLC:
                case CHAN_TYPE_HDLC:
                case CHAN_TYPE_UART:
                case CHAN_TYPE_232:
                case CHAN_TYPE_485:
                /* --------------------- */
                {
                    DDC_IOCTL_PARAMS sIoctlParams;
                    sIoctlParams.Channel = u16Channel;
                    sIoctlParams.Param2 = UART_INTERRUPT_ENABLE;

                    /* If any of the four UARTs are being enabled, enable the board level interrupt for the UART block */
                    if (u32Condition)
                    {
                        if (u16Channel < 32)
                        {
                            sIoctlParams.Param3 = 1 << u16Channel;
                        }
                        else
                        {
                            sIoctlParams.Param3 = 1 << (u16Channel - 32);
                        }

                        /* Open and create host buffer */
                        /* might_sleep();*/
                        serial_ioOpen(pDeviceContext, (U8BIT)u16Channel);

                        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_UART);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_RX, "429 - _ARINC429RxCommand bdInterruptSet sIoctlParams.Param3 %x  channel %x\n", sIoctlParams.Param3, u16Channel);
                    }
                    else
                    {
                        if (u16Channel < 32)
                        {
                            sIoctlParams.Param3 &= ~(1 << u16Channel);
                        }
                        else
                        {
                            sIoctlParams.Param3 &= ~(1 << (u16Channel - 32));
                        }

                        /* Close host buffer */
                        serial_ioClose(pDeviceContext,(U8BIT)u16Channel);

                        ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_UART);
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_RX, "429 - _ARINC429RxCommand bdInterruptClear sIoctlParams.Param3 %x  channel %x\n", sIoctlParams.Param3, u16Channel);
                    }

                    ARINC429CastSetSerialIOConfig(pDeviceContext, &sIoctlParams);
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_TT:
                /* --------------------- */
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);

                    if (u32Condition == DD429_ENABLE)
                    {
                        u32TempRegisterValue |= ACEX_429_RX_GLOBAL_INT_ENABLE_IRIG_TT_RO_MASK;
                    }
                    else
                    {
                        u32TempRegisterValue &= ~(ACEX_429_RX_GLOBAL_INT_ENABLE_IRIG_TT_RO_MASK);
                    }

                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_VOLT_MON:
                /* --------------------- */
                {
                    if (u32Condition == DD429_ENABLE)
                    {
                        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT);
                    }
                    else
                    {
                        ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT);
                    }

                    break;
                }

                /* --------------------- */
                default:
                /* --------------------- */
                {
                    /* ERROR: unsupported channel type */
                    status = ERR_IRQ;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_RX, "429 - _ARINC429RxCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429RxControlCommandSet
 *
 * Description:
 *
 *      This function processes DD429_RX_CONTROL_COMMAND_SET__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429RxControlCommandSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32SettingType;
    U32BIT u32SettingValue;
    U32BIT u32MaskOffset;
    U32BIT u16Channel;

    U32BIT u32RegisterAddress = 0;
    U32BIT u32RegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;
    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;

    /* read the RX Control register */
    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

    u32SettingType = DDC_IOCTL_U32(pIoctlParams->Param1);
    u32SettingValue = DDC_IOCTL_U32(pIoctlParams->Param2);

    switch (u32SettingType)
    {
        /* ---------------------------------------------------- */
        case DD429_RX_CONTROL_COMMAND_SET__PARITY:
        /* ---------------------------------------------------- */
        {
            /* clear out setting first and default to odd parity */
            u32RegisterValue &= ~(DD429_RX_CONTROL_MASK__PARITY);

            if (u32SettingValue == DD429_EVEN_PARITY)
            {
                u32RegisterValue |= DD429_RX_PARITY_MASK__EVEN;
            }
            else if (u32SettingValue == DD429_NO_PARITY)
            {
                u32RegisterValue |= DD429_RX_PARITY_MASK__NO;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_RX_CONTROL_COMMAND_SET__SPEED:
        /* ---------------------------------------------------- */
        {
            switch (u32SettingValue)
            {
                case DD429_ARINC575:
                {
                    /* set to ARINC575 and low speed */
                    u32RegisterValue |= DD429_RX_CONTROL_MASK__ARINC575;
                    u32RegisterValue |= DD429_RX_CONTROL_MASK__SPEED;
                    break;
                }

                case DD429_LOW_SPEED:
                {
                    /* set to low speed and ARINC429 */
                    u32RegisterValue |= DD429_RX_CONTROL_MASK__SPEED;
                    u32RegisterValue &= ~(DD429_RX_CONTROL_MASK__ARINC575);
                    break;
                }

                case DD429_HIGH_SPEED:
                {
                    /* set to high speed and ARINC429 */
                    u32RegisterValue &= ~(DD429_RX_CONTROL_MASK__SPEED);
                    u32RegisterValue &= ~(DD429_RX_CONTROL_MASK__ARINC575);
                    break;
                }
            }

            ARINC429OnRxHostBufferChannelSpeedChanged(pDeviceContext, u16Channel);

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            u32MaskOffset = u32SettingType - DD429_RX_CONTROL_COMMAND_SET__INDEX_START;

            /* make sure an offset containing a placeholder was not selected */
            if (dd429RxControlMaskTable[u32MaskOffset] != DD429_RX_CONTROL_MASK__PLACEHOLDER)
            {
                /* clear out existing setting first */
                u32RegisterValue &= ~(dd429RxControlMaskTable[u32MaskOffset]);

                /* set the value, making sure to mask off the value to prevent overwriting of other bits */
                u32RegisterValue |= (u32SettingValue << u32MaskOffset) & dd429RxControlMaskTable[u32MaskOffset];
            }

            break;
        }
    }

    /* write the RX Control register */
    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429RxControlCommandGet
 *
 * Description:
 *
 *      This function gets the specified item (defined in IOCTL command param1)
 *      from the    RX Control Register.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  OutputBufferLength      length of output buffer in bytes
 * Out: pLocalInOutBuffer                 value of command item
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  error condition
 ******************************************************************************/
static S16BIT _ARINC429RxControlCommandGet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE *psRxControlCommandGetOutput = (DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE *) pLocalInOutBuffer;
    U32BIT u32RegisterAddress = 0;
    U32BIT u32BitOffset;
    U32BIT u32RegisterValue;
    U32BIT u16Channel;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;
    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;

    if (OutputBufferLength >= sizeof(U32BIT))
    {
        /* read the RX Control register */
        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

        u16Channel = pIoctlParams->Channel;
        u32BitOffset = DDC_IOCTL_U32(pIoctlParams->Param1 - DD429_RX_CONTROL_COMMAND_GET__INDEX_START);

        if ((u32BitOffset < DD429_RX_CONTROL_BIT_OFFSET_MAX) && (OutputBufferLength >= sizeof(U32BIT)))
        {
            /* mask off value, then shift right */
            psRxControlCommandGetOutput->u32Command = u32RegisterValue & dd429RxControlMaskTable[u32BitOffset];
            psRxControlCommandGetOutput->u32Command >>= u32BitOffset;

            *pBytesReturned = sizeof(U32BIT);
        }
    }
    else
    {
        status = DDC_UDL_ERROR__BUFFER_SIZE;
    }

    return status;
}

/*******************************************************************************
 * Name:    _arinc429GetFilter
 *
 * Description:
 *
 *      This function gets the specified filter.
 *
 * In:  pDeviceContext          device context
 * In:  u16Channel              channel to operate on
 * In:  u32LabelSDI             Label/SDI to get
 * Out: return                  filter status or error condition
 ******************************************************************************/
static U32BIT _arinc429GetFilter
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32LabelSDI
)
{
    U32BIT u32MemoryAddress;
    U32BIT u32AddrOffset;
    U32BIT u32Bit;
    U32BIT u32Data;

    u32AddrOffset = FILTER_LABEL_SDI_ADDRESS(u32LabelSDI);
    u32Bit = FILTER_LABEL_SDI_BIT(u32LabelSDI);


    u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN) + u32AddrOffset;
    DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);

    if (u32Data & au32DD429_DataMatchTable_BitTable[u32Bit])
    {
        return 1; /* filter exists */
    }
    else
    {
        return 0; /* filter does not exist */
    }
}

/*******************************************************************************
 * Name:    _ARINC429FilterCommand
 *
 * Description:
 *
 *      This function processes DD429_FILTER_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429FilterCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    DDC_IOCTL_PARAMS tempIoctlParams;
    U32BIT u16Channel;
    U32BIT u32MemoryAddress;
    U32BIT i;

    U32BIT u32Data = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FILTER, "429 - _ARINC429FilterCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__ENABLE:
        /* ---------------------------------------------------- */
        {
            /* pIoctlParams->Param2 is the Enable setting */

            /* calculate the DMT address for the RX channel */
            u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN);

            if (pIoctlParams->Param2 == DD429_ENABLE)
            {
                u32Data = DATA_MATCH_TABLE__ALL_ENABLED;
            }
            else
            {
                u32Data = DATA_MATCH_TABLE__ALL_DISABLED;
            }

            for (i = 0; i < DD429_DATA_MATCH_TABLE_LEN; i++)
            {
                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + i), &u32Data, ACEX_32_BIT_ACCESS);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__CONFIG:
        /* ---------------------------------------------------- */
        {
            /* pIoctlParams->Param2 is the Mode */

            /* ------------ */
            /* Parity Error */
            /* ------------ */

            /* tempIoctlParams.Param1: setting type */
            /* tempIoctlParams.Param2: setting value */

            tempIoctlParams.Channel = u16Channel;
            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__FILTER_PARITY_ERR;

            /* check for parity error setting */
            if (pIoctlParams->Param2 & FILTER_PARITY_ERR)
            {
                tempIoctlParams.Param2 = DD429_ENABLE;
            }
            else
            {
                tempIoctlParams.Param2 = DD429_DISABLE;
            }

            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            /* ------------ */
            /* Stale Msg    */
            /* ------------ */

            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__FILTER_DATA;

            /* check for stale message setting */
            if (pIoctlParams->Param2 & FILTER_STALE_MSG)
            {
                tempIoctlParams.Param2 = DD429_ENABLE;
            }
            else
            {
                tempIoctlParams.Param2 = DD429_DISABLE;
            }

            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__ADD:
        case DD429_FILTER_COMMAND__DELETE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32AddrOffset;
            U32BIT u32Bit;
            U32BIT u32TimeTagEnabledValue;
            U32BIT u32ModeValue;
            U32BIT u32LabelSDI;
            U8BIT bSdiIndex;

            U32BIT u32DataMatchTableEntry = 0;
            BOOLEAN bMailboxAndTimetagEnabled = FALSE;

            u32LabelSDI = pIoctlParams->Param2;

            u32TimeTagEnabledValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED);
            u32ModeValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__MODE);

            /* if in MailBox mode and TimeTag enabled, truncate the SDI */
            if ((u32TimeTagEnabledValue == DD429_ENABLE) && (u32ModeValue == DD429_MAILBOX_MODE))
            {
                u32LabelSDI &= DD429_LABEL_MASK;
                bMailboxAndTimetagEnabled = TRUE;
            }

            /* A '1' in a LabelSDI bit field turns on the filter */
            u32AddrOffset = FILTER_LABEL_SDI_ADDRESS(u32LabelSDI);
            u32Bit = FILTER_LABEL_SDI_BIT(u32LabelSDI);

            /* calculate the DMT address for the RX channel */
            u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN);

            DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u32AddrOffset), &u32DataMatchTableEntry, ACEX_32_BIT_ACCESS);

            /* if we are adding, see if the filter already exsits or */
            /* if we are deleting, see if the filter does NOT exist */
            if (((pIoctlParams->Param1 == DD429_FILTER_COMMAND__ADD) && (u32DataMatchTableEntry & au32DD429_DataMatchTable_BitTable[u32Bit])) ||
                ((pIoctlParams->Param1 == DD429_FILTER_COMMAND__DELETE) && !(u32DataMatchTableEntry & au32DD429_DataMatchTable_BitTable[u32Bit])))
            {
                status = 0;
            }
            else
            {
                if (pIoctlParams->Param1 == DD429_FILTER_COMMAND__ADD)
                {
                    u32DataMatchTableEntry |= au32DD429_DataMatchTable_BitTable[u32Bit]; /* add bit */
                }
                else /* delete */
                {
                    u32DataMatchTableEntry &= ~(au32DD429_DataMatchTable_BitTable[u32Bit]); /* clear bit */
                }

                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + u32AddrOffset), &u32DataMatchTableEntry, ACEX_32_BIT_ACCESS);

                /* If in MailBox - TimeTag mode, only the labels can be filtered */
                /* The SDI's are considered don't cares. Filter all four labels */
                if (bMailboxAndTimetagEnabled)
                {
                    for (bSdiIndex = 1; bSdiIndex <= 3; bSdiIndex++)
                    {
                        DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + (((DD429_DATA_MATCH_TABLE_LEN >> 2) * bSdiIndex) + u32AddrOffset)), &u32DataMatchTableEntry, ACEX_32_BIT_ACCESS);
                   }
                }

                status = 1;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__CLEAR:
        /* ---------------------------------------------------- */
        {
            u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN);
            u32Data = DATA_MATCH_TABLE__ALL_ENABLED;

            for (i = 0; i < DD429_DATA_MATCH_TABLE_LEN; i++)
            {
                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + i), &u32Data, ACEX_32_BIT_ACCESS);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__GET:
        /* ---------------------------------------------------- */
        {
            /* pIoctlParams->Param2 is the Label/SDI */
            *pLocalInOutBuffer = _arinc429GetFilter(pDeviceContext, u16Channel, pIoctlParams->Param2);
            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__GET_ALL:
        /* ---------------------------------------------------- */
        {
            S16BIT s16LabelSDI;
            U32BIT u32Count = 0;

            S16BIT *ps16LabelSDI = (S16BIT *) pLocalInOutBuffer;

            for (s16LabelSDI = MIN_LABEL_SDI; s16LabelSDI <= MAX_LABEL_SDI; s16LabelSDI++)
            {
                if (_arinc429GetFilter(pDeviceContext, u16Channel, s16LabelSDI) == DD429_ENABLE)
                {
                    ps16LabelSDI[u32Count] = s16LabelSDI;

                    u32Count++;
                }
            }

            /* the count loaded will be indirectly be sent back via the bytes returned */
            *pBytesReturned = u32Count * sizeof(S16BIT);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__GET_STATUS:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Offset;

            u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN);
            *pLocalInOutBuffer = DD429_DISABLE;

            for (u32Offset = 0; u32Offset < DD429_DATA_MATCH_TABLE_LEN; u32Offset++)
            {
                DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u32Offset), &u32Data, ACEX_32_BIT_ACCESS);

                if (u32Data != DATA_MATCH_TABLE__ALL_DISABLED)
                {
                    *pLocalInOutBuffer = DD429_ENABLE;
                    break;
                }
            }

            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FILTER_COMMAND__GET_NUM_OF_FILTER:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Offset;
            U32BIT u32BitOffset;

            *pLocalInOutBuffer = 0;

            u32MemoryAddress = *(pDeviceContext->sArinc429RxGlobal.pu32MemBA) + (u16Channel * DD429_DATA_MATCH_TABLE_LEN);
            for (u32Offset = 0; u32Offset < DD429_DATA_MATCH_TABLE_LEN; u32Offset++)
            {
                DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u32Offset), &u32Data, ACEX_32_BIT_ACCESS);

                for (u32BitOffset = 0; u32BitOffset < DD429_DATA_MATCH_TABLE_LEN; u32BitOffset++)
                {
                    if (u32Data & 0x00000001)
                    {
                        (*pLocalInOutBuffer)++;
                    }

                    u32Data >>= 1;
                }
            }

            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FILTER, "429 - _ARINC429FilterCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429MailboxCommand
 *
 * Description:
 *
 *      This function processes DD429_MAILBOX_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429MailboxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    DDC_IOCTL_PARAMS tempIoctlParams;
    U32BIT u16Channel;
    U32BIT u32MemoryAddress;
    U32BIT i;

    size_t bytesReturned = 0;    /* not used */
    U32BIT u32Data = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    tempIoctlParams.Channel = u16Channel;

    /* tempIoctlParams.Param1: setting type */
    /* tempIoctlParams.Param2: setting value */

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_MAILBOX, "429 - _ARINC429MailboxCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_MAILBOX_COMMAND__CLEAR_MAILBOX:
        /* ---------------------------------------------------- */
        {
            DD429_RX_CONTROL_COMMAND_GET__OUTPUT_TYPE sRxControlCommandGetOutput;
            U32BIT u32MaxMailboxMessages = DD429_RX_FIFO_MAILBOX_LENGTH;

            /* U32BIT u32TimeTagEnabledValue; */
            U32BIT u32AutoClearEnabled;

            /* initialize value */
            sRxControlCommandGetOutput.u32Command = 0;

            /* determine if the time tag is enabled */
            /* u32TimeTagEnabledValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED); */

            /* read auto clear setting */
            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_GET__LABEL_AUTO_CLEAR;
            _ARINC429RxControlCommandGet(pDeviceContext, &tempIoctlParams, (U32BIT *) &sRxControlCommandGetOutput, sizeof(&sRxControlCommandGetOutput), &bytesReturned);
            u32AutoClearEnabled = sRxControlCommandGetOutput.u32Command;

            /* enable label auto clear if currently disabled */
            if (u32AutoClearEnabled == DD429_DISABLE)
            {
                tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__LABEL_AUTO_CLEAR;
                tempIoctlParams.Param2 = DD429_ENABLE;
                _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);
            }

            /* calculate the mailbox address for the RX channel */
            u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);

            /* for each mailbox... */
            for (i = 0; i < u32MaxMailboxMessages; i++)
            {
                /* read to clear the 'new' indication */
                DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + i), &u32Data, ACEX_32_BIT_ACCESS);

                /* clear any stale data */
                if ((i & DD429_LABEL_MASK) == 0)
                {
                    /* write 0xFF to the location for all label 0 locations (SDI:0-3) */
                    u32Data = 0x000000FF;
                }
                else
                {
                    /* write 0x00 to the location */
                    u32Data = 0x00000000;
                }

                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + i), &u32Data, ACEX_32_BIT_ACCESS);
                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + i) + DD429_RX_FIFO_MAILBOX_LENGTH, &u32Data, ACEX_32_BIT_ACCESS);
                DD429_MEM_WRITE(pDeviceContext, (u32MemoryAddress + i) + (DD429_RX_FIFO_MAILBOX_LENGTH * 2), &u32Data, ACEX_32_BIT_ACCESS);
            }

            /* if auto clear was not enabled, we need to disable */
            if (u32AutoClearEnabled == DD429_DISABLE)
            {
                tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__LABEL_AUTO_CLEAR;
                tempIoctlParams.Param2 = DD429_DISABLE;
                _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_MAILBOX_COMMAND__GET_MAILBOX:
        /* ---------------------------------------------------- */
        {
            DD429_GET_MAILBOX__OUTPUT_TYPE *psGetMailbox = (DD429_GET_MAILBOX__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(psGetMailbox))
            {
                U32BIT u32MaxMailboxMessages = DD429_RX_FIFO_MAILBOX_LENGTH;
                U32BIT u32MaxArraySize;
                U8BIT u8IsBufferNonNull;

                u32MaxArraySize = DDC_IOCTL_U32(pIoctlParams->Param2); /* parameter N passed into the GetMailbox API */
                u8IsBufferNonNull = (U8BIT) pIoctlParams->Param3;
                psGetMailbox->s16N = 0;

                u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);

                for (i = 0; i < u32MaxMailboxMessages; i++)
                {
                    DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + i), &u32Data, ACEX_32_BIT_ACCESS);

                    /* check for a label: (i != 0 and label non-zero) or (i == 0 and label is zero) */
                    if (((i % DD429_NUM_LABELS != 0) && (u32Data & DD429_LABEL_MASK)) ||
                        ((i % DD429_NUM_LABELS == 0) && ((u32Data & DD429_LABEL_MASK) == 0))) /* special processing for label 0 */
                    {
                        /* if the user passed in an array size and non-NULL buffer, update the list */
                        if ((u32MaxArraySize > 0) && (u8IsBufferNonNull))
                        {
                            psGetMailbox->su16LabelSDI[psGetMailbox->s16N] = (U16BIT)i;
                            psGetMailbox->s16N++;

                            /* make sure we do not go over the array size */
                            if (psGetMailbox->s16N >= (S16BIT)u32MaxArraySize)
                            {
                                break;
                            }
                        }
                    }
                }

                *pBytesReturned = sizeof(*psGetMailbox);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_MAILBOX_COMMAND__GET_MAILBOX_STATUS:
        /* ---------------------------------------------------- */
        {
            U16BIT u16LabelSDI = (U16BIT) pIoctlParams->Param2;

            if (u16LabelSDI <= MAX_LABEL_SDI)
            {
                u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);

                DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u16LabelSDI), &u32Data, ACEX_32_BIT_ACCESS);

                /* do special processing if checking label 0 */
                if ((u16LabelSDI & DD429_LABEL_MASK) == 0x0000)
                {
                    /* an inverse of the label indicates stale data, as the label is already 0 */
                    if ((u32Data & DD429_LABEL_MASK) == 0x000000FF)
                    {
                        *pLocalInOutBuffer = 0; /* did not receive a new word */
                    }
                    else
                    {
                        *pLocalInOutBuffer = 1; /* received a new word */
                    }
                }
                else if (u32Data & DD429_LABEL_MASK) /* for all labels not 0 */
                {
                    *pLocalInOutBuffer = 1; /* received a new word */
                }
                else
                {
                    *pLocalInOutBuffer = 0; /* did not receive a new word */
                }
            }
            else /* time tag is enabled and the Label/SDI contains an SDI */
            {
                status = ERR_LABELSDI; /* ERROR */
            }

            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_MAILBOX_COMMAND__READ_MAILBOX_IRIG:
        /* ---------------------------------------------------- */
        {
            DD429_READ_MAILBOX_IRIG__OUTPUT_TYPE *psReadRxMailboxIrig = (DD429_READ_MAILBOX_IRIG__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*psReadRxMailboxIrig))
            {
                U32BIT u32TimeTagEnabledValue;

                U32BIT u32TimetagHi = 0;
                U32BIT u32TimetagLo = 0;
                U16BIT u16LabelSDI = (U16BIT) pIoctlParams->Param2;

                /* init return values */
                psReadRxMailboxIrig->u32RxMailbox_NewWordReceived = FALSE;
                psReadRxMailboxIrig->u32RxMailbox_LabelSdiError = FALSE;

                /* determine if the time tag is enabled */
                u32TimeTagEnabledValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED);

                /* enable label auto clear */
                tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__LABEL_AUTO_CLEAR;
                tempIoctlParams.Param2 = DD429_ENABLE;
                _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

                /* get the data */
                u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);
                DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u16LabelSDI), &psReadRxMailboxIrig->u32Data, ACEX_32_BIT_ACCESS);

                if (u32TimeTagEnabledValue == DD429_ENABLE)
                {
                    /* Must read both TT values from card to unlock internal logic */
                    DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u16LabelSDI + (DD429_RX_FIFO_MAILBOX_LENGTH)), &u32TimetagLo, ACEX_32_BIT_ACCESS);
                    DD429_MEM_READ(pDeviceContext, (u32MemoryAddress + u16LabelSDI + (DD429_RX_FIFO_MAILBOX_LENGTH * 2)), &u32TimetagHi, ACEX_32_BIT_ACCESS);

                    /* just populate the data - the library side will handle the copy */
                    psReadRxMailboxIrig->u32StampHi = u32TimetagHi;
                    psReadRxMailboxIrig->u32StampLo = u32TimetagLo;
                }
                else
                {
                    /* just populate the data - the library side will handle the copy */
                    psReadRxMailboxIrig->u32StampHi = 0xFFFFFFFF;
                    psReadRxMailboxIrig->u32StampLo = 0xFFFFFFFF;
                }

                /* disable label auto clear */
                tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__LABEL_AUTO_CLEAR;
                tempIoctlParams.Param2 = DD429_DISABLE;
                _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

                /* check for stale data: (label != 0 and label value is 0x00) or (label == 0 and label value is 0xFF) */
                if ((((u16LabelSDI & DD429_LABEL_MASK) != 0x0000) && (((psReadRxMailboxIrig->u32Data & DD429_LABEL_MASK) == 0x00000000))) ||
                    (((u16LabelSDI & DD429_LABEL_MASK) == 0x0000) && ((psReadRxMailboxIrig->u32Data & DD429_LABEL_MASK) == 0x000000FF)))
                {
                    psReadRxMailboxIrig->u32RxMailbox_NewWordReceived = FALSE; /* indicate old word in mailbox */

                    /* clean-up placeholder label and SDI from firmware */
                    psReadRxMailboxIrig->u32Data &= ~(DD429_LABEL_SDI_MASK); /* wipe out placeholder label and SDI */
                    psReadRxMailboxIrig->u32Data |= u16LabelSDI;             /* restore user's label and SDI */
                }
                /* new data */
                else
                {
                    psReadRxMailboxIrig->u32RxMailbox_NewWordReceived = TRUE; /* indicate new word in mailbox */
                }

                *pBytesReturned = sizeof(*psReadRxMailboxIrig);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_MAILBOX, "429 - _ARINC429MailboxCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429FifoCommand
 *
 * Description:
 *
 *      This function processes DD429_FIFO_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * In:  pwdfRequest             poitner to WDF request object
 * Out: pbRequestPending        pointer to the request pending status
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429FifoCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    DDC_IOCTL_PARAMS tempIoctlParams;
    U32BIT u16Channel;
    U32BIT u32MemoryAddress;
    U32BIT u32RegisterAddress;
    U32BIT u32HeadTailRegisterAddress;

    U32BIT u32RegisterValue = 0;
    U32BIT u32HeadTailRegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    tempIoctlParams.Channel = u16Channel;

    /* tempIoctlParams.Param1: setting type */
    /* tempIoctlParams.Param2: setting value */

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FIFO, "429 - _ARINC429FifoCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_FIFO_COMMAND__CLEAR_RX_QUEUE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32RegisterAddress;
            U32BIT u32RegisterValue;

            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_MSG_COUNT_REG;

            u32RegisterValue = ACEX_429_RX_FIFO_MSG_COUNT__CLEAR_FIFO_POINTERS_MASK;
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            u32RegisterValue = (U32BIT) ~(ACEX_429_RX_FIFO_MSG_COUNT__CLEAR_FIFO_POINTERS_MASK);
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FIFO_COMMAND__GET_RX_QUEUE_STATUS:
        /* ---------------------------------------------------- */
        {
            U32BIT u32ModeValue;
            DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE *psRxQueueStatus = (DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*psRxQueueStatus))
            {
                *pBytesReturned = sizeof(*psRxQueueStatus);

                u32ModeValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__MODE);

                if (u32ModeValue == DD429_FIFO_MODE)
                {
                    /* read the RX FIFO count register */
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_MSG_COUNT_REG;
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                    if ((u32RegisterValue & DD429_RX_FIFO_MSG_COUNT_MASK__OVERFLOW) == DD429_OVERFLOW_NONE)
                    {
                        psRxQueueStatus->u32RxFIFO_Count = (u32RegisterValue & DD429_RX_FIFO_MSG_COUNT_MASK__MSG_COUNT);
                        psRxQueueStatus->u32Error = (U32BIT) DDC_UDL_ERROR__SUCCESS;
                    }
                    else
                    {
                        /* ERROR: Overflow */
                        psRxQueueStatus->u32RxFIFO_Count = 0;
                        psRxQueueStatus->u32Error = (U32BIT) ERR_OVERFLOW;
                    }
                }
                else
                {
                    /* ERROR: Mode */
                    psRxQueueStatus->u32RxFIFO_Count = 0;
                    psRxQueueStatus->u32Error = (U32BIT) ERR_MODE;
                }
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FIFO_COMMAND__READ_RX_QUEUE_IRIG_ONE:
        /* ---------------------------------------------------- */
        {
            DD429_READ_DATA_IRIG__OUTPUT_TYPE *psReadRxFifoIrig = (DD429_READ_DATA_IRIG__OUTPUT_TYPE *) pLocalInOutBuffer;
            DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE sRxQueueStatus;
            U32BIT u32MemoryBaseAddress;
            size_t tempOutputBufferLength;
            size_t tempBytesReturned;

            sRxQueueStatus.u32Error = DDC_UDL_ERROR__SUCCESS;
            psReadRxFifoIrig->u32Error = DDC_UDL_ERROR__SUCCESS;

            /* get the FIFO status */
            tempOutputBufferLength = sizeof(sRxQueueStatus);
            tempIoctlParams.Param1 = DD429_FIFO_COMMAND__GET_RX_QUEUE_STATUS;
            _ARINC429FifoCommand(pDeviceContext, &tempIoctlParams, (U32BIT *) &sRxQueueStatus, tempOutputBufferLength, &tempBytesReturned);

            /* read the RX FIFO count register */
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_MSG_COUNT_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            if (sRxQueueStatus.u32Error != DDC_UDL_ERROR__SUCCESS)
            {
                /* ERROR */
                psReadRxFifoIrig->u32Error = sRxQueueStatus.u32Error;
                break;
            }

            if (OutputBufferLength >= sizeof(*psReadRxFifoIrig))
            {
                U32BIT u32TimeTagEnabledValue;
                U32BIT u32Tail;
                U32BIT u32Reserved;

                U32BIT u32TimetagHi = 0;
                U32BIT u32TimetagLo = 0;

                /* read the RX FIFO count register */
                u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_MSG_COUNT_REG;
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                /* does the FIFO contain data? */
                if ((u32RegisterValue & DD429_RX_FIFO_MSG_COUNT_MASK__EMPTY) == DD429_FIF0_NOT_EMPTY)
                {
                    /* calculate the FIFO address for the RX channel */
                    u32MemoryBaseAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);

                    u32HeadTailRegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_HEAD_TAIL_REG;
                    DDC_REG_READ(pDeviceContext, u32HeadTailRegisterAddress, &u32HeadTailRegisterValue);

                    u32Tail = (u32HeadTailRegisterValue & DD429_RX_HEAD_TAIL_MASK__TAIL) >> DD429_RX_HEAD_TAIL_OFFSET__TAIL;

                    u32MemoryAddress = u32MemoryBaseAddress + u32Tail++;
                    DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &psReadRxFifoIrig->u32Data, ACEX_32_BIT_ACCESS);

                    /* determine if the time tag is enabled */
                    u32TimeTagEnabledValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED);

                    if (u32TimeTagEnabledValue == DD429_ENABLE)
                    {
                        u32MemoryAddress = u32MemoryBaseAddress + u32Tail++;
                        DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32TimetagLo, ACEX_32_BIT_ACCESS);

                        u32MemoryAddress = u32MemoryBaseAddress + u32Tail++;
                        DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32TimetagHi, ACEX_32_BIT_ACCESS);

                        u32MemoryAddress = u32MemoryBaseAddress + u32Tail;
                        DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Reserved, ACEX_32_BIT_ACCESS);

                        /* just populate the data - the library side will handle the copy */
                        psReadRxFifoIrig->u32StampHi = u32TimetagHi;
                        psReadRxFifoIrig->u32StampLo = u32TimetagLo;

                        /* u32Reserved - not used */
                    }
                    else
                    {
                        /* just populate the data - the library side will handle the copy */
                        psReadRxFifoIrig->u32StampHi = 0xFFFFFFFF;
                        psReadRxFifoIrig->u32StampLo = 0xFFFFFFFF;
                    }

                    /* indicate we read 1 item */
                    psReadRxFifoIrig->u32Error = 1;
                }
                else
                {
                    /* FIFO empty */
                    psReadRxFifoIrig->u32Error = 0;
                }

                *pBytesReturned = sizeof(*psReadRxFifoIrig);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_FIFO_COMMAND__READ_RX_QUEUE_IRIG_MORE:
        /* ---------------------------------------------------- */
        {
            DD429_GET_RX_QUEUE_STATUS__OUTPUT_TYPE sRxQueueStatus;
            U32BIT u32Tail;
            U32BIT u32TimeTagEnabledValue;
            U32BIT u32MaxNumberWords;
            U32BIT u32RxChannelMemoryBaseAddress;
            size_t tempOutputBufferLength;
            size_t tempBytesReturned;
            U32BIT u32NumMsgsToDMA = 0;
            U32BIT u32DataEntriesToRead = 0;

            DD429_READ_DATA_IRIG_MORE__OUTPUT_TYPE *psReadRxFifoIrigMore = (DD429_READ_DATA_IRIG_MORE__OUTPUT_TYPE *) pLocalInOutBuffer;

            u32MaxNumberWords = DDC_IOCTL_U32(pIoctlParams->Param2);

            /* initialize the returned bytes and msg count */
            *pBytesReturned =  2 * sizeof(U32BIT); /* as minimum, return 8 bytes of header */
            u32DataEntriesToRead = 0;
            psReadRxFifoIrigMore->s16FifoCount = 0;
            psReadRxFifoIrigMore->u32Error = 0;

            if (OutputBufferLength >= sizeof(*psReadRxFifoIrigMore))
            {
                /* get the FIFO status */
                tempOutputBufferLength = sizeof(sRxQueueStatus);
                tempIoctlParams.Param1 = DD429_FIFO_COMMAND__GET_RX_QUEUE_STATUS;
                _ARINC429FifoCommand(pDeviceContext, &tempIoctlParams, (U32BIT *) &sRxQueueStatus, tempOutputBufferLength, &tempBytesReturned);

                if (sRxQueueStatus.u32Error != DDC_UDL_ERROR__SUCCESS)
                {
                    /* ERROR */
                    psReadRxFifoIrigMore->u32Error = sRxQueueStatus.u32Error;
                    break;
                }
                else if (sRxQueueStatus.u32RxFIFO_Count == 0)
                {
                    /* no messages, so nothing to process */
                    break;
                }

                /* save the FIFO address for the RX channel */
                u32RxChannelMemoryBaseAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);

                /* determine if the time tag is enabled */
                u32TimeTagEnabledValue = _RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED);

                psReadRxFifoIrigMore->s16TimeTagEnabledValue = (S16BIT)u32TimeTagEnabledValue;

                /* get the starting offset of the data */
                u32HeadTailRegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_HEAD_TAIL_REG;
                DDC_REG_READ(pDeviceContext, u32HeadTailRegisterAddress, &u32HeadTailRegisterValue);
                u32Tail = (u32HeadTailRegisterValue & DD429_RX_HEAD_TAIL_MASK__TAIL) >> DD429_RX_HEAD_TAIL_OFFSET__TAIL;

                /* determine how many msgs to DMA - up to the # of msgs in the FIFO or the max size of the user buffer */
                u32NumMsgsToDMA = (sRxQueueStatus.u32RxFIFO_Count <= u32MaxNumberWords) ? sRxQueueStatus.u32RxFIFO_Count : u32MaxNumberWords;

                if (u32TimeTagEnabledValue == DD429_ENABLE)
                {
                    /* +-------+-------+-------+-------+ */
                    /* | Data1 | TT Lo | TT Hi | Rsvd  | */
                    /* +-------+-------+-------+-------+ */
                    /* | Data2 | TT Lo | TT Hi | Rsvd  | */
                    /* +-------+-------+-------+-------+ */

                    u32DataEntriesToRead = u32NumMsgsToDMA * 4;
                }
                else /* time tag not enabled */
                {
                    /* +-------+-------+-------+-------+ */
                    /* | Data1 | Data2 | Data3 | Data4 | */
                    /* +-------+-------+-------+-------+ */
                    /* | Data5 | Data6 | Data7 | Data8 | */
                    /* +-------+-------+-------+-------+ */

                    u32DataEntriesToRead = u32NumMsgsToDMA;
                }

#if DDC_DMA_429
                {
                    /* we want to DMA at least 4 entries. reading 1 to 3 entries can happen only if TT is not enabled */
                    /* if this is the case, just do direct reads instead of DMA */
                    if (u32DataEntriesToRead < 4)
                    {
                        DDC_BLK_MEM_READ(pDeviceContext, u32RxChannelMemoryBaseAddress + u32Tail,
                            (U32BIT *)&psReadRxFifoIrigMore->au32Data[0], u32DataEntriesToRead, ACEX_32_BIT_ACCESS);
                    }
                    else
                    {
                        /* check to see if the tail is not on an 8 byte (2 DWORD) boundary */
                        /* in other words, see if the tail is odd */
                        if (u32Tail % 2)
                        {
                            /* since the tail is odd, we will know that at least 5 DWORD values */
                            /* need to be read from the RX FIFO - this can only happen when TT */
                            /* is not enabled */

                            /* read one entry from the receive FIFO */
                            DDC_MEM_READ(pDeviceContext, u32RxChannelMemoryBaseAddress + u32Tail,
                                (U32BIT *)&psReadRxFifoIrigMore->au32Data[0], ACEX_32_BIT_ACCESS);

                            /* after reading from the FIFO, the firmware automatically increment the tail pointer */
                            /* read the tail pointer again, otherwise we would have to do a check for a rollover */
                            DDC_REG_READ(pDeviceContext, u32HeadTailRegisterAddress, &u32HeadTailRegisterValue);
                            u32Tail = (u32HeadTailRegisterValue & DD429_RX_HEAD_TAIL_MASK__TAIL) >> DD429_RX_HEAD_TAIL_OFFSET__TAIL;

                            /* indicate that there is one item in our RX FIFO and one less entry to read */
                            psReadRxFifoIrigMore->s16FifoCount++;
                            u32DataEntriesToRead--;
                        }

                        /* start the DMA transfer */
                        dmaARINC429RxFifoSetup(pDeviceContext,
                            (U8BIT)u16Channel,
                            (u32DataEntriesToRead << 2),         /* convert # of 32-bit entries to bytes */
                            ((u32RxChannelMemoryBaseAddress + u32Tail) << 2),         /* convert to byte address */
                            (U8BIT *)&psReadRxFifoIrigMore->au32Data[psReadRxFifoIrigMore->s16FifoCount]);
                            /* use the FIFO count as an index to the data since we may have data already in the buffer */
                            /* from the single read case above. the count will always be 0 when using TT. */

                        DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);
                        if (pDeviceContext->pRxChnl429[u16Channel]->u16RxFifoEventCond)
                        {
                            pDeviceContext->pRxChnl429[u16Channel]->u16RxFifoEventCond--;
                        }
                        DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);

                        /* start DMA and wait for DMA completion */
                        dmaQueueStart(pDeviceContext);

                        DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);
                        if (0 == pDeviceContext->pRxChnl429[u16Channel]->u16RxFifoEventCond)
                        {
                            int nResult = 0;

                            DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);

                            nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                                pDeviceContext->pRxChnl429[u16Channel]->waitqueueARINC429RxFifoEvent,
                                pDeviceContext->pRxChnl429[u16Channel]->u16RxFifoEventCond,
                                500);

                            if (nResult)
                            {
                                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FIFO, "nResult %d\n", nResult);
                            }
                        }
                        else
                        {
                            DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429RxFifoEventCond, pDeviceContext->semArinc429RxFifoEventCondFlag);
                        }
                    }
                }
#else /* use direct memory reads */
                {
                    U32BIT u32MemSize = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemSize);
                    U32BIT u32DataEntriesToRead_part1;
                    U32BIT u32DataEntriesToRead_part2;
                    U32BIT *pu32RdData;

                    /* check if we will wrap around when reading */
                    if ((u32Tail + u32DataEntriesToRead) > u32MemSize)
                    {
                        int i = 0;

                        u32DataEntriesToRead_part1 = u32MemSize - u32Tail;
                        u32DataEntriesToRead_part2 = u32DataEntriesToRead - u32DataEntriesToRead_part1;

                        pu32RdData = &psReadRxFifoIrigMore->au32Data[0];

                        for (i = 0; i < u32DataEntriesToRead_part1; i++, pu32RdData++)
                        {
                            DD429_MEM_READ(pDeviceContext,u32RxChannelMemoryBaseAddress + u32Tail + i, pu32RdData, ACEX_32_BIT_ACCESS);
                        }

                        pu32RdData = (U32BIT *)&psReadRxFifoIrigMore->au32Data[u32DataEntriesToRead_part1];

                        for (i = 0; i < u32DataEntriesToRead_part2; i++, pu32RdData++)
                        {
                            DD429_MEM_READ(pDeviceContext,u32RxChannelMemoryBaseAddress + i, pu32RdData, ACEX_32_BIT_ACCESS);
                        }
                    }
                    else
                    {
                        int i = 0;
                        U32BIT *pu32RdData = psReadRxFifoIrigMore->au32Data;

                        for (i = 0; i < u32DataEntriesToRead; i++, pu32RdData++)
                        {
                            DD429_MEM_READ(pDeviceContext,u32RxChannelMemoryBaseAddress + u32Tail + i, pu32RdData, ACEX_32_BIT_ACCESS);
                        }
                    }

                }
#endif /* DDC_DMA_429 */

                /* set the msg count */
                psReadRxFifoIrigMore->s16FifoCount = (S16BIT) u32NumMsgsToDMA;

            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            /* set returned bytes by adding data dwords */
            *pBytesReturned = *pBytesReturned + u32DataEntriesToRead * sizeof(U32BIT);

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FIFO, "429 - _ARINC429FifoCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _arinc429SetRepeated
 *
 * Description:
 *
 *      This function writes repeated data to the card and to the pDeviceContext shadow copy.
 *
 * In:  pDeviceContext     device context
 * In:  u32Channel         channel to operate on
 * In:  Index              Index of the repeated data to update
 * In:  Data               The data to write
 * In:  Control            The control word to write
 * In:  Frequency          The new frequency to write
 * In:  Offset             The new offset to write
 * In:  Active             Enable/Disable the scheduled data
 ******************************************************************************/
static VOID _arinc429SetRepeated
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Options,
    PSCHEDULER_DATA pSchedulerData
)
{
    U32BIT u32MemoryAddress = 0;
    U32BIT u32Value = 0;

    /* write active */
    /* check to do this first if we are disabling the item */
    if ((u32Options & DD429_SCHED_ACTIVE_UPDATE_MASK) && (pSchedulerData->u8Active == DD429_NOT_ACTIVE))
    {
        u32MemoryAddress = *(pDeviceContext->sTxChnlExtend429[pSchedulerData->u32Channel].pu32MemBA) + (pSchedulerData->u16Index);
        u32Value = pSchedulerData->u8Active;
        DDC_MEM_WRITE(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
    }

    /* write data */
    if (u32Options & DD429_SCHED_DATA_UPDATE_MASK)
    {
        u32MemoryAddress = *(pDeviceContext->pRxChnl429[pSchedulerData->u32Channel]->pu32MemBA) + (pSchedulerData->u16Index);
        u32Value = pSchedulerData->u32Data;
        DDC_MEM_WRITE(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
    }

    /* write ctrl */
    if (u32Options & DD429_SCHED_CONTROL_UPDATE_MASK)
    {
        u32MemoryAddress = *(pDeviceContext->pRxChnl429[pSchedulerData->u32Channel]->pu32MemBA) + (pSchedulerData->u16Index + DD429_TX_SCHED_CONTROL_OFFSET);
        u32Value = pSchedulerData->u32Control;
        DDC_MEM_WRITE(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
    }

    /* write freq/offset */
    if ((u32Options & DD429_SCHED_FREQUENCY_UPDATE_MASK) && (u32Options & DD429_SCHED_OFFSET_UPDATE_MASK))
    {
        /* NOTE: This implementation is slightly different then the legacy method. Since the Freq and offset */
        /* are now combined into 1 memory location it is no longer possible to write one without the other */
        /* the closest we can get to the old implementation is to avoiding writing both if neither has changed. */
        /* Rewriting this value while the item is already active will cause a message to go out in offset + 1ms */
        u32MemoryAddress = *(pDeviceContext->pTxChnl429[pSchedulerData->u32Channel]->pu32MemBA) + (pSchedulerData->u16Index);
        DDC_MEM_READ(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
        u32Value &= DD429_TX_SCHED_FREQ_MASK;

        if ((pSchedulerData->u16Frequency != u32Value) || (pSchedulerData->u16Offset != pDeviceContext->U.u16TxSchedulerOffset[pSchedulerData->u32Channel][pSchedulerData->u16Index]))
        {
            u32MemoryAddress = *(pDeviceContext->pTxChnl429[pSchedulerData->u32Channel]->pu32MemBA) + (pSchedulerData->u16Index);
            u32Value = (pSchedulerData->u16Frequency << DD429_TX_SCHED_FREQ_SHIFT) + (pSchedulerData->u16Offset);
            DDC_MEM_WRITE(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
            pDeviceContext->U.u16TxSchedulerOffset[pSchedulerData->u32Channel][pSchedulerData->u16Index] = pSchedulerData->u16Offset;
        }
    }

    /* write active */
    /* do this last if we are enabling the item */
    if ((u32Options & DD429_SCHED_ACTIVE_UPDATE_MASK) && (pSchedulerData->u8Active == DD429_ACTIVE))
    {
        u32MemoryAddress = *(pDeviceContext->sTxChnlExtend429[pSchedulerData->u32Channel].pu32MemBA) + (pSchedulerData->u16Index);
        u32Value = pSchedulerData->u8Active;
        DDC_MEM_WRITE(pDeviceContext, (u32MemoryAddress), &u32Value, ACEX_32_BIT_ACCESS);
    }

    return;
}

/*******************************************************************************
 * Name:    _ARINC429GeneralCommand
 *
 * Description:
 *
 *      This function retrieves the requested RX command item.
 *
 * In:  pDeviceContext
 * In:  pIoctlParams
 * In:  pLocalInOutBuffer
 * In:  OutputBufferLength
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  RX command value or error condition
 ******************************************************************************/
static S16BIT _ARINC429GeneralCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    U32BIT u16Channel;

    U32BIT u32TempRegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_GENERAL, "429 - _ARINC429GeneralCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__RESET:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;
            U32BIT u32RegisterValue = 0;
            U32BIT u32Value  = 0;
            U8BIT u16Channel = 0;
            size_t bytesReturned = 0;

            /* Global Reset */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegisterValue);
            u32RegisterValue |= DD429_RX_GLOBAL_MASK__RESET;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32RegisterValue);

            /* since the RX Global Control register was just read, set/clear the capabilities bit to indicate if this device supports IRIG-B enhanced format or not */
            if (u32RegisterValue & ACEX_429_RX_GLOBAL_GEN_CTRL_IRIG_B_ENHANCED_TIMETAG_SUPPORTED_MASK)
            {
                /* set it */
                pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_IRIG_B_ENHANCED;
            }
            else
            {
                /* clear it */
                pDeviceContext->sHwVersionInfo.dwCapabilities2 &= ~(HWVER_CAPABILITY2_IRIG_B_ENHANCED);
            }

            /* Reset and disable TX */
            for (u16Channel = 0; u16Channel < pDeviceContext->u8NumProg429RxTx; u16Channel++)
            {
                U32BIT u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;

                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                u32RegisterValue |= DD429_TX_CONTROL_MASK__RESET;
                u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__ENABLE);
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            }

            /*
             * Set the scheduler resolution - physical time equates to multiples
             * of the master clock (e.g. 40MHz or 25ns). Example: If resolution
             * is set to 1000, the resolution interval is 25ns * 1000 = 25us.
             * Resolution register is used by legacy and 1024 scheduler.
             */
            u32Value = DD429_TX_SCHEDULER_RESOLUTION_1MS;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_SCHEDULER_RESOLUTION_REG), &u32Value);

            /* scalable queue size for legacy scheduler */
            if (!pDeviceContext->bExtendedScheduler)
            {
                u32Value = DD429_TX_SCHED_QUEUE_DEPTH;
                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_SCHEDULER_MESSAGE_QUEUE_LEN_REG), &u32Value);
            }

            /* enable time stamp */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);
            u32TempRegisterValue |= IRIGB_CONTROL_DMY_FORMAT_ENABLE | IRIGB_CONTROL_CLOCK_ENABLE;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sIrigB_RX->pu32RegBA) + REG_IRIGB_CONTROL_RW, &u32TempRegisterValue);

            /* reset time stamp */
            tempIoctlParams.Param1 = DD429_RX_COMMAND__RESET_TIMESTAMP;
            _ARINC429RxCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &bytesReturned);

#if 0

            /* enable TT rollover interrupt status */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);
            u32TempRegisterValue |= ACEX_429_RX_GLOBAL_INT_ENABLE_IRIG_TT_RO_MASK;
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32TempRegisterValue);
#endif

            /* Reset RX Filter */
            for (u16Channel = 0; u16Channel < pDeviceContext->u8NumProg429RxTx; u16Channel++)
            {
                tempIoctlParams.Channel = u16Channel;
                tempIoctlParams.Param1 = DD429_FILTER_COMMAND__ENABLE;
                tempIoctlParams.Param2 = DD429_DISABLE;
                _ARINC429FilterCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &bytesReturned);

                tempIoctlParams.Channel = u16Channel;
                tempIoctlParams.Param1 = DD429_FILTER_COMMAND__CONFIG;
                tempIoctlParams.Param2 = FILTER_LABEL_SDI;
                _ARINC429FilterCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &bytesReturned);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__SET_BIT_FORMAT:
        /* ---------------------------------------------------- */
        {
            U32BIT u32RxRegisterAddress;
            U32BIT u32TxRegisterAddress;
            U8BIT u16Channel;

            U32BIT u32RxRegisterValue = 0;
            U32BIT u32TxRegisterValue = 0;
            U8BIT u8Arinc429BitFormat = (U8BIT)pIoctlParams->Param2;

            for (u16Channel = 0; u16Channel < pDeviceContext->u8NumProg429RxTx; u16Channel++)
            {
                u32RxRegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
                u32TxRegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;

                DDC_REG_READ(pDeviceContext, u32RxRegisterAddress, &u32RxRegisterValue);
                DDC_REG_READ(pDeviceContext, u32TxRegisterAddress, &u32TxRegisterValue);

                if (u8Arinc429BitFormat == DD429_BITFORMAT_ALT)
                {
                    u32RxRegisterValue |= DD429_RX_CONTROL_MASK__BIT_FORMAT;
                    u32TxRegisterValue |= DD429_TX_CONTROL_MASK__BIT_FORMAT;
                }
                else
                {
                    u32RxRegisterValue &= ~(DD429_RX_CONTROL_MASK__BIT_FORMAT);
                    u32TxRegisterValue &= ~(DD429_TX_CONTROL_MASK__BIT_FORMAT);
                }

                DDC_REG_WRITE(pDeviceContext, u32RxRegisterAddress, &u32RxRegisterValue);
                DDC_REG_WRITE(pDeviceContext, u32TxRegisterAddress, &u32TxRegisterValue);
            }

            pDeviceContext->u8Arinc429BitFormat = u8Arinc429BitFormat;

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__INTERRUPT_ENABLE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32IntEnable = DDC_IOCTL_U32(pIoctlParams->Param3);

            switch (pIoctlParams->Param2)
            {
                /* --------------------- */
                case CHAN_TYPE_429:
                /* --------------------- */
                {
                    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);

                    if (u32IntEnable == DD429_ENABLE)
                    {
                        u32TempRegisterValue |= DD429_RX_GLOBAL_MASK__INTERRUPT_ENABLE;
                    }
                    else
                    {
                        u32TempRegisterValue &= ~(DD429_RX_GLOBAL_MASK__INTERRUPT_ENABLE);
                    }

                    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_GEN_CTRL_REG, &u32TempRegisterValue);
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_UART:
                case CHAN_TYPE_232:
                case CHAN_TYPE_485:
                case CHAN_TYPE_SDLC:
                case CHAN_TYPE_HDLC:
                /* --------------------- */
                {
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_CAN:
                /* --------------------- */
                {
                    if (u32IntEnable == DD429_ENABLE)
                    {
                        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_CAN_1);
                        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_CAN_2);
                    }
                    else
                    {
                        ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_CAN_1);
                        ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_CAN_2);
                    }

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_TT:
                /* --------------------- */
                {
                    /* nothing to disable or enable as the IRIG 1 SEC interrupt should always enabled */
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_VOLT_MON:
                /* --------------------- */
                {
                    if (u32IntEnable == DD429_ENABLE)
                    {
                        ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT);
                    }
                    else
                    {
                        ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_ARINC_429_VOLT_MON_CMPLT);
                    }

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_717_PROG:
                /* --------------------- */
                {
                    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_PROG_ARINC_717)
                    {                      
                        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_CONFIG, &u32TempRegisterValue);

                        if (u32IntEnable == DD429_ENABLE)
                        {
                            u32TempRegisterValue |= REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE;
                        }
                        else
                        {
                            u32TempRegisterValue &= ~(REG_ARINC_717_GLOBAL_CONFIG_INT_ENABLE);
                        }

                        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_CONFIG, &u32TempRegisterValue);
                    }
                    break;
                }

                /* --------------------- */
                default:
                /* --------------------- */
                {
                    status = ERR_IRQ;
                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__GET_INTERRUPT_STATUS:
        /* ---------------------------------------------------- */
        {
            U32BIT *pu32Status = (U32BIT *) pLocalInOutBuffer;
            U32BIT u32RegisterAddress;

            switch (pIoctlParams->Param2)
            {
                /* --------------------- */
                case CHAN_TYPE_429:
                /* --------------------- */
                {
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_STATUS_REG;

                    /* Read then clear interrupt status register */
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TempRegisterValue);

                    /* send back the interrupt status register value */
                    pu32Status[0] = u32TempRegisterValue;
                    *pBytesReturned = sizeof(u32TempRegisterValue);

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_SDLC:
                case CHAN_TYPE_HDLC:
                case CHAN_TYPE_UART:
                case CHAN_TYPE_232:
                case CHAN_TYPE_485:
                /* --------------------- */
                {
                    DDC_ISR_LOCK_TAKE(pDeviceContext->sSerialIORxHBuf.sHBuf[u16Channel].hMutex, pDeviceContext->sSerialIORxHBuf.sHBuf[u16Channel].slSerialIOModeFlag);
                    pu32Status[0] = pDeviceContext->u32CastIO_bdIntStatus[0]; /* Returns channel Interrupted */
					pu32Status[1] = pDeviceContext->u32CastIO_bdIntStatus[1]; /* Returns number of bytes added to hbuf */
					pu32Status[2] = pDeviceContext->u32CastIO_bdIntStatus[2]; /* Returns MIO_IIR register (bits 0 - 3) */
                    DDC_ISR_LOCK_GIVE(pDeviceContext->sSerialIORxHBuf.sHBuf[u16Channel].hMutex, pDeviceContext->sSerialIORxHBuf.sHBuf[u16Channel].slSerialIOModeFlag);
                    *pBytesReturned = sizeof(U32BIT) * 3;
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_CAN:
                /* --------------------- */
                {
                    pu32Status[0] = (pDeviceContext->u32CanBusIntStatus.u32BdInfoInt & 0xffff);
                    pu32Status[1] = pDeviceContext->u32CanBusIntStatus.u32MsgCountStatus;
                    /* db_printf("GET_INTERRUPT_STATUS %x %x \n", pu32Status[0],pu32Status[1]);*/
                    pDeviceContext->u32CanBusIntStatus.u32BdInfoInt = 0;
                    pDeviceContext->u32CanBusIntStatus.u32MsgCountStatus = 0;

                    /* need to fix the hard defined value here - revisit */
                    *pBytesReturned = CAN_STATUS_RETURN_BYTE_COUNT;
                  break;
                }
                /* --------------------- */
                case CHAN_TYPE_TT:
                /* --------------------- */
                {
                    /* rollover - nothing to return */
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_VOLT_MON:
                /* --------------------- */
                {
                    /* nothing to return */
                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_717_PROG:
                /* --------------------- */
                {
                    U32BIT u32BytesRead;

                    u32BytesRead = (U32BIT)(sizeof(*pu32Status) * (2 + pDeviceContext->u8NumProg717));

                    if (OutputBufferLength >= u32BytesRead)
                    {
                        ARINC_717_INTERRUPT *pArinc717Interrupt = (ARINC_717_INTERRUPT *) pLocalInOutBuffer;
                        U16BIT i;

                        /* send back the master, 717 global, and 717 channel interrupt status values */

                        u32RegisterAddress = *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_STATUS;

                        pArinc717Interrupt->u32MasterStatus = 0x00000000; /* NOTE: master was read previously and can not be read again */

                        /* 717 Global General */

                        /* NOTE: */
                        /*  when the ARINC 717 Interrupt Enable bit is cleared in the Global Configuration Register (0x02) */
                        /*  this clears out the REG_ARINC_717_GLOBAL_INT_STATUS bits. we will need to create the contents  */
                        /*  of this register when reading the channel registers later on */
#if 0
                        u32RegisterAddress = *(pDeviceContext->sArincGlobal717.pu32RegBA) + REG_ARINC_717_GLOBAL_INT_STATUS;
                        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &pArinc717Interrupt->u32General717);
#endif

                        pu32Status++;

                        pArinc717Interrupt->u32General717 = 0x00000000;

                        for (i = 0; i < pDeviceContext->u8NumProg717; i++)
                        {
                            u32RegisterAddress = *(pDeviceContext->sChannelArinc717[i].pu32RegBA) + REG_ARINC_717_CH_INT_STATUS;
                            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &pArinc717Interrupt->u32RxStatus[i]);

                            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_GENERAL, "ARINC717 u32RxStatus[%d] 0x%08x\n", i, pArinc717Interrupt->u32RxStatus[i]);

                            if (pArinc717Interrupt->u32RxStatus[i] != 0x00000000)
                            {
                                pArinc717Interrupt->u32General717 |= (0x000000001 << i);
                            }
                        }

                        *pBytesReturned = sizeof(*pArinc717Interrupt);
                    }
                    else
                    {
                        status = DDC_UDL_ERROR__BUFFER_SIZE;
                    }

                    break;
                }

                /* --------------------- */
                case CHAN_TYPE_AIO:
                /* --------------------- */
                {
                    /* send back the interrupt status register value */
                    pu32Status[0] = ddcUdlOsBusGetInterruptStatus(
                        pDeviceContext,
                        UM_DEVICE_ID_BRD_GLOBAL,
                        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA),
                        REG_BD_AIO_INTERRUPT_STATUS);

                    break;
                }

                /* --------------------- */
                default:
                /* --------------------- */
                {
                    /* ERROR: unsupported channel type */
                    status = ERR_CHAN_TYPE;
                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__RESET_429_PROGRAMMABLE:
        /* ---------------------------------------------------- */
        {
            ARINC_429_PROGRMMABLE_CONFIG sConfig;
            DDC_IOCTL_PARAMS tempIoctlParams;
            U32BIT u32RegisterAddress;
            U32BIT u32RegisterValue;
            U32BIT u32TempData;
            U16BIT u16Index;
            S16BIT s16Result;
            size_t tempOutputBufferLength;
            size_t tempBytesReturned;

            /* Disable channel and place type in undefined state */
            sConfig.sConfigID.u16Channel = (U16BIT)u16Channel;
            sConfig.u32ConfigOption = ARINC_429_PROGRMMABLE_RESET_OPT;
            sConfig.bReset = TRUE;

            status = ARINC429ProgrammableConfig(pDeviceContext, &sConfig);
            if (status)
            {
                status = ERR_429_PROG_RESET;
                break;
            }

            s16Result = _ARINC429TxSetStateBusy(pDeviceContext);
            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                return s16Result;
            }

            if (!pDeviceContext->bExtendedScheduler)
            {
                /* Select the channel */
                u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_CHANNEL_SELECT_REG);
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u16Channel);

                /* Disable the scheduler */
                u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_SCHEDULER_DISABLE_REG);
                u32RegisterValue = 0x00000001;
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            }

            ARINC429TxSetStateIdle(pDeviceContext);

            /* we do this to clear everything to 0 and disabled in ClearRepeated we only clear the items which are active */
            if (!pDeviceContext->bExtendedScheduler)
            {
                /* Clear scheduler data */
                for (u16Index = 0; u16Index < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16Index++)
                {
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16Index), 0x00000000);
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u16Index), 0x00000000);
                    pDeviceContext->U.sTxScheduler.u32Data[u16Index] = (U32BIT)0x00000000; /* Tx data */
                }

                /* Clear the other queues */
                for (u16Index = 0; u16Index < DD429_TX_SCHED_QUEUE_DEPTH; u16Index++)
                {
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16Index), DD429_NOT_ACTIVE);
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16Index), 0x00000000);
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16Index), 0x00000000);
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16Index), 0x00000000);
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET + u16Index), 0x00000000);
                    pDeviceContext->U.sTxScheduler.u16Channel[u16Index] = (U8BIT)0x00;
                    pDeviceContext->U.sTxScheduler.u32Address[u16Index] = (U32BIT)0x00000000; /* Tx address */
                }
            }
            else
            {
                SCHEDULER_DATA SchedulerData;
                SchedulerData.u32Channel = u16Channel;
                SchedulerData.u32Data = 0x00000000;
                SchedulerData.u32Control = 0x00000000;
                SchedulerData.u16Frequency = 0x0000;
                SchedulerData.u16Offset = 0x0000;
                SchedulerData.u8Active = DD429_NOT_ACTIVE;
                for (u16Index=0; u16Index < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16Index++)
                {
                    SchedulerData.u16Index = u16Index;
                    _arinc429SetRepeated(pDeviceContext,
                        DD429_SCHED_DATA_UPDATE_MASK | DD429_SCHED_CONTROL_UPDATE_MASK | DD429_SCHED_FREQUENCY_UPDATE_MASK | DD429_SCHED_OFFSET_UPDATE_MASK | DD429_SCHED_ACTIVE_UPDATE_MASK,
                        &SchedulerData);
                }
            }
            /* Clear receiver queue */
            tempIoctlParams.Channel = u16Channel;
            tempIoctlParams.Param1 = DD429_FIFO_COMMAND__CLEAR_RX_QUEUE;
            tempOutputBufferLength = sizeof(u32TempData);
            _ARINC429FifoCommand(pDeviceContext, &tempIoctlParams, (U32BIT *) &u32TempData, tempOutputBufferLength, &tempBytesReturned);

            /* Reset Rx mailbox */
            tempIoctlParams.Channel = u16Channel;
            tempIoctlParams.Param1 = DD429_MAILBOX_COMMAND__CLEAR_MAILBOX;
            tempOutputBufferLength = sizeof(u32TempData);
            _ARINC429MailboxCommand(pDeviceContext, &tempIoctlParams, (U32BIT *) &u32TempData, tempOutputBufferLength, &tempBytesReturned);

            /* Set TX and RX to low speed */
            tempIoctlParams.Channel = u16Channel;
            tempIoctlParams.Param1 = DD429_TX_COMMAND__SET_TX_SPEED;
            tempIoctlParams.Param2 = DD429_LOW_SPEED;
            _ARINC429TxCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &tempBytesReturned);

            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__SPEED;
            tempIoctlParams.Param2 = DD429_LOW_SPEED;
            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            tempIoctlParams.Param1 = DD429_TESTER_COMMAND__SET_TX_VARIABLE_SPEED;
            tempIoctlParams.Param2 = DD429_CHANNEL_SPEED_BPS_DEFAULT;
            _ARINC429TesterCommand(pDeviceContext, &tempIoctlParams, &u32TempData, sizeof(u32TempData), pBytesReturned);

            tempIoctlParams.Param1 = DD429_TESTER_COMMAND__SET_RX_VARIABLE_SPEED;
            tempIoctlParams.Param2 = DD429_CHANNEL_SPEED_BPS_DEFAULT;
            _ARINC429TesterCommand(pDeviceContext, &tempIoctlParams, &u32TempData, sizeof(u32TempData), pBytesReturned);

            /* Set TX and RX to odd parity */
            tempIoctlParams.Param1 = DD429_TX_COMMAND__SET_TX_PARITY;
            tempIoctlParams.Param2 = DD429_ODD_PARITY;
            _ARINC429TxCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &tempBytesReturned);

            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__PARITY;
            tempIoctlParams.Param2 = DD429_ODD_PARITY;
            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            /* Disable loopback and Clear Mapping */
            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__WRAP_AROUND;
            tempIoctlParams.Param2 = DD429_DISABLE;
            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            _ARINC429SetLoopbackMapping(pDeviceContext, u16Channel, 0, TRUE);
            _ARINC429SetLoopbackMapping(pDeviceContext, u16Channel, 32, TRUE);

            /* Disable and reset Rx Time stamp */
            tempIoctlParams.Param1 = DD429_RX_CONTROL_COMMAND_SET__TIMETAG;
            tempIoctlParams.Param2 = DD429_DISABLE;
            _ARINC429RxControlCommandSet(pDeviceContext, &tempIoctlParams);

            tempIoctlParams.Param1 = DD429_RX_COMMAND__RESET_TIMESTAMP;
            _ARINC429RxCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &tempBytesReturned);

            /* Reset Rx filter */
            tempIoctlParams.Param1 = DD429_FILTER_COMMAND__ENABLE;
            tempIoctlParams.Param2 = DD429_DISABLE;
            _ARINC429FilterCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &tempBytesReturned);

            tempIoctlParams.Param1 = DD429_FILTER_COMMAND__CONFIG;
            tempIoctlParams.Param2 = FILTER_LABEL_SDI;
            _ARINC429FilterCommand(pDeviceContext, &tempIoctlParams, NULL, 0, &tempBytesReturned);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__CONFIG_INTERRUPT_CONDITIONS:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS *pIoConfigureOutput = (DDC_IOCTL_PARAMS *) pLocalInOutBuffer;

            status = ddcUdlBdConfigureIoInterruptConditions(
                pDeviceContext,
                (U16BIT)pIoctlParams->Channel,  /* Channel (not used for 429) */
                pIoctlParams->Param2,   /* Command */
                pIoctlParams->Param3,   /* Rising Edge Enable/Disable */
                pIoctlParams->Param4,   /* Falling Edge Enable/Disable */
                0x00000000,             /* Reserved */
                pIoConfigureOutput
            );

            *pBytesReturned = sizeof(*pIoConfigureOutput);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_GENERAL_COMMAND__GET_IRIG_RX_LATCHED_TIME:
        /* ---------------------------------------------------- */
        {
            DD429_READ_DATA_IRIG__OUTPUT_TYPE *psReadRxFifoIrig = (DD429_READ_DATA_IRIG__OUTPUT_TYPE *) pLocalInOutBuffer;

            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sIrigB_RX[0].pu32RegBA) + REG_IRIGB_LATCHED_1SEC_TIME_STAMP_MSB_RW, &psReadRxFifoIrig->u32StampHi);
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->sIrigB_RX[0].pu32RegBA) + REG_IRIGB_LATCHED_1SEC_TIME_STAMP_LSB_RW, &psReadRxFifoIrig->u32StampLo);

            psReadRxFifoIrig->u32Error = 0;

            *pBytesReturned = sizeof(*psReadRxFifoIrig);

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_GENERAL, "429 - _ARINC429GeneralCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);

            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429ControlCommand
 *
 * Description:
 *
 *      This function processes DD429_CONTROL_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429ControlCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_CONTROL, "429 - _ARINC429ControlCommand(CH:%d) - Cmd %d, ch %d\n", pIoctlParams->Channel, pIoctlParams->Param1, pIoctlParams->Param2);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_AVIONIC_OUTPUT:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;

            /* remap IOCTL parameters */
            tempIoctlParams.Param1 = pIoctlParams->Param2; /* avionic discrete */
            tempIoctlParams.Param2 = pIoctlParams->Param3; /* level */

            status = SetAioOutput(pDeviceContext, &tempIoctlParams);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_AVIONIC_OUTPUT:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;

            /* remap IOCTL parameters */
            tempIoctlParams.Param1 = pIoctlParams->Param2; /* avionic discrete */

            *pLocalInOutBuffer = GetAioOutput(pDeviceContext, &tempIoctlParams);
            *pBytesReturned = sizeof(S16BIT);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_AVIONIC_OUTPUT_ENABLE:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;

            /* remap IOCTL parameters */
            tempIoctlParams.Param1 = pIoctlParams->Param2; /* avionic discrete */
            tempIoctlParams.Param2 = pIoctlParams->Param3; /* direction */

            status = SetAioDirection(pDeviceContext, &tempIoctlParams);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_AVIONIC_OUTPUT_ENABLE:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;

            /* remap IOCTL parameters */
            tempIoctlParams.Param1 = pIoctlParams->Param2; /* avionic discrete */

            *pLocalInOutBuffer = GetAioDirection(pDeviceContext, &tempIoctlParams);

            *pBytesReturned = sizeof(U32BIT);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_AVIONIC_INPUT:
        /* ---------------------------------------------------- */
        {
            DDC_IOCTL_PARAMS tempIoctlParams;

            /* remap IOCTL parameters */
            tempIoctlParams.Param1 = pIoctlParams->Param2; /* avionic discrete */

            *pLocalInOutBuffer = GetAioInput(pDeviceContext, &tempIoctlParams);
            *pBytesReturned = sizeof(U32BIT);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_AVIONIC_ALL:
        /* ---------------------------------------------------- */
        {
            *pLocalInOutBuffer = GetAioAll(pDeviceContext);
            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_AVIONIC_ALL:
        /* ---------------------------------------------------- */
        {
            SetAioAll(pDeviceContext, DDC_IOCTL_U32(pIoctlParams->Param2));
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_DIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            /* Param2 - DIO discrete */
            /* Param3 - level */
            status = (S16BIT) SetDioOutput(pDeviceContext, pIoctlParams->Param2, pIoctlParams->Param3);

#if 0
            /* translate the error to a 429 library error value if needed */
            if (*pLocalInOutBuffer == ACEX_ERR_AVIONIC)
                *pLocalInOutBuffer = (U32BIT)ERR_AVIONIC;
#endif
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_DIO_OUTPUT:
        /* ---------------------------------------------------- */
        {
            /* Param2 - DIO discrete */
            *pLocalInOutBuffer = GetDioOutput(pDeviceContext, pIoctlParams->Param2);
            *pBytesReturned = sizeof(U32BIT);

#if 0
            /* translate the error to a 429 library error value if needed */
            if (*pLocalInOutBuffer == ACEX_ERR_AVIONIC)
                *pLocalInOutBuffer = (U32BIT)ERR_AVIONIC;
#endif
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_DIO_OUTPUT_ENABLE:
        /* ---------------------------------------------------- */
        {
            /* Param2 - DIO discrete */
            /* Param3 - direction */
            status = SetDioDirection(pDeviceContext, pIoctlParams->Param2, pIoctlParams->Param3);

#if 0
            /* translate the error to a 429 library error value if needed */
            if (*pLocalInOutBuffer == ACEX_ERR_AVIONIC)
                *pLocalInOutBuffer = (U32BIT)ERR_AVIONIC;
#endif
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_DIO_OUTPUT_ENABLE:
        /* ---------------------------------------------------- */
        {
            /* Param2 - DIO discrete */
            *pLocalInOutBuffer = GetDioDirection(pDeviceContext, pIoctlParams->Param2);
            *pBytesReturned = sizeof(U32BIT);

#if 0
            /* translate the error to a 429 library error value if needed */
            if (*pLocalInOutBuffer == ACEX_ERR_AVIONIC)
                *pLocalInOutBuffer = (U32BIT)ERR_AVIONIC;
#endif
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_DIO_INPUT:
        /* ---------------------------------------------------- */
        {
            /* Param2 - DIO discrete */
            *pLocalInOutBuffer = GetDioInput(pDeviceContext, pIoctlParams->Param2);
            *pBytesReturned = sizeof(U32BIT);
#if 0
            /* translate the error to a 429 library error value if needed */
            if (*pLocalInOutBuffer == ACEX_ERR_AVIONIC)
                *pLocalInOutBuffer = (U32BIT)ERR_AVIONIC;
#endif
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__GET_DIO_ALL:
        /* ---------------------------------------------------- */
        {
            *pLocalInOutBuffer = GetDioAll(pDeviceContext);
            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_CONTROL_COMMAND__SET_DIO_ALL:
        /* ---------------------------------------------------- */
        {
            SetDioAll(pDeviceContext, pIoctlParams->Param2);
            break;
        }


        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_CONTROL, "429 - _ARINC429ControlCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    ARINC429ProgrammableConfig
 *
 * Description:
 *
 *      This function sets up the programmable channels.
 *
 * In:  pDeviceContext          device context
 * In:  pConfig                 pointer to programmable configuration structure
 * Out: ps16Error               pointer to error value
 * Out: return                  error condition
 ******************************************************************************/
S16BIT ARINC429ProgrammableConfig
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    ARINC_429_PROGRMMABLE_CONFIG *pConfig
)
{
    U16BIT u16Channel = pConfig->sConfigID.u16Channel;
    U32BIT u32RegisterValue = 0;

    /* Max channel check; 0 based, therefore deduct 1 from max for comparison purpose only */
    if (u16Channel > (pDeviceContext->u8NumProg429RxTx - 1))
    {
        return ERR_INVALID_CHANNEL_NO;
    }

    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) || (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H))
    {
        if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_TYPE_OPT) == ARINC_429_PROGRMMABLE_TYPE_OPT)
        {
            /* Due to programmability feature, it is possible to define channel as a transmitter and receiver.
             * Therefore, for flexibility purpose, it will be allowed in driver */
            DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX), &u32RegisterValue);

            /* Transmitter/Receiver selection */
            if (pConfig->bType == ARINC429_TX_CHANNEL)
            {
                u32RegisterValue |= (U32BIT)((U32BIT)ARINC_429_PROGRAMMABLE_CH1_TX << u16Channel);
            }
            else if (pConfig->bType == ARINC429_RX_CHANNEL)
            {
                u32RegisterValue |= ((U32BIT)ARINC_429_PROGRAMMABLE_CH1_RX << u16Channel);
            }
            else /* defaults to ARINC429_UNDEFINED_CHANNEL (Undefined) */
            {
                /* Clear Tx/Rx mask 1st. Makes channel undefined to due both Tx and Rx relays will be placed in a open state */
                u32RegisterValue &= ~(U32BIT)((U32BIT)ARINC_429_PROGRAMMABLE_MASK << u16Channel);
            }

            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX, &u32RegisterValue);

            if (u16Channel < 2)
            {
                DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX_2, &u32RegisterValue);

                if (pConfig->bType == ARINC429_TX_CHANNEL)
                {
                    u32RegisterValue |= (ARINC_429_PROGRAMMABLE2_CH1_TX << u16Channel);
                }
                else if (pConfig->bType == ARINC429_RX_CHANNEL)
                {
                    if (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H)
                    {
                        /*
                         D40001 when switching from 717 back to 429 mode we need
                         to disable the tx enable, need to check with Walter to see if
                         this is the same on the bu67118.
                        */
                        u32RegisterValue |= (ARINC_429_PROGRAMMABLE2_CH1_TX << u16Channel);
                    }
                    else
                    {
                        u32RegisterValue &= ~(ARINC_429_PROGRAMMABLE2_CH1_TX << u16Channel);
                    }
                }

                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RX_TX_2, &u32RegisterValue);
            }
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_SPEED_OPT) == ARINC_429_PROGRMMABLE_SPEED_OPT)
    {
        /* High/Low Speed selection */
        if (pConfig->u8Speed == ARINC_429_PROGRMMABLE_SPEED_OPT)
        {
        }
        else
        {
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_PARITY_OPT) == ARINC_429_PROGRMMABLE_PARITY_OPT)
    {
        /* Parity selection */
        if (pConfig->u8Parity == ARINC_429_PROGRMMABLE_PARITY_OPT)
        {
        }
        else
        {
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_ENABLE_OPT) == ARINC_429_PROGRMMABLE_ENABLE_OPT)
    {
        DDC_REG_READ(pDeviceContext, (*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RELAY), &u32RegisterValue);

        /* Enable/Disaable selection */
        if (pConfig->bEnable == ARINC_429_PROGRMMABLE_ENABLE_OPT)
        {
            /* Connect to interface via releay closure */
            u32RegisterValue |= (U32BIT)((U32BIT)ARINC_429_PROGRAMMABLE_BUS_ISOLATION_ENABLE << u16Channel);
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RELAY, &u32RegisterValue);
        }
        else
        {
            /* Disconnect channel from interface via relay deactivation */
            u32RegisterValue &= ~(U32BIT)((U32BIT)ARINC_429_PROGRAMMABLE_BUS_ISOLATION_ENABLE << u16Channel);
            DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_P429_RELAY, &u32RegisterValue);
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_BIT_FORMAT_OPT) == ARINC_429_PROGRMMABLE_BIT_FORMAT_OPT)
    {
        /* FIFO/MAILBOX selection */
        if (pConfig->u8BitFormat == ARINC_429_PROGRMMABLE_BIT_FORMAT_OPT)
        {
        }
        else
        {
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_MODE_OPT) == ARINC_429_PROGRMMABLE_MODE_OPT)
    {
        /* FIFO/MAILBOX selection */
        if (pConfig->u8Mode == ARINC_429_PROGRMMABLE_MODE_OPT)
        {
        }
        else
        {
        }
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_ENABLE_TX_PASSIVE_OPT) == ARINC_429_PROGRMMABLE_ENABLE_TX_PASSIVE_OPT)
    {
        /* Do nothing for now. Need to re-visit (BAE resolution) */
    }

    if ((pConfig->u32ConfigOption & ARINC_429_PROGRMMABLE_RESET_OPT) == ARINC_429_PROGRMMABLE_RESET_OPT)
    {
        /* Reset Channel selection */
        if (pConfig->bReset == TRUE)
        {
            /* Reset channel's Rx and Tx configurations */
            DDC_REG_READ(pDeviceContext, (*pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA + ACEX_429_TX_CONTROL_REG), &u32RegisterValue);
            u32RegisterValue |= ARINC_429_PROGRAMMABLE_TX_RESET;
            DDC_REG_WRITE(pDeviceContext, (*pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA + ACEX_429_TX_CONTROL_REG), &u32RegisterValue);
            u32RegisterValue &= ~ARINC_429_PROGRAMMABLE_TX_RESET;   /* Take out of reset */
            DDC_REG_WRITE(pDeviceContext, (*pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA + ACEX_429_TX_CONTROL_REG), &u32RegisterValue);

            DDC_REG_READ(pDeviceContext, (*pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA + ACEX_429_RX_CONTROL_REG), &u32RegisterValue);
            u32RegisterValue |= ARINC_429_PROGRAMMABLE_RX_RESET;
            DDC_REG_WRITE(pDeviceContext, (*pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA + ACEX_429_RX_CONTROL_REG), &u32RegisterValue);
            u32RegisterValue &= ~ARINC_429_PROGRAMMABLE_RX_RESET;   /* Take out of reset */
            DDC_REG_WRITE(pDeviceContext, (*pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA + ACEX_429_RX_CONTROL_REG), &u32RegisterValue);
        }
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    _ARINC429TxCommand
 *
 * Description:
 *
 *      This function processes DD429_TX_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429TxCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    U32BIT u16Channel;
    U32BIT u32RegisterAddress;

    U32BIT u32RegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TX, "429 - _ARINC429TxCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__ENABLE_TX:
        /* ---------------------------------------------------- */
        {
            ARINC_429_PROGRMMABLE_CONFIG sConfig;
            U32BIT u32EnableValue = DDC_IOCTL_U32(pIoctlParams->Param2);
            U32BIT u32PassiveValue = DDC_IOCTL_U32(pIoctlParams->Param3);
            S16BIT s16Result;

            sConfig.u32ConfigOption = ARINC_429_PROGRMMABLE_TYPE_OPT;
            sConfig.bEnable = (U8BIT)u32EnableValue;
            sConfig.sConfigID.u16Channel = (U16BIT)u16Channel;

            if (u32PassiveValue == FALSE)
            {
                sConfig.u32ConfigOption |= ARINC_429_PROGRMMABLE_ENABLE_OPT;
            }

            /* If disabling channel, force channel to Undefined */
            if (u32EnableValue == FALSE)
            {
                sConfig.bType = ARINC429_UNDEFINED_CHANNEL;
            }
            else
            {
                sConfig.bType = ARINC429_TX_CHANNEL;
            }

            /* put TX into reset and disable */
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            u32RegisterValue |= DD429_TX_CONTROL_MASK__RESET;
            u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__ENABLE);
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /* select the TX channel */

            s16Result = _ARINC429TxSetStateBusy(pDeviceContext);
            if (s16Result != DDC_UDL_ERROR__SUCCESS)
            {
                return s16Result;
            }

            if (!pDeviceContext->bExtendedScheduler)
            {
                u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_CHANNEL_SELECT_REG);
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u16Channel);
            }

            if (u32EnableValue)
            {
                /* take TX out of reset and enable */
                u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__RESET);
                u32RegisterValue |= DD429_TX_CONTROL_MASK__ENABLE;

                /* always set variable amplitude to max even if channel does not support feature */
                u32RegisterValue &= ~(DD429_TX_VARIABLE_AMPLITUDE_MASK);
                u32RegisterValue |= (DD429_TX_VARIABLE_AMPLITUDE_DEFAULT << DD429_TX_VARIABLE_AMPLITUDE_OFFSET);

                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                /* enable the Scheduler */
                if (!pDeviceContext->bExtendedScheduler)
                {
                    u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + ACEX_429_TX_GLOBAL_SCHEDULER_DISABLE_REG;
                    u32RegisterValue = 0x00000000;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                }
                /* need to set type and connection */
                status = ARINC429ProgrammableConfig(pDeviceContext, &sConfig);
                if (status)
                {
                    ARINC429TxSetStateIdle(pDeviceContext);
                    break;
                }
            }
            else
            {
                DDC_IOCTL_PARAMS tempIoctlParams;

                status = ARINC429ProgrammableConfig(pDeviceContext, &sConfig);
                if (status)
                {
                    ARINC429TxSetStateIdle(pDeviceContext);
                    break;
                }

                /* disable the Scheduler */
                if (!pDeviceContext->bExtendedScheduler)
                {
                    u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + ACEX_429_TX_GLOBAL_SCHEDULER_DISABLE_REG;
                    u32RegisterValue = 0x00000001;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                }

                /* disable the and clear the Frame */
                tempIoctlParams.Channel = u16Channel;
                tempIoctlParams.Param1 = DD429_TESTER_COMMAND__SET_TX_FRAME_CONTROL;
                tempIoctlParams.Param2 = DD429_TX_FRAME_STOP;
                _ARINC429TesterCommand(pDeviceContext, &tempIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);

            }

            ARINC429TxSetStateIdle(pDeviceContext);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__SET_TX_PARITY:
        /* ---------------------------------------------------- */
        {
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;

            /* read the TX Control register */
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /* clear out setting first and default to odd parity */
            u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__PARITY);

            switch (pIoctlParams->Param2)
            {
                case DD429_EVEN_PARITY:
                {
                    u32RegisterValue |= DD429_TX_PARITY_MASK__EVEN;
                    break;
                }

                case DD429_NO_PARITY:
                {
                    u32RegisterValue |= DD429_TX_PARITY_MASK__NO;
                    break;
                }
            }

            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__SET_TX_SPEED:
        /* ---------------------------------------------------- */
        {
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;

            /* read the TX Control register */
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            switch (pIoctlParams->Param2)
            {
                case DD429_ARINC575:
                {
                    /* set bit 6 to 1, ARINC575 and set bit 4 to 1, low speed */
                    u32RegisterValue |= DD429_TX_CONTROL_MASK__ARINC575;
                    u32RegisterValue |= DD429_TX_CONTROL_MASK__SPEED;
                    break;
                }

                case DD429_LOW_SPEED:
                {
                    /* set 4 bit to 1, low speed, and set bit 6 to 0, ARINC429 */
                    u32RegisterValue |= DD429_TX_CONTROL_MASK__SPEED;
                    u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__ARINC575);
                    break;
                }

                case DD429_HIGH_SPEED:
                {
                    /* set bit 4 to 0, high speed, and set bit 6 to 0, ARINC429 */
                    u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__SPEED);
                    u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__ARINC575);
                    break;
                }

                default:
                {
                    u32RegisterValue &= 0xFFFFFFFF;
                    break;
                }
            }

            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__GET_TX_QUEUE_STATUS:
        /* ---------------------------------------------------- */
        {
            if (OutputBufferLength >= sizeof(U32BIT))
            {
                u32RegisterAddress = *(pDeviceContext->sArinc429TxGlobal.pu32RegBA) + (ACEX_429_TX_GLOBAL_MESSAGE_FIFO_STATUS_0_REG + u16Channel);
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                *pLocalInOutBuffer = ((u32RegisterValue & DD429_TX_FIFO_MASK__COUNT) >> DD429_TX_FIFO_OFFSET__COUNT);

                *pBytesReturned = sizeof(U32BIT);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__LOAD_TX_QUEUE_ONE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32TxData = DDC_IOCTL_U32(pIoctlParams->Param2);

            status = _ARINC429SendAsync(pDeviceContext, u16Channel, u32TxData, DD429_CONTROL_WORD__NO_OPTIONS);

            if (status == DDC_UDL_ERROR__SUCCESS)
            {
                /* indicate to user that 1 message was sent, otherwise we will send the error status */
                status = 1;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__LOAD_TX_QUEUE_MORE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32NumLoaded = 0;
            S16BIT s16Error;
            U32BIT u32DataIndex;
            U32BIT u32NumItems = pIoctlParams->Param2;
            U32BIT *pu32Data = NULL;

            /* reassign pointer to new the buffer */
            pu32Data = pLocalInOutBuffer;

            for (u32DataIndex = 0; u32DataIndex < u32NumItems; u32DataIndex++)
            {
                s16Error = _ARINC429SendAsync(pDeviceContext, u16Channel, pu32Data[u32DataIndex], DD429_CONTROL_WORD__NO_OPTIONS);
                if (s16Error != DDC_UDL_ERROR__SUCCESS)
                {
                    status = (S16BIT) s16Error;
                    break;
                }

                u32NumLoaded += 1;
            }

            /* return the number of messages sent if status has not already been set */
            if (status == DDC_UDL_ERROR__SUCCESS)
            {
                status =  (S16BIT)u32NumLoaded;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__GET_TX_QUEUE_FREE_COUNT:
        /* ---------------------------------------------------- */
        {
            U32BIT *pu32TxQueueFreeCount = pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(U32BIT))
            {
                /* Check Capability Bit for HI priority fifo data availability counter */
                if (pDeviceContext->sHwVersionInfo.dwCapabilities2 & HWVER_CAPABILITY2_ARINC_429_HI_PRIORITY_FIFO_FREE_COUNTER)
                {
                    /* read the channel FIFO status register */
                    u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + (ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_REG);
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    u32RegisterValue &= (ACEX_429_TX_HI_ASYNC_FIFO_STATUS_MSG_FREE_COUNT_MASK);
                    u32RegisterValue >>= DD429_TX_FIFO_OFFSET__FREE_COUNT;

                    *pu32TxQueueFreeCount = u32RegisterValue / 2;
                    *pBytesReturned = sizeof(U32BIT);
                }
                else
                {
                    status = ERR_UNKNOWN;
                }
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__CLEAR_REPEATED:
        /* ---------------------------------------------------- */
        {
            U16BIT u16QueueIndex;

            U32BIT u32ActiveData = 0;
            U32BIT u32QueueChannel = 0;
            U32BIT u32DataPointer = 0;

            if (!pDeviceContext->bExtendedScheduler)
            {
                for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                {
                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);
                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);

                    /* check if the item is active and the transmitter channel matches */
                    if ((u32ActiveData == DD429_ACTIVE) && (u32QueueChannel == u16Channel))
                    {
                        /* save the data pointer */
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), &u32DataPointer);

                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), DD429_NOT_ACTIVE);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), 0x00000000);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), 0x00000000);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16QueueIndex), 0x00000000);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET + u16QueueIndex), 0x00000000);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u32DataPointer), 0x00000000);
                        DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u32DataPointer), 0x00000000);

                        pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex] = (U16BIT)0x0000;
                        pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] = (U32BIT)0x00000000;     /* Tx data */
                        pDeviceContext->U.sTxScheduler.u32Address[u16QueueIndex] = (U32BIT)0x00000000;  /* Tx address */
                    }
                }
            }
            else /*clear_rpt*/
            {
                /*printf("Clear RPT\n");*/
                SCHEDULER_DATA SchedulerData;
                SchedulerData.u32Channel = u16Channel;
                SchedulerData.u32Data = 0x00000000;
                SchedulerData.u32Control = 0x00000000;
                SchedulerData.u16Frequency = 0x0000;
                SchedulerData.u16Offset = 0x0000;
                SchedulerData.u8Active = DD429_NOT_ACTIVE;

                for (u16QueueIndex=0; u16QueueIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16QueueIndex++)
                {
                    u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (u16QueueIndex);
                    DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                    /* check if the item is active and the transmitter channel matches */
                    if (u32RegisterValue == DD429_ACTIVE)
                    {
                        SchedulerData.u16Index = u16QueueIndex;
                        _arinc429SetRepeated(pDeviceContext,
                            DD429_SCHED_DATA_UPDATE_MASK | DD429_SCHED_CONTROL_UPDATE_MASK | DD429_SCHED_FREQUENCY_UPDATE_MASK | DD429_SCHED_OFFSET_UPDATE_MASK | DD429_SCHED_ACTIVE_UPDATE_MASK,
                            &SchedulerData);
                    }
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__DEL_REPEATED:
        /* ---------------------------------------------------- */
        {
            U32BIT u32DataLabelSDI = 0;
            U32BIT u32DataSDI = 0;
            U32BIT u32ActiveData = 0;
            U32BIT u32QueueChannel = 0;
            U32BIT u32DataPointer = 0;
            U16BIT u16DataIndex = 0;
            U16BIT u16QueueIndex = 0;
            U8BIT bMatch = FALSE;

            if (!pDeviceContext->bExtendedScheduler)
            {
                for (u16DataIndex = 0; u16DataIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16DataIndex++)
                {
                    if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
                    {
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32DataLabelSDI);
                        u32DataLabelSDI &= DD429_LABEL_SDI_MASK;
                    }
                    else
                    {
                        U32BIT u32Data = 0;
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32Data);

                        /* convert alt format to normal */
                        u32DataSDI = ((u32Data & DD429_ALT_SDI_MASK) >> DD429_ALT_TO_NORMAL_SDI_OFFSET);
                        u32DataLabelSDI = ((u32Data & DD429_LABEL_MASK) | u32DataSDI);
                    }

                    if (u32DataLabelSDI == pIoctlParams->Param2)
                    {
                        for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                        {
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), &u32DataPointer);
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);

                            if ((u32DataPointer == u16DataIndex) &&
                                (u32QueueChannel == u16Channel) &&
                                (u32ActiveData == DD429_ACTIVE))
                            {
                                bMatch = TRUE;
                                break;
                            }
                        }
                    }

                    if (bMatch == TRUE)
                    {
                        break;
                    }
                }

                /* there was no match if we reached the end */
                if (u16DataIndex == DD429_TX_SCHED_MESSAGE_MEM_DEPTH)
                {
                    status = ERR_LABELSDI;
                    break;
                }

                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), DD429_NOT_ACTIVE);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), 0x00000000);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), 0x00000000);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16QueueIndex), 0x00000000);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET + u16QueueIndex), 0x00000000);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u32DataPointer), 0x00000000);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u32DataPointer), 0x00000000);

                pDeviceContext->pTxChnl429[u16Channel]->txOffset[u32DataLabelSDI] = 0;

                pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex] = (U16BIT)0x0000;
                pDeviceContext->U.sTxScheduler.u16Offset[u16QueueIndex]          = (U16BIT)0x0000;
                pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex]    = (U32BIT)0x00000000;     /* Tx data */
                pDeviceContext->U.sTxScheduler.u32Address[u16QueueIndex] = (U32BIT)0x00000000;  /* Tx address */
            }
            else
            {
                /* check if the item is active and the transmitter channel matches */
                u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (pIoctlParams->Param2);
                DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                if (u32RegisterValue == DD429_ACTIVE)
                {
                    SCHEDULER_DATA SchedulerData;
                    SchedulerData.u32Channel = u16Channel;
                    SchedulerData.u16Index =  (U16BIT)pIoctlParams->Param2;
                    SchedulerData.u32Data = 0x00000000;
                    SchedulerData.u32Control = 0x00000000;
                    SchedulerData.u16Frequency = 0x0000;
                    SchedulerData.u16Offset = 0x0000;
                    SchedulerData.u8Active = DD429_NOT_ACTIVE;
                    _arinc429SetRepeated(pDeviceContext,
                        DD429_SCHED_DATA_UPDATE_MASK | DD429_SCHED_CONTROL_UPDATE_MASK | DD429_SCHED_FREQUENCY_UPDATE_MASK | DD429_SCHED_OFFSET_UPDATE_MASK | DD429_SCHED_ACTIVE_UPDATE_MASK,
                        &SchedulerData);
                }
                else
                {
                    status = ERR_LABELSDI;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__GET_NUM_OF_REPEATED:
        /* ---------------------------------------------------- */
        {
            if (OutputBufferLength >= sizeof(U32BIT))
            {
                U16BIT u16QueueIndex;

                U32BIT u32ActiveData = 0;
                U32BIT u32QueueChannel = 0;
                U32BIT u32Found = 0;

                if (!pDeviceContext->bExtendedScheduler)
                {
                    for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                    {
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);

                        /* check if the item is active and the transmitter channel matches */
                        if ((u32ActiveData == DD429_ACTIVE) && (u32QueueChannel == u16Channel))
                        {
                            u32Found++;
                        }
                    }
                }
                else
                {
                    for (u16QueueIndex=0; u16QueueIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16QueueIndex++)
                    {
                        u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (u16QueueIndex);
                        DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);

                        if (u32RegisterValue == DD429_ACTIVE)
                        {
                            u32Found++;
                        }
                    }
                }

                *pLocalInOutBuffer = u32Found;
                *pBytesReturned = sizeof(U32BIT);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TX_COMMAND__GET_ALL_REPEATED:
        /* ---------------------------------------------------- */
        {
            DD429_GET_ALL_REPEATED__OUTPUT_TYPE *psAllRepeated = (DD429_GET_ALL_REPEATED__OUTPUT_TYPE *) pLocalInOutBuffer;

            psAllRepeated->u32Error = DDC_UDL_ERROR__SUCCESS;

            if (OutputBufferLength >= sizeof(*psAllRepeated))
            {
                U16BIT u16QueueIndex;

                U32BIT u32DataLabelSDI = 0;
                U32BIT u32DataSDI = 0;
                U32BIT u32ActiveData = 0;
                U32BIT u32QueueChannel = 0;
                U32BIT u32DataPointer = 0;

                psAllRepeated->s16NumFound = 0;

                if (!pDeviceContext->bExtendedScheduler)
                {
                    for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                    {
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);
                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);

                        /* check if the item is active and the transmitter channel matches */
                        if ((u32ActiveData == DD429_ACTIVE) && (u32QueueChannel == u16Channel))
                        {
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), &u32DataPointer);

                            if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
                            {
                                DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u32DataPointer), &u32DataLabelSDI);
                                u32DataLabelSDI &= DD429_LABEL_SDI_MASK;
                            }
                            else
                            {
                                U32BIT u32Data = 0;
                                DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u32DataPointer), &u32Data);

                                /* convert alt format to normal */
                                u32DataSDI = ((u32Data & DD429_ALT_SDI_MASK) >> DD429_ALT_TO_NORMAL_SDI_OFFSET);
                                u32DataLabelSDI = ((u32Data & DD429_LABEL_MASK) | u32DataSDI);
                            }

                            psAllRepeated->as16Data[psAllRepeated->s16NumFound] = (S16BIT)u32DataLabelSDI;

                            psAllRepeated->s16NumFound++;
                        }
                    }
                }
                else
                {
                    pIoctlParams->Param1 = DD429_TESTER_COMMAND__GET_ALL_REPEATED_ITEM;
                    pIoctlParams->Param2 = DD429_TX_SCHED_MESSAGE_MEM_DEPTH; /* dd429x_getallrepeated assumes max buffer size */
                    _ARINC429TesterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
                }

                *pBytesReturned = sizeof(*psAllRepeated);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TX, "429 - _ARINC429TxCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/*******************************************************************************
 * Name:    _ARINC429TesterCommand
 *
 * Description:
 *
 *      This function processes DD429_TESTER_COMMAND__xxxx commands.
 *
 * In:  pDeviceContext          device context
 * In:  pIoctlParams            pointer to IOCTL command structure
 * In:  pLocalInOutBuffer       output buffer
 * In:  OutputBufferLength      length of output buffer
 * Out: pBytesReturned          pointer to number of bytes retuned in the output buffer
 * In:  pwdfRequest             pointer to WDF request object
 * Out: pbRequestPending        pointer to the request pending status
 * Out: return                  value (cmd dependent) or error condition
 ******************************************************************************/
static S16BIT _ARINC429TesterCommand
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    size_t OutputBufferLength,
    size_t *pBytesReturned
)
{
    U32BIT u16Channel;
    U32BIT u32RegisterAddress;

    U32BIT u32RegisterValue = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;

    u16Channel = pIoctlParams->Channel;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TESTER,"429 - _ARINC429TesterCommand(CH:%d) - Cmd %d\n", pIoctlParams->Channel, pIoctlParams->Param1);

    switch (pIoctlParams->Param1)
    {
        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__ADD_REPEATED:
        /* ---------------------------------------------------- */
        {
            S16BIT s16Match;
            S16BIT s16LabelSDI;
            U16BIT u16QueueIndex;
            U16BIT u16DataIndex;

            U32BIT u32Data = pIoctlParams->Param2;
            U32BIT u32TesterOptions = pIoctlParams->Param3;
            S16BIT s16Frequency = (S16BIT)(pIoctlParams->Param4);
            S16BIT s16Offset = (S16BIT)(pIoctlParams->Param5);
            S16BIT s16RepeatCount = 0;

            *pLocalInOutBuffer = 0;
            *pBytesReturned = sizeof(U32BIT);

            /* Currently issues exist for offset values of 0 & 1 during startup */
            if ((s16Offset == 0) || (s16Offset == 1))
            {
                s16Offset = 2;
            }

            if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
            {
                s16LabelSDI = (S16BIT)(u32Data & DD429_LABEL_SDI_MASK);
            }
            else
            {
                s16LabelSDI = (S16BIT)(((u32Data & DD429_ALT_SDI_MASK) >> DD429_ALT_TO_NORMAL_SDI_OFFSET) || (u32Data & DD429_LABEL_MASK));
            }

            if (!pDeviceContext->bExtendedScheduler)
            {
                /* Determine the number of words currently loaded for this channel */
                for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                {
                    U32BIT u32ActiveData = 0;
                    U32BIT u32QueueChannel = 0;

                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);
                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);

                    /* check if the item is active and the transmitter channel matches */
                    if ((u32ActiveData == DD429_ACTIVE) && (u32QueueChannel == u16Channel))
                    {
                        s16RepeatCount++;
                    }
                }

                /* If the entire queue is full, return error */
                if (s16RepeatCount == DD429_TX_SCHED_QUEUE_DEPTH)
                {
                    *pLocalInOutBuffer = (U32BIT)ERR_TXQUEUESZ;
                    break;
                }

                /* Look for an entry with the same Label/SDI and channel and */
                /* simply update the data, frequency and offset then exit */
                for (u16DataIndex = 0; u16DataIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16DataIndex++)
                {
                    U32BIT u32QueueData = 0;

                    s16Match = 0;

                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32QueueData);

                    if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
                    {
                        if ((u32Data & DD429_LABEL_SDI_MASK) == (u32QueueData & DD429_LABEL_SDI_MASK))
                        {
                            s16Match++;
                        }
                    }
                    else /* alternate format */
                    {
                        if ((u32Data & DD429_ALT_LABEL_SDI_MASK) == (u32QueueData & DD429_ALT_LABEL_SDI_MASK))
                        {
                            s16Match++;
                        }
                    }

                    if (s16Match)
                    {
                        /* If a message of the same LabelSdi and channel already    */
                        /* exists, just update the data, control, frequency and offset       */
                        for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                        {
                            U32BIT u32DataPointer = 0;
                            U32BIT u32QueueChannel = 0;

                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), &u32DataPointer);
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);

                            if ((u32DataPointer == u16DataIndex) && (u32QueueChannel == (U32BIT)u16Channel))
                            {
                                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), u32Data);
                                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u16DataIndex), u32TesterOptions);
                                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16QueueIndex), s16Frequency);
                                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), DD429_ACTIVE);

                                /* the HOST_MSG_TIME_TEMP memory is a scratch pad that is dynamically changing  */
                                /* by the scheduler state machine. it can be preloaded to a value. so if we     */
                                /* don't want to change it, then don't write to it.                             */
                                if (pDeviceContext->U.sTxScheduler.u16Offset[u16QueueIndex] != s16Offset)
                                {
                                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET + u16QueueIndex), s16Offset);

                                    pDeviceContext->U.sTxScheduler.u16Offset[u16QueueIndex] = s16Offset;
                                }

                                /* Update Tx Scheduler Table for data modification function (0 base) */
                                pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex] =   (U16BIT)(u16Channel +  1);

                                pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] = u32Data; /* Tx data */

                                pDeviceContext->U.sTxScheduler.u32Address[u16QueueIndex] = u16DataIndex; /* Tx address */

                                *pLocalInOutBuffer = s16RepeatCount;
                                return status;
                            }
                        }
                    }
                }

                /* If one doesn't exist, add a new entry    */
                /* Search for an available queue entry      */
                for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                {
                    U32BIT u32ActiveData = 0;

                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);

                    if (u32ActiveData == DD429_NOT_ACTIVE)
                    {
                        break;
                    }
                }

                if (u16QueueIndex == DD429_TX_SCHED_QUEUE_DEPTH)
                {
                    *pLocalInOutBuffer = (U32BIT)ERR_TXQUEUESZ;
                    break;
                }

                /* Search for an available message memory entry */
                for (u16DataIndex = 0; u16DataIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16DataIndex++)
                {
                    U32BIT u32QueueData = 0;

                    DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32QueueData);

                    if (u32QueueData == 0x00000000)
                    {
                        break;
                    }
                }

                if (u16DataIndex == DD429_TX_SCHED_MESSAGE_MEM_DEPTH)
                {
                    *pLocalInOutBuffer = (U32BIT)ERR_TXQUEUESZ;
                    break;
                }

                /* Add the message to the queue */
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), u32Data);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u16DataIndex), u32TesterOptions);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), u16DataIndex);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), u16Channel);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16QueueIndex), s16Frequency);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_OFFSET_TIME_OFFSET + u16QueueIndex), s16Offset);
                DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), DD429_ACTIVE);

                /* Update TX Scheduler Table for data modification function (0 base) */
                pDeviceContext->U.sTxScheduler.u16Offset[u16QueueIndex] = s16Offset;
                pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex] =  (U16BIT)(u16Channel +  1);
                pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] = u32Data;
                pDeviceContext->U.sTxScheduler.u32Address[u16QueueIndex] = u16DataIndex;
            }
            else /* extended scheduler */
            {
                /* we parsed the label/SDI which we use as the index */
                pIoctlParams->Param1 = DD429_TESTER_COMMAND__ADD_REPEATED_ITEM;
                pIoctlParams->Param6 = s16LabelSDI;

                status = _ARINC429TesterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
            }
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__ADD_REPEATED_ITEM:
        /* ---------------------------------------------------- */
        {
            SCHEDULER_DATA SchedulerData;
            DDC_IOCTL_PARAMS tempIoctlCmd;
            size_t tempBytesReturned;

            SchedulerData.u32Channel = u16Channel;
            SchedulerData.u32Data = pIoctlParams->Param2;
            SchedulerData.u32Control = pIoctlParams->Param3;
            SchedulerData.u16Frequency = (U16BIT)pIoctlParams->Param4;
            SchedulerData.u16Offset = (U16BIT)(pIoctlParams->Param5);
            SchedulerData.u16Index =  (U16BIT)pIoctlParams->Param6;
            SchedulerData.u8Active = DD429_ACTIVE;

            *pLocalInOutBuffer = 0;
            *pBytesReturned = sizeof(U32BIT);

            /* By design offset must be at least 1. If offset was 0 it would take 0xffff time to send the first message. */
            if (SchedulerData.u16Offset == 0)
            {
                SchedulerData.u16Offset = 1;
            }

            u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (SchedulerData.u16Index);
            DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);

            if (u32RegisterValue == DD429_ACTIVE)
            {
                /* if we are modifying an item return the # of currently active items */
                tempIoctlCmd.Channel = u16Channel;
                tempIoctlCmd.Param1 = DD429_TX_COMMAND__GET_NUM_OF_REPEATED;
                _ARINC429TxCommand(pDeviceContext, &tempIoctlCmd, pLocalInOutBuffer, sizeof(U32BIT), (size_t *)&tempBytesReturned);

                /* we cant updated the time when the item */
                /* u32RegisterAddress is still the same */
                u32RegisterValue = DD429_NOT_ACTIVE;
                DDC_MEM_WRITE(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
            }

            _arinc429SetRepeated(pDeviceContext,
                DD429_SCHED_DATA_UPDATE_MASK |
                DD429_SCHED_CONTROL_UPDATE_MASK |
                DD429_SCHED_FREQUENCY_UPDATE_MASK |
                DD429_SCHED_OFFSET_UPDATE_MASK |
                DD429_SCHED_ACTIVE_UPDATE_MASK,
                &SchedulerData);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_REPEATED:
        /* ---------------------------------------------------- */
        {
            DD429_GET_REPEATED__OUTPUT_TYPE *psGetRepeatedOutput = (DD429_GET_REPEATED__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*psGetRepeatedOutput))
            {
                U16BIT u16DataIndex;

                U32BIT u32DataLabelSDI = 0;
                U32BIT u32DataSDI = 0;
                U32BIT u32RequestedLabelSDI = DDC_IOCTL_U32(pIoctlParams->Param2);
                U16BIT u16QueueIndex = 0;
                U8BIT bMatch = FALSE;


                if (!pDeviceContext->bExtendedScheduler)
                {
                    for (u16DataIndex = 0; u16DataIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16DataIndex++)
                    {
                        if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
                        {
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32DataLabelSDI);
                            u32DataLabelSDI &= DD429_LABEL_SDI_MASK;
                        }
                        else
                        {
                            U32BIT u32Data = 0;
                            DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32Data);

                            /* convert alt format to normal */
                            u32DataSDI = ((u32Data & DD429_ALT_SDI_MASK) >> DD429_ALT_TO_NORMAL_SDI_OFFSET);
                            u32DataLabelSDI = ((u32Data & DD429_LABEL_MASK) | u32DataSDI);
                        }

                        if (u32DataLabelSDI == u32RequestedLabelSDI)
                        {
                            for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_QUEUE_DEPTH; u16QueueIndex++)
                            {
                                U32BIT u32DataPointer = 0;
                                U32BIT u32QueueChannel = 0;
                                U32BIT u32ActiveData = 0;

                                DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_POINTER_OFFSET + u16QueueIndex), &u32DataPointer);
                                DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_CHANNEL_OFFSET + u16QueueIndex), &u32QueueChannel);
                                DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_ACTIVE_OFFSET + u16QueueIndex), &u32ActiveData);

                                if ((u32DataPointer == u16DataIndex) &&
                                    (u32QueueChannel == u16Channel) &&
                                    (u32ActiveData == DD429_ACTIVE))
                                {
                                    bMatch = TRUE;
                                    break;
                                }
                            }
                        }

                        if (bMatch == TRUE)
                        {
                            break;
                        }
                    }

                    if (bMatch)
                    {
                        U32BIT u32ControlWord = 0;
                        U32BIT u32Frequency  = 0;

                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &psGetRepeatedOutput->u32Data);

                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16DataIndex), &u32ControlWord);
                        _ConvertControlWordToTesterOptions(u32ControlWord, &psGetRepeatedOutput->sTesterOptions);

                        DD429_MEM_READ_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_QUEUE_FREQ_OFFSET + u16QueueIndex), &u32Frequency);
                        psGetRepeatedOutput->s16Frequency = (S16BIT) u32Frequency;

                        /* since hardware is actively uses this memory address, data may have been written over */
                        /* so use the local cached version instead */
                        psGetRepeatedOutput->s16Offset = (S16BIT)pDeviceContext->U.sTxScheduler.u16Offset[u16QueueIndex];

                        psGetRepeatedOutput->s16Found = TRUE;
                    }
                    else
                    {
                        psGetRepeatedOutput->s16Found = FALSE;
                    }
                }
                else
                {
                    u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (u32RequestedLabelSDI);
                    DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                    if (u32RegisterValue == DD429_ACTIVE)
                     {
                        /* read data */
                        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA) + (u32RequestedLabelSDI);
                        DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &psGetRepeatedOutput->u32Data, ACEX_32_BIT_ACCESS);

                        /* read ctrl */
                        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA) + (u32RequestedLabelSDI + DD429_TX_SCHED_CONTROL_OFFSET);
                        DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                        _ConvertControlWordToTesterOptions(u32RegisterValue, &psGetRepeatedOutput->sTesterOptions);

                        /* read frequency */
                        u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) + (u32RequestedLabelSDI);
                        DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                        psGetRepeatedOutput->s16Frequency = (S16BIT)((u32RegisterValue & DD429_TX_SCHED_FREQ_MASK) >> DD429_TX_SCHED_FREQ_SHIFT);

                        /* since hardware is actively uses this memory address, data may have been written over */
                        /* so use the local cached version instead */
                        psGetRepeatedOutput->s16Offset = (S16BIT)pDeviceContext->U.u16TxSchedulerOffset[u16Channel][u32RequestedLabelSDI];

                        psGetRepeatedOutput->s16Found = TRUE;
                    }
                    else
                    {
                        psGetRepeatedOutput->s16Found = FALSE;
                    }
                }

                *pBytesReturned = sizeof(*psGetRepeatedOutput);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_ALL_REPEATED_ITEM:
        /* ---------------------------------------------------- */
        {
            DD429_GET_ALL_REPEATED__OUTPUT_TYPE *psAllRepeated = (DD429_GET_ALL_REPEATED__OUTPUT_TYPE *) pLocalInOutBuffer;

            if (OutputBufferLength >= sizeof(*psAllRepeated))
            {
                U16BIT BufferSize =  (U16BIT)pIoctlParams->Param2;
                U16BIT u16QueueIndex;

                memset(psAllRepeated, 0, sizeof(*psAllRepeated));

                for (u16QueueIndex=0; u16QueueIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16QueueIndex++)
                {
                    u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (u16QueueIndex);
                    DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                    if (u32RegisterValue == DD429_ACTIVE)
                    {
                        psAllRepeated->as16Data[psAllRepeated->s16NumFound] = u16QueueIndex;
                        psAllRepeated->s16NumFound++;
                        if (psAllRepeated->s16NumFound >= BufferSize)
                            break;
                    }
                }

                *pBytesReturned = sizeof(*psAllRepeated);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__MODIFY_REPEATED_DATA:
        /* ---------------------------------------------------- */
        {
            S16BIT s16Result = ERR_FUNCTION;
            U16BIT u16QueueIndex = 0;
            U8BIT u8SDI;
            U8BIT u8Label;
            U32BIT u32ModifiedData = pIoctlParams->Param2;
            U32BIT u32ModifiedControl = pIoctlParams->Param3;
            U32BIT u32ModifiedOption = pIoctlParams->Param4;

            /* Extract SDI and Label from input data, depending on bit format */
            if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
            {
                u8SDI = (U8BIT)((u32ModifiedData & DD429_SDI_MASK) >> DD429_SDI_OFFSET);
            }
            else
            {
                u8SDI = (U8BIT)((u32ModifiedData & DD429_ALT_SDI_MASK) >> DD429_ALT_SDI_OFFSET);
            }

            u8Label = (U8BIT)(u32ModifiedData & DD429_LABEL_MASK);

            if (!pDeviceContext->bExtendedScheduler)
            {
                /* Scan for user selected modification options and if found, modify data */
                switch (u32ModifiedOption)
                {
                    U8BIT u8LabelOld;
                    U8BIT u8SDIOld;
                    /* --------------------------------------- */
                    /* Label and transmitter match only match */
                    /* --------------------------------------- */
                    case ARINC_429_MODIFY_REPEATED_VIA_LABEL:
                    {
                        for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16QueueIndex++)
                        {
                            u8LabelOld = (U8BIT)(pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] & DD429_LABEL_MASK);
                            if ((u8Label == u8LabelOld) &&
                                ((u16Channel + 1) == pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex]))
                            {
                                s16Result = DDC_UDL_ERROR__SUCCESS;
                                break;
                            }
                        }
                        break;
                    }

                    /* -------------------------------- */
                    /* SDI, Label and transmitter match */
                    /* -------------------------------- */
                    case ARINC_429_MODIFY_REPEATED_VIA_SDI_LABEL:
                    {
                        for (u16QueueIndex = 0; u16QueueIndex < DD429_TX_SCHED_MESSAGE_MEM_DEPTH; u16QueueIndex++)
                        {
                            u8LabelOld = (U8BIT)(pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] & DD429_LABEL_MASK);
                            /* SDI is 2 bits wide & Label is 8 bits wide */
                            if (pDeviceContext->u8Arinc429BitFormat == DD429_BITFORMAT_ORIG)
                            {
                                u8SDIOld = (U8BIT)((pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] & DD429_SDI_MASK) >> DD429_SDI_OFFSET);
                            }
                            else
                            {
                                u8SDIOld = (U8BIT)((pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] & DD429_ALT_SDI_MASK) >> DD429_ALT_SDI_OFFSET);
                            }
                            if ((u8SDI == u8SDIOld) &&
                                ((u16Channel + 1) == pDeviceContext->U.sTxScheduler.u16Channel[u16QueueIndex]) &&
                                (u8Label == u8LabelOld))
                            {
                                s16Result = DDC_UDL_ERROR__SUCCESS;
                                break;
                            }
                        }
                        break;
                    }
                }

                if (s16Result == DDC_UDL_ERROR__SUCCESS)
                {
                    /* Re-write data */
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_DATA_OFFSET + u16QueueIndex), u32ModifiedData);

                    /* Re-write control */
                    DD429_MEM_WRITE_TX_GLOBAL(pDeviceContext, (DD429_TX_SCHED_MSG_CONTROL_OFFSET + u16QueueIndex), u32ModifiedControl);

                    /* update table */
                    pDeviceContext->U.sTxScheduler.u32Data[u16QueueIndex] = u32ModifiedData; /* Tx data */
                }
            }
            else /* extended scheduler */
            {
                pIoctlParams->Param1 = DD429_TESTER_COMMAND__MODIFY_REPEATED_DATA_ITEM;

                if (u32ModifiedOption == ARINC_429_MODIFY_REPEATED_VIA_SDI_LABEL)
                {
                    pIoctlParams->Param4 = u8Label + (u8SDI << DD429_SDI_OFFSET);
                    u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (pIoctlParams->Param4);

                    DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);
                    if (u32RegisterValue == DD429_ACTIVE)
                    {
                        _ARINC429TesterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
                    }
                }
                else /* (u32ModifiedOption == ARINC_429_MODIFY_REPEATED_VIA_LABEL) */
                {
                    for (u16QueueIndex = (U16BIT) u8Label; u16QueueIndex <  (U16BIT)DD429_TX_SCHED_MESSAGE_MEM_DEPTH;  u16QueueIndex = (U16BIT)(u16QueueIndex + 256))
                    {
                        pIoctlParams->Param4 = u16QueueIndex;
                        u32RegisterAddress = *(pDeviceContext->sTxChnlExtend429[u16Channel].pu32MemBA) + (pIoctlParams->Param4);

                        DDC_MEM_READ(pDeviceContext, (u32RegisterAddress), &u32RegisterValue, ACEX_32_BIT_ACCESS);

                        if (u32RegisterValue == DD429_ACTIVE)
                        {
                            _ARINC429TesterCommand(pDeviceContext, pIoctlParams, pLocalInOutBuffer, OutputBufferLength, pBytesReturned);
                            break;
                        }
                    }
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__MODIFY_REPEATED_DATA_ITEM:
        /* ---------------------------------------------------- */
        {
            SCHEDULER_DATA SchedulerData;

            SchedulerData.u32Channel = u16Channel;
            SchedulerData.u32Data = pIoctlParams->Param2;
            SchedulerData.u32Control = pIoctlParams->Param3;
            SchedulerData.u16Index = (U16BIT)pIoctlParams->Param4;
            SchedulerData.u8Active = DD429_ACTIVE;

            _arinc429SetRepeated(pDeviceContext,
                DD429_SCHED_DATA_UPDATE_MASK | DD429_SCHED_CONTROL_UPDATE_MASK,
                &SchedulerData);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_AMPLITUDE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Amplitude = DDC_IOCTL_U32(pIoctlParams->Param2);

            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   (
                     ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum != 0x06) && (u16Channel >= 8)) ||
                     ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06) && (u16Channel >= 6))
                   )
                 )
               )
            {
                /* ERROR */

                /* only the first 8 or 6 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            /* clear out setting first */
            u32RegisterValue &= ~(DD429_TX_VARIABLE_AMPLITUDE_MASK);

            u32RegisterValue |= (u32Amplitude << DD429_TX_VARIABLE_AMPLITUDE_OFFSET);

            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_AMPLITUDE:
        /* ---------------------------------------------------- */
        {
            U32BIT *pu32Amplitude = pLocalInOutBuffer;

            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   (
                     ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum != 0x06) && (u16Channel >= 8)) ||
                     ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06) && (u16Channel >= 6))
                   )
                 )
               )
            {
                /* ERROR */

                /* only the first 8 or 6 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            if (OutputBufferLength >= sizeof(*pu32Amplitude))
            {
                u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                *pu32Amplitude = ((u32RegisterValue & DD429_TX_VARIABLE_AMPLITUDE_MASK) >> DD429_TX_VARIABLE_AMPLITUDE_OFFSET);

                *pBytesReturned = sizeof(*pu32Amplitude);
            }
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__LOAD_TX_QUEUE_ONE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Data = pIoctlParams->Param2;
            U32BIT u32ControlWord = pIoctlParams->Param3;

            status = _ARINC429SendAsync(pDeviceContext, u16Channel, u32Data, u32ControlWord);

            if (status == DDC_UDL_ERROR__SUCCESS)
            {
                /* indicate to user that 1 message was sent, otherwise we will send the error status */
                status = 1;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__LOAD_TX_QUEUE_MORE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32NumItems = pIoctlParams->Param2;
            U32BIT *pu32Data;
            DD429_TESTER_OPTIONS_TYPE *psTesterOptions;
            U32BIT u32ControlWord;
            U32BIT u32NumLoaded;
            U32BIT u32Index;
            S16BIT s16Error;

            /* the data portion comes first */
            pu32Data = pLocalInOutBuffer;

            /* the tester options come after the data */
            psTesterOptions = (DD429_TESTER_OPTIONS_TYPE *)(pLocalInOutBuffer + u32NumItems);

            u32NumLoaded = 0;

            for (u32Index = 0; u32Index < u32NumItems; u32Index++)
            {
                u32ControlWord = _ConvertTesterOptionsToControlWord(&psTesterOptions[u32Index]);

                s16Error = _ARINC429SendAsync(pDeviceContext, u16Channel, pu32Data[u32Index], u32ControlWord);

                if (s16Error != DDC_UDL_ERROR__SUCCESS)
                {
                    status = s16Error;
                    break;
                }

                u32NumLoaded += 1;
            }

            status = (S16BIT)u32NumLoaded;

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_RX_VARIABLE_SPEED:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Speed = DDC_IOCTL_U32(pIoctlParams->Param2);
            U32BIT u32TableIndex;

            u32TableIndex = _ConvertSpeedToTableIndex(&u32Speed);

            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_SAMPLE_RATE_REG;
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TableIndex);

            /* return the actual speed used (overwrite error value) */
            *pLocalInOutBuffer = u32Speed;
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_TX_VARIABLE_SPEED:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Speed = DDC_IOCTL_U32(pIoctlParams->Param2);
            U32BIT u32TableIndex;

            u32TableIndex = _ConvertSpeedToTableIndex(&u32Speed);

            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_ALT_SPEED_REG;
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32TableIndex);

            /* return the actual speed used (overwrite error value) */
            *pLocalInOutBuffer = u32Speed;
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_RX_VARIABLE_SPEED:
        /* ---------------------------------------------------- */
        {
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_SAMPLE_RATE_REG;

            /* overwrite error value */
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, pLocalInOutBuffer);

            *pLocalInOutBuffer = _ConvertTableIndexToSpeed(*pLocalInOutBuffer);
            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_TX_VARIABLE_SPEED:
        /* ---------------------------------------------------- */
        {
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_ALT_SPEED_REG;

            DDC_REG_READ(pDeviceContext, u32RegisterAddress, pLocalInOutBuffer);

            /* overwrite error value */
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, pLocalInOutBuffer);

            *pLocalInOutBuffer = _ConvertTableIndexToSpeed(*pLocalInOutBuffer);
            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_TX_FRAME_RESOLUTION:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Resolution;

            u32Resolution = DDC_IOCTL_U32(pIoctlParams->Param2);

            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            u32RegisterValue &= ~(DD429_TX_CONTROL_MASK__RESOLUTION);
            u32RegisterValue |= (u32Resolution << DD429_TX_CONTROL_OFFSET__RESOLUTION);

            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_TX_FRAME_RESOLUTION:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Resolution;
            DD429_GET_TX_SCHEDULE_RESOLUTION__OUTPUT_TYPE *psGetTxFrameResolution = (DD429_GET_TX_SCHEDULE_RESOLUTION__OUTPUT_TYPE *) pLocalInOutBuffer;


            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + ACEX_429_TX_CONTROL_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

            u32Resolution = (u32RegisterValue & DD429_TX_CONTROL_MASK__RESOLUTION) >> DD429_TX_CONTROL_OFFSET__RESOLUTION;

            psGetTxFrameResolution->u32Error = DDC_UDL_ERROR__SUCCESS;
            psGetTxFrameResolution->u8Resolution = (U8BIT) u32Resolution;
            *pBytesReturned = sizeof(DD429_GET_TX_SCHEDULE_RESOLUTION__OUTPUT_TYPE);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_TX_FRAME_CONTROL:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Control;
            U32BIT u32PayloadCount;

            u32Control = DDC_IOCTL_U32(pIoctlParams->Param2);
            u32PayloadCount = DDC_IOCTL_U32(pIoctlParams->Param3);

            switch (u32Control)
            {
                case DD429_TX_FRAME_STOP:
                {
                    u32RegisterValue = ACEX_429_TX_FRAME_TABLE_REG__STOP;
                    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pTxChnl429[pIoctlParams->Channel]->pu32RegBA) + ACEX_429_TX_FRAME_TABLE_REG), &u32RegisterValue);
                    break;
                }

                case DD429_TX_FRAME_START:
                {
                    u32RegisterValue = ACEX_429_TX_FRAME_TABLE_REG__START;
                    DDC_REG_WRITE(pDeviceContext, (*(pDeviceContext->pTxChnl429[pIoctlParams->Channel]->pu32RegBA) + ACEX_429_TX_FRAME_TABLE_REG), &u32RegisterValue);
                    break;
                }

                case DD429_TX_FRAME_LOAD:
                {
                    U32BIT *pu32TxTable;

                    pu32TxTable = pLocalInOutBuffer;
#if DDC_DMA_429
                    {
                        /* start the DMA transfer */
                        dmaARINC429SetTxFrameControlSetup(pDeviceContext,
                            (U8BIT)u16Channel,
                            (u32PayloadCount << 2),                     /* convert # of 32-bit entries to bytes */
                            (*(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) << 2),
                            (U8BIT *)pu32TxTable);

                        DDC_ISR_LOCK_TAKE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);
                        if (pDeviceContext->pTxChnl429[u16Channel]->u16ARINC429SetTxFrameControlEventCond)
                        {
                            pDeviceContext->pTxChnl429[u16Channel]->u16ARINC429SetTxFrameControlEventCond--;
                        }
                        DDC_ISR_LOCK_GIVE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);

                        /* start DMA and wait for DMA completion */
                        dmaQueueStart(pDeviceContext);

                        DDC_ISR_LOCK_TAKE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);
                        if (0 == pDeviceContext->pTxChnl429[u16Channel]->u16ARINC429SetTxFrameControlEventCond)
                        {
                            int nResult = 0;

                            DDC_ISR_LOCK_GIVE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);

                            nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                                pDeviceContext->pTxChnl429[u16Channel]->waitqueueARINC429SetTxFrameControlEvent,
                                pDeviceContext->pTxChnl429[u16Channel]->u16ARINC429SetTxFrameControlEventCond,
                                500);

                            if (nResult)
                            {
                                DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FIFO, "nResult %d\n", nResult);
                            }
                        }
                        else
                        {
                            DDC_ISR_LOCK_GIVE(pDeviceContext->semARINC429SetTxFrameControlEventCond, pDeviceContext->semARINC429SetTxFrameControlEventCondFlag);
                        }
                    }
#else /* do not use DDC_DMA_429 */
                    {
                        unsigned int i;

                        for (i = 0; i < u32PayloadCount; i++)
                        {
                            DD429_MEM_WRITE(pDeviceContext,
                                *(pDeviceContext->pTxChnl429[pIoctlParams->Channel]->pu32MemBA) + i,
                                &pu32TxTable[i], ACEX_32_BIT_ACCESS);
                        }
                    }
#endif /* DDC_DMA_429 */

                    break;
                }

                case DD429_TX_FRAME_INIT:
                {
                    /* nothing to do at this time */
                    break;
                }

                default:
                {
                    status = ERR_FRAME_CONTROL_TYPE;
                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SEND_TX_FRAME_ASYNC:
        /* ---------------------------------------------------- */
        {
            U8BIT u16Channel = (U8BIT)pIoctlParams->Channel;
            U32BIT u32Data = pIoctlParams->Param2;
            U32BIT u32ControlWord = pIoctlParams->Param3;
            U32BIT u32Priority = pIoctlParams->Param4;
            U32BIT u32FullMask;
            U32BIT u32RegisterOffset;
            U32BIT u32ClockTimeEnd;
            U32BIT u32ClockTimeNow;

            if (u32Priority == DD429_ASYNC_PRIORITY_LOW)
            {
                u32FullMask = ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_LO_PRI_FULL_MASK;
                u32RegisterOffset = ACEX_429_TX_LO_PRIORITY_DATA_INPUT_REG;
            }
            else
            {
                u32FullMask = ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_HI_PRI_FULL_MASK;
                u32RegisterOffset = ACEX_429_TX_HI_PRIORITY_DATA_INPUT_REG;
            }

            /* loop while FIFO is full */
            u32ClockTimeEnd = ddcUdlOsGetClockMs() + LOAD_ASYNC_FIFO_TIMEOUT;
            do
            {
                /* read the FIFO status register */
                u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + (ACEX_429_TX_HILO_ASYNC_FIFO_STATUS_REG);
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                /*check if timeout is reached*/
                u32ClockTimeNow = ddcUdlOsGetClockMs();
                if (u32ClockTimeNow >= u32ClockTimeEnd)
                {
                    break;
                }
            } while (u32RegisterValue & u32FullMask);

            /* write the data & control word */
            u32RegisterAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32RegBA) + u32RegisterOffset;

            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32Data);
            DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32ControlWord);

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__VOLTAGE_MONITOR_ENABLE:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Index;
            S16BIT s16NumChannels = (U16BIT)pIoctlParams->Param2;
            S16BIT *ps16Channels;

            ps16Channels = (S16BIT *)pLocalInOutBuffer;

            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   ( pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06 )
                 )
               )
            {
                /* ERROR */

                /* 6 channel variant does not support this feature */
                /* other cards, only the first 8 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            u32RegisterValue = 0x00000000;

            for (u32Index = 0; u32Index < (U32BIT)s16NumChannels; u32Index++)
            {
                u32RegisterValue |= 1 << (ps16Channels[u32Index] - 1); /* minus 1 to convert to 0 based index */
            }

            if (status == DDC_UDL_ERROR__SUCCESS)
            {
                if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA)
                {
                    /* write the voltage montior channel enable register */
                    u32RegisterAddress = *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA) + BD_VOLT_MON_X8__CHANNEL_ENABLE_REG;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__VOLTAGE_MONITOR_START:
        /* ---------------------------------------------------- */
        {
            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   ( pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06 )
                 )
               )
            {
                /* ERROR */

                /* 6 channel variant does not support this feature */
                /* other cards, only the first 8 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA)
            {
                /* write the voltage montior control register */
                u32RegisterAddress = *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA) + BD_VOLT_MON_X8__CONTROL_REG;
                u32RegisterValue = BD_VOLT_MON_X8_CONTROL__START_CAPTURE;
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__VOLTAGE_MONITOR_GET_STATUS:
        /* ---------------------------------------------------- */
        {
            U32BIT u32Status = 0;

            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   ( pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06 )
                 )
               )
            {
                /* ERROR */

                /* 6 channel variant does not support this feature */
                /* other cards, only the first 8 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA)
            {
                /* read the voltage montior status register - use error status to return value */
                u32RegisterAddress = *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32RegBA) + BD_VOLT_MON_X8__STATUS_REG;

                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32Status);

                *pLocalInOutBuffer = u32Status;
                *pBytesReturned = sizeof(u32Status);
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__VOLTAGE_MONITOR_GET_DATA:
        /* ---------------------------------------------------- */
        {
            U32BIT u32BufferLength;

            if ( (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H) ||
                 (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M) ||
                 ( (
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                     (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F)
                   )
                   &&
                   ( pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum == 0x06 )
                 )
               )
            {
                /* ERROR */

                /* 6 channel variant does not support this feature */
                /* other cards, only the first 8 channels support this feature */
                status = ERR_FEATURE_NOT_SUPPORTED_CH;
                break;
            }

            /* determine how much to transfer */
            u32BufferLength = (U32BIT)((OutputBufferLength < DD429_VOLTAGE_MONITOR_BUFFER_BYTE_SIZE) ? OutputBufferLength : DD429_VOLTAGE_MONITOR_BUFFER_BYTE_SIZE);
            *pBytesReturned = u32BufferLength;

#if DDC_DMA_429
            {
                /* start the DMA transfer */
                dmaArinc429VoltageMonitoringSetup(pDeviceContext,
                    u32BufferLength,
                    ((*(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemBA)) << 2),             /* convert to byte address */
                    (U8BIT *)pLocalInOutBuffer);

                    DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);
                    if (pDeviceContext->u16Arinc429VoltageMonitoringEventCond)
                    {
                        pDeviceContext->u16Arinc429VoltageMonitoringEventCond--;
                    }
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);

                    /* start DMA and wait for DMA completion */
                    dmaQueueStart(pDeviceContext);

                    DDC_ISR_LOCK_TAKE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);
                    if (0 == pDeviceContext->u16Arinc429VoltageMonitoringEventCond)
                    {
                        int nResult = 0;

                        DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);

                    nResult = DDC_WAIT_INTERRUPTABLE_TIMEOUT_MS(
                        pDeviceContext->waitqueueArinc429VoltageMonitoringEvent,
                        pDeviceContext->u16Arinc429VoltageMonitoringEventCond,
                        500);

                    if (nResult)
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_FIFO, "nResult %d\n", nResult);
                    }
                }
                else
                {
                    DDC_ISR_LOCK_GIVE(pDeviceContext->semArinc429VoltageMonitoringEventCond, pDeviceContext->semArinc429VoltageMonitoringEventCondFlag);
                }
            }
#else /* use direct memory reads */
            {
                DDC_BLK_MEM_READ(pDeviceContext,
                    *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdVoltMonX8.pu32MemBA),
                    pLocalInOutBuffer,
                    u32BufferLength >> 2,
                    ACEX_32_BIT_ACCESS);
            }
#endif /* DDC_DMA_429 */

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SERIAL_WRITE_REG:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TESTER, "429 - ARINC429CastSerialIORegWrite - \n");
            ARINC429CastSerialIORegWrite(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SERIAL_READ_REG:
        /* ---------------------------------------------------- */
        {
            if ((pIoctlParams->Param2 != 0)  &  (pIoctlParams->Param2 != 0x74))   /* MIO_SDLC_RFIFO = 0x74 */
            {
                 *pLocalInOutBuffer = ARINC429CastSerialIORegRead(pDeviceContext, pIoctlParams);
            }
            else
            {
                if(pDeviceContext->sSerialIORxHBuf.bRxFiFoHbufInstalled[pIoctlParams->Channel] == TRUE)
                {
                    *pLocalInOutBuffer = serial_ioReadUART(pDeviceContext, (U8BIT)pIoctlParams->Channel);
                }
                else
                {
                    *pLocalInOutBuffer = ARINC429CastSerialIORegRead(pDeviceContext, pIoctlParams);
                }
            }

            *pBytesReturned = sizeof(U32BIT);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SERIAL_READ_REG_EXT:
        /* ---------------------------------------------------- */
        {
            if(pDeviceContext->sSerialIORxHBuf.bRxFiFoHbufInstalled[pIoctlParams->Channel] == TRUE)
            {
                U16BIT  RetVal = 0;

                RetVal = serial_ioReadUART_EXT(pDeviceContext, (U8BIT )pIoctlParams->Channel, (U8BIT *)pLocalInOutBuffer, (U32BIT)OutputBufferLength);
                *pBytesReturned = RetVal;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SERIAL_SET_CONFIG:
        /* ---------------------------------------------------- */
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TESTER, "429 - ARINC429CastSetSerialIOConfig - \n");
            ARINC429CastSetSerialIOConfig(pDeviceContext, pIoctlParams);
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SERIAL_GET_CONFIG:
        /* ---------------------------------------------------- */
        {
            *pLocalInOutBuffer = ARINC429CastGetSerialIOConfig(pDeviceContext, pIoctlParams);
            *pBytesReturned = OutputBufferLength;
            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__CONFIG_REPEATER:
        /* ---------------------------------------------------- */
        {
            U32BIT u32RepeaterInUse = 0;

            if (pIoctlParams->Param2 < DD429_MAX_CHANNELS_PER_REGISTER)
            {
                u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
            }
            else
            {
                u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_2_REG;
            }

            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
            switch (pIoctlParams->Param3)
            {
                case DD429_REPEATER_MAP:
                {
                    u32RegisterValue |= LOOPBACK_MAPPING_MASK[pIoctlParams->Param2];
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    u32RegisterValue |= DD429_RX_CONTROL_MASK__DATA_REPEATER;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                    /*
                       The default mode when a map is created is REPEATER_MODE__REPEAT. This can only be set the first time an RX is
                       mapped with a TX. Following mappings between the RX and TX should not set the mode to REPEATER_MODE__REPEAT
                       as the user could have already changed the default configuration behavior.
                    */
                    if (u16Channel <= 31)
                    {
                        u32RepeaterInUse = pDeviceContext->u32RepeaterActiveCh1to32 & (1 << u16Channel);
                    }
                    else
                    {
                        u32RepeaterInUse = pDeviceContext->u32RepeaterActiveCh32to64 & (1 << (u16Channel - 32));
                    }

                    if (!u32RepeaterInUse)
                    {
                        U32BIT u32TableAddress;
                        U16BIT u16LabelSdiLoop;
                        U32BIT u32ModeValue = REPEATER_MODE__REPEAT;

                        for (u16LabelSdiLoop = 0; u16LabelSdiLoop < MAX_NUM_LABEL_SDI; u16LabelSdiLoop++)
                        {
                            u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) +
                                (u16LabelSdiLoop + DD429_REPEATER__MODE_TABLE_OFFSET);
                            DDC_MEM_WRITE(pDeviceContext, u32TableAddress, &u32ModeValue, ACEX_32_BIT_ACCESS);
                        }
                    }

                    /* set the repeater usage flag */
                    if (u16Channel <= 31)
                    {
                        pDeviceContext->u32RepeaterActiveCh1to32 |= (1 << u16Channel);
                    }
                    else
                    {
                        pDeviceContext->u32RepeaterActiveCh32to64 |= (1 << (u16Channel - 32));
                    }

                    break;
                }

                case DD429_REPEATER_UNMAP:
                {
                    U32BIT u32Temp  = 0;

                    u32RegisterValue &= ~(LOOPBACK_MAPPING_MASK[pIoctlParams->Param2]);
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);

                    /* Only disable repeater in the control register if no other channels are mapped for data repeater */
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_REG;
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_WRAP_AROUND_2_REG;
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32Temp);
                    if ((u32Temp == 0) && (u32RegisterValue == 0))
                    {
                        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
                        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                        u32RegisterValue &= ~(DD429_RX_CONTROL_MASK__DATA_REPEATER);
                        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    }

                    /* clear out the repeater usage flag */
                    if (u16Channel <= 31)
                    {
                        pDeviceContext->u32RepeaterActiveCh1to32 &= ~(1 << u16Channel);
                    }
                    else
                    {
                        pDeviceContext->u32RepeaterActiveCh32to64 &= ~(1 << (u16Channel - 32));
                    }

                    break;
                }
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__SET_REPEATER_MODE:
        /* ---------------------------------------------------- */
        {
            DD429_REPEATER_MODE_TYPE *pSetRepeaterMode = (DD429_REPEATER_MODE_TYPE *) pLocalInOutBuffer;
            U16BIT u16LabelSdiLoop = 0;
            U16BIT u16NumLabelsToIterate = 1; /* use Label with SDI by default, so only need to loop for one item */
            U32BIT u32TableAddress;
            U32BIT u32TempTableAddress;

            /* Label only mode? */
            if (pSetRepeaterMode->u32Mode & REPEATER_OPTION__MODE_LABEL_SDI)
            {
                /* nothing to do */
            }
            else
            {
                /* clear out the SDI */
                pSetRepeaterMode->u16LabelSdi =  (U16BIT)( pSetRepeaterMode->u16LabelSdi &  DD429_LABEL_MASK);

                /* iterate for all 4 SDI value combimations */
                u16NumLabelsToIterate = 4;
            }

            /* ----------------------------------------------------------------------------------------------- */
            /* disable the repeater before changing the settings (in case the repeater was already configured) */
            /* ----------------------------------------------------------------------------------------------- */

            u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) +
                (pSetRepeaterMode->u16LabelSdi + DD429_REPEATER__MODE_TABLE_OFFSET);

            for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
            {
                U32BIT u32ModeValue = REPEATER_MODE__DISABLE;

                DDC_MEM_WRITE(pDeviceContext, u32TableAddress, &u32ModeValue, ACEX_32_BIT_ACCESS);

                /* increment to the next Label/SDI index */
                u32TableAddress += 0x100;
            }

            /* --------------------------------------------------- */
            /* write the CLEAR, SET, FLIP, and REPLACE data tables */
            /* --------------------------------------------------- */

            /* get the start address of the data tables, offsets to the tabels will be added to this value */
            /* the tables are located in the upper portion of the RX memory */

            /*
                NOTE: A side effect of enabling data pollution will be a decrease in the amount of RX RAM available to the receiver.
                      For example, a 16k RX memory will only have 12k available for storage.
            */

            u32TableAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA) +
                (*(pDeviceContext->pRxChnl429[u16Channel]->pu32MemSize) - DD429_REPEATER_MODE_TABLE_SIZE) + pSetRepeaterMode->u16LabelSdi;

            /* CLEAR */
            if (pSetRepeaterMode->u32Mode & REPEATER_MODE__CLEAR)
            {
                u32TempTableAddress = u32TableAddress;

                for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
                {
                    DDC_MEM_WRITE(pDeviceContext, (u32TempTableAddress + DD429_REPEATER__CLEAR_TABLE_OFFSET), &pSetRepeaterMode->u32ClearData, ACEX_32_BIT_ACCESS);

                    /* increment to the next Label/SDI index */
                    u32TempTableAddress += 0x100;
                }
            }

            /* SET */
            if (pSetRepeaterMode->u32Mode & REPEATER_MODE__SET)
            {
                u32TempTableAddress = u32TableAddress;

                for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
                {
                    DDC_MEM_WRITE(pDeviceContext, (u32TempTableAddress + DD429_REPEATER__SET_TABLE_OFFSET), &pSetRepeaterMode->u32SetData, ACEX_32_BIT_ACCESS);

                    /* increment to the next Label/SDI index */
                    u32TempTableAddress += 0x100;
                }
            }

            /* FLIP */
            if (pSetRepeaterMode->u32Mode & REPEATER_MODE__FLIP)
            {
                u32TempTableAddress = u32TableAddress;

                for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
                {
                    DDC_MEM_WRITE(pDeviceContext, (u32TempTableAddress + DD429_REPEATER__FLIP_TABLE_OFFSET), &pSetRepeaterMode->u32FlipData, ACEX_32_BIT_ACCESS);

                    /* increment to the next Label/SDI index */
                    u32TempTableAddress += 0x100;
                }
            }

            /* REPLACE */
            if (pSetRepeaterMode->u32Mode & REPEATER_MODE__REPLACE)
            {
                u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) + (pSetRepeaterMode->u16LabelSdi + DD429_REPEATER__REPLACE_TABLE_OFFSET);

                for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
                {
                    DDC_MEM_WRITE(pDeviceContext, u32TableAddress, &pSetRepeaterMode->u32ReplaceData, ACEX_32_BIT_ACCESS);

                    /* increment to the next Label/SDI index */
                    u32TableAddress += 0x100;
                }
            }


            /* -------------------- */
            /* write the Mode table */
            /* -------------------- */

            u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) + (pSetRepeaterMode->u16LabelSdi + DD429_REPEATER__MODE_TABLE_OFFSET);

            for (u16LabelSdiLoop = 0; u16LabelSdiLoop < u16NumLabelsToIterate; u16LabelSdiLoop++)
            {
                DDC_MEM_WRITE(pDeviceContext, u32TableAddress, &pSetRepeaterMode->u32Mode, ACEX_32_BIT_ACCESS);

                /* increment to the next Label/SDI index */
                u32TableAddress += 0x100;
            }

            break;
        }

        /* ---------------------------------------------------- */
        case DD429_TESTER_COMMAND__GET_REPEATER_MODE:
        /* ---------------------------------------------------- */
        {
            DD429_REPEATER_MODE_TYPE *pGetRepeaterMode = (DD429_REPEATER_MODE_TYPE *) pLocalInOutBuffer;
            U32BIT u32TableAddress;
            U16BIT u16LabelSdi = (U16BIT) pIoctlParams->Param2;

            if (OutputBufferLength >= sizeof(*pGetRepeaterMode))
            {
                /* -------------------------------------------------- */
                /* read the CLEAR, SET, FLIP, and REPLACE data tables */
                /* -------------------------------------------------- */

                /* get the start address of the data tables, offsets to the tabels will be added to this value */
                /* the tables are located in the upper portion of the RX memory */

                u32TableAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA) +
                    (*(pDeviceContext->pRxChnl429[u16Channel]->pu32MemSize) - DD429_REPEATER_MODE_TABLE_SIZE) + u16LabelSdi;

                /* CLEAR, SET, FLIP */
                DDC_MEM_READ(pDeviceContext, (u32TableAddress + DD429_REPEATER__CLEAR_TABLE_OFFSET), &pGetRepeaterMode->u32ClearData, ACEX_32_BIT_ACCESS);
                DDC_MEM_READ(pDeviceContext, (u32TableAddress + DD429_REPEATER__SET_TABLE_OFFSET), &pGetRepeaterMode->u32SetData, ACEX_32_BIT_ACCESS);
                DDC_MEM_READ(pDeviceContext, (u32TableAddress + DD429_REPEATER__FLIP_TABLE_OFFSET), &pGetRepeaterMode->u32FlipData, ACEX_32_BIT_ACCESS);

                /* REPLACE */
                u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) + (u16LabelSdi + DD429_REPEATER__REPLACE_TABLE_OFFSET);
                DDC_MEM_READ(pDeviceContext, u32TableAddress, &pGetRepeaterMode->u32ReplaceData, ACEX_32_BIT_ACCESS);


                /* ------------------- */
                /* read the Mode table */
                /* ------------------- */

                u32TableAddress = *(pDeviceContext->pTxChnl429[u16Channel]->pu32MemBA) + (u16LabelSdi + DD429_REPEATER__MODE_TABLE_OFFSET);
                DDC_MEM_READ(pDeviceContext, u32TableAddress, &pGetRepeaterMode->u32Mode, ACEX_32_BIT_ACCESS);

                pGetRepeaterMode->u16LabelSdi = u16LabelSdi;
                pGetRepeaterMode->u32Reserved = ERR_SUCCESS;

                *pBytesReturned = sizeof(*pGetRepeaterMode);
            }
            else
            {
                status = DDC_UDL_ERROR__BUFFER_SIZE;
            }

            break;
        }

        /* ---------------------------------------------------- */
        default:
        /* ---------------------------------------------------- */
        {
            /* unknown type */
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_COMMAND_TESTER, "429 - _ARINC429TesterCommand(CH:%d) - Unknown Command %d !!!\n", pIoctlParams->Channel, pIoctlParams->Param1);
            status = ERR_UNKNOWN;
            break;
        }
    }

    return status;
}

/* ============================================================================ */
/* ============================================================================ */
/* HOST BUFFER FUNCTIONS                                                        */
/* ============================================================================ */
/* ============================================================================ */

/*-------------------------------------------------------------------------------
   Function:
       ARINC429OnRxHostBufferChannelEnabled

   Description:
        This function enables the interrupt of an ARINC 429 Rx channel and clears
        the host buffer.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429OnRxHostBufferChannelEnabled
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    ACEX_429HBUF_TYPE *sHbuf;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer != NULL)
    {
        /* Empty host buffer */
        DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
        sHbuf->u32HbufNextWrite = 0;
        sHbuf->u32HbufNextRead = 0;
        sHbuf->u32HbufNumEntries = 0;
        DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);

        /* Enabele internal interrupts to fill the internal buffer with incoming messages */
        ARINC429ConfigInternalRxInterrupt(pDeviceContext, u16Channel);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429OnRxHostBufferChannelSpeedChanged

   Description:
        This function deletes the host buffer of an ARINC 429 Rx channel if the
        speed changed.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429OnRxHostBufferChannelSpeedChanged
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    ACEX_429HBUF_TYPE *sHbuf;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer != NULL)
    {
        /* Empty host buffer */
        DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
        sHbuf->u32HbufNextWrite = 0;
        sHbuf->u32HbufNextRead = 0;
        sHbuf->u32HbufNumEntries = 0;
        DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429ConfigInternalRxInterrupt

   Description:
        This function configures the interrupts for an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429ConfigInternalRxInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    U32BIT u32RegisterValueBefore;
    U32BIT u32RegisterValue = 0;
    U32BIT u32RegisterAddress = 0;
    ACEX_429HBUF_TYPE *sHbuf;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    /* Configure interrupt for receive channel */
    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);

    if (sHbuf->pu8Buffer != NULL)
    {
        /* Set interrupt mask for the current channel */
        u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_INT_ENABLE_REG;
        DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValueBefore);

        u32RegisterValue = u32RegisterValueBefore | (ACEX_429_RX_INT_ENABLE__FIFO_OVERFLOW | ACEX_429_RX_INT_ENABLE__QUARTER_HALF_FULL);
        DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429HostBufferControl

   Description:
        This function manages the host buffer for an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
S16BIT ARINC429HostBufferControl
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t           *pBytesReturned,
    U32BIT           *pReadData
)
{
    U8BIT u16Channel;
    ACEX_429HBUF_TYPE *sHbuf;

    *pBytesReturned = sizeof(U32BIT);

    switch (pIoctlParams->Param1)
    {
        case ARINC_RX_HBUF_INSTALL:
        {
            ARINC429InstallRxHostBuffer(pDeviceContext);

            /* Initialize metric values*/
            for (u16Channel = 0; u16Channel < MAX_NUM_429_CHANNELS; u16Channel++)
            {
                pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull = 0;
                pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentHigh = 0;
                pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufOverflowCount = 0;
            }
            break;
        }

        case ARINC_RX_HBUF_UNINSTALL:
        {
            ARINC429UninstallRxHostBuffer(pDeviceContext, FALSE);
            break;
        }

        case ARINC_RX_HBUF_ENABLE:
        {
            ARINC429CreateRxHostBuffer(pDeviceContext, pIoctlParams->Channel);
            break;
        }

        case ARINC_RX_HBUF_DISABLE:
        {
            ARINC429DeleteRxHostBuffer(pDeviceContext, pIoctlParams->Channel);
            break;
        }

        case ARINC_RX_HBUF_READ:
        {
            *pBytesReturned = ARINC429ReadRxHostBuffer(pDeviceContext, pIoctlParams->Channel, DDC_IOCTL_U32(pIoctlParams->Param2), pReadData);
            break;
        }

        case ARINC_HBUF_METRICS:
        {
            u16Channel = (U8BIT)pIoctlParams->Channel;
            sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);

            DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
            pReadData[0] = pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull;
            pReadData[1] = pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentHigh;
            pReadData[2] = pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufOverflowCount;
            DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);

            *pBytesReturned = sizeof(U32BIT) * 3;
            break;
        }
    }
    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429WriteRxHostBuffer

   Description:
        This function writes messages to the host buffer of an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  interruptStatus   - interrupt status structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
DDC_LOCAL void ARINC429WriteRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    U32BIT u32FifoMessageSize;
    U32BIT u32RegisterAddress;
    U32BIT*  pHostBuffer;
    U32BIT u32Value;
    ACEX_429HBUF_TYPE *sHbuf;

    U32BIT u32NumMessagesToRead = 0;
    U32BIT u32NumMessagesToCopy = 0;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer != NULL)
    {
        if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H)  ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M)  ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
            (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX))
        {
            /* Get control word to read the number of messages in the queue */
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_FIFO_MSG_COUNT_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32NumMessagesToRead);
            u32NumMessagesToRead &= DD429_RX_FIFO_MSG_COUNT_MASK__MSG_COUNT;
        }
        else
        {
            /* Get control word to read the number of messages in the queue */
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32NumMessagesToRead);
            u32NumMessagesToRead = ((u32NumMessagesToRead & MIO_RX_CNT_FIFO_COUNT) >> 15);
        }

        if (u32NumMessagesToRead > 0)
        {
            /* Get the host buffer pointer */
            pHostBuffer = (U32BIT*)sHbuf->pu8Buffer;

            DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);

            if (_RxControlCommandGetValue(pDeviceContext, u16Channel, DD429_RX_CONTROL_COMMAND_GET__TIMETAG_ENABLED))
            {
                u32FifoMessageSize = ARINC_RX_FIFO_MAX_MSG_SIZE;
                sHbuf->bHbufStoreTimeTag = TRUE;
            }
            else
            {
                u32FifoMessageSize = 1;
                sHbuf->bHbufStoreTimeTag = FALSE;
            }

            /* Host buffer overflow? */
            if (ARINC429CheckHostBufferOverflow(sHbuf->u32HbufNextRead, sHbuf->u32HbufNextWrite, ARINC_RX_HBUF_MAX_MSG_SIZE * u32NumMessagesToRead))
            {
                pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufOverflowCount += 1;  /* Update for metrics reporting */

                ARINC429CopyFifoToHostBuffer(pDeviceContext, u16Channel, &(pHostBuffer[0]), u32NumMessagesToRead, u32FifoMessageSize);
                sHbuf->u32HbufNextWrite = (u32NumMessagesToRead * ARINC_RX_HBUF_MAX_MSG_SIZE);
                sHbuf->u32HbufNextRead = 0;
                sHbuf->u32HbufNumEntries = u32NumMessagesToRead;
                sHbuf->bHbufOverflow = TRUE;
            }
            else
            {
                /* Host buffer rollover? */
                if ((sHbuf->u32HbufNextWrite + (u32NumMessagesToRead * ARINC_RX_HBUF_MAX_MSG_SIZE)) > ARINC_RX_HBUF_MAX)
                {
                    u32NumMessagesToCopy = (ARINC_RX_HBUF_MAX - sHbuf->u32HbufNextWrite) / ARINC_RX_HBUF_MAX_MSG_SIZE;

                    /* Copy items up to the end of the host buffer */
                    ARINC429CopyFifoToHostBuffer(pDeviceContext,
                        u16Channel,
                        &(pHostBuffer[sHbuf->u32HbufNextWrite]),
                        u32NumMessagesToCopy,
                        u32FifoMessageSize);

                    /* Copy the remaining items starting at the beginning of the host buffer */
                    u32NumMessagesToCopy = u32NumMessagesToRead - u32NumMessagesToCopy;
                    ARINC429CopyFifoToHostBuffer(pDeviceContext,
                        u16Channel,
                        &(pHostBuffer[0]),
                        u32NumMessagesToCopy,
                        u32FifoMessageSize);

                    sHbuf->u32HbufNextWrite = u32NumMessagesToCopy * ARINC_RX_HBUF_MAX_MSG_SIZE;
                    sHbuf->u32HbufNumEntries += u32NumMessagesToRead;
                }
                else
                {
                    ARINC429CopyFifoToHostBuffer(pDeviceContext,
                        u16Channel,
                        &(pHostBuffer[sHbuf->u32HbufNextWrite]),
                        u32NumMessagesToRead,
                        u32FifoMessageSize);

                    sHbuf->u32HbufNumEntries += u32NumMessagesToRead;
                    sHbuf->u32HbufNextWrite += (u32NumMessagesToRead * ARINC_RX_HBUF_MAX_MSG_SIZE);
                    if (sHbuf->u32HbufNextWrite >= ARINC_RX_HBUF_MAX)
                    {
                        sHbuf->u32HbufNextWrite = 0;
                    }
                }
            }
            DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
        }

        /* DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_RX_HOST_BUFFER, "429 - ARINC429WriteRxHostBuffer # entries: %08d Channel: %02d\n", sHbuf->u32HbufNumEntries, u16Channel); */

        /* Metrics update */
        if (sHbuf->u32HbufNextWrite >= sHbuf->u32HbufNextRead)
        {
            u32Value = (sHbuf->u32HbufNextWrite - sHbuf->u32HbufNextRead) * (U32BIT)100;
            pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull = u32Value / (U32BIT)ARINC_RX_HBUF_MAX;
            if (pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull > 100)
            {
                pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull = 100;
            }
        }
        else
        {
            pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull =
                (((U32BIT)ARINC_RX_HBUF_MAX - sHbuf->u32HbufNextRead + sHbuf->u32HbufNextWrite) * (U32BIT)100) / (U32BIT)ARINC_RX_HBUF_MAX;
        }
        if (pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentHigh < pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull)
        {
            pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentHigh = pDeviceContext->pRxChnl429[u16Channel]->u32Arinc429HbufPercentFull;
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CheckHostBufferOverflow

   Description:
        This function checks, whether the next write sequence would create a
        host buffer overflow.

   Parameters:
      In  u32NextRead    - next read position
      In  u32NextWrite   - next write position
      In  u32NumElements - number of elements to copy

   Returns:
      overflow condition.
   ---------------------------------------------------------------------------------*/
static BOOLEAN ARINC429CheckHostBufferOverflow
(
    U32BIT u32NextRead,
    U32BIT u32NextWrite,
    U32BIT u32NumElements
)
{
    BOOLEAN result = FALSE;

    if (u32NextWrite < u32NextRead)
    {
        result = (BOOLEAN)((u32NextWrite + u32NumElements) >= u32NextRead);
    }
    else
    {
        if ((u32NextWrite + u32NumElements) > ARINC_RX_HBUF_MAX)
        {
            result = (BOOLEAN)(((u32NumElements - (ARINC_RX_HBUF_MAX - u32NextWrite) - 1)) >= u32NextRead);
        }
    }
    return result;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CopyFifoToHostBuffer

   Description:
        This function copies the FIFO to the host buffer.

   Parameters:
      In  pDeviceContext       - device-specific structure
      In  u16Channel           - channel number
      In  pHostBufferMemory    - host buffer OS memory
      In  u32NumMessagesToRead - number of messages to copy

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429CopyFifoToHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT* pHostBufferMemory,
    U32BIT u32NumMessagesToCopy,
    U32BIT u32FifoMessageSize
)
{
    U32BIT i;
    U32BIT hbufMessageBaseIndex;
    DDC_IOCTL_PARAMS tempIoctlParams;
    size_t bytesReturned;
    DD429_READ_DATA_IRIG__OUTPUT_TYPE sReadRxFifoIrig;

    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H)  ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M)  ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX))
    {
        tempIoctlParams.Channel = u16Channel;
        tempIoctlParams.Param1 = DD429_FIFO_COMMAND__READ_RX_QUEUE_IRIG_ONE;

        for (i = 0; i < u32NumMessagesToCopy; i++)
        {
            _ARINC429FifoCommand(
                pDeviceContext,
                &tempIoctlParams,
                (U32BIT *) &sReadRxFifoIrig,
                sizeof(sReadRxFifoIrig),
                &bytesReturned);

            hbufMessageBaseIndex = i * ARINC_RX_HBUF_MAX_MSG_SIZE;

            /* Copy 429 data */
            pHostBufferMemory[hbufMessageBaseIndex + 0] = sReadRxFifoIrig.u32Data;
            pHostBufferMemory[hbufMessageBaseIndex + 1] = sReadRxFifoIrig.u32StampLo;
            pHostBufferMemory[hbufMessageBaseIndex + 2] = sReadRxFifoIrig.u32StampHi;
            pHostBufferMemory[hbufMessageBaseIndex + 3] = 0x00000000;
        }
    }
    else
    {
        U32BIT hbufMessageBaseIndex;
        U32BIT u32MemoryAddress;
        U32BIT u32Data;

        u32MemoryAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32MemBA);
        for (i = 0; i < u32NumMessagesToCopy; i++)
        {
            hbufMessageBaseIndex = i * ARINC_RX_HBUF_MAX_MSG_SIZE;

            /* Copy 429 data */
            u32Data = 0x00000000;
            DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);
            pHostBufferMemory[hbufMessageBaseIndex] = u32Data;

            /* If time tag is used */
            if (u32FifoMessageSize > 1)
            {
                /* Copy LSB time tag */
                u32Data = 0x00000000;
                DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);
                pHostBufferMemory[hbufMessageBaseIndex + 1] = u32Data;

                /* Copy MSB time tag */
                u32Data = 0x00000000;
                DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);
                pHostBufferMemory[hbufMessageBaseIndex + 2] = u32Data;

                /* Dummy read for the fourth unused value */
                DD429_MEM_READ(pDeviceContext, u32MemoryAddress, &u32Data, ACEX_32_BIT_ACCESS);

                /* Clear spare dword */
                pHostBufferMemory[hbufMessageBaseIndex + 3] = 0x00000000;
            }
            else
            {
                pHostBufferMemory[hbufMessageBaseIndex + 1] = 0x00000000;
                pHostBufferMemory[hbufMessageBaseIndex + 2] = 0x00000000;
                pHostBufferMemory[hbufMessageBaseIndex + 3] = 0x00000000;
            }
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429ReadRxHostBuffer

   Description:
        This function reads messages from the host buffer of an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static size_t ARINC429ReadRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    U32BIT u32NumMsgsToRead,
    U32BIT* pReadData
)
{
    size_t numBytesToCopy = 0;
    U32BIT*            pMessages;
    U32BIT u32RegisterAddress;
    U32BIT u32RegisterValue;
    U32BIT u32Flags = 0x00000000;
    ACEX_429HBUF_TYPE *sHbuf;

    pReadData[0] = 0x00000000;
    pReadData[1] = 0x00000000;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return 0;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return 0;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer != NULL)
    {
        DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
        if (sHbuf->bHbufOverflow)
        {
            u32Flags |= 0x40000000;
            sHbuf->bHbufOverflow = FALSE;
        }
        if (sHbuf->bHbufQueueOverflow)
        {
            u32Flags |= 0x20000000;
            sHbuf->bHbufQueueOverflow = FALSE;
        }
        pReadData[0] = u32Flags;
        if ((u32Flags & 0x60000000) == 0)
        {
            if (sHbuf->u32HbufNumEntries == 0)
            {
                /* Read the number of messages in the queue */
                u32RegisterValue = 0x00000000;
                u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
                DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                if (((u32RegisterValue & DD429_RX_CONTROL_MASK__FIFO_COUNT) >> 15))
                {
                    DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                    ARINC429WriteRxHostBuffer(pDeviceContext, u16Channel);
                    DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                }
            }
            if (sHbuf->u32HbufNumEntries > 0)
            {
                if (u32NumMsgsToRead > sHbuf->u32HbufNumEntries)
                {
                    u32NumMsgsToRead = sHbuf->u32HbufNumEntries;
                }

                /* do we have to read beyond the end of the buffer (wrapped around messages) */
                if ((sHbuf->u32HbufNextRead + (u32NumMsgsToRead * ARINC_RX_HBUF_MAX_MSG_SIZE)) > ARINC_RX_HBUF_MAX)
                {
                    u32NumMsgsToRead = (ARINC_RX_HBUF_MAX - sHbuf->u32HbufNextRead) / ARINC_RX_HBUF_MAX_MSG_SIZE;
                }

                /* get pointer to the starting address for copying of msgs */
                pMessages = (U32BIT*)sHbuf->pu8Buffer;

                /* calculate the number of bytes to copy */
                numBytesToCopy = u32NumMsgsToRead * ARINC_RX_HBUF_MAX_MSG_SIZE * sizeof(U32BIT);
                memcpy(((U8BIT *)(&(pReadData[2]))), ((U8BIT *)(&(pMessages[sHbuf->u32HbufNextRead]))), numBytesToCopy);

                if (sHbuf->bHbufStoreTimeTag)
                {
                    /* Using time tag */
                    pReadData[0] |= 0x80000000;
                }
                pReadData[1] = u32NumMsgsToRead;
                sHbuf->u32HbufNextRead += (u32NumMsgsToRead * ARINC_RX_HBUF_MAX_MSG_SIZE);
                sHbuf->u32HbufNumEntries -= u32NumMsgsToRead;
                if (sHbuf->u32HbufNextRead >= ARINC_RX_HBUF_MAX)
                {
                    sHbuf->u32HbufNextRead = 0;
                }
            }
        }
        DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
    }
    numBytesToCopy += (2 * sizeof(U32BIT));
    return numBytesToCopy;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429CreateRxHostBuffer

   Description:
        This function creates a host buffer for an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429CreateRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    ACEX_429HBUF_TYPE *sHbuf;
    size_t u32BufferByteSize;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer == NULL)
    {
        DDC_ISR_LOCK_INIT(sHbuf->sem429HBuf);

        /* Create host buffer */
        u32BufferByteSize = ARINC_RX_HBUF_MAX * sizeof(U32BIT);
        sHbuf->pu8Buffer = (U8BIT *) DDC_KERNEL_VIRTUAL_MALLOC(pDeviceContext, u32BufferByteSize);
        if (sHbuf->pu8Buffer == NULL)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_RX_HOST_BUFFER, "429 - ARINC429CreateRxHostBuffer Create Failure (pu8Buffer)\n");
            return;
        }
    }

    DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
    sHbuf->u32HbufNextRead = 0;
    sHbuf->u32HbufNextWrite = 0;
    sHbuf->u32HbufNumEntries = 0;
    sHbuf->bHbufOverflow = 0;
    sHbuf->bHbufQueueOverflow = 0;
    sHbuf->bHbufStoreTimeTag = FALSE;
    DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429DeleteRxHostBuffer

   Description:
        This function deletes the host buffer of an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429DeleteRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel
)
{
    ACEX_429HBUF_TYPE *sHbuf;

    /* does this device have programmable 429 channels */
    if (pDeviceContext->u8NumProg429RxTx > 0)
    {
        if (u16Channel > pDeviceContext->u8NumProg429RxTx)
        {
            return;
        }
    }
    else
    {
        /* device has dedicated 429 RX channels */
        if (u16Channel > pDeviceContext->u8NumDed429Rx)
        {
            return;
        }
    }

    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[u16Channel]);
    if (sHbuf->pu8Buffer != NULL)
    {
        DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);

        DDC_KERNEL_VIRTUAL_FREE(pDeviceContext, sHbuf->pu8Buffer);

        sHbuf->pu8Buffer = NULL;
        sHbuf->u32HbufNextRead = 0;
        sHbuf->u32HbufNextWrite = 0;
        sHbuf->u32HbufNumEntries = 0;
        sHbuf->bHbufOverflow = 0;
        sHbuf->bHbufQueueOverflow = 0;
        sHbuf->bHbufStoreTimeTag = FALSE;
        DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429InstallRxHostBuffer

   Description:
        This function installs a host buffer for Rx fifo mode.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429InstallRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    DDC_IOCTL_PARAMS tempIoctlParams;
    U32BIT u32RegisterValue;
    size_t bytesReturned;
    U32BIT u32Error;

    /* Enable 429 interrupts */
    tempIoctlParams.Param1 = DD429_GENERAL_COMMAND__INTERRUPT_ENABLE;
    tempIoctlParams.Param2 = CHAN_TYPE_429;
    tempIoctlParams.Param3 = DD429_ENABLE;
    _ARINC429GeneralCommand(pDeviceContext, &tempIoctlParams, &u32Error, sizeof(u32Error), &bytesReturned);

    pDeviceContext->s429RxHBuf.bRxFiFoHbufInstalled = TRUE;

    /* Enable 429 timer interrupt */
    u32RegisterValue = DD429_RX_HOST_BUFFER_INT_ENABLE | DD429_RX_HOST_BUFFER_INTERVAL_100MS;

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_429_TIMER_INTERRUPT_CONFIG, &u32RegisterValue);

    DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegisterValue);
    u32RegisterValue |= ACEX_429_RX_GLOBAL_INT_ENABLE_MS_TIMER_MASK;
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegisterValue);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429UninstallRxHostBuffer

   Description:
        This function uninstalls the host buffer for Rx fifo mode.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  disposing         - indicates whether the device is getting unloaded
   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void ARINC429UninstallRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    BOOLEAN disposing
)
{
    U32BIT i;
    U32BIT u32RegisterValue;
    U8BIT u8NumRx;

    /* Disable 429 interrupts */
    if (!disposing)
    {
        DDC_IOCTL_PARAMS tempIoctlParams;
        size_t bytesReturned;
        U32BIT u32Error;

        /* Disable 429 interrupts */
        tempIoctlParams.Param1 = DD429_GENERAL_COMMAND__INTERRUPT_ENABLE;
        tempIoctlParams.Param2 = CHAN_TYPE_429;
        tempIoctlParams.Param3 = DD429_DISABLE;
        _ARINC429GeneralCommand(pDeviceContext, &tempIoctlParams, &u32Error, sizeof(u32Error), &bytesReturned);
    }

    /* Disable 429 timer interrupt */
    if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H)  ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M)  ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||
        (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX))
    {
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_429_TIMER_INTERRUPT_CONFIG, &u32RegisterValue);
        u32RegisterValue &= ~(DD429_RX_HOST_BUFFER_INT_ENABLE);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_429_TIMER_INTERRUPT_CONFIG, &u32RegisterValue);

        DDC_REG_READ(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegisterValue);
        u32RegisterValue &= ~(ACEX_429_RX_GLOBAL_INT_ENABLE_MS_TIMER_MASK);
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sArinc429RxGlobal.pu32RegBA) + ACEX_429_RX_GLOBAL_INT_ENABLE_REG, &u32RegisterValue);
    }

    /* delete host buffer */

    /* does this device contain fixed RX channels? */
    if (pDeviceContext->u8NumDed429Rx > 0)
    {
        u8NumRx = pDeviceContext->u8NumDed429Rx;
    }
    else
    {
        /* this device contains programmable channels */
        u8NumRx = pDeviceContext->u8NumProg429RxTx;
    }

    for (i = 0; (i < u8NumRx) && (i < MAX_NUM_429_CHANNELS); i++)
    {
        ARINC429DeleteRxHostBuffer(pDeviceContext, i);
    }
    pDeviceContext->s429RxHBuf.bRxFiFoHbufInstalled = FALSE;
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429ChFree

   Description:
        This function releases resources created during initialization.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429ChFree
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    ARINC429UninstallRxHostBuffer(pDeviceContext, TRUE);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429HandleRxHostBufferInterrupt

   Description:
        This function handles interrupts to read data off the FIFO queue and copy
        them into the host buffer.

   Parameters:
      In  pDeviceContext        device-specific structure
      In  u32RxGlobalIS16BIT

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void ARINC429HandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32RxGlobalIS16BIT
)
{
    U32BIT i;
    U32BIT u32HbufNumEntries;
    U32BIT u32RegisterValue;
    U32BIT u32RegisterAddress;
    U32BIT u32RegisterValueBefore = 0;
    ACEX_429HBUF_TYPE *sHbuf;
    U32BIT u32Value;
    U8BIT u8NumRx;

    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_RX_HOST_BUFFER, "ARINC429HandleRxHostBufferInterrupt: Enter\n");

    /* does this device contain fixed RX channels? */
    if (pDeviceContext->u8NumDed429Rx > 0)
    {
        u8NumRx = pDeviceContext->u8NumDed429Rx;
    }
    else
    {
        /* this device contains programmable channels */
        u8NumRx = pDeviceContext->u8NumProg429RxTx;
    }

    if (pDeviceContext->s429RxHBuf.bRxFiFoHbufInstalled)
    {
        for (i = 0; (i < u8NumRx) && (i < MAX_NUM_429_CHANNELS); i++)
        {
            /* Read receiver interrupt status register */
            u32RegisterAddress = *(pDeviceContext->pRxChnl429[i]->pu32RegBA) + ACEX_429_RX_STATUS_REG;
            DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValueBefore);

            DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_RX_HOST_BUFFER, "Ch%d: Status: 0x%08x\n", i, u32RegisterValueBefore);

            if (u32RegisterValueBefore & 0x00000017)
            {
                /* Clear interrupt status register */
                DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValueBefore);

                if (u32RegisterValueBefore & 0x00000001)
                {
                    /* Queue overflow */
                    sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[i]);
                    if (sHbuf->pu8Buffer != NULL)
                    {
                        DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                        sHbuf->u32HbufNextWrite = 0;
                        sHbuf->u32HbufNextRead = 0;
                        sHbuf->u32HbufNumEntries = 0;
                        sHbuf->bHbufQueueOverflow = 1;
                        DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                    }
                    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_RX_HOST_BUFFER, "Ch%d: queue overflow\n", i);

                    /* Reset receiver */
                    u32RegisterAddress = *(pDeviceContext->pRxChnl429[i]->pu32RegBA) + ACEX_429_RX_CONTROL_REG;
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    u32RegisterValue |= 0x00002000;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    u32RegisterValue &= 0xFFFFDFFF;
                    DDC_REG_WRITE(pDeviceContext, u32RegisterAddress, &u32RegisterValue);
                    ARINC429ConfigInternalRxInterrupt(pDeviceContext, i);
                }
                else
                {
                    ARINC429WriteRxHostBuffer(pDeviceContext, i);
                }
            }
            else
            {
                /* check timer interrupt */
                if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40000K) ||
                    (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40100F) ||
                    (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40001H)  ||
                    (pDeviceContext->u16DeviceID == DDC_DEV_ID_DD40002M)  ||
                    (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) ||
					(pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700) ||					
                    (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX))
                {
                    if (u32RxGlobalIS16BIT & 0x40000000)
                    {
                        /* On 429 timer interrupt read messages only if the host buffer is empty
                           and no other interrupt flags are set */
                        sHbuf = &(pDeviceContext->s429RxHBuf.sHBuf[i]);
                        if (sHbuf->pu8Buffer != NULL)
                        {
                            DDC_ISR_LOCK_TAKE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                            u32HbufNumEntries = sHbuf->u32HbufNumEntries;
                            DDC_ISR_LOCK_GIVE(sHbuf->sem429HBuf, sHbuf->sem429HBufFlag);
                            if (u32HbufNumEntries == 0)
                            {
                                ARINC429WriteRxHostBuffer(pDeviceContext, i);
                            }

                            /* Metrics update */
                            /* Calcuate metrics */
                            if (sHbuf->u32HbufNextWrite >= sHbuf->u32HbufNextRead)
                            {
                                u32Value = (sHbuf->u32HbufNextWrite - sHbuf->u32HbufNextRead) * (U32BIT)100;
                                pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull = u32Value / (U32BIT)ARINC_RX_HBUF_MAX;
                                if (pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull > 100)
                                {
                                    pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull = 100;
                                }
                            }
                            else
                            {
                                pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull =
                                    (((U32BIT)ARINC_RX_HBUF_MAX - sHbuf->u32HbufNextRead + sHbuf->u32HbufNextWrite) * (U32BIT)100) / (U32BIT)ARINC_RX_HBUF_MAX;
                            }
                            if (pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentHigh < pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull)
                            {
                                pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentHigh = pDeviceContext->pRxChnl429[i]->u32Arinc429HbufPercentFull;
                            }
                        }
                    }
                }
            } /* else  */
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429GetQueueStatus

   Description:
        This function reads the count register (4) for an ARINC 429 Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u16Channel        - channel number

   Returns:
      Error code.
   ---------------------------------------------------------------------------------*/
S16BIT ARINC429GetQueueStatus
(
/* //todo - this is for usb, need to make sure that it works for all boards */
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u16Channel,
    size_t *pBytesReturned,
    U32BIT* pReadData
)
{
    U32BIT u32RegisterValue;
   /* U32BIT u32RegisterAddress; */

    u32RegisterValue = 0x00000000;
    /*u32RegisterAddress = *(pDeviceContext->pRxChnl429[u16Channel]->pu32RegBA) + MIO_429_RX_COUNT;*/

/* //todo    DDC_REG_READ(pDeviceContext, u32RegisterAddress, &u32RegisterValue, DDC_USB_ACC_TYPE_WAIT); */

    *pBytesReturned = sizeof(U32BIT);
    *pReadData = u32RegisterValue;

/* //todo    DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_GETRXQUEUESTATUS, "429 - ARINC429GetQueueStatus: Channel: %02d status: 0x%08X u32RegisterAddress  %x\n",
//todo        u16Channel, u32RegisterValue, u32RegisterAddress );
*/

        return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ArincDisableIrq
 *
 * Description:
 *
 *      This function disables all ARINC IRQ's for this card.
 *
 * In:  pDeviceContext
 * In:  u8DeviceNumber
 * Out: none
 ******************************************************************************/
void ArincDisableIrq
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8DeviceNumber
)
{

    if (pDeviceContext->bArincSerialIsrEnabled)
    {
        /* cancel pending 429 interrupt request */
        irqDisableInterrupt429(pDeviceContext, u8DeviceNumber);

        ARINC429DisableRxInterrupts(pDeviceContext);
    }
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429LoadTxQueueMore

   Description:
        This function sends a bunch of messages to be transmitted to the USB device
        in one bulk transfer.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pIoctlParams         - will contain the register address
      In  pLocalInOutBuffer           - list of messages to be transmitted.
      In  u32NumMessages    - number of messages

   Returns:
     Noner.

   History:

   ---------------------------------------------------------------------------------*/
VOID ARINC429LoadTxQueueMore
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pLocalInOutBuffer,
    U32BIT OutputBufferLength
)
{
    U32BIT u32Address;
    U16BIT  i;
    U16BIT  j;

    u32Address = ARINC429GetRegisterAddress(pDeviceContext, pIoctlParams);

    for (i = 0, j = 0; j < (OutputBufferLength); i=(U16BIT)(i+2), j++)
    {
        *(pDeviceContext->p429TxMemory +i) = u32Address;
        *(pDeviceContext->p429TxMemory + i + 1) = *(pLocalInOutBuffer + j);

        DDC_DBG_PRINT(DDC_DBG_MODULE_ARINC429, DDC_DBG_ARINC429_LOADTXQUEUEMORE,
            "429 - ARINC429BlockRegWrite Address: 0x%08X, Value: 0x%08X\n",
            *(pDeviceContext->p429TxMemory + i),
            *(pDeviceContext->p429TxMemory + i + 1));
    }

    DDC_BLK_REG_WRITE(pDeviceContext, u32Address, (void *)pDeviceContext->p429TxMemory, OutputBufferLength);
}

/*-------------------------------------------------------------------------------
   Function:
       ARINC429LoadTxQueueOne

   Description:
        This function sends a 2 messages to be transmitted to the USB device
        in one bulk transfer.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  pIoctlParams         - will contain the register address

   Returns:
     Noner.

   History:

   ---------------------------------------------------------------------------------*/
VOID ARINC429LoadTxQueueOne
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams
)
{
    U32BIT u32ChannSelectAddress;
    U32BIT u32TxFIFOAddress;

    u32ChannSelectAddress = ARINC429GetRegisterAddress(pDeviceContext, pIoctlParams);
    pIoctlParams->Param1 = pIoctlParams->Param2;
    u32TxFIFOAddress = ARINC429GetRegisterAddress(pDeviceContext, pIoctlParams);

    *(pDeviceContext->p429TxMemory + 0) = u32ChannSelectAddress;
    *(pDeviceContext->p429TxMemory + 1) = pIoctlParams->Channel;
    *(pDeviceContext->p429TxMemory + 2) = u32TxFIFOAddress;
    *(pDeviceContext->p429TxMemory + 3) = (U32BIT)pIoctlParams->Param3;

    DDC_BLK_REG_WRITE(pDeviceContext, 0, (void *)pDeviceContext->p429TxMemory, 2);
}

