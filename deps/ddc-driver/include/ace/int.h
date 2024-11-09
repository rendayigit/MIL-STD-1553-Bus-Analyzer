#ifndef INT_H_INCLUDED
#define INT_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * INT.H (HARDWARE INTERFACE MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for INT.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    used interface routines from rel 1.6 STDACE.C
 *
 *   2.1       01-JUN-1995    added __BUDEBUG_FUNCTIONS converts functions to
 *                            #define macros; __BUDEBUG_ERROR_CHECKING used
 *                            for general range/state/error checking
 *
 *   2.2       01-SEP-1995    fixed problem with BuWriteRam() when compiled
 *                            with __BUDEBUG_ERROR_CHECKING turned on. old
 *                            version was picking up an (ELSE) statement
 *                            in certain situations, an (ELSE) was added
 *                            to correct this problem
 *
 *   2.3       01-JAN-1996    none
 *
 *   2.4       01-MAY-1996    none
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       07-June-1996   replaced 'far' with __BUFAR in BuBlockRead and
 *                            BuBlockWrite; __BUFAR is defined based upon 
 *                            whether the application is compiled as a 16bit 
 *                            or a WIN32 application   
 *
 *   4.0       29-NOV-1997    Inserted function call constants for VxD 
 *							  DeviceIoControl calls.
 *
 *   5.3.1     11-APR-2003    none
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* INTERFACE CONSTANTS ----------------------------------------------------*/

/* register address offset */
#define INTRPT_MASK       0x00   /* interrupt mask reg (rd/wr)       */
#define CONFIG_1          0x01   /* configuration reg #1 (rd/wr)     */
#define CONFIG_2          0X02   /* configuration reg #2 (rd/wr)     */
#define CONTROL           0X03   /* ace control (start/reset) (wr)   */
#define CMD_STK           0X03   /* command stack pointer rd)        */
#define CNTRL_WORD        0X04   /* bc ctrl word / rt sacword (rd/wr)*/
#define TIMETAG           0X05   /* timetag register (rd/wr)         */
#define INTRPT_STATUS     0X06   /* interrupt status reg (rd/wr)     */
#define CONFIG_3          0X07   /* configuration reg #3 (rd/wr)     */
#define CONFIG_4          0X08   /* configuration reg #4 (rd/wr)     */
#define CONFIG_5          0X09   /* configuration reg #5 (rd/wr)     */
#define MT_DATA_STK       0X0A   /* monitor data stack addr (rd/wr)  */
#define BC_FT_REMAIN      0X0B   /* bc frame time remaining (rd)     */
#define BC_MSG_REMAIN     0X0C   /* bc message time remaining (rd)   */
#define BC_FRAME_TIME     0X0D   /* bc frame time (rd/wr)            */
#define RT_LAST_CMD       0X0D   /* rt last command (rd/wr)          */
#define MT_TRIGGER_WORD   0X0D   /* mt trigger word (rd/wr)          */
#define RT_STATUS_WORD    0X0E   /* rt status word register (rd)     */
#define RT_BIT_WORD       0X0F   /* rt bit word register (rd)        */
#define CONFIG_6          0X18   /* configuration reg #6 (rd/wr) M'  */
#define CONFIG_7	  0x19   /* configuration reg #7 (rd/wr) M'  */

#define MEM_ENABLE        0X18   /* BUS-600000645 memory enable (wr) */
#define MEM_PAGE          0x20   /* 65552 RAM page register          */
#define CARD_ID_REG		  0x1A0	 /*Card ID Register					 */
#define MEM_PAGE_EBR_80   0x1A2   /* 6558x MIL RAM page register      */
#define MEM_PAGE_MIL_80   0x1B0   /* 6558x MIL RAM page register      */
#define INT_80            0x1A4   /* 65580 Interrupt Stat register  */
#define REG_EBR_DIO1	  0x1A6   /* Discrete IO register for EBR PC/104 */
#define REG_EBR_DIO2	  0x1A8   /* Discrete IO register for EBR PC/104 */
#define HUB_80            0x1B2  /* 65580/81 HUB Select register  */
#define MEM_PAGE_68       0x101  /* 65568/7 RAM page register        */
#define INT_68            0x102  /* 65568/7 Interrupt Stat register  */
#define MSCI0			  0x180   /* MSCI Serial Address Register 0 EBR PC/104 */
#define MSCI1			  0x182   /* MSCI Serial Address Register 1 EBR PC/104 */
#define STORE_RT_ADDR0	  0x184   /* STORE RT Serial Address Register 0 EBR PC/104 */
#define STORE_RT_ADDR1	  0x186   /* STORE RT Serial Address Register 1 EBR PC/104 */
#define CAN_CTRL_0		  0x102   /* CANBus Control Register 0 EBR PC/104 */
#define CAN_CTRL_1		  0x122   /* CANBus Control Register 1 EBR PC/104 */
#define CAN_CTRL_2		  0x142   /* CANBus Control Register 2 EBR PC/104 */
#define CAN_CTRL_3		  0x162   /* CANBus Control Register 3 EBR PC/104 */
#define CAN_EBR_LOOP_CTRL  0x1AA   /* CANBus-to-EBR Loopback Control Register EBR PC/104 */
#define CAN_EBR_LOOP_READ  0x1AC   /* CANBus-to-EBR Loopback Read Back Register EBR PC/104 */

/* Case statement labels for DeviceIoControl calls */
/*#define ACE_GET_VERSION    0x01  
#define ACE_GET_REGADDR    0x02  
#define ACE_GET_MEMADDR    0x03  
#define ACE_GET_IRQLEVEL   0x04   
#define ACE_SET_IRQHANDLER 0x05   
#define ACE_GET_MEMLENGTH  0x06   
#define ACE_SIMULATE_IRQ   0x07
#define ACE_CLOSE_DEVICE   0x08
#define ACE_GET_STATUS	   0x09
#define ACE_SET_IOPORT     0x0A
#define ACE_SET_SAME_IRQHANDLER 0x0B*/

/* INTERFACE FUNCTION PROTOTYPES ------------------------------------------*/

/* write register,(offset) words */
__BUEXTERN BuError_t __BUDECL BuWriteReg( U16BIT offset,U16BIT data);

/* read register,(offset) words */
__BUEXTERN U16BIT __BUDECL BuReadReg( U16BIT offset);

/* read block from memory into word array Ptr[],(offset,length) words */
/* modified for WIN32, replaced far with __BUFAR                      */   
__BUEXTERN BuError_t __BUDECL BuBlockRead( U16BIT offset,
                                           U16BIT __BUFAR *Ptr,
                                           U16BIT length);

/* read block to memory from word array Ptr[],(offset,length) words */
/* modified for WIN32, replaced far with __BUFAR                    */   
__BUEXTERN BuError_t __BUDECL BuBlockWrite( U16BIT offset,
                                            U16BIT __BUFAR *Ptr,
                                            U16BIT length);

/* fill an area of memory with data,(offset,length) words */
__BUEXTERN BuError_t __BUDECL BuBlockFill( U16BIT offset,
                                           U16BIT data,
                                           U16BIT length);

/* memory copy routine re-written to support paged memory */
BuError_t __BUDECL BuMemCpy(U16BIT __BUFAR* src,U16BIT __BUFAR* dest,U16BIT size);

#ifdef __BUDEBUG_FUNCTIONS
/* write ram,(offset) words */
__BUEXTERN BuError_t __BUDECL BuWriteRam( U16BIT offset,U16BIT data);

/* read ram,(offset) words */
__BUEXTERN U16BIT __BUDECL BuReadRam( U16BIT offset);

#else
 #ifdef __BUDEBUG_ERROR_CHECKING
 /* #define BuWriteRam(ofs,dat) if(BuConf)*(BuConf->BuBaseMem+ofs)=dat
  * fixed 01-SEP-1995 problem with above BuWriteRam() was picking up
  * else statments when macro was before an else. fixed by adding an
  * else at the end
  */
 #define BuWriteRam(ofs,dat) if(BuConf)*(BuConf->BuBaseMem+ofs)=dat; else
 #define BuReadRam(ofs) ((BuConf)?(*(BuConf->BuBaseMem+ofs)):0)
 
 #else
 #define BuWriteRam(ofs,dat) *(BuConf->BuBaseMem+ofs)=dat
 #define BuReadRam(ofs) *(BuConf->BuBaseMem+ofs)
 #endif
#endif

/*
 * END
 * INT.H (HARDWARE INTERFACE MODULE)
 */
#endif /*INT_H_INCLUDED */

