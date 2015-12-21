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

#include <mupnp/control/control.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

/****************************************
* mupnp_action_clearoutputargumentvalues
****************************************/

void mupnp_action_clearoutputargumentvalues(mUpnpAction* action)
{
  mUpnpArgumentList* argList;
  mUpnpArgument* arg;

  mupnp_log_debug_l4("Entering...\n");

  argList = mupnp_action_getargumentlist(action);
  for (arg = mupnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isoutdirection(arg) == true)
      mupnp_argument_setvalue(arg, "");
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_action_performlistener
****************************************/

bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq)
{
  MUPNP_ACTION_LISTNER listener;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* actionReqHttpReq;
  mUpnpHttpResponse* actionResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_action_getlistner(action);
  if (listener == NULL)
    return false;

  actionRes = mupnp_control_action_response_new();

  mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ACTION);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));

  mupnp_action_clearoutputargumentvalues(action);

  if (listener(action) == true)
    mupnp_control_action_response_setresponse(actionRes, action);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_action_response_getsoapresponse(actionRes), mupnp_action_getstatuscode(action), mupnp_action_getstatusdescription(action));

  actionReqHttpReq = mupnp_soap_request_gethttprequest(mupnp_control_action_request_getsoaprequest(actionReq));
  actionResHttpRes = mupnp_soap_response_gethttpresponse(mupnp_control_action_response_getsoapresponse(actionRes));
  mupnp_http_request_postresponse(actionReqHttpReq, actionResHttpRes);

  mupnp_control_action_response_delete(actionRes);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_action_post
****************************************/

bool mupnp_action_post(mUpnpAction* action)
{
  mUpnpActionRequest* actionReq;
  mUpnpActionResponse* actionRes;
  bool actionSuccess;

  mupnp_log_debug_l4("Entering...\n");

  actionReq = mupnp_control_action_request_new();

  mupnp_control_action_request_setaction(actionReq, action);
  actionRes = mupnp_control_action_request_post(actionReq);
  actionSuccess = mupnp_control_action_response_issuccessful(actionRes);
  if (actionSuccess == true) {
    /* Reset status code to 0 (otherwise latest error stays in action) */
    mupnp_action_setstatuscode(action, 0);
    if (mupnp_control_action_response_getresult(actionRes, action) == false) {
      actionSuccess = false;
    }
  }
  else {
    /* Action was unsuccesful, but put the error to the action */
    mupnp_control_action_response_geterror(actionRes, action);
  }

  mupnp_control_action_request_delete(actionReq);

  mupnp_log_debug_l4("Leaving...\n");

  return actionSuccess;
}

/****************************************
* MUPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif
