/*******************************************************************************
 * FILE: ddc_os_lib_util.h
 *
 * DESCRIPTION:
 *
 *  These routines are Linux substitutes for functions that are not provided
 *  natively.
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

#ifndef _DDC_OS_LIB_UTIL_H_
#define _DDC_OS_LIB_UTIL_H_

#include <time.h>

#include "os/include/ddc_os_types.h"

typedef struct _COORDS
{
        short X;
        short Y;
} COORD;

#define STD_OUTPUT_HANDLE 123L


#ifndef _REENTRANT
#define _REENTRANT /* Stones and Matthew: Beginning Linux Programming 3rd.Ed. p.480. */
#endif

#define LPTHREAD_START_ROUTINE void*

#define DDC_MSLEEP(u32Milliseconds) \
{ \
    struct timespec ts; \
    ts.tv_sec = (u32Milliseconds / 1000); /* get whole seconds to sleep */ \
    ts.tv_nsec = (u32Milliseconds - (ts.tv_sec * 1000)) * 1000000; /* get remainder */ \
    while(nanosleep(&ts,&ts) == -1) \
    { \
        continue; \
    } \
}

#define DDC_USLEEP(u32Microseconds) \
{ \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = (u32Microseconds * 1000); /* get remainder */ \
    while(nanosleep(&ts,&ts) == -1) \
    { \
        continue; \
    } \
}

/* key input */
extern int kbhit
(
    void
);

extern int getch
(
    void
);

void ddcPressAKey
(
    char *prompt
);

extern void Sleep
(
    unsigned long int dwMs
);

extern void SetConsoleCursorPosition
(
    DDC_HANDLE hConsole,
    COORD CursCoord
);

extern void SetConsoleTitle
(
    char *title
);

DDC_THREAD_HANDLE CreateThread
(
    DDC_SECURITY_ATTRIBUTES pAttr,
    int DefaultStackSize,
    FUNCPTR pFunction,
    PVOID pThreadFunctionArg,
    int CreationFlags,
    DDC_THREAD_ID *pThreadID
);


extern pthread_t GetCurrentThreadId
(
    void
);

#ifdef DDC_LINUX_THREAD
extern int CloseHandle
(
    pthread_t* pThreadID
);

#else /* ifdef DDC_LINUX_THREAD */

extern void CloseHandle
(
    DDC_HANDLE handle
);

#endif /* DDC_LINUX_THREAD */

#endif /* _DDC_OS_LIB_UTIL_H_ */
