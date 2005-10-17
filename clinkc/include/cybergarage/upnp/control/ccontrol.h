/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontrol.h
*
*	Re1vision:
*
*	04/19/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CCONTROL_H_
#define _CG_UPNP_CCONTROL_H_

#include <cybergarage/typedef.h>

#include <cybergarage/util/cstring.h>
#include <cybergarage/soap/csoap.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/caction.h>
#include <cybergarage/upnp/cargument.h>
#include <cybergarage/upnp/cstatevariable.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/**** MAN ****/
#define CG_UPNP_MAN_DISCOVER "ssdp:discover"

/**** NT ****/
#define CG_UPNP_NT_ROOTDEVICE "upnp:rootdevice"
#define CG_UPNP_NT_EVENT "upnp:event"

/**** NTS ****/
#define CG_UPNP_NTS_ALIVE "ssdp:alive"
#define CG_UPNP_NTS_BYEBYE "ssdp:byebye"
#define CG_UPNP_NTS_PROPCHANGE "upnp:propchange"

/**** ST ****/
#define CG_UPNP_ST_ALL_DEVICE "ssdp:all"
#define CG_UPNP_ST_ROOT_DEVICE "upnp:rootdevice"
#define CG_UPNP_ST_UUID_DEVICE "uuid"
#define CG_UPNP_ST_URN_DEVICE "urn:schemas-upnp-org:device:"
#define CG_UPNP_ST_URN_SERVICE "urn:schemas-upnp-org:service:"

/**** USN ****/
#define CG_UPNP_USN_ROOTDEVICE "upnp:rootdevice"

#define CG_UPNP_CONTROL_NS "u"
#define CG_UPNP_CONTROL_QUERY_SOAPACTION "urn:schemas-upnp-org:control-1-0#QueryStateVariable"
#define CG_UPNP_CONTROL_XMLNS "urn:schemas-upnp-org:control-1-0"
#define CG_UPNP_CONTROL_QUERY_STATE_VARIABLE "QueryStateVariable"
#define CG_UPNP_CONTROL_QUERY_STATE_VARIABLE_RESPONSE "QueryStateVariableResponse"
#define CG_UPNP_CONTROL_VAR_NAME "varName"
#define CG_UPNP_CONTROL_RETURN "return"

#define CG_UPNP_CONTROL_FAULT_CODE "Client"
#define CG_UPNP_CONTROL_FAULT_STRING "UPnPError"
#define CG_UPNP_CONTROL_ERROR_CODE "errorCode"
#define CG_UPNP_CONTROL_ERROR_DESCRIPTION "errorDescription"

/****************************************
* Data Type
****************************************/

/**** Action ****/

typedef struct _CgUpnpActionResponse {
	CgSoapResponse *soapRes;
	BOOL isSoapResCreated;
	CgUpnpArgumentList *argList;
} CgUpnpActionResponse;

typedef struct _CgUpnpActionRequest{
	CgSoapRequest *soapReq;
	BOOL isSoapReqCreated;
	CgUpnpArgumentList *argList;
	CgUpnpActionResponse *actionRes;
} CgUpnpActionRequest;

/**** Query ****/

typedef struct _CgUpnpQueryResponse {
	CgSoapResponse *soapRes;
	BOOL isSoapResCreated;
} CgUpnpQueryResponse;

typedef struct _CgUpnpQueryRequest{
	CgSoapRequest *soapReq;
	BOOL isSoapReqCreated;
	CgUpnpQueryResponse *queryRes;
} CgUpnpQueryRequest;

/****************************************
* Function
****************************************/

#define cg_upnp_control_isqueryrequest(httpReq) ((0 < cg_strstr(cg_soap_request_getsoapaction(httpReq), CG_UPNP_CONTROL_QUERY_SOAPACTION)) ? TRUE : FALSE)
#define cg_upnp_control_isactionrequest(httpReq) ((cg_upnp_control_isqueryrequest(httpReq) == TRUE) ? FALSE : TRUE)

void cg_upnp_control_soap_request_initializeenvelopenode(CgSoapRequest *soapReq);

void cg_upnp_control_soap_response_initializeenvelopenode(CgSoapResponse *soapRes);
CgXmlNode *cg_upnp_control_soap_response_createfaultresponsenode(int errCode, char *errDescr);
void cg_upnp_control_soap_response_setfaultresponse(CgSoapResponse *soapRes, int errCode, char *errDescr);

BOOL cg_upnp_action_performlistner(CgUpnpAction *action, CgUpnpActionRequest *actionReq);
BOOL cg_upnp_statevariable_performlistner(CgUpnpStateVariable *statVar, CgUpnpQueryRequest *queryReq);

void cg_upnp_control_request_sethostfromservice(CgSoapRequest *soapReq, CgUpnpService *service);

/****************************************
* Function (ActionRequest)
****************************************/

CgUpnpActionRequest *cg_upnp_control_action_request_new();
void cg_upnp_control_action_request_delete(CgUpnpActionRequest *actionReq);
void cg_upnp_control_action_request_clear(CgUpnpActionRequest *actionReq);

void cg_upnp_control_action_request_setsoaprequest(CgUpnpActionRequest *actionReq, CgSoapRequest *soapReq);
#define cg_upnp_control_action_request_getsoaprequest(actionReq) (actionReq->soapReq)

#define cg_upnp_control_action_request_getargumentlist(actionReq) (actionReq->argList)

CgXmlNode *cg_upnp_control_action_request_getactionnode(CgUpnpActionRequest *actionReq);
char *cg_upnp_control_action_request_getactionname(CgUpnpActionRequest *actionReq);

void cg_upnp_control_action_request_setaction(CgUpnpActionRequest *actionReq, CgUpnpAction *action);

#define cg_upnp_control_action_request_getactionresponse(actionReq) (actionReq->actionRes)
CgUpnpActionResponse *cg_upnp_control_action_request_post(CgUpnpActionRequest *actionReq);

/****************************************
* Function (ActionResponse)
****************************************/

CgUpnpActionResponse *cg_upnp_control_action_response_new();
void cg_upnp_control_action_response_delete(CgUpnpActionResponse *actionReq);
void cg_upnp_control_action_response_clear(CgUpnpActionResponse *actionReq);

void cg_upnp_control_action_response_setsoapresponse(CgUpnpActionResponse *actionRes, CgSoapResponse *soapRes);
#define cg_upnp_control_action_response_getsoapresponse(actionRes) (actionRes->soapRes)

void cg_upnp_control_action_response_setresponse(CgUpnpActionResponse *actionRes, CgUpnpAction *action);

#define cg_upnp_control_action_response_issuccessful(actionRes) cg_soap_response_issuccessful(actionRes->soapRes)

CgXmlNode *cg_upnp_control_action_response_getactionresponsenode(CgUpnpActionResponse *actionRes);
BOOL cg_upnp_control_action_response_getresult(CgUpnpActionResponse *actionRes, CgUpnpAction *action);

/****************************************
* Function (QueryRequest)
****************************************/

CgUpnpQueryRequest *cg_upnp_control_query_request_new();
void cg_upnp_control_query_request_delete(CgUpnpQueryRequest *queryReq);
void cg_upnp_control_query_request_clear(CgUpnpQueryRequest *queryReq);

void cg_upnp_control_query_request_setsoaprequest(CgUpnpQueryRequest *queryReq, CgSoapRequest *soapReq);
#define cg_upnp_control_query_request_getsoaprequest(queryReq) (queryReq->soapReq)

CgXmlNode *cg_upnp_control_query_request_getvarnamenode(CgUpnpQueryRequest *queryReq);
char *cg_upnp_control_query_request_getvarname(CgUpnpQueryRequest *queryReq);

void cg_upnp_control_query_request_setstatevariable(CgUpnpQueryRequest *queryReq, CgUpnpStateVariable *statVar);

#define cg_upnp_control_query_request_getqueryresponse(queryReq) (queryReq->queryRes)
CgUpnpQueryResponse *cg_upnp_control_query_request_post(CgUpnpQueryRequest *queryReq);

/****************************************
* Function (QueryResponse)
****************************************/

CgUpnpQueryResponse *cg_upnp_control_query_response_new();
void cg_upnp_control_query_response_delete(CgUpnpQueryResponse *queryReq);
void cg_upnp_control_query_response_clear(CgUpnpQueryResponse *queryReq);

void cg_upnp_control_query_response_setsoapresponse(CgUpnpQueryResponse *queryRes, CgSoapResponse *soapRes);
#define cg_upnp_control_query_response_getsoapresponse(queryRes) (queryRes->soapRes)

void cg_upnp_control_query_response_setresponse(CgUpnpQueryResponse *queryRes, CgUpnpStateVariable *statVar);

#define cg_upnp_control_query_response_issuccessful(queryRes) cg_soap_response_issuccessful(queryRes->soapRes)

CgXmlNode *cg_upnp_control_query_response_getreturnnode(CgUpnpQueryResponse *queryRes);
char *cg_upnp_control_query_response_getreturnvalue(CgUpnpQueryResponse *queryRes);

/****************************************
* Function (MAN)
****************************************/

#define cg_upnp_man_isdiscover(str) ((0 <= cg_strstr(str, CG_UPNP_MAN_DISCOVER)) ? TRUE : FALSE)

/****************************************
* Function (ST)
****************************************/

#define cg_upnp_st_isalldevice(str) ((0 <= cg_strstr(str, CG_UPNP_ST_ALL_DEVICE)) ? TRUE : FALSE)
#define cg_upnp_st_isrootdevice(str) ((0 <= cg_strstr(str, CG_UPNP_ST_ROOT_DEVICE)) ? TRUE : FALSE)
#define cg_upnp_st_isuuiddevice(str) ((0 <= cg_strstr(str, CG_UPNP_ST_UUID_DEVICE)) ? TRUE : FALSE)
#define cg_upnp_st_isurndevice(str) ((0 <= cg_strstr(str, CG_UPNP_ST_URN_DEVICE)) ? TRUE : FALSE)
#define cg_upnp_st_isurnservice(str) ((0 <= cg_strstr(str, CG_UPNP_ST_URN_SERVICE)) ? TRUE : FALSE)

/****************************************
* Function (NT)
****************************************/

#define cg_upnp_nt_isrootdevice(str) ((0 <= cg_strstr(str, CG_UPNP_NT_ROOTDEVICE)) ? TRUE : FALSE)

/****************************************
* Function (NTS)
****************************************/

#define cg_upnp_nts_isalive(str) ((0 <= cg_strstr(str, CG_UPNP_NTS_ALIVE)) ? TRUE : FALSE)
#define cg_upnp_nts_isbyebye(str) ((0 <= cg_strstr(str, CG_UPNP_NTS_BYEBYE)) ? TRUE : FALSE)
#define cg_upnp_nts_ispropchange(str) ((0 <= cg_strstr(str, CG_UPNP_NTS_PROPCHANGE)) ? TRUE : FALSE)

/****************************************
* Function (USN)
****************************************/

#define cg_upnp_usn_isrootdevice(str) ((0 <= cg_strstr(str, CG_UPNP_USN_ROOTDEVICE)) ? TRUE : FALSE)

char *cg_upnp_usn_getudn(char *usn, char *udnBuf, int udnBufLen);

/****************************************
* Function (Action)
****************************************/

BOOL cg_upnp_action_post(CgUpnpAction *action);

/****************************************
* Function (Query)
****************************************/

BOOL cg_upnp_statevariable_post(CgUpnpStateVariable *statVar);

#ifdef  __cplusplus
}
#endif

#endif
