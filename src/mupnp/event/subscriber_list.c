/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csubscriber_list.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*	04/03/06
*		- added search for SID 
*	10/22/07 Aapo Makela
*		- Fixed search for SID
*	
******************************************************************/

#include <mupnp/event/subscriber.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_subscriberlist_new
****************************************/

CgUpnpSubscriberList *mupnp_upnp_subscriberlist_new()
{
	CgUpnpSubscriberList *subscriberList;

	mupnp_log_debug_l4("Entering...\n");

	subscriberList = (CgUpnpSubscriberList *)malloc(sizeof(CgUpnpSubscriberList));

	if ( NULL != subscriberList )
		mupnp_list_header_init((CgList *)subscriberList);

	return subscriberList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_subscriberlist_delete
****************************************/

void mupnp_upnp_subscriberlist_delete(CgUpnpSubscriberList *subscriberList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_subscriberlist_clear(subscriberList);
	free(subscriberList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_subscriberlist_get
****************************************/

CgUpnpSubscriber *mupnp_upnp_subscriberlist_get(CgUpnpSubscriberList *subscriberList, char *sid)
{
	CgUpnpSubscriber *sub;
	ssize_t uuidIdx;

	if (mupnp_strlen(sid) <= 0)
		return NULL;
	
	uuidIdx = mupnp_strstr(sid, CG_UPNP_ST_UUID_DEVICE);
	if (0 <= uuidIdx)
		sid += (uuidIdx + mupnp_strlen(CG_UPNP_ST_UUID_DEVICE) + 1);

	for (sub = mupnp_upnp_subscriberlist_gets(subscriberList); sub != NULL; sub = mupnp_upnp_subscriber_next(sub)) {
		if (mupnp_streq(sid, mupnp_upnp_subscriber_getsid(sub)) == TRUE)
			return sub;
	}
	
	return NULL;
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
