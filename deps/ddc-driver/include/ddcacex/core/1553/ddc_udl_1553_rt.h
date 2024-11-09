/*******************************************************************************
 * FILE: ddc_udl_1553_rt.h
 *
 * DESCRIPTION:
 *
 *  Public RT definitions
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

#ifndef _DDC_UDL_1553_RT_H_
#define _DDC_UDL_1553_RT_H_

/* These are general definitions for use at the library level, that can be          */
/* interpreted by the driver and translated in the the appropriate register access  */
/* wOptions parameter can be any of the following bit "OR'd" together               */

#define ACE_RT_OPT_CLR_SREQ             0x00000001  /* Clear sreq after tx vector wrd   */
#define ACE_RT_OPT_LOAD_TT              0x00000002  /* Load Time Tag on sync MCODE      */
#define ACE_RT_OPT_CLEAR_TT             0x00000004  /* Clear Time Tag on sync MCODE     */
#define ACE_RT_OPT_OVR_DATA             0x00000008  /* Overwrite inv data -circ buf     */
#define ACE_RT_OPT_OVR_MBIT             0x00000010  /* T/R*=0,MC MSB=0, ACE will resp   */
#define ACE_RT_OPT_ALT_STS              0x00000020  /* Use RT alternate status word     */
#define ACE_RT_OPT_IL_RX_D              0x00000040  /* Illegal receive disable          */
#define ACE_RT_OPT_BSY_RX_D             0x00000080  /* Busy receive disable             */
#define ACE_RT_OPT_SET_RTFG             0x00000100  /* set flag if loopback tsts fail   */
#define ACE_RT_OPT_1553A_MC             0x00000200  /* 1553a mode codes enabled         */
#define ACE_RT_OPT_MC_O_BSY             0x00000400  /* Busy bit and data word sent      */
#define ACE_RT_OPT_BCST_DIS             0x00000800  /* Broadcast disabled               */

/* the following REMOTE TERMINAL OPTIONS ARE NEW FOR ACEX */
#define ACE_RT_OPT_ILL_DIS              0x00001000  /* illegalization disabled          */
#define ACE_RT_OPT_1553A_ERESP_EN       0x00002000  /* 1553A Error Response enabled     */
#define ACE_RT_OPT_INHIBIT_BTWD_IF_BUSY 0x00004000  /* Inhibit BITWD if Busy            */
#define ACE_RT_OPT_EXTERN_BTWD_IF_BUSY  0x00008000  /* Extern BITWD if Busy             */

#define ACE_RT_OPTIONS_MAX              0x00008000

/* RT Address Sources for Single-RT compatibility mode */
#define ACE_RT_INTERNAL_ADDR            0x0001
#define ACE_RT_EXTERNAL_ADDR            0x0000

#define ACEX_MRT_MODE                   0x0001      /* Multi-RT Mode                                    */
#define ACEX_SRT_MODE                   0x0000      /* Single RT mode for EXTERNAL SRT compatibility    */

#endif /* _DDC_UDL_1553_RT_H_ */
