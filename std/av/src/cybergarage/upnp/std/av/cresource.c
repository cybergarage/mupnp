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
	char *dlnaAttr;

	nodeData = (CgUpnpMediaResourceData *)cg_xml_node_getuserdata(res);
	mimeType = (0 < cg_string_length(nodeData->mimeType)) ? cg_string_getvalue(nodeData->mimeType) : "*/*";
	dlnaAttr = (0 < cg_string_length(nodeData->dlnaAttr)) ? cg_string_getvalue(nodeData->dlnaAttr) : "*";

	resAttr = cg_string_new();
	cg_string_addvalue(resAttr, "http-get:*:");
	cg_string_addvalue(resAttr, mimeType);
	cg_string_addvalue(resAttr, ":");
	cg_string_addvalue(resAttr, dlnaAttr);
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
* cg_upnp_media_resource_getnprotocolinfos
****************************************/

static char CG_UPNP_AV_SUPPORTED_MIMETYPES[][CG_UPNP_MEDIA_PROTOCOLINFO_MAXLEN] = {
CG_UPNP_MEDIA_MIMETYPE_JPEG,
CG_UPNP_MEDIA_MIMETYPE_MPEG,
CG_UPNP_MEDIA_MIMETYPE_MP3,
};

int cg_upnp_media_resource_getnprotocolinfos()
{
	return 3; /*sizeof(CG_UPNP_AV_SUPPORTED_MIMETYPES/sizeof(CG_UPNP_AV_SUPPORTED_MIMETYPES[0][0]))*/;
}

/****************************************
* cg_upnp_media_resource_getdlnaattributesbymimetype
****************************************/

char *cg_upnp_media_resource_getdlnaattributesbymimetype(char *mimeType, char *dlnaAttr, int dlnaAttrSize)
{
	char *dlnaOrgOp;
	char *dlnaOrgCi;
	char *dlnaOrgPn;
	char *dlnaOrgFlags;

	dlnaOrgOp = "00";
	dlnaOrgCi = "1";
	dlnaOrgPn = "*";
	dlnaOrgFlags = "00000000000000000000000000000000";
	
	if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_JPEG)) {
		dlnaOrgPn = CG_UPNP_MEDIA_DLNA_PN_JPEG_LRG;
		dlnaOrgFlags = "00200000000000000000000000000000";
		dlnaOrgCi = "0";
		dlnaOrgOp = "00";
	}
	else if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_MPEG)) {
		dlnaOrgPn = CG_UPNP_MEDIA_DLNA_PN_MPEG_PS_NTSC;
		dlnaOrgFlags = "00200000000000000000000000000000";
		dlnaOrgCi = "0";
		dlnaOrgOp = "01";
	}
	else if (cg_strcaseeq(mimeType, CG_UPNP_MEDIA_MIMETYPE_MP3)) {
		dlnaOrgPn = CG_UPNP_MEDIA_DLNA_PN_MP3;
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
* cg_upnp_media_resource_getdlnaattributes
****************************************/

char *cg_upnp_media_resource_getdlnaattributes(CgUpnpMediaResource *res, char *dlnaAttr, int dlnaAttrSize)
{
	return cg_upnp_media_resource_getdlnaattributesbymimetype(cg_upnp_media_resource_getmimetype(res), dlnaAttr, dlnaAttrSize);
}

/****************************************
* cg_upnp_media_resource_getprotocolinfo
****************************************/

char *cg_upnp_media_resource_getprotocolinfo(int n, char *protoInfoBuf, int protoInfoBufSize)
{
	char dlnaAttr[CG_UPNP_MEDIA_DLNAATTR_MAXLEN];

#if defined(WIN32)
	_snprintf(
#else
	snprintf(
#endif
		protoInfoBuf,
		protoInfoBufSize-1,
		"http-get:*:%s:%s",
		CG_UPNP_AV_SUPPORTED_MIMETYPES[n],
		cg_upnp_media_resource_getdlnaattributesbymimetype(CG_UPNP_AV_SUPPORTED_MIMETYPES[n], dlnaAttr, sizeof(dlnaAttr)));

	return protoInfoBuf;
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
