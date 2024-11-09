#ifndef DIOOPACE_H_INCLUDED
#define DIOOPACE_H_INCLUDED


/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2004 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 *	DioopAce.h (Discrete IO operations module for ACE RTL)
 *
 *	Created 2/6/04 RR
 *
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

 
/* These defines are for the calls that operate on individual ports. */
/* The individual port references. */
#define DIO_0		0
#define DIO_1		1
#define DIO_2		2
#define DIO_3		3
#define DIO_4		4
#define DIO_5		5
#define DIO_6		6
#define DIO_7		7
#define DIO_8		8
#define DIO_9		9
#define DIO_10		10
#define DIO_11		11

/* Direction options. */
#ifndef DIO_IN
#define DIO_IN		0x0000
#endif
#ifndef DIO_OUT
#define DIO_OUT		0x8000
#endif

/* State options. */
#define DIO_OFF		0
#define DIO_ON		1

/* Directive to read the direction or state of a port. */
#define DIO_READ	0xFFFF

/* Mode Functions */
#define DIO_ONLY	0x0000
#define DIO_HUB		0x4000

/*-----------------------------------------------------------------------
Name:	BuDioRead

Description:
	This function reads a bit in one of the discrete IO banks.
	Upon a read command, the given bit's value is returned in
	the command parameter.

In		bPort  = port working with ( DIO_0 - DIO_12 )
Out		wCmd   = returned read status (DIO_IN, DIO_OUT)
Out		return = error condition
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuDioRead(U16BIT bPort, U16BIT *wCmd);

/*-----------------------------------------------------------------------
Name:	BuDioWrite

Description:
    This function writes a bit in one of the discrete IO banks.

In		bPort  = port working with ( DIO_0 - DIO_11 )
In		wCmd   = write state (DIO_OFF, DIO_ON)
Out		return = error condition
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuDioWrite(U16BIT bPort, U16BIT wCmd);


/*-----------------------------------------------------------------------
Name:	BuDioDirBank

Description:
	This function sets or determines the state of all IO ports in a
	discrete bank.

In/Out	wDirection = Input / Output state of all ports in a given bank 
		 0x0000 = All Inputs, 0x8000 = All Outputs
In		bMask  = logic '0' indicates a bank write,
		         logic '1' indicates a bank read is to be done.
In		bPort  = Port number (DIO_0 - DIO_7 in bank1 {0x01a6},
							 DIO_8 - DIO_11 in bank2 {0x01a8} )
Out		return = error condition.
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuDioDirBank(U16BIT *wDirection, U8BIT bMask, U16BIT bPort);

/*-----------------------------------------------------------------------
Name:	BuSetDioFunction

Description:
	This function sets the mode that the DIO registers will operate -
	either DIO or HUB and DIO.

In		wFunc = Function type DIO Only = 0x0000, DIO & HUB = 0x4000.
Out		return = error condition.
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuSetDioFunction(U16BIT wFunc);

/*-----------------------------------------------------------------------
Name:	BuGetDioFunction

Description:
	This function gets the mode that the DIO registers are currently
	operating in -	either DIO or HUB and DIO.

Out		wFunc = Function type DIO Only = 0x0000, DIO & HUB = 0x4000.
Out		return = error condition.
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuGetDioFunction(U16BIT *wFunc);

/*-----------------------------------------------------------------------
Name:	BuSetDioHubPort

Description:
	This function sets address of the discrete I/O HUB.

In		wPort = Hub port address.
Out		return = error condition.
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuSetDioHubPort(U16BIT wPort);

/*-----------------------------------------------------------------------
Name:	BuGetDioHubPort

Description:
	This function gets address of the discrete I/O HUB.

In/Out	wPort = Hub port address.
Out		return = error condition.
------------------------------------------------------------------------*/
__BUEXTERN BuError_t __BUDECL BuGetDioHubPort(U16BIT *wPort);

#endif /* DIOOPACE_H_INCLUDED */

