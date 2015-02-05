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
*	File: caction_ctrl.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
*
*	10/31/05
*		- mupnp_upnp_action_post: If action post was unsuccessful, put the 
*		  error message to the action response
*
******************************************************************/

#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* mupnp_upnp_action_clearoutputargumentvalues
****************************************/

void mupnp_upnp_action_clearoutputargumentvalues(CgUpnpAction *action)
{
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	
	mupnp_log_debug_l4("Entering...\n");

	argList = mupnp_upnp_action_getargumentlist(action);
	for (arg=mupnp_upnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_upnp_argument_next(arg)) {
		if (mupnp_upnp_argument_isoutdirection(arg) == TRUE)
			mupnp_upnp_argument_setvalue(arg, "");
	}

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_action_performlistener
****************************************/

BOOL mupnp_upnp_action_performlistner(CgUpnpAction *action, CgUpnpActionRequest *actionReq)
{
	CG_UPNP_ACTION_LISTNER listener;
	CgUpnpActionResponse *actionRes;
	CgHttpRequest *actionReqHttpReq;
	CgHttpResponse *actionResHttpRes;
	
	mupnp_log_debug_l4("Entering...\n");

	listener = mupnp_upnp_action_getlistner(action);
	if (listener == NULL)
		return FALSE;

	actionRes = mupnp_upnp_control_action_response_new();
	
	mupnp_upnp_action_setstatuscode(action, CG_UPNP_STATUS_INVALID_ACTION);
	mupnp_upnp_action_setstatusdescription(action, mupnp_upnp_status_code2string(CG_UPNP_STATUS_INVALID_ACTION));

	mupnp_upnp_action_clearoutputargumentvalues(action);
	
	if (listener(action) == TRUE)
		mupnp_upnp_control_action_response_setresponse(actionRes, action);
	else
		mupnp_upnp_control_soap_response_setfaultresponse(mupnp_upnp_control_action_response_getsoapresponse(actionRes), mupnp_upnp_action_getstatuscode(action), mupnp_upnp_action_getstatusdescription(action));
	
	actionReqHttpReq = mupnp_soap_request_gethttprequest(mupnp_upnp_control_action_request_getsoaprequest(actionReq));
	actionResHttpRes = mupnp_soap_response_gethttpresponse(mupnp_upnp_control_action_response_getsoapresponse(actionRes));
	mupnp_http_request_postresponse(actionReqHttpReq, actionResHttpRes);	

	mupnp_upnp_control_action_response_delete(actionRes);
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_upnp_action_post
****************************************/

BOOL mupnp_upnp_action_post(CgUpnpAction *action)
{
	CgUpnpActionRequest *actionReq;
	CgUpnpActionResponse *actionRes;
	BOOL actionSuccess;

	mupnp_log_debug_l4("Entering...\n");
	
	actionReq = mupnp_upnp_control_action_request_new();
	
	mupnp_upnp_control_action_request_setaction(actionReq, action);
	actionRes = mupnp_upnp_control_action_request_post(actionReq);
	actionSuccess = mupnp_upnp_control_action_response_issuccessful(actionRes);
	if (actionSuccess == TRUE) {
                /* Reset status code to 0 (otherwise latest error stays in action) */
                mupnp_upnp_action_setstatuscode(action, 0);
		if (mupnp_upnp_control_action_response_getresult(actionRes, action) == FALSE) {
			actionSuccess = FALSE;
		}
	} else {
		/* Action was unsuccesful, but put the error to the action */
		mupnp_upnp_control_action_response_geterror(actionRes, action);
	}

	mupnp_upnp_control_action_request_delete(actionReq);
	
	mupnp_log_debug_l4("Leaving...\n");

	return actionSuccess;
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

