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
*	File: cquery_ctrl.c
*
*	Revision:
*
*	06/09/05
*		- first revision
*
******************************************************************/

#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* mupnp_upnp_action_performlistener
****************************************/

BOOL mupnp_upnp_statevariable_performlistner(CgUpnpStateVariable *statVar, CgUpnpQueryRequest *queryReq)
{
	CG_UPNP_STATEVARIABLE_LISTNER	 listener;
	CgUpnpQueryResponse *queryRes;
	CgHttpRequest *queryReqHttpReq;
	CgHttpResponse *queryResHttpRes;
	
	mupnp_log_debug_l4("Entering...\n");

	listener = mupnp_upnp_statevariable_getlistener(statVar);
	if (listener == NULL)
		return FALSE;
	
	queryRes = mupnp_upnp_control_query_response_new();

	mupnp_upnp_statevariable_setstatuscode(statVar, CG_UPNP_STATUS_INVALID_ACTION);
	mupnp_upnp_statevariable_setstatusdescription(statVar, mupnp_upnp_status_code2string(CG_UPNP_STATUS_INVALID_ACTION));
	mupnp_upnp_statevariable_setvalue(statVar, "");
	
	if (listener(statVar) == TRUE)
		mupnp_upnp_control_query_response_setresponse(queryRes, statVar);
	else
		mupnp_upnp_control_soap_response_setfaultresponse(mupnp_upnp_control_query_response_getsoapresponse(queryRes), mupnp_upnp_statevariable_getstatuscode(statVar), mupnp_upnp_statevariable_getstatusdescription(statVar));
	
	queryReqHttpReq = mupnp_soap_request_gethttprequest(mupnp_upnp_control_query_request_getsoaprequest(queryReq));
	queryResHttpRes = mupnp_soap_response_gethttpresponse(mupnp_upnp_control_query_response_getsoapresponse(queryRes));
	mupnp_http_request_postresponse(queryReqHttpReq, queryResHttpRes);	

	mupnp_upnp_control_query_response_delete(queryRes);
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_statevariable_post
****************************************/

BOOL mupnp_upnp_statevariable_post(CgUpnpStateVariable *statVar)
{
	CgUpnpQueryRequest *queryReq;
	CgUpnpQueryResponse *queryRes;
	BOOL querySuccess;
	
	mupnp_log_debug_l4("Entering...\n");

	queryReq = mupnp_upnp_control_query_request_new();
	
	mupnp_upnp_control_query_request_setstatevariable(queryReq, statVar);
	queryRes = mupnp_upnp_control_query_request_post(queryReq);
	querySuccess = mupnp_upnp_control_query_response_issuccessful(queryRes);
	mupnp_upnp_statevariable_setvalue(statVar, (querySuccess == TRUE) ? mupnp_upnp_control_query_response_getreturnvalue(queryRes) : "");
	
	mupnp_upnp_control_query_request_delete(queryReq);
	
	mupnp_log_debug_l4("Leaving...\n");

	return querySuccess;
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif
