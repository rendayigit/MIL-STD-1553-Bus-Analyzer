/*******************************************************************************
 * FILE: memmgr.h
 *
 * DESCRIPTION:
 *
 *  The purpose of this module is to define memory manager functions and 
 *  definitions.
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

#ifndef __MEMMGR_H__
#define __MEMMGR_H__

#include "internal.h"

/* Memory Word Count Calculation Macro - Device */
#define ADDR_UNITS_DEVICE(CARD_TYPE, MEM_BYTES) (CARD_TYPE & ACE_FLEX_DEVICE ? MEM_BYTES >> 2 : MEM_BYTES >> 1)

/* Memory Word Count Calculation Macro - Data */
#define ADDR_UNITS_DATA(CARD_TYPE, \
        DATA_WORDS) ((U16BIT)(CARD_TYPE & ACE_FLEX_DEVICE ? ((DATA_WORDS / 2) + (DATA_WORDS % 2)) : DATA_WORDS))

/* Memory Word To Byte Calculation Macro - Memory */
#define ADDR_UNITS_TO_BYTES(CARD_TYPE, ADDR_UNITS) (CARD_TYPE & \
        ACE_FLEX_DEVICE ? (ADDR_UNITS * sizeof(U32BIT)) : (ADDR_UNITS * sizeof(U16BIT)))

/* Memory Block Nodes can have the following nMemTypes */
#define ACE_MEMTYPE_FREE        0x0000  /* Free memory segment */
#define ACE_MEMTYPE_USED        0x0001  /* Used memory segment */
#define ACE_MEMTYPE_PERM        0x0020  /* Fixed/Used memory location */

/* Memory Block Nodes can have the following wBndry conditions */
#define ACE_MEMBNDRY_NONE       0x0000  /* No Boundary condition */
#define ACE_MEMBNDRY_2          0x0002  /* falls on 2 word boundary */
#define ACE_MEMBNDRY_4          0x0004  /* falls on 4 word boundary */
#define ACE_MEMBNDRY_8          0x0008  /* falls on 8 word boundary */
#define ACE_MEMBNDRY_16         0x0010  /* falls on 16 word boundary */
#define ACE_MEMBNDRY_32         0x0020  /* falls on 32 word boundary */
#define ACE_MEMBNDRY_64         0x0040  /* falls on 64 word boundary */
#define ACE_MEMBNDRY_128        0x0080  /* falls on 128 word boundary */
#define ACE_MEMBNDRY_256        0x0100  /* falls on 256 word boundary */
#define ACE_MEMBNDRY_512        0x0200  /* falls on 512 word boundary */
#define ACE_MEMBNDRY_1K         0x0400  /* falls on 1K word boundary */
#define ACE_MEMBNDRY_2K         0x0800  /* falls on 2K word boundary */
#define ACE_MEMBNDRY_4K         0x1000  /* falls on 4K word boundary */
#define ACE_MEMBNDRY_8K         0x2000  /* falls on 8K word boundary */
#define ACE_MEMBNDRY_16K        0x4000  /* falls on 16K word boundary */
#define ACE_MEMBNDRY_32K        0x8000  /* falls on 32K word boundary */
#define ACE_MEMBNDRY_64K        0x10000 /* falls on 64K double word boundary */
#define ACE_MEMBNDRY_128K       0x20000 /* falls on 128K double word boundary */
#define ACE_MEMBNDRY_256K       0x40000 /* falls on 256K double word boundary */
#define ACE_MEMBNDRY_512K       0x80000 /* falls on 512K double word boundary */

/* The following mem block IDs are reserved for internal use. No user
 * created memory block (opcode,data table,message, etc...) can have one
 * of these IDs. RESERVED 0xF000-0xFFFF
 */
#define ACE_MEMID_STK_PTRS      0xF000  /* Stack pointers mem block */
#define ACE_MEMID_MT_CSTKA      0xF100  /* MT command stack A */
#define ACE_MEMID_MT_CSTKB      0xF101  /* MT command stack B */
#define ACE_MEMID_MT_DSTKA      0xF102  /* MT data stack A */
#define ACE_MEMID_MT_DSTKB      0xF103  /* MT data stack B */
#define ACE_MEMID_MT_DBSW       0xF104  /* MT dummy block status word location for stack overflow condition USB SOM Purge */
#define ACE_MEMID_MT_LKUP       0xF110  /* MT Sel. Msg Monitor lkup tbl */
#define ACE_MEMID_RT_CSTKA      0xF200  /* RT command stack A */
#define ACE_MEMID_RT_CSTKB      0xF201  /* RT command stack B (not used)*/
#define ACE_MEMID_RT_MC_IRQ     0xF202  /* RT mode code irq table */
#define ACE_MEMID_RT_MC_DATA    0xF203  /* RT mode code data */
#define ACE_MEMID_RT_LKUPA      0xF210  /* RT lookup table A */
#define ACE_MEMID_RT_LKUPB      0xF211  /* RT lookup table B */
#define ACE_MEMID_RT_BUSY       0xF220  /* RT busy bit lookup table */
#define ACE_MEMID_RT_CMD_ILL    0xF221  /* RT cmd illegalizing table */
#define ACE_MEMID_BC_STKA       0xF300  /* BC stack A (Legacy) */
#define ACE_MEMID_BC_STKB       0xF301  /* BC stack B (Legacy) */
#define ACE_MEMID_BC_GPQ        0xF310  /* BC General Purpose Q (emace)*/
#define ACE_MEMID_BC_LPQ        0xF311  /* BC LP Async Msg Q (acex) */
#define ACE_MEMID_BC_HPQ        0xF312  /* BC HP Async Msg Q (acex) */
#define ACE_MEMID_BC_HALT       0xF313  /* BC Default Instruction Code (acex) */
#define ACE_MEMID_BC_SLOT_PARAM 0xF314  /* BC Time Slot Parameters (acex) */
#define ACE_MEMID_BC_SLOT_MAIN  0xF315  /* BC Time Slot Main Routine (acex) */
#define ACE_MEMID_BC_SLOT_RESET 0xF316  /* BC Time Slot Reset Routine (acex) */
#define ACE_MEMID_BC_SLOT_PTR   0xF317  /* BC Time Slot Pointers (acex) */
#define ACE_MEMID_BC_REPLAY     0xF318  /* BC Replay Data Area (acex) */
#define ACE_MEMID_RTMT_ISQ      0xF400  /* Interrupt status Q (emace)*/
#define ACE_MEMID_MTI_DATA      0xF500  /* MTI data storage area (flex) */

/* The following are mem block sizes for the different fixed structs */
#define ACE_MEMSZ_STK_PTRS      0x0008  /* Always 8 words */
#define ACE_MEMSZ_MT_LKUP       0x0080  /* Always 128 words */
#define ACE_MEMSZ_RT_MC_IRQ     0x0008  /* Always 8 words */
#define ACE_MEMSZ_RT_MC_DATA    0x0030  /* Always 48 words */
#define ACE_MEMSZ_RT_LKUP       0x0080  /* Always 128 words */
#define ACE_MEMSZ_RT_BUSY       0x0008  /* Always 8 words */
#define ACE_MEMSZ_RT_CMD_ILL    0x0100  /* Always 256 words */
#define ACE_MEMSZ_BC_GPQ        0x0040  /* BC GPQ 64 words long*/
#define ACE_MEMSZ_RTMT_ISQ      0x0040  /* Irq Q 64 words long*/
#define ACE_MEMSZ_MTI_DATA      aDevices[DevNum].dwMemLength  /* Always Half of Available Memory*/

/* The following are fixed locations in ACE memory */
#define ACE_MEMAD_BC_GPQ        0x0000  /* BC general purpose queue loc*/
#define ACE_MEMAD_RTMT_ISQ      0x0000  /* interrupt status queue loc*/
#define ACE_MEMAD_STK_PTRS      0x0100  /* Always starts here */
#define ACE_MEMAD_RT_CSTKA_PTR  0x0100  /* RT command stack A ptr */
#define ACE_MEMAD_RT_GBLCA_PTR  0x0101  /* RT global circ buf A ptr */
#define ACE_MEMAD_MT_CSTKA_PTR  0x0102  /* MT command stack A ptr */
#define ACE_MEMAD_MT_DSTKA_PTR  0x0103  /* MT data stack A ptr */
#define ACE_MEMAD_RT_CSTKB_PTR  0x0104  /* RT command stack B ptr */
#define ACE_MEMAD_RT_GBLCB_PTR  0x0105  /* RT global circ buf B ptr */
#define ACE_MEMAD_MT_CSTKB_PTR  0x0106  /* MT command stack B ptr */
#define ACE_MEMAD_MT_DSTKB_PTR  0x0107  /* MT data stack B ptr */
#define ACE_MEMAD_RT_MC_IRQ     0x0108  /* RT mode code sel irq table */
#define ACE_MEMAD_RT_MC_DATA    0x0110  /* RT mode code data */
#define ACE_MEMAD_RT_LKUPA      0x0140  /* RT Lookup table A */
#define ACE_MEMAD_RT_LKUPB      0x01C0  /* RT Lookup table B */
#define ACE_MEMAD_RT_BUSY       0x0240  /* RT busy bit lookup table */
#define ACE_MEMAD_MT_LKUP       0x0280  /* MT Sel msg lookup table */
#define ACE_MEMAD_RT_CMD_ILL    0x0300  /* RT cmd illegalizing table */

#define RT_BSW_OFFSET           0       /* Block Status Word */
#define RT_TT_OFFSET            1       /* TimeTag */
#define RT_DP_OFFSET            2       /* Data Pointer */
#define RT_CMD_OFFSET           3       /* Received CMD word */
#define RT_CMD_SIZE             4
#define RT_MSG_TYPE_OFFSET      2
#define RT_MSG_WC_OFFSET        2
#define RT_CMD_SIZE32           2

#define BC_BCW_OFFSET           0       /* BC Ctrl word offset */
#define BC_CMD1_OFFSET          1       /* CMD word 1 offset */
#define BC_DP_OFFSET            2       /* BC Data Pointer */
#define BC_T2N_OFFSET           3       /* Time to Next Msg */
#define BC_TT_OFFSET            4       /* BC TimeTag word */
#define BC_BSW_OFFSET           5       /* BC Block Status Word */
#define BC_LPK_OFFSET           6       /* BC Loopback word */
#define BC_RTSW_OFFSET          7       /* BC RT Status word */
#define BC_CMD2_OFFSET          8       /* CMD word 2 */
#define BC_RTSW2_OFFSET         9       /* 2nd RT Status word for RT-to-RT */
#define BC_CSBRT_SIZE           10
#define BC_CSB_SIZE             8       /* BC Message Control/Status Block */
#define BC_RT2RT_SIZE           10
#define BC_CSB_SIZE32           4
#define BC_CSBRT_SIZE32         5
#define BC_DUAL_MSG_SIZE        32
#define BC_DUAL_MSG_SIZE32      16
#define BC_RT2_MSG_SIZE         16
#define BC_NRM_MSG_SIZE         8
#define ACE_FRAME_OTHER_SIZE    8
#define ACE_FRAME_MAJOR_SIZE    10
#define FLEX_FRAME_MAJOR_DEC    9
#define FLEX_FRAME_MAJOR_SIZE   17
#define ASYNC_BOTH_SIZE         46
#define ASYNC_LMODE_SIZE        20
#define ASYNC_HMODE_SIZE30      30
#define ASYNC_HMODE_SIZE28      28
#define ASYNC_HMODE_SIZE18      18
#define ASYNC_HMODE_SIZE2       2
#define ASYNC_HMODE_SIZE4       4
#define BC_INST_INC             2       /* BC Instruction pointer increment */

#define BC_HALT                 0x1D4F

#define MT_BSW_OFFSET           RT_BSW_OFFSET           /* Block Status Word */
#define MT_TT_OFFSET            RT_TT_OFFSET            /* TimeTag */
#define MT_DP_OFFSET            RT_DP_OFFSET            /* Data Pointer */
#define MT_CMD_OFFSET           RT_CMD_OFFSET           /* Received CMD word */
#define MT_CMD_SIZE             RT_CMD_SIZE
#define MT_MSG_TYPE_OFFSET      RT_MSG_TYPE_OFFSET
#define MT_MSG_WC_OFFSET        RT_MSG_WC_OFFSET
#define MT_CMD_SIZE32           RT_CMD_SIZE32
#define MT_TT2_OFFSET           38
#define MT_TT3_OFFSET           39

/* 
 * Multi-RT specific: 
 *
 *  Each RT has Control Block in low memory and Data/CMD block in high memory areas
 *  due to 16bit and 19bit registers
 */
#define MRT_CB_BASE             0x00000000  /* MultiRT Control Block base offset    */
#define MRT_CB_SIZE             0x00001000  /* MultiRT Ctrl Block size for each RT  */
#define MRT_DB_BASE             0x00022000  /* MultiRT Data/CMD Stack base offset   */
#define MRT_DB_SIZE             0x0000E000  /* MultiRT Data/CMD size for each RT    */

/* _mmAllocMemBlock -> allocates a segment of memory for use */
S16BIT _DECL _mmAllocMemBlock
(
    MEMBLK_LIST *pMemBlkList,
    S16BIT nID,
    S16BIT nType,
    S16BIT nMemFlag,
    U32BIT dwSize,
    U32BIT dwStartAddr,
    U32BIT wBndry,
    DLISTNODE** ppNode
);

/* _mmFindMemBlock -> finds a memory block given its ID and type */
S16BIT _DECL _mmFindMemBlock
(
    MEMBLK_LIST *pMemBlkList,
    S16BIT nID,
    S16BIT nType,
    DLISTNODE** ppSearch
);

/* _mmFreeMemBlocksList -> frees up all memory blocks in memory manager */
S16BIT _DECL _mmFreeMemBlockList
(
    MEMBLK_LIST *pMemBlkList
);

/* _mmInitMemBlockList -> initializes the libraries memory manager */
S16BIT _DECL _mmInitMemBlockList
(
    MEMBLK_LIST *pMemBlkList
);

/* _mmReleaseMemBlock -> frees up a allocated (used) memory block */
S16BIT _DECL _mmReleaseMemBlock
(
    MEMBLK_LIST *pMemBlkList,
    S16BIT nID,
    S16BIT nType
);

/* _mmResetFreeMemBlockPtr -> sets the internal free mem block ptr to
 *      first free memory block in list.
 */
S16BIT _DECL _mmResetFreeMemBlockPtr
(
    MEMBLK_LIST *pMemBlkList
);

/* _mmReleaseModMemBlocks -> removes module-specific memory blocks */
S16BIT _DECL _mmReleaseModMemBlocks
(
    MEMBLK_LIST *pMemBlkList,
    U8BIT u8Mod
);

#endif /* ifndef __MEMMGR_H__ */
