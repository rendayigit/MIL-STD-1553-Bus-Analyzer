/*******************************************************************************
 * FILE: ddc_udl_serial_io.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to provide functions to support serialIO
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

#ifndef _DDC_UDL_SERIAL_IO_H_
#define _DDC_UDL_SERIAL_IO_H_

#include "driver_sdk/ddc_udl_sdk.h"

#define NUM_CAST_SIO_CHANNELS           64

#define CAST_NO_SERIAL_CAP              0x00
#define CAST_UART_ONLY                  0x01
#define CAST_SDLC_ONLY                  0x02
#define CAST_SDCL_UART                  0x03
#define CAST_HDLC_ONLY                  0x04
#define CAST_HDLC_UART                  0x05
#define CAST_HDLC_SDLC                  0x06
#define CAST_HDLC_SDLC_UART             0x07

#endif /* _DDC_UDL_SERIAL_IO_H_ */
