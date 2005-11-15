/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontrolpoint_http_server.c
*
*	Revision:
*
*	07/07/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/event/cnotify.h>

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_CONTROLPOINT)

/****************************************
* cg_upnp_controlpoint_httprequestrecieved
****************************************/

void cg_upnp_controlpoint_httprequestrecieved(CgHttpRequest *httpReq)
{
	CgUpnpControlPoint *ctrlPoint;
	CgUpnpNotifyRequest *notifyReq;
	CgUpnpPropertyList *propList;
	CgUpnpProperty *prop;
	CgUpnpEventListenerList *eventListeners;
	
	ctrlPoint = (CgUpnpControlPoint *)cg_http_request_getuserdata(httpReq);
		
	eventListeners = cg_upnp_controlpoint_geteventlisteners(ctrlPoint);

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	if (cg_http_request_isnotiryrequest(httpReq) == TRUE) {	
		notifyReq = cg_upnp_event_notify_request_new();
		cg_upnp_event_notify_request_sethttprequest(notifyReq, httpReq);
	
		propList = cg_upnp_event_notify_request_getpropertylist(notifyReq); 
		for (prop=cg_upnp_propertylist_gets(propList); prop != NULL; prop = cg_upnp_property_next(prop)) {
			cg_upnp_eventlistenerlist_notify(eventListeners, prop);
		}	
		
		cg_upnp_event_notify_request_delete(notifyReq);
		cg_http_request_postokequest(httpReq);
		
		return;
	}
#endif
	
	cg_http_request_postbadrequest(httpReq);
}

/****************************************
* CG_UPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif
