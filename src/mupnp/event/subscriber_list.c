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

#include <mupnp/event/subscriber.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_subscriberlist_new
****************************************/

mUpnpUpnpSubscriberList *mupnp_upnp_subscriberlist_new()
{
	mUpnpUpnpSubscriberList *subscriberList;

	mupnp_log_debug_l4("Entering...\n");

	subscriberList = (mUpnpUpnpSubscriberList *)malloc(sizeof(mUpnpUpnpSubscriberList));

	if ( NULL != subscriberList )
		mupnp_list_header_init((mUpnpList *)subscriberList);

	return subscriberList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_subscriberlist_delete
****************************************/

void mupnp_upnp_subscriberlist_delete(mUpnpUpnpSubscriberList *subscriberList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_subscriberlist_clear(subscriberList);
	free(subscriberList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_subscriberlist_get
****************************************/

mUpnpUpnpSubscriber *mupnp_upnp_subscriberlist_get(mUpnpUpnpSubscriberList *subscriberList, char *sid)
{
	mUpnpUpnpSubscriber *sub;
	ssize_t uuidIdx;

	if (mupnp_strlen(sid) <= 0)
		return NULL;
	
	uuidIdx = mupnp_strstr(sid, MUPNP_ST_UUID_DEVICE);
	if (0 <= uuidIdx)
		sid += (uuidIdx + mupnp_strlen(MUPNP_ST_UUID_DEVICE) + 1);

	for (sub = mupnp_upnp_subscriberlist_gets(subscriberList); sub != NULL; sub = mupnp_upnp_subscriber_next(sub)) {
		if (mupnp_streq(sid, mupnp_upnp_subscriber_getsid(sub)) == TRUE)
			return sub;
	}
	
	return NULL;
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
