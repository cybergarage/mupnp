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

#ifndef _MUPNP_CONTROL_H_
#define _MUPNP_CONTROL_H_

#include <mupnp/typedef.h>

#include <mupnp/util/string.h>
#include <mupnp/soap/soap.h>
#include <mupnp/service.h>
#include <mupnp/action.h>
#include <mupnp/argument.h>
#include <mupnp/statevariable.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/**** MAN ****/

/**
 * Definition for mandatory discover header
 */
#define MUPNP_MAN_DISCOVER "\"ssdp:discover\""

/**** NT ****/

/**
 * Definition for NT (Notification type) root device
 */
#define MUPNP_NT_ROOTDEVICE "upnp:rootdevice"

/**
 * Definition for NT event
 */
#define MUPNP_NT_EVENT "upnp:event"

/**** NTS ****/

/**
 * Definition for NTS alive
 */
#define MUPNP_NTS_ALIVE "ssdp:alive"

/**
 * Definition for NTS byebye
 */
#define MUPNP_NTS_BYEBYE "ssdp:byebye"

/**
 * Definition for NTS property change
 */
#define MUPNP_NTS_PROPCHANGE "upnp:propchange"

/**** ST ****/

/**
 * Definition for general ST (search target)
 */
#define MUPNP_ST_ALL_DEVICE "ssdp:all"

/**
 * Definition for root device ST
 */
#define MUPNP_ST_ROOT_DEVICE "upnp:rootdevice"

/**
 * Definition for ST UUID (universally unique identifier) prefix
 */
#define MUPNP_ST_UUID_DEVICE "uuid"

/**
 * Definition for URN device
 */
#define MUPNP_ST_URN "urn"

/**
 * Definition for URN device
 */
#define MUPNP_ST_URN_DEVICE "urn:schemas-upnp-org:device"

/**
 * Definition for URN service
 */
#define MUPNP_ST_URN_SERVICE "urn:schemas-upnp-org:service"

/**** USN ****/

/**
 * Definition for USN rootdevice
 */
#define MUPNP_USN_ROOTDEVICE "upnp:rootdevice"

/**
 * Definition for control name space
 */
#define MUPNP_CONTROL_NS "u"

/**
 * Definition for soap action: state variable query
 */
#define MUPNP_CONTROL_QUERY_SOAPACTION "\"urn:schemas-upnp-org:control-1-0#QueryStateVariable\""

/**
 * Definition for control XML name space
 */
#define MUPNP_CONTROL_XMLNS "urn:schemas-upnp-org:control-1-0"

/**
 * Definition for query state variable
 */
#define MUPNP_CONTROL_QUERY_STATE_VARIABLE "QueryStateVariable"

/**
 * Definition for query state variable response
 */
#define MUPNP_CONTROL_QUERY_STATE_VARIABLE_RESPONSE "QueryStateVariableResponse"

/**
 * Definition for control variable name
 */
#define MUPNP_CONTROL_VAR_NAME "varName"

/**
 * Definition for control return
 */
#define MUPNP_CONTROL_RETURN "return"

/**
 * Definition for control fault code
 */
#define MUPNP_CONTROL_FAULT_CODE "Client"

/**
 * Definition for contol fault string
 */
#define MUPNP_CONTROL_FAULT_STRING "UPnPError"

/** 
 * Definition fro control error code
 */
#define MUPNP_CONTROL_ERROR_CODE "errorCode"

/**
 * Definition for control error description
 */
#define MUPNP_CONTROL_ERROR_DESCRIPTION "errorDescription"

/****************************************
* Data Type
****************************************/

/**** Action ****/

/**
 * Data type definition for action response
 */
typedef struct _mUpnpActionResponse {
  mUpnpSoapResponse* soapRes;
  bool isSoapResCreated;
  mUpnpArgumentList* argList;
} mUpnpActionResponse;

/**
 * Data type definition for action request
 */
typedef struct _mUpnpActionRequest {
  mUpnpSoapRequest* soapReq;
  bool isSoapReqCreated;
  mUpnpArgumentList* argList;
  mUpnpActionResponse* actionRes;
} mUpnpActionRequest;

/**** Query ****/

/**
 * Data type definition for query response
 */
typedef struct _mUpnpQueryResponse {
  mUpnpSoapResponse* soapRes;
  bool isSoapResCreated;
} mUpnpQueryResponse;

/**
 * Data type definition for query request
 */
typedef struct _mUpnpQueryRequest {
  mUpnpSoapRequest* soapReq;
  bool isSoapReqCreated;
  mUpnpQueryResponse* queryRes;
} mUpnpQueryRequest;

/****************************************
* Function
****************************************/

/* After POST has failed we send M-POST and change the SOAPACTION header into 01-SOAPACTION.
 * Next ensures that also in this case query is interpreted as a query.
 */

/**
 * Check if HTTP request query is UPnP query request
 *
 * @param httpReq HTTP request
 *
 * @return True if request is UPnP query request, false othewise
 */
#define mupnp_control_isqueryrequest(httpReq) (((0 <= mupnp_strstr(mupnp_soap_request_getsoapaction(httpReq), MUPNP_CONTROL_QUERY_SOAPACTION)) ? true : false) || ((0 <= mupnp_strstr(mupnp_soap_request_getsoapactionwithns(httpReq), MUPNP_CONTROL_QUERY_SOAPACTION)) ? true : false))

/* Check if HTTP request is UPnP action request
 *
 * @param httpReq HTTP request
 * 
 * @return True if request is UPnP action request, false otherwise
 */
#define mupnp_control_isactionrequest(httpReq) ((mupnp_control_isqueryrequest(httpReq) == true) ? false : true)

/**
 * Initializes soap requests envelope node
 *
 * @param soapReq Soap request
 */
void mupnp_control_soap_request_initializeenvelopenode(mUpnpSoapRequest* soapReq);

/**
 * Initialize soap responses envelope node
 *
 * @param soapRes Soap response
 */
void mupnp_control_soap_response_initializeenvelopenode(mUpnpSoapResponse* soapRes);
mUpnpXmlNode* mupnp_control_soap_response_createfaultresponsenode(int errCode, char* errDescr);

/**
 * Set failed response data
 *
 * @param soapRes Soap response
 * @param errCode Error code
 * @param errDescr Error description
 */
void mupnp_control_soap_response_setfaultresponse(mUpnpSoapResponse* soapRes, int errCode, char* errDescr);

/**
 * Perform action listener
 *
 * @param action Action
 * @param actionReq Action request
 */
bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq);

/**
 * Perform query listener
 *
 * @param statVar State variable
 * @param queryReq Query request
 */
bool mupnp_statevariable_performlistner(mUpnpStateVariable* statVar, mUpnpQueryRequest* queryReq);

/**
 * Set host from service
 *
 * @param soapReq Soap request
 * @param service UPnP service
 */
void mupnp_control_request_sethostfromservice(mUpnpSoapRequest* soapReq, mUpnpService* service);

/****************************************
* Function (ActionRequest)
****************************************/

/**
 * Create new action request object
 */
mUpnpActionRequest* mupnp_control_action_request_new();

/**
 * Delete action request object
 *
 * @param actionReq Action request
 */
void mupnp_control_action_request_delete(mUpnpActionRequest* actionReq);

/**
 * Clear action request
 *
 * @param actionReq Clear Action request
 */
void mupnp_control_action_request_clear(mUpnpActionRequest* actionReq);

/**
 * Set soap request associated with action request
 *
 * @param actionReq Action request
 * @param soapReq Soap request
 */
void mupnp_control_action_request_setsoaprequest(mUpnpActionRequest* actionReq, mUpnpSoapRequest* soapReq);

/**
 * Get soap request associated with action request
 *
 * @param actionReq Action request
 *
 * @return Soap request
 */
#define mupnp_control_action_request_getsoaprequest(actionReq) (actionReq->soapReq)

/**
 * Get argument list from action request
 *
 * @param actionReq Action request
 *
 * @return Argument list
 */
#define mupnp_control_action_request_getargumentlist(actionReq) (actionReq->argList)

/**
 * Get XML action node from action request
 *
 * @param actionReq
 *
 * @return Action request XML node
 */
mUpnpXmlNode* mupnp_control_action_request_getactionnode(mUpnpActionRequest* actionReq);

/**
 * Get action name from action request
 *
 * @param actionReq Action request
 *
 * @return Action name
 */
char* mupnp_control_action_request_getactionname(mUpnpActionRequest* actionReq);

/**
 * Set action to action request
 *
 * @param actionReq Action request
 * @param action UPnP action
 */
void mupnp_control_action_request_setaction(mUpnpActionRequest* actionReq, mUpnpAction* action);

/**
 * Get action response from action request
 *
 * @param actionReq Action request
 *
 * @return Action response
 */
#define mupnp_control_action_request_getactionresponse(actionReq) (actionReq->actionRes)

/**
 * Send action request
 *
 * @param actionReq Action request
 */
mUpnpActionResponse* mupnp_control_action_request_post(mUpnpActionRequest* actionReq);

/****************************************
* Function (ActionResponse)
****************************************/

/**
 * Create new action response
 */
mUpnpActionResponse* mupnp_control_action_response_new();

/**
 * Delete action response
 *
 * @param actionReq Action response
 */
void mupnp_control_action_response_delete(mUpnpActionResponse* actionReq);

/**
 * Clear action response
 *
 * @param actionReq Action response
 */
void mupnp_control_action_response_clear(mUpnpActionResponse* actionReq);

/**
 * Set soap response to action response
 *
 * @param actionRes Action response
 * @param soapRes Soap reaponse
 */
void mupnp_control_action_response_setsoapresponse(mUpnpActionResponse* actionRes, mUpnpSoapResponse* soapRes);

/**
 * Get soap response from action response
 *
 * @param actionRes Action response
 *
 * @return Soap response
 */
#define mupnp_control_action_response_getsoapresponse(actionRes) (actionRes->soapRes)

/**
 * Set action response to action
 *
 * @param actionRes Action response
 * @param action UPnP action
 */
void mupnp_control_action_response_setresponse(mUpnpActionResponse* actionRes, mUpnpAction* action);

/**
 * Check if action response indicates that action invoke was successfull
 *
 * @param actionRes Action response
 *
 * @return True if action has been successful, false otherwise
 */
#define mupnp_control_action_response_issuccessful(actionRes) mupnp_soap_response_issuccessful(actionRes->soapRes)

/** 
 * Get action response XML node from action response
 *
 * @param actionRes Action response
 *
 * @return Action response XML node
 */
mUpnpXmlNode* mupnp_control_action_response_getactionresponsenode(mUpnpActionResponse* actionRes);

/**
 * Get result from action response
 * \todo Check this out more carefully
 *
 * @param actionRes Action response
 * @param action UPnP action
 */
bool mupnp_control_action_response_getresult(mUpnpActionResponse* actionRes, mUpnpAction* action);

/**
 * Get error associated to action response
 * \todo Check this out more carefully
 * @param actionRes Action response
 * @param action UPnP action
 */
bool mupnp_control_action_response_geterror(mUpnpActionResponse* actionRes, mUpnpAction* action);

/****************************************
* Function (QueryRequest)
****************************************/

/**
 * Create new query request object
 */
mUpnpQueryRequest* mupnp_control_query_request_new();

/**
 * Delete query request
 *
 * @param queryReq Query request
 */
void mupnp_control_query_request_delete(mUpnpQueryRequest* queryReq);

/**
 * Clear query request
 *
 * @param queryReq Query request
 */
void mupnp_control_query_request_clear(mUpnpQueryRequest* queryReq);

/**
 * Set soap request for query request
 *
 * @param queryReq Query request
 * @param soapReq Soap request
 */
void mupnp_control_query_request_setsoaprequest(mUpnpQueryRequest* queryReq, mUpnpSoapRequest* soapReq);

/**
 * Get soap request from query request
 *
 * @param queryReq
 *
 * @return Soap request
 */
#define mupnp_control_query_request_getsoaprequest(queryReq) (queryReq->soapReq)

/**
 * Get state variable XML node
 *
 * @param queryReq
 *
 * @return State variable XML node
 */
mUpnpXmlNode* mupnp_control_query_request_getvarnamenode(mUpnpQueryRequest* queryReq);

/**
 * Get state variable name associated with this query request
 *
 * @param queryReq Query request
 *
 * @return State variable variable name
 */
char* mupnp_control_query_request_getvarname(mUpnpQueryRequest* queryReq);

/**
 * Set state variable to query request
 *
 * @param queryReq Query request
 * @param statVar State variable
 */
void mupnp_control_query_request_setstatevariable(mUpnpQueryRequest* queryReq, mUpnpStateVariable* statVar);

/**
 * Get response from query request
 *
 * @param queryReq Query request
 *
 * @return Query response
 */
#define mupnp_control_query_request_getqueryresponse(queryReq) (queryReq->queryRes)

/**
 * Send query request
 *
 * @param queryReq Query request
 *
 * @return Query response
 */
mUpnpQueryResponse* mupnp_control_query_request_post(mUpnpQueryRequest* queryReq);

/****************************************
* Function (QueryResponse)
****************************************/

/**
 * Create new query response
 */
mUpnpQueryResponse* mupnp_control_query_response_new();

/**
 * Delete query response
 *
 * @param queryReq Query request
 */
void mupnp_control_query_response_delete(mUpnpQueryResponse* queryReq);

/**
 * Clear query response
 *
 * @param queryReq Query request
 */
void mupnp_control_query_response_clear(mUpnpQueryResponse* queryReq);

/**
 * Set soap response for query response
 *
 * @param queryRes Query response
 * @param soapRes Soap response
 */
void mupnp_control_query_response_setsoapresponse(mUpnpQueryResponse* queryRes, mUpnpSoapResponse* soapRes);

/**
 * Get soap response from query response
 *
 * @param queryRes Query response
 *
 * @return Soap response
 */
#define mupnp_control_query_response_getsoapresponse(queryRes) (queryRes->soapRes)

/**
 * Create and set XML response data into query response
 *
 * @param queryRes Query response
 * @param statVar State variable
 */
void mupnp_control_query_response_setresponse(mUpnpQueryResponse* queryRes, mUpnpStateVariable* statVar);

/**
 * Check if query response indicates that query request has succeeded
 *
 * @param queryRes Query response
 */
#define mupnp_control_query_response_issuccessful(queryRes) mupnp_soap_response_issuccessful(queryRes->soapRes)

/**
 * Get XML return node from query response
 *
 * @param queryRes
 *
 * @return XML return node
 */
mUpnpXmlNode* mupnp_control_query_response_getreturnnode(mUpnpQueryResponse* queryRes);

/**
 * Get return valuse from query response
 *
 * @param queryRes Query response
 *
 * @return UPnP return value
 */
char* mupnp_control_query_response_getreturnvalue(mUpnpQueryResponse* queryRes);

/****************************************
* Function (MAN)
****************************************/

/**
 * Check if string is correct mandatory header value as specified
 * in UPnP Device Architecture 1.2.2
 *
 * @param str String to be checked
 *
 * @return True if string is exactly the required one, false otherwise
 */
#define mupnp_man_isdiscover(str) ((0 == mupnp_strstr(str, MUPNP_MAN_DISCOVER)) ? true : false)

/****************************************
* Function (ST)
****************************************/

/**
 * Check if device is of type "all"
 *
 * @param str String to be checked
 * 
 * @return True if device type is "all", false otherwise
 */
#define mupnp_st_isalldevice(str) ((0 <= mupnp_strstr(str, MUPNP_ST_ALL_DEVICE)) ? true : false)

/**
 * Check if device string is from root device
 *
 * @param str String to be checked
 *
 * @return True if string is from device type "root device", false otherwise
 */
#define mupnp_st_isrootdevice(str) ((0 <= mupnp_strstr(str, MUPNP_ST_ROOT_DEVICE)) ? true : false)

/**
 * Check if string is from uuid device
 *
 * @param str String to be checked
 *
 * @return True if string is from uuid device, false otherwise
 */
#define mupnp_st_isuuiddevice(str) ((0 <= mupnp_strstr(str, MUPNP_ST_UUID_DEVICE)) ? true : false)

/**
 * Check if string is from urn
 *
 * @param str String to be checked
 *
 * @return True if string is from urn, false otherwise
 */
#define mupnp_st_isurn(str) ((0 <= mupnp_strstr(str, MUPNP_ST_URN)) ? true : false)

/**
 * Check if string is from urn device
 *
 * @param str String to be checked
 *
 * @return True if string is from urn device, false otherwise
 */
#define mupnp_st_isurndevice(str) ((0 <= mupnp_strstr(str, MUPNP_ST_URN_DEVICE)) ? true : false)

/**
 * Check if string is from urn service
 *
 * @param str String to be checked
 *
 * @return True if string is from urn service, false otherwise
 */
#define mupnp_st_isurnservice(str) ((0 <= mupnp_strstr(str, MUPNP_ST_URN_SERVICE)) ? true : false)

/****************************************
* Function (NT)
****************************************/

/**
 * Check if NT (notification target) is for root device
 *
 * @param str String to be checked
 *
 * @return True if NT is for root device, false otherwise
 */
#define mupnp_nt_isrootdevice(str) ((0 <= mupnp_strstr(str, MUPNP_NT_ROOTDEVICE)) ? true : false)

/****************************************
* Function (NTS)
****************************************/

/**
 * Check if NTS (notification sub target) is of type "alive"
 *
 * @param str String to be checked
 *
 * @return True if string is of type "alive", false otherwise
 */
#define mupnp_nts_isalive(str) ((0 <= mupnp_strstr(str, MUPNP_NTS_ALIVE)) ? true : false)

/**
 * Check if string type is "byebye"
 *
 * @param str String to be checked
 *
 * @return True if string is of type "byebye", false othewise
 */
#define mupnp_nts_isbyebye(str) ((0 <= mupnp_strstr(str, MUPNP_NTS_BYEBYE)) ? true : false)

/**
 * Check if string type is "property change"
 *
 * @param str String to be checked
 *
 * @return True if string type is "property change"
 */
#define mupnp_nts_ispropchange(str) ((0 <= mupnp_strstr(str, MUPNP_NTS_PROPCHANGE)) ? true : false)

/****************************************
* Function (USN)
****************************************/

/**
 * Check if usn is from root device
 *
 * @param str String to be checked
 *
 * @return True if usn associates to root device, false otherwise
 */
#define mupnp_usn_isrootdevice(str) ((0 <= mupnp_strstr(str, MUPNP_USN_ROOTDEVICE)) ? true : false)

/**
 * Get udn from usn
 *
 * @param usn Universal serial number
 * @param udnBuf Buffer where udn is stored after successfull execution
 * @param udnBufLen Buffer length for udnBuf
 *
 * @return Pointer to beginning of udnBuf.
 */
const char* mupnp_usn_getudn(const char* usn, char* udnBuf, size_t udnBufLen);

/****************************************
* Function (Action)
****************************************/

/**
 * Send action
 *
 * @param action Action
 */
bool mupnp_action_post(mUpnpAction* action);

/****************************************
* Function (Query)
****************************************/

/**
 * Send query
 *
 * @param statVar State variable
 */
bool mupnp_statevariable_post(mUpnpStateVariable* statVar);

#ifdef __cplusplus
}
#endif

#endif
