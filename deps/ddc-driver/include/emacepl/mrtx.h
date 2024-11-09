/**************************************************************************
 *  Data Device Corporation
 *  105 Wilbur Place
 *  Bohemia N.Y. 11716
 *  (631) 567-5600
 *
 *      AceXtreme 'C' Run Time Library
 *
 *      Copyright (c) 2009 by Data Device Corporation
 *      All Rights Reserved.
 *
 *
 *      mrtx.h (Multi-Remote Terminal private header file)
 *
 *
 *
 *      Created 2/09/09 CDF
 *
 *
 *
 ****************************************************************************/
#ifndef __MRTX_H__
#define __MRTX_H__

/* ========================================================================== */
/*                       ACEX MTI MACRO DEFINITIONS                           */
/* ========================================================================== */
#define SRT_NOT_ASSIGNED -1

/* wStatusBits can be any of the following OR'd together - set to pre-sfp meanings*/
#define ACE_MRT_STSBIT_DBCA             0x0800
#define ACE_MRT_STSBIT_BUSY             0x0400
#define ACE_MRT_STSBIT_SREQ             0x0200
#define ACE_MRT_STSBIT_SSFLAG           0x0100
#define ACE_MRT_STSBIT_RTFLAG           0x0080

/* wStatusBits when using the RT Alternate Status mode - set to pre-sfp meanings */
#define ACE_MRT_STSBIT_S10              0x0800
#define ACE_MRT_STSBIT_S09              0x0400
#define ACE_MRT_STSBIT_S08              0x0200
#define ACE_MRT_STSBIT_S07              0x0100
#define ACE_MRT_STSBIT_S06              0x0080
#define ACE_MRT_STSBIT_S05              0x0040
#define ACE_MRT_STSBIT_S04              0x0020
#define ACE_MRT_STSBIT_S03              0x0010
#define ACE_MRT_STSBIT_S02              0x0008
#define ACE_MRT_STSBIT_S01              0x0004
#define ACE_MRT_STSBIT_S00              0x0002
#define ACE_MRT_STSTBIT_MASK            0x07FF
#define ACE_MRT_STSBIT_MASK_LEGACY      0x0FFE

/* intermessage routine macros */
#define ACEX_MRT_IMR_SET_BSY_IN_STATUS      0x80000000
#define ACEX_MRT_IMR_RST_BSY_IN_STATUS      0x40000000
#define ACEX_MRT_IMR_SET_SRQ_IN_STATUS      0x20000000
#define ACEX_MRT_IMR_RST_SRQ_IN_STATUS      0x10000000
#define ACEX_MRT_IMR_WAIT_FOR_INPUT_TRIG    0x08000000
#define ACEX_MRT_IMR_NO_RESP_BOTH_BUS       0x04000000
#define ACEX_MRT_IMR_RST_DISCRETE_8         0x00000080
#define ACEX_MRT_IMR_RST_DISCRETE_7         0x00000040
#define ACEX_MRT_IMR_RST_DISCRETE_6         0x00000020
#define ACEX_MRT_IMR_RST_DISCRETE_5         0x00000010
#define ACEX_MRT_IMR_SET_DISCRETE_8         0x00000008
#define ACEX_MRT_IMR_SET_DISCRETE_7         0x00000004
#define ACEX_MRT_IMR_SET_DISCRETE_6         0x00000002
#define ACEX_MRT_IMR_SET_DISCRETE_5         0x00000001

#define ACEX_MRT_IMR_TRIG_EVENT                         0x100

#define ACEX_MRT_SACW_RXIMR_MASK                        0xFF000000      /* top 8 bits */
#define ACEX_MRT_SACW_TXIMR_MASK                        0x00FF0000      /* top 2nd 8 bits */
#define ACEX_MRT_RTDP_IMR_MASK                          0xFC000000      /* IMR uses top 6 bits only */
#define ACEX_MRT_RTMC_IMR_MASK                          0x00003FFF      /* MC IMR uses lower 13 bits only */

/* RT Global Configuration Register 1 bits */
#define ACEX_MRT_GCONFIG_GBL_DSTK_EN        0x00000010  /* Global data stack enable bit */
#define ACEX_MRT_GCONFIG_GBL_DSTK_SIZE_MASK 0x0000000F  /* Global data stack size bits */

/* ========================================================================== */
/*               ACEX (M)RT STRUCTURE AND ENUMERATION DEFINITIONS             */
/* ========================================================================== */

/*============================================================================*/
/*                   PRIVATE API ACCESS FUNCTIONS                             */
/*============================================================================*/
_EXTERN S16BIT _DECL _acexMRTConfigure
(
    S16BIT DevNum,
    U16BIT wCmdStkSize,
    U32BIT u32GblDataStkType, /*only valid if Glb type specified*/
    U16BIT u16GblDataStkBlkID
);

_EXTERN S16BIT _DECL _acexMRTEnableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL _acexMRTDisableRT
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTStart
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Options
);

_EXTERN S16BIT _DECL _acexMRTStop
(
    S16BIT DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTDataBlkMapToRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT wSA,
    U16BIT wMsgType,
    U16BIT wIrqOptions,
    U16BIT wLegalizeSA
);

_EXTERN S16BIT _DECL _acexMRTDataBlkUnmapFromRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    S16BIT nDataBlkID,
    U16BIT u16SA,
    U16BIT u16MsgType
);

_EXTERN S16BIT _DECL _acexMRTConfigRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT wBITBusyInh
);

_EXTERN S16BIT _DECL _acexMRTReadRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITLoc,
    U16BIT *pBITWrd
);

_EXTERN S16BIT _DECL _acexMRTWriteRTBITWrd
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wBITWrd
);

_EXTERN S16BIT _DECL _acexMRTClearRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexMRTGetRTBusyBitsTblStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT *pdwSABusyBits
);

_EXTERN S16BIT _DECL _acexMRTSetRTBusyBitsTbl
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U32BIT dwSAMask
);

_EXTERN S16BIT _DECL _acexMRTClearRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL _acexMRTGetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT *pwStatusBits
);

_EXTERN S16BIT _DECL _acexMRTSetRTStatusBits
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wStatusBits
);

_EXTERN S16BIT _DECL _acexMRTDisableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL _acexMRTEnableRTMsgLegality
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT dwWC_MCMask
);

_EXTERN S16BIT _DECL _acexMRTGetRTMsgLegalityStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wOwnAddrOrBcst,
    U16BIT wTR,
    U16BIT wSA,
    U32BIT *pdwWC_MCMask
);

_EXTERN S16BIT _DECL _acexMRTEnableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL _acexMRTDisableRTModeCodeIrq
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT wModeCodeIrq
);

_EXTERN S16BIT _DECL _acexMRTGetRTModeCodeIrqStatus
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCodeType,
    U16BIT *pwMCIrqStatus
);

_EXTERN S16BIT _DECL _acexMRTWriteRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT wMCData
);

_EXTERN S16BIT _DECL _acexMRTReadRTModeCodeData
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT wModeCode,
    U16BIT *pMCData
);

_EXTERN S16BIT _DECL _acexMRTDataStreamCreate
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    U32BIT u32SAMask
);

_EXTERN S16BIT _DECL _acexMRTDataStreamDelete
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL _acexMRTDataStreamSend
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID    *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL _acexMRTDataStreamReceive
(
    S16BIT s16DevNum,
    S16BIT s16DataStrId,
    S8BIT s8RTAddr,
    VOID    *pBuffer,
    U16BIT u16BufferBytes,
    S32BIT s32TimeoutMs
);

_EXTERN S16BIT _DECL _acexMRTDataArraySend
(
    S16BIT DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U8BIT *pBuf
);

_EXTERN S16BIT _DECL _acexMRTDataArrayCreate
(
    S16BIT s16DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr,
    U16BIT u16SA,
    S16BIT s16DblkID,
    U32BIT u32BufWdSize,
    BOOLEAN bContinuous
);

_EXTERN S16BIT _DECL _acexMRTDataArrayDelete
(
    S16BIT s16DevNum,
    U16BIT u16ID,
    S8BIT s8RTAddr
);

_EXTERN S16BIT _DECL _acexMRTSetMsgError
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    ACEX_ERR_INJ *psError
);

_EXTERN S16BIT _DECL _acexMRTMsgErrorEnable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTMsgErrorDisable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTSetRespTime
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL _acexMRTRespTimeEnable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTRespTimeDisable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTSetRespTimeout
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32Multiple
);

_EXTERN S16BIT _DECL _acexMRTDbcEnable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr,
    U32BIT u32RtHoldoffTime
);

_EXTERN S16BIT _DECL _acexMRTDbcDisable
(
    S16BIT s16DevNum,
    S8BIT s8RtAddr
);

_EXTERN S16BIT _DECL _acexMRTImrMapToRTSA
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT u16SA,
    U16BIT u16MsgType,
    U32BIT u32ImrType
);

_EXTERN S16BIT _DECL _acexMRTImrTrigSelect
(
    S16BIT DevNum,
    S8BIT s8RtAddr,
    U16BIT u16Select
);

#endif /* ifndef __MRTX_H__ */
