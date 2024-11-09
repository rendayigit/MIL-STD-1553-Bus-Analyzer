/*******************************************************************************
 * FILE: ddc_os_lib_util.c
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

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>

#include "ddc_os_lib_util.h"

/*******************************************************************************
 * Name:    kbhit
 *
 * Description:
 *
 *      Checks for keyboard input.
 *
 * In   none
 * Out  returns     1=key pressed, 0=no key press
 ******************************************************************************/
int kbhit
(
    void
)
{
    struct timeval  tv;
    fd_set      read_fd;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&read_fd);
    FD_SET(0, &read_fd);
    usleep(1);
    if( select(1, &read_fd, NULL, NULL, &tv) == -1 )
    {
        return 0;
    }

    if( FD_ISSET(0, &read_fd) )
    {
        return 1;
    }

    return 0;
}

/*******************************************************************************
 * Name:    getch
 *
 * Description:
 *
 *      Read a character from standard input.
 *
 * In   none
 * Out  returns     character
 ******************************************************************************/
int getch
(
    void
)
{
    return getchar();
}

/*******************************************************************************
 * Name:    ddcPressAKey
 *
 * Description:   
 *      Wait for key press with given prompt information. 
 *        
 * In   prompt    prompt information to print to screen
 * Out            none
 ******************************************************************************/
void ddcPressAKey
(
    char *prompt
)
{
    /* flush input buffer */
    while (kbhit())
    {
        getchar();
    }

    if (prompt)
    {
        printf("\n%s\n", prompt);
    }

    /* flush the output buffer */
    fflush(stdout);

    while (!kbhit())
    {
        /* wait for keypress... */
        Sleep(100);
    }

    /* flush input buffer */
    while (kbhit())
    {
        getchar();
    }
}

/*******************************************************************************
 * Name:    Sleep
 *
 * Description:
 *
 *      Sleeps for 'dwMs' milliseconds.
 *
 * In   dwMs        number of milliseconds to sleep
 * Out  none
 ******************************************************************************/
void Sleep
(
    unsigned long int dwMs
)
{
    usleep(dwMs * 1000);
}

/*******************************************************************************
 * Name:    SetConsoleCursorPosition
 *
 * Description:
 *
 *      This function is not used.
 *
 * In   none
 * Out  none
 ******************************************************************************/
void SetConsoleCursorPosition
(
    DDC_HANDLE hConsole,
    COORD CursCoord
)
{
    /* do nothing */
}

/*******************************************************************************
 * Name:    SetConsoleTitle
 *
 * Description:
 *
 *      Prints out the title to the display.
 *
 * In   title       charater string to display
 * Out  none
 ******************************************************************************/
void SetConsoleTitle
(
    char *title
)
{
    printf("%s\n", title);
}

/*******************************************************************************
 * Name:    CreateThread
 *
 * Description:
 *
 *      Creates a new thread.
 *
 * In   pAttr               default security attributes
 * In   DefaultStackSize    use default stack size
 * In   ThreadFunction      thread function
 * In   ThreadFunctionArg   function argument
 * In   CreationFlags       creation flags
 * Out  returns             thread ID
 ******************************************************************************/
DDC_THREAD_HANDLE CreateThread
(
    DDC_SECURITY_ATTRIBUTES pAttr,
    int DefaultStackSize,
    FUNCPTR pFunction,
    PVOID pThreadFunctionArg,
    int CreationFlags,
    DDC_THREAD_ID *pThreadID
)
{
    if (pthread_create(pThreadID, (pthread_attr_t*)pAttr, (void*)pFunction, pThreadFunctionArg))
    {
        pthread_cancel(*pThreadID);
        return 0;
    }

    return (DDC_THREAD_HANDLE)(pThreadID);
}

/*******************************************************************************
 * Name:    GetCurrentThreadId
 *
 * Description:
 *
 *      Gets the current thread ID.
 *
 * In   none
 * Out  returns             thread ID
 ******************************************************************************/
pthread_t GetCurrentThreadId
(
    void
)
{
    return pthread_self();
}

/*******************************************************************************
 * Name:    CloseHandle
 *
 * Description:
 *
 *      Creates a new thread.
 *
 * In   pThreadID           thread ID
 * Out  error condition
 ******************************************************************************/
#ifdef DDC_LINUX_THREAD
int CloseHandle
(
    pthread_t* pThreadID
)
{
    void *resultThread;

    if (pthread_join(      /* Matthew & Stones: Beginning Linux Programming 3rd.Ed. p.481 */
                      *pThreadID,
                      &resultThread) )
    {
        printf("Join thread failed (pthread_join).\n");
        return 0;        /* Needs inverting return value for Windows CloseHandle(...). */
    }

    return 1;
}

#else

/* ========================================================================== */
/* ========================================================================== */
void CloseHandle
(
    DDC_HANDLE handle
)
{
    /* doing nothing */
}

#endif
