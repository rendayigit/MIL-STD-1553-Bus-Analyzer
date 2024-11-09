/*******************************************************************************
 * FILE: ddc_ioctl.h
 *
 * DESCRIPTION:
 *
 *  Input/Output Control (IOCTL) Interface Definitions
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

#ifndef _DDC_IOCTL_H_
#define _DDC_IOCTL_H_

/*  Version information is defined at this location */
#include "os/include/ddc_os_version.h"   

/* 
    NOTE:   The following macros generate the alternate forms of the
            release version number used in the libraries and drivers.
            
            ONLY THE VERSION NUMBERS'S FROM ddc_os_version.h NEED TO BE UPDATED.
*/


/* ==================================================== */
/* Macro:   DDC_RELEASE_VERSION_X_Y_Z_B                 */
/*                                                      */
/* Format:  32-bit Hex Number                           */
/*          0x01020304                                  */
/* ==================================================== */
#define DDC_RELEASE_VERSION_X_Y_Z_B  ( \
    (DDC_RELEASE_VERSION_X << 24) | \
    (DDC_RELEASE_VERSION_Y << 16) | \
    (DDC_RELEASE_VERSION_Z << 8) | \
    (DDC_RELEASE_VERSION_B) \
    )


/* ==================================================== */
/* Macro:   DDC_RELEASE_VERSION_X_Y_Z_B_STRING          */
/*                                                      */
/* Format:  String                                      */
/*          "1.2.3.4"                                   */
/* ==================================================== */

/* ---------------------------------------------------- */
/* helper macro to generate version # as a string       */
/* ---------------------------------------------------- */
#define DDC_RELEASE_VERSION_NUMBER_AS_STRING_HELPER(ver) #ver
#define DDC_RELEASE_VERSION_NUMBER_AS_STRING(ver) DDC_RELEASE_VERSION_NUMBER_AS_STRING_HELPER(ver)

/* ---------------------------------------------------- */
/* macro to used in the library/driver                  */
/* ---------------------------------------------------- */
#define DDC_RELEASE_VERSION_X_Y_Z_B_STRING \
    DDC_RELEASE_VERSION_NUMBER_AS_STRING(DDC_RELEASE_VERSION_X) \
    "." \
    DDC_RELEASE_VERSION_NUMBER_AS_STRING(DDC_RELEASE_VERSION_Y) \
    "." \
    DDC_RELEASE_VERSION_NUMBER_AS_STRING(DDC_RELEASE_VERSION_Z) \
    "." \
    DDC_RELEASE_VERSION_NUMBER_AS_STRING(DDC_RELEASE_VERSION_B) 


/* ==================================================== */
/* Macro:   DDC_RELEASE_VERSION_X_Y_Z_B_COMMA_SEPARATED */
/*                                                      */
/* Format:  Comma separated decimal numbers             */
/*          1,2,3,4                                     */
/* ==================================================== */
#define DDC_RELEASE_VERSION_X_Y_Z_B_COMMA_SEPARATED \
    DDC_RELEASE_VERSION_X,DDC_RELEASE_VERSION_Y,DDC_RELEASE_VERSION_Z,DDC_RELEASE_VERSION_B

#include "os/include/ddc_os_ioctl.h"   /* DDC_IOCTL() macro definition */

#define DDC_IOCTL_U32(x) (U32BIT)(x) /* porting to 64 bit caused hundreds of warnings
                                        because Param* below changed size, but,
                                        because sometimes params are pointers the use
                                        of unsigned long is necessary.
                                        Using this macro helps identify the places where 
                                        casting had to be introduced */

/* ========================================================================== */
/* IOCTL Structure Definition                                                 */
/* ========================================================================== */

typedef struct _DDC_IOCTL_BUFFER_INFO_TYPE
{
    unsigned long pDataBuffer;          /* pointer to the data buffer */
    U32BIT u32BufferLength;             /* size of the data buffer */

} DDC_IOCTL_BUFFER_INFO_TYPE;

typedef struct _DDC_IOCTL_DATA_BUFFERS_TYPE
{
    DDC_IOCTL_BUFFER_INFO_TYPE sInBuffer;                   /* input buffer */
    DDC_IOCTL_BUFFER_INFO_TYPE sReturnStatus;               /* return status of command */
    DDC_IOCTL_BUFFER_INFO_TYPE sOutBufferBytesReturned;     /* # of bytes returned in output buffer */
    DDC_IOCTL_BUFFER_INFO_TYPE sInOutBuffer;                /* output or additional input buffer (optional) */

} DDC_IOCTL_DATA_BUFFERS_TYPE;


/* general purpose IOCTL input buffer for sending 32-bit data */
typedef struct _DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE
{
    U32BIT pDataBuffer;         /* pointer to the data buffer */
    U32BIT u32BufferLength;     /* size of the data buffer */

} DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE;

typedef struct _DDC_IOCTL_COMPAT_DATA_BUFFERS_TYPE
{
    DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE sInBuffer;                   /* input buffer */
    DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE sReturnStatus;               /* return status of command */
    DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE sOutBufferBytesReturned;     /* # of bytes returned in output buffer */
    DDC_IOCTL_COMPAT_BUFFER_INFO_TYPE sInOutBuffer;                /* output or additional input buffer (optional) */

} DDC_IOCTL_COMPAT_DATA_BUFFERS_TYPE;

typedef struct _DDC_IOCTL_PARAMS
{
    U32BIT Channel;

    U32BIT Param1;
    U32BIT Param2;
    U32BIT Param3;
    U32BIT Param4;
    U32BIT Param5;
    U32BIT Param6;

} DDC_IOCTL_PARAMS;

/* ========================================================================== */
/* IOCTL Group Definitions                                                    */
/* ========================================================================== */

#define DDC_IOCTL_GROUP_DEVICE              (0 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_I_O_SERIAL          (1 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_EMA_E2MA            (2 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_1553                (3 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_1553_BC             (4 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_1553_RT             (5 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_1553_MT             (6 + DDC_IOCTL_OFFSET)
#define DDC_IOCTL_GROUP_ARINC               (7 + DDC_IOCTL_OFFSET)



/* ========================================================================== */
/* Device Group                                                               */
/* ========================================================================== */
#define IOCTL_GET_INFO                          DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 0, METHOD_BUFFERED)
#define IOCTL_GET_DEV_INFO                      DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 1, METHOD_BUFFERED)
/*                                                      ***** DO NOT USE VALUE OF 2 - LINUX RESERVED ***** */
#define IOCTL_GET_ENHANCED_CAPABILITIES         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 3, METHOD_BUFFERED)
#define IOCTL_GET_HW_VERSION_INFO               DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 4, METHOD_OUT_DIRECT)
#define IOCTL_GET_IO_COUNT                      DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 5, METHOD_OUT_DIRECT)
#define IOCTL_GET_DEV_FEATURE_INFO              DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 6, METHOD_BUFFERED)
#define IOCTL_GET_BOARD_STATUS                  DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 7, METHOD_BUFFERED)

#define IOCTL_DEVICE_INIT                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 8, METHOD_BUFFERED)
#define IOCTL_DEVICE_CONFIGURE                  DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 9, METHOD_BUFFERED)
#define IOCTL_BOARD_COMPONENT_RESET             DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 10, METHOD_BUFFERED)

#define IOCTL_REG_READ                          DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 11, METHOD_BUFFERED)
#define IOCTL_REG_READ32                        DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 12, METHOD_BUFFERED)
#define IOCTL_REG_READ_BLK                      DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 13, METHOD_OUT_DIRECT)
#define IOCTL_REG_WRITE                         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 14, METHOD_BUFFERED)
#define IOCTL_REG_WRITE32                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 15, METHOD_BUFFERED)
#define IOCTL_REG_WRITE_BLK                     DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 16, METHOD_IN_DIRECT)

#define IOCTL_MEM_READ                          DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 17, METHOD_BUFFERED)
#define IOCTL_MEM_READ32                        DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 18, METHOD_BUFFERED)
#define IOCTL_MEM_READ_BLK                      DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 19, METHOD_OUT_DIRECT)
#define IOCTL_MEM_WRITE                         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 20, METHOD_BUFFERED)
#define IOCTL_MEM_WRITE32                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 21, METHOD_BUFFERED)
#define IOCTL_MEM_WRITE_BLK                     DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 22, METHOD_IN_DIRECT)

#define IOCTL_BLK_READ                          DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 23, METHOD_BUFFERED)
#define IOCTL_BLK_WRITE                         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 24, METHOD_IN_DIRECT)

#define IOCTL_CLEAR_MEMORY                      DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 25, METHOD_BUFFERED)

#define IOCTL_FLASH_MEM_INFO                    DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 26, METHOD_OUT_DIRECT)
#define IOCTL_FLASH_MEM_WRITE_PROTECTED         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 27, METHOD_OUT_DIRECT)
#define IOCTL_FLASH_MEM_BLOCK_ERASE             DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 28, METHOD_IN_DIRECT)
#define IOCTL_FLASH_MEM_READ                    DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 29, METHOD_OUT_DIRECT)
#define IOCTL_FLASH_MEM_WRITE                   DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 30, METHOD_IN_DIRECT)

#define IOCTL_IRIG_GET_TX                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 31, METHOD_OUT_DIRECT)
#define IOCTL_IRIG_SET_TX                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 32, METHOD_IN_DIRECT)
#define IOCTL_IRIG_SET_ID                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 33, METHOD_IN_DIRECT)
#define IOCTL_IRIG_SET_INT_STATE                DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 34, METHOD_IN_DIRECT)

#define IOCTL_SET_TT_RESOLUTION                 DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 35, METHOD_IN_DIRECT)
#define IOCTL_SET_TT_ROLLOVER_POINT             DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 36, METHOD_IN_DIRECT)
#define IOCTL_SET_TT_INTERRUPT                  DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 37, METHOD_IN_DIRECT)
#define IOCTL_EXT_TT_CNT_CTRL                   DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 38, METHOD_IN_DIRECT)

#define IOCTL_DIO_TT_CFG                        DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 39, METHOD_IN_DIRECT)
#define IOCTL_DIO_TT_CTL                        DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 40, METHOD_IN_DIRECT)
#define IOCTL_DIO_TT_READ                       DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 41, METHOD_OUT_DIRECT)
#define IOCTL_DIO_TT_BLOCK_ON_IRQ               DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 42, METHOD_OUT_DIRECT)
#define IOCTL_DIO_TT_RELEASE_IRQ                DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 43, METHOD_OUT_DIRECT)

#define IOCTL_WAIT                              DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 44, METHOD_OUT_DIRECT)

#define IOCTL_PLX_READ                          DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 45, METHOD_OUT_DIRECT)
#define IOCTL_PLX_WRITE                         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 46, METHOD_IN_DIRECT)

#define IOCTL_GET_CAPABILITIES                  DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 47, METHOD_BUFFERED)
#define IOCTL_SET_CONFIGURATION_OPTIONS         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 48, METHOD_BUFFERED)

#define IOCTL_SET_BOARD_FEATURE                 DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 49, METHOD_IN_DIRECT)
#define IOCTL_GET_HANDLE_INFO                   DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 50, METHOD_BUFFERED)

/* ---------------------------------------------------------------------------------------------------- */
/*                                              MUST BE LAST VAUE IN GROUP - DO NOT CHANGE OR ADD BELOW */
#define IOCTL_DDC_DEBUG                         DDC_IOCTL(DDC_IOCTL_GROUP_DEVICE, 127, METHOD_BUFFERED)
/* ---------------------------------------------------------------------------------------------------- */


/* ========================================================================== */
/* I/O & Serial Group                                                         */
/* ========================================================================== */

#define IOCTL_GET_AIO_OUTPUT                    DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 0, METHOD_OUT_DIRECT)
#define IOCTL_GET_AIO_DIRECTION                 DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 1, METHOD_OUT_DIRECT)
#define IOCTL_GET_AIO_INPUT                     DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 2, METHOD_OUT_DIRECT)
#define IOCTL_GET_AIO_ALL                       DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 3, METHOD_OUT_DIRECT)
#define IOCTL_SET_AIO_OUTPUT                    DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 4, METHOD_IN_DIRECT)
#define IOCTL_SET_AIO_DIRECTION                 DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 5, METHOD_IN_DIRECT)
#define IOCTL_SET_AIO_ALL                       DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 6, METHOD_IN_DIRECT)

#define IOCTL_GET_DIO_OUTPUT                    DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 7, METHOD_OUT_DIRECT)
#define IOCTL_GET_DIO_DIRECTION                 DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 8, METHOD_OUT_DIRECT)
#define IOCTL_GET_DIO_INPUT                     DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 9, METHOD_OUT_DIRECT)
#define IOCTL_GET_DIO_ALL                       DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 10, METHOD_OUT_DIRECT)
#define IOCTL_SET_DIO_OUTPUT                    DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 11, METHOD_IN_DIRECT)
#define IOCTL_SET_DIO_DIRECTION                 DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 12, METHOD_IN_DIRECT)
#define IOCTL_SET_DIO_ALL                       DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 13, METHOD_IN_DIRECT)

#define IOCTL_CAN_BUS_TX_DATA                   DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 14, METHOD_OUT_DIRECT)
#define IOCTL_CAN_BUS_RX_DATA                   DDC_IOCTL(DDC_IOCTL_GROUP_I_O_SERIAL, 15, METHOD_OUT_DIRECT)


/* ========================================================================== */
/* EMA/E2MA Legacy Group                                                      */
/* ========================================================================== */

#define IOCTL_EMA_GP_REG_READ                   DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 0, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GP_REG_WRITE                  DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 1, METHOD_IN_DIRECT)

#define IOCTL_EMA_GET_BC_DATA_BLOCK_ADDR        DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 2, METHOD_OUT_DIRECT)
#define IOCTL_EMA_SET_BC_MESSAGE_BLK            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 3, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_BC_MESSAGE_DATA_PTR       DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 4, METHOD_IN_DIRECT)
#define IOCTL_EMA_GET_BC_EXT_TRIGGER_AVAIL      DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 5, METHOD_OUT_DIRECT)

#define IOCTL_EMA_GET_RT_CMD_STK_PTR            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 6, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_CMD_STK_NEXT           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 7, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_CMD_STK_LATEST         DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 8, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_CMD_STK_DATA           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 9, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_DATA_BLK_OFFSET        DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 10, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_DATA_BLK_PTR           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 11, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_SUB_ADDR_CTL_WORD      DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 12, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_RT_AUTO_BOOT_INFO         DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 13, METHOD_OUT_DIRECT)

#define IOCTL_EMA_SET_RT_CMD_STK_PURGE          DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 14, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_RT_CMD_STK_PTR            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 15, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_RT_DATA_BLK_PTR           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 16, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_RT_CMD_STK_SIZE           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 17, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_RT_SUB_ADDR_CTL_WORD      DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 18, METHOD_IN_DIRECT)

#define IOCTL_EMA_GET_MT_CMD_STK_PTR            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 19, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_MT_CMD_STK_NEXT           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 20, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_MT_CMD_STK_LATEST         DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 21, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_MT_CMD_STK_DATA           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 22, METHOD_OUT_DIRECT)
#define IOCTL_EMA_GET_MT_CMD_STK_DATA_COUNT     DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 23, METHOD_OUT_DIRECT)

#define IOCTL_EMA_SET_MT_CMD_STK_PURGE          DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 24, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_MT_CMD_STK_PTR            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 25, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_MT_DATA_STK_PTR           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 26, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_MT_CMD_STK_SIZE           DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 27, METHOD_IN_DIRECT)
#define IOCTL_EMA_SET_MT_DATA_STK_SIZE          DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 28, METHOD_IN_DIRECT)

#define IOCTL_EMA_SET_MTI_CONFIG                DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 29, METHOD_IN_DIRECT)

#define IOCTL_EMA_GET_CMD_STACK_AREA            DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 30, METHOD_OUT_DIRECT)
#define IOCTL_EMA_CMD_STACK_READ                DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 31, METHOD_OUT_DIRECT)
#define IOCTL_EMA_CMD_STACK_WRITE               DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 32, METHOD_OUT_DIRECT)
#define IOCTL_EMA_DATA_STACK_READ               DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 33, METHOD_OUT_DIRECT)
#define IOCTL_EMA_DATA_STACK_WRITE              DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 34, METHOD_OUT_DIRECT)

#define IOCTL_EMA_GET_TIME_TAG                  DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 35, METHOD_OUT_DIRECT)
#define IOCTL_EMA_SET_TIME_TAG                  DDC_IOCTL(DDC_IOCTL_GROUP_EMA_E2MA, 36, METHOD_OUT_DIRECT)


/* ========================================================================== */
/* 1553 Group                                                                 */
/* ========================================================================== */

#define IOCTL_1553_ENABLE_IRQ                   DDC_IOCTL(DDC_IOCTL_GROUP_1553, 0, METHOD_IN_DIRECT)
#define IOCTL_1553_DISABLE_IRQ                  DDC_IOCTL(DDC_IOCTL_GROUP_1553, 1, METHOD_IN_DIRECT)
#define IOCTL_1553_BLOCK_ON_IRQ                 DDC_IOCTL(DDC_IOCTL_GROUP_1553, 2, METHOD_BUFFERED)
#define IOCTL_1553_BLOCK_ON_IRQ_READY           DDC_IOCTL(DDC_IOCTL_GROUP_1553, 3, METHOD_IN_DIRECT)
#define IOCTL_1553_SET_MODULE_INTERRUPT_STATE   DDC_IOCTL(DDC_IOCTL_GROUP_1553, 4, METHOD_OUT_DIRECT)
#define IOCTL_1553_SET_IRQ_CONDITIONS           DDC_IOCTL(DDC_IOCTL_GROUP_1553, 5, METHOD_OUT_DIRECT)

#define IOCTL_1553_SET_RAM_PARITY_CHECKING      DDC_IOCTL(DDC_IOCTL_GROUP_1553, 6, METHOD_IN_DIRECT)
#define IOCTL_1553_IMP_READ                     DDC_IOCTL(DDC_IOCTL_GROUP_1553, 7, METHOD_OUT_DIRECT)
#define IOCTL_1553_SET_RESP_TIME_OUT            DDC_IOCTL(DDC_IOCTL_GROUP_1553, 8, METHOD_IN_DIRECT)

#define IOCTL_1553_GET_COUPLING                 DDC_IOCTL(DDC_IOCTL_GROUP_1553, 9, METHOD_OUT_DIRECT)
#define IOCTL_1553_SET_COUPLING                 DDC_IOCTL(DDC_IOCTL_GROUP_1553, 10, METHOD_IN_DIRECT)
#define IOCTL_1553_GET_AMPLITUDE                DDC_IOCTL(DDC_IOCTL_GROUP_1553, 11, METHOD_OUT_DIRECT)
#define IOCTL_1553_SET_AMPLITUDE                DDC_IOCTL(DDC_IOCTL_GROUP_1553, 12, METHOD_IN_DIRECT)

#define IOCTL_1553_CHECK_MF_CAPABLE             DDC_IOCTL(DDC_IOCTL_GROUP_1553, 13, METHOD_OUT_DIRECT)

#define IOCTL_1553_TRG_RESET                    DDC_IOCTL(DDC_IOCTL_GROUP_1553, 14, METHOD_IN_DIRECT)
#define IOCTL_1553_TRG_IRQ_STATUS               DDC_IOCTL(DDC_IOCTL_GROUP_1553, 15, METHOD_OUT_DIRECT)

#define IOCTL_1553_BLOCK_ON_IRQ_EX                  DDC_IOCTL(DDC_IOCTL_GROUP_1553, 16, METHOD_BUFFERED)
#define IOCTL_1553_BLOCK_ON_IRQ_READY_EX            DDC_IOCTL(DDC_IOCTL_GROUP_1553, 17, METHOD_IN_DIRECT)
#define IOCTL_1553_CONFIGURE_INTERRUPT_CONDITIONS   DDC_IOCTL(DDC_IOCTL_GROUP_1553, 18, METHOD_BUFFERED)


/* ========================================================================== */
/* 1553 BC Group                                                              */
/* ========================================================================== */

#define IOCTL_BC_GET_ASYNC_COUNT                DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 0, METHOD_OUT_DIRECT)
#define IOCTL_BC_GET_DATA_BUFFER                DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 1, METHOD_OUT_DIRECT)
#define IOCTL_BC_GET_FRAME_TO_HBUF              DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 2, METHOD_BUFFERED)
#define IOCTL_BC_GET_HBUF_MESSAGE               DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 3, METHOD_OUT_DIRECT)
#define IOCTL_BC_GET_METRICS                    DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 4, METHOD_OUT_DIRECT)
#define IOCTL_BC_GET_MESSAGE                    DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 5, METHOD_OUT_DIRECT)
#define IOCTL_BC_GPQ_CLEAR                      DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 6, METHOD_BUFFERED)
#define IOCTL_BC_GPQ_COUNT                      DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 7, METHOD_OUT_DIRECT)
#define IOCTL_BC_GPQ_READ                       DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 8, METHOD_OUT_DIRECT)
#define IOCTL_BC_POST_ASYNC_MESSAGE             DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 9, METHOD_BUFFERED)
#define IOCTL_BC_SET_MSG_BUFFER                 DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 10, METHOD_IN_DIRECT)
#define IOCTL_BC_SET_DATA_BUFFER                DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 11, METHOD_IN_DIRECT)
#define IOCTL_BC_STATE                          DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 12, METHOD_BUFFERED)
#define IOCTL_BC_DATASTR_INIT                   DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 13, METHOD_BUFFERED)
#define IOCTL_BC_DATASTR_STATUS                 DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 14, METHOD_OUT_DIRECT)
#define IOCTL_BC_DATA_ARRAY_INIT                DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 15, METHOD_BUFFERED)
#define IOCTL_BC_DATA_ARRAY_STATUS              DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 16, METHOD_OUT_DIRECT)
#define IOCTL_BC_GET_EXT_TRIGGER_AVAIL          DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 17, METHOD_OUT_DIRECT)
#define IOCTL_BC_ASYNC_QUEUE_INFO               DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 18, METHOD_OUT_DIRECT)
#define IOCTL_BC_CMD_STACK_READ                 DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 19, METHOD_OUT_DIRECT)
#define IOCTL_BC_DATA_STACK_READ                DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 20, METHOD_OUT_DIRECT)

#define IOCTL_BC_REPLAY_STATUS                  DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 21, METHOD_OUT_DIRECT)
#define IOCTL_BC_REPLAY_BLOCK_ON_IRQ            DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 22, METHOD_BUFFERED)
#define IOCTL_BC_REPLAY_RELEASE_IRQ             DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 23, METHOD_BUFFERED)
#define IOCTL_BC_REPLAY_DMA                     DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 24, METHOD_IN_DIRECT)
#define IOCTL_BC_REPLAY_RAW                     DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 25, METHOD_BUFFERED)
#define IOCTL_BC_REPLAY_WRITE                   DDC_IOCTL(DDC_IOCTL_GROUP_1553_BC, 26, METHOD_IN_DIRECT)


/* ========================================================================== */
/* DDC_IOCTL_GROUP_1553_MT                                                    */
/* ========================================================================== */

#define IOCTL_MT_SET_STATE                      DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 0, METHOD_IN_DIRECT)
#define IOCTL_MT_SET_FREE                       DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 1, METHOD_IN_DIRECT)
#define IOCTL_MT_SET_STROBE_REG                 DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 2, METHOD_IN_DIRECT)
#define IOCTL_MT_INTERUPT_CONFIG                DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 3, METHOD_IN_DIRECT)
#define IOCTL_MT_GET_METRICS_CONFIG             DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 4, METHOD_IN_DIRECT)
#define IOCTL_MT_FREE_MEM_COUNT                 DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 5, METHOD_IN_DIRECT)

#define IOCTL_MTI_GET_CH10_STATUS               DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 6, METHOD_IN_DIRECT)
#define IOCTL_MTI_GET_CH10_DATA_PKT             DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 7, METHOD_OUT_DIRECT)
#define IOCTL_MTI_GET_CH10_TIME_PKT             DDC_IOCTL(DDC_IOCTL_GROUP_1553_MT, 8, METHOD_OUT_DIRECT)


/* ========================================================================== */
/* 1553 RT Group                                                              */
/* ========================================================================== */

#define IOCTL_RT_DATA_SEND_STREAM               DDC_IOCTL(DDC_IOCTL_GROUP_1553_RT, 0, METHOD_IN_DIRECT)
#define IOCTL_RT_DATA_RECEIVE_STREAM            DDC_IOCTL(DDC_IOCTL_GROUP_1553_RT, 1, METHOD_IN_DIRECT)

#define IOCTL_RT_READ_MODE_CODE_DATA            DDC_IOCTL(DDC_IOCTL_GROUP_1553_RT, 2, METHOD_IN_DIRECT)
#define IOCTL_RT_WRITE_MODE_CODE_DATA           DDC_IOCTL(DDC_IOCTL_GROUP_1553_RT, 3, METHOD_OUT_DIRECT)


/* ========================================================================== */
/* DDC_IOCTL_GROUP_ARINC                                                      */
/* ========================================================================== */

#define IOCTL_ARINC_ENABLE_IRQ                  DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 0, METHOD_IN_DIRECT)
#define IOCTL_ARINC_DISABLE_IRQ                 DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 1, METHOD_IN_DIRECT)
#define IOCTL_ARINC_BLOCK_ON_IRQ                DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 2, METHOD_BUFFERED)
#define IOCTL_ARINC_BLOCK_ON_IRQ_READY          DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 3, METHOD_BUFFERED)
#define IOCTL_ARINC_RELEASE_IRQ                 DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 4, METHOD_BUFFERED)

#define IOCTL_ARINC_429_LOADTXONE               DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 5, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_LOADTXMORE              DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 6, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_GET_TT_CONFIG           DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 7, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_429_SET_TT_CONFIG           DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 8, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_GET_PGRM_LOOPBACK       DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 9, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_429_SET_PGRM_LOOPBACK       DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 10, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_REG_READ                DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 11, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_429_REG_WRITE               DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 12, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_MEM_READ                DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 13, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_429_MEM_READ_MORE           DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 14, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_429_MEM_WRITE               DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 15, METHOD_IN_DIRECT)

#define IOCTL_ARINC_429_SET_OLD_LOOPBACK        DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 16, METHOD_IN_DIRECT)
#define IOCTL_ARINC_429_GET_OLD_LOOPBACK        DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 17, METHOD_OUT_DIRECT)

#define IOCTL_ARINC_429_RX_HBUF_CTRL            DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 18, METHOD_BUFFERED)
#define IOCTL_ARINC_429_GET_QUEUE_STATUS        DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 19, METHOD_IN_DIRECT)

#define IOCTL_ARINC_DD429X_COMMAND              DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 20, METHOD_OUT_DIRECT)


#define IOCTL_ARINC_717_LOAD_TX                 DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 21, METHOD_OUT_DIRECT)
#define IOCTL_ARINC_717_GET_RX                  DDC_IOCTL(DDC_IOCTL_GROUP_ARINC, 22, METHOD_OUT_DIRECT)


/* ========================================================================== */

/* ========================================================================== */
/* IOCTL_GET_INFO Sub-Commands                                                */
/* ========================================================================== */
/* Param1 Types */
#define DDC_GET_INFO__REG_BASE_ADDR             0
#define DDC_GET_INFO__MEM_BASE_ADDR             1

/* Param2 Types */
#define DDC_GET_INFO_ADDR__BD_GLOBAL            0

/* ========================================================================== */
/* IOCTL_DEVICE_CONFIGURE Sub-Commands                                        */
/* associated with u16Type in ACEX_CONFIG_ID                                  */
/* ========================================================================== */
#define ACEX_MRT_CONFIG                         1
#define ACEX_MRT_FREE                           2
#define ACEX_RT_OPEN                            3
#define ACEX_RT_CONFIG_SET                      4   /* enable configuration features after open     */
#define ACEX_RT_CONFIG_CLR                      5   /* disable configuration features after open    */
#define ACEX_RT_BITWD_RD                        6
#define ACEX_RT_START                           7
#define ACEX_RT_STOP                            8
#define ACEX_RT_CLOSE                           9
#define ACEX_RT_LATCH_CTRL                      10
#define ACEX_RT_SOURCE_CTRL                     11
#define ACEX_MRT_HBUF_CTRL                      12
#define ACEX_MRT_CMDSTK_STATS                   13
#define ACEX_MRT_CMDSTK_SET_TO_LATEST           14
#define ACEX_MRT_GET_BRDCST_ADDRS               15
#define ACEX_RT_STREAM_CTRL                     16
#define ACEX_RT_START_RESET_CMDSTK              17
#define ACEX_RT_DATA_ARRAY_CTRL                 18
#define ACEX_MRT_CONFIG_UPDATE                  19  /* this command is used to update mrt global config after initial configuration is completed */

#define ACEX_MT_CONFIG                          20
#define ACEX_MT_FREE                            21
#define ACEX_MT_INIT                            22

#define ACEX_BC_INIT                            40
#define ACEX_BC_CONFIG                          41
#define ACEX_BC_FREE                            42
#define ACEX_BC_HBUF_INSTALL                    43
#define ACEX_BC_HBUF_UNINSTALL                  44
#define ACEX_BC_GPQ_INIT                        45
#define ACEX_BC_LPQ_INIT                        46
#define ACEX_BC_HPQ_INIT                        47
#define ACEX_BC_REPLAY_INIT                     48
#define ACEX_BC_MUX_INIT                        49

#define ACEX_ARINC_429_PROGRAMABLE              50

#define ACEX_ARINC_717_CONFIG                   70
#define ACEX_ARINC_717_FREE                     71
#define ACEX_ARINC_717_STATE                    72
#define ACEX_ARINC_717_INTERRUPTS               73

#define ACEX_CAN_BUS_CONFIG                     74
#define ACEX_CAN_BUS_FREE                       75
#define ACEX_CAN_BUS_STATE                      76
#define ACEX_CAN_FIRWARE_VERSION                77

#define ACEX_CHAN_INIT                          200 /* case to clear all resources for a given channel */

/* ========================================================================== */
/* IOCTL_1553_IMP_READ Sub-Commands                                           */
/* ========================================================================== */
#define IOCTL_IMP_MRT_CMD                       0
#define IOCTL_IMP_MRT_DATA                      1
#define IOCTL_IMP_BC_CMD                        2
#define IOCTL_IMP_BC_DATA                       3


/* ========================================================================== */
/* IOCTL_SET_RESP_TIME_OUT Sub-Commands                                       */
/* ========================================================================== */
#define ACEX_RESP_MAX_VALUE                     0x0007
#define ACEX_RESP_BC                            0x0001
#define ACEX_RESP_MRT                           0x0002
#define ACEX_RESP_MTI                           0x0004


/* ========================================================================== */
/* ========================================================================== */
#define DDC_REGISTER_ACCESS                     0
#define DDC_MEMORY_ACCESS                       1


/* ========================================================================== */
/* PLX IOCTL Access Types                                                     */
/* ========================================================================== */
#define PLX_READ_REG                            0x01
#define PLX_READ_EEPROM                         0x02
#define PLX_WRITE_REG                           0x03
#define PLX_WRITE_EEPROM                        0x04

/* ========================================================================== */
/* Debug Sub-Commands                                                         */
/* ========================================================================== */
#define DDC_DEBUG_SET_TRACE_LEVEL               0
#define DDC_DEBUG_PRINT_UDL_MEM_INFO            1
#define DDC_DEBUG_PRINT_DMA_MEM_INFO            2

/* ========================================================================== */
/* ========================================================================== */


#endif /* _DDC_IOCTL_H_ */
