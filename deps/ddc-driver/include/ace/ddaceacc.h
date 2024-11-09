/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * ACE / EMA User Application Interface				   *
 * ------------------------------------				   *
 * Copyright (c) 2003 Data Device Corporation		   *
 * Portions Copyright (c) 1996-2003, BlueWater Systems *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * Library and Card Defines  * 
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* Ace Library Version */
#define ACE_LIB_MAJOR		4
#define ACE_LIB_MINOR		8

/* EMA Library Version */
#define EMA_LIB_MAJOR   	1
#define EMA_LIB_MINOR   	2

/* Card Types */
#define ACE_DRV_OTHER		0
#define ACE_DRV_ISA			1
#define ACE_DRV_PCI			2
#define ACE_DRV_PCCARD		3

/* HW Access */
#define ACE_REGIO			1
#define ACE_REGMEM			2

/* Card Models */
#define BU_OTHER			0
#define BU_600000645		1
#define BU_65550			2
#define BU_65539			3
#define BU_65550W			4
#define BU_WIN32			5
#define BU_65528			6
#define BU_65551			7
#define BU_65549			8
#define BU_65552	        9
#define BU_65553        	10
#define BU_65565			11
#define BU_65569			12
#define BU_PPRIME			13
#define BU_65568			14
#define BU_65566			15
#define BU_65580			16

/*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * Windows 9x Definitions  *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* Legacy IOCTL defines */
#define IOCTL_GET_VERSION				0x001
#define IOCTL_REG_WINDOW				0x102  
#define IOCTL_MEM_WINDOW				0x103  
#define IOCTL_IRQ_LEVEL					0x104
#define IOCTL_OPEN_CARD					0x107   
#define IOCTL_CLOSE_CARD				0x108
#define IOCTL_GET_STATUS				0x140
#define IOCTL_GET_DEVNODE				0x150
#define IOCTL_GET_SOCKET				0x160
#define IOCTL_WRITE_IOPORT				0x170
#define IOCTL_INSTALL_IRQEVENT			0x1A0
#define IOCTL_UNINST_IRQEVENT			0x1A1
#define IOCTL_IRQ_BLOCK					0x1A2
#define IOCTL_BC_AUTO_FLIPSTK			0x1B0

/* 2nd Gen IOCTL defines */
#define IOCTL_MP9X_GET_VERSION			0x001
#define IOCTL_ACE9X_GET_VERSION			0x001
#define IOCTL_ACE9X_GET_REGWIN			0x102
#define IOCTL_ACE9X_GET_MEMWIN			0x103
#define IOCTL_ACE9X_GET_IRQLVL			0x104
#define IOCTL_MP9X_GET_IRQ_LEVEL		0x104
#define IOCTL_ACE9X_OPEN_CARD			0x107
#define IOCTL_ACE9X_CLOSE_CARD			0x108
#define IOCTL_MP9X_GET_STATUS			0x140
#define IOCTL_ACE9X_GET_STATUS			0x140
#define IOCTL_MP9X_GET_DEVNODE			0x150
#define IOCTL_ACE9X_GET_DEVNODE			0x150
#define IOCTL_MP9X_GET_INFOREG			0x180
#define IOCTL_MP9X_GET_REGWIN			0x200
#define IOCTL_MP9X_GET_MEMWIN			0x201
#define IOCTL_MP9X_INSTALL_IRQEVENT		0x1A0
#define IOCTL_ACE9X_INSTALL_IRQEVENT	0x1A0
#define IOCTL_MP9X_UNINSTALL_IRQEVENT	0x1A1
#define IOCTL_ACE9X_UNINSTALL_IRQEVENT	0x1A1
#define IOCTL_MP9X_BLOCK_ON_IRQ			0x1A2
#define IOCTL_ACE9X_BLOCK_ON_IRQ		0x1A2
#define IOCTL_ACE9X_BC_FLIP_STACK		0x1B0

#ifdef _ACELINUX
#define METHOD_BUFFERED 0
#define FILE_ANY_ACCESS 0
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * Windows NT Definitions  *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_GETINFO		 *
 * -------------------		 *
 * Input:  none			     *
 * Output: ACEINFO structure *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_GETINFO				ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 0)
#define IOCTL_ACENT_GET_ACEINFO			ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 0)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_MAPTOUSERSPACE  *
 * -------------------		   *
 * Input:  none			       *
 * Output: none                *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_MAPTOUSERSPACE		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 1)
#define IOCTL_ACENT_MAP_USER_SPACE		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 1)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_UNMAPUSERSPACE  *
 * -------------------		   *
 * Input:  none			       *
 * Output: none                *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_UNMAPUSERSPACE		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 2)
#define IOCTL_ACENT_UNMAP_USER_SPACE	ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 2)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_BLOCKONIRQ  *
 * -------------------	   *
 * Input:  none			   *
 * Output: none            *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_BLOCKONIRQ    		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 3)
#define IOCTL_ACENT_BLOCK_ON_IRQ   		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 3)
#define IOCTL_MPNT_BLOCK_ON_IRQ			ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 3)


/*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_SIMULATEIRQ *
 * -------------------	   *
 * Input:  none			   *
 * Output: none            *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_SIMULATEIRQ   		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 4)
#define IOCTL_ACENT_SIMULATE_IRQ   		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 4)
#define IOCTL_MPNT_RELEASE_IRQ			ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 4)
#define IOCTL_ACENT_RELEASE_IRQ   		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 4)


/*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_SHUTDOWN  *
 * -------------------   *
 * Input:  none		     *
 * Output: none          *
 *-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_SHUTDOWN      		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 5)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_READREGISTER  *
 * -------------------	     *
 * Input:  none			     *
 * Output: none              *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_READREGISTER  		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 6)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_WRITEREGISTER *
 * -------------------	     *
 * Input:  none			     *
 * Output: none              *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_WRITEREGISTER 		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 7)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_WRITEIO							   *
 * -------------------                             *
 * Input:  USHORT - value to be written to IO port *
 * Output: none									   *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_WRITEIO       		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 8)
#define IOCTL_ACENT_WRITE_IO       		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 8)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_AUTOFLIPSTK											   *
 * -------------------												   *
 * Input:  USHORT(bool) - determines if driver should auto-flip stacks *
 * Output: none														   *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_AUTOFLIPSTK    		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 9)
#define IOCTL_ACENT_AUTO_FLIP_STK  		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 9)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_GET_CARDCOUNT 					   *
 * -------------------                             *
 * Input:  none									   *
 * Output: USHORT - number of cards in the system  *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_GET_CARDCOUNT  		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 10)
#define IOCTL_ACENT_GET_CARD_COUNT 		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 10)

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * IOCTL_ACENT_GET_INFOREG 					       *
 * -------------------                             *
 * Input:  none									   *
 * Output: USHORT - number of channels on the card *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define IOCTL_ACENT_GET_INFOREG  		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 16)

#define IOCTL_ACENT_GET_IRQLEVEL  		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 17)

#define IOCTL_ACENT_HALT_DMA			ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 18)

#define IOCTL_ACENT_SET_DMA_LENGTH		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 19)

/*
 * IOCTL_ACENT_USB_INT_ENABLE
 */
#define IOCTL_ACENT_USB_INT_ENABLE		ACENT_MAKE_IOCTL(ACENT_DEVICE_TYPE, 24)


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * Windows 2000 Definitions  *
 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* GUID Indentifier */
#ifdef DEFINE_GUID
	DEFINE_GUID(GUID_CLASS_ACEPCCRD, 0xB8FD1EBC, 0x0BFD, 0x4973, 0x9A, 0x61, 0x25, 0x8D, 0x72, 0xBD, 0xA6, 0x9F);
#endif

/*-*-*-*-*-*-*-*-*-*
 * Common Defines  *
 *-*-*-*-*-*-*-*-*-*/

/* IOCTL Creation */
#define ACENT_DEVICE_TYPE              0x8000
#define ACENT_MAKE_IOCTL(t,c)\
        (ULONG)CTL_CODE((t), 0x800+(c), METHOD_BUFFERED, FILE_ANY_ACCESS)

/* pack define */
#pragma pack(push, 4)

#ifdef _ACELINUX
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif

/* Mem Window */
typedef struct MEM_WINDOW
{
	ULONG dwBaseAddr;	/* Virtual base address of memory window */
	ULONG dwPhysAddr;	/* Physical base address of memory window */
	ULONG dwMemWinLen;	/* Size in bytes of the memory window */
	ULONG dwPages;		/* Number of 4K pages used by the memory window*/
	ULONG dwType;		/* IO = 0, MEM = 1 */
} MEM_WINDOW;


/* ACE Window */
typedef struct LEGACY_ACEMWIN
{
	ULONG dwBaseAddr;	/* Virtual base address of memory window */
	ULONG dwPhysAddr;	/* Physical base address of memory window */
	ULONG dwMemWinLen;	/* Size in bytes of the memory window */
	ULONG dwType;		/* IO = 0, MEM = 1 */
} LEGACY_ACEMWIN;


/* ACEINFO structure */
typedef struct _ACEINFO
{
	/* Physical Card Addr */
	ULONG	ulPhysicalAddress;
	ULONG	ulLength;	/* it's free, use as Flexcore DMA transfer length */
	ULONG	ulChannel;

	/* Memory Window */
	PVOID	pMappedWindow;
	LONG 	lWindowLength;

	/* Reg Window */
	PVOID	pMappedRegisters;
	LONG 	lRegisterLength;

	/* Interrupt */
	LONG 	lInterrupt;
 
	/* Version Info */
	UCHAR	cDriverMajor;
	UCHAR	cDriverMinor;
	UCHAR	cDriverType;
	LONG 	cDriverBuild;
	UCHAR   cLibMajor;
	UCHAR	cLibMinor;

	/* Options */
	ULONG	fOptions;
} LEGACY_ACEINFO, ACEINFO, *PACEINFO;

/* pack define */
#pragma pack(pop)

/* Reg/Mem sizes */
#define ACEPCI_REGSIZE		128
#define ACEPCI_4K			0x2000
#define ACEPCI_64K			0x20000


/* #define ACEPCIEX_DMA_LENGTH	0x10000    64x1024 = 64K */
#define ACEPCIEX_DMA_LENGTH	0x40000		/* 256x1024 */

/************************************************
 Flexcore DMA registers -low 16bit in BAR1
 1) set PCI BAR0 offset 
 2) set Transfer Size 
 3) set Host addr  & transfer direction bit
 4) Enable DMA  & Start DMA
************************************************/
#define FLEX_CFG2_REG		0x0008		/* i.e. ACE CFG 2, 02h */
#define FLEX_SR_REG			0x000C		/* 03h Start/Reset Register */
#define FLEX_IMR2_REG		0x0074		/* 1Dh  Interrupt Maks Register */
#define FLEX_DMA_CONFIG		0x0080		/* 20h Enable DMA transfer */
#define FLEX_HOST_ADDR_LOW	0x0090		/* 24h x 4 */
#define FLEX_HOST_ADDR_HI	0x0094		/* 25h x 4 */
#define FLEX_PCI_ADDR_LOW	0x0098		/* 26h x 4 */
#define FLEX_PCI_ADDR_HI	0x009C		/* 27h x 4 */
#define FLEX_DMA_SIZE		0x00A0		/* 28h DMA transfer size reg. max 32K */

/* without x4 */
#define REG_FLEX_PCI_ADDR_LOW	0x0026	/* 26h  */
#define REG_FLEX_PCI_ADDR_HI	0x0027	/* 27h  */

#define DMA_HOST_TO_PCI		0x0001		/* xfer direction: HOST to PCI */
#define DMA_PCI_TO_HOST		0x0000		/* xfer direction: PCI to Host */
#define DMA_ENABLE_MSK		0x0002		/* bit 1 of DMA_CONFIG */
#define DMA_START_MSK		0x0100		/* bit 8 of SR_REG */
#define DMA_IMR2_INT_MSK	0x0002		/* bit 1 IMR2 */

#define FLEX_READ_DATA_OFFSET	0x0000	/* PCI BAR offset for Read */
#define FLEX_WRITE_DATA_OFFSET	0x0000	/* PCI BAR offset for Write */

/* FLEXCORE, RTL, BAR0 -mem, BAR1 - Reg */
#define FLEX_MEM_LENGTH		0x00400000  
#define FLEX_REG_LENGTH		0x00001000
