/*******************************************************************************
 * FILE: ddc_os_ioctl.h
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

#ifndef _DDC_OS_IOCTL_H_
#define _DDC_OS_IOCTL_H_

/* ========================================================================== */
/* IOCTL Definitions                                                          */
/* ========================================================================== */

#define METHOD_BUFFERED         0
#define METHOD_IN_DIRECT        0
#define METHOD_OUT_DIRECT       0
#define METHOD_NEITHER          0

#define DDC_IOCTL_OFFSET	0

#define DDC_IOCTL__GROUP_MASK       0x0780 /* bits 15 - 7 */
#define DDC_IOCTL__GROUP_OFFSET     7
#define DDC_IOCTL__GROUP_SET_VAL(x) ((x << DDC_IOCTL__GROUP_OFFSET) & DDC_IOCTL__GROUP_MASK)

#define DDC_IOCTL__TYPE_MASK        0x007F /* bits 6 - 0 */
#define DDC_IOCTL__TYPE_OFFSET      0
#define DDC_IOCTL__TYPE_SET_VAL(x)      ((x << DDC_IOCTL__TYPE_OFFSET) & DDC_IOCTL__TYPE_MASK)

/* used for driver debugging */
#define DDC_IOCTL__GROUP_GET_VAL(ioctlVal) ((ioctlVal & DDC_IOCTL__GROUP_MASK) >> DDC_IOCTL__GROUP_OFFSET)
#define DDC_IOCTL__TYPE_GET_VAL(ioctlVal) ((ioctlVal & DDC_IOCTL__TYPE_MASK) >> DDC_IOCTL__TYPE_OFFSET)


/* NOTE: ioctl_method parameter is not used */
#define DDC_IOCTL(ioctl_group, ioctl_type, ioctl_method) \
    (DDC_IOCTL__GROUP_SET_VAL(ioctl_group) + DDC_IOCTL__TYPE_SET_VAL(ioctl_type))


#endif /* _DDC_OS_IOCTL_H_ */
