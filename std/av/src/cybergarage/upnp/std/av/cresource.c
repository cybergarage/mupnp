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

#include <cybergarage/upnp/std/av/cresource.h>
#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_upnp_media_resource_new
****************************************/

CgUpnpMediaResource *cg_upnp_media_resource_new()
{
	CgUpnpMediaResource *node;
	CgUpnpMediaResourceData *nodeData;
	
	node = cg_xml_node_new();

	cg_xml_node_setname(node, CG_UPNP_MEDIA_RESOURCE_NAME);

	nodeData = cg_upnp_media_resource_data_new();
	cg_xml_node_setuserdata(node, nodeData);

	return node;
}

/****************************************
* cg_upnp_media_resource_delete
****************************************/

void cg_upnp_media_resource_delete(CgUpnpMediaResource *res)
{
	CgUpnpMediaResourceData *nodeData;

	nodeData = cg_xml_node_getuserdata(res);
	cg_upnp_media_resource_data_new(nodeData);
	free(nodeData);

	cg_xml_node_delete(res);
}

/****************************************
* cg_upnp_media_content_copy
****************************************/

void cg_upnp_media_resource_copy(CgUpnpMediaResource *destRes, CgUpnpMediaResource *srcRes)
{
	CgXmlAttribute *attr;

	cg_xml_node_setname(destRes, cg_xml_node_getname(srcRes));
	cg_xml_node_setvalue(destRes, cg_xml_node_getvalue(srcRes));
	for (attr=cg_xml_node_getattributes(srcRes); attr; attr=cg_xml_attribute_next(attr))
		cg_xml_node_setattribute(destRes, cg_xml_attribute_getname(attr), cg_xml_attribute_getvalue(attr));
	cg_upnp_media_resource_data_copy((CgUpnpMediaResourceData *)cg_xml_node_getuserdata(destRes), (CgUpnpMediaResourceData *)cg_xml_node_getuserdata(srcRes));
}

/****************************************
* cg_upnp_media_resource_updateattributes
****************************************/

void cg_upnp_media_resource_updateattributes(CgUpnpMediaResource *res)
{
	CgUpnpMediaResourceData *nodeData;
	CgString *resAttr;
	char *mimeType;
	char *dlnaPn;

	nodeData = (CgUpnpMediaResourceData *)cg_xml_node_getuserdata(res);
	mimeType = (0 < cg_string_length(nodeData->mimeType)) ? cg_string_getvalue(nodeData->mimeType) : "*/*";
	dlnaPn = (0 < cg_string_length(nodeData->dlnaAttr)) ? cg_string_getvalue(nodeData->dlnaAttr) : "*";

	resAttr = cg_string_new();
	cg_string_addvalue(resAttr, "http-get:*:");
	cg_string_addvalue(resAttr, mimeType);
	cg_string_addvalue(resAttr, ":");
	cg_string_addvalue(resAttr, dlnaPn);
	cg_xml_node_setattribute(res, CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO, cg_string_getvalue(resAttr));
	cg_string_delete(resAttr);
}

/****************************************
* cg_upnp_media_resource_setmimetype
****************************************/

void cg_upnp_media_resource_setmimetype(CgUpnpMediaResource *res, char *mimeType)
{
	CgUpnpMediaResourceData *nodeData;

	nodeData = (CgUpnpMediaResourceData *)cg_xml_node_getuserdata(res);
	cg_string_setvalue(nodeData->mimeType, mimeType);

	cg_upnp_media_resource_updateattributes(res);
}

/****************************************
* cg_upnp_media_resource_setdlnaattribute
****************************************/

void cg_upnp_media_resource_setdlnaattribute(CgUpnpMediaResource *res, char *attr)
{
	CgUpnpMediaResourceData *nodeData;

	nodeData = (CgUpnpMediaResourceData *)cg_xml_node_getuserdata(res);
	cg_string_setvalue(nodeData->dlnaAttr, attr);

	cg_upnp_media_resource_updateattributes(res);
}

/****************************************
* cg_upnp_media_resource_getdlnapnfrommimetype
****************************************/

char *cg_upnp_media_resource_getdlnapnfrommimetype(CgUpnpMediaResource *res, char *dlnaAttr, int dlnaAttrSize)
{
	char *mimeType;
	char *dlnaPn;

	dlnaPn = "*";
	mimeType = cg_upnp_media_resource_getmimetype(res);

	if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_JPEG))
		dlnaPn = CG_UPNP_MEDIA_DLNA_PN_JPEG_LRG;
	else if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_MPEG))
		dlnaPn = CG_UPNP_MEDIA_DLNA_PN_MPEG_PS_NTSC;
	else if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_MP3))
		dlnaPn = CG_UPNP_MEDIA_DLNA_PN_MP3;

#if defined(WIN32)
		_snprintf(dlnaAttr, dlnaAttrSize-1, "DLNA.ORG_PN=%s;DLNA.ORG_OP=00;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=00100000000000000000000000000000", dlnaPn);
#else
		snprintf(dlnaAttr, dlnaAttrSize-1, "DLNA.ORG_PN=%s;DLNA.ORG_OP=00;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=00100000000000000000000000000000", dlnaPn);
#endif

	return dlnaAttr;
}

/****************************************
* cg_upnp_media_resource_setsize
****************************************/

void cg_upnp_media_resource_setsize(CgUpnpMediaResource *res, long value)
{
	char strBuf[CG_STRING_LONG_BUFLEN];

	if (value <= 0) {
		cg_xml_node_removeattribute(res, CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO_SIZE);
		return;
	}
	cg_xml_node_setattribute(res, CG_UPNP_MEDIA_RESOURCE_PROTOCOLINFO_SIZE, cg_long2str(value, strBuf, (CG_STRING_LONG_BUFLEN-1)));
}
