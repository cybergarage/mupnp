/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifndef _MUPNP_UTIL_LIST_H_
#define _MUPNP_UTIL_LIST_H_

#include <mupnp/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
 * Define
 ****************************************/

/**
 * Prototype for individual list node destructor functions
 *
 */
typedef void (*MUPNP_LIST_DESTRUCTORFUNC)(void*);

/**
 * Macro for list node members
 *
 */

#define MUPNP_LIST_STRUCT_MEMBERS          \
  /** Marks the beginning of a list */     \
  bool headFlag;                           \
  /** Pointer to the next list node */     \
  struct _mUpnpList* prev;                 \
  /** Pointer to the previous list node */ \
  struct _mUpnpList* next;

/****************************************
 * Data Type
 ****************************************/

/**
 * \brief The generic list interface in mUPnP.
 *
 * Each struct in mUPnP, that is designed to be a part of
 * a list, must have these elements in their definition before the
 * actual struct definition. The struct pointers are then cast to
 * mUpnpList* and operated with mupnp_list_* functions.
 */
typedef struct _mUpnpList {
  MUPNP_LIST_STRUCT_MEMBERS
} mUpnpList;

/****************************************
 * Functions
 ****************************************/

/**
 * Initialize a list node to act as the first node
 *
 * \param list List node
 */
void mupnp_list_header_init(mUpnpList* list);

/**
 * Initialize a list node to act as a regular node, part of a list.
 *
 * \param list List node
 */
void mupnp_list_node_init(mUpnpList* list);

/**
 * Insert a list node or a complete list structure after the given node
 *
 * \param prevList Insert after this node
 * \param list List node or list structure to insert
 */
void mupnp_list_insert(mUpnpList* prevList, mUpnpList* list);

/**
 * \todo This works essentially like insert, although it is more error-prone?!
 * \todo There might be a bug in this function.
 *
 */
void mupnp_list_add(mUpnpList* headList, mUpnpList* list);

/**
 * Remove a node from a list. Does not free any memory, but only removes
 * the next and previous link associations regarding the given node.
 *
 * \param list List node to remove
 */
void mupnp_list_remove(mUpnpList* list);

/**
 * Get the number of nodes in the current list structure. Counts forwards from the given
 * node, so if you want to get the complete size, give a header node as the parameter.
 *
 * \param headList List header
 */
int mupnp_list_size(mUpnpList* headList);

/**
 * Get an item from the list by the item's index
 *
 * \param headList List header
 * \param index The index of the item to get
 */
mUpnpList* mupnp_list_get(mUpnpList* headList, int index);

/**
 * Get the previous node. Wrap around if the beginning has been reached.
 * 
 *
 * \param list Current node
 */
mUpnpList* mupnp_list_prev_circular(mUpnpList* list);

/**
 * Get the previous node. Returns NULL if beginning has been reached
 *
 * \param list Current node
 */
mUpnpList* mupnp_list_prev(mUpnpList* list);

/**
 * Get the next node. Wrap around if the end has been reached.
 *
 * \param list Current node
 */
mUpnpList* mupnp_list_next_circular(mUpnpList* list);

/**
 * Get the next node. Returns NULL if end has been reached.
 *
 * \param list Current node
 */
mUpnpList* mupnp_list_next(mUpnpList* list);

/**
 * Clear the list and delete all of its contents with \ref MUPNP_LIST_DESTRUCTORFUNC
 *
 * \param headList List header
 * \param destructorFunc Function pointer that clears the contents of individual nodes
 */
void mupnp_list_clear(mUpnpList* headList, MUPNP_LIST_DESTRUCTORFUNC destructorFunc);

/**
 * Get the first actual item from a list for iteration
 *
 * \param headList List header
 */
#define mupnp_list_gets(headList) mupnp_list_next(headList)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
