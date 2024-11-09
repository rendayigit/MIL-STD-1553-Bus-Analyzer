#ifndef ERR_H_INCLUDED
#define ERR_H_INCLUDED
 /* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * ERR.H (ERROR CONTROL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for ERR.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    initial release
 *
 *   2.1       01-JUN-1995    error constants were added
 *
 *   2.2       01-SEP-1995    none
 *
 *   2.3       01-JAN-1996    none
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       04-DEC-1996    none
 *
 *   4.0       29-NOV-1997    none
 *
 *   5.3.1     11-APR-2003    none
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* ERROR TYPE DEFINITIONS AND STRUCTURES ----------------------------------*/

typedef int BuError_t;

/* ERROR CONSTANTS --------------------------------------------------------*/

#define BU_SUCCESS                  0x00

/* conf module errors */
#define BU_ERROR_OPENING_FILE       -100
#define BU_ERROR_UNKNOWN_CARD       -101
#define BU_ERROR_REGISTERCLIENT     -102
#define BU_ERROR_ENABLE             -103
#define BU_ERROR_DISABLE            -104
#define BU_ERROR_DEREGISTERCLIENT   -105
#define BU_ERROR_NO65550ENA         -106
#define BU_ERROR_OPEN				-107
#define BU_ERROR_OPEN_DRIVER        -1070
#define BU_ERROR_OPEN_BAD_CONF		-1071

#define BU_ERROR_RESET_ACE          -108
#define BU_ERROR_NO_BUCONF_CLOSE    -109
#define BU_ERROR_OS					-110
#define BU_ERROR_NOPC104			-111
#define BU_ERROR_PAGEMODE			-112
#define BU_ERROR_NOEBR104			-113
#define BU_ERROR_NOEBR104HUB		-114
#define BU_ERROR_EBR104DIO			-115
#define BU_ERROR_DRIVER				-120
#define BU_ERROR_NOLOGICALDEVICE	-130
#define BU_ERROR_PARAMETER          -131


/* mt module errors */
#define BU_ERROR_MTSTKSIZE          -200
#define BU_ERROR_MTSTKSIZE2         -201
#define BU_ERROR_MTSTKSIZE3         -202
#define BU_ERROR_MTBUFNFLIP         -203
#define BU_ERROR_MTSETSTACK         -204
#define BU_ERROR_MTBUFEMPTY         -205
#define BU_ERROR_MTSTKBUFOVR        -206
#define BU_ERROR_MTENDSTK           -207
#define BU_ERROR_MTDECODERR         -208
#define BU_ERROR_MTNOTOPENED        -209
#define BU_ERROR_MTCMDSTKOVR		-210
#define BU_ERROR_MTSTKBUFANDSTKOVR  -211
#define BU_ERROR_MTINVALIDSTRATEGY  -212
#define BU_ERROR_MTCMDSTKOVRANDCLR	-213

/* bc module errors */
#define BU_ERROR_BCNOTOPENED        -300
#define BU_ERROR_BCINVSENDLENGTH    -301
#define BU_ERROR_BCSENDTIMOUT       -302
#define BU_ERROR_BCNRSENDING        -303
#define BU_ERROR_BCFESENDING        -304
#define BU_ERROR_BCLFSENDING        -305
#define BU_ERROR_BCSTKBUFOVR        -306
#define BU_ERROR_BCSTKBUFFULL       -307
#define BU_ERROR_BCDECODERR         -308
#define BU_ERROR_BCBUFEMPTY         -309
#define BU_ERROR_ALLOCCMDSTKFAIL    -310
#define BU_ERROR_BCBUSY             -311
#define BU_ERROR_INVFRAMETIME       -312
#define BU_ERROR_BCINVMSGNUM        -313

#define BU_ERROR_FRMNOTCONFIGURED	-315
#define BU_ERROR_INVMSGCOUNT		-316
#define BU_ERROR_INVFRMCOUNT		-317
#define BU_ERROR_BCSTARTFRAME       -318

/* rt module errors */
#define BU_ERROR_RT_NOMSG           -400
#define BU_ERROR_RTDEFMSGILLTYPE    -401
#define BU_ERROR_RTDEFMSGILLSA      -402
#define BU_ERROR_RTDEFMSGILLWC      -403
#define BU_ERROR_INVALIDMODECODE    -404
#define BU_ERROR_ENHANCEDMODEOFF    -405
#define BU_ERROR_RTNOTOPENED        -406
#define BU_ERROR_RTMONNOTOPENED     -407
#define BU_ERROR_RTILLADDR			-408

/* acemem module errors */
#define BU_ERROR_OUTOFMEMORY        -500
#define BU_ERROR_BADBLOCK           -501
#define BU_ERROR_BLOCKTOOSMALL      -502
#define BU_ERROR_BUFFERTOOSMALL     -503

/* interrupt module errors */
#define BU_ERROR_INVALIRQ           -600

/* interface module errors */
#define BU_ERROR_NOTCONFIGURED      -700
#define BU_ERROR_RAMOUTOFRANGE      -701
#define BU_ERROR_NO65528ENA         -702
#define BU_ERROR_CANADDR			-703

/* expanded buffer module errors */
#define BU_ERROR_NOMEMORY		    -800
#define BU_ERROR_ALREADYCREATED     -801
#define BU_ERROR_BUFNOTOPENED       -802
#define BU_ERROR_BUFNOTALLOCD       -803

/* Numerical functions errors */
#define BU_ERROR_HIGHBOUND 0xFFFF
#define BU_ERROR_LOWBOUND 0x0000

/* Linux dependent errors */
#ifdef _ACELINUX
#define BU_ERROR_LINUX_PANEL	 	        -900
#define BU_ERROR_LINUX_PANEL_ENTRY	 	    -901
#define BU_ERROR_LINUX_DEVICE_NOT_FOUND     -902
#define BU_ERROR_LINUX_DEVICE_FAILED_OPEN   -903
#define BU_ERROR_LINUX_REG_FAILED_MMAP      -904
#define BU_ERROR_LINUX_MEM_FAILED_MMAP      -905
#define BU_ERROR_LINUX_FAILED_IOCTL_CALL    -906
#define BU_ERROR_LINUX_IRQ_INSTALL_FAILED   -907
#define BU_ERROR_LINUX_IRQ_UNINSTALL_FAILED -908
#endif



/* ERROR FUNCTION PROTOTYPES ----------------------------------------------*/

/* convert error value into string */
__BUEXTERN char* __BUDECL BuErrorStr(BuError_t BuError);

/*
 * END
 * ERR.H (ERROR CONTROL MODULE)
 */

#endif /*ERR_H_INCLUDED */

