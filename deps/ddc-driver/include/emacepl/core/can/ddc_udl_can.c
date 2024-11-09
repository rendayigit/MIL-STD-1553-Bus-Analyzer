/*******************************************************************************
 * FILE: ddc_udl_can.c
 *
 * DESCRIPTION:
 *
 * The purpose of this module is to provide functions to support CAN Bus.
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
#include "os/include/ddc_os_types_private.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "include/ddc_error_list.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/can/ddc_udl_can_private.h"

#define CAN_BUS_MEM_RX_SIZE                  (U32BIT)0xF00  /* 3840 Dwords */
#define CAN_BUS_MEM_TX_SIZE                  (U32BIT)0xF00  /* 3840 Dwords */

#define CAN_BUS_MEM_CONFIG_SIZE              (U32BIT)0x100  /* 256 Dwords */
#define CAN_BUS_MEM_STATUS_SIZE              (U32BIT)0x100  /* 256 Dwords */

#define CAN_PCI_DWORD_CHUNK_SIZE             0x0780 / CAN_BUS_DWORDS_PER_MSG /* Each message contains 6 DWords */


static void canDeleteRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel);

static BOOLEAN canCheckHostBufferOverflow(U32BIT u32NextRead, U32BIT u32NextWrite, U32BIT u32NumElements);

static void canCopyToHostBuffer(U32BIT* pHostBufferMemory, U32BIT* pMemory, U32BIT u32NumMessagesToCopy);

static S16BIT canCreateRxHostBuffer(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext, U8BIT u8Channel);


/*******************************************************************************
 * Name:    canBusInitialize
 *
 * Description:
 *      This function initializes all the CAN Bus data structures with the
 *      base register and memory locations for the various CAN Bus 717 components,
 *      Removes channels off the bus
 *
 * In   pDeviceContext          device-specific structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    U16BIT numDevicesIndex;
    size_t i;
    size_t j;
    UM_DEVICE_INFO       *pUmDevicePtr;
    ENHANCED_CAPABILITY_INFO sCapabilityInfo;
    U32BIT u32AddrBaseAddress;

    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "CAN Bus - Number of channels: %d\n", pDeviceContext->u8NumCanBus);

    memset(&(pDeviceContext->sCanBus), 0, sizeof(CAN_BUS_TYPE));

    /* Above setting will default CAN Bus state to ACEX_MOD_RESET */

    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "CAN Bus - numDevicesIndex %d umDevType %x\n", numDevicesIndex, pUmDevicePtr->umDevType);

        if (pUmDevicePtr->umDevType == UM_DEVICE_ID_CAN_BUS)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "CAN Bus - UM_DEVICE_ID_CAN_BUS\n");

            for (j = 0; j < pUmDevicePtr->umDevNumComponents; j++)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "CAN Bus - umComponentType %x \n", pUmDevicePtr->umComponentInfo[j].umComponentType);

                switch (pUmDevicePtr->umComponentInfo[j].umComponentType)
                {
                    case UM_COMPONENTS_ID_CAN_BUS:
                    {
                        pDeviceContext->sCanBus.pu32RegBA = &(pUmDevicePtr->umComponentInfo[j].umRegBaseAddr[0]);
                        pDeviceContext->sCanBus.pu32RegSize = &(pUmDevicePtr->umComponentInfo[j].umComponentRegSize);
                        pDeviceContext->sCanBus.pu32MemBA = &(pUmDevicePtr->umMemBaseAddr[0]);
                        pDeviceContext->sCanBus.pu32MemSize = &(pUmDevicePtr->umDevMemSize);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "        - Register: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sCanBus.pu32RegBA),
                            *(pDeviceContext->sCanBus.pu32RegSize));

                        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "        - Memory: BA:%08X  Size:%d\n",
                            *(pDeviceContext->sCanBus.pu32MemBA),
                            *(pDeviceContext->sCanBus.pu32MemSize));
                        break;
                    }

                    default:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE, "CAN Bus - UM_ERROR_UNKNOWN_COMPONENT_TYPE \n");
                        break;
                    }
                } /* switch */
            } /* for components */
        } /* if umDevType */
    } /* for numDevicesIndex */

    /* Retrieve channel count */
    ddcUdlBdReadEnhancedCapabilities(pDeviceContext, &sCapabilityInfo);

    pDeviceContext->u32CanBusIntStatus.u32BdInfoInt = 0;
    pDeviceContext->u32CanBusIntStatus.u32MsgCountStatus = 0;

    /* CAN Message Structure Structure

                                1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
       OFFSET     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +--------------------------------+----+-+-------+---------------+
     |        MSG PENDING ID          |    |E|C|C|S|R|               |
     | 0 0 0 0|0 1 0 0|1 0 1 1|1 0 1 1|1 1 0 0 |    |R|M|E|S|T|      CAN      |
     |  0     |   4   |    D  |   D   |   C    |X X |R|P|X|R|R|    CHANNEL    | Header
     |                                |    |O|N|I| | |    NUMBER     |
     |                                |    |R|D|D| | |               |
     +--------------------------------+----+-+-+-+-+-+---------------+
       1      |X X X|             C H A N N E L   I D                         |
     |+-----+-------------------------------------------------+-------+
       2      |X X X X X X X X X X X X X X X X X X X X X X X X X X X X| DLEN  |  # of data bytes
     |+-------------------------------+-----------------------+-------+
       3      |X X X X X X X X X X X X X X X X|    T I M E   S T A M P        |  Message time stamp
     |+---------------+---------------+---------------+---------------+
       4      |  DATA BYTE 4  |  DATA BYTE 3  |  DATA BYTE 2  |  DATA BYTE 1  |  Data bytes 1 - 4
     |+---------------+---------------+---------------+---------------+
       5      |  DATA BYTE 8  |  DATA BYTE 7  |  DATA BYTE 6  |  DATA BYTE 5  |  Data bytes 5 - 8
     |+---------------+---------------+---------------+---------------+

          "X" denotes not used at this time.

     */

    /* Need to configure CAN Bus dedicated memory into a structure encompassing 2 channels
     *  Configuration and status conbimed will occupy 0x100 (256 DWords) and remaining
     *  memory will be subdivided between transmitters/receivers.
     *  Currently:
     *      Memory base addres is 0x84000 with a size of 0x4000(16384) DWords.
     *      Removing 0x100 for configuration/status storage leave 0x3F00(16128) DWords for
     *      Transmitter/Receiver memory.  For 2 channels memory size translates to
     *      0x1F80(8064) DWords; 0xFC0(4032) DWords for Transmitter and same for Receiver.
     *      - 128 DWords per channel will be used for config & status per channel
     *      - 1st DWord thru 127th are config options for Channel 1
     *      - 128th DWord is interrupt status for Channel 1
     *      - 129th DWord thru 255th are config options for Channel 2
     *      - 256h DWord is interrupt status for Channel 2
     *      - Config word 1 contains config options to set.
     *        bits 1 - 4 is channel # to config
     *        bit  2 - if set denotes speed setting
     *        bit  3 - Set filter
     *        bit  31 - enable Debug UART
     *          Device IRQ 8 will be used to notify CAN processor, new configuration available.
     *
     *      */

    /* Memory Map - DWords
     *
     * Example  +-----------+ -- Firmware Ver, 4 digits, 8 bits/digit (255.255.255.255)
     *          |Ch2 Status |
     *  0x87F00 +-----------+ -- Ch2 Config + CAN_BUS_MEM_CONFIG_SIZE
     *          |Ch2 Config |
     *  0x87E00 +-----------+ -- Ch1 Config + CAN_BUS_MEM_STATUS_SIZE + CAN_BUS_MEM_STATUS_SIZE
     *          |Ch1 Status |
     *  0x87D00 +-----------+ -- Ch1 Config Base + CAN_BUS_MEM_CONFIG_SIZE
     *          |Ch1 Config |
     *  0x87C00 +-----------+ -- Ch2 Tx Base + CAN_BUS_MEM_TX_SIZE
     *          |  Ch2 Tx   |
     *  0x86D00 +-----------+ -- Ch2 Rx Base + CAN_BUS_MEM_RX_SIZE
     *          |  Ch2 Rx   |
     *  0x85E00 +-----------+ -- sCanBus.pu32MemBA + CAN_BUS_MEM_RX_SIZE + CAN_BUS_MEM_TX_SIZE
     *          |  Ch1 Tx   |
     *  0x84F00 +-----------+ -- Ch1 Rx Base + CAN_BUS_MEM_RX_SIZE
     *          |  Ch1 Rx   |
     *  0x84000 +-----------+ -- sCanBus.pu32MemBA
     *
     *  */

    /* Configure memory map and set default values */

    /*
       Always configure memory the same way - was using sCapabilityInfo.channelCountCanBus
       now using MaxCanBusChannelCount. CAN BUS memory allocations are fixed in both
       the 67211 and 67118 boards we must configure memory for Max of 2 channels.
     */
    for (i = 0; (i < MaxCanBusChannelCount) && (i < MAX_NUM_CAN_CHANNELS); i++)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE,
            "  Receiver Base Address 0x%08x, Size 0x%08x\n", pDeviceContext->sCanBus.u32MemRxBA[i], pDeviceContext->sCanBus.u32MemRxSize[i]);

        if (pDeviceContext->sCanBus.pu32MemBA == NULL)
        {
            return -1;
        }

        pDeviceContext->sCanBus.u32MemRxBA[i] =
            *pDeviceContext->sCanBus.pu32MemBA + (U32BIT)(i * (CAN_BUS_MEM_RX_SIZE + CAN_BUS_MEM_TX_SIZE));

        pDeviceContext->sCanBus.u32MemRxSize[i] = CAN_BUS_MEM_RX_SIZE;

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE,
            "        - Receiver Base Address 0x%08X, Size 0x%08X\n",
            pDeviceContext->sCanBus.u32MemRxBA[i], pDeviceContext->sCanBus.u32MemRxSize[i]);

        pDeviceContext->sCanBus.u32MemTxBA[i] = pDeviceContext->sCanBus.u32MemRxBA[i] + CAN_BUS_MEM_RX_SIZE;
        pDeviceContext->sCanBus.u32MemTxSize[i] = CAN_BUS_MEM_TX_SIZE;
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE,
            "        -  Transmitter Base Address 0x%08X, Size 0x%08X\n",
            pDeviceContext->sCanBus.u32MemTxBA[i], pDeviceContext->sCanBus.u32MemTxSize[i]);

        pDeviceContext->sCanBus.u32MemRxIndex[i] = 0;  /* Current index into Rx memory */
        pDeviceContext->sCanBus.u32MemTxIndex[i] = 0;  /* Current index into Tx memory */

    }

    if (i == 0)
    {
        j = 0;          /* just in case, avoids a crash scenario */
    }
    else
    {
        j = (i - 1);   /* j denotes last Tx/Rx channel mapped above */
    }

    /* Config & status memory; comes after last transmit memory */

    /*
       Always configure memory the same way - was using sCapabilityInfo.channelCountCanBus
       now using MaxCanBusChannelCount. CAN BUS memory allocations are fixed in both
       the 67211 and 67118 boards we must configure memory for Max of 2 channels.
     */
    for (i = 0; (i < MaxCanBusChannelCount) && (i < MAX_NUM_CAN_CHANNELS); i++)
    {
        pDeviceContext->sCanBusConfig[i].u8Speed = CAN_BUS_SPEED_1_MBS;
        pDeviceContext->eCanBusState[i] = CAN_BUS_RESET;

        pDeviceContext->sCanBus.u32MemConfig[i] =
            pDeviceContext->sCanBus.u32MemTxBA[j] + (U32BIT)CAN_BUS_MEM_TX_SIZE + (U32BIT)(i * (CAN_BUS_MEM_STATUS_SIZE + CAN_BUS_MEM_CONFIG_SIZE));
        pDeviceContext->sCanBus.u32MemStatus[i] = pDeviceContext->sCanBus.u32MemConfig[i] + CAN_BUS_MEM_CONFIG_SIZE;

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_INITIALIZE,
            "        - Configuration Base Address 0x%08X, Status Base Address 0x%08X\n",
            pDeviceContext->sCanBus.u32MemConfig[i], pDeviceContext->sCanBus.u32MemStatus[i]);

        /* Insure Host Buffer deleted.  May have been kept alive due to an abrupt termination */
        canDeleteRxHostBuffer(pDeviceContext, (U8BIT)i);
    }

    /* Board configuration resets CAN Bus
     * CAN Bus comes out of reset with the following default capability.
     * speed of 1MB/sec, no filtering and the following interrupt usage
     *  - IRQ8 indicates to the CAN Bus processor a new configuration is requested.
     *  - IRQ9 indicates to the CAN Bus processor new Rx data, on Channel 1, got moved to the host.
     *  - IRQ10 indicates to the CAN Bus processor new Rx data, on Channel 2, got moved to the host.
     *  - IRQ10 indicates to the CAN Bus processor new Rx data, on Channel 2, got moved to the host.
     *  - IRQ11 indicates to the CAN Bus processor new Rx data, on Channel 2, got moved to the host.
     *  */

    /* Firware Version located in last 4 bytes of shared RAM

       Always configure memory the same way - was using sCapabilityInfo.channelCountCanBus
       now using MaxCanBusChannelCount. CAN BUS memory allocations are fixed in both
       the 67211 and 67118 boards we must configure memory for Max of 2 channels.
     */
    if (sCapabilityInfo.channelCountCanBus > 0)
    {
        u32AddrBaseAddress = (MaxCanBusChannelCount *  /* was sCapabilityInfo.channelCountCanBus * */
            (CAN_BUS_MEM_RX_SIZE +
                CAN_BUS_MEM_TX_SIZE +
                CAN_BUS_MEM_CONFIG_SIZE +
                CAN_BUS_MEM_STATUS_SIZE)) - 1;

        /* Add 1st channel of Rx memory offset; start of memory */
        pDeviceContext->sCanBus.u32FirmwareVersionAddress = u32AddrBaseAddress + pDeviceContext->sCanBus.u32MemRxBA[0];
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    canBusOpen
 *
 * Description:
 *      This function configures all desired parameters of the CAN Bus
 *      engine.
 *
 * In   pDeviceContext    - device-specific structure
 * In   pConfig           - will contain configuration parameters
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
)
{
    S16BIT s32Status = DDC_UDL_ERROR__SUCCESS;
    U8BIT u8Channel = pConfig->u8Channel;
    U32BIT u32ConfigOption = pConfig->u32ConfigOption;
    U32BIT u32RegisterValue = 0;
    U8BIT i;
    U32BIT u32MemAddress;

    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumCanBus) || (u8Channel == 0))
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - u8NumCanBus %d  u8Channel %d\n", pDeviceContext->u8NumCanBus, u8Channel);
        return CAN_BUS_INVALID_CHANNEL;
    }

    /* Deduct 1 from channel number due to arrrays are 0 based */
    u8Channel--;
    if ((pDeviceContext->eCanBusState[u8Channel] == CAN_BUS_RUN) &&
        ((pConfig->u32ConfigOption & CAN_BUS_RESET_OPT) != CAN_BUS_RESET_OPT) &&
        ((pConfig->u32ConfigOption & CAN_BUS_TX_ONE_MSG_CH1) != CAN_BUS_TX_ONE_MSG_CH1) &&
        ((pConfig->u32ConfigOption & CAN_BUS_TX_ONE_MSG_CH2) != CAN_BUS_TX_ONE_MSG_CH2)) /* we do not want to open again if already opened, unless a channel reset is requested */
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "ACEX_ERROR_STATE u32ConfigOption %x eCanBusState[u8Channel] %x \n", pConfig->u32ConfigOption, pDeviceContext->eCanBusState[u8Channel]);
        return DDC_UDL_ERROR__STATE;
    }

    /* CAN Bus Configuration Words
     *      1st DWord determines which config option to perform.
     *          bit #  Action
     *           31   Denotes channel configation pending
     *            1   Speed configuration
     *            2   Reset CAN Bus Processor
     *           27   Enable CAN processor debug PORT (UART at 115Kbaud, 8 bits, 1 stop & no parity
     *      2nd DWord is speed setting
     *  */

    if ((u32ConfigOption & CAN_BUS_SPEED_OPT) == CAN_BUS_SPEED_OPT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - Channel %d Speed Set to %d\n",
            pConfig->u8Channel, pConfig->u8Speed);

        /* Speed selection */
        switch (pConfig->u8Speed)
        {
            case CAN_BUS_SPEED_1_MBS:
            case CAN_BUS_SPEED_800_KBS:
            case CAN_BUS_SPEED_500_KBS:
            case CAN_BUS_SPEED_250_KBS:
            case CAN_BUS_SPEED_125_KBS:
            case CAN_BUS_SPEED_100_KBS:
            case CAN_BUS_SPEED_50_KBS:
            case CAN_BUS_SPEED_40_KBS:
            case CAN_BUS_SPEED_20_KBS:
#if 0
            case CAN_BUS_SPEED_10_KBS:
            case CAN_BUS_SPEED_5_KBS:
#endif
                {
                    u32RegisterValue = (U32BIT)pConfig->u8Speed;

                    /* Save value in global device storage */
                    pDeviceContext->sCanBusConfig[u8Channel].u8Speed = pConfig->u8Speed;
                    u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_SPEED_OFFSET;
                    DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32RegisterValue, 0, 1);
                    break;
                }
            default:    /* For invalid speed, exit with error condition */
            {
                return CAN_BUS_INVALID_SPEED;
            }
        } /* switch (pConfig->u8Speed) */
    }

    pDeviceContext->u32CanBusIntStatus.u32BdInfoInt = 0;
    pDeviceContext->u32CanBusIntStatus.u32MsgCountStatus = 0;

    if ((u32ConfigOption & CAN_BUS_RESET_OPT) == CAN_BUS_RESET_OPT)
    {
        /* Insure CAN board level interrupt disabled */
        ddcUdlBdInterruptClear(pDeviceContext, (BD_INT_STATUS_MASK_CAN_1 << u8Channel));

        for (i = 0; (i < pDeviceContext->u8NumCanBus) && (i < MAX_NUM_CAN_CHANNELS); i++)
        {
            pDeviceContext->sCanBusConfig[i].u8Speed = CAN_BUS_SPEED_1_MBS;
            pDeviceContext->sCanBus.u32MemRxIndex[i] = 0;  /* Current index into Rx memory */
            pDeviceContext->sCanBus.u32MemTxIndex[i] = 0;  /* Current index into Tx memory */
            pDeviceContext->sCanBus.state[i] = ACEX_MOD_RESET;
            canDeleteRxHostBuffer(pDeviceContext, i);
        }

        ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, BD_RESET_CAN_BUS);

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - Reset CAN Bus");
    }
    if ((u32ConfigOption & CAN_BUS_RX_FILTER_OPT) == CAN_BUS_RX_FILTER_OPT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - Channel %d Filter Set\n",
            pConfig->u8Channel);

        /* Write filter values
         * 1st 29 bits are for ID, extended and standard.  Bit 31 denotes to use extended or standard ID */
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_RX_FILTER_OFFSET;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, (U32BIT *)&pConfig->u32FilterValues, 0, CAN_BUS_RX_FILTER_MAX);
    }
    if ((u32ConfigOption & CAN_BUS_RX_INTERRUPT_OPT) == CAN_BUS_RX_INTERRUPT_OPT)
    {
        if (pConfig->bInterrupt == TRUE)
        {
            /* Enable CAN board level interrupt */
            ddcUdlBdInterruptSet(pDeviceContext, (BD_INT_STATUS_MASK_CAN_1 << u8Channel));
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - Channel %d Interrupt Enabled\n",
                pConfig->u8Channel);
        }
        else
        {
            /* Disable CAN board level interrupt */
            ddcUdlBdInterruptClear(pDeviceContext, (BD_INT_STATUS_MASK_CAN_1 << u8Channel));
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "canBusOpen - Channel %d Interrupt Disabled\n",
                pConfig->u8Channel);
        }
    }
    if ((u32ConfigOption & CAN_BUS_TX_ONE_MSG_CH1) == CAN_BUS_TX_ONE_MSG_CH1)
    {
        /* Debug transmission on channel 1 */
    }
    if ((u32ConfigOption & CAN_BUS_TX_ONE_MSG_CH2) == CAN_BUS_TX_ONE_MSG_CH2)
    {
        /* Debug transmission on channel 2 */
    }
    if ((u32ConfigOption & CAN_BUS_TIMER_OPT) == CAN_BUS_TIMER_OPT)
    {
        u32RegisterValue = pConfig->u16TimerValue;

        /* 1 to 1000ms range */
        if ((u32RegisterValue > CAN_BUS_MAX_TIMER_MS_INT) || (u32RegisterValue == 0))
        {
            return DDC_UDL_ERROR__HARDWARE_CONFIGURATION;
        }
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_TIMER_INT_OFFSET;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32RegisterValue, 0, 1);
    }
    if ((u32ConfigOption & CAN_BUS_MESSAGE_COUNT_INT_OPT) == CAN_BUS_MESSAGE_COUNT_INT_OPT)
    {
        u32RegisterValue = pConfig->u16MessageCountInt;

        /* 1 to 500 message count range */
        if ((u32RegisterValue > CAN_BUS_MAX_MSG_COUNT_INT) || (u32RegisterValue == 0))
        {
            return DDC_UDL_ERROR__HARDWARE_CONFIGURATION;
        }
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_MSG_CNT_INT_OFFSET;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32RegisterValue, 0, 1);
    }
    if ((u32ConfigOption & CAN_BUS_MONITOR_ONLY_OPT) == CAN_BUS_MONITOR_ONLY_OPT)
    {
        u32RegisterValue = pConfig->u8EnableMonitor;
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_MONITOR_STATE;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32RegisterValue, 0, 1);
    }
    if ((u32ConfigOption & CAN_BUS_INTERNAL_LOOP_BACK_OPT) == CAN_BUS_INTERNAL_LOOP_BACK_OPT)
    {
        u32RegisterValue = pConfig->u8EnableLoopback;
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_LOOP_BACK_OFFSET;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32RegisterValue, 0, 1);
    }

    if ((u32ConfigOption & CAN_BUS_RESET_OPT) != CAN_BUS_RESET_OPT)
    {
        u32ConfigOption |= CAN_BUS_CONFIG_PENDING;
        u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_MEM_OFFSET;
        DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, &u32ConfigOption, 0, 1);

        /* Initiate CAN Bus configuration interrupt.  Interrupt causes CAN Bus to read shared memory */
        /* configuration address, and act on it. Any value written to this register will cause an interrupt. */
        /* Currently Firmware will toggle CAN Bus IRQ8 to achieve this action. */
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sCanBus.pu32RegBA) + REG_CAN_BUS_CONFIG_CH1_INTERRUPT_TRIGGER,
            &u32MemAddress);

        s32Status = canCreateRxHostBuffer(pDeviceContext, u8Channel);

        pDeviceContext->eCanBusState[u8Channel] = CAN_BUS_READY;
        pDeviceContext->sCanBus.state[u8Channel] = ACEX_MOD_OPEN;
    }
    else
    {
        pDeviceContext->eCanBusState[u8Channel] = CAN_BUS_RESET;
        pDeviceContext->sCanBus.state[u8Channel] = ACEX_MOD_RESET;
    }

    return s32Status;
}

/*******************************************************************************
 * Name:    canBusClose
 *
 * Description:
 *      This function closes operations for the desired CAN Bus channel
 *      engine.
 *
 * In   pDeviceContext  - device-specific structure
 * In   pConfig         - channel to close within structure
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
)
{
    U8BIT i;
    S16BIT s32Status = DDC_UDL_ERROR__SUCCESS;
    U8BIT u8Channel = pConfig->u8Channel;

    /* Max channel check */
    if (u8Channel > pDeviceContext->u8NumCanBus)
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    /* Deduct 1 from channel number due to arrrays are 0 based */
    u8Channel--;

    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_CLOSE, "canBusClose - Channel %d Closed\n", (u8Channel + 1));

    for (i = 0; i < pDeviceContext->u8NumCanBus; i++)
    {
        pDeviceContext->sCanBus.u32MemRxIndex[i] = 0;  /* Current index into Rx memory */
        pDeviceContext->sCanBus.u32MemTxIndex[i] = 0;  /* Current index into Tx memory */
        /* Disable CAN board level interrupts */
        ddcUdlBdInterruptClear(pDeviceContext, (BD_INT_STATUS_MASK_CAN_1 << i));
        canDeleteRxHostBuffer(pDeviceContext, i); /* Delete host buffer */
    }

    /* Reset CAN Bus processor */
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, BD_RESET_CAN_BUS);

    /* Initialize state machines */
    pDeviceContext->eCanBusState[u8Channel] = CAN_BUS_RESET;
    pDeviceContext->sCanBus.state[u8Channel] = ACEX_MOD_CLOSED;

    return s32Status;
}

/*******************************************************************************
 * Name:    CanBusSetState
 *
 * Description:
 *      Sets The State Of The CAN Bus Engine (RESET, READY, RUN).
 *
 * In   pDeviceContext - input value for instance information
 * In   pConfig - input values for Channel & State
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusSetState
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    CAN_BUS_CONFIG *pConfig
)
{
    U32BIT u32MemAddress;
    U32BIT u32ConfigOption[2];
    U8BIT u8Channel = (U8BIT)pConfig->u8Channel;
    CAN_BUS_RUN_STATE canBusRunState = pConfig->eState;

    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE,  "CanBusSetState - Begin, New State Channel %d State is now %d\n",
        pConfig->u8Channel, pConfig->eState);

    /* Max channel check */
    if ((u8Channel > pDeviceContext->u8NumCanBus) || (u8Channel == 0))
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    /* Register and status are 0 based arrays; adjust accordingly */
    u8Channel--;

    switch (canBusRunState)
    {
        case CAN_BUS_RUN:
        case CAN_BUS_PAUSE:
        case CAN_BUS_READY:
        case CAN_BUS_RESET:
        {
            /* Save State */
            pDeviceContext->eCanBusState[u8Channel] = canBusRunState;
            break;
        }

        default:
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE, "CanBusSetState - Unknown CAN Bus State Set\n");
            break;
        }
    }

    u32ConfigOption[0] = CAN_BUS_CONFIG_PENDING | CAN_BUS_RUN_STATE_OPT;

    /* Run state comes immediately after config option to speed process due to USB pipeline*/
    u32ConfigOption[1] = (U32BIT)canBusRunState;
    u32MemAddress = pDeviceContext->sCanBus.u32MemConfig[u8Channel] + CAN_BUS_CONFIG_OPTION_MEM_OFFSET;
    DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32MemAddress, (U32BIT *)&u32ConfigOption, 0, 2);

    /* Initiate CAN Bus configuration interrupt.  Interrupt causes CAN Bus to read shared memory */
    /* configuration address, and act on it. Any value written to thsi register will cause an interrupt. */
    /* Currently Firmware will toggle CAN Bus IRQ8 to achieve this action. */
    if (u8Channel == 1)
    {
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sCanBus.pu32RegBA) + REG_CAN_BUS_CONFIG_CH1_INTERRUPT_TRIGGER,
            (U32BIT *)&u32ConfigOption);
    }
    else {
        DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->sCanBus.pu32RegBA) + REG_CAN_BUS_CONFIG_CH2_INTERRUPT_TRIGGER,
            (U32BIT *)&u32ConfigOption);
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_SET_STATE, "CanBusSetState - End\n");

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    canBusTransmitData
 *
 * Description:
 *      This function loads CAN Bus transmit data into shared memory
 *
 * In   pDeviceContext          device-specific structure
 * In   pIoctlParams            Transmission parameters
 * In   pWrData                 Raw Data to be transmitted
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusTransmitData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT *pWrData
)
{
    U32BIT u32TxAddress = 0;
    U32BIT i = 0;
    U32BIT u8MsgCount = DDC_IOCTL_U32(pIoctlParams->Param1);
    U8BIT u8Channel = (U8BIT)(pIoctlParams->Channel - 1);
    U8BIT u8DWordsTx = CAN_BUS_DWORDS_PER_MSG;
    U32BIT u32MemReadValue;

#if 1
    U32BIT u32PendingAddress = 0;
    U32BIT u32HeaderValue = 0;
#endif

    /* Max channel check */
    if ((pIoctlParams->Channel > pDeviceContext->u8NumCanBus) || (pIoctlParams->Channel == 0))
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    if (pDeviceContext->eCanBusState[u8Channel] != CAN_BUS_RUN)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_TRANSMIT,  "canBusTransmitData - Ch %i, Invalid state for operation.\n",
            (int)pIoctlParams->Channel);
        return CAN_BUS_INVALID_STATE_ERR;
    }

    if (pWrData == NULL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_TRANSMIT,  "canBusTransmitData Start - Ch %d, pWrData = NULL.\n",
            (int)pIoctlParams->Channel);
        return DDC_UDL_ERROR__NULL_PTR;
    }

    /* Each messages consists of 6 DWords. See canBusInitialize for further details */
    for (i = 0; i < u8MsgCount; i++)
    {
        u32TxAddress = pDeviceContext->sCanBus.u32MemTxBA[u8Channel] + pDeviceContext->sCanBus.u32MemTxIndex[u8Channel];

        DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
            u32TxAddress,
            &u32MemReadValue,
            4,                                  /* bytes */
            NULL);

        /* This will slow down transmission through-put */
        if ((u32MemReadValue & CAN_MESSAGE_PENDING) == CAN_MESSAGE_PENDING)
        {
            return CAN_BUS_TX_BUSY;
        }

        /* Check if 6 DWORDS can be placed in shared memory without rollover */
        /* NOTE: depending on boundary, may never execute this path */
        if ((pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] + CAN_BUS_DWORDS_PER_MSG) > CAN_BUS_MEM_TX_SIZE)
        {
            u8DWordsTx = (U8BIT)(CAN_BUS_MEM_TX_SIZE - pDeviceContext->sCanBus.u32MemTxIndex[u8Channel]);

            /* For speeds less than 500K bps, overruns can occur, therefore must perform an addition */
            /* read to determine if data currently in CAN processor transmission queue prior to */
            /* transmission */

            u32PendingAddress = u32TxAddress;
            u32HeaderValue = pWrData[0];
            pWrData[0] = 0;

            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_TRANSMIT,
                "canBusTransmitData - Ch %d Transmitting %d DWord(s), Start Address 0x%08X\n",
                (int)pIoctlParams->Channel, (int)u8DWordsTx, u32TxAddress);

            DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32TxAddress, pWrData, 0, u8DWordsTx);

            /* Header value written last, to insure CAN processor will not read message packet prior to a complet fill */
            pWrData[0] = u32HeaderValue;
            DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32PendingAddress, pWrData, 0, 1);

            pWrData = pWrData + u8DWordsTx;

            /* Transmitt remaining DWords with address adjusted to beginning of Tx memory */
            u8DWordsTx = (U8BIT)(CAN_BUS_MEM_TX_SIZE - u8DWordsTx);
            pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] = 0;
            u32TxAddress = pDeviceContext->sCanBus.u32MemTxBA[u8Channel] + pDeviceContext->sCanBus.u32MemTxIndex[u8Channel];
            DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32TxAddress, pWrData, 0, u8DWordsTx);
            pWrData = pWrData + u8DWordsTx;

            /* Rollover check not required here because pointer was placed at begining of memory and */
            /*  DWords transmitted will always be smaller than memory size */
            pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] += u8DWordsTx;
        }
        else
        {
            u8DWordsTx = CAN_BUS_DWORDS_PER_MSG;
            u32PendingAddress = u32TxAddress;
            u32HeaderValue = pWrData[0];
            pWrData[0] = 0;

            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_TRANSMIT,
                "canBusTransmitData No Roll Over - Ch %d Transmitting %d DWord(s), Start Address 0x%08X\n",
                (int)pIoctlParams->Channel, (int)u8DWordsTx, u32TxAddress);

            DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32TxAddress, pWrData, 0, u8DWordsTx);

            pWrData[0] = u32HeaderValue;
            DDC_BLK_MEM_WRITE_SWAP(pDeviceContext, u32PendingAddress, pWrData, 0, 1);

            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_TRANSMIT,
                "  0 = %x 1 = %x 2 = %x 3 = %x 4 = %x 5 = %x u32PendingAddress = %x \n",
                (int)pWrData[0], (int)pWrData[1], (int)pWrData[2], (int)pWrData[3], (int)pWrData[4], (int)pWrData[5], u32PendingAddress);

            pWrData = pWrData + CAN_BUS_DWORDS_PER_MSG;
            pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] += CAN_BUS_DWORDS_PER_MSG;

            /* Rollover Check */
            if (pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] >= CAN_BUS_MEM_TX_SIZE)
            {
                pDeviceContext->sCanBus.u32MemTxIndex[u8Channel] = 0;
            }
        }
#if 0
        /* Cause CAN processor transmit interrupt via write of appropriate Firmware register */
        DDC_REG_WRITE(pDeviceContext,
            *(pDeviceContext->sCanBus.pu32RegBA) + (REG_CAN_BUS_TX_CH1_INTTERRUPT_TRIGGER + u8Channel),
            &i, 1);
#endif
    } /* for (i = 0; i < u8MsgCount; i++) */

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    canBusReadSharedMemory
 *
 * Description:
 *      This function reads data off the CAN Bus shared memory in bulk transfers
 *
 * In   pDeviceContext          device-specific structure
 * In   u32Channel              Channel number
 * In   u32NumMessagesToRead    No of messages to read
 * In   pRdData                 pointer to buffer where read data will go
 *
 * Returns: Error condition
 ******************************************************************************/
S16BIT canBusReadSharedMemory
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel,
    U32BIT *pRdData,
    U32BIT u32RxMsgsToRead
)
{
    U32BIT u32AddrAddress = 0;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U16BIT u16DWords = CAN_BUS_DWORDS_PER_MSG;
#if 0
    U32BIT u32EraseIndex = 0;
    U8BIT u8MsgsRead = 0;
    U8BIT i;
#endif

    /* Max channel check */
    if (u8Channel > pDeviceContext->u8NumCanBus)
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    /* Shared memory rollover check */
    if ((pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] + (CAN_BUS_DWORDS_PER_MSG * u32RxMsgsToRead)) > CAN_BUS_MEM_RX_SIZE)
    {
        u16DWords = (U16BIT)(CAN_BUS_MEM_RX_SIZE - pDeviceContext->sCanBus.u32MemRxIndex[u8Channel]);
        u32AddrAddress = pDeviceContext->sCanBus.u32MemRxBA[u8Channel] + pDeviceContext->sCanBus.u32MemRxIndex[u8Channel];

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RECEIVE,  "canBusReadSharedMemory - Ch %d Reading %d DWs at Start Address 0x%08X\n",
            (u8Channel + 1), u16DWords, u32AddrAddress);

        DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
            u32AddrAddress,
            pRdData,
            ((U32BIT)u16DWords),                              /* bytes */
            NULL);

        pRdData = pRdData + u16DWords;

        /* Retrieve remaining DWords with address adjusted to beginning of Rx memory */
        u16DWords = (U16BIT)((u32RxMsgsToRead * CAN_BUS_DWORDS_PER_MSG) - u16DWords);
        pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] = 0;
        u32AddrAddress = pDeviceContext->sCanBus.u32MemRxBA[u8Channel];

        DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
            u32AddrAddress,
            pRdData,
            ((U32BIT)u16DWords * 4),                               /* bytes */
            NULL);

/*        pRdData = pRdData + u16DWords; */

        /* Rollover check not required here because pointer was placed at begining of memory and */
        /*  DWords transmitted will always be smaller than memory size */
        pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] += u16DWords;
    } /* Shared memory rollover check */
    else
    {
        u16DWords = (U16BIT)(u32RxMsgsToRead * CAN_BUS_DWORDS_PER_MSG);
        u32AddrAddress = pDeviceContext->sCanBus.u32MemRxBA[u8Channel] + pDeviceContext->sCanBus.u32MemRxIndex[u8Channel];

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RECEIVE,  "canBusReadSharedMemory a - Ch %d Reading %d DWs at Start Address 0x%08X\n",
            (u8Channel + 1), u16DWords, u32AddrAddress);

        DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
            u32AddrAddress,
            pRdData,
            ((U32BIT)u16DWords * 4),                               /* bytes */
            NULL);

/*        pRdData = pRdData + (u32RxMsgsToRead * CAN_BUS_DWORDS_PER_MSG); */
        pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] += (u32RxMsgsToRead * CAN_BUS_DWORDS_PER_MSG);
#if 0
        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RECEIVE,  "pRdData  %x  %x  %x  %x  %x  %x  %x\n",
            pRdData[0], pRdData[1], pRdData[2], pRdData[3], pRdData[4], pRdData[5], pRdData[6]);

#endif

        /* Rollover Check */
        if (pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] >= CAN_BUS_MEM_RX_SIZE)
        {
            pDeviceContext->sCanBus.u32MemRxIndex[u8Channel] = 0;
        }
    }

    return status;
}

/*-------------------------------------------------------------------------------
   Function:
       canCreateRxHostBuffer

   Description:
        This function creates a host buffer for an CAN Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static S16BIT canCreateRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    CAN_HBUF_TYPE *sHbuf;

    if (u8Channel < pDeviceContext->u8NumCanBus)
    {
        sHbuf = &(pDeviceContext->sCanRxHBuf.sHBuf[u8Channel]);

        /* Check if buffer already created */
        if (sHbuf->hMemory == NULL)
        {
            size_t u32BufferByteSize;

            /* initialize INT spin lock */
            DDC_ISR_LOCK_INIT(sHbuf->hMutex);
            DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slCanModeFlag);

            u32BufferByteSize = (U32BIT)CAN_RX_HBUF_MAX * sizeof(U32BIT);

            /* Create host buffer */
            sHbuf->hMemory = DDC_KERNEL_VIRTUAL_MALLOC(pDeviceContext, u32BufferByteSize);
            if (!sHbuf->hMemory)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_CREATE_RX_HOST_BUFFER,
                    "CAN Bus - canCreateRxHostBuffer Memory Create Failure (hUsbReadBuffer)\n");
            }
            else
            {
                sHbuf->u32HbufNextRead = 0;
                sHbuf->u32HbufNextWrite = 0;
                sHbuf->u32HbufNumEntries = 0;
                sHbuf->bHbufOverflow = 0;
                sHbuf->bHbufQueueOverflow = 0;
                sHbuf->bHbufStoreTimeTag = FALSE;

                /* Create usb read buffer buffer */
                u32BufferByteSize = ((CAN_RX_QUEUE_MAX * CAN_RX_HBUF_MAX_MSG_SIZE) + 1) * sizeof(U32BIT);

                sHbuf->hUsbReadBuffer = DDC_KERNEL_MALLOC(pDeviceContext, u32BufferByteSize);

                if (!sHbuf->hUsbReadBuffer)
                {
                    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_CREATE_RX_HOST_BUFFER,
                        "CAN - canCreateRxHostBuffer Memory Create Failure (hUsbReadBuffer)\n");
                }
            }
            DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slCanModeFlag);
        } /* if (sHbuf->hMemory == NULL) */
    }
    return status;
}

/*-------------------------------------------------------------------------------
   Function:
       canHandleRxHostBufferInterrupt

   Description:
        This function writes messages to the host buffer of an CAN Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  interruptStatus   - interrupt status structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
void canHandleRxHostBufferInterrupt
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT *pu32IntStatus
)
{
    U32BIT u32AddrAddress = 0;
    U32BIT numMessagesToCopy = 0;
    U32BIT numMessagesToRead;
    U32BIT u32NumMessagesLeftToRead;
    U32BIT*  pu32UsbReadBuffer;
    U32BIT pu32UsbReadBufferIndex;
    U8BIT u32Channel = 0;
    S16BIT status;
    U32BIT*  pHostBuffer;
    CAN_HBUF_TYPE *sHbuf;

    if ((pu32IntStatus[0] & BD_INT_STATUS_MASK_CAN_1) == BD_INT_STATUS_MASK_CAN_1)
    {
        u32Channel = 0;
    }
    else if ((pu32IntStatus[0] & BD_INT_STATUS_MASK_CAN_2) == BD_INT_STATUS_MASK_CAN_2)
    {
        u32Channel = 1;
    }

    pDeviceContext->u32CanBusIntStatus.u32BdInfoInt = pu32IntStatus[0];

    sHbuf = &(pDeviceContext->sCanRxHBuf.sHBuf[u32Channel]);
    if ((sHbuf->hMemory != NULL) && (sHbuf->hUsbReadBuffer != NULL))
    {
        pHostBuffer = (U32BIT*)sHbuf->hMemory;
        pu32UsbReadBuffer = (U32BIT*)sHbuf->hUsbReadBuffer;

        /* Read the number of messages in the queue */
#if 0  /* <UDL20> */
        numMessagesToRead = pDeviceContext->sCanBus.u32MemStatus[u32Channel];
#else
        u32AddrAddress = pDeviceContext->sCanBus.u32MemStatus[u32Channel];
        DDC_BLK_MEM_READ_IOCTL( pDeviceContext,
            u32AddrAddress,
            &numMessagesToRead,
            (sizeof(U32BIT)),                              /* bytes */
            NULL);

        /* db_printf("canHandleRxHostBufferInterrupt numMessagesToRead %d \n", numMessagesToRead); */
        pDeviceContext->u32CanBusIntStatus.u32MsgCountStatus = numMessagesToRead;
#endif

        /* Read Maximum Of 4K Bytes due to USB pipeline */

        /* NOTE: each message is 6 DWords in size and shared memory can hold maximum of */
        /* 640 messages; translates to maximum of 3840 DWords, which is much greater than 1024 DWords */
        /* alloted per USB read transfer */
        if (numMessagesToRead > CAN_PCI_DWORD_CHUNK_SIZE)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HOST_BUFFER,
                "CAN - Ch %d Request read > 320 msgs, is %d\n", (u32Channel + 1), numMessagesToRead);

            numMessagesToRead = CAN_PCI_DWORD_CHUNK_SIZE;
            u32NumMessagesLeftToRead = numMessagesToRead - CAN_PCI_DWORD_CHUNK_SIZE;
        }
        else
        {
            u32NumMessagesLeftToRead = 0;
        }

        while (numMessagesToRead > 0)
        {
            DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HOST_BUFFER,
                "CAN - Ch %d Msg Read # %d\n", (u32Channel + 1), numMessagesToRead);

            status = canBusReadSharedMemory(pDeviceContext, u32Channel, pu32UsbReadBuffer, numMessagesToRead);

            if (status != DDC_UDL_ERROR__SUCCESS)
            {
                DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HOST_BUFFER,
                    "CAN - canWriteRxHostBuffer Channel %02d Memory Read Failure\n", (u32Channel + 1));
            }
            else
            {
                /* CAN'T use ExAcquireFastMutex at dispatch level must be used at APC or Passive level */
                /* DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slCanModeFlag); */

                /* Host buffer overflow? */
                if (canCheckHostBufferOverflow(sHbuf->u32HbufNextRead,
                        sHbuf->u32HbufNextWrite, CAN_RX_HBUF_MAX_MSG_SIZE * numMessagesToRead))
                {
                    pDeviceContext->u32CanHbufOverflowCount[u32Channel] += 1;  /* Update for metrics reporting */

                    canCopyToHostBuffer(&(pHostBuffer[0]), &(pu32UsbReadBuffer[0]), numMessagesToRead);
                    sHbuf->u32HbufNextWrite = (numMessagesToRead * CAN_RX_HBUF_MAX_MSG_SIZE);
                    sHbuf->u32HbufNextRead = 0;
                    sHbuf->u32HbufNumEntries = numMessagesToRead;
                    sHbuf->bHbufOverflow = TRUE;
                    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HOST_BUFFER,
                        "CAN - Host Buffer overflow: Channel: %02d, Read addr. 0x%08X: Write addr. 0x%08X: Msg Count %d\n",
                        u32Channel, sHbuf->u32HbufNextRead, sHbuf->u32HbufNextWrite, numMessagesToRead);
                }
                else
                {
                    /* Host buffer rollover? */
                    if ((sHbuf->u32HbufNextWrite + (numMessagesToRead * CAN_RX_HBUF_MAX_MSG_SIZE)) > (U32BIT)CAN_RX_HBUF_MAX)
                    {
                        pu32UsbReadBufferIndex = 0;
                        numMessagesToCopy = ((U32BIT)CAN_RX_HBUF_MAX - sHbuf->u32HbufNextWrite) / CAN_RX_HBUF_MAX_MSG_SIZE;

                        /* Copy items up to the end of the host buffer */
                        canCopyToHostBuffer(&(pHostBuffer[sHbuf->u32HbufNextWrite]),
                            &(pu32UsbReadBuffer[pu32UsbReadBufferIndex]),
                            numMessagesToCopy);

                        /* Copy the remaining items starting at the beginning of the host buffer */
                        pu32UsbReadBufferIndex += (numMessagesToCopy * CAN_RX_HBUF_MAX_MSG_SIZE);
                        numMessagesToCopy = numMessagesToRead - numMessagesToCopy;
                        canCopyToHostBuffer(&(pHostBuffer[0]),
                            &(pu32UsbReadBuffer[pu32UsbReadBufferIndex]),
                            numMessagesToCopy);

                        sHbuf->u32HbufNextWrite = numMessagesToCopy * CAN_RX_HBUF_MAX_MSG_SIZE;
                        sHbuf->u32HbufNumEntries += numMessagesToRead;
                    }
                    else
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HOST_BUFFER,
                            "numMessagesToRead %d\n", numMessagesToRead);

                        canCopyToHostBuffer(&(pHostBuffer[sHbuf->u32HbufNextWrite]),
                            &(pu32UsbReadBuffer[0]),
                            numMessagesToRead);

                        sHbuf->u32HbufNumEntries += numMessagesToRead;
                        sHbuf->u32HbufNextWrite += (numMessagesToRead * CAN_RX_HBUF_MAX_MSG_SIZE);
                        if (sHbuf->u32HbufNextWrite >= (U32BIT)CAN_RX_HBUF_MAX)
                        {
                            sHbuf->u32HbufNextWrite = 0;
                        }
                    }
                }

                /* CAN'T use ExReleaseFastMutex at dispatch level must be used at APC or Passive level */
                /* DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slCanModeFlag);*/
            }

            /* Check if messages still remain to be read */
            if (u32NumMessagesLeftToRead > 0)
            {
                numMessagesToRead = u32NumMessagesLeftToRead;
            }
            else
            {
                numMessagesToRead = 0;
            }
        } /* while(numMessagesToRead > 0) */
    }
}

/*-------------------------------------------------------------------------------
   Function:
       canCheckHostBufferOverflow

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
static BOOLEAN canCheckHostBufferOverflow
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
        if ((u32NextWrite + u32NumElements) > (U32BIT)CAN_RX_HBUF_MAX)
        {
            result = (BOOLEAN)(((u32NumElements - ((U32BIT)CAN_RX_HBUF_MAX - u32NextWrite) - 1)) >= u32NextRead);
        }
    }
    return result;
}

/*-------------------------------------------------------------------------------
   Function:
       canBusReadData

   Description:
        This function reads messages from the host buffer of an CAN Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
S32BIT canBusReadData
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    U32BIT* pReadData,
    size_t *pOutputBufferLength
)
{
    size_t numBytesToCopy = 0;
    U32BIT*        pMessages;
    U32BIT u32Flags = 0x00000000;
    CAN_HBUF_TYPE *sHbuf;
    U8BIT u8Channel = (U8BIT)(pIoctlParams->Channel - 1);
    U32BIT u32NumMsgsToRead = DDC_IOCTL_U32(pIoctlParams->Param1);

    *pOutputBufferLength = 0;

    pReadData[0] = 0x00000000;
    pReadData[1] = 0x00000000;

    /* Max channel check */
    if ((pIoctlParams->Channel > pDeviceContext->u8NumCanBus) || (pIoctlParams->Channel == 0))
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    sHbuf = &(pDeviceContext->sCanRxHBuf.sHBuf[u8Channel]);
    if (sHbuf->hMemory != NULL)
    {
        DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slCanModeFlag);
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
            if (sHbuf->u32HbufNumEntries > 0)
            {
                if (u32NumMsgsToRead > sHbuf->u32HbufNumEntries)
                {
                    u32NumMsgsToRead = sHbuf->u32HbufNumEntries;
                }

                /* do we have to read beyond the end of the buffer (wrapped around messages) */
                if ((sHbuf->u32HbufNextRead + (u32NumMsgsToRead * CAN_RX_HBUF_MAX_MSG_SIZE)) > (U32BIT)CAN_RX_HBUF_MAX)
                {
                    u32NumMsgsToRead = ((U32BIT)CAN_RX_HBUF_MAX - sHbuf->u32HbufNextRead) / CAN_RX_HBUF_MAX_MSG_SIZE;
                }

                /* get pointer to the starting address for copying of msgs */
                pMessages = (U32BIT*)sHbuf->hMemory;

                /* calculate the number of bytes to copy */
                numBytesToCopy = u32NumMsgsToRead * CAN_RX_HBUF_MAX_MSG_SIZE * sizeof(U32BIT);
                memcpy(((U8BIT*)(&(pReadData[2]))), ((U8BIT*)(&(pMessages[sHbuf->u32HbufNextRead]))), numBytesToCopy);

                pReadData[1] = u32NumMsgsToRead;
                sHbuf->u32HbufNextRead += (u32NumMsgsToRead * CAN_RX_HBUF_MAX_MSG_SIZE);
                sHbuf->u32HbufNumEntries -= u32NumMsgsToRead;
                if (sHbuf->u32HbufNextRead >= (U32BIT)CAN_RX_HBUF_MAX)
                {
                    sHbuf->u32HbufNextRead = 0;
                }
            }
        }
        DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slCanModeFlag);
    }

    if(numBytesToCopy > 0)
    {
        /* Need to add 16 bytes to count to accommodate for  number of messages DWORD and Flags DWORD */
        *pOutputBufferLength = numBytesToCopy + CAN_RX_HEADER_BYTE_COUNT;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
       canDeleteRxHostBuffer

   Description:
        This function deletes the host buffer of an CAN Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void canDeleteRxHostBuffer
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Channel
)
{
    CAN_HBUF_TYPE *sHbuf;

    if (u8Channel < pDeviceContext->u8NumCanBus)
    {
        sHbuf = &(pDeviceContext->sCanRxHBuf.sHBuf[u8Channel]);

        if (sHbuf->hMemory != NULL)
        {
            DDC_ISR_LOCK_TAKE(sHbuf->hMutex, sHbuf->slCanModeFlag);

            DDC_KERNEL_VIRTUAL_FREE(pDeviceContext, sHbuf->hMemory);

            sHbuf->hMemory = NULL;
            sHbuf->u32HbufNextRead = 0;
            sHbuf->u32HbufNextWrite = 0;
            sHbuf->u32HbufNumEntries = 0;
            sHbuf->bHbufOverflow = 0;
            sHbuf->bHbufQueueOverflow = 0;
            sHbuf->bHbufStoreTimeTag = FALSE;

            if (sHbuf->hUsbReadBuffer != NULL)
            {
                DDC_KERNEL_FREE(pDeviceContext, sHbuf->hUsbReadBuffer);
                sHbuf->hUsbReadBuffer = NULL;
            }
            
            DDC_ISR_LOCK_GIVE(sHbuf->hMutex, sHbuf->slCanModeFlag);
        }
    }
}

/*-------------------------------------------------------------------------------
   Function:
       canHostBufferMetrics

   Description:
        This function manages the host buffer for an CAN Bus Rx channel.

   Parameters:
      In  pDeviceContext    - device-specific structure
      In  u32Channel        - channel number

   Returns:
      Error code.
   ---------------------------------------------------------------------------------*/
S16BIT canHostBufferMetrics
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    size_t *pBytesReturned,
    U32BIT* pReadData
)
{
    U8BIT u8Channel = (U8BIT)(pIoctlParams->Channel - 1);

    if (u8Channel < pDeviceContext->u8NumCanBus)
    {
        DDC_ISR_LOCK_TAKE(pDeviceContext->sCanRxHBuf.sHBuf[u8Channel].hMutex, pDeviceContext->sCanRxHBuf.sHBuf[u8Channel].slCanModeFlag);
        pReadData[0] = pDeviceContext->u32CanHbufPercentFull[u8Channel];
        pReadData[1] = pDeviceContext->u32CanHbufPercentHigh[u8Channel];
        pReadData[2] = pDeviceContext->u32CanHbufOverflowCount[u8Channel];
        DDC_ISR_LOCK_GIVE(pDeviceContext->sCanRxHBuf.sHBuf[u8Channel].hMutex, pDeviceContext->sCanRxHBuf.sHBuf[u8Channel].slCanModeFlag);

        /*ExReleaseFastMutex(&pDeviceContext->sCanRxHBuf.sHBuf[u8Channel].hMutex);*/

        DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_RX_HBUF_METRICS,
            "CAN RX Host Buf Metrics: Channel %d Full: %d, High %d, Overflows %d\n",
            u8Channel, pReadData[0], pReadData[1], pReadData[2]);

        *pBytesReturned = sizeof(U32BIT) * 3;
    }
    else
    {
        return CAN_BUS_INVALID_CHANNEL;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*-------------------------------------------------------------------------------
   Function:
       canCopyToHostBuffer

   Description:
        This function copies the shared memory to the host buffer.

   Parameters:
      In  pHostBufferMemory    - host buffer OS memory
      In  pMemory              - pointer to shared memory
      In  u32NumMessagesToRead - number of messages to copy

   Returns:
      None.
   ---------------------------------------------------------------------------------*/
static void canCopyToHostBuffer
(
    U32BIT* pHostBufferMemory,
    U32BIT* pMemory,
    U32BIT u32NumMessagesToCopy
)
{
    U32BIT i;

    DDC_DBG_PRINT(DDC_DBG_MODULE_CAN, DDC_DBG_CANBUS_COPY_HOST_BUFFER,
        "Copying %d messages to host buffer\n", u32NumMessagesToCopy);

    for (i = 0; i < (u32NumMessagesToCopy * CAN_RX_HBUF_MAX_MSG_SIZE); i++)
    {
        /* Copy CAN Bus data */
        pHostBufferMemory[i] = pMemory[i];
    }
}
