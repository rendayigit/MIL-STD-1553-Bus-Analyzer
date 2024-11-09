/*******************************************************************************
 * FILE: ddc_udl_iodev_private.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to support the I/O only device.
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

#ifndef _DDC_UDL_IODEV_PRIVATE_H_
#define _DDC_UDL_IODEV_PRIVATE_H_

#include "driver_sdk/ddc_udl_private.h"

/* IO device Registers */
#define REG_IODEV_BOARD_MODEL_NUMBER_HI             0x00
#define REG_IODEV_BOARD_MODEL_NUMBER_LO             0x01
#define REG_IODEV_DATA_ARCHIVE_NUMBER               0x02
#define REG_IODEV_FW_RELEASE_REV_NUMBER             0x03
#define REG_IODEV_FW_INTERNAL_VER_NUMBER            0x04
#define REG_IODEV_QPRM_CHANNEL_COUNT                0x05
#define REG_IODEV_DEV_CAPABILITY                    0x06
#define REG_IODEV_DEV_CAPABILITY_2                  0x07
#define REG_IODEV_AIO                               0x08
#define REG_IODEV_BOARD_CONFIG                      0x09
#define REG_IODEV_IRIGB_TX_CONTROL_1                0x0A
#define REG_IODEV_IRIGB_TX_CONTROL_2                0x0B

/* bits for IO device Registers */
#define IODEV_BOARD_CONFIG_IRIGB_TX_ENABLE          0x00000001


/* ========================================================================== */
/*                            FUNCTION PROTOTYPES                             */
/* ========================================================================== */
struct _DDC_UDL_DEVICE_CONTEXT;


extern S16BIT ioDevOpen
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

extern void ioDevBinding
(
    struct _DDC_UDL_DEVICE_CONTEXT *pDeviceContext
);

#endif /* _DDC_UDL_IODEV_PRIVATE_H_ */
