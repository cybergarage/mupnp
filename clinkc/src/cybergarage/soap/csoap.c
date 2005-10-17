/******************************************************************
*
*	CyberSOAP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csoap.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/soap/csoap.h>

/****************************************
* cg_soap_createenvelopebodynode
****************************************/

CgXmlNode *cg_soap_createenvelopebodynode()
{
	CgXmlNode *envNode;
	CgXmlNode *bodyNode;
	
	envNode = cg_xml_node_new();
	cg_xml_node_setname(envNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_SOAP_ENVELOPE);
	cg_xml_node_setattribute(envNode, CG_SOAP_ATTRIBUTE_XMLNS CG_SOAP_DELIM CG_SOAP_XMLNS, CG_SOAP_XMLNS_URL);
	cg_xml_node_setattribute(envNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_SOAP_ENCORDING, CG_SOAP_ENCSTYLE_URL);

	bodyNode = cg_xml_node_new();
	cg_xml_node_setname(bodyNode, CG_SOAP_XMLNS CG_SOAP_DELIM CG_SOAP_BODY);
	cg_xml_node_addchildnode(envNode, bodyNode);

	return envNode;
}
