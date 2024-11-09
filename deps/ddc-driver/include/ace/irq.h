#ifndef IRQ_H_INCLUDED
#define IRQ_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * IRQ.H (INTERRUPT HANDLER AND CONTROL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for IRQ.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    converted rel 1.6 STDACE routines for use
 *                            with BuConf structure; added multiple wrap
 *                            around functions for BuSystemIsr
 *
 *   2.1       01-JUN-1995    added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros
 *
 *   2.2       01-SEP-1995    added BuSetUsrPtr() and BuGetUsrPtr()
 *                            functions used for c++ compatibility in
 *                            access of member data/functions from inside
 *                            a user defined interrupt service
 *
 *   2.3       01-JAN-1996    none
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       07-JUNE-1996   modified BuInstallIsr() function call definition in
 *                            WIN32; replaced 'far' with __BUFAR for Microsoft C 
 *                            version of BuInstallIsr() declaration, the MSC
 *                            versions of BuSystemIsr_Irq#(), and 
 *                            BuInstallUsrHandler; added BuIsrHan(int IRQLevel)
 *                            prototype for new function in WIN32.
 *
 *   4.0       29-NOV-1997    none
 *   
 *   5.3.1     11-APR-2003    none
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* INTERRUPT CONSTANTS ----------------------------------------------------*/

/* IRQ status reg and IRQ mask reg */
#define IRQ_ALL                          0xffff
#define IRQ_MASTER                       0x8000
#define IRQ_RAM_PARITY_ERROR             0x4000
#define IRQ_TRANSMITTER_TIMEOUT          0x2000
#define IRQ_BC_RT_CMD_STK_ROLLOVR        0x1000
#define IRQ_MT_CMD_STACK_ROLLOVR         0x0800
#define IRQ_MT_DTA_STACK_ROLLOVR         0X0400
#define IRQ_HANDSHAKE_FAILURE            0X0200
#define IRQ_BC_RETRY                     0X0100
#define IRQ_RT_ADDR_PARITY_ERROR         0X0080
#define IRQ_TIMETAG_ROLLOVR              0X0040
#define IRQ_RT_CIRC_BUFFR_ROLLOVR        0X0020
#define IRQ_RT_BC_MESSAGE_INT            0X0010
#define IRQ_BC_END_OF_FRAME              0X0008
#define IRQ_RT_BC_MT_FORMAT_ERROR        0X0004
#define IRQ_STATUS_SET_MODE_INT_TRIG     0X0002
#define IRQ_END_OF_MESSAGE               0X0001

/* parameter for BuIrqType */
#define PULSE        0
#define LEVEL        1

#ifdef _WIN32
/* Event defines for interrupt handling */
#define BU_IRQEVENT		0
#define BU_EXITEVENT	1
#define BU_BLOCKEVENT	2 /* Win95/98 only */

/* Threads for interrupt handling */
#define BU_DISPATCH		0
#define BU_WORKER		1

#endif

/* INTERRUPT FUNCTION PROTOTYPES  -----------------------------------------*/

/* Install interrupt service routine for a given compiler*/
#if defined(__TURBOC__) & !defined(_WIN32)     /* Borland Turbo C */
	BuError_t BuInstallIsr(void interrupt(*Isr)(void));
#endif

#if defined(_MSC_VER) & ! defined(_WIN32)       /* Microsoft C */
	BuError_t BuInstallIsr(void(_interrupt __BUFAR *Isr)(void)); 
#endif

#if defined(_WIN32)								/* Win32 compiler */
 #ifdef __BU65528
	BuError_t BuInstallIsr(BU28_CardInfo_p);   /*65528 support*/
 #else
	BuError_t BuInstallIsr(void);				/*CVI support*/
 #endif
#endif

#ifdef _ACELINUX
	BuError_t BuInstallIsr();
	void BuUninstallIsr(void);
#else
/* Un-install interrupt service routine */
void BuUninstallIsr(void);
#endif

/* Each IRQ level has a unique service routine vector   */
/* BuSystemIsr is called with the irq level information */
#ifndef _WIN32  /* added for WIN32 */

#ifdef __TURBOC__
void interrupt BuSystemIsr_Irq3(void);
void interrupt BuSystemIsr_Irq4(void);
void interrupt BuSystemIsr_Irq5(void);
void interrupt BuSystemIsr_Irq6(void);
void interrupt BuSystemIsr_Irq7(void);
void interrupt BuSystemIsr_Irq8(void);
void interrupt BuSystemIsr_Irq9(void);
void interrupt BuSystemIsr_Irq10(void);
void interrupt BuSystemIsr_Irq11(void);
void interrupt BuSystemIsr_Irq12(void);
void interrupt BuSystemIsr_Irq13(void);
void interrupt BuSystemIsr_Irq14(void);
void interrupt BuSystemIsr_Irq15(void);
#endif

#ifdef _MSC_VER
/* modified for WIN32 replaced far with __BUFAR   */
void _interrupt __BUFAR BuSystemIsr_Irq3(void);
void _interrupt __BUFAR BuSystemIsr_Irq4(void);
void _interrupt __BUFAR BuSystemIsr_Irq5(void);
void _interrupt __BUFAR BuSystemIsr_Irq6(void);
void _interrupt __BUFAR BuSystemIsr_Irq7(void);
void _interrupt __BUFAR BuSystemIsr_Irq8(void);
void _interrupt __BUFAR BuSystemIsr_Irq9(void);
void _interrupt __BUFAR BuSystemIsr_Irq10(void);
void _interrupt __BUFAR BuSystemIsr_Irq11(void);
void _interrupt __BUFAR BuSystemIsr_Irq12(void);
void _interrupt __BUFAR BuSystemIsr_Irq13(void);
void _interrupt __BUFAR BuSystemIsr_Irq14(void);
void _interrupt __BUFAR BuSystemIsr_Irq15(void);
#endif

#endif  


/* system interrupt handler */
#ifndef _WIN32
void BuSystemIsr(int IrqLevel);
#endif


/* installs a hook to a system isr */
BuError_t BuInstallSysHandler(U16BIT(*Handler)(U16BIT status));

/* explicitly calls the Usr defined handler */
__BUEXTERN void __BUDECL BuTestUsrHandler( U16BIT value);

/* installs a hook to a user isr */
/* modified for WIN32            */ 
#ifdef _WIN32
__BUEXTERN BuError_t __BUDECL BuInstallUsrHandler( void (__BUFAR __BUDECL *Handler)(U16BIT status));  
#else  
__BUEXTERN BuError_t __BUDECL BuInstallUsrHandler( void (__BUFAR *Handler)(U16BIT status));  
#endif

/* installs a hook to a usr isr for can interrupts */
#ifdef _WIN32
BuError_t __BUDECL BuInstallCanHandler( void(__BUFAR __BUDECL *Handler)(U16BIT status));
#else
BuError_t __BUDECL BuInstallCanHandler( void(__BUFAR *Handler)(U16BIT status));
#endif

/* sets user defined pointer for user handler */
__BUEXTERN BuError_t __BUDECL BuSetUsrPtr(void *UsrPtr);

/* gets user defined pointer for user handler */
__BUEXTERN void* __BUDECL BuGetUsrPtr(void);


#ifdef __BUDEBUG_FUNCTIONS
/* sets auto clear irq on read of irq status register */
__BUEXTERN BuError_t __BUDECL BuIrqAutoClear( U8BIT Sel);

/* sets irq line for level or pulse irq signal */
__BUEXTERN BuError_t __BUDECL BuIrqType( U8BIT Sel);

/* allows the enable of interrupt conditions */
__BUEXTERN BuError_t __BUDECL BuIrqEnable( U16BIT Mask);

/* allows the disable of interrupt conditions */
__BUEXTERN BuError_t __BUDECL BuIrqDisable( U16BIT Mask);

/* get interrupt register status */
__BUEXTERN U16BIT __BUDECL BuGetIrqStatus( void);

/* resets ace INT* output to a logic 1 */
__BUEXTERN BuError_t __BUDECL BuIrqReset( void);

#else
#define BuIrqAutoClear(sel) BuWriteReg(CONFIG_2,\
        ((BuReadReg(CONFIG_2)&0xffef)|((sel)?0x0010:0)))
#define BuIrqType(sel) BuWriteReg(CONFIG_2,\
        ((BuReadReg(CONFIG_2)&0xfff7)|((sel)?0x0008:0)))
#define BuIrqEnable(msk) BuWriteReg(INTRPT_MASK,\
        (BuReadReg(INTRPT_MASK)|msk))
#define BuIrqDisable(msk) BuWriteReg(INTRPT_MASK,\
        (BuReadReg(INTRPT_MASK)&(~msk)))
#define BuGetIrqStatus() BuReadReg(INTRPT_STATUS)
#define BuIrqReset() BuWriteReg(CONTROL,0x0004)

#endif
/*
 * END
 * IRQ.H (INTERRUPT HANDLER AND CONTROL MODULE)
 */

#endif /*IRQ_H_INCLUDED */

