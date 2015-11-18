/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cresource.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <mupnp/std/av/cresource.h>
#include <mupnp/std/av/ccontent.h>
#include <mupnp/util/string.h>

/****************************************
* mupnp_upnpav_resource_new
****************************************/

mUpnpAvResource *mupnp_upnpav_resource_new()
{
	mUpnpAvResource *node;
	mUpnpAvResourceData *nodeData;
	
	node = mupnp_xml_node_new();

	mupnp_xml_node_setname(node, CG_UPNPAV_RESOURCE_NAME);

	nodeData = mupnp_upnpav_resource_data_new();
	mupnp_xml_node_setuserdata(node, nodeData);

	return node;
}

/****************************************
* mupnp_upnpav_resource_delete
****************************************/

void mupnp_upnpav_resource_delete(mUpnpAvResource *res)
{
	mUpnpAvResourceData *nodeData;

	nodeData = mupnp_xml_node_getuserdata(res);
	mupnp_upnpav_resource_data_new(nodeData);
	free(nodeData);

	mupnp_xml_node_delete(res);
}

/****************************************
* mupnp_upnpav_content_copy
****************************************/

void mupnp_upnpav_resource_copy(mUpnpAvResource *destRes, mUpnpAvResource *srcRes)
{
	mUpnpXmlAttribute *attr;

	mupnp_xml_node_setname(destRes, mupnp_xml_node_getname(srcRes));
	mupnp_xml_node_setvalue(destRes, mupnp_xml_node_getvalue(srcRes));
	for (attr=mupnp_xml_node_getattributes(srcRes); attr; attr=mupnp_xml_attribute_next(attr))
		mupnp_xml_node_setattribute(destRes, mupnp_xml_attribute_getname(attr), mupnp_xml_attribute_getvalue(attr));
	mupnp_upnpav_resource_data_copy((mUpnpAvResourceData *)mupnp_xml_node_getuserdata(destRes), (mUpnpAvResourceData *)mupnp_xml_node_getuserdata(srcRes));
}

/****************************************
* mupnp_upnpav_resource_updateattributes
****************************************/

void mupnp_upnpav_resource_updateattributes(mUpnpAvResource *res)
{
	mUpnpAvResourceData *nodeData;
	mUpnpString *resAttr;
	char *mimeType;
	char *dlnaAttr;

	nodeData = (mUpnpAvResourceData *)mupnp_xml_node_getuserdata(res);
	mimeType = (0 < mupnp_string_length(nodeData->mimeType)) ? mupnp_string_getvalue(nodeData->mimeType) : "*/*";
	dlnaAttr = (0 < mupnp_string_length(nodeData->dlnaAttr)) ? mupnp_string_getvalue(nodeData->dlnaAttr) : "*";

	resAttr = mupnp_string_new();
	mupnp_string_addvalue(resAttr, "http-get:*:");
	mupnp_string_addvalue(resAttr, mimeType);
	mupnp_string_addvalue(resAttr, ":");
	mupnp_string_addvalue(resAttr, dlnaAttr);
	mupnp_xml_node_setattribute(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO, mupnp_string_getvalue(resAttr));
	mupnp_string_delete(resAttr);
}

/****************************************
* mupnp_upnpav_resource_setmimetype
****************************************/

void mupnp_upnpav_resource_setmimetype(mUpnpAvResource *res, char *mimeType)
{
	mUpnpAvResourceData *nodeData;

	nodeData = (mUpnpAvResourceData *)mupnp_xml_node_getuserdata(res);
	mupnp_string_setvalue(nodeData->mimeType, mimeType);

	mupnp_upnpav_resource_updateattributes(res);
}

/****************************************
* mupnp_upnpav_resource_setdlnaattribute
****************************************/

void mupnp_upnpav_resource_setdlnaattribute(mUpnpAvResource *res, char *attr)
{
	mUpnpAvResourceData *nodeData;

	nodeData = (mUpnpAvResourceData *)mupnp_xml_node_getuserdata(res);
	mupnp_string_setvalue(nodeData->dlnaAttr, attr);

	mupnp_upnpav_resource_updateattributes(res);
}

/****************************************
* mupnp_upnpav_resource_getdlnaattributesbymimetype
****************************************/

char *mupnp_upnpav_resource_getdlnaattributesbymimetype(char *mimeType, char *dlnaAttr, int dlnaAttrSize)
{
	char *dlnaOrgOp;
	char *dlnaOrgCi;
	char *dlnaOrgPn;
	char *dlnaOrgFlags;

	dlnaOrgOp = "00";
	dlnaOrgCi = "1";
	dlnaOrgPn = "*";
	dlnaOrgFlags = "00000000000000000000000000000000";
	
	if (mupnp_strcaseeq(mimeType, CG_UPNPAV_MIMETYPE_JPEG)) {
		dlnaOrgPn = CG_UPNPAV_DLNA_PN_JPEG_LRG;
		dlnaOrgFlags = "00200000000000000000000000000000";
		dlnaOrgCi = "0";
		dlnaOrgOp = "00";
	}
	else if (mupnp_strcaseeq(mimeType, CG_UPNPAV_MIMETYPE_AVC)) {
		dlnaOrgPn = CG_UPNPAV_DLNA_PN_AVC;
		dlnaOrgFlags = "00200000000000000000000000000000";
		dlnaOrgCi = "0";
		dlnaOrgOp = "01";
	}
	else if (mupnp_strcaseeq(mimeType, CG_UPNPAV_MIMETYPE_MP3)) {
		dlnaOrgPn = CG_UPNPAV_DLNA_PN_MP3;
		dlnaOrgFlags = "00200000000000000000000000000000";
		dlnaOrgCi = "0";
		dlnaOrgOp = "01";
	}

#if defined(WIN32)
	_snprintf(dlnaAttr, dlnaAttrSize-1,
#else
	snprintf(dlnaAttr, dlnaAttrSize-1,
#endif
		"DLNA.ORG_PN=%s;DLNA.ORG_OP=%s;DLNA.ORG_CI=%s;DLNA.ORG_FLAGS=%s", 
			dlnaOrgPn,
			dlnaOrgOp,
			dlnaOrgCi,
			dlnaOrgFlags);

	return dlnaAttr;
}

/****************************************
* mupnp_upnpav_resource_getdlnaattributes
****************************************/

char *mupnp_upnpav_resource_getdlnaattributes(mUpnpAvResource *res, char *dlnaAttr, int dlnaAttrSize)
{
	return mupnp_upnpav_resource_getdlnaattributesbymimetype(mupnp_upnpav_resource_getmimetype(res), dlnaAttr, dlnaAttrSize);
}

/****************************************
* mupnp_upnpav_resource_setsize
****************************************/

void mupnp_upnpav_resource_setsize(mUpnpAvResource *res, long value)
{
	char strBuf[MUPNP_STRING_LONG_BUFLEN];

	if (value <= 0) {
		mupnp_xml_node_removeattribute(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE);
		return;
	}
	mupnp_xml_node_setattribute(res, CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE, mupnp_long2str(value, strBuf, (MUPNP_STRING_LONG_BUFLEN-1)));
}
