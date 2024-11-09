/*
 * Ace BU-65549M2-300 Linux Device Driver
 * Device Driver (Header File)
 * Beta 1.0
 * 10/16/2000
 * MR
 * 
 */

#ifndef  _DDACE49_H_
#define  _DDACE49_H_

#ifdef __KERNEL__

/* We really should do a 'ddclinux' module here. */
#ifndef DECLARE_WAIT_QUEUE_HEAD
typedef  struct wait_queue *wait_queue_head_t;
#define init_waitqueue_head(head) (*(head)) = NULL

#endif /* ! DECLARE_WAIT_QUEUE_HEAD */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)

#define VMA_OFFSET(vma)  ((vma)->vm_offset)
#define pci_resource_start(dev, bar)	dev->base_address[bar]

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0))

#define VMA_OFFSET(vma)  ((vma)->vm_pgoff << PAGE_SHIFT)

#endif /* Kernel version <, >= 2.4.0 */

/* This is not the top level of the module.  No versioning. */
#define __NO_VERSION__

#endif	/* __KERNEL__ */

/* Name of the loadable module. */
#define MODULE_NAME	"ace49"

/* Length of module name string. */
#define DEV_NAME_LEN		8	/* sizeof("ace49xx'\0'") = 8 */ 

/* Driver version. */
#define	ACE49_VER			1
#define	ACE49_REV			0
#define	ACE49_REL			2
#define ACE49_VERSION		(ACE49_VER<<16) | (ACE49_REV<<8) | ACE49_REL

/* PCI vendor and card information. */
#define VEND_ID_DDC			0x4DDC
#define DEV_ID_65549			0x0000

/* Maximum number of cards supported. */
#define MAX_NUM_CARDS    16
#define MAX_NUM_DEVICES  32
#define MAX_NUM_CHANS    32

/* max number of channels per card */
#define CHANS_PER_CARD 2


/* Linux kernel driver reference number. */
#define MAGIC_NUM  0xDD

/* Channel declares */
#define ACE_MEM_WND  0
#define ACE_REG_WND  1

/* Number of windows 'mmap'ped to user space. */
#define MAX_USER_MEM_WINDOWS  2	/* Shared and FIFO windows only. */

/* Memory size of shared and FIFO windows, (Parameter for 'mmap' method). */
#define ACE_CHAN_MEM_LENGTH   0x20000	/* 128k bytes = 64k words */
#define ACE_CHAN_REG_LENGTH   0x01000     /* 128 bytes  = 64 words = <4k */

/* Case statement labels for DeviceIoControl calls */
#define IOCTL_ACE49_GET_VERSION		_IOR(MAGIC_NUM, 1, void *)
#define IOCTL_ACE49_GET_CARD_INFO		_IOR(MAGIC_NUM, 2, void *)
#define IOCTL_ACE49_GET_CHANNEL_INFO	_IOR(MAGIC_NUM, 3, void *)	
#define IOCTL_ACE49_BLOCK_IN_DRIVER       _IOR(MAGIC_NUM, 4, void *)
#define IOCTL_ACE49_BC_AUTO_FLIPSTK       _IOR(MAGIC_NUM, 5, void *)
#define IOCTL_ACE49_INSTALL_IRQ           _IO(MAGIC_NUM, 6)
#define IOCTL_ACE49_UNINSTALL_IRQ         _IO(MAGIC_NUM, 7)

/* Driver call return values. */
#define	STATUS_ACE49_SUCCESS					(int)0

/* Initialization problems.  These values are returned to the 'insmod' utility. */
/* It will indicate 'Device or resource busy' if initialization fails.	   */
#define	STATUS_ACE49_INIT_NO_PCI_BIOS			(int)-100
#define 	STATUS_ACE49_INIT_TOO_MANY_PCI_CARDS		(int)-102

/* General 'open' and 'close' problems. */
#define	STATUS_ACE49_DEVICE_NOT_EXIST			(int)-200
#define	STATUS_ACE49_CARD_ALREADY_OPEN			(int)-201
#define	STATUS_ACE49_CARD_NOT_OPEN				(int)-202
#define	STATUS_ACE49_CARD_IMPROPERLY_CLOSED		(int)-203
#define	STATUS_ACE49_PCICFG_REMAP_FAIL			(int)-204
#define       STATUS_ACE49_CHANNEL_NOT_AVAILABLE               (int)-205

/* 'ioctl' call problems. */
#define	STATUS_ACE49_IOCTL_NOT_IMPLEMENTED			(int)-210
#define	STATUS_ACE49_IOCTL_IRQ_ALREADY_INSTALLED		(int)-211
#define 	STATUS_ACE49_IOCTL_IRQ_NOT_INSTALLED		(int)-212
#define	STATUS_ACE49_IOCTL_REQUEST_IRQ_FAIL		(int)-213
#define	STATUS_ACE49_IOCTL_INTERRUPT_NOT_ENABLED		(int)-214

/* 'mmap' call problems. */
#define	STATUS_ACE49_MMAP_CARD_ALREADY_MMAPPED		(int)-220
#define	STATUS_ACE49_MMAP_SEQUENCE_ERROR			(int)-221
#define	STATUS_ACE49_MMAP_INVALID_WINDOW_SIZE		(int)-222
#define	STATUS_ACE49_MMAP_INVALID_PAGE_SIZE		(int)-223
#define	STATUS_ACE49_MMAP_BAD_ALIGNMENT			(int)-224
#define	STATUS_ACE49_MMAP_REMAP_FAIL			(int)-225

/* Defines and typedef's. */
#define DWORD unsigned long
#define DDC_HANDLE unsigned long
#define CRITICAL_SECTION unsigned long
#define WINAPI
#define APIENTRY

/* Card Access Variable Types */
typedef signed char S8;
typedef unsigned char U8;
typedef signed short int S16;
typedef unsigned short int U16;
typedef signed long int S32;
typedef unsigned long int U32;
#define TYPES

/* Ace Channel Structure */
typedef struct _ACE_CHANNEL_49
{
	U8   bCardNum;	   /* Card Number [0-3] */
	U8   bChanNum;	   /* Channel Number [0-1] */
	U32  dwMemWndUsrAddr;   /* Memory Windows in User space (1 per channel) */
	U32  dwRegWndUsrAddr;   /* Register Windows in User space (1 per channel) */  
	U16  wIntStatus;        /* Status Register of the interrupt */
	U8   bBlocked;	   /* Blocking status of driver */
	U8   bMmapWinNum;       /* mmapped window count */
	U8   bIsrEnabled;	   /* Status of driver ISR */
	U16  wIrq;		   /* Channel's IRQ */
	U8   bOpen;		   /* Open status flag for channels */
	U8   bAutoFlipStk;      /* BC Eof Auto Flip Stack */
} ACE_CHANNEL_49, *pACE_CHANNEL_49;

 /* Ace Card Structure */
typedef struct _ACE49_CARD 
{
	U8			bPciBusNum;		/* PCI bus number */
	U8			bPciDevNum;		/* PCI device (slot) number */
	U8			bPciFcnNum;		/* PCI function number */
	U16			wDeviceID;		/* DDC card type */
	S8			cDevFileCardRef;	/* card reference of device special file entry [a-d] */
	U32			dwChanMemLen;		/* mem size of each channel on card */
	U16			wIrq;			/* interrupt level */
	U8			bNumChnls;		/* number of channels on card */
	U32			dwMemWndPciAddr;	/* Base address of Memory Window in PCI space */
	U32			dwRegWndPciAddr;	/* Base address of Registry Window in PCI space */
	U32  			dwRegWndKrnAddr;     /* Base address of Registry Window in Kernel space */
	ACE_CHANNEL_49          Channel[2];          /* Channel Specific information */		
} ACE49_CARD, *pACE49_CARD;

#endif

