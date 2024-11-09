#ifndef ACEMEM_H_INCLUDED
#define ACEMEM_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * ACEMEM.H (ACE MEMORY MANAGMENT MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for ACEMEM.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    modified rel 1.6 stdace.h
 *
 *   2.1       01-JUN-1995    modified MemBlockStruct and took *addr
 *                            since it was redundant with absaddr
 *
 *                            added BuWriteBlk and BuReadBlk prototypes
 *
 *                            added BuAllocHandleBoundary prototype
 *
 *                            modified BuFindSpace prototype by adding the
 *                            boundary parameter
 *
 *                            'gaptime' field in mem block handle changed
 *
 *                            to 'gp', general purpose 16 bit word.
 *
 *                            changed Special memory block condition to
 *                            Permanent to be more descriptive and added
 *                            CreatePermanentMemBlock() function.
 *
 *   2.2       01-SEP-1995    commented out #ifdef __cplusplus since it
 *                            is in the stdace.h header file
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
 *   5.3.1     11-APR-2003 cf none
 */


/* ACEMEM CONSTANTS ------------------------------------------------------*/

/* memory block conditions */

#define UnUsed       0x00
#define Protected    0x01
#define Used         0x02
#define Active       0x04
#define OffBoard     0x08
#define CmdStack     0x10
#define Permanent    0x20 /* changed [01-JUN-1995 Special to Permanent */

/* DDC translation layer mod */
/*-------------------------- */
#define EmaceData	 0x40
#define EmaceMsg	 0x80
/* */
/* ACEMEM TYPE DEFINITIONS AND STRUCTURES --------------------------------*/

/* memory block handle structure */

typedef struct MemBlockStruct {
	U16BIT absaddr;
	U8BIT  status;
	U16BIT size;
	U16BIT gp; /* gaptime changed to gp "general purpose" [modified 01-JUN-1995] */
	U16BIT condition;
	struct MemBlockStruct *next,*prev;

	/* New Members added for BC Major Frame Support */
	U16BIT count;
	U16BIT blkstatus;
	U16BIT timetag;

	/* LynnSoft transition layer mod */
	/*------------------------------ */
	U16BIT mblk_id;
	U16BIT dblk_id;
	U16BIT wCtrlWrd;
	U16BIT wCmdWrd1;
	U16BIT wCmdWrd2;
	U16BIT pData[32];
	U16BIT wGapTime;
	U16BIT wCondition;
	U16BIT wWordCount;
	U16BIT dblk_type;
} *MemBlockHandle, MemBlockType;

/* ACEMEM FUNCTION PROTOTYPES ---------------------------------------------*/

/* get block absolute address from memory block handle */
#define BuGetBlkAddress(blk) (blk->absaddr)

/* get block absolute size from memory block handle */
#define BuGetBlkSize(blk) (blk->size)


__BUEXTERN BuError_t __BUDECL BuFreeMemBlockEx(MemBlockHandle hOldBlk,U8BIT cFreeType);
__BUEXTERN MemBlockHandle __BUDECL BuAllocMemBlockEx(U8BIT wType,
													 U16BIT wSize,
													 U16BIT wStartAddr,
													 U16BIT wBoundary,
													 U16BIT wCrossOver,
													 MemBlockHandle hStartBlk);

/* closes memory block list and frees all structures */
__BUEXTERN BuError_t __BUDECL BuCloseBlockList(void);

/* clears memory block list of everyting but protected blocks */
__BUEXTERN BuError_t __BUDECL BuClearBlockList(void);

/* initializes the memory block list */
__BUEXTERN BuError_t __BUDECL BuInitBlockList(void);

/* creates the handle for a memory block */
__BUEXTERN MemBlockHandle __BUDECL CreateMemBlockHandle(void);

/* removes memory block from list */
__BUEXTERN BuError_t __BUDECL MemBlockRemove(MemBlockHandle p);

/* inserts memory block before (at) */
__BUEXTERN BuError_t __BUDECL MemBlockInsert(MemBlockHandle at,MemBlockHandle msg);

/* swaps one memory block with another */
__BUEXTERN BuError_t __BUDECL SwapMemBlocks( MemBlockHandle p,
                                             MemBlockHandle q);

/* creates protected memory block at specific address and size */
__BUEXTERN MemBlockHandle __BUDECL CreateProtectedMemBlock( MemBlockHandle blk,
				                                                    U16BIT addr,
				                                                    U16BIT size);

/* creates permanent memory block at specific address and size */
__BUEXTERN MemBlockHandle __BUDECL CreatePermanentMemBlock( MemBlockHandle blk,
				                                                    U16BIT addr,
				                                                    U16BIT size);

/* allocates memory block within unused memory block (area) */
__BUEXTERN MemBlockHandle __BUDECL AllocateOnBoard( MemBlockHandle area,
                                                    U16BIT size);

/* allocates an off board memory block (host memory) */
__BUEXTERN MemBlockHandle __BUDECL AllocOffBoard( U16BIT size);

/* allocates memory block handle on boundary condition */
__BUEXTERN MemBlockHandle __BUDECL BuAllocHandleBoundary( U16BIT size,
                                                          U16BIT boundary);
/* allocates a memory block and returns the handle */
__BUEXTERN MemBlockHandle __BUDECL BuAllocHandle( U16BIT size);

/* frees an allocated memory block */
__BUEXTERN BuError_t __BUDECL BuReleaseHandle( MemBlockHandle p);

/* finds an unused memory block of >=size with boundry condition and
 * returns a handle [01-JUN-1995 added boundary parameter]
 */
__BUEXTERN MemBlockHandle __BUDECL BuFindSpace( U16BIT size,U16BIT boundary);

/* write data buffer to memory block */
__BUEXTERN BuError_t __BUDECL BuWriteBlk( MemBlockHandle BlockHdl,
                                          U16BIT *DataPtr,U16BIT Size);

/* reads memory block contents into data buffer */
__BUEXTERN BuError_t __BUDECL BuReadBlk( MemBlockHandle BlockHdl,
                                         U16BIT *DataPtr,U16BIT Size);

__BUEXTERN void __BUDECL BuListDump(void);

/*
 * END
 * ACEMEM.H (ACE MEMORY MANAGMENT MODULE)
 */
#endif /*ACEMEM_H_INCLUDED */

