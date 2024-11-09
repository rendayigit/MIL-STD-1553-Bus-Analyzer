#ifndef BUF_H_INCLUDED
#define BUF_H_INCLUDED
/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * BUF.H (BUFFER CONTROL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for BUF.H (HEADER FILE)
 *
 *   Release   Date           Description
 *
 *   2.0       01-MAY-1995    modified header file BUBUF.H from rel 1.6
 *                            to work with rel 2.0 BUF.C functions.
 *
 *   2.1       01-JUN-1995    none
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
 */

/* BUF CONSTANTS ----------------------------------------------------------*/

/* BufStruct.flags value*/
#define BUFFER_FULL     0x01

/* flags returned from some routines */
#define BUF_CLEAR       0x00               /* clear flags           */
#define BUF_NOBUFFERS   0x01               /* no buffers allocated  */
#define BUF_BUFFERFULL  0x02               /* buffer full condition */


/* BUF TYPE DEFINITIONS AND STRUCTURES ------------------------------------*/

/* each buffer has this structure to keep track of its status */
/* these structures are linked together */
typedef struct BufStruct {
    U16BIT *BufferBase;          /* pointer to first slot in buffer */
    U16BIT *Buffer;              /* pointer to next avaiable slot */
    U16BIT WordCount;            /* number of words in the buffer */
    U16BIT flags;                /* state of the buffer */
    struct BufStruct *Next;      /* link to next buffer */
} BufType, *BufPtrType;

/* structure containing data private to the module */
/* this is malloc'd in whatever module is using buf */
/* and should be free'd when that module is finished */
typedef struct BUFStruct {
	U16BIT BufAllocated;		/* was BuCreateRWBuffers run? */
	U16BIT NumBufs; 			/* number of buffers allocated */
	U32BIT BufSize;				/* size of each buffer */
	U32BIT BufFull; 			/* word count for a full buffer state */
	BufPtrType WriteBuffer;		/* pointer to current write buffer */
	BufPtrType ReadBuffer;		/* pointer to current read buffer */
} BUFType, *BUFPtr;


/* BUF FUNCTION PROTOTYPES ------------------------------------------------*/

/* allocated BUF module private data */
__BUEXTERN void __BUDECL BuBufOpen(void);

/* frees BUF module private data */
__BUEXTERN void __BUDECL BuBufClose(void);

/* creates the desired number of BufferSize word read write buffers */
__BUEXTERN U16BIT __BUDECL BuCreateRWBuffers(U16BIT NumBuffers,
											 U32BIT BufferSize,
											 U32BIT BufferFull);

/* frees all the buffers and associated structures */
__BUEXTERN void __BUDECL BuFreeRWBuffers(void);

/* determines if the current write buffer is full */
__BUEXTERN U16BIT __BUDECL BuWriteBufFull(void);

/* returns the pointer to the next avaiable write buffer slot */
__BUEXTERN U16BIT* __BUDECL BuGetWriteBufPtr(void);

/* adds count words to the pointers and counters of write buffer */
__BUEXTERN U16BIT __BUDECL BuUpdateWriteBuf(U32BIT count);

/* assigns Ptr the next avaiable slot to read data from */
__BUEXTERN U16BIT __BUDECL BuGetReadBufPtr(U16BIT **Ptr,
										   U16BIT *Words);

/* resets the read buffer to a clear condition avaiable for writing */
__BUEXTERN U16BIT __BUDECL BuResetReadBuf(void);


/*
 * END
 * BUF.H (BUFFER CONTROL MODULE)
 */

#endif /*BUF_H_INCLUDED */

