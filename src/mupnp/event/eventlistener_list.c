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

#include <mupnp/event/event.h>
#include <mupnp/util/log.h>

#include <string.h>

/**
 * Create a new event listener list
 *
 */
mUpnpEventListenerList* mupnp_eventlistenerlist_new(void)
{
  mUpnpEventListenerList* eventListenerList;

  mupnp_log_debug_l4("Entering...\n");

  eventListenerList = (mUpnpEventListenerList*)malloc(sizeof(mUpnpEventListenerList));

  if (NULL != eventListenerList)
    mupnp_list_header_init((mUpnpList*)eventListenerList);

  return eventListenerList;

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Delete a event listener list.
 *
 * @param eventListenerList The event listener list to delete
 *
 */
void mupnp_eventlistenerlist_delete(mUpnpEventListenerList* eventListenerList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_eventlistenerlist_clear(eventListenerList);
  free(eventListenerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Remove a listener from the event listener list
 *
 * @param eventListenerList The event listener list
 * @param listener The listener to remove
 *
 */
void mupnp_eventlistenerlist_remove(mUpnpEventListenerList* eventListenerList, MUPNP_EVENT_LISTENER listener)
{
  mUpnpEventListenerList* listNode = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (listener == NULL)
    return;

  for (listNode = mupnp_eventlistenerlist_gets(eventListenerList);
       listNode != NULL;
       listNode = mupnp_eventlistenerlist_next(listNode)) {
    if (listNode->listener == listener) {
      mupnp_list_remove((mUpnpList*)listNode);
      free(listNode);
      break;
    }
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Add a listener to the event listener list
 *
 * @param eventListenerList The event listener list
 * @param listener The listener to add
 *
 */
void mupnp_eventlistenerlist_add(mUpnpEventListenerList* eventListenerList, MUPNP_EVENT_LISTENER listener)
{
  mUpnpEventListenerList* listNode;

  mupnp_log_debug_l4("Entering...\n");

  if (listener == NULL)
    return;

  listNode = (mUpnpEventListenerList*)malloc(sizeof(mUpnpEventListenerList));

  if (NULL != listNode) {
    memset(listNode, 0, sizeof(mUpnpEventListenerList));
    listNode->listener = listener;
    mupnp_list_node_init((mUpnpList*)listNode);
    mupnp_list_add((mUpnpList*)eventListenerList, (mUpnpList*)listNode);
  }
  else
    mupnp_log_debug_s("Memory allocation failure!\n");

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Call all event listeners in the list with the given evented data.
 *
 * @param eventListenerList The list to iterate thru
 * @param property The property that has been evented
 */
void mupnp_eventlistenerlist_notify(mUpnpEventListenerList* eventListenerList, mUpnpProperty* property)
{
  mUpnpEventListenerList* listNode = NULL;

  mupnp_log_debug_l4("Entering...\n");

  for (listNode = mupnp_eventlistenerlist_gets(eventListenerList);
       listNode != NULL;
       listNode = mupnp_eventlistenerlist_next(listNode)) {
    if (listNode->listener != NULL) {
      listNode->listener(property);
    }
  }

  mupnp_log_debug_l4("Leaving...\n");
}
