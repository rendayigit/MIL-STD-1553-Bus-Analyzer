/*******************************************************************************
 * FILE: ddc_udl_irigb_private.h
 *
 * DESCRIPTION:
 *
 *  TODO
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

#ifndef _DDC_UDL_IRIGB_PRIVATE_H_
#define _DDC_UDL_IRIGB_PRIVATE_H_


#define IRIG_B_TX_ENABLE                    0x80000000
#define IRIG_B_TX_DAYS                      0x03FE0000
#define IRIG_B_TX_HOURS                     0x0001F000
#define IRIG_B_TX_MINUTES                   0x00000FC0
#define IRIG_B_TX_SECONDS                   0x0000003F
#define IRIG_B_TX_YEAR                      0x01FC0000
#define IRIG_B_TX_CONTROL                   0x0001FFFF

#define ACEX_GET_IRIG_TX_SUPPORTED(data)    ((data & BD_CAPABILITIES_TX_IRIG_B) >> 3)
#define ACEX_GET_IRIG_TX_ENABLE(data)       ((data & IRIG_B_TX_ENABLE) >> 31)
#define ACEX_GET_IRIG_TX_DAYS(data)         ((data & IRIG_B_TX_DAYS) >> 17)
#define ACEX_GET_IRIG_TX_HOURS(data)        ((data & IRIG_B_TX_HOURS) >> 12)
#define ACEX_GET_IRIG_TX_MINUTES(data)      ((data & IRIG_B_TX_MINUTES) >> 6)
#define ACEX_GET_IRIG_TX_SECONDS(data)      (data & IRIG_B_TX_SECONDS)
#define ACEX_GET_IRIG_TX_YEAR(data)         ((data & IRIG_B_TX_YEAR) >> 18)
#define ACEX_GET_IRIG_TX_CONTROL(data)      (data & IRIG_B_TX_CONTROL)

#define ACEX_SET_IRIG_TX_CONTROL_REG_2(year, control) \
        ((((U32BIT)year << 18) & IRIG_B_TX_YEAR) | ((U32BIT)control & IRIG_B_TX_CONTROL))

#define ACEX_SET_IRIG_TX_CONTROL_REG_1(enable, days, hours, minutes, seconds) \
        ((((U32BIT)enable << 31) & IRIG_B_TX_ENABLE) | \
        (((U32BIT)days << 17) & IRIG_B_TX_DAYS) | \
        (((U32BIT)hours << 12) & IRIG_B_TX_HOURS) | \
        (((U32BIT)minutes << 6) & IRIG_B_TX_MINUTES) | \
        ((U32BIT)seconds & IRIG_B_TX_SECONDS))


typedef struct _IRIG_TX
{
    U16BIT u16IRIGBTxSupported;
    U16BIT u16Enable;
    U16BIT u16Seconds;
    U16BIT u16Minutes;
    U16BIT u16Hours;
    U16BIT u16Days;
    U16BIT u16Year;
    U32BIT u32Control;

} IRIG_TX;


typedef struct _ACEX_1553_IRIGB_TYPE
{
    U32BIT *pu32RegBA;     /* ptr to 1553 IRIGB Registers base address */
    U32BIT *pu32RegSize;   /* ptr to 1553 IRIGB Register size          */

} ACEX_1553_IRIGB_TYPE;

/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;
struct _DDC_IOCTL_PARAMS;


extern S16BIT irigbOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern S16BIT irigInterruptSet
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    struct _DDC_IOCTL_PARAMS *pIoctlParams
);

extern S16BIT irigbSetIRIGTx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    IRIG_TX *pIRIGTx
);

extern S16BIT irigbGetIRIGTx
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext,
    IRIG_TX *pIRIGTx
);

#endif /* _DDC_UDL_IRIGB_PRIVATE_H_ */
