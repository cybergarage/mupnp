/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cservice_notify.c
*
*	Revision:
*
*	06/21/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cservice.h>

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

/****************************************
* cg_upnp_service_notify
****************************************/

BOOL cg_upnp_service_notify(CgUpnpService *service, CgUpnpStateVariable *statVar)
{
	CgUpnpSubscriber *sub;
	CgUpnpSubscriber **subArray;
	int subArrayCnt;
	int n;
		
	/**** Remove expired subscribers ****/
	subArrayCnt = cg_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);
	sub = cg_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = cg_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (cg_upnp_subscriber_isexpired(sub) == TRUE)
			cg_upnp_service_removesubscriber(service, sub);
	}
	free(subArray);
		
	/**** Notify to subscribers ****/
	subArrayCnt = cg_upnp_service_getnsubscribers(service);
	subArray = (CgUpnpSubscriber **)malloc(sizeof(CgUpnpSubscriber *) * subArrayCnt);
	sub = cg_upnp_service_getsubscribers(service);
	for (n=0; n<subArrayCnt; n++) {
		subArray[n] = sub;
		sub = cg_upnp_subscriber_next(sub);
	}
	for (n=0; n<subArrayCnt; n++) {
		sub = subArray[n];
		if (sub == NULL)
			continue;
		if (cg_upnp_subscriber_notify(sub, statVar) == FALSE) {
			/**** remove invalid the subscriber but don't remove in NMPR specification ****/
			cg_upnp_service_removesubscriber(service, sub);
		}
	}
	free(subArray);

	return TRUE;
}

/****************************************
* cg_upnp_service_notifyall
****************************************/

BOOL cg_upnp_service_notifyallstatevariables(CgUpnpService *service)
{
	CgUpnpStateVariable *statVar;
	
	for (statVar = cg_upnp_service_getstatevariables(service); statVar != NULL; statVar = cg_upnp_statevariable_next(statVar)) {
		if (cg_upnp_statevariable_issendevents(statVar) == TRUE)
			cg_upnp_service_notify(service, statVar);
	}

	return TRUE;
}

/****************************************
* CG_UPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
