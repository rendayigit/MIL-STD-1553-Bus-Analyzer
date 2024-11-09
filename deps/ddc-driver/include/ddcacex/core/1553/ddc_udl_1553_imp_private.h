/*******************************************************************************
 * FILE: ddc_udl_1553_imp_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support
 *  configuration/management of the 1553 improvements block.
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

#ifndef _DDC_UDL_1553_IMP_PRIVATE_H_
#define _DDC_UDL_1553_IMP_PRIVATE_H_

#include "core/1553/ddc_udl_1553_common_private.h"

/*----------------------------------------------------------------------------*/
/*                                IMP DEFINES                                 */
/*----------------------------------------------------------------------------*/

#define IMP_MRT_CMD_MODE_CTRL_MASK      0x02000000
#define IMP_MRT_DATA_MODE_CTRL_MASK     0x03000000
#define IMP_BC_CMD_MODE_CTRL_MASK       0x04000000
#define IMP_BC_DATA_MODE_CTRL_MASK      0x05000000

#define IMP_PACK_DATA                   0x08000000
#define IMP_DONT_PACK_DATA              0x0

#define IMP_FLUSH_ENABLE                0x08000000
#define IMP_FLUSH_DISABLE               0x0

#define IMP_16_BIT_CMD_ENABLE           0x04000000
#define IMP_16_BIT_CMD_DISABLE          0x0

/*----------------------------------------------------------------------------*/
/*                              IMP DATA TYPES                                */
/*----------------------------------------------------------------------------*/

typedef struct _ACEX_1553_IMP_STATS_TYPE
{
    U32BIT u32NumOutFifoFull;       /* number of times output FIFO reported full    */
    U32BIT u32OutFifoStkOverflw;    /* number of times stk overflow reported        */
    U32BIT u32OutFifoEomNotSet;

} ACEX_1553_IMP_STATS_TYPE;


typedef struct _IMP_REG_TYPE
{
    U32BIT u32Channel;
    U32BIT u32IntMaskEn;                /* interrupt mask enable 02H WR */
    U32BIT u32IntNumWds;
    U32BIT u32IntNumQTfr;
    U32BIT u32IntBlkTime;
    U32BIT u32IntMsgTime;
    U32BIT u32TgtMemBA;                 /* in 16bit                     */
    U32BIT u32TgtMemSzDWD;
    U32BIT u32IgnoreDataTfr;
    U32BIT u32ImpBlkTrigHostInitInt;
    U32BIT u32IntStatus;
    U32BIT u32OutFifoStatus;
    U32BIT u32InFifoStatus;
    U32BIT u32RtCmdStkPtrLR;
    U32BIT u32RtCmdStkDWL;
    U32BIT u32RtGblDBufPtrLR;
    U32BIT u32MtCmdStkPtrLR;
    U32BIT u32MtCmdStkDWL;

} IMP_REG_TYPE;


struct _ACEX_1553_IMP_TYPE
{
    U32BIT u32ComponentType;   /* 1553 IMP component type                   */
    U32BIT *pu32RegBA;         /* ptr to 1553 IMP Registers base address    */
    U32BIT *pu32RegSize;       /* ptr to 1553 IMP Register size             */

    /* host id in descriptor will correspond to array index */

    U32BIT u32BdIntMask;       /* bd component interrupt mask for channel   */
    U32BIT u321553ChIntMask;   /* 1553 Ch Component interrupt mask          */

    IMP_REG_TYPE sImpCfgReg;
    ACEX_MOD_STATE state;
    ACEX_1553_IMP_STATS_TYPE stats;
};


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;

extern S16BIT impInitialize
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void impPostQueue
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U16BIT u16Ch,
    U32BIT u32InFifoHi,
    U32BIT u32InFifoLo
);

extern void impRtInterruptHandler
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern S16BIT impRtOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void impRtClose
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    U8BIT u8Ch
);

extern void impSyncRtCmdStkPtr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _ACEX_1553_CHANNEL_TYPE *pCh,
    U32BIT u32RtCmdStkPtr
);

extern void impSetRtCmdStkPtr
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _ACEX_1553_CHANNEL_TYPE *pCh,
    U32BIT u32CurrentCmdStkPtr
);

#endif /* _DDC_UDL_1553_IMP_PRIVATE_H_ */
