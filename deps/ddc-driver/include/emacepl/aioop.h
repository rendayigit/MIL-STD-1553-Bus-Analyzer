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
 * a_io_op.c (Avionic discrete IO operations module)
 *
 *  Created 11/13/02 KWH
 *
 * Programmers Note:
 *  The avionic IO ports associated with a particular logical device
 *  are also associated with up to  four other devices.  These ports
 *  are common to cards that have up to four Enhanced Mini-ACE chips
 *  on them.  This must be taken into consideration when working with
 *  multiple channels on the same card while manipulating the ports.
 */

#ifndef __AIOOP_H__
#define __AIOOP_H__

/* BU-65590 avionic IO register */
#define AIO_OUTREG   0x41D

/* Max Supported Avionic Bits */
#define AIO_MAX_BITS 16

/* These defines are for the calls that operate on individual ports. */
/* The individual port references. */
#define AIO_OUT1     1
#define AIO_OUT2     2
#define AIO_OUT3     3
#define AIO_OUT4     4
#define AIO_OUT5     5
#define AIO_OUT6     6
#define AIO_OUT7     7
#define AIO_OUT8     8
#define AIO_OUT9     9
#define AIO_OUT10    10
#define AIO_OUT11    11
#define AIO_OUT12    12
#define AIO_OUT13    13
#define AIO_OUT14    14
#define AIO_OUT15    15
#define AIO_OUT16    16

/* Direction masks */
extern const U16BIT AIO_DIR_BIT[AIO_MAX_BITS];

/* State masks */
extern const U16BIT AIO_STATE_BIT[AIO_MAX_BITS];

/* Directive to read the direction or state of a port. */
#define AIO_OUTREAD 0xFFFF

/* this get/set one bit at a time. */
_EXTERN S16BIT _DECL _aceGetAioOut
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _aceSetAioDir
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_DIRECTION Direction
);

_EXTERN S16BIT _DECL _aceGetAioDir
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _aceGetAioIn
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _aceSetAioOut
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_LEVEL Level
);

/* these get/set all the bits at once. */
_EXTERN S16BIT _DECL _aceGetAioAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL _aceSetAioAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

#endif /* __A_IO_OP_H__ */
