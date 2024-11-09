#ifndef STDACE_H_INCLUDED
#define STDACE_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * STDACE.H (STANDARD ACE MODULE)
 *
 * This is the master header file for the ACE Library. It contains compiler
 * directives used throught the library. It makes the necessary defines for
 * the dynamic link library compilation and use. In addition, macros are
 * defined for standard data types and dynamic memory allocation to make
 * the ACE library transportable.
 *
 * NOTE: Any changes to this file will require re-compilation of the
 *       ACE Library for the changes to take effect.
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for STDACE.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    initial release
 *
 *   2.1       01-JUN-1995    added rt.h inclusion
 *
 *                            changed __BUDEBUG to __BUDEBUG_FUNCTIONS and
 *                            __BUDEBUG_ERROR_CHECKING
 *
 *   2.2       01-SEP-1995    none
 *
 *   2.3       01-JAN-1996    added #define for signed integers (8,16,32)
 *                            and boolean types
 *
 *                            added __BUEXTINT compile option used in
 *                            the interface module for external linking
 *
 *   2.4       01-MAY-1996    added optional compile time option __BUMEMCPY
 *                            to optionally use the memcpy routine or a loop
 *
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       07-JUNE-1996   removed BU65550 enabler from WIN32 compiles; 
 *                            removed DOS screen graphics functions from WIN32 
 *                            compiles;added _Windows and __IMPORTDLL__ defines 
 *                            for WIN32 compiles;added __BUFAR which is defined 
 *                            dependant upon whether a 16bit or a WIN32 
 *                            application is being compiled; added necessary 
 *                            BUDECL and BUEXTERN defines for WIN32 compiles
 *
 *   4.0       29-NOV-1997    changed __BU65528ENA compilation to support NIVXI
 *                            WindowsNT software. Changed defines to allow support
 *                            for LabWindows CVI.
 *                                       
 *   5.3.1     11-APR-2003    none
 */

/* independent standard 'C' libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* uncomment below line if in linux */
#define _ACELINUX

#ifndef	DDC_HANDLE	
    #define DDC_HANDLE unsigned long
#endif

/* no headers in CVI, not supported in linux */
#if (!defined(_CVI_)) && (!defined(_ACELINUX)) && (!defined(VX_WORKS))
	#include <conio.h>
	#include <dos.h>
#endif

/* not supported in MS-C, CVI, and Linux */
#if (!defined(_MSC_VER)) && (!defined(_CVI_)) && (!defined(_ACELINUX)) && (!defined(VX_WORKS))
	#include <mem.h>
#else
	/* define un-defined primitives */
    #if (!defined(_WIN32)) && (!defined(_ACELINUX)) && (!defined(VX_WORKS))
		#include <graph.h>
		#define clrscr()  _clearscreen(_GCLEARSCREEN)
		#define outport outpw
		#define inport inpw
		#define getvect _dos_getvect
		#define setvect _dos_setvect
		#define disable _disable
		#define enable _enable
	#else
		/* linux overrides */
        #if (defined(_ACELINUX)) || (defined(VX_WORKS))
	    	#define clrscr()
			#define outport
			#define inport
			#define getvect
			#define setvect
			#define disable()
			#define enable()
			#if (defined(_ACELINUX))
				#define TRUE 1
				#define FALSE 0
			#endif
		#endif
	#endif


	/* Microsoft C defaults to word alignment on structure members. This will
	 * corrupt the format of our strucutres which must support
	 * byte aligned structure members. This pragma will set alignment to 1 byte.
	 */
	#if (defined(_ACELINUX)) || (defined(VX_WORKS))
		#pragma pack(1)
	#endif

	/* Library uses borland _Windows not MSC _WINDOWS or WIN32
	 * modified for WIN32.
	 */
	#if defined(_WINDOWS) | defined(_WIN32)
		#define _Windows
	#endif
#endif

#ifdef _Windows 
	#include <windows.h>
#endif

#ifdef _ACELINUX
#include <pthread.h>
#endif


/* library is written in 'C' */
#ifdef __cplusplus
	extern "C" {
#endif

#if defined(WIN32) && !defined(_WIN32)
	#define _WIN32
#endif

/* uncomment to enable 65550 pcmcia enabler option */
/* enabler no longer used in Windows 95            */
/* modified for WIN32                              */
#ifdef _WIN32
	#include <BuWin32.h>
#endif

#if (!defined(WIN32)) && (!defined(_ACELINUX)) && (!defined(VX_WORKS))
	#define __BU65550ENA
#endif

/* uncomment to enable 65528 MXI-1/2 enabler option */
/*#define __BU65528ENA*/

/* uncomment if using ace interrupt capibility */
#define __BUACEIRQ

/* uncomment if using external interface module added [01-JAN-1996] */
/* #define __BUEXTINT */

/* Runtime performance can be increased by converting functions to
 * inline macros and taking out general error/range checking. It is
 * recommended (at least) error checking is used during development.
 * modified from just __BUDEBUG [01-JUN-1995]
 */

#define __BUDEBUG_FUNCTIONS 
#define __BUDEBUG_ERROR_CHECKING

/* Uncomment the following to use the memcpy routine from the mem.h
 * standard C library. If this is commented, a iterative loop is used.
 * The 65539 card in known to have problems with memcopy on some computers,
 * the iterative loop fixes this problem.
 */
/* #define __BUMEMCPY */

/* Not necessary for Linux */
#if (!defined(_ACELINUX)) && (!defined(VX_WORKS))
	/* If compiling a dll then define __IMPORTDLL__ */
	/* added _DLL for MSC WIN32 compatability       */
	#if (defined(__DLL__)) || (defined(_DLL)) || (defined(_WINDLL))
		#define __IMPORTDLL__
	#endif
	
	/* __IMPORTDLL__ creates exportable function declarations */
	
	#ifdef __IMPORTDLL__
		/* modified for WIN32   */
		#ifdef _WIN32
		       /* WIN32 DLL     */
			#define __BUDECL WINAPI
			#define __BUEXTERN
			#define __BUFAR
		#else
		      /* WIN16 DLL     */
			#define __BUDECL far pascal _export
			#define __BUEXTERN extern
			#define __BUFAR far
		#endif
	#else
		#ifdef _WIN32
			/* WIN32 application  */
			#define __BUDECL WINAPI
			#define __BUEXTERN
			#define __BUFAR
		#else
			/* 16bit / Unknown */
			#define __BUDECL
			#define __BUEXTERN
			#define __BUFAR far
		#endif
	#endif
#else
	/* Linux */
	#define __BUDECL 
	#define __BUEXTERN extern
	#define __BUFAR
#endif

/* The standard memory allocation routine can be changed here */
#define advBuMalloc(size) malloc(size) /* size,bytes */

/* The standard memory free routine can be changed here */
#define advBuFree(ptr) free(ptr)

/* Removed and placed into a routine 11/17/98 */

/*   
The standard memory copy routine can be changed here 
#ifdef __BUMEMCPY added compile time option [01-MAY-1996]
#define BuMemCpy(src,dest,size) memcpy(dest,src,size*2) size,words 
#else
#define BuMemCpy(src,dest,size) 
{int i;for(i=0;i<size;i++)*(dest+i)=*(src+i);}
#endif
*/

/* The standard 32/16/8 bit unsigned integers can be changed here. */
#if (defined(_ACELINUX)) || (defined(VX_WORKS))
#define U32BIT unsigned int
#else
#define U32BIT unsigned long
#endif
#define U16BIT unsigned short
#define U8BIT unsigned char

/* The standard signed integers added [01-JAN-1996] */
#if (defined(_ACELINUX)) || (defined(VX_WORKS))
#define S32BIT int
#else
#define S32BIT long
#endif
#define S16BIT short
#define S8BIT char

/* The standard boolean integer/states added [01-JAN-1996] */
#ifndef BOOLEAN
#define BOOLEAN char
#endif

#if (!defined(VX_WORKS))
#define TRUE                         1
#define FALSE                        0
#endif /* (!defined(VX_WORKS)) */
#define ON                           1
#define OFF                          0

/* Definitions not provided by UDL */
#define ULONG unsigned long
#define LONG  signed long
#define UCHAR unsigned char

typedef struct BuConfStruct *BuConf_p;

/* All module header files are included */
#include "err.h"
#include "ace.h"
#include "int.h"
#include "irq.h"
#include "acemem.h"
#include "test.h"
#include "buf.h"
#include "mt.h"
#include "bc.h"
#include "rt.h"
#include "dioopAce.h"

#ifdef __BU65550ENA   /* PCMCIA enabler */
	#include <65550ena.h>
#endif

#ifdef __BU65528ENA   /* 65528 enabler */
#if (!defined(_ACELINUX)) || (!defined(VX_WORKS))
	     #define __PROTOTYPES__
	     #define VXINT       /* Define NIVXI platform here */
	     /* #define VXIDOS */
	     /* #define VXIWIN */
	     #include <NIVXI.H>
	     #include <bu65528.h>
    #endif
#endif

#include "conf.h"

#ifdef ADVACE
#include "advstdace.h"
#endif

#ifdef __cplusplus
}
#endif

/* next line added by syk to restore packing to as specified by /Zp (default is /Zp8 */
#pragma pack()

#define DDC_PRINTF(X,...) /*printf */

/*
 * END
 * STDACE.H (STANDARD ACE MODULE)
 */

#endif /* STDACE_H_INCLUDED */

