/*******************************************************************************
 * FILE: deviceop.h
 *
 * DESCRIPTION:
 *
 *   This is the include file for deviceop.c
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
 * Copyright (c) 2018 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __DEVICEOP_H__
#define __DEVICEOP_H__


#include <unistd.h>
#include "include/ddc_types.h"

#define PCI_MEM_WIN_LEN 0x20000 /* EMACE PCI Boards Channel RAM window size (128Kb) */
#define PCI_REG_WIN_LEN 0x4000  /* EMACE PCI Boards REG window size (16Kb) */

#define USB_MAX_CARD            8
#define USB_1553_CHANNELS       2


S16BIT _DECL _dvDriverResourceAllocation
(
	S16BIT s16DevNum
);

S16BIT _DECL _dvDriverResourceRelease
(
	S16BIT s16DevNum
);
S16BIT _DECL _dvLinuxOpenDDCDriver
(
	S16BIT s16DevNum
);

S16BIT _DECL _dvLinuxCloseDDCDriver
(
	S16BIT s16DevNum
);

S16BIT _DECL _dvLinuxInstallIrq
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvLinuxUninstallIrq
(
    S16BIT s16DevNum
);

extern S16BIT _dvIODriverResourceAllocation
(
    S16BIT s16DevNum
);

extern S16BIT _dvIODriverResourceRelease
(
    S16BIT s16DevNum
);

extern VOID _dvInitializeCriticalSection
(
    S16BIT s16DevNum
);

extern VOID _DECL _dvDeleteCriticalSection
(
	S16BIT s16DevNum
);

extern VOID _DECL _dvEnterCriticalSection
(
	S16BIT s16DevNum
);

extern VOID _DECL _dvLeaveCriticalSection
(
	S16BIT s16DevNum
);

void _dvIrqDispatcher
(
    S16BIT *pwDevNum
);

void _dvIrqWorker
(
    S16BIT *pwDevNum
);

void _DECL _dvSystemIsr
(
    S16BIT wDevNum,
    U32BIT dwStatus
);

S16BIT _DECL _osFreeIO
(
	S16BIT s16DevNum
);

S16BIT _DECL _osInitIO
(
	S16BIT s16DevNum,
	S16BIT wOptions
);


#endif /* __DEVICEOP_H_ */
