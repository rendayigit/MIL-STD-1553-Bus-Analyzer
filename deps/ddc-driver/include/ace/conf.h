#ifndef CONF_H_INCLUDED
#define CONF_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * CONF.H (CONFIGURATION MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for CONF.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    initial release
 *
 *   2.1       01-JUN-1995    added pointer to RT global data structure
 *                            to BuConf struct; added VectTestCnt to
 *                            BuConf struct for BuVectorTest()
 *
 *                            changed extended revision character to
 *                            extended minor number in BuGetRev()
 *
 *                            BuConf had corruption problems with previous
 *                            version, fixed with #define of _Windows if
 *                            _WINDOWS defined for compatibility with MSC
 *                            also fixed BuOldIsr in Conf structure, added
 *                            void to make prototype valid
 *
 *   2.2       01-SEP-1995    added void* UsrPtr to BuConf structure
 *                            this can be set/read back with BuSetUsrPtr
 *                            and BuGetUsrPtr...added for c++ 'this' pointer
 *                            compatibility in user defined isr
 *
 *                            The Filename and EnvVar char strings were
 *                            added to the BuConf structure to implement
 *                            an alternate filename and environment variable
 *                            optionally used in BuInit().
 *
 *                            The dynamic data pointers used in the different
 *                            modes of operation RT,MT,BC, and BUF were made
 *                            FAR for user access in not Microsoft compilation
 *                            of DLL's and huge memory model applications.
 *
 *                            Modified BuGetRev as Code parameter should be
 *                            U32BIT pointer not U8BIT
 *
 *   2.3       01-JAN-1996    Added external mapping #define constant
 *                            for use as an option in the RegType register
 *                            mapping BuConf field.
 *
 *                            Added BuRegBaseAbs to BuConf structure for
 *                            use as a 32 bit base register address.
 *
 *   2.4       01-MAY-1996    Added BU_65550W #define for a new card option.
 *                            This option is used with the BU-65550 in
 *                            Windows 95.
 *
 *                            The BU_65550 BuOpen() under windows allocated
 *                            a single global memory block with a single
 *                            selector. There were separate pointers for memory
 *                            and register access - though they were based off
 *                            the single selector. For Windows 95 support
 *                            two seperate global memory blocks and selectors
 *                            are allocated. The DDC_HANDLE RegSelector; was added
 *                            to the BuConf_t structure.
 *
 *
 *   2.5       09-OCT-1996    none
 *
 *   3.0       07-JUNE-1996   replaced 'far' with __BUFAR in BuConf_t structure
 *                            12 places, in BuInit, BuSetConf and BuOpen;
 *                            modified *BuOldIsr prototype definition for
 *                            WIN32; replaced 'far' with __BUFAR in BuInit,
 *                            BuSetConf and BuOpen
 *
 *   4.0	   29-NOV-1997	  Added case for BU-65528 (option number 6). Also
 *                            inserted CardInfo28 into BuConf structure to allow
 *                            multiple 28 cards with multpile aces to work.
 *                            Inserted BuAllocConf & BuDeallocConf to dynamically create and
 *							  destroy storage for BuConf structures.
 *
 *   5.3.1     11-APR-2003    Added new structure for advanced ISRs and for
 *                            EBR(BU-6558xCx) variables.
 *  5/27/04  KWH  Added support for BU-6558xCx EBR series PC/104 cards.
 *
 */

/* CONF CONSTANTS ---------------------------------------------------------*/

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

/* Register mapping options available with ACE software library */
#define BU_IOMAP        0
#define BU_MEMMAP       1
#define BU_EXTMAP       2 /*added external register mapping option [01-JAN-1996]*/
#define BU_REGVXI       3 /*added VXI register mapping option [24-JULY-1997]*/

/* interrupt type options available with ACE software library */
#define BU_PULSE        0
#define BU_LEVEL        1

/* Linux Conf Definitions */
#ifdef _ACELINUX
#define MAX_NUM_LOGDEVNUM   32

typedef struct _DEVICENODECONTEXT
{
	char *sNode;
	U32BIT dwChannel;
} DEVICENODECONTEXT;

typedef struct _CONF_INFO
{
	int bOpen;
	DEVICENODECONTEXT sDevice;
	DDC_HANDLE hDriver;
	U32BIT dwMinorNumber;
}CONF_INFO;

typedef struct _LINUXCONTEXT
{
	DDC_HANDLE hDriver;
	DDC_HANDLE hBlock;
	DDC_HANDLE hBlkEvt;
	DDC_HANDLE hExtEvt;
	pthread_t hThread;
	void *hThreadResult;
	DEVICENODECONTEXT sDevice;
} LINUXCONTEXT;

#endif

/* Win32 Conf Definitions */
#ifdef _WIN32

#define MAX_NUM_CARDS	8

/* CONF TYPE DEFINITIONS AND STRUCTURES -----------------------------------*/

typedef struct _IOCTLCONTEXT
{
	DWORD dwDevice;
	DWORD dwChannel;
} IOCTLCONTEXT;

typedef struct _ACE_MEM_WINDOW
{
	DWORD dwBaseAddr;
	DWORD dwPhysAddr;
	DWORD dwMemWinLen;
	DWORD dwType;
} ACE_MEM_WINDOW;

typedef struct _WIN32CONTEXT
{
	DDC_HANDLE hDriver;
	DDC_HANDLE hBlock;
	DDC_HANDLE hIsrMutex;
	DDC_HANDLE hBlkEvt;
	DDC_HANDLE hExtEvt;
	DDC_HANDLE hThread[2];
	DWORD dwThreadID[2];
	IOCTLCONTEXT sDevice;
} WIN32CONTEXT;

typedef struct _CONF_INFO
{
	BOOL bOpen;
	IOCTLCONTEXT sDevice;
	DDC_HANDLE hDriver;
}CONF_INFO;

PVOID iBuMapMemory(BuConf_p BuConf);

#endif

/* structure used to keep track of the context */
struct BuConfStruct
{
	U8BIT ConfDev;

#ifdef _WIN32
	WIN32CONTEXT *pContext;
		/*U32BIT dwDevInfo; */
	char *pServiceName;
#endif

#ifdef _ACELINUX
	LINUXCONTEXT *pContext;
#endif

	/* modified for WIN32, replaced 'far' with __BUFAR 2 places     */
	U8BIT  BuCardType;      	/* card type option                     */
	U32BIT BuBaseMemAbs;    	/* absolute base address     (N/A WIN95)*/
	U32BIT BuRegBaseAbs;    	/* absolute register address (N/A WIN95)*/
	U16BIT __BUFAR *BuBaseMem;  /* pointer to ace memory            */
	U16BIT __BUFAR *BuBaseReg;  /* pointer to ace registers(mem map)*/
	U16BIT BuIO;            	/* offset to register(i/o map) (Win95->IO data)*/
	U16BIT BuIrq;           	/* level of irq                         */
	U32BIT BuMemoryLength;  	/* size of ace memory(words)            */
	U8BIT  BuIsrEnabled;    	/* isr enable option                    */
	U8BIT  BuIrqType;       	/* level/pulse interrupt option         */
	U8BIT  BuRegType;       	/* register map option                  */
	U8BIT  BuBus;                /* PCI Bus Number */
	U8BIT  BuSlot;          	/* pcmcia slot installation  (N/A WIN95)*/
	U8BIT  BuIrqInstalled;  	/* is isr installed?                    */
	U16BIT IrqTestFlag;     	/* used in BuIrqTest                    */
	U16BIT VectTestCnt;     	/* used in BuVectorTest                 */

#if defined(_Windows) && !defined(_WIN32)
	DDC_HANDLE RegSelector;     /* global selector for mem mapped reg block*/
	DDC_HANDLE Selector;        /* global selector for windows       */
#endif

	MemBlockHandle AceMemory; /* Head of mem linked list, size is all mem*/
	MemBlockHandle AceListEnd;/* Last block in mem linked list */
	MemBlockHandle AceCurrent;/* Next available free block in linked list*/

	/* BuOldIsr must have correct prototype for given compiler*/
#if defined(__TURBOC__) & !defined(_WIN32)
	void interrupt far (*BuOldIsr)(void); /* old interrupt vector */
#endif
#if defined(_MSC_VER) & !defined(_WIN32)
	void (_interrupt __BUFAR *BuOldIsr)(void); /* old interrupt vector */
#endif
#ifdef _WIN32
	void (__BUFAR *BuOldIsr)(void); /* old interrupt vector not used in WIN32 */
#endif
#if (defined(_ACELINUX)) || (defined(VX_WORKS))
	int *BuOldIsr;
#endif

#ifndef _MSC_VER
	MTPtr  __BUFAR MT;    /* MT module private data structure  */
	BCPtr  __BUFAR BC;    /* BC module private data structure  */
	RTPtr  __BUFAR RT;    /* RT module private data structure  */
	BUFPtr __BUFAR BUF;   /* BUF module private data structure */
#else
	MTPtr  MT;    /* MT module private data structure  */
	BCPtr  BC;    /* BC module private data structure  */
	RTPtr  RT;    /* RT module private data structure  */
	BUFPtr BUF;   /* BUF module private data structure */
#endif

#if (defined(_ACELINUX)) || (defined(VX_WORKS))
	U8BIT bChardNum;
	U8BIT bChanNum;
#endif

	/* Pointers to advanced interrupt routines */
	void(__BUFAR *BuSystemHandler)(U16BIT status);		/* ptr system isr */
#ifdef _WIN32
	void(__BUFAR __BUDECL *BuUsrHandler)(U16BIT status);/* ptr to user defined isr (legacy)*/
#else
	void(__BUFAR *BuUsrHandler)(U16BIT status);/* ptr to user defined isr (legacy)*/
#endif
	U16BIT(__BUFAR *BuSysHandler)(U16BIT status);		/* ptr to system defined isr */

#if (defined(_ACELINUX)) || (defined(VX_WORKS))
	void* __BUFAR UsrPtr;
#else
	void __BUFAR *UsrPtr; 		  /* void pointer passed in BuSetUsrPtr,
						   * this can be read back with BuGetUsrPtr
						   * added for c++ this pointer compatibility in
						   * user defined isr [01-SEP-1995]
						   */

#endif

	char EnvVar[12];   /* User programmable environment variable and */
	char Filename[26]; /* file name added [01-SEP-1995] */

	U32BIT dwStatus;
	U8BIT  wErrorCheck; /* Used to determine if error checking is enabled */

	/* 65528 structure holds important info for device */
#ifdef __BU65528ENA
	BU28_CardInfo_t BU28Info;
#endif

    /* DDC ADDED */
    U8BIT  BuIrqAutoClear;  	/* autoclear interrupt option  */
    U8BIT  u81553AMC;           /* 1553A Mode Code option      */
};   /* End of BuConf Structure */

typedef struct BuConfStruct BuConf_t;

#define EMACE_ID_BANK_COUNT		32
#define EMACE_ID_BANK_SIZE		32
#define EMACE_ID_BANK_FULL		0xFFFFFFFFUL
#define EMACE_ID_MAX			1024

typedef struct _Ace2EmaceIdMapStruct
{
	U32BIT Bank[EMACE_ID_BANK_COUNT];
} Ace2EmaceIdPoolStruct;

typedef struct _Ace2EmaceStruct
{
	/* LynnSoft Translation Layer Mod */
	/*------------------------------- */
	U16BIT	ActiveStack;
	U16BIT	MjrIdA;
	U16BIT	MjrIdB;
	U16BIT	StopOnFrame;
	S16BIT	wMode;
	U16BIT	wIrqMask;
	U16BIT	StackType;
	U16BIT	*RTCMD;
	U16BIT	*RTDATA;
	U16BIT	StkPtr;
	U16BIT	MinorFrameActive;
	U16BIT	MajorFrameActive;
	U16BIT	satx[32];
	U16BIT	sarx[32];
	U16BIT	sabx[32];
	U16BIT	saall[32];
	U16BIT	bc_mblk_id;
	U16BIT	bc_dblk_id;
	U16BIT	rt_dblk_id;

	/* DDC Translation Layer Mod */
	/*-------------------------- */
	Ace2EmaceIdPoolStruct BcMsgIdPool;
	Ace2EmaceIdPoolStruct BcOpcodeIdPool;
	Ace2EmaceIdPoolStruct BcFrameIdPool;
	Ace2EmaceIdPoolStruct BcDataIdPool;
	Ace2EmaceIdPoolStruct RtDataIdPool;

	void(__BUFAR __BUDECL *advBuUsrHandler)(BuConf_p BuConf,U16BIT status);
} Ace2EmaceStruct;

typedef struct _ADV_INFO
{

#if (defined(_ACELINUX)) || (defined(VX_WORKS))
	void* __BUFAR UsrPtr;
	void* __BUFAR Ace2EmaceUsrPtr;
#else
	void __BUFAR *UsrPtr;
	void __BUFAR *Ace2EmaceUsrPtr;
#endif
	U16BIT BuRegOffset;
	U16BIT BuChannel;
	U16BIT BuCurrentChnl;
	U16BIT BuCurrentPage;
	S16BIT BuEbrMode;

	void(__BUFAR *advBuSystemHandler)(BuConf_p BuConf,U16BIT status);		/* ptr system isr */
	U16BIT(__BUFAR *advBuSysHandler)(BuConf_p BuConf,U16BIT status);		/* ptr to system defined isr */

#ifdef _WIN32
	void(__BUFAR __BUDECL *advBuUsrHandler)(BuConf_p BuConf,U16BIT status);/* ptr to user defined isr */
#else
	void(__BUFAR *advBuUsrHandler)(BuConf_p BuConf,U16BIT status);/* ptr to user defined isr */
#endif

#ifdef _WIN32
	void(__BUFAR __BUDECL *advBuCanHandler)(BuConf_p BuConf,U16BIT status);/* ptr to user defined Can Bus isr */
#else
	void(__BUFAR *advBuCanHandler)(BuConf_p BuConf,U16BIT status);/* ptr to user defined Can Bus isr */
#endif
}ADV_INFO;

/* CONF FUNCTION PROTOTYPES -----------------------------------------------*/

/* sets the logical device number in a BuConf structure [05-AUG-98] */
__BUEXTERN BuError_t __BUDECL BuSetCardNum(BuConf_p Conf, U8BIT card_num);

/* retrieves string value from keyword in a .cfg file [30-JULY-97] */
__BUEXTERN char* __BUDECL BuGetValue(FILE *in, const char *str);

/* dynamically (de)allocates storage for a Conf structure */
__BUEXTERN BuConf_p __BUDECL BuAllocConf(void);
__BUEXTERN BuError_t __BUDECL BuDeallocConf(BuConf_p Conf);

/* loads .cfg file into Conf structure and executes BuOpen(Conf) */
/* modified for WIN32, replaced 'far' with __BUFAR */
__BUEXTERN BuError_t __BUDECL BuInit( char *fn,BuConf_t __BUFAR * Conf);

/* stores active BuConf structure in text file */
__BUEXTERN BuError_t __BUDECL BuStoreConf( char *fn);

/* sets the library configuration ptr BuConf */
/* modified for WIN32, replaced 'far' with __BUFAR */
__BUEXTERN BuError_t __BUDECL BuSetConf( BuConf_t __BUFAR * NewBuConf);

/* enables/disables ACE memory (for 65529 card)*/
__BUEXTERN BuError_t __BUDECL BuRam29( U8BIT Sel);

/* enables/disables ACE memory (for 65539 card)*/
__BUEXTERN BuError_t __BUDECL BuRam39( U8BIT Sel);

/* enables/disables ACE memory (for 65568 card)*/
__BUEXTERN BuError_t __BUDECL BuRam68( U8BIT Sel);

/* enables/disables ACE memory (for 65580 card)*/
__BUEXTERN BuError_t __BUDECL BuRam80( U8BIT Sel);

/* enables/disables Interrupts (for 65580 card)*/
__BUEXTERN BuError_t __BUDECL BuInt80( U16BIT Sel);	/* Dead, Right?  kwh */

/* enables/disables SouthBridge mode (for 65580 card)*/
__BUEXTERN BuError_t __BUDECL BuSouthBridge( U8BIT Sel);

/* opens library and makes Conf active */
/* modified for WIN32, replaced 'far' with __BUFAR */
/* modified for Linux, added Linux path */
__BUEXTERN BuError_t __BUDECL BuOpen( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL BuOpenWNT( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL BuOpenW95( BuConf_t __BUFAR * Conf);
__BUEXTERN BuError_t __BUDECL BuOpenLinux( BuConf_t __BUFAR * Conf);

/* returns text revision info on ACE library */
__BUEXTERN char* __BUDECL BuRev( void);

__BUEXTERN BuError_t __BUDECL BuDumpMem(char *fn);

/* returns number CORE revision info on ACE library */
__BUEXTERN void __BUDECL BuGetCoreRev( U8BIT *Major,U8BIT *Minor,U8BIT *XMinor);

/* returns number revision info on ACE library */
__BUEXTERN void __BUDECL BuGetRev( U8BIT *Major,U8BIT *Minor,U8BIT *XMinor,U32BIT *Code);

/* Sets the channel number for PC/104 cards */
__BUEXTERN BuError_t __BUDECL BuSetChannel(U16BIT wChannel);

/* set the current hub address of BU-6558xCx device */
__BUEXTERN BuError_t __BUDECL BuSetEbrHubAddress(U8BIT HubPort);

/* CANBus to EBR loop back test */
__BUEXTERN BuError_t __BUDECL BuCanToEBRLoopTest(U8BIT HubPort, U16BIT DataWrite, U16BIT *DataRead);

/* Linux functions */
#ifdef _ACELINUX
__BUEXTERN char* __BUDECL _lnxFindLinuxDevice(U32BIT dwBusNum,U32BIT dwDevNum,U32BIT dwChanNum);
#endif

/* Parametric Checking enable */
__BUEXTERN BuError_t __BUDECL BuParametricChecking( U8BIT Sel);

/* closes library at active Conf */
__BUEXTERN BuError_t __BUDECL BuClose( void);
__BUEXTERN BuError_t __BUDECL BuCloseW95( void);
__BUEXTERN BuError_t __BUDECL BuCloseWNT( void);
__BUEXTERN BuError_t __BUDECL BuCloseLinux( void);

#ifdef _WIN32
__BUEXTERN U32BIT __BUDECL Write39IO_95(U16BIT wCtrl);
__BUEXTERN U32BIT __BUDECL Write39IO_NT(U16BIT wCtrl);

/* opens library and makes Conf active  for Win32 bit*/
__BUEXTERN BuError_t __BUDECL BuOpen32(BuConf_t *Conf);

/* determines whether the Win32 platform is Windows 95 or NT */
S8BIT __BUDECL advBuWin32Platform(void); /* Void for CVI support*/

__BUEXTERN U16BIT __BUDECL BuGetPCCardStatus(U8BIT card_num);
#endif
__BUEXTERN BuError_t __BUDECL advBuAllocEmaceId(Ace2EmaceIdPoolStruct *pPool, U16BIT *pId);
__BUEXTERN BuError_t __BUDECL advBuDeallocEmaceId(Ace2EmaceIdPoolStruct *pPool, U16BIT Id);


/*
 * END
 * CONF.H (CONFIGURATION MODULE)
 */
#endif /*CONF_H_INCLUDED */

