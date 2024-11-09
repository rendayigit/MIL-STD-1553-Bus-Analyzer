/*******************************************************************************
 * FILE: TODO
 *
 * DESCRIPTION:
 *
 *      TODO
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

#ifndef __DEVICE_R_H__
#define __DEVICE_R_H__

#define DDC_REG_KEY_SIZE 128

extern S16BIT _dvrGetRemoteDeviceInfo
(
    S16BIT s16DevNum,
    U32BIT *pu32DevID,
    U32BIT *pu32IpAddr,
    U32BIT *pu32RemoteLDN
);

extern S16BIT DDCSockConnectToServer
(
    char * pServerAddr,
    DDC_SOCKET * pClientConnSock
);

extern S16BIT DDCSockDisconnect
(
    DDC_SOCKET * pClientConnSock
);

extern void DDCPrepareMsgDispatcher
(
    S16BIT DevNum
);

extern void DDCReleaseMsgDispatcher
(
    S16BIT DevNum
);

extern S16BIT DDCClientSendAndRecv
(
    S16BIT s16DevNum,
    S32BIT s32TimeOut,
    PDDC_SOCK_MSG_STRUCT pReqMsg,
    PDDC_SOCK_MSG_STRUCT pRespMsg
);

extern S16BIT DDCClientRemoteApiCall
(
    S16BIT DevNum,
    U32BIT u32MsgCmd,
    U16BIT u16ReqParamCount,
    DDC_SOCK_DATA_TYPE* psReqParam,
    U16BIT u16RespInfoCount,
    DDC_SOCK_DATA_TYPE* psRespInfo,
    S32BIT s32TimeOut
);

extern S16BIT DDCClientRemoteApiCallOne
(
    U32BIT u32MsgCmd,
    S16BIT DevNum
);

extern S16BIT DDCClientRemoteApiCallTwo
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT Param2
);

extern S16BIT DDCClientRemoteApiCallTwoS8
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S8BIT Param2
);

extern S16BIT DDCClientRemoteApiCallTwoS16
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S16BIT Param2
);

extern S16BIT DDCClientRemoteApiCallTwoU32
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U32BIT Param2
);

extern U16BIT DDCClientRemoteApiCallTwoU16Ret
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT Param2
);

extern U32BIT DDCClientRemoteApiCallTwoU32Ret
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT Param2
);

extern U32BIT DDCClientRemoteApiCallTwoU32All
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U32BIT Param2
);

extern S16BIT DDCClientRemoteApiCallTwoGet
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT *Param2
);

extern S16BIT DDCClientRemoteApiCallTwoU32Get
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U32BIT *Param2
);

extern S16BIT DDCClientRemoteApiCallTwoTestGet
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    TESTRESULT *pTest
);

extern S16BIT DDCClientRemoteApiCallThree
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT Param2,
    U16BIT Param3
);

extern S16BIT DDCClientRemoteApiCallThreeS8
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S8BIT Param2,
    U16BIT Param3
);

extern S16BIT DDCClientRemoteApiCallThreeS16
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S16BIT Param2,
    U16BIT Param3
);

extern S16BIT DDCClientRemoteApiCallThreeU32
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    U16BIT Param2,
    U32BIT Param3
);

extern S16BIT DDCClientRemoteApiCallThreeS8U32
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S8BIT Param2,
    U32BIT Param3
);

extern S16BIT DDCClientRemoteApiCallThreeS16U32
(
    U32BIT u32MsgCmd,
    S16BIT DevNum,
    S16BIT Param2,
    U32BIT Param3
);

extern S16BIT DDCSockSetupIrqListener
(
    S16BIT DevNum
);

extern S16BIT DDCSockCloseIrqListener
(
    S16BIT DevNum
);

#endif /* ifndef __DEVICE_R_H__ */
