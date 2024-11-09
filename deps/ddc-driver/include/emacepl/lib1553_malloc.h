/******************************************************************************
* Data Device Corporation
*
* Filename: lib1553_malloc.h
*
* Copyright (c) 2015 Data Device Corporation
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
* This non-disclosure statement may not be removed or modified without
* prior written consent of Data Device Corporation.
*****************************************************************************/

#ifndef __LIB1553_MALLOC_H__
#define __LIB1553_MALLOC_H__

#ifndef DDC_USE_STATIC_MEMORY_ALLOCATION_1553

   #define DDC_LIB1553_MALLOC(size)                 malloc(size)
   #define DDC_LIB1553_FREE(ptr)                    free(ptr)

#else /* use DDC malloc/free */

   #define DDC_LIB1553_MALLOC(size)                 ddcLib1553Malloc(size)
   #define DDC_LIB1553_FREE(ptr)                    ddcLib1553Free(ptr)

/******************************************************************************
 * Name: lib1553_print_dynamic_memory_info
 *
 * Description: This function prints out the current dynamic memory usage.
 *
 * Returns: none
 *****************************************************************************/
void lib1553_print_dynamic_memory_info
(
    void
);

/******************************************************************************
 * Name: ddcLib1553Malloc
 *
 * Description : This function takes the call and returns a data buffer AT LEAST
 *               as large as the one requested.
 *
 * Returns:
 *
 *  Pointer to the Data Buffer or NULL on error.
 *
 *  Causes for an error:
 *  1) MMU out of Data Buffers of the size requested
 *  2) Data buffers have not been initialized
 *  3) Request was for a Data Buffer larger than any available
******************************************************************************/
void *ddcLib1553Malloc
(
    size_t size
);

/******************************************************************************
******************************************************************************/
void ddcLib1553Free
(
    void *memblock
);

#endif /* DDC_USE_STATIC_MEMORY_ALLOCATION_1553 */

#endif /* __LIB1553_MALLOC_H__ */

