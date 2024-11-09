/*******************************************************************************
 * FILE: ddc_arinc_error_list.h
 *
 * DESCRIPTION:
 *
 *  ARINC Error Codes.
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

#ifndef _DDC_ARINC_ERROR_LIST_H_
#define _DDC_ARINC_ERROR_LIST_H_

#define ERR_SUCCESS                     0
#define ERR_IRQ                         -6
#define ERR_NOCRD                       -20
#define ERR_INUSE                       -29
#define ERR_TIMEOUT                     -31
#define ERR_NULL                        -95
#define ERR_CRDSERV                     -96
#define ERR_INITFAIL                    -100
#define ERR_UNKNOWN                     -101
#define ERR_CRDINIT                     -102
#define ERR_CRDINITFAIL                 -103
#define ERR_NORES                       -105
#define ERR_FLASH                       -106
#define ERR_TXQUEUESZ                   -111
#define ERR_RXQUEUESZ                   -113
#define ERR_CHNLGROUP                   -120
#define ERR_LOOPBACK                    -121
#define ERR_BITFORMAT                   -122
#define ERR_TX                          -130
#define ERR_PARITY                      -131
#define ERR_SPEED                       -132
#define ERR_ENABLE                      -133
#define ERR_MODE                        -134
#define ERR_TIMETAG                     -135
#define ERR_TTFORMAT                    -136
#define ERR_TTRO                        -137
#define ERR_TTRES                       -138
#define ERR_FREQ                        -140
#define ERR_OFFSET                      -141
#define ERR_RX                          -150
#define ERR_RXGROUP                     -151
#define ERR_BUFFER_REQUEST              -158
#define ERR_INDEX                       -159
#define ERR_LABELSDI                    -160
#define ERR_FILTER                      -161
#define ERR_AVIONIC                     -169
#define ERR_DISCRETE                    -170
#define ERR_DLEVEL                      -171
#define ERR_SERIAL                      -175
#define ERR_SERCHNL                     -176
#define ERR_SERREG                      -177
#define ERR_INT_COND                    -180
#define ERR_INT_HANDLER                 -181
#define ERR_CHAN_TYPE                   -185
#define ERR_OVERFLOW                    -200
#define ERR_FPGA_REV                    -210
#define ERR_FEATURE_NOT_SUPPORTED       -211
#define ERR_INVALID_CHANNEL_NO          -212
#define ERR_429_PROG_RESET              -213
#define ERR_717_PROG_CONFIG             -214
#define ERR_INVALID_CH_TYPE             -215
#define ERR_717_PROG_TX_LOAD            -216
#define ERR_717_PROG_SET_STATE          -217
#define ERR_717_PROG_INTERRUPT          -218
#define ERR_717_PROG_RX_DATA            -219
#define ERR_RX_HBUF_INSTALL             -220
#define ERR_NO_RX_HBUF                  -221
#define ERR_RX_HBUF_OVERFLOW            -222
#define ERR_CAN_BUS_SET_STATE           -223
#define ERR_CAN_BUS_TX_DATA             -224
#define ERR_CAN_BUS_FIRMWARE            -225

#define ERR_ASYNC_PRIORITY              -230
#define ERR_RESOLUTION                  -231
#define ERR_FRAME_CONTROL_TYPE          -232
#define ERR_REG_ACCESS                  -233
#define ERR_INTER_WORD_GAP              -234
#define ERR_WORD_SIZE                   -235
#define ERR_PARTIY_ERROR                -236
#define ERR_BIT_33                      -237
#define ERR_AMPLITUDE                   -238
#define ERR_TX_FRAME                    -239
#define ERR_TX_FRAME_SIZE               -240
#define ERR_TX_FRAME_RUNNING            -241
#define ERR_TX_FRAME_REPEAT_COUNT       -242
#define ERR_LABELSDI_NOT_FOUND          -243
#define ERR_CHANNELS                    -244

#define ERR_FEATURE_NOT_SUPPORTED_CH    -250
#define ERR_REPEATER_OPTION             -260
#define ERR_DEVICE_INFO                 -261
#define ERR_REG_MEM_ACCESS              -262

#define ERR_DEVICE                      -400
#define ERR_FUNCTION                    -500

/* Socket Errors */
#define ERR_SOCK_CLOSED                 -1001
#define ERR_SOCK_TIMEOUT                -1002
#define ERR_SOCK_ERROR                  -1003
#define ERR_SOCK_MSG_ALLOC_FAIL         -1004
#define ERR_SOCK_MSG_ERROR              -1005
#define ERR_SOCK_MSG_NO_SPACE           -1006
#define ERR_SOCK_MSG_SEND_FAIL          -1007
#define ERR_SOCK_MSG_RECV_FAIL          -1008
#define ERR_SOCK_FILE_OPEN_FAIL         -1009
#define ERR_SOCK_INVALID                -1010
    
#define CAN_NO_MESSAGE_PENDING          1
#define CAN_BUS_TX_BUSY                 2


#endif /* _DDC_ARINC_ERROR_LIST_H_ */
