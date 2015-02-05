/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cservice_notify.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#include <mupnp/service.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_upnp_service_notifymain
****************************************/

static BOOL mupnp_upnp_service_notifymain(CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	CgUpnpSubscriber *sub;
	CgUpnpSubscriber **subArray;
	int subArrayCnt;
	int n;
		
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_service_lock(service);

	/**** Remove expired subscribers ****/
	subArrayCnt = mupnp_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);

	if ( NULL == subArray )
	{
		mupnp_log_debug_s("Memory allocation problem!\n");
		mupnp_upnp_service_unlock(service);
		return FALSE;
	}

	sub = mupnp_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = mupnp_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (mupnp_upnp_subscriber_isexpired(sub) == TRUE)
			mupnp_upnp_service_removesubscriber(service, sub);
	}
	free(subArray);
		
	/**** Notify to subscribers ****/
	subArrayCnt = mupnp_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);

	if ( NULL == subArray ) {
		mupnp_log_debug_s("Memory allocation problem!\n");
		mupnp_upnp_service_unlock(service);
		return FALSE;
	}

	sub = mupnp_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = mupnp_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (statVar) {
			if (mupnp_upnp_subscriber_notify(sub, statVar) == FALSE) {
				/**** remove invalid the subscriber but don't remove in NMPR specification ****/
				mupnp_upnp_service_removesubscriber(service, sub);
			}
		}
		else {
			if (mupnp_upnp_subscriber_notifyall(sub, service) == FALSE) {
				/**** remove invalid the subscriber but don't remove in NMPR specification ****/
				mupnp_upnp_service_removesubscriber(service, sub);
			}
		}
	}
	free(subArray);
	
	mupnp_upnp_service_unlock(service);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_service_notify
****************************************/

BOOL mupnp_upnp_service_notify(CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	return mupnp_upnp_service_notifymain(service, statVar);
}

/****************************************
* mupnp_upnp_service_notifyall
****************************************/

BOOL mupnp_upnp_service_notifyallbracket(CgUpnpService *service)
{
	return mupnp_upnp_service_notifymain(service, NULL);
}

/****************************************
* mupnp_upnp_service_notifyall
****************************************/

BOOL mupnp_upnp_service_notifyall(CgUpnpService *service, BOOL doBracket)
{
	CgUpnpStateVariable *statVar;
	
	mupnp_log_debug_l4("Entering...\n");

	if (doBracket) {
		mupnp_upnp_service_notifyallbracket(service);
	}
	else {
		for (statVar = mupnp_upnp_service_getstatevariables(service); statVar != NULL; statVar = mupnp_upnp_statevariable_next(statVar)) {
			if (mupnp_upnp_statevariable_issendevents(statVar) == TRUE)
				mupnp_upnp_service_notify(service, statVar);
		}
	}

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
 * mupnp_upnp_service_notifyall
 ****************************************/

static void mupnp_upnp_service_notifyall_thread(CgThread *thread)
{
	CgUpnpService *service;
	
	service = (CgUpnpService *)mupnp_thread_getuserdata(thread);
	mupnp_upnp_service_notifyall(service, TRUE);
	mupnp_thread_delete(thread);	
}

void mupnp_upnp_service_createnotifyallthread(CgUpnpService *service, CgTime waitTime)
{
	CgThread *thread;
	
	thread = mupnp_thread_new();
	mupnp_thread_setaction(thread, mupnp_upnp_service_notifyall_thread);
	mupnp_thread_setuserdata(thread, service);
	
	mupnp_wait(waitTime);
	mupnp_thread_start(thread);
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
