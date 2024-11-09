/*******************************************************************************
 * FILE: devicex.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide ACEX driver access for the RTL.
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

#ifndef __DEVICEX_H__
#define __DEVICEX_H__

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
S16BIT _DECL _dvxGetApiType
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxDriverResourceAllocation
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxDriverResourceRelease
(
    S16BIT s16DevNum
);

S16BIT _dvxLinuxInstallIrq(S16BIT s16DevNum);

S16BIT _dvxLinuxUninstallIrq(S16BIT s16DevNum);

void _DECL _dvxIrqDispatcher(S16BIT *pwDevNum);

void _DECL _dvxIrqWorker(S16BIT *pwDevNum);

S16BIT _DECL _dvxIODriverResourceAllocation
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxIODriverResourceRelease
(
    S16BIT s16DevNum
);

S16BIT _DECL _dvxSetIRQCond
(
    S16BIT s16DevNum,
    U16BIT u16Enable,
    U32BIT dwIrqMask
);

S16BIT _DECL _dvxBcReplayThreadCreate
(
    S16BIT s16DevNum
);

void _DECL _dvxBcReplayIrqRelease
(
    S16BIT s16DevNum
);

void _DECL _dvxBcReplayDispatcher
(
    S16BIT *pwDevNum
);

#endif /* __DEVICEX_H__ */
