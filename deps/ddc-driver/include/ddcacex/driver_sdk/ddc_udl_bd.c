/*******************************************************************************
 * FILE: ddC_udl_bd.c
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the Global Board Module.
 *
 *  This module has a single, global board component. This component
 *  contains the following:
 *
 *      - Capabilities information
 *      - Board interrupt Mask access
 *      - Device reset access
 *      - DIO access
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
#include "include/ddc_types.h"
#include "include/ddc_device_ids.h"
#include "include/ddc_error_list.h"
#include "os/interface/ddc_udl_os_hook.h"
#include "os/interface/ddc_udl_os_util_private.h"
#include "driver_sdk/ddc_udl_private.h"
#include "driver_sdk/ddc_udl_bd_private.h"
#include "driver_sdk/ddc_udl_version_private.h"
#include "driver_sdk/ddc_udl_sdk.h"
#include "driver_sdk/debug/ddc_udl_debug_private.h"
#include "core/1553/ddc_udl_1553_private.h"


/* device capabilities */
static const U32BIT deviceCapabilities_BUQPRM =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_MTIE_MEC |
    HWVER_CAPABILITY_BC_DISABLE |
    HWVER_CAPABILITY_BC_EXT_TRIGGER |
    HWVER_CAPABILITY_RT_AUTO_BOOT |
    HWVER_CAPABILITY_TX_INHIBIT |
    HWVER_CAPABILITY_QPRM
);

static const U32BIT deviceCapabilities_BU67101Q =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN
);

static const U32BIT deviceCapabilities_BU67104C =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN
);

static const U32BIT deviceCapabilities_BU67105C =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN
);

static const U32BIT deviceCapabilities_BU67106XK =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
);

static const U32BIT deviceCapabilities_BU67107FM =
(
    HWVER_CAPABILITY_MTI
);

static const U32BIT deviceCapabilities_BU67108C_09C[] =
{
    (

        /* C0 */
        HWVER_CAPABILITY_MTI |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
    ),
    (

        /* C1 */
        HWVER_CAPABILITY_MTI |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
    ),
    (

        /* C2 */
        HWVER_CAPABILITY_MTI |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
    ),
    (

        /* C3 */
        HWVER_CAPABILITY_MTI |
        HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
    )
};

static const U32BIT deviceCapabilities_BU67110FM =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
);

static const U32BIT deviceCapabilities_BU67118M700 =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN  |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT |
    HWVER_CAPABILITY_PROG_ARINC_429     |
    HWVER_CAPABILITY_PROG_ARINC_429 |
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |    
    HWVER_CAPABILITY_QPRM               |
    HWVER_CAPABILITY_BC_DISABLE         |
    HWVER_CAPABILITY_RT_AUTO_BOOT       |
    HWVER_CAPABILITY_TX_INHIBIT
);

static const U32BIT deviceCapabilities_BU67118FM =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT |
    HWVER_CAPABILITY_PROG_ARINC_429 |
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |
    HWVER_CAPABILITY_PROG_ARINC_717 |
    HWVER_CAPABILITY_QPRM |
    HWVER_CAPABILITY_BC_DISABLE |
    HWVER_CAPABILITY_RT_AUTO_BOOT |
    HWVER_CAPABILITY_TX_INHIBIT
);

static const U32BIT deviceCapabilities_BU67206XK =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
);

static const U32BIT deviceCapabilities_BU67210FM =
(
    HWVER_CAPABILITY_MTI |
    HWVER_CAPABILITY_BC_EXT_TRIGGER |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT
);

static const U32BIT deviceCapabilities_DD40000K =
(
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT |
    HWVER_CAPABILITY_PROG_ARINC_429 |
    HWVER_CAPABILITY_PROG_ARINC_717 |
    HWVER_CAPABILITY_MSI
);

static const U32BIT deviceCapabilities_DD40001H =
(
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |
    HWVER_CAPABILITY_PROG_ARINC_429 |
    HWVER_CAPABILITY_PROG_ARINC_717 |
    HWVER_CAPABILITY_MSI
);

static const U32BIT deviceCapabilities_DD40002M =
(
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |
    HWVER_CAPABILITY_PROG_ARINC_429
);

static const U32BIT deviceCapabilities_DD40100F =
(
    HWVER_CAPABILITY_429_TESTER_OPTIONS |
    HWVER_CAPABILITY_PROG_VAR_SPEED_429 |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_IN |
    HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT |
    HWVER_CAPABILITY_PROG_ARINC_429 |
    HWVER_CAPABILITY_PROG_ARINC_717
);

/*---------------------------------------------------
    board model string
   ----------------------------------------------------*/
#define MODEL_STRING_LEN                            14
#define DD40X00X_MODEL_VARIANT_START                9
#define DD40X00BX_MODEL_VARIANT_START               10

/* Q-Prime card */
static U8BIT *u8ModelStringQPRM [] =
{
    (U8BIT *)"BU-67301B0\0"
};

/* ExpressCard */
static U8BIT * u8ModelString67101Q[] =
{
    (U8BIT *)"BU-67101Q0\0",
    (U8BIT *)"BU-67101Q1\0",
    (U8BIT *)"BU-67101Q2\0",
    (U8BIT *)"BU-67101Q3\0",
    (U8BIT *)"BU-67101Q4\0"
};

/* PCI-104+ SFP */
static U8BIT * u8ModelString67104C[] =
{
    (U8BIT *)"BU-67104C0\0",
    (U8BIT *)"BU-67104C1\0",
    (U8BIT *)"BU-67104C2\0",
    (U8BIT *)"BU-67104C3\0",
    (U8BIT *)"BU-67104C4\0"
};

/* PC-104+ SFP */
static U8BIT * u8ModelString67105C[] =
{
    (U8BIT *)"BU-67105C0\0",
    (U8BIT *)"BU-67105C1\0",
    (U8BIT *)"BU-67105C2\0",
    (U8BIT *)"BU-67105C3\0",
    (U8BIT *)"BU-67105C4\0"
};

/* PCIe SFP, BU67106 A card */
static U8BIT * u8ModelString67106K[] =
{
    (U8BIT *)"BU-67106K0\0",
    (U8BIT *)"BU-67106K1\0",
    (U8BIT *)"BU-67106K2\0",
    (U8BIT *)"BU-67106K3\0",
    (U8BIT *)"BU-67106K4\0"
};

/* PCIe SFP, BU67106 B card */
static U8BIT * u8ModelString67106BK[] =
{
    (U8BIT *)"BU-67106BK0\0",
    (U8BIT *)"BU-67106BK1\0",
    (U8BIT *)"BU-67106BK2\0",
    (U8BIT *)"BU-67106BK3\0",
    (U8BIT *)"BU-67106BK4\0"
};

/* PMC-MIO SFP 67107 */
/* the ModelString will be generated in the code */

/* PC-104+ SFP-MIO 67108 */
static U8BIT * u8ModelString67108CX[] =
{
    (U8BIT *)"BU-67108C0\0",
    (U8BIT *)"BU-67108C1\0",
    (U8BIT *)"BU-67108C2\0",
    (U8BIT *)"BU-67108C3\0",
    (U8BIT *)"BU-67108C4\0",
    (U8BIT *)"BU-67108C5\0",
    (U8BIT *)"BU-67108C6\0"
};

/* PC-104+ SFP-MIO 67109 */
static U8BIT * u8ModelString67109CX[] =
{
    (U8BIT *)"BU-67109C0\0",
    (U8BIT *)"BU-67109C1\0",
    (U8BIT *)"BU-67109C2\0",
    (U8BIT *)"BU-67109C3\0",
    (U8BIT *)"BU-67109C4\0",
    (U8BIT *)"BU-67109C5\0",
    (U8BIT *)"BU-67109C6\0"
};

/* PMC HD SFP 67110 */
/* the ModelString will be generated in the code */

/* PCIe MF, BU67206 A card */
static U8BIT * u8ModelString67206K[] =
{
    (U8BIT *)"BU-67206K0\0",
    (U8BIT *)"BU-67206K1\0",
    (U8BIT *)"BU-67206K2\0",
    (U8BIT *)"BU-67206K3\0",
    (U8BIT *)"BU-67206K4\0"
};

/* PCIe MF, BU67206 B card */
static U8BIT * u8ModelString67206BK[] =
{
    (U8BIT *)"BU-67206BK0\0",
    (U8BIT *)"BU-67206BK1\0",
    (U8BIT *)"BU-67206BK2\0",
    (U8BIT *)"BU-67206BK3\0",
    (U8BIT *)"BU-67206BK4\0"
};

/* PMC HD SFP 67210 */
/* the ModelString will be generated in the code */

/* 1 - 36 channel DD-40000K PCIe ARINC-429 Device */
static U8BIT * u8ModelStringDD40000K[] =
{
    (U8BIT *)"DD-40000Kxxx\0",
    (U8BIT *)"DD-40000K   \0"
};

static U8BIT * u8ModelStringDD40000BK[] =
{
    (U8BIT *)"DD-40000BCKxxx\0",
    (U8BIT *)"DD-40000BCK   \0"
};

/* 6 channel DD-40001H Mini PCIe ARINC-429 Device */
static U8BIT * u8ModelStringDD40001H[] =
{
    (U8BIT *)"DD-40001Hxxx\0",
    (U8BIT *)"DD-40001H   \0"
};

static U8BIT * u8ModelStringDD40002M[] =
{
    (U8BIT *)"DD-40002Mxxx\0",
    (U8BIT *)"DD-40002M\0"
};

/* 1 - 36 channel DD-40100F PMC ARINC-429 Device */
static U8BIT * u8ModelStringDD40100F[] =
{
    (U8BIT *)"DD-40100Fxxx\0",
    (U8BIT *)"DD-40100F   \0"
};

static U8BIT * u8ModelStringDD40100BF[] =
{
    (U8BIT *)"DD-40100BCFxxx\0",
    (U8BIT *)"DD-40100BCF   \0"
};

static U8BIT * u8ModelStringUnknown[] =
{
    (U8BIT *)"BU-XXXXXXX\0"
};


#define DDC_AIO_INTERRUPT_DISABLED 0x00000000


DDC_LOCAL S16BIT ddcUdlBdInitialize(struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext);


/*******************************************************************************
 * Name:    ddcUdlBdClose
 *
 * Description:
 *      close board-specific module.  This includes clearing all
 *      interrupt enable masks.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBdClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT i;

    ddcUdlBdInterruptClear(pDeviceContext, ACEX_BD_MASK_ALL);

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn = 0;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_CLOSE, "Closing Board Module\n");

    for (i = 0; i < MAX_NUM_FPGAS; i++)
    {
        if (pDeviceContext->pBoardInfo[i] != NULL)
        {
            DDC_KERNEL_FREE(pDeviceContext, pDeviceContext->pBoardInfo[i]);
            pDeviceContext->pBoardInfo[i] = NULL;
        }
    }

    pDeviceContext->eState = ACEX_CLOSED;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdInfoInitialize
 *
 * Description:
 *      allocate memory for pBoardInfo and clear the memory for future use.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBdInfoInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT i;

    for (i = 0; i < MAX_NUM_FPGAS; i++)
    {
        /* allocate memory for struct */
        pDeviceContext->pBoardInfo[i] = DDC_KERNEL_MALLOC(pDeviceContext, sizeof(ACEXPCI_BOARD_TYPE));

        if (pDeviceContext->pBoardInfo[i] == NULL)
        {
            return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
        }

        /*initialize members to zero */
        memset(pDeviceContext->pBoardInfo[i], 0, sizeof(ACEXPCI_BOARD_TYPE));
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdInitialize
 *
 * Description:
 *      Clear out bdinfo structure and load hdw access information for the board
 *      module.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
static S16BIT ddcUdlBdInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    struct _UM_INFO_TYPE *pUmInfo = pDeviceContext->pUmInfo;
    U16BIT numDevicesIndex;
    U16BIT i;
    UM_DEVICE_INFO *pUmDevicePtr;
    U8BIT u8BoardIndex = 0;
    BOOLEAN bDeviceFound = FALSE;
    U32BIT u32AioInterruptDisabledRegValue = DDC_AIO_INTERRUPT_DISABLED;
    U16BIT u16Channel;

    if (pDeviceContext->eState != ACEX_CLOSED)
    {
        return DDC_UDL_ERROR__STATE;
    }

    if (pUmInfo == NULL)
    {
        return DDC_UDL_ERROR__NULL_PTR;
    }

    /* search for Board virtual device */
    for (numDevicesIndex = 0; numDevicesIndex < pUmInfo->numDevices; numDevicesIndex++)
    {
        pUmDevicePtr = &pUmInfo->umDeviceInfo[numDevicesIndex];

        if (pUmDevicePtr->umDevType == UM_DEVICE_ID_BRD_GLOBAL)
        {
            if (pUmDevicePtr->umDevNumInstances > 1) /* there should only be 1 board instance */
            {
                return DDC_UDL_ERROR__HARDWARE_CONFIGURATION;
            }

            DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE,
                "BOARD GLOBAL NUM of Componetns: %d\n", pUmDevicePtr->umDevNumComponents);

            for (i = 0; i < pUmDevicePtr->umDevNumComponents; i++)
            {
                switch (pUmDevicePtr->umComponentInfo[i].umComponentType)
                {
                    case UM_COMPONENTS_ID_BRD_GLOBAL:
                    {
                        /* Must handle processing for 2 FPGAs via usage of pBoardInfo[u8BoardIndex] */
                        pDeviceContext->pBoardInfo[u8BoardIndex]->pu32RegSize = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->pu32RegBA = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "BOARD GLOBAL REG: BA:%08x  Size:%d\n",
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->pu32RegBA), *(pDeviceContext->pBoardInfo[u8BoardIndex]->pu32RegSize));

                        break;
                    }
                    case UM_COMPONENTS_ID_BRD_RESETS:
                    {
                        /* Must handle processing for 2 FPGAs via usage of pBoardInfo[u8BoardIndex] */
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sddcUdlBdReset.pu32RegSize = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sddcUdlBdReset.pu32RegBA = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "BOARD RESET REG: BA:%08x  Size:%d\n",
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sddcUdlBdReset.pu32RegBA), *(pDeviceContext->pBoardInfo[u8BoardIndex]->sddcUdlBdReset.pu32RegSize));

                        break;
                    }
                    case UM_COMPONENTS_ID_BRD_MEM_CONFIG:
                    {
                        /* Must handle processing for 2 FPGAs via usage of pBoardInfo[u8BoardIndex] */
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdMemConfig.pu32RegSize = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdMemConfig.pu32RegBA = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);

                        break;
                    }

                    case UM_COMPONENTS_ID_QPRM_GENERAL:
                    {
                        /* No processing needed here at the moment */
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "QPRM Gen\n");
                        break;
                    }

                    case UM_COMPONENTS_ID_QPRM_DMA:
                    {
                        pDeviceContext->sDMA.pu32TacexRegSize = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->sDMA.pu32TacexRegBA = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "QPRM DMA: BA:%08x  Size:%d\n",
                            *(pDeviceContext->sDMA.pu32TacexRegBA), *(pDeviceContext->sDMA.pu32TacexRegSize));

                        break;
                    }
                    case UM_COMPONENTS_ID_QPRM_BIST:
                    {
                        /* No processing needed here at the moment */
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "QPRM BIST\n");
                        break;
                    }

					case UM_COMPONENTS_ID_BRD_GLOBAL_THERMAL_MON:
					{
                        /* No processing needed here at the moment */
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "bdInitialize: THERMAL Detection Monitor\n");
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdThermalMon.pu32RegSize = &(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdThermalMon.pu32RegBA = (U32BIT*)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdThermalMon.u32Options = HAS_THERMAL_DETECTION_HARDWARE;

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD,
                            DDC_DBG_BD_INITIALIZE, "bdInitialize: BOARD THERMAL MON REG:  BA:%08x  Size:%d\n",
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdThermalMon.pu32RegBA),
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdThermalMon.pu32RegSize) );

                        break;
					}
                    
                    case UM_COMPONENTS_ID_BRD_GLOBAL_VOLT_MON_X8:
                    {
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32RegSize = &(pUmDevicePtr->umComponentInfo[i].umComponentRegSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32RegBA = (U32BIT *)&(pUmDevicePtr->umComponentInfo[i].umRegBaseAddr);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "BOARD VOLT MON X8 REG:  BA:%08x  Size:%d\n",
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32RegBA),
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32RegSize));

                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32MemSize = &(pUmDevicePtr->umDevMemSize);
                        pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32MemBA = (U32BIT *)&(pUmDevicePtr->umMemBaseAddr[0]);

                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "BOARD VOLT MON X8 MEM:  BA:%08x  Size:%d\n",
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32MemBA),
                            *(pDeviceContext->pBoardInfo[u8BoardIndex]->sBdVoltMonX8.pu32MemSize));

                        break;
                    }

                    default:
                    {
                        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INITIALIZE, "UNKNOWN BOARD Component:%x\n",
                            pUmDevicePtr->umComponentInfo[i].umComponentType);
                        break;
                    }
                } /* switch (pUmDevicePtr->umComponentInfo[j].umComponentType) */
            } /* for (i = 0 ... */

            u8BoardIndex = (U8BIT)(u8BoardIndex + 1);
            bDeviceFound = TRUE;

        } /* if (pUmDevicePtr->umDevType == UM_DEVICE_ID_BRD_GLOBAL) */
    } /* for (numDevicesIndex=0; numDevicesIndex<pUmInfo->numDevices; numDevicesIndex++) */

    /* init the IO Interrupt Mask values to the default */
    pDeviceContext->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;

    for (u16Channel = 0; u16Channel < pDeviceContext->u8Num1553Channels; u16Channel++)
    {
        pDeviceContext->pChannel1553[u16Channel]->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;
    }


    if (!bDeviceFound)
    {
        return DDC_UDL_ERROR__CANT_FIND_DEVICE;
    }

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdInterruptClear
 *
 * Description:
 *      This function removes the interrupt mask settings passed in the call
 *      from the existing master interrupt mask settings.  Hw is updated.
 *
 *      NOTE: to clear all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32IntMask      interrupt mask
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBdInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
)
{
    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INT_CLEAR, "Clear Mask: %08x\n", u32IntMask);

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn & ~(u32IntMask);
    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_DIS_STR, &(u32IntMask));

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdInterruptSet
 *
 * Description:
 *      This function adds the interrupt mask passed in the call
 *      to the existing master interrupt mask settings. Hw is updated.
 *
 *      NOTE: to set all interrupt masks, pass 0xFFFFFFFF in the call.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32IntMask      interrupt mask
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBdInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
)
{
    if (pDeviceContext->eState != ACEX_OPEN)
    {
        return DDC_UDL_ERROR__STATE;
    }

    DDC_ISR_LOCK_TAKE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);
    pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn | u32IntMask;
    DDC_ISR_LOCK_GIVE(pDeviceContext->semDeviceOpenEventCond, pDeviceContext->semDeviceOpenEventCondFlag);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_INT_SET, "WRITE BDINT REG:%08x VALUE:%08x\n",
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_EN, pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn);

    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_INT_EN, &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32IntEn));

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdOpen
 *
 * Description:
 *      open board-specific module.  This includes retrieving
 *      board-specific information from target hw and configuring the
 *      initial interrupt mask.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32IntMask     - interrupt enable mask for master int register.
 * Out  none
 *
 * Returns: error condition
 ******************************************************************************/
S16BIT ddcUdlBdOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
)
{
    /*U32BIT u32Data;*/
    U32BIT i;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "ENTER->\n");
    if (pDeviceContext->eState != ACEX_CLOSED)
    {
        return DDC_UDL_ERROR__STATE;
    }

    /* initialize hardware version info struct... some capabilities will be populated in other modules */
    memset(&pDeviceContext->sHwVersionInfo, 0, sizeof(pDeviceContext->sHwVersionInfo));

    /* initialize bdinfo data structure */
    ddcUdlBdInitialize(pDeviceContext);

    /* reset board component */
    ddcUdlBdReset(pDeviceContext, REG_BD_RESET_MODULE, BD_RESET_BD);

    /*------------------------------------------------------------------------*/
    /* read board information                                                 */
    /*------------------------------------------------------------------------*/

    /* read model number */
    DDC_REG_READ(pDeviceContext,
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_MODEL_NUMBER,
        &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum));

    pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum &= (BD_CAPABILITIES_REAR_CONNECTOR | BD_CAPABILITIES_MODEL_NUMBERS);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
        "pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum = %d\n",
        (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum);

    /* read capabilities */
    DDC_REG_READ(pDeviceContext,
        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CAPBILITIES,
        &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability));

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
        "pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability = 0x%08X\n",
        (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability);

    /* read device count registers */
    for (i = 0; i < ACEX_BD_NUM_DEVCOUNT_REGS; i++)
    {
        DDC_REG_READ(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_DEVICE_COUNT_0 + i,
            &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[i]));

        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
            "pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[%d] = %d\n",
            (int)i, (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[i]);
    }

    /* read flash control registers */
    if (pDeviceContext->u16DriverType != ACEX_QPRM_DRIVER)
    {
        DDC_REG_READ(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_BOOTFLASH_START_ADDR,
            &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashStartAddr));

        DDC_REG_READ(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_BOOTFLASH_CLUSTER_NUM,
            &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashNumClusters));

        DDC_REG_READ(pDeviceContext,
            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_BOOTFLASH_CLUSTER_SIZE,
            &(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashClusterSize));

        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
            "Flash Start Addr: %d  Flash Num Clusters: %d  Flash Cluster Size: %d\n",
            (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashStartAddr,
            (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashNumClusters,
            (int)pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32FlashClusterSize);
    }

    ddcUdlBdInterruptSet(pDeviceContext, u32IntMask); /* TODO: no use??, because it will work only when eState is OPEN, by BZ */

    pDeviceContext->eState = ACEX_OPEN;

    /* fill in structure for hardware version info API */
    DDC_SET_1553_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, ACEX);
    DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, ACEX);

    pDeviceContext->sHwVersionInfo.dwDriverVersion = ACEX_DRIVER_VERSION;
    pDeviceContext->sHwVersionInfo.dwHdlNumber = pDeviceContext->pUmInfo->u32DataArcNum;
    memcpy(pDeviceContext->sHwVersionInfo.szDriverVersion, ACEX_DRIVER_VERSION_STR, sizeof(ACEX_DRIVER_VERSION_STR));

    /*------------------------------------------------------------------------*/
    /* Set capability                                                         */
    /*------------------------------------------------------------------------*/
    /* reset capability */
    pDeviceContext->sHwVersionInfo.dwCapabilities = 0;

    if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "40000") != NULL)
    {
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, DDCFAMILY_429X);
        pDeviceContext->sHwVersionInfo.dwModelNumber = DD40000K_PCIE_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_DD40000K;

        switch (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum)
        {
            case 36:
            case 18:
            case 10:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_VAR_VOLT_OUT_429;

                memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40000K[0], MODEL_STRING_LEN * sizeof(U8BIT));

                if (pDeviceContext->pUmInfo->u32DataArcNum == DD40000BK_DATA_ARCHIVE)
                {
                    /* Rev B */
                    memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40000BK[0], (MODEL_STRING_LEN + 1) * sizeof(U8BIT));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START + 1] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum / 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START+2] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum % 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START+3] = '0';
                }
                else
                {
                    /* Rev A */

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum / 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 1] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum % 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 2] = '0';
                }
                break;
            }

            default:
            {
                if (pDeviceContext->pUmInfo->u32DataArcNum == DD40000BK_DATA_ARCHIVE)
                {
                    /* Rev B */
                    memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40000BK[1], (MODEL_STRING_LEN + 1) * sizeof(U8BIT));
                }
                else
                {
                    /* Rev A */
                    memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40000K[1], MODEL_STRING_LEN * sizeof(U8BIT));
                }

                break;
            }
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "40001") != NULL)
    {
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, DDCFAMILY_429X);
        pDeviceContext->sHwVersionInfo.dwModelNumber = DD40001H_PCIE_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_DD40001H;

        memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40001H[0], MODEL_STRING_LEN * sizeof(U8BIT));

        pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 0] =
               (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum/10));
        pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 1] =
                (U8BIT)('0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum%10));
        pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 2] =
                '0';

        if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_TX_INHIBIT)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_TX_INHIBIT;
        }                

    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "40002") != NULL)
    {
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, DDCFAMILY_429X);
        pDeviceContext->sHwVersionInfo.dwModelNumber = DD40002M_PMC_XMC_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_DD40002M;

#if 0 /* //DDC TODO need to find out how to determine if MSI capable */
        if (pDeviceContext->sBdInfo[ACEX_BD_INSTANCE_0].sBdReg.u32ModelNum == 16)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MSI;
        }
#endif

        switch (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum)
        {
            case 32:
            case 16:
            {

                memcpy(pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40002M[0], MODEL_STRING_LEN * sizeof(U8BIT)); 

                pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START] = (U8BIT)('0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum / 10));
                pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START+1] = (U8BIT)('0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum % 10));
                pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START+2] = '0';
                break;
            }

            default:
            {
                memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40002M[0], MODEL_STRING_LEN * sizeof(U8BIT)); 
                break;
            }
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "40100") != NULL)
    {
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, DDCFAMILY_429X);
        pDeviceContext->sHwVersionInfo.dwModelNumber = DD40100F_PCIE_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_DD40100F;

        switch (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum)
        {
            case 36:
            case 18:
            case 10:
            {
                if (pDeviceContext->pUmInfo->u32DataArcNum == DD40100BF_DATA_ARCHIVE)
                {
                    /* Rev B */
                    memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40100BF[0], (MODEL_STRING_LEN + 1) * sizeof(U8BIT));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START + 1] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum / 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START + 2] =
                       (U8BIT)( '0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum % 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00BX_MODEL_VARIANT_START + 3] = '0';
                }
                else
                {
                    /* Rev A */
                    pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_VAR_VOLT_OUT_429;

                    memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40100F[0], MODEL_STRING_LEN * sizeof(U8BIT));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START] =
                        (U8BIT)('0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum / 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 1] =
                        (U8BIT)('0' + (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum % 10));

                    pDeviceContext->sHwVersionInfo.szModelName[DD40X00X_MODEL_VARIANT_START + 2] = '0';
                }
                break;
            }

            default:
            {
                memcpy( pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringDD40100F[1], MODEL_STRING_LEN * sizeof(U8BIT));
                break;
            }
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "QPRM") != NULL)
    {
        /* TACEX (Q-Prime) card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BUQPRIME_CARD_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BUQPRM;

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelStringQPRM[0], 11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67101") != NULL)
    {
        /* Express card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67101Q_EXPRESS_CARD_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67101Q;

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelString67101Q[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
            11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67104") != NULL)
    {
        /* PC-104P card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67104C_PC104P_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67104C;

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelString67104C[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
            11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67105") != NULL)
    {
        /* PC-104P card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67105C_PC104P_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67105C;

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelString67105C[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
            11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67106") != NULL)
    {
        /* PCI-e card */
        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67106K_PCIE_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67106XK;

        if (pDeviceContext->pUmInfo->s8BrdModelNum[7] == 'B')
        {
            /* Rev B */
            pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;

            pDeviceContext->u8BoardInstanceCount = 1;
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MSI;

            memcpy(pDeviceContext->sHwVersionInfo.szModelName,
                u8ModelString67106BK[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
                11 * sizeof(U8BIT));
        }
        else
        {
            /* Rev A */
            pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_1;
            pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_1;

            if ((U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] & 0x00ff) > 2)
            {
                pDeviceContext->u8BoardInstanceCount = 2;
            }
            else
            {
                pDeviceContext->u8BoardInstanceCount = 1;
            }

            memcpy(pDeviceContext->sHwVersionInfo.szModelName,
                u8ModelString67106K[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
                11 * sizeof(U8BIT));
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67107") != NULL)
    {
        /* PMC card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67107FM_PMC_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67107FM;

        /* If front IO */
        if ((pDeviceContext->pUmInfo->u32DataArcNum == BU67107F0_PMC_SFP_DATA_ARCHIVE) ||
            (pDeviceContext->pUmInfo->u32DataArcNum == BU67107F1_PMC_SFP_DATA_ARCHIVE) ||
            (pDeviceContext->pUmInfo->u32DataArcNum == BU67107F2_PMC_SFP_DATA_ARCHIVE) ||
            (pDeviceContext->pUmInfo->u32DataArcNum == BU67107FX_PMC_SFP_DATA_ARCHIVE))
        {
            sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
                "BU-%d%c%d", BU67107FM_PMC_ACEX_MODEL_NUMBER, 'F',
                (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
        }
        else /* Rear IO */
        {
            sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
                "BU-%d%c%d", BU67107FM_PMC_ACEX_MODEL_NUMBER, 'M',
                (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67108") != NULL)
    {
        /* PC-104P card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67108C_PC104P_ACEX_MODEL_NUMBER;

        switch (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)
        {
            case 0:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[0];
                break;
            }

            case 1:
            {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[1];
                break;
            }

            case 2:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[2];
                break;
            }

            case 3:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[3];
                break;
            }

            default:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[0];
                break;
            }
        }

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelString67108CX[ (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS) ],
            11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67109") != NULL)
    {
        /* PC-104P card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67109C_PC104P_ACEX_MODEL_NUMBER;

        switch (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)
        {
            case 0:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[0];
                break;
            }

            case 1:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[1];
                break;
            }

            case 2:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[2];
                break;
            }

            case 3:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[3];
                break;
            }

            default:
            {
                pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67108C_09C[0];
                break;
            }
        }

        memcpy(pDeviceContext->sHwVersionInfo.szModelName,
            u8ModelString67109CX[ (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS) ],
            11 * sizeof(U8BIT));
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67110") != NULL)
    {
        /* PMC card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[4] = ACEX_BD_INSTANCE_1;
        pDeviceContext->sBoardInstanceIndex[5] = ACEX_BD_INSTANCE_1;
        pDeviceContext->sBoardInstanceIndex[6] = ACEX_BD_INSTANCE_1;
        pDeviceContext->sBoardInstanceIndex[7] = ACEX_BD_INSTANCE_1;

        if ((U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] & 0x00ff) > 4)
        {
            pDeviceContext->u8BoardInstanceCount = 2;
        }
        else
        {
            pDeviceContext->u8BoardInstanceCount = 1;
        }

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67110FM_PMC_HD_ACEX_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67110FM;

        sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
            "BU-%d%c%d", BU67110FM_PMC_HD_ACEX_MODEL_NUMBER,
            ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_REAR_CONNECTOR) ? 'M' : 'F'),
            (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
    }
    else if (strstr(pDeviceContext->pUmInfo->s8BrdModelNum, "67118") != NULL)
    {
        /* PMC card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        DDC_SET_429_FAMILY_NUMBER(pDeviceContext->sHwVersionInfo.dwFamilyNumber, DDCFAMILY_429X);
        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67118F_M_PMC_HD_SFP_MODEL_NUMBER;
        
        if (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67118M700;
        }
        else
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67118FM;
        }        

        if (pDeviceContext->pUmInfo->u32DataArcNum == BU67118Z310_DATA_ARCHIVE)
        {
            sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
                "BU-%s", BU67118Z310_HIGH_SPEED_SERIAL_MODEL_NUMBER);
        }
        else if ((pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118FMX) || (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118M700))
        {
            sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
                "BU-%d%c%d", BU67118F_M_PMC_HD_SFP_MODEL_NUMBER,
                ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_REAR_CONNECTOR) ? 'M' : 'F'),
                (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
        }
        else {
            sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
                "BU-%d%c%d", BU67118F_M_PMC_HD_SFP_MODEL_NUMBER,
                ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_REAR_CONNECTOR) ? 'Z' : 'Y'),
                (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
        }

        if (pDeviceContext->u32ComponentMask & UM_COMPONENTS_MASK_CAN_BUS)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_CAN;
        }

        /* If we support Can Bus add it here */
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
            "BU67118 Model Number %x, can u32ComponentMask %x\n",
            pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum, pDeviceContext->u32ComponentMask & UM_COMPONENTS_MASK_CAN_BUS);

        /* Add MSI capabilities if card is XMC */
        if (pDeviceContext->u16DeviceID == DDC_DEV_ID_BU67118YZX)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MSI;
        }

        /* all BU67118 cards will support HBuf with its serial ports */
        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_SERIAL_HBUF;

        /* Does BU67118 card support Enhanced FIFO mode */
        if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_ENHANCED_SERIAL_IO_FIFO)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_SERIAL_ENHANCED_FIFO;
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67206") != NULL)
    {
        /* MF PCI-e card */
        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67206XK_PCIE_MF_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67206XK;

        if (pDeviceContext->pUmInfo->s8BrdModelNum[7] == 'B')
        {
            /* Rev B */
            pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;

            pDeviceContext->u8BoardInstanceCount = 1;
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MSI;

            memcpy(pDeviceContext->sHwVersionInfo.szModelName,
                u8ModelString67206BK[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
                11 * sizeof(U8BIT));
        }
        else
        {
            /* Rev A */
            pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
            pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_1;
            pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_1;

            if ((U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] & 0x00ff) > 2)
            {
                pDeviceContext->u8BoardInstanceCount = 2;
            }
            else
            {
                pDeviceContext->u8BoardInstanceCount = 1;
            }

            memcpy(pDeviceContext->sHwVersionInfo.szModelName,
                u8ModelString67206K[(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS)],
                11 * sizeof(U8BIT));
        }
    }
    else if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "67210") != NULL)
    {
        /* MF PMC card */
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_1;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_1;

        if ((U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] & 0x00ff) > 2)
        {
            pDeviceContext->u8BoardInstanceCount = 2;
        }
        else
        {
            pDeviceContext->u8BoardInstanceCount = 1;
        }

        pDeviceContext->sHwVersionInfo.dwModelNumber = BU67210FM_PMC_HD_MF_MODEL_NUMBER;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= deviceCapabilities_BU67210FM;

        sprintf((char *)pDeviceContext->sHwVersionInfo.szModelName,
            "BU-%d%c%d", BU67210FM_PMC_HD_MF_MODEL_NUMBER,
            ((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_REAR_CONNECTOR) ? 'M' : 'F'),
            (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS));
    }
    else
    {
        pDeviceContext->sBoardInstanceIndex[0] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[1] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[2] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[3] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[4] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[5] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[6] = ACEX_BD_INSTANCE_0;
        pDeviceContext->sBoardInstanceIndex[7] = ACEX_BD_INSTANCE_0;
        pDeviceContext->u8BoardInstanceCount = 1;

        pDeviceContext->sHwVersionInfo.dwModelNumber = UNKNOWN_MODEL_NUMBER;
        memcpy(pDeviceContext->sHwVersionInfo.szModelName, u8ModelStringUnknown[0], 11 * sizeof(U8BIT));
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_RX_IRIG_B)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_IRIG_IN_DIGITAL;

        /* Q-PRIME card does not have Analog In */
        if (strstr((char *)pDeviceContext->pUmInfo->s8BrdModelNum, "QPRM") == NULL)
        {
            pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_IRIG_IN_ANALOG;
        }
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_TX_IRIG_B)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_IRIG_OUT_DIGITAL;
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_IRIG_RANGE)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_IRIG_RANGE;
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_TX_INHIBIT_BC_DISABLE)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_TX_INHIBIT_BC_DISABLE;
    }

    if (pDeviceContext->u32ComponentMask & UM_COMPONENTS_MASK_MIL_STD_1553_REPLAY)
    {
        /* these 2 features come as a pair */
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_REPLAY;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MTIR_AES;
    }

    if (pDeviceContext->u32ComponentMask & UM_COMPONENTS_MASK_MIL_STD_1553_MTIE)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MTI | HWVER_CAPABILITY_MTIE_MEC;
    }

    if (pDeviceContext->u32DeviceMask & UM_DEVICE_MASK_MIL_STD_1553_MF)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_MF;
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_VARIABLE_XCVR)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_VAR_VOLT_OUT;
    }

    if  (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_BUS_COUPLING)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_COUPLING;
        pDeviceContext->sHwVersionInfo.dwCapabilities |= HWVER_CAPABILITY_TERMINATION;
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_AIO_INTERRUPT)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_AIO_INTERRUPT;
    }

    if (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_DIO_INTERRUPT)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_DIO_INTERRUPT;
    }

    if  (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_ARINC_429_REPEATER_SUPPORTED)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_ARINC_429_REPEATER;
    }

    if  (pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32Capability & BD_CAPABILITIES_ARINC_429_REPEATER_DATA_POLLUTION_SUPPORTED)
    {
        pDeviceContext->sHwVersionInfo.dwCapabilities2 |= HWVER_CAPABILITY2_ARINC_429_REPEATER_DATA_POLLUTION;
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,
        " Number of FPGAS is %d\n", pDeviceContext->u8BoardInstanceCount);

    pDeviceContext->sHwVersionInfo.dwHdlVersion = pDeviceContext->pUmInfo->firmwareRelVersion;
    pDeviceContext->sHwVersionInfo.dwFwVersion = pDeviceContext->pUmInfo->firmwareIntVersion;

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,"========================================\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "Driver Version: %s\n", pDeviceContext->sHwVersionInfo.szDriverVersion);

    /*------------------------------------------------------------------------*/
    /* Set channel count                                                      */
    /*------------------------------------------------------------------------*/
    /* set enhanced capacities from 3 capability register */
    memset(&pDeviceContext->sEnhancedCapabilityInfo, 0, sizeof(pDeviceContext->sEnhancedCapabilityInfo));

    pDeviceContext->sEnhancedCapabilityInfo.modelNumber = (U8BIT)(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount1553 = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] >> 0) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount429Rx = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] >> 8) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount429Tx = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] >> 16) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount429Prog = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[0] >> 24) & 0x00ff);

    pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[1] >> 0) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[1] >> 8) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485 = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[1] >> 16) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232 = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[1] >> 24) & 0x00ff);

    pDeviceContext->sEnhancedCapabilityInfo.channelCountUart = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[2] >> 0) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount717Rx = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[2] >> 8) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount717Tx = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[2] >> 16) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.channelCount717Prog = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[2] >> 24) & 0x00ff);
    pDeviceContext->sEnhancedCapabilityInfo.u8deviceNumber = pDeviceContext->ddcOsDevInfo.u8DeviceNumber;

    /* print device capability information */
    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "\n");
    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount1553 > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " 1553 Channels................ %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount1553);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount429Rx > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " 429 Rx Channels.............. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount429Rx);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount429Tx > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " 429 Tx Channels.............. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount429Tx);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount429Prog > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " Programmable 429 Channels.... %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount429Prog);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount717Rx > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " 717 Rx Channels.............. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount717Rx);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount717Tx > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " 717 Tx Channels.............. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount717Tx);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCount717Prog > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " Programmable 717 Channels.... %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCount717Prog);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " Digital I/O.................. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " Avionic I/O.................. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232 > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " RS-232....................... %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485 > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " RS-422/485................... %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountUart > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " Programmable Serial Channels. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountUart);
    }

    if (pDeviceContext->sEnhancedCapabilityInfo.channelCountCanBus > 0)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, " CAN Channel.................. %d\n", (int)pDeviceContext->sEnhancedCapabilityInfo.channelCountCanBus);
    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "\n");
    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "Capabilities:\n");
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_MF)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Multi-Function\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_REPLAY)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Replay\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_MTI)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * MTI\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_MTIE_MEC)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * MTIe (Message Error Capture)\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_MTIR_AES)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * MTIr (Advanced Error Sampling)\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_VAR_VOLT_OUT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Variable Voltage Output\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_PROG_VAR_SPEED_429)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * 429 Variable Speed\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_VAR_VOLT_OUT_429)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * 429 Variable Voltage Output\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_COUPLING)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Coupling\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_TERMINATION)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Termination\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_EXTERNAL_CLOCK_IN)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * External Clock In\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_EXTERNAL_CLOCK_OUT)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * External Clock Out\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_IN_ANALOG)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * IRIG In (Analog)\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_IN_DIGITAL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * IRIG In (Digital)\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_OUT_DIGITAL)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * IRIG Out (Digital)\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_IRIG_RANGE)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * IRIG Range\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_TX_INHIBIT_BC_DISABLE)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * TX Inhibit / BC Disable\n");
    }

    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_PPS)
    {
        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN, "      * Pulse-Per-Second\n");
    }

    if (strstr(pDeviceContext->pUmInfo->s8BrdModelNum, "67118") != NULL)
    {

        pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485 = pDeviceContext->sEnhancedCapabilityInfo.channelCountUart;
        pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232 = pDeviceContext->sEnhancedCapabilityInfo.channelCountUart;
        pDeviceContext->sEnhancedCapabilityInfo.channelCountSyncRS232 = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[3] >> 8) & 0x00ff);
        pDeviceContext->u8NumCanBus = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[3]) & 0x00ff);

    }

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_OPEN,"========================================\n");

    /* set channel counts */
    pDeviceContext->u8Num1553Channels = pDeviceContext->sEnhancedCapabilityInfo.channelCount1553;
    pDeviceContext->u8NumProg429RxTx = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Prog;
    pDeviceContext->u8NumDed429Rx = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Rx;
    pDeviceContext->u8NumDed429Tx = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Tx;

    pDeviceContext->u8NumProg717 = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Prog;
    pDeviceContext->u8NumDed717Rx = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Rx;
    pDeviceContext->u8NumDed717Tx = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Tx;

    pDeviceContext->u8NumDiscreteIO = pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO;
    pDeviceContext->u8NumAvionicIO = pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO;

    pDeviceContext->u8NumRS485 = pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485;
    pDeviceContext->u8NumRS232 = pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232;
    pDeviceContext->u8NumUart = pDeviceContext->sEnhancedCapabilityInfo.channelCountUart;

    pDeviceContext->u32AvionicIOMask = ddcUdlBdGenerateAvionicIOMask(pDeviceContext);
    pDeviceContext->sEnhancedCapabilityInfo.channelCountCanBus = (U8BIT)((pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32DeviceCount[3]) & 0x00ff);

    pDeviceContext->bBoardLoadCompleted = TRUE;

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdReset
 *
 * Description:
 *      This function writes the reset mask to the identified reset register.
 *      module-specific resets, and one register for component-specific resets.
 *
 *      The register and bit definitions are identified in um_definitions.h
 *
 *      NOTE: to reset the entire board, write 0xffffffff to REG_BD_RESET_MODULE register.
 *
 * In   pDeviceContext  device-specific structure
 * In   u32RegOffset    Register Offset
 * In   u32ResetMask    Reset Mask - multiple bits can be or'd together
 * Out  none
 *
 * Returns: none
 ******************************************************************************/
void ddcUdlBdReset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32RegOffset,
    U32BIT u32ResetMask
)
{
    DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sddcUdlBdReset.pu32RegBA) + u32RegOffset, &(u32ResetMask));

    if ((u32RegOffset == REG_BD_RESET_MODULE) &&
        (u32ResetMask & BD_RESET_DIO_TT))
    {
        pDeviceContext->sDioTt.state = ACEX_MOD_RESET;
    }

    /* With dual FPGA in PCIe card, there is about 450ns delay for reset to happen. */
    /* In order to provent reset from happening after register writes, a time       */
    /* delay of ACEX_BD_RESET_DELAY_MS ms is added here.                            */

    ddcUdlOsWaitMs(ACEX_BD_RESET_DELAY_MS);

}

/******************************************************************************
 * Name:    ddcUdlBdClearMemory
 *
 * Description:
 *      This function is used to set a region of memory to 0.  For USB this
 *      is particularly important, since a packet must be sent filled with zero.
 *      This driver feature allows a caller to specify a device address and number
 *      of 16 bit words to set to zero.  This function uses the tx wdf memory
 *      block (max size ACEX_MAX_TFR_BUFSIZE bytes) - the block is filled with
 *      0 and sent to the device.
 *
 * In   pDev  input value for instance information associated with this particular device
 * In   u32Address16    Address (16-bit addressing)
 * In   u32NumWds       number of 16-bit words to write
 * Out  none
 *
 * Returns: error status
 *****************************************************************************/
S16BIT ddcUdlBdClearMemory
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Address16,
    U32BIT u32NumWds
)
{
    U16BIT *pBuffer;
    U32BIT u32BufByteLenReq; /* this is the length required including header */
    u32BufByteLenReq = (u32NumWds * 2) + 8; /* 8 bytes of header */

    pBuffer = DDC_KERNEL_MALLOC(pDeviceContext, u32BufByteLenReq);

    if (!pBuffer)
    {
        return DDC_UDL_ERROR__INSUFFICIENT_MEMORY;
    }

    memset(pBuffer, 0, u32BufByteLenReq);

    DDC_16BIT_BLK_MEM_WRITE(pDeviceContext, u32Address16, pBuffer, u32NumWds);

    DDC_KERNEL_FREE(pDeviceContext, pBuffer);

    return DDC_UDL_ERROR__SUCCESS;
}

/*******************************************************************************
 * Name:    ddcUdlBdReadCapabilities
 *
 * Description:
 *      This function takes the information that was in several new capabilities
 *      registers that were gotten when the driver loaded and saved in the
 *      devicecontext and packs them into the flexcore format.
 *
 * In   pDeviceContext  device-specific structure
 * Out  none
 *
 * Returns: A flexcore formatted capabilities register.
 ******************************************************************************/
U32BIT ddcUdlBdReadCapabilities
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RegisterValue;
    U32BIT u32TempValue;

    u32RegisterValue = 0;

    /* 1553 Channel count.  Flexcore uses bits 25 (MSB), 2:0(LSBs) for channel count.
     * Therefore, for SFP to be compatible to Flexcore, bit translation must be performed.  */
    u32RegisterValue |= pDeviceContext->u8Num1553Channels & 0x07;
    if (pDeviceContext->u8Num1553Channels & 0x00000008)
    {
        u32RegisterValue |= 0x02000000;
    }

    /* 429 Rx */
    u32TempValue = pDeviceContext->u8NumDed429Rx;
    u32RegisterValue |= (u32TempValue << 3);

    /* 429 Tx Channel count.  Flexcore uses bits 26 (MSB), 10:8(LSBs) for channel count.
     * Therefore, for SFP to be compatible to Flexcore, bit translation must be performed.  */
    u32TempValue = pDeviceContext->u8NumDed429Tx & 0x07;
    u32RegisterValue |= (u32TempValue << 8);
    if (pDeviceContext->u8NumDed429Tx & 0x08)
    {
        u32RegisterValue |= 0x04000000;
    }

    /* RS232 */
    u32TempValue = pDeviceContext->u8NumRS232;
    u32RegisterValue |= (u32TempValue << 11);

    /* RS422/RS485 */
    u32TempValue = pDeviceContext->u8NumRS485;
    u32RegisterValue |= (u32TempValue << 13);

    /* DIO */
    u32TempValue = pDeviceContext->u8NumDiscreteIO;
    u32RegisterValue |= (u32TempValue << 15);

    /* AIO */
    u32TempValue = pDeviceContext->u8NumAvionicIO;
    u32RegisterValue |= (u32TempValue << 20);

    /* model number */
    u32TempValue = pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdReg.u32ModelNum & BD_CAPABILITIES_MODEL_NUMBERS;
    u32RegisterValue |= (u32TempValue << 28);

    DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_READ_CAPABILITIES, "MIO_CAPABILITIES:0x%08X\n", u32RegisterValue);

    return u32RegisterValue;
}

/*-------------------------------------------------------------------------------
   Function:
       ddcUdlBdReadEnhancedCapabilities

   Description:
        This function returns device feature capability channel count.

   Parameters:
      In  None

   Returns:
      Capabilities structure.
   ---------------------------------------------------------------------------------*/
void ddcUdlBdReadEnhancedCapabilities
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PENHANCED_CAPABILITY_INFO penhancedCapabilityInfo
)
{
    memset(penhancedCapabilityInfo, 0, sizeof(ENHANCED_CAPABILITY_INFO));

    penhancedCapabilityInfo->modelNumber = pDeviceContext->sEnhancedCapabilityInfo.modelNumber;
    penhancedCapabilityInfo->channelCount1553 = pDeviceContext->sEnhancedCapabilityInfo.channelCount1553;
    penhancedCapabilityInfo->channelCount429Rx = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Rx;
    penhancedCapabilityInfo->channelCount429Tx = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Tx;
    penhancedCapabilityInfo->channelCount429Prog = pDeviceContext->sEnhancedCapabilityInfo.channelCount429Prog;
    penhancedCapabilityInfo->channelCountDIO = pDeviceContext->sEnhancedCapabilityInfo.channelCountDIO;
    penhancedCapabilityInfo->channelCountAIO = pDeviceContext->sEnhancedCapabilityInfo.channelCountAIO;
    penhancedCapabilityInfo->channelCountRS232 = pDeviceContext->sEnhancedCapabilityInfo.channelCountRS232;
    penhancedCapabilityInfo->channelCountRS485 = pDeviceContext->sEnhancedCapabilityInfo.channelCountRS485;
    penhancedCapabilityInfo->channelCountUart = pDeviceContext->sEnhancedCapabilityInfo.channelCountUart;
    penhancedCapabilityInfo->channelCountCanBus = pDeviceContext->sEnhancedCapabilityInfo.channelCountCanBus;
    penhancedCapabilityInfo->channelCount717Rx = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Rx;
    penhancedCapabilityInfo->channelCount717Tx = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Tx;
    penhancedCapabilityInfo->channelCount717Prog = pDeviceContext->sEnhancedCapabilityInfo.channelCount717Prog;
    penhancedCapabilityInfo->u8deviceNumber = pDeviceContext->sEnhancedCapabilityInfo.u8deviceNumber;
}

/*-------------------------------------------------------------------
 * Function: ddcUdlBdRtAutoBootInitialize
 *
 * Description:
 * This function initialize RT AUto Boot information if the
 * board has the capability.
 *
 * In pDeviceContext   - device-specific structure
 *
 * Out: none
 *
 * Returns: none
   --------------------------------------------------------------------*/
void ddcUdlBdRtAutoBootInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32RtGConfig = 0;
    U16BIT u16Ch;

    /* loop through all channels */
    for (u16Ch = 0; u16Ch < pDeviceContext->u8Num1553Channels; u16Ch++)
    {
        pDeviceContext->pChannel1553[u16Ch]->bRtAutoBoot = FALSE;
        pDeviceContext->pChannel1553[u16Ch]->u16RtAddr = 0;

        if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_RT_AUTO_BOOT)
        {
            /* read RT Global configuration register */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->pChannel1553[u16Ch]->sRT.pu32RegBA) + REG_MRT_GCONFIG, &u32RtGConfig);
            DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_RT_AUTO_BOOT_INIT, "ch%d RT GLOBAL Config  0x%08x\n",
                u16Ch, u32RtGConfig);

            /* save for future use*/
            pDeviceContext->pChannel1553[u16Ch]->u32RtGConfig = u32RtGConfig;

            /* check if RT Auto BOOT on enabled */
            if (u32RtGConfig & MRT_GCONFIG_MODULE_EN)
            {
                /* update RT auto boot info */
                pDeviceContext->pChannel1553[u16Ch]->bRtAutoBoot = TRUE;
                pDeviceContext->pChannel1553[u16Ch]->u16RtAddr = (U16BIT)((u32RtGConfig & MRT_GCONFIG_SRT_ADDRESS) >> MRT_GCONFIG_ADDR_SHIFT);

                DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_RT_AUTO_BOOT_INIT, "ch%d RT Auto Boot Enabled, RT addr %d\n",
                    u16Ch, pDeviceContext->pChannel1553[u16Ch]->u16RtAddr);
            }
        }
    }
}

/*-------------------------------------------------------------------
 * Function: ddcUdlBdRtAutoBootAddrRestore
 *
 * Description:
 * This function restores RT address if RT Auto BOOT is not ON but
 * SRT addr Latch is ON after a channel or RT reset.
 *
 * In pDeviceContext   - device-specific structure
 *
 * Out: none
 *
 * Returns: none
   --------------------------------------------------------------------*/
void ddcUdlBdRtAutoBootAddrRestore
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U16BIT u16Ch;
    U32BIT u32RtGConfig;

    /* loop through all channels */
    for (u16Ch = 0; u16Ch < pDeviceContext->u8Num1553Channels; u16Ch++)
    {
        /* after reset, restore RT addr if RT Auto BOOT is not ON */
        if (!pDeviceContext->pChannel1553[u16Ch]->bRtAutoBoot)
        {
            /* restore RT addr if there is a RT addr Latched */
            if ((pDeviceContext->pChannel1553[u16Ch]->u32RtGConfig & MRT_GCONFIG_SRT_ADDRESS_LATCH_TRK) &&
                (pDeviceContext->pChannel1553[u16Ch]->u32RtGConfig & (MRT_GCONFIG_SRT_ADDRESS | MRT_GCONFIG_SRT_ADDRESS_PARITY)))
            {
                u32RtGConfig = pDeviceContext->pChannel1553[u16Ch]->u32RtGConfig;
                u32RtGConfig |= (MRT_GCONFIG_SRT_ADDRESS_SOURCE | MRT_GCONFIG_LATCH_SRT_ADDRESS);

                DDC_REG_WRITE(pDeviceContext, *(pDeviceContext->pChannel1553[u16Ch]->sRT.pu32RegBA) + REG_MRT_GCONFIG, &u32RtGConfig);

                DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_RT_AUTO_BOOT_RESTORE,
                    "ch%d RT GLOBAL Config restored to 0x%08x\n",
                    u16Ch, u32RtGConfig);
            }
        }
    }
}

/******************************************************************************
 * Name:          ddcUdlBdGenerateAvionicIOMask
 *
 * Description:   Returns AIO the mask used for reading/writing AIO registers.
 *
 *                  +----------+----------+----------+----------+
 *                  |DIR15-DIR8|VAL15-VAL8| DIR7-DIR0| VAL7-VAL0|
 *                  +----------+----------+----------+----------+
 *
 * In   pDeviceContext  device-specific structure
 * Out  mask
 *****************************************************************************/
U32BIT ddcUdlBdGenerateAvionicIOMask
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
)
{
    U32BIT u32Mask_7_0 = 0;
    U32BIT u32Mask_15_8 = 0;
    U8BIT i;

    /* channels 0 to 7 */
    for (i = 0; (i < pDeviceContext->u8NumAvionicIO) && (i < 8); i++)
    {
        u32Mask_7_0 |= (0x00000101 << i);
    }

    /* channels 8 to 16 */
    for (/* use existing value */; (i < pDeviceContext->u8NumAvionicIO) && (i < 16); i++)
    {
        u32Mask_15_8 |= (0x01010000 << (i - 8));
    }

    return (u32Mask_15_8 | u32Mask_7_0);
}


/*******************************************************************************
 * Name:    ddcUdlGetBoardStatus
 *
 * Description:
 *      This function gets the board status for a particular channel.
 *
 *      Function currently supports only the Thermal Detection command:
 *      Each thermal detection register (8 total) will be formated as follows
 *      and must be decoded to return values in either Celsius or Fahrenheit.
 *      See DEVICE_TEMPERATURE_TYPE structor for more information.
 *
 *      BIT     DESCRIPTION
 *      -----------------------------------------
 *      31:13   Reserved
 *      12      Signed
 *      11:4    2's Complement value
 *      3:0     Extended bits (set as 0's)
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to IOCTL structure
 *      pIoctlParams->Param1 - will contain the command.
 *      pIoComd->Buffer - will contain the structure pertaining to the command.
 *
 * Out  depends on function.
 * Returns: none
 ******************************************************************************/
void ddcUdlGetBoardStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    S16BIT *ps16Data
)
{
    U32BIT u32RegisterAddress = 0;
    int i = 0;
    U32BIT dwData = 0;

    switch (pIoctlParams->Param1)
    {
        case DDC_BOARD_STATUS_CMD_TEMPERATURE:
        {
            /*
               Here we will retrieve all the device temps and return them in the struct.
               Returned default values are in Celcius unless the user requests Fahrenheit
                           - check options flag for Fahrenheit bit.
             */

            /* Check to see if thermal component exist */
            if (!pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdThermalMon.pu32RegBA)
            {
                memset(ps16Data, 0, sizeof(DEVICE_TEMPERATURE_TYPE));
                break;  /* Exit - thermal device not supported */
            }
            else
            {
                u32RegisterAddress = *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->sBdThermalMon.pu32RegBA);
            }

            for (i = 0; i < MAX_TEMPERATURE_DEVICE_NUMBER; i++)
            {
                DDC_REG_READ(pDeviceContext, u32RegisterAddress + i, &dwData);

                /* clear the extended bits - bit shift down by 4 */
                ps16Data[2 + i] = (S16BIT)((dwData & 0x0fff) >> 4);

                /* Check for sign bit */
                if (dwData & 0x1000)
                {
                    ps16Data[2 +(S16BIT) i]  =  (S16BIT)(ps16Data[2 +(S16BIT) i]   | 0xff00);
                }

                /* Convert to Fahrenheit if necessary */
                if (((DEVICE_TEMPERATURE_TYPE *)ps16Data)->u32_Options_Flags & DEVICE_TEMPERATURE_FAHRENHEIT)
                {
                    ps16Data[2 + i] = (S16BIT)(180 * ps16Data[2 + i] + 3200) / 100; /* Celsius to Fahrenheit conversion formula */
                }
            }

            /* Tell caller we returned all temperatures */
            ((DEVICE_TEMPERATURE_TYPE *)ps16Data)->u32_Options_Flags |= DEVICE_TEMPERATURE_ALL;
            break;
        }

        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 * Name:    ddcUdlSetBoardFeature
 *
 * Description:
 *      This function sets a board feature.
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams          pointer to IOCTL structure
 *      pIoctlParams->Param1  will contain the command
 *      pData will contain the structure pertaining to the command
 *
 * Out  depends on function
 * Returns: none
 ******************************************************************************/
VOID ddcUdlSetBoardFeature
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    void *pData
)
{
    U32BIT u32BdCfg = 0;

    switch (pIoctlParams->Param1)
    {
        case DDC_BOARD_FEATURE_CMD_EXTERNAL_CLOCK:
        {
            U32BIT u32Data = *(U32BIT *)(pData);

            /* Set the Global Ctrl bit */
            DDC_REG_READ(pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CONFIG, &(u32BdCfg));

            /* clear out existing setting */
            u32BdCfg &= ~BD_CONFIG_MASK_CLK_20_OUT_EN;

            /* set value */
            u32BdCfg |= u32Data;

            DDC_REG_WRITE( pDeviceContext, *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_CONFIG, &u32BdCfg);
            break;
        }

        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 * Name:    bdGetFeatureInfo
 *
 * Description:
 *      This function returns following feature status information:
 *           BC disable
 *           BC external trigger
 *           MRT disable
 *           RT Auto Boot
 *           TX Inhibit
 *
 * In   pDeviceContext  device-specific structure
 * In   pIoctlParams    pointer To The I/O Control Command
 * Out  IOBUffer        pointer to a feature structure
 *
 * Returns: none
 ******************************************************************************/
void bdGetFeatureInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    void *IOBuffer
)
{
    U32BIT u32RegVal;
    U32BIT u32GControl = 0;
    S16BIT status;

    U16BIT u16Ch = (U16BIT)pIoctlParams->Channel;
    PDEV_FEATURE_STATUS_INFO pDevFeatureInfo = (PDEV_FEATURE_STATUS_INFO)IOBuffer;

    /* RT Auto Boot information was set during device boot up */
    pDevFeatureInfo->bRtAutoBoot = pDeviceContext->pChannel1553[u16Ch]->bRtAutoBoot;
    pDevFeatureInfo->u16RtAddr = pDeviceContext->pChannel1553[u16Ch]->u16RtAddr;

    /* other status must be evaluated when this function is called */
    pDevFeatureInfo->bBcDisable = FALSE;
    pDevFeatureInfo->bBcExtTrigger = FALSE;
    pDevFeatureInfo->bMrtDisable = FALSE;
    pDevFeatureInfo->bTxInhibit = FALSE;

    /* check if BC is disabled */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_BC_DISABLE)
    {
        /* BC is disabled if BC register can not be accessed */
        status = DDC_REG_READ(pDeviceContext, *(pDeviceContext->pChannel1553[u16Ch]->sBC.pu32RegBA) + REG_BC_CONFIG, &u32RegVal);

        if (status != DDC_UDL_ERROR__SUCCESS)
        {
            pDevFeatureInfo->bBcDisable = TRUE;

            DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_GET_FEATURE_INFO, "ch%d BC is disabled\n", u16Ch);
        }
    }

    /* check if BC External Trigger is enabled */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_BC_EXT_TRIGGER)
    {
        /* doing nothing since there is no status information */
    }

    /* check if MRT is disabled */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_MRT_DISABLE)
    {
        /* doing nothing since there is no status information */
    }

    /* check if TX is inhibited */
    if (pDeviceContext->sHwVersionInfo.dwCapabilities & HWVER_CAPABILITY_TX_INHIBIT)
    {
        DDC_REG_READ(pDeviceContext, *(pDeviceContext->pChannel1553[u16Ch]->pu32RegBA) + REG_GENERAL_CTRL,  &u32GControl);

        DDC_DBG_PRINT(DDC_DBG_MODULE_BOARD, DDC_DBG_BD_GET_FEATURE_INFO,
            "ch%d GLOBAL Control  0x%08x\n",
            u16Ch, u32GControl);

        if ((u32GControl & GENERAL_CTRL_CHA_EXTERNAL_TX_INHIBIT) &&
            (u32GControl & GENERAL_CTRL_CHB_EXTERNAL_TX_INHIBIT))
        {
            pDevFeatureInfo->bTxInhibit = TRUE;
        }
    }
}

/******************************************************************************
 * Name:    ddcUdlBdConfigureIoInterruptConditions
 *
 * Description:
 *      Configures the AIO or DIO interrupt conditions.
 *
 * In   pDeviceContext  logical device number
 * In   Acionics        Discretes Register value
 * Out
 *
 * Returns: write status
 *****************************************************************************/
S16BIT ddcUdlBdConfigureIoInterruptConditions
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U32BIT u32Command,
    U32BIT u32RisingEdge,
    U32BIT u32FallingEdge,
    U32BIT u32Reserved,
    DDC_IOCTL_PARAMS *pIoConfigureOutput
)
{
    U32BIT u32RegData = 0x00000000;
    U32BIT u32BitData = 0x00000000;
    U32BIT u32RisingEdgeTemp = 0x00000000;
    U32BIT u32FallingEdgeTemp = 0x00000000;
    U32BIT u32AioInterruptDisabledRegValue = DDC_AIO_INTERRUPT_DISABLED;
    S16BIT status = DDC_UDL_ERROR__SUCCESS;
    U8BIT i;

    switch (u32Command)
    {
        case ACEX_AIO_INTERRUPT_CONFIGURE__ENABLE:
        case DD429_AIO_INTERRUPT_CONFIGURE__ENABLE:
        {
            U8BIT u8InUse = 0;

            /* clear out existing value */
            if (u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__ENABLE)
            {
                pDeviceContext->pChannel1553[u16Channel]->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;
            }
            else if (u32Command == DD429_AIO_INTERRUPT_CONFIGURE__ENABLE)
            {
                pDeviceContext->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;
            }

            /* check to see if we need to enable the interrupt */
            /* determine if the interrupt is in use by any entity */
            for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
            {
                if (pDeviceContext->pChannel1553[i]->u32IrqAioInterruptMask != 0x00000000)
                {
                    u8InUse = 1;
                }

                break;
            }

            if (pDeviceContext->u32IrqAioInterruptMask != 0x00000000)
            {
                u8InUse = 1;
            }

            /* if not in use, enable the interrupt */
            if (u8InUse == 0)
            {
                /* disable all AIO interrupt conditions */
                status = DDC_REG_WRITE(
                            pDeviceContext,
                            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AIO_INTERRUPT_CONTROL,
                            &u32AioInterruptDisabledRegValue);

                /* enable the AIO interrupt */
                ddcUdlBdInterruptSet(pDeviceContext, BD_INT_STATUS_MASK_AIO_INTERRUPT);
            }

            break;
        }

        case ACEX_AIO_INTERRUPT_CONFIGURE__DISABLE:
        case DD429_AIO_INTERRUPT_CONFIGURE__DISABLE:
        {
            U8BIT u8InUse = 0;

            /* clear out value */
            if (u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__DISABLE)
            {
                pDeviceContext->pChannel1553[u16Channel]->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;
            }
            else if (u32Command == DD429_AIO_INTERRUPT_CONFIGURE__DISABLE)
            {
                pDeviceContext->u32IrqAioInterruptMask = u32AioInterruptDisabledRegValue;
            }

            /* check to see if we need to disable the interrupt */
            /* determine if the interrupt is in use by any entity */
            for (i = 0; i < pDeviceContext->u8Num1553Channels; i++)
            {
                if (pDeviceContext->pChannel1553[i]->u32IrqAioInterruptMask != 0x00000000)
                {
                    u8InUse = 1;
                }

                break;
            }

            if (pDeviceContext->u32IrqAioInterruptMask != 0x00000000)
            {
                u8InUse = 1;
            }

            /* if not in use by any entity, disable the interrupt */
            if (u8InUse == 0)
            {
                /* disable the AIO interrupt */
                ddcUdlBdInterruptClear(pDeviceContext, BD_INT_STATUS_MASK_AIO_INTERRUPT);

                /* disable all AIO interrupt conditions */
                status = DDC_REG_WRITE(
                            pDeviceContext,
                            *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AIO_INTERRUPT_CONTROL,
                            &u32AioInterruptDisabledRegValue);
            }

            break;
        }

        case ACEX_AIO_INTERRUPT_CONFIGURE__SET:
        case ACEX_AIO_INTERRUPT_CONFIGURE__CLEAR:
        case DD429_AIO_INTERRUPT_CONFIGURE__SET:
        case DD429_AIO_INTERRUPT_CONFIGURE__CLEAR:
        {
            U32BIT u32AioInterruptRegValue = 0x00000000;

            /* read the current register value */
            status = DDC_REG_READ(
                        pDeviceContext,
                        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AIO_INTERRUPT_CONTROL,
                        &u32AioInterruptRegValue);

            if (status != DDC_UDL_ERROR__SUCCESS)
            {
                return status;
            }

            /* encode the rising edge values - values are stored in the odd bit indexes */

            /* first odd bit index */
            u32BitData = 0x00000002;

            for (i=0; i<MAX_NUM_AIO; i++)
            {
                if (u32RisingEdge & 0x00000001)
                {
                    u32RegData |= u32BitData;
                }

                /* shift to the next odd bit index */
                u32BitData <<= 2;

                /* shift to the next rising edge bit setting */
                u32RisingEdge >>= 1;
            }


            /* encode the falling edge values - values are stored in the even bit indexes */

            /* first even bit index */
            u32BitData = 0x00000001;

            for (i=0; i<MAX_NUM_AIO; i++)
            {
                if (u32FallingEdge & 0x00000001)
                {
                    u32RegData |= u32BitData;
                }

                /* shift to the next even bit index */
                u32BitData <<= 2;

                /* shift to the next falling edge bit setting */
                u32FallingEdge >>= 1;
            }

            if ((u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__CLEAR) || (u32Command == DD429_AIO_INTERRUPT_CONFIGURE__CLEAR))
            {
                /* in order to clear, we first need to zero out the bits that are set to 1 */

                /* this is done by inverting the bits then AND'ing it with the current register value */
                u32RegData = ~u32RegData;

                u32AioInterruptRegValue &= u32RegData;
            }
            else if ((u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__SET) || (u32Command == DD429_AIO_INTERRUPT_CONFIGURE__SET))
            {
                u32AioInterruptRegValue |= u32RegData;
            }

            /* store the mask locally - it will be used in the ISR to check which channel enabled this interrupt */
            if ((u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__SET) || (u32Command == ACEX_AIO_INTERRUPT_CONFIGURE__CLEAR))
            {
                pDeviceContext->pChannel1553[u16Channel]->u32IrqAioInterruptMask = u32AioInterruptRegValue;
            }
            else if ((u32Command == DD429_AIO_INTERRUPT_CONFIGURE__SET) || (u32Command == DD429_AIO_INTERRUPT_CONFIGURE__CLEAR))
            {
                pDeviceContext->u32IrqAioInterruptMask = u32AioInterruptRegValue;
            }

            /* write the data back */
            status = DDC_REG_WRITE(
                        pDeviceContext,
                        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AIO_INTERRUPT_CONTROL,
                        &u32AioInterruptRegValue);

            break;
        }

        case ACEX_AIO_INTERRUPT_CONFIGURE__GET:
        case DD429_AIO_INTERRUPT_CONFIGURE__GET:
        {
            U32BIT u32RegDataTemp;

            status = DDC_REG_READ(
                        pDeviceContext,
                        *(pDeviceContext->pBoardInfo[ACEX_BD_INSTANCE_0]->pu32RegBA) + REG_BD_AIO_INTERRUPT_CONTROL,
                        &u32RegData);

            if (status != DDC_UDL_ERROR__SUCCESS)
            {
                return status;
            }

            /* decode the rising edge values - values are stored in the odd bit indexes */

            /* use a scratchpad variable to manipulate the register data */
            u32RegDataTemp = u32RegData;

            /* shift to the first rising edge register value */
            u32RegDataTemp >>= 1;

            /* first rising edge value index */
            u32BitData = 0x00000001;

            for (i=0; i<MAX_NUM_AIO; i++)
            {
                if (u32RegDataTemp & 0x00000001)
                {
                    u32RisingEdgeTemp |= u32BitData;
                }

                /* shift to the next bit value index */
                u32BitData <<= 1;

                /* shift to the next rising edge bit setting */
                u32RegDataTemp >>= 2;
            }


            /* encode the falling edge values - values are stored in the even bit indexes */

            /* use a scratchpad variable to manipulate the register data */
            u32RegDataTemp = u32RegData;

            /* first falling edge bit index */
            u32BitData = 0x00000001;

            for (i=0; i<MAX_NUM_AIO; i++)
            {
                if (u32RegDataTemp & 0x00000001)
                {
                    u32FallingEdgeTemp |= u32BitData;
                }

                /* shift to the next bit value index */
                u32BitData <<= 1;

                /* shift to the next falling edge bit setting */
                u32RegDataTemp >>= 2;
            }

            if (pIoConfigureOutput)
            {
                /* Rising Edge Enable/Disable */
                pIoConfigureOutput->Param2 = u32RisingEdgeTemp;

                /* Falling Edge Enable/Disable */
                pIoConfigureOutput->Param3 = u32FallingEdgeTemp;
            }

            break;
        }
    }

    return status;
}
