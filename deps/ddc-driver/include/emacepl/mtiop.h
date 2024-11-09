/******************************************************************************
 * Data Device Corporation
 *
 * Filename: mtiop.h
 *
 *
 * Copyright (c) 2009 Data Device Corporation
 ******************************************************************************
 * Non-Disclosure Statement
 * ------------------------
 * This software is the sole property of Data Device Corporation. All
 * rights, title, ownership, or other interests in the software remain
 * the property of Data Device Corporation. This software may be used
 * in accordance with applicable licenses. Any unauthorized use,
 * duplication, transmission, distribution, or disclosure is expressly
 * forbidden.
 *
 * This non-disclosure statement may not be removed or modified without*
 * prior written consent of Data Device Corporation.
 ******************************************************************************
 * Module Description:
 *      ENHANCED MINI-ACE 'C' Run Time Library
 *
 *
 *  Monitor Terminal improvements operations header file.
 *
 *****************************************************************************/

#ifndef __MTIOP_H__
#define __MTIOP_H__

/* Chapter 10 Packet Header Size */
#define MTI_CH10_PKT_HEADER_SIZE    24

/* Device Memory Buffer Sizes in Bytes*/
#define MTI_DEVBUF_SIZE_4K      0x001000
#define MTI_DEVBUF_SIZE_8K      0x002000
#define MTI_DEVBUF_SIZE_16K     0x004000
#define MTI_DEVBUF_SIZE_32K     0x008000
#define MTI_DEVBUF_SIZE_64K     0x010000
#define MTI_DEVBUF_SIZE_128K    0x020000
#define MTI_DEVBUF_SIZE_256K    0x040000
#define MTI_DEVBUF_SIZE_512K    0x080000
#define MTI_DEVBUF_SIZE_1M      0x100000

/* Interrupt Condition Masks */
#define MTI_OVERFLOW_INT       0x020     /*  interrupt host on overflow of unified cmd/data
                                             stack */
#define MTI_HOST_INT           0x010     /* asynchronous interrupt event forced by host  */
#define MTI_TIME_MSG_TRIG_INT  0x008     /* interrupt on time reached, triggered by msg  */
#define MTI_TIME_INT           0x004     /* interrupt on time period                     */
#define MTI_NUM_MSGS           0x002     /* interrupt on number of messages reached      */
#define MTI_NUM_WORDS          0x001     /* interrupt on number of words reached         */

/* Interrupt Mask Register #3 (IMR3) Bits */
#define IMR3_MTI_NUM_WORDS      0x0001
#define IMR3_MTI_NUM_MSGS       0x0002
#define IMR3_MTI_TIME           0x0004
#define IMR3_MTI_MSG_TIME       0x0008
#define IMR3_MTI_HOST           0x0010
#define IMR3_MTI_OVR            0x0020

/* Chapter 10 time packet IRIG formats */
#define IRIG_FMT_MASK           0x0016L
#define IRIG_FMT_NONE           0x0000L
#define IRIG_FMT_1              0x0002L
#define IRIG_FMT_2              0x0004L
#define IRIG_FMT_3              0x0006L
#define IRIG_FMT_4              0x0008L

/* Chapter 10 time packet enable mask */
#define MTI_CH10_TIME_PKT_MASK      0x20000
#define MTI_CH10_TIME_PKT_ENABLE    0x20000
#define MTI_CH10_TIME_PKT_DISABLE   0x00000

/* Chapter 10 time packet IRIG enable */
#define MTI_CH10_TIME_PKT_IRIG_MASK     0x1
#define MTI_CH10_TIME_PKT_IRIG_DISABLE  0x0
#define MTI_CH10_TIME_PKT_IRIG_ENABLE   0x1

/* MTI Configure parameter range definitions */
#define MTI_BUFFER_COUNT_MIN        4
#define MTI_BUFFER_COUNT_MAX        32

#define MTI_BUFFER_SIZE_MIN         0x1000
#define MTI_BUFFER_SIZE_MAX         0x80000

#define MTI_INTERRUPT_TIME_MIN      10       /* 1ms Minimum Interrupt Interval */
#define MTI_INTERRUPT_TIME_MAX      65535  /* 6.5535s Maximun Interrupt Interval */

#define MTI_INTERRUPT_MSG_MIN       1
#define MTI_INTERRUPT_MSG_MAX       1000     /* Based On Max Buffer Size */

#define MTI_INTERRUPT_WORDS_MIN     (MTI_BUFFER_SIZE_MIN / 2) - 40 /* Subtract 40 Words For Potential In-Process Message */
#define MTI_INTERRUPT_WORDS_MAX     (MTI_BUFFER_SIZE_MAX / 2) - 40 /* Subtract 40 Words For Potential In-Process Message */

#define IRIG_CH10_PKT_SYNC_PATT                 0xEB25
#define IRIG_CH10_PKT_MSG_COUNT_MASK            0x007FFFFF
#define IRIG_CH10_PKT_CHNL_SP_DATA_WORD_SIZE    2

#define MTI_FORMAT_ERR_NO_RESP_MASK             (ACE_MT_BSW_NORES | ACE_MT_BSW_FMTERR)
#define MTI_STS2_WORD_MASK                      (ACE_MT_BSW_GDATA | ACE_MT_BSW_RTRT)
#define MTI_ERR_STS2_FORMAT_ERR_NO_RESP_MASK    (MTI_FORMAT_ERR_NO_RESP_MASK | MTI_STS2_WORD_MASK | ACE_MT_BSW_ERRFLG)

/* MTI CH10 File Access */
#define MTI_CH10_FILE_READ                  0
#define MTI_CH10_FILE_WRITE                 1

/* MTI CH10 File Read Control */
#define MTI_CH10_FILE_READ_CURRENT_PACKET   0
#define MTI_CH10_FILE_READ_NEXT_PACKET      1

/*-----------------------------------------------------------------------
Struct: MTI_CONFIG

Description:
    This structure describes MTI Mode Configuration.
------------------------------------------------------------------------*/
typedef struct MTI_CONFIG
{
    U32BIT u32DevBufByteSize;
    U32BIT u32DevBufWordAddr;
    U32BIT u32NumBufBlks;
    U32BIT u32BufBlkByteSize;
    BOOLEAN fZeroCopyEnable;
    U32BIT u32IrqDataLen;
    U32BIT u32IrqMsgCnt;
    U16BIT u16IrqTimeInterval;
    U32BIT u32IntConditions;
    U16BIT u16Ch10ChnlId;
    U8BIT u8HdrVer;
    U8BIT u8RelAbsTime;
    U8BIT u8Ch10Checksum;
} MTI_CONFIG, *PMTI_CONFIG;

#define MTI_CH10_PKT_HDR_SIZE       24
#define MTI_CH10_PKT_SEC_HDR_SIZE   12
#define MTI_CH10_PKT_HDR_SYNC       0xEB25

#define MTI_CH10_PKT_CH_SPEC_DATA_SIZE  4

#define MTI_CH10_1553_MSG_FMT1_HDR_SIZE 14

/* DDC chapter 10 packet data type definitions */
#define ACE_MTI_CH10_DATA_TYPE_TMATS        0x1
#define ACE_MTI_CH10_DATA_TYPE_1553_FMT1    0x19 /* CH10 1553 Format 1 */
#define ACE_MTI_CH10_DATA_TYPE_1553_MTI     0xF1 /* DDC MTI 1553 */
#define ACE_MTI_CH10_DATA_TYPE_1553_MEC     0xF2 /* DDC MEC (Message Error Capture */
#define ACE_MTI_CH10_DATA_TYPE_1553_AES     0xF3 /* DDC AES (Advanced Error Sampling */

/* Time tag resolution default TMATS file tag (0 = 2us) */
#define MTI_FILE_HEADER_TTRES_2US       "V-1\\DDC\\TTRES:0"
#define MTI_FILE_HEADER_TTRES_4US       "V-1\\DDC\\TTRES:1"
#define MTI_FILE_HEADER_TTRES_8US       "V-1\\DDC\\TTRES:2"
#define MTI_FILE_HEADER_TTRES_16US      "V-1\\DDC\\TTRES:3"
#define MTI_FILE_HEADER_TTRES_32US      "V-1\\DDC\\TTRES:4"
#define MTI_FILE_HEADER_TTRES_64US      "V-1\\DDC\\TTRES:5"
#define MTI_FILE_HEADER_TTRES_TEST      "V-1\\DDC\\TTRES:6"
#define MTI_FILE_HEADER_TTRES_EXT       "V-1\\DDC\\TTRES:7"
#define MTI_FILE_HEADER_TTRES_1US       "V-1\\DDC\\TTRES:8"
#define MTI_FILE_HEADER_TTRES_500NS     "V-1\\DDC\\TTRES:9"
#define MTI_FILE_HEADER_TTRES_100NS     "V-1\\DDC\\TTRES:10"

/*-----------------------------------------------------------------------
                    pack from this point
------------------------------------------------------------------------*/
#if defined (WIN32) || defined (VX_WORKS) || defined (INTEGRITY)
#pragma pack(1)
#endif /* (WIN32) || (VX_WORKS) || defined (INTEGRITY) */

/*-----------------------------------------------------------------------
Struct: MTI_CH10_PKT_HEADER

Description:
    This structure describes MTI CH10 Packet Header.
------------------------------------------------------------------------*/
typedef struct _MTI_CH10_PKT_HEADER
{
    /* Header, 24 bytes - MTI_CH10_PKT_HEADER_SIZE */
    U16BIT u16PktSyncPattern;
    U16BIT u16ChannelId;
    U32BIT u32PktLength;
    U32BIT u32DataLength;
    U16BIT u16SeqNumHdrVer;
    U16BIT u16DatTypePktFlags;
    U16BIT u16RelativeTimeCntr[3];
    U16BIT u16HeaderChksum;
}

#ifdef linux
__attribute__ ((packed))
#endif /* linux */
MTI_CH10_PKT_HEADER, *PMTI_CH10_PKT_HEADER;

/*-----------------------------------------------------------------------
Struct: MTI_CH10_DATA_PKT

Description:
    This structure describes MTI CH10 Data Packet.
------------------------------------------------------------------------*/
typedef struct _MTI_CH10_DATA_PKT /* Header, 24 bytes - MTI_CH10_PKT_HEADER_SIZE */
{
 
 U16BIT u16PktSyncPattern;
 U16BIT u16ChannelId;
 U32BIT u32PktLength;
 U32BIT u32DataLength;
 U16BIT u16SeqNumHdrVer;
 U16BIT u16DatTypePktFlags;
 U16BIT u16RelativeTimeCntr[3];
 U16BIT u16HeaderChksum;
 U32BIT u32ChnlSpecificData;

 /* Packet Body - filled in by hardware */
 U16BIT u16MsgData[1]; 
 }
#ifdef linux
__attribute__ ((packed))
#endif /* linux */
MTI_CH10_DATA_PKT, *PMTI_CH10_DATA_PKT;

#define GET_MTI_CH10_PKT_MSG_DATA_POINTER(pPkt)   (((U16BIT*)pPkt) + (U16BIT)((sizeof(MTI_CH10_DATA_PKT) - sizeof(U16BIT))>>1))
#define GET_MTI_CH10_PKT_TIME_CNTR_POINTER(pPkt)  (((U16BIT*)pPkt) + (U16BIT)(((4*sizeof(U16BIT)) + (2*sizeof(U32BIT)))>>1))

/*-----------------------------------------------------------------------
Struct: MTI_CH10_TIME_PKT

Description:
    This structure describes MTI CH10 Time Packet.
------------------------------------------------------------------------*/
typedef struct _MTI_CH10_TIME_PKT
{
    /* Header, 24 bytes - MTI_CH10_PKT_HEADER_SIZE */
    U16BIT u16PktSyncPattern;
    U16BIT u16ChannelId;
    U32BIT u32PktLength;
    U32BIT u32DataLength;
    U16BIT u16SeqNumHdrVer;
    U16BIT u16DatTypePktFlags;
    U16BIT u16RelativeTimeCntr[3];
    U16BIT u16HeaderChksum;
    U32BIT u32ChnlSpecificData;

    /* Packet Body - filled in by hardware */
    U64BIT ullGlobalTime;
}

#ifdef linux
__attribute__ ((packed))
#endif /* linux */
MTI_CH10_TIME_PKT, *PMTI_CH10_TIME_PKT;

#define GET_MTI_CH10_TIME_PKT_GLOBAL_TIME_POINTER(pPkt) (((U16BIT*)pPkt) + ((sizeof(MTI_CH10_TIME_PKT) - sizeof(U64BIT)) >> 1))
#define GET_MTI_CH10_TIME_PKT_TIME_CNTR_POINTER(pPkt)   (((U16BIT*)pPkt) + (U16BIT)((4*sizeof(U16BIT) + 2*sizeof(U16BIT))>> 1))

typedef struct _MTI_CH10_1553_MSG_FMT1
{
    /* Header, 24 bytes - MTI_CH10_PKT_HEADER_SIZE */
    U32BIT u32IntraPktTimeStamp[2];
    U16BIT u16IntraPktHdrWord;
    U8BIT u8GapTime1;
    U8BIT u8GapTime2;
    U16BIT u32MsgLength;
    U16BIT u16MsgData[1];
}

#ifdef linux
__attribute__ ((packed))
#endif /* linux */
MTI_CH10_1553_MSG_FMT1, *PMTI_CH10_1553_MSG_FMT1;

/*-----------------------------------------------------------------------
Struct: MTI_DEF_FILE_HEADER

Description:
    This structure describes MTI capture default file header.
------------------------------------------------------------------------*/
typedef struct _MTI_DEF_FILE_HEADER
{
    U16BIT u16PktSyncPattern;
    U16BIT u16ChannelId;
    U32BIT u32PktLength;
    U32BIT u32DataLength;
    U16BIT u16SeqNumHdrVer;
    U16BIT u16DatTypePktFlags;
    U16BIT u16RelativeTimeCntr[3];
    U16BIT u16HeaderChksum;
    U32BIT u32ChnlSpecificData;
    char szTagTimeTagRes[20];
}

#ifdef linux
__attribute__ ((packed))
#endif /* linux */
MTI_DEF_FILE_HEADER, *PMTI_DEF_FILE_HEADER;

/*-----------------------------------------------------------------------
  unpack from this point
------------------------------------------------------------------------*/
#if defined (WIN32) || defined (VX_WORKS) || defined (INTEGRITY)
#pragma pack()
#endif /* (WIN32) || (VX_WORKS) || defined (INTEGRITY) */

/**************** MTI FUNCTIONS **************************/
_EXTERN S16BIT _DECL _mtiSetDefaultConfig
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTISetExternalClk
(
    S16BIT DevNum,
    U16BIT wTTRes,
    U16BIT wExtCtrl
);

_EXTERN S16BIT _DECL _aceMTIStop
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTIPause
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTIContinue
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTIStart
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTIInitiateHostIrq
(
    S16BIT DevNum
);

_EXTERN S16BIT _DECL _aceMTIConfigure
(
    S16BIT DevNum,
    U32BIT u32DevBufByteSize,
    U32BIT u32NumBufBlks,
    U32BIT u32BufBlkByteSize,
    BOOLEAN fZeroCopyEnable,
    U32BIT u32IrqDataLen,
    U32BIT u32IrqMsgCnt,
    U16BIT u16IrqTimeInterval,
    U32BIT u32IntConditions,
    U16BIT u16Ch10ChnlId,
    U8BIT u8HdrVer,
    U8BIT u8RelAbsTime,
    U8BIT u8Ch10Checksum,
    U32BIT dwOptions
);

_EXTERN S16BIT _DECL _aceMTIGetCh10DataPkt
(
    S16BIT DevNum,
    PMTI_CH10_DATA_PKT *ppCh10Pkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL _aceMTIGetCh10TimePkt
(
    S16BIT DevNum,
    PMTI_CH10_TIME_PKT *ppCh10Pkt,
    S16BIT Timeout
);

_EXTERN S16BIT _DECL _aceMTICh10TimeFmt
(
    S16BIT DevNum,
    U16BIT Format
);

_EXTERN S16BIT _DECL _aceMTICh10TimePktEnable
(
    S16BIT DevNum,
    BOOLEAN bEnable
);

_EXTERN S16BIT _DECL _aceMTIDecodeRawMsg
(
    PMTI_CH10_DATA_PKT pCh10Pkt,
    U16BIT **ppCurMsg,
    MSGSTRUCT *pDecMsg
);

/* MCS 9.3.19 MTI Decode 1553A -- Add DevNum*/
_EXTERN S16BIT _DECL _aceMTIDecodeRawMsgEx
(
	S16BIT DevNum,
	PMTI_CH10_DATA_PKT pCh10Pkt,
	U16BIT **ppCurMsg,
	MSGSTRUCT *pDecMsg,
	U32BIT u32Options  /* Future expansion */
);

_EXTERN S16BIT _DECL _aceMTISetCh10TimePktId
(
    S16BIT DevNum,
    U16BIT u16ChannelId
);

_EXTERN S16BIT _DECL _aceMTICh10FileOpen
(
    PMTI_CH10_FILE_HANDLE   *pCh10FileHandle,
    const CHAR              *pFilename,
    U8BIT u8FileAccessMode,
    CHAR                    *pFileHeader,
    U32BIT u32FileHeaderLen
);

_EXTERN S16BIT _DECL _aceMTICh10FileClose
(
    PMTI_CH10_FILE_HANDLE   *pCh10FileHandle
);

_EXTERN S16BIT _DECL _aceMTICh10FileRead
(
    PMTI_CH10_FILE_HANDLE pCh10FileHandle,
    U8BIT u8PacketReadType,
    PMTI_CH10_DATA_PKT pMtiCh10Header,
    VOID                    *pDataPacket,
    U32BIT u32DataPacketLen
);

_EXTERN S16BIT _DECL _aceMTICh10FileReadRaw
(
    PMTI_CH10_FILE_HANDLE pCh10FileHandle,
    U8BIT u8PacketReadType,
    PMTI_CH10_DATA_PKT pMtiCh10Header,
    VOID                    *pDataPacket,
    U32BIT u32DataPacketLen
);

_EXTERN S16BIT _DECL _aceMTICh10FileGetOffset
(
    PMTI_CH10_FILE_HANDLE pCh10FileHandle,
    S64BIT                  *pOffset
);

_EXTERN S16BIT _DECL _aceMTICh10FileSetOffset
(
    PMTI_CH10_FILE_HANDLE pCh10FileHandle,
    S64BIT s64Offset
);

_EXTERN S16BIT _DECL _aceMTICh10FileWrite
(
    PMTI_CH10_FILE_HANDLE pCh10FileHandle,
    VOID                    *pPacket,
    U32BIT u32PktLength
);

#endif /* ifndef __MTIOP_H__ */
