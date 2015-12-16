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
* MUPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_QUERYCTRL)

/****************************************
* mupnp_action_performlistener
****************************************/

bool mupnp_statevariable_performlistner(mUpnpStateVariable* statVar, mUpnpQueryRequest* queryReq)
{
  MUPNP_STATEVARIABLE_LISTNER listener;
  mUpnpQueryResponse* queryRes;
  mUpnpHttpRequest* queryReqHttpReq;
  mUpnpHttpResponse* queryResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_statevariable_getlistener(statVar);
  if (listener == NULL)
    return false;

  queryRes = mupnp_control_query_response_new();

  mupnp_statevariable_setstatuscode(statVar, MUPNP_STATUS_INVALID_ACTION);
  mupnp_statevariable_setstatusdescription(statVar, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));
  mupnp_statevariable_setvalue(statVar, "");

  if (listener(statVar) == true)
    mupnp_control_query_response_setresponse(queryRes, statVar);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_query_response_getsoapresponse(queryRes), mupnp_statevariable_getstatuscode(statVar), mupnp_statevariable_getstatusdescription(statVar));

  queryReqHttpReq = mupnp_soap_request_gethttprequest(mupnp_control_query_request_getsoaprequest(queryReq));
  queryResHttpRes = mupnp_soap_response_gethttpresponse(mupnp_control_query_response_getsoapresponse(queryRes));
  mupnp_http_request_postresponse(queryReqHttpReq, queryResHttpRes);

  mupnp_control_query_response_delete(queryRes);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_statevariable_post
****************************************/

bool mupnp_statevariable_post(mUpnpStateVariable* statVar)
{
  mUpnpQueryRequest* queryReq;
  mUpnpQueryResponse* queryRes;
  bool querySuccess;

  mupnp_log_debug_l4("Entering...\n");

  queryReq = mupnp_control_query_request_new();

  mupnp_control_query_request_setstatevariable(queryReq, statVar);
  queryRes = mupnp_control_query_request_post(queryReq);
  querySuccess = mupnp_control_query_response_issuccessful(queryRes);
  mupnp_statevariable_setvalue(statVar, (querySuccess == true) ? mupnp_control_query_response_getreturnvalue(queryRes) : "");

  mupnp_control_query_request_delete(queryReq);

  mupnp_log_debug_l4("Leaving...\n");

  return querySuccess;
}

/****************************************
* MUPNP_NOUSE_QUERY (End)
****************************************/

#endif
