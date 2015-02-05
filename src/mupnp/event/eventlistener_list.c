/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cthread_list.c
*
*	Revision:
*
*	11/10/05
*		- first revision
*
******************************************************************/

#include <mupnp/event/event.h>
#include <mupnp/util/log.h>

#include <string.h>

/**
 * Create a new event listener list
 *
 */
CgUpnpEventListenerList *mupnp_upnp_eventlistenerlist_new()
{
	CgUpnpEventListenerList *eventListenerList;

	mupnp_log_debug_l4("Entering...\n");

	eventListenerList = (CgUpnpEventListenerList *)malloc(sizeof(CgUpnpEventListenerList));

	if ( NULL != eventListenerList )
		mupnp_list_header_init((CgList *)eventListenerList);

	return eventListenerList;

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * Delete a event listener list.
 *
 * @param eventListenerList The event listener list to delete
 *
 */
void mupnp_upnp_eventlistenerlist_delete(CgUpnpEventListenerList *eventListenerList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_eventlistenerlist_clear(eventListenerList);
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
void mupnp_upnp_eventlistenerlist_remove(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	if (listener == NULL) return;
	
	for (list_node = mupnp_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = mupnp_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener == listener)
		{
			mupnp_list_remove((CgList*)list_node);
			free(list_node);
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
void mupnp_upnp_eventlistenerlist_add(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node;
	
	mupnp_log_debug_l4("Entering...\n");

	if (listener == NULL) return;
	
	list_node = (CgUpnpEventListenerList*)malloc(sizeof(CgUpnpEventListenerList));

	if ( NULL != list_node )
	{	
		memset(list_node, 0, sizeof(CgUpnpEventListenerList));
		list_node->listener = listener;
		mupnp_list_node_init((CgList *)list_node);
		mupnp_list_add((CgList*)eventListenerList, (CgList*)list_node);
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
void mupnp_upnp_eventlistenerlist_notify(CgUpnpEventListenerList* eventListenerList, CgUpnpProperty *property)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	mupnp_log_debug_l4("Entering...\n");

	for (list_node = mupnp_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = mupnp_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener != NULL)
		{
			list_node->listener(property);
		}
	}

	mupnp_log_debug_l4("Leaving...\n");
}
