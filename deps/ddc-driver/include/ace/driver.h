/* ILC Data Device Corp.
 *
 *        Advanced Communication Engine Integrated 1553 Terminal
 *        (ACE) 'C' Software Library
 *
 *
 *        Copyright (c) 1995-2003 by ILC Data Device Corp.
 *        All Rights Reserved.
 *
 * DRIVER.H (THREAD CONTROL MODULE)
 *
 * ILC Data Device Corp.
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Revision History for DRIVER.H (HEADER FILE)
 *
 *   Release   Date           Description
 *   4.5       22-OCT-1998    (re)created
 *
 *   5.3.1     11-APR-2003    none
 */

/* DRIVER FUNCTION PROTOTYPES -----------------------------------------------*/

/* WindowsNT */
BuError_t iBuStartService(char *Srvc);
BuError_t iBuStopService(char *Srvc);	


BuError_t iBuLoadLogicalDeviceWNT(int i);
