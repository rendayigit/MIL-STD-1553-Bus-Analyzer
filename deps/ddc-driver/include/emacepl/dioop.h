/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *          Copyright (c) 1999 by Data Device Corporation
 *          All Rights Reserved.
 *
 *  Dioop.c (Discrete IO operations module)
 *
 *  Created 11/13/02 KWH
 *
 * Programmers Note:
 *  The discrete IO ports associated with a particular logical device
 *  are also associated with up to  four other devices.  These ports
 *  are common to cards that have up to four Enhanced Mini-ACE chips
 *  on them.  This must be taken into consideration when working with
 *  multiple channels on the same card while manipulating the ports.
 */

#ifndef __DIOOP_H__
#define __DIOOP_H__

/* BU-65567/8 discrete IO register */
#define DIO_REG             0x206

/* Max Supported Discrete Bits */
#define DIO_MAX_BITS    16

/* These defines are for the calls that operate on individual ports. */
/* The individual port references. */
#define DIO_1       1
#define DIO_2       2
#define DIO_3       3
#define DIO_4       4
#define DIO_5       5
#define DIO_6       6
#define DIO_7       7
#define DIO_8       8
#define DIO_9       9
#define DIO_10      10
#define DIO_11      11
#define DIO_12      12

/* Direction options. */
#define DIO_IN      0
#define DIO_OUT     1

/* State options. */
#define DIO_OFF     0
#define DIO_ON      1

/* Directive to read the direction or state of a port. */
#define DIO_READ    0xFFFF

/* this get/set once bit at a time. */
_EXTERN S16BIT _DECL _aceGetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _aceGetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _aceGetDiscIn
(
    S16BIT DevNum,
    S16BIT Discrete
);

_EXTERN S16BIT _DECL _aceSetDiscOut
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_LEVEL Level
);

_EXTERN S16BIT _DECL _aceSetDiscDir
(
    S16BIT DevNum,
    S16BIT Discrete,
    DISC_DIR Direction
);

/* these get/set all the bits at once. */
_EXTERN S16BIT _DECL _aceGetDiscAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL _aceSetDiscAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

#endif /* __DIOOP_H__ */
