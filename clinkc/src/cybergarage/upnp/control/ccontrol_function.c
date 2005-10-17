/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontrol_function.c
*
*	Revision:
*
*	06/03/05
*		- first revision
*	09/11/05
*		- Thanks for Visa Smolander <visa.smolander@nokia.com>
*		- Fixed cg_upnp_control_soap_response_createfaultresponsenode() to set
*		  the namespace attribute in the response <UPnPError> node.
*
*************************************\*****************************/

#include <cybergarage/upnp/control/ccontrol.h>

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_upnp_usn_getudn
****************************************/

char *cg_upnp_usn_getudn(char *usn, char *udnBuf, int udnBufLen)
{
	int idx;
	
	if (usn == NULL)
		return "";
	
	idx = cg_strstr(usn, "::");
	if (idx < 0) {
		cg_strncpy(udnBuf, usn, udnBufLen);
		cg_strtrim(udnBuf, " ", 1);
		return udnBuf;
	}

	cg_strncpy(udnBuf, usn, (idx+1));
	cg_strtrim(udnBuf, " ", 1);
	return udnBuf;
}

/****************************************
* cg_upnp_control_soap_response_initializeenvelopenode
****************************************/

void cg_upnp_control_soap_response_initializeenvelopenode(CgSoapResponse *soapRes)
{
	CgXmlNodeList *rootNodeList;
	CgHttpResponse *httpRes;

	rootNodeList = cg_soap_response_getrootnoodelist(soapRes);
	cg_xml_nodelist_add(rootNodeList, cg_soap_createenvelopebodynode());

	httpRes = cg_soap_response_gethttpresponse(soapRes);
	cg_http_request_setcontenttype(httpRes, CG_SOAP_CONTENT_TYPE);
}

/****************************************
* cg_upnp_control_soap_request_initializeenvelopenode
****************************************/

void cg_upnp_control_soap_request_initializeenvelopenode(CgSoapRequest *soapReq)
{
	CgXmlNodeList *rootNodeList;
	CgHttpRequest *httpReq;

	rootNodeList = cg_soap_request_getrootnoodelist(soapReq);
	cg_xml_nodelist_add(rootNodeList, cg_soap_createenvelopebodynode());

	httpReq = cg_soap_request_gethttprequest(soapReq);
	cg_http_request_setcontenttype(httpReq, CG_SOAP_CONTENT_TYPE);
}

/****************************************
* cg_upnp_control_soap_response_createfaultresponsenode
****************************************/

CgXmlNode *cg_upnp_control_soap_response_createfaultresponsenode(int errCode, char *errDescr)
{
	CgXmlNode *faultNode;
	CgXmlNode *faultCodeNode;
	CgXmlNode *faultStringNode;
	CgXmlNode *detailNode;
	CgXmlNode *upnpErrorNode;
	CgXmlNode *errorCodeNode;
	CgXmlNode *errorDesctiprionNode;
	
	/**** <s:Fault> ****/
	faultNode = cg_xml_node_new();
	cg_xml_node_setname(faultNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_SOAP_FAULT);
	
	/**** <faultcode>s:Client</faultcode> ****/
	faultCodeNode = cg_xml_node_new();
	cg_xml_node_setname(faultCodeNode, CG_SOAP_FAULT_CODE);
	cg_xml_node_setvalue(faultCodeNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_UPNP_CONTROL_FAULT_CODE);
	cg_xml_node_addchildnode(faultNode, faultCodeNode);
		
	/**** <faultstring>UPnPError</faultstring> ****/
	faultStringNode = cg_xml_node_new();
	cg_xml_node_setname(faultStringNode, CG_SOAP_FAULT_STRING);
	cg_xml_node_setvalue(faultStringNode, CG_UPNP_CONTROL_FAULT_STRING);
	cg_xml_node_addchildnode(faultNode, faultStringNode);

	/**** <detail> ****/
	detailNode = cg_xml_node_new();
	cg_xml_node_setname(detailNode, CG_SOAP_DETAIL);
	cg_xml_node_addchildnode(faultNode, detailNode);

	/**** <UPnPError xmlns="urn:schemas-upnp-org:control-1-0"> ****/
	upnpErrorNode = cg_xml_node_new();
	cg_xml_node_setname(upnpErrorNode, CG_UPNP_CONTROL_FAULT_STRING);
	cg_xml_node_setattribute(upnpErrorNode, CG_SOAP_ATTRIBUTE_XMLNS, CG_SOAP_XMLNS);
	/**** Thanks for Visa Smolander (09/11/2005) ****/
	cg_xml_node_setattribute(upnpErrorNode, CG_SOAP_ATTRIBUTE_XMLNS, CG_UPNP_CONTROL_XMLNS);
	cg_xml_node_addchildnode(detailNode, upnpErrorNode);

	/**** <errorCode>error code</errorCode> ****/
	errorCodeNode = cg_xml_node_new();
	cg_xml_node_setname(errorCodeNode, CG_UPNP_CONTROL_ERROR_CODE);
	cg_xml_node_setintvalue(errorCodeNode, errCode);
	cg_xml_node_addchildnode(upnpErrorNode, errorCodeNode);

	/**** <errorDescription>error string</errorDescription> ****/
	errorDesctiprionNode = cg_xml_node_new();
	cg_xml_node_setname(errorDesctiprionNode, CG_UPNP_CONTROL_ERROR_DESCRIPTION);
	cg_xml_node_setvalue(errorDesctiprionNode, errDescr);
	cg_xml_node_addchildnode(upnpErrorNode, errorDesctiprionNode);
	
	return faultNode;
}


/****************************************
* cg_upnp_control_soap_response_setfaultresponse
****************************************/

void cg_upnp_control_soap_response_setfaultresponse(CgSoapResponse *soapRes, int errCode, char *errDescr)
{
	CgHttpResponse *httpRes;
	CgXmlNode *bodyNode;
	CgXmlNode *faultNode;
	CgXmlNode *envNode;

	httpRes = cg_soap_response_gethttpresponse(soapRes);

	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_INTERNAL_SERVER_ERROR);
	cg_upnp_control_soap_response_initializeenvelopenode(soapRes);

	bodyNode = cg_soap_response_getbodynode(soapRes);
	faultNode = cg_upnp_control_soap_response_createfaultresponsenode(errCode, errDescr);
	cg_xml_node_addchildnode(bodyNode, faultNode);
	
	envNode = cg_soap_response_getenvelopenode(soapRes);
	cg_soap_response_setcontent(soapRes, envNode);
}

/****************************************
* cg_upnp_control_request_sethostfromservice
****************************************/

void cg_upnp_control_request_sethostfromservice(CgSoapRequest *soapReq, CgUpnpService *service)
{
	CgHttpRequest *httpReq;
	CgUpnpDevice *rootDev;
	CgNetURL *ctrlURL;
	CgNetURL *postURL;
	char *urlBase;
	
	httpReq = cg_soap_request_gethttprequest(soapReq);

	ctrlURL = cg_net_url_new();
	cg_net_url_set(ctrlURL, cg_upnp_service_getcontrolurl(service));
	
	/*** URI ***/
	if (cg_net_url_isabsolute(ctrlURL) == TRUE)
		cg_http_request_seturi(httpReq, cg_net_url_getpath(ctrlURL));
	else
		cg_http_request_seturi(httpReq, cg_upnp_service_getcontrolurl(service));
	
	/**** Host ****/
	postURL = cg_http_request_getposturl(httpReq);
	if (cg_net_url_isabsolute(ctrlURL) == FALSE) {
		rootDev = cg_upnp_service_getrootdevice(service);
		urlBase = cg_upnp_device_geturlbase(rootDev);
		if (0 < cg_strlen(urlBase))
			cg_net_url_set(postURL, urlBase);
		else
			cg_net_url_set(postURL, cg_upnp_device_getlocationfromssdppacket(rootDev));
	}
	else
		cg_net_url_set(postURL, cg_upnp_service_getcontrolurl(service));
	
	cg_net_url_delete(ctrlURL);
}
