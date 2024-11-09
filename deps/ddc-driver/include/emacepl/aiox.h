/*  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *          AceXtreme 'C' Run Time Library
 *
 *          Copyright (c) 2008 by Data Device Corporation
 *          All Rights Reserved.
 *
 *          axaioop.c (Avionic IO operations module)
 *
 *  Created 11/13/02 KWH
 *          2008 SF+ support added
 *
 * Programmers Note:
 *  The avionic IO ports associated with a particular logical device
 *  are also associated with up to  four other devices.  These ports
 *  are common to cards that have up to four AceXtreme chips
 *  on them.  This must be taken into consideration when working with
 *  multiple channels on the same card while manipulating the ports.
 */

#ifndef __AIOX_H__
#define __AIOX_H__

#include "aio.h"


_EXTERN S16BIT _DECL _acexGetAioOut
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _acexSetAioDir
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_DIRECTION Direction
);

_EXTERN S16BIT _DECL _acexGetAioDir
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _acexGetAioIn
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL _acexGetAioAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL _acexSetAioAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);

_EXTERN S16BIT _DECL _acexSetAioOut
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_LEVEL Level
);

_EXTERN S16BIT _DECL _acexConfigureIoInterruptConditions
(
    S16BIT s16DevNum,
    U16BIT u16Command,
    ACEX_IO_CONFIGURE_TYPE *pIoConfigure
);

#endif /* __AIOX_H__ */
