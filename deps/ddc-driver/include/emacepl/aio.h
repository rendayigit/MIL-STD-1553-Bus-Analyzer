/*******************************************************************************
 * FILE: aio.h
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

#ifndef __AIO_H__
#define __AIO_H__

/* ========================================================================== */
/*                                DATA TYPES                                  */
/* ========================================================================== */


/* ========================================================================== */
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/* ========================================================================== */
_EXTERN S16BIT _DECL aceGetAioOut
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL aceSetAioDir
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_DIRECTION Direction
);

_EXTERN S16BIT _DECL aceGetAioDir
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL aceGetAioIn
(
    S16BIT DevNum,
    S16BIT Channel
);

_EXTERN S16BIT _DECL aceSetAioOut
(
    S16BIT DevNum,
    S16BIT Channel,
    AVIONIC_LEVEL Level
);

_EXTERN S16BIT _DECL aceGetAioAll
(
    S16BIT DevNum,
    U16BIT *Directions,
    U16BIT *Levels
);

_EXTERN S16BIT _DECL aceSetAioAll
(
    S16BIT DevNum,
    U16BIT Directions,
    U16BIT Levels
);


/*******************************************************************************
 * Name:    acexConfigureIoInterruptConditions
 *
 * Description:
 *
 *      This function configures the I/O interrupt conditions. An interrupt can be generated when
 *      either a rising or falling edge is encountered.
 *
 * In:  s16DevNum               Card Number
 *
 * In: u16Command
 *
 *      =================================================================================
 *      ACEX_AIO_INTERRUPT_CONFIGURE__ENABLE
 *      =================================================================================
 *
 *      Enables the AIO Interrupt and sets the callback ISR.
 *
 *      ----------------------
 *      pAioConfigure Settings
 *      ----------------------
 *
 *      funcExternalIsr
 * 
 *          This is the user designated ISR callback function written by the user.This function will
 *          be called when the device generates an interrupt caused by an AIO source, either a rising
 *          or falling edge.
 * 
 *              Valid pointer = sets the callback function to the given function pointer
 *              Null pointer = leaves the function pointer as is
 * 
 *          The u32IrqStatus indicates which AIO(s) generated the interrupt.
 * 
 *          The value returned will be encoded as follows:
 * 
 *          +================================================+
 *          |            u32IrqStatus Register               |
 *          |================================================|
 *          | BIT  DESCRIPTION                               |
 *          |================================================|
 *          | 31   Avionic I/O 16  Interrupt on Rising Edge  |
 *          | 30   Avionic I/O 16  Interrupt on Falling Edge |
 *          | 29   Avionic I/O 15  Interrupt on Rising Edge  |
 *          | 28   Avionic I/O 15  Interrupt on Falling Edge |
 *          | 27   Avionic I/O 14  Interrupt on Rising Edge  |
 *          | 26   Avionic I/O 14  Interrupt on Falling Edge |
 *          | 25   Avionic I/O 13  Interrupt on Rising Edge  |
 *          | 24   Avionic I/O 13  Interrupt on Falling Edge |
 *          | 23   Avionic I/O 12  Interrupt on Rising Edge  |
 *          | 22   Avionic I/O 12  Interrupt on Falling Edge |
 *          | 21   Avionic I/O 11  Interrupt on Rising Edge  |
 *          | 20   Avionic I/O 11  Interrupt on Falling Edge |
 *          | 19   Avionic I/O 10  Interrupt on Rising Edge  |
 *          | 18   Avionic I/O 10  Interrupt on Falling Edge |
 *          | 17   Avionic I/O 9   Interrupt on Rising Edge  |
 *          | 16   Avionic I/O 9   Interrupt on Falling Edge |
 *          | 15   Avionic I/O 8   Interrupt on Rising Edge  |
 *          | 14   Avionic I/O 8   Interrupt on Falling Edge |
 *          | 13   Avionic I/O 7   Interrupt on Rising Edge  |
 *          | 12   Avionic I/O 7   Interrupt on Falling Edge |
 *          | 11   Avionic I/O 6   Interrupt on Rising Edge  |
 *          | 10   Avionic I/O 6   Interrupt on Falling Edge |
 *          | 9    Avionic I/O 5   Interrupt on Rising Edge  |
 *          | 8    Avionic I/O 5   Interrupt on Falling Edge |
 *          | 7    Avionic I/O 4   Interrupt on Rising Edge  |
 *          | 6    Avionic I/O 4   Interrupt on Falling Edge |
 *          | 5    Avionic I/O 3   Interrupt on Rising Edge  |
 *          | 4    Avionic I/O 3   Interrupt on Falling Edge |
 *          | 3    Avionic I/O 2   Interrupt on Rising Edge  |
 *          | 2    Avionic I/O 2   Interrupt on Falling Edge |
 *          | 1    Avionic I/O 1   Interrupt on Rising Edge  |
 *          | 0    Avionic I/O 1   Interrupt on Falling Edge |
 *          +================================================+
 * 
 *          Interrupt on Rising Edge
 *              1 = Indicates a rising edge was encountered for the corresponding Avionic I/O
 *              0 = No interrupt generated for this source
 * 
 *          Interrupt on Falling Edge
 *              1 = Indicates a falling edge was encountered for the corresponding Avionic I/O
 *              0 = No interrupt generated for this source
 *
 *
 *      =================================================================================
 *      ACEX_AIO_INTERRUPT_CONFIGURE__DISABLE
 *      =================================================================================
 *
 *      Disables the AIO Interrupt and clears the callback ISR.
 *
 *      pAioConfigure - Not used
 *
 *
 *      =================================================================================
 *      ACEX_AIO_INTERRUPT_CONFIGURE__SET
 *      =================================================================================
 *
 *      Sets the AIO Interrupt Conditions.
 *
 *      ----------------------
 *      pIoConfigure Settings
 *      ----------------------
 *
 *      u32RisingEdge
 * 
 *          Generates an interrupt on a rising edge, when set. If the corresponding bit for
 *          an AIO is set and a rising edge is encountered, the funcExternalIsr function will
 *          be called.
 *
 *              Bit 0 corresponds to AIO 0
 *              Bit 1 corresponds to AIO 1
 *              Etc...
 * 
 *          Bit value of 1 = enables the generation of an interrupt when a rising edge is encountered
 *          Bit value of 0 = leaves the current operation as is
 * 
 *      u32FallingEdge
 * 
 *          Generates an interrupt on a falling edge, when set. If the corresponding bit for
 *          an AIO is set and a falling edge is encountered, the funcExternalIsr function will
 *          be called.
 *
 *              Bit 0 corresponds to AIO 0
 *              Bit 1 corresponds to AIO 1
 *              Etc...
 * 
 *          Bit value of 1 = enables the generation of an interrupt when a falling edge is encountered
 *          Bit value of 0 = leaves the current operation as is
 *
 *
 *      =================================================================================
 *      ACEX_AIO_INTERRUPT_CONFIGURE__CLEAR
 *      =================================================================================
 *
 *      Clears the AIO interrupt conditions.
 *
 *      ----------------------
 *      pIoConfigure Settings
 *      ----------------------
 *
 *      u32RisingEdge
 *
 *          Clears "Interrupt on Rising Edge" setting. If the corresponding bit for an AIO is set,
 *          the rising edge interrupt source is disabled.
 *
 *              Bit 0 corresponds to AIO 0
 *              Bit 1 corresponds to AIO 1
 *              Etc...
 *
 *          Bit value of 1 = disables the generation of an interrupt when a rising edge is encountered
 *          Bit value of 0 = leaves the current operation as is
 *
 *      u32FallingEdge
 *
 *          Clears "Interrupt on Falling Edge" Setting. If the corresponding bit for an AIO is set,
 *          the falling edge interrupt source is disabled.
 *
 *              Bit 0 corresponds to AIO 0
 *              Bit 1 corresponds to AIO 1
 *              Etc...
 *
 *          Bit value of 1 = disables the generation of an interrupt when a falling edge is encountered
 *          Bit value of 0 = leaves the current operation as is
 *
 *      funcExternalIsr (not used)
 *
 *          Should be set to NULL
 *
 *
 *      =================================================================================
 *      ACEX_AIO_INTERRUPT_CONFIGURE__GET
 *      =================================================================================
 *
 *      Gets the interrupt condition settings and the user callback function pointer.
 *
 *      ----------------------
 *      pIoConfigure Settings
 *      ----------------------
 *
 *      u32RisingEdge
 * 
 *          Bit 0 corresponds to AIO 0
 *          Bit 1 corresponds to AIO 1
 *          Etc...
 * 
 *          Bit value of 1 = "generation of an interrupt when a rising edge is encountered" is enabled
 *          Bit value of 0 = not enabled
 * 
 *      u32FallingEdge
 * 
 *          Bit 0 corresponds to AIO 0
 *          Bit 1 corresponds to AIO 1
 *          Etc...
 * 
 *          Bit value of 1 = "generation of an interrupt when a falling edge is encountered" is enabled
 *          Bit value of 0 = not enabled
 *
 *
 * In: *pIoConfigure - pointer to a ACEX_IO_CONFIGURE_TYPE structure
 *
 * Out: return - error condition
 ******************************************************************************/
_EXTERN S16BIT acexConfigureIoInterruptConditions
(
    S16BIT s16DevNum,
    U16BIT u16Command,
    ACEX_IO_CONFIGURE_TYPE *pIoConfigure
);

#endif /* ifndef __AIO_H__ */
