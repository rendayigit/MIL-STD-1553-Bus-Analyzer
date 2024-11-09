/*******************************************************************************
 * FILE: ddc_udl_diott.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide function definitions of the
 *  DIO Time Tag module interface that is common between RTL and Driver.
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

#ifndef _DDC_UDL_DIOTT_H_
#define _DDC_UDL_DIOTT_H_

/* Time tag clock source options */
#define TT_RO_16_BIT    0x00000000
#define TT_RO_17_BIT    0x00000010
#define TT_RO_18_BIT    0x00000020
#define TT_RO_19_BIT    0x00000030
#define TT_RO_20_BIT    0x00000040
#define TT_RO_21_BIT    0x00000050
#define TT_RO_22_BIT    0x00000060
#define TT_RO_48_BIT    0x00000070
#define TT_RESO_64US    0x00000000
#define TT_RESO_32US    0x00000001
#define TT_RESO_16US    0x00000002
#define TT_RESO_08US    0x00000003
#define TT_RESO_04US    0x00000004
#define TT_RESO_02US    0x00000005
#define TT_RESO_01US    0x00000006
#define TT_RESO_500NS   0x00000007
#define TT_RESO_100NS   0x00000008
#define TT_TST_CLK      0x00000009
#define TT_EXT_CLK      0x0000000A
#define TT_IRIGB        0x00002000

#define TT_CFG_MSK      TT_RO_16_BIT  | \
                        TT_RO_17_BIT  | \
                        TT_RO_18_BIT  | \
                        TT_RO_19_BIT  | \
                        TT_RO_20_BIT  | \
                        TT_RO_21_BIT  | \
                        TT_RO_22_BIT  | \
                        TT_RO_48_BIT  | \
                        TT_RESO_64US  | \
                        TT_RESO_32US  | \
                        TT_RESO_16US  | \
                        TT_RESO_08US  | \
                        TT_RESO_04US  | \
                        TT_RESO_02US  | \
                        TT_RESO_01US  | \
                        TT_RESO_500NS | \
                        TT_RESO_100NS | \
                        TT_TST_CLK    | \
                        TT_EXT_CLK    | \
                        TT_IRIGB

/* Time tag interrupt conditions */
#define TT_INT_BUF_OVFL 0x00000040
#define TT_INT_ENT_CNT  0x00000020
#define TT_INT_BUF_100  0x00000010
#define TT_INT_BUF_75   0x00000008
#define TT_INT_BUF_50   0x00000004
#define TT_INT_BUF_25   0x00000002
#define TT_INT_RO       0x00000001

#define TT_INT_MSK      TT_INT_BUF_OVFL | \
                        TT_INT_ENT_CNT  | \
                        TT_INT_BUF_100  | \
                        TT_INT_BUF_75   | \
                        TT_INT_BUF_50   | \
                        TT_INT_BUF_25   | \
                        TT_INT_RO

/* Time tag configuration structure */
typedef struct _DIO_TT_CFG
{
    U32BIT u32Dio;      /* Discrete input signals to enable.  D15-8 => falling edge.  D7-0 => rising edge */
    U32BIT u32TtCfg;    /* Time tag clock source options */
    U32BIT u32IntMsk;   /* Interrupt conditions */
    U32BIT u32EntCnt;   /* Threshold for entry count interrupt */

} DIO_TT_CFG;


#define TT_CTL_STOP   0x00000008
#define TT_CTL_START  0x00000004
#define TT_CTL_RESET  0x00000001

#define TT_CTL_MSK    TT_CTL_START | \
                      TT_CTL_STOP  | \
                      TT_CTL_RESET

                      
#endif /* _DDC_UDL_DIOTT_H_ */
