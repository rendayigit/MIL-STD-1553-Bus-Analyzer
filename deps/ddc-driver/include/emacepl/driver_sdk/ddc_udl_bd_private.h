/*******************************************************************************
 * FILE: ddc_udl_bd_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to support the global board component.
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

#ifndef _DDC_UDL_BD_PRIVATE_H_
#define _DDC_UDL_BD_PRIVATE_H_

#include "os/include/ddc_os_types.h"
#include "driver_sdk/ddc_udl_private.h"
#include "include/ddc_ioctl.h"

#define MAX_NUM_AIO                         16


#define ACEX_BD_INSTANCE_0                  0
#define ACEX_BD_INSTANCE_1                  1
#define ACEX_BD_INSTANCE_2                  2
#define ACEX_BD_INSTANCE_3                  3
#define ACEX_BD_INSTANCE_4                  4
#define ACEX_BD_INSTANCE_5                  5
#define ACEX_BD_INSTANCE_6                  6
#define ACEX_BD_INSTANCE_7                  7

#define ACEX_BD_RESET_DELAY_MS              1 /* ms */
#define ACEX_BD_NUM_DEVCOUNT_REGS           8  

typedef struct _ACEXPCI_BRD_GLOBAL_REG_TYPE
{
    U32BIT u32ModelNum;
    U32BIT u32Capability;
    U32BIT u32DeviceCount[ACEX_BD_NUM_DEVCOUNT_REGS];
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

} ACEXPCI_BRD_GLOBAL_REG;


typedef struct _ACEXPCI_BRD_RESET_TYPE
{
    U32BIT   *pu32RegBA;            /* ptr to Reset Registers base address      */
    U32BIT   *pu32RegSize;          /* ptr to Reset Register size               */

} ACEXPCI_BRD_RESET;


typedef struct _ACEXPCI_BRD_MEM_CONFIG_TYPE
{
    U32BIT   *pu32RegBA;            /* ptr to Reset Registers base address      */
    U32BIT   *pu32RegSize;          /* ptr to Reset Register size               */

} ACEXPCI_BRD_MEM_CONFIG;




typedef struct _BRD_VOLT_MON_X8
{
    U32BIT *pu32RegBA;              /* ptr to Reset Registers base address */
    U32BIT *pu32RegSize;            /* ptr to Reset Register size */

    U32BIT *pu32MemSize;            /* ptr to UM memory size */
    U32BIT *pu32MemBA;              /* ptr to UM memory base address */

} BRD_VOLT_MON_X8;



/*------------------------------------------------------
     Thermal Detection Component
-------------------------------------------------------*/
#define HAS_THERMAL_DETECTION_HARDWARE       0x00000001

typedef struct _BRD_THERMAL_MON
{
    U32BIT *pu32RegBA;              /* ptr to DMA Registers base address */
    U32BIT *pu32RegSize;            /* ptr to DMA Register size */
    U32BIT u32Options;

} BRD_THERMAL_MON;


typedef struct _ACEXPCI_BOARD_TYPE
{
    ACEXPCI_BRD_MEM_CONFIG sBdMemConfig;
    U32BIT *pu32RegBA;              /* ptr to 1553 BC Registers base address    */
    U32BIT *pu32RegSize;            /* ptr to 1553 Global Register size         */
    ACEXPCI_BRD_GLOBAL_REG sBdReg;
    ACEXPCI_BRD_RESET sddcUdlBdReset;
    BRD_VOLT_MON_X8 sBdVoltMonX8;
    BRD_THERMAL_MON sBdThermalMon;

} ACEXPCI_BOARD_TYPE;


/*-----------------------------------------------------------
      Single-Function cards
-------------------------------------------------------------*/
#define BU67101Q_EXPRESS_CARD_ACEX_MODEL_NUMBER     67101   /* ExpressCard */
#define BU67104C_PC104P_ACEX_MODEL_NUMBER           67104   /* PCI-104+ SFP */
#define BU67105C_PC104P_ACEX_MODEL_NUMBER           67105   /* PC-104+ SFP */
#define BU67106K_PCIE_ACEX_MODEL_NUMBER             67106   /* PCIe SFP */
#define BU67107FM_PMC_ACEX_MODEL_NUMBER             67107   /* PMC-MIO SFP 67107 */
#define BU67108C_PC104P_ACEX_MODEL_NUMBER           67108   /* PC-104+ SFP-MIO - 67108 */
#define BU67109C_PC104P_ACEX_MODEL_NUMBER           67109   /* PC-104+ SFP-MIO - 67109 */
#define BU67110FM_PMC_HD_ACEX_MODEL_NUMBER          67110   /* PMC HD SFP */
#define BU67118F_M_PMC_HD_SFP_MODEL_NUMBER          67118   /* PMC HD SFP */
#define BU67118Z310_HIGH_SPEED_SERIAL_MODEL_NUMBER  "67118Z310"   /* PMC HD SFP */

/*-----------------------------------------------------------
      Multi-Function cards
-------------------------------------------------------------*/
#define BU67206XK_PCIE_MF_MODEL_NUMBER              67206   /* PCIe MF */
#define BU67210FM_PMC_HD_MF_MODEL_NUMBER            67210   /* PMC HD MF */

/*-----------------------------------------------------------
      Q-PRM card
-------------------------------------------------------------*/
#define BUQPRIME_CARD_ACEX_MODEL_NUMBER             67301

/*-----------------------------------------------------------
      429 test card
-------------------------------------------------------------*/
#define DD40000K_PCIE_MODEL_NUMBER                  40000   /* 1 - 36 channel DD-40000K PCIe ARINC-429 Device */
#define DD40001H_PCIE_MODEL_NUMBER                  40001   /* 6 channel DD-40001K Mini PCIe ARINC-429 Device */
#define DD40002M_PMC_XMC_MODEL_NUMBER               40002   /* DD-40002M PMC/XMC ARINC-429 Device */
#define DD40100F_PCIE_MODEL_NUMBER                  40100   /* 1 - 36 channel DD-40100F PMC ARINC-429 Device */

/*-----------------------------------------------------------
      unknown cards
-------------------------------------------------------------*/
#define UNKNOWN_MODEL_NUMBER                        00000   /* unknown card */

/*-----------------------------------------------------------
      DATA ARCHIVE number
-------------------------------------------------------------*/

/* PMC-MIO SFP 67107F - Front IO */
#define BU67107F0_PMC_SFP_DATA_ARCHIVE              76780
#define BU67107F1_PMC_SFP_DATA_ARCHIVE              76895
#define BU67107F2_PMC_SFP_DATA_ARCHIVE              76896
#define BU67107FX_PMC_SFP_DATA_ARCHIVE              76779

/* PMC-MIO SFP 67107M - Rear IO */
#define BU67107M0_PMC_SFP_DATA_ARCHIVE              76777
#define BU67107M1_PMC_SFP_DATA_ARCHIVE              76897
#define BU67107M2_PMC_SFP_DATA_ARCHIVE              76898
#define BU67107MX_PMC_SFP_DATA_ARCHIVE              76776

/* 429 MF DD-40x00x Cards */
#define DD40000BK_DATA_ARCHIVE                      79669
#define DD40100BF_DATA_ARCHIVE                      79668

/* MIO MF BU-67118Z310 High Speed Serial */
#define BU67118Z310_DATA_ARCHIVE                    83327

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
extern S16BIT ddcUdlBdInfoInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlBdOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
);

extern S16BIT ddcUdlBdClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void bdInterruptTest
(
    void
);

extern void ddcUdlBdReset
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32RegOffset,
    U32BIT u32ResetMask
);

extern S16BIT ddcUdlBdInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
);

extern S16BIT ddcUdlBdInterruptClear
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32IntMask
);

extern void bdFlashInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT ddcUdlBdClearMemory
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U32BIT u32Address16,
    U32BIT u32NumWds
);

extern U32BIT ddcUdlBdReadCapabilities
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlBdRtAutoBootInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlBdRtAutoBootAddrRestore
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlBdReadEnhancedCapabilities
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    PENHANCED_CAPABILITY_INFO penhancedCapabilityInfo
);

extern void bdGetFeatureInfo
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pCmdInfo,
    void *IOBuffer
);

extern U32BIT ddcUdlBdGenerateAvionicIOMask
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ddcUdlGetBoardStatus
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    S16BIT *ps16Data
);

extern void ddcUdlSetBoardFeature
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    DDC_IOCTL_PARAMS *pIoctlParams,
    void *pdata
);

extern S16BIT ddcUdlBdConfigureIoInterruptConditions
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Channel,
    U32BIT u32Command,
    U32BIT u32RisingEdge,
    U32BIT u32FallingEdge,
    U32BIT u32Reserved,
    DDC_IOCTL_PARAMS *pIoConfigureOutput
);

#endif /* _DDC_UDL_BD_PRIVATE_H_ */
