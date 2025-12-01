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
 * @brief Initialize a list node as a list header (sentinel node)
 *
 * @details
 * Initializes a node to act as a circular doubly-linked list header.
 * The header is a sentinel node that marks the beginning and end of the list.
 * It simplifies list operations by eliminating special cases for empty lists.
 *
 * After initialization:
 * - prev and next point to the node itself (empty circular list)
 * - headFlag is set to true
 *
 * @param list The node to initialize as a header. Must not be NULL.
 *
 * @note Thread-safe: Can be called from any thread, but typically called
 *       during list creation before the list is shared between threads.
 * @note The header node itself does not contain data; it only manages links.
 *
 * @see mupnp_list_node_init()
 * @see mupnp_list_add()
 */
void mupnp_list_header_init(mUpnpList* list);

/**
 * @brief Initialize a list node as a regular data node
 *
 * @details
 * Initializes a node to act as a regular list member (not a header).
 * The node is initially not linked to any list.
 *
 * After initialization:
 * - prev and next are set to NULL
 * - headFlag is set to false
 *
 * @param list The node to initialize. Must not be NULL.
 *
 * @note Thread-safe: Can be called from any thread, but typically called
 *       before the node is added to a shared list.
 * @note The node must be properly populated with data before adding to a list.
 *
 * @see mupnp_list_header_init()
 * @see mupnp_list_insert()
 * @see mupnp_list_add()
 */
void mupnp_list_node_init(mUpnpList* list);

/**
 * @brief Insert a node or list segment after a given node
 *
 * @details
 * Inserts a single node or an entire list segment immediately after the
 * specified node in the list. This maintains the circular doubly-linked
 * structure and properly updates all prev/next pointers.
 *
 * If inserting multiple nodes, they should already be linked together
 * (forming a chain).
 *
 * @param prevList The node after which to insert. Must not be NULL and
 *                 should be part of a properly initialized list.
 * @param list The node or list segment to insert. Must not be NULL.
 *
 * @note Thread-safe: This function is NOT thread-safe. The caller must
 *       ensure exclusive access to the list (e.g., via mutex) when
 *       modifying list structure.
 * @note Side effect: Updates prev/next pointers of adjacent nodes.
 *
 * @see mupnp_list_add()
 * @see mupnp_list_remove()
 */
void mupnp_list_insert(mUpnpList* prevList, mUpnpList* list);

/**
 * @brief Add a node to the end of a list
 *
 * @details
 * Appends a node to the end of a list (just before the header node).
 * This is a convenience function for adding elements to the tail of a list.
 *
 * @param headList The list header node. Must not be NULL and must be
 *                 initialized with mupnp_list_header_init().
 * @param list The node to add. Must not be NULL and should be initialized
 *             with mupnp_list_node_init().
 *
 * @note Thread-safe: This function is NOT thread-safe. The caller must
 *       ensure exclusive access to the list when modifying structure.
 * @note Side effect: Updates prev/next pointers of the last node and header.
 *
 * @todo Review implementation for potential edge cases with empty lists.
 *
 * @see mupnp_list_insert()
 * @see mupnp_list_remove()
 */
void mupnp_list_add(mUpnpList* headList, mUpnpList* list);

/**
 * @brief Remove a node from its list
 *
 * @details
 * Removes a node from the list it belongs to by updating the prev/next
 * pointers of adjacent nodes. This does NOT free any memory; it only
 * unlinks the node from the list structure.
 *
 * After removal, the node's prev and next pointers point to itself,
 * making it safe to check if a node is in a list.
 *
 * @param list The node to remove. Must not be NULL and should be part of
 *             a list (not a standalone node).
 *
 * @note Thread-safe: This function is NOT thread-safe. The caller must
 *       ensure exclusive access to the list when modifying structure.
 * @note Memory management: The caller is responsible for freeing the node's
 *       memory if needed after removal.
 * @note Side effect: Updates prev/next pointers of adjacent nodes.
 * @note Safe to call on an already removed node (has no effect).
 *
 * @see mupnp_list_add()
 * @see mupnp_list_clear()
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
