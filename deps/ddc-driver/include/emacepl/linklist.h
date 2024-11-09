/*******************************************************************************
 * FILE: linklist.h
 *
 * DESCRIPTION:
 *
 *  The source code module contains generic functions for
 *  manipulating double link lists. Each double link lists node
 *  will contain an ID so that the lists can be sorted if
 *  necessary. Structures containing the DLISTNODE structure
 *  are the backbone of the RTL memory management and BC setup
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

#ifndef __DLIST_H__
#define __DLIST_H__

/* Types of Nodes */
#define ACE_NODE_MEMBLOCK       0
#define ACE_NODE_DATABLOCK      1
#define ACE_NODE_MESSAGE        2
#define ACE_NODE_FRAME          3
#define ACE_NODE_OPCODE         4
#define ACE_NODE_ASYNC_OPCODE   5
#define ACE_NODE_SLOT           6
#define ACE_NODE_STREAM         7
#define ACE_NODE_ARRAY          8
#define ACE_NODE_MEMWORD        9
#define ACE_NODE_MEMOBJECT      10

/* _llCreateNode -> malloc's a new node  */
DLISTNODE * _DECL _llCreateNode
(
    S16BIT nType,
    S16BIT nID
);

/* _llDeleteNode -> free's a node */
void _DECL _llDeleteNode
(
    DLISTNODE** ppNode
);

/* _llDeleteAsyncNode -> free's a node */
void _DECL _llDeleteAsyncNode
(
    ALISTNODE** ppNode
);

/* _llGetNextNode -> gets the next node in the list */
DLISTNODE* _DECL _llGetNextNode
(
    DLISTNODE* pNode
);

/* _llGetPrevNode -> gets the previous node in the list */
DLISTNODE* _DECL _llGetPrevNode
(
    DLISTNODE* pNode
);

/* _llInsertNodeAt -> inserts a node after a given node */
S16BIT _DECL _llInsertNodeAt
(
    DLISTNODE** ppHead,
    DLISTNODE* pPrevNode,
    DLISTNODE* pNode
);

/* _llInsertNodeSorted -> inserts node into a sorted list based on ID */
S16BIT _DECL _llInsertNodeSorted
(
    DLISTNODE** ppHead,
    DLISTNODE* pNode
);

/* _llRemoveAllNodes -> removes all nodes from a list */
S16BIT _DECL _llRemoveAllNodes
(
    DLISTNODE** ppHead
);

/* _llRemoveAndFreeAllNodes -> free contents and removes all nodes from a list */
S16BIT _DECL _llRemoveAndFreeAllNodes
(
    DLISTNODE** ppHead,
    U32BIT u32NodeType
);

/* _llRemoveDeleteNodeFromID -> removes and deletes from list given ID */
S16BIT _DECL _llRemoveDeleteNodeFromID
(
    DLISTNODE** ppHead,
    S16BIT nID,
    S16BIT nType
);

/* _llRemoveNode -> removes a node from a list (no delete) */
S16BIT _DECL _llRemoveNode
(
    DLISTNODE** ppHead,
    DLISTNODE* pNode
);

/* _llSearchForNode -> attempts to find a node in a list given ID,Type */
S16BIT _DECL _llSearchForNode
(
    DLISTNODE* pHead,
    S16BIT nID,
    S16BIT nType,
    DLISTNODE** ppSearch,
    U16BIT bSortedList
);

/* _llCreateAsyncNode -> malloc's a new async message node */
ALISTNODE* _DECL _llCreateAsyncNode
(
    S16BIT nID
);

/* _llInsertAsyncNode -> inserts node to end of sorted list */
S16BIT _DECL _llInsertAsyncNode
(
    ALISTNODE** ppHead,
    ALISTNODE* pNode
);

/* _llSearchForAysncNode -> search for node in list */
S16BIT _DECL _llSearchForAsyncNode
(
    ALISTNODE* pHead,
    S16BIT nMsgID,
    ALISTNODE** ppSearch
);

/* _llSearchForAsyncNodebyOp -> search for node in list */
S16BIT _DECL _llSearchForAsyncNodebyOp
(
    ALISTNODE* pHead,
    S16BIT nOpID,
    ALISTNODE** ppSearch
);

/* _llRemoveAsyncNodeFromID -> removes and deletes from list given ID */
S16BIT _DECL _llRemoveAsyncNodeFromID
(
    ALISTNODE** ppHead,
    S16BIT nID
);

/* _llRemoveAllAsyncNodes -> removes all async nodes from a list */
S16BIT _DECL _llRemoveAllAsyncNodes
(
    ALISTNODE** ppHead
);

/* _llCreateFrameNode -> malloc's a new frame node */
FLISTNODE* _DECL _llCreateFrameNode
(
    S16BIT nID
);

/* _llInsertFrameNode -> inserts node to end list */
S16BIT _DECL _llInsertFrameNode
(
    FLISTNODE** ppHead,
    FLISTNODE* pNode
);

/* _llSearchForFrameNode -> search for node in list */
S16BIT _DECL _llSearchForFrameNode
(
    FLISTNODE* pHead,
    S16BIT nMsgID,
    FLISTNODE** ppSearch
);

/* _llRemoveAllFrameNodes -> removes all async nodes from a list */
S16BIT _DECL _llRemoveAllFrameNodes
(
    FLISTNODE** ppHead
);

#endif /* ifndef __DLIST_H__ */
