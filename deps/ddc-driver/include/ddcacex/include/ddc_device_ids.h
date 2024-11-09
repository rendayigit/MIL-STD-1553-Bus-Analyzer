/*******************************************************************************
 * FILE: ddc_device_ids.h
 *
 * DESCRIPTION:
 *
 *  This file contains Vendor ID, Device ID's, Product ID's (PID), and 
 *  related items.
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

#ifndef _DDC_DEVICE_IDS_H_
#define _DDC_DEVICE_IDS_H_

#define DDC_VENDOR_ID                   0x4DDC

/* Legacy Cards */
#define DDC_DEV_ID_BU65565C1            0x0901  /* 1 channel  PMC card                      */
#define DDC_DEV_ID_BU65565C2            0x0902  /* 2 channel  PMC card                      */
#define DDC_DEV_ID_BU65565C3            0x0903  /* 3 channel  PMC card                      */
#define DDC_DEV_ID_BU65565C4            0x0904  /* 4 channel  PMC card                      */
#define DDC_DEV_ID_BU65569I1            0x0B01  /* 1 channel  PCI card                      */
#define DDC_DEV_ID_BU65569I2            0x0B02  /* 2 channel  PCI card                      */
#define DDC_DEV_ID_BU65569I3            0x0B03  /* 3 channel  PCI card                      */
#define DDC_DEV_ID_BU65569I4            0x0B04  /* 4 channel  PCI card                      */
#define DDC_DEV_ID_BU65569T1            0x0C01  /* 1 channel CPCI card                      */
#define DDC_DEV_ID_BU65569T2            0x0C02  /* 2 channel CPCI card                      */
#define DDC_DEV_ID_BU65569T3            0x0C03  /* 3 channel CPCI card                      */
#define DDC_DEV_ID_BU65569T4            0x0C04  /* 4 channel CPCI card                      */
#define DDC_DEV_ID_BU65569B1            0x0C05  /* 1 channel CPCI card                      */
#define DDC_DEV_ID_BU65569B2            0x0C06  /* 2 channel CPCI card                      */
#define DDC_DEV_ID_BU65569B3            0x0C07  /* 3 channel CPCI card                      */
#define DDC_DEV_ID_BU65569B4            0x0C08  /* 4 channel CPCI card                      */
#define DDC_DEV_ID_BU65566XX            0x0E06  /* 1 - 4 channel 66MHz PCI card             */
#define DDC_DEV_ID_BU65566RX            0x1100  /* 1 - 4 channel 66MHz cPCI card            */
#define DDC_DEV_ID_BU65843              0x0400  /* BC/RT/MT WITH 4K OF RAM                  */
#define DDC_DEV_ID_BU65586N2            0x0401  /* BC/RT/MT WITH 64K OF RAM                 */
#define DDC_DEV_ID_BU65864              0x0402  /* BC/RT/MT WITH 64K OF RAM                 */
#define DDC_DEV_ID_BU65743              0x0404  /* RT ONLY WITH 4K OF RAM                   */
#define DDC_DEV_ID_BU65764              0x0406  /* RT ONLY WITH 64K OF RAM                  */

/* EBR Card */
#define DDC_DEV_ID_BU65580MX            0x2300  /* 1x 1553 and 1x EBR + 4 Port EBR Hub      */

/* Legacy Cards - High-Reliable */
#define DDC_DEV_ID_BU65596_97F1         0x2400  /* 1 channel PMC card, Front                */
#define DDC_DEV_ID_BU65596_97F2         0x2401  /* 2 channel PMC card, Front                */
#define DDC_DEV_ID_BU65596_97F4         0x2402  /* 4 channel PMC card, Front                */
#define DDC_DEV_ID_BU65596_97M1         0x2403  /* 1 channel PMC card, Rear                 */
#define DDC_DEV_ID_BU65596_97M2         0x2404  /* 2 channel PMC card, Rear                 */
#define DDC_DEV_ID_BU65596_97M4         0x2405  /* 4 channel PMC card, Rear                 */


/* Flexcore Cards */
#define DDC_DEV_ID_BU65578CX            0x1200  /* 1 - 4 channel PC-104+ card               */
#define DDC_DEV_ID_BU65590FMX           0x1300  /* 1 - 4 channel MultiIO card               */
#define DDC_DEV_ID_BU65578FMX           0x1600  /* 1 - 8 channel PMC card                   */
#define DDC_DEV_ID_BU65590CX            0x1700  /* 1 - 4 channel MultiIO PC104P card        */
#define DDC_DEV_ID_BU65590UX            0x5590  /* 1 - 4 channel MultiIO USB card           */

/* ACEX Cards */
#define DDC_DEV_ID_QPRM                 0x1A00  /* 1 channel Q-PRIME device                 */

#define DDC_DEV_ID_BU67101QX            0x1B00  /* 1 - 4 channel Express card               */
#define DDC_DEV_ID_BU67105CX            0x1D00  /* 1 - 4 channel PC-104P card               */
#define DDC_DEV_ID_BU67106KX            0x1C00  /* 1 - 4 channel PCIe Type A card           */
#define DDC_DEV_ID_BU67106BKX           0x1C80  /* 1 - 4 channel PCIe type B card           */
#define DDC_DEV_ID_BU67107FM            0x2000  /* 1 - 2 channel PMC Front/Rear card        */
#define DDC_DEV_ID_BU67108C             0x1F00  /* 1 - 2 channel PMC Front/Rear card        */
#define DDC_DEV_ID_BU67110FM            0x1E00  /* 1 - 8 channel High-Density PMC card      */
#define DDC_DEV_ID_BU67112Xx            0x2500  /* 1 - 8 channel PCIe Tacex based card      */
#define DDC_DEV_ID_BU67116WX            0x2800  /* 1 - 2 channel ABD device                   */
#define DDC_DEV_ID_BU67118FMX           0x2900  /* 1 - 4 channel MF PMC F=Front Panel IO B=Rear panel IO */
#define DDC_DEV_ID_BU67118M700          0x2910  /* 1 - 6 channel MF PMC F=Front Panel IO B=Rear panel IO */
#define DDC_DEV_ID_BU67118YZX           0x2C00  /* 1 - 4 channel MF XMC Y=Front Panel IO Z=Rear panel IO */

#define DDC_DEV_ID_BU67206KX            0x2100  /* 1 - 4 channel MF PCIe type A card (Obsoleted)  */
#define DDC_DEV_ID_BU67206BKX           0x2180  /* 1 - 4 channel MF PCIe type B card        */
#define DDC_DEV_ID_BU67210FMX           0x2200  /* 1 - 4 channel MF PCI B card              */
#define DDC_DEV_ID_DD40000K             0x2600  /* ARINC 429 & 717 PCIe card                */
#define DDC_DEV_ID_DD40100F             0x2700  /* ARINC 429 & 717 PMC card                 */
#define DDC_DEV_ID_DD40001H             0x2D00  /* ARINC 429 & 717 Mini PCIe card           */
#define DDC_DEV_ID_DD40002M             0x2E00  /* ARINC 429 & 717 PMC/XMC card             */

#define DDC_LAST_DEVICE_ID              0x0000  /* used to indicate the last device ID in a list */

#endif /* _DDC_DEVICE_IDS_H_ */
