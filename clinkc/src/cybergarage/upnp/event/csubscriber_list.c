/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csubscriber_list.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/event/csubscriber.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_subscriberlist_new
****************************************/

CgUpnpSubscriberList *cg_upnp_subscriberlist_new()
{
	CgUpnpSubscriberList *subscriberList = (CgUpnpSubscriberList *)malloc(sizeof(CgUpnpSubscriberList));
	cg_list_header_init((CgList *)subscriberList);
	return subscriberList;
}

/****************************************
* cg_upnp_subscriberlist_delete
****************************************/

void cg_upnp_subscriberlist_delete(CgUpnpSubscriberList *subscriberList)
{
	cg_upnp_subscriberlist_clear(subscriberList);
	free(subscriberList);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
