/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*	File: cthread_list.c
*
*	Revision:
*
*	11/10/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/cevent.h>

/****************************************
* cg_upnp_eventlistenerlist_new
****************************************/

CgUpnpEventListenerList *cg_upnp_eventlistenerlist_new()
{
	CgUpnpEventListenerList *eventListenerList = (CgUpnpEventListenerList *)malloc(sizeof(CgUpnpEventListenerList));
	cg_list_header_init((CgList *)eventListenerList);
	return eventListenerList;
}

/****************************************
* cg_upnp_eventlistenerlist_delete
****************************************/

void cg_upnp_eventlistenerlist_delete(CgUpnpEventListenerList *eventListenerList)
{
	cg_upnp_eventlistenerlist_clear(eventListenerList);
	free(eventListenerList);
}

/****************************************
* cg_upnp_eventlistenerlist_remove
****************************************/

void cg_upnp_eventlistenerlist_remove(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	if (listener == NULL) return;
	
	for (list_node = cg_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = cg_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener == listener)
		{
			cg_list_remove((CgList*)list_node);
			free(list_node);
			break;
		}
	}
	
}

/****************************************
* cg_upnp_eventlistenerlist_add
****************************************/

void cg_upnp_eventlistenerlist_add(CgUpnpEventListenerList* eventListenerList, CG_UPNP_EVENT_LISTENER listener)
{
	CgUpnpEventListenerList *list_node;
	
	if (listener == NULL) return;
	
	list_node = (CgUpnpEventListenerList*)malloc(sizeof(CgUpnpEventListenerList));
	
	list_node->listener = listener;
	cg_list_add((CgList*)eventListenerList, (CgList*)list_node);
}

/****************************************
* cg_upnp_eventlistenerlist_notify
****************************************/

void cg_upnp_eventlistenerlist_notify(CgUpnpEventListenerList* eventListenerList, CgUpnpProperty *property)
{
	CgUpnpEventListenerList *list_node = NULL;
	
	for (list_node = cg_upnp_eventlistenerlist_gets(eventListenerList);
	     list_node != NULL;
	     list_node = cg_upnp_eventlistenerlist_next(list_node))
	{
		if (list_node->listener != NULL)
		{
			list_node->listener(property);
		}
	}
}
