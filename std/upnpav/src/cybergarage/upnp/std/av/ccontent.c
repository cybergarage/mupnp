/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontent.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/util/cstring.h>

#define CG_UPNP_MEDIA_CONTENT_CONTAINER_STRING "container"
#define CG_UPNP_MEDIA_CONTENT_ITEM_STRING "item"

/****************************************
* cg_upnp_media_content_settype
****************************************/

void cg_upnp_media_content_settype(CgUpnpMediaContent *con, int type)
{
	cg_xml_node_setname(con, (type == CG_UPNP_MEDIA_CONTENT_ITEM) ? CG_UPNP_MEDIA_CONTENT_ITEM_STRING : CG_UPNP_MEDIA_CONTENT_CONTAINER_STRING);
}

/****************************************
* cg_upnp_media_content_gettype
****************************************/

int cg_upnp_media_content_gettype(CgUpnpMediaContent *con)
{
	if (cg_streq(cg_xml_node_getname(con), CG_UPNP_MEDIA_CONTENT_ITEM_STRING))
		return CG_UPNP_MEDIA_CONTENT_ITEM;
	if (cg_streq(cg_xml_node_getname(con), CG_UPNP_MEDIA_CONTENT_CONTAINER_STRING))
		return CG_UPNP_MEDIA_CONTENT_CONTAINER;
	return CG_UPNP_MEDIA_CONTENT_NONE;
}

/****************************************
* cg_upnp_media_content_copy
****************************************/

void cg_upnp_media_content_copy(CgUpnpMediaContent *destContent, CgUpnpMediaContent *srcContent)
{
	CgXmlAttribute *attr;
	CgUpnpMediaContent *destNode;
	CgUpnpMediaContent *srcNode;

	cg_xml_node_setname(destContent, cg_xml_node_getname(srcContent));
	cg_xml_node_setvalue(destContent, cg_xml_node_getvalue(srcContent));
	for (attr=cg_xml_node_getattributes(srcContent); attr; attr=cg_xml_attribute_next(attr))
		cg_xml_node_setattribute(destContent, cg_xml_attribute_getname(attr), cg_xml_attribute_getvalue(attr));

	for (srcNode=cg_xml_node_getchildnodes(srcContent); srcNode; srcNode=cg_xml_node_next(srcNode)) {
		if (cg_upnp_media_content_gettype(srcNode) != CG_UPNP_MEDIA_CONTENT_NONE)
			continue;
		if (cg_streq(cg_xml_node_getname(srcNode), CG_UPNP_MEDIA_RESOURCE_NAME)) {
			destNode = cg_upnp_media_resource_new();
			cg_upnp_media_resource_copy(destNode, srcNode);
		}
		else {
			destNode = cg_upnp_media_content_new();
			cg_xml_node_setname(destNode, cg_xml_node_getname(srcNode));
			cg_xml_node_setvalue(destNode, cg_xml_node_getvalue(srcNode));
			for (attr=cg_xml_node_getattributes(srcNode); attr; attr=cg_xml_attribute_next(attr))
				cg_xml_node_setattribute(destNode, cg_xml_attribute_getname(attr), cg_xml_attribute_getvalue(attr));
		}
		cg_upnp_media_content_addchildcontent(destContent, destNode);
	}
}

/****************************************
* cg_upnp_media_content_findbyuserfunc
****************************************/

CgUpnpMediaContent *cg_upnp_media_content_findbyuserfunc(CgUpnpMediaContent *con, CG_UPNP_MEDIA_CONTENT_COMPARE_FUNC userFunc, void *userData)
{
	CgUpnpMediaContent *childCon;

	if (userFunc(con, userData))
		return con;

	for (childCon=cg_upnp_media_content_getchildcontents(con); childCon; childCon=cg_upnp_media_content_next(childCon)) {
		if (userFunc(childCon, userData))
			return childCon;
	}

	return NULL;
}

/****************************************
* cg_upnp_media_content_getbyobjectid
****************************************/

static BOOL cg_upnp_media_content_comparebyid(CgUpnpMediaContent *con, void *userData)
{
	if (!userData)
		return FALSE;
	return cg_streq(cg_upnp_media_content_getid(con), (char *)userData);
}

CgUpnpMediaContent *cg_upnp_media_content_getbyid(CgUpnpMediaContent *con, char *id)
{
	return cg_upnp_media_content_findbyuserfunc(con, cg_upnp_media_content_comparebyid, id);
}

/****************************************
* cg_upnp_media_content_getbytitle
****************************************/

static BOOL cg_upnp_media_content_comparebytitle(CgUpnpMediaContent *con, void *userData)
{
	if (!userData)
		return FALSE;
	return cg_streq(cg_upnp_media_content_gettitle(con), (char *)userData);
}

CgUpnpMediaContent *cg_upnp_media_content_getbytitle(CgUpnpMediaContent *con, char *title)
{
	return cg_upnp_media_content_findbyuserfunc(con, cg_upnp_media_content_comparebytitle, title);
}

/****************************************
* cg_upnp_media_content_addchildcontent
****************************************/

void cg_upnp_media_content_addchildcontent(CgUpnpMediaContent *con, CgUpnpMediaContent *childCon)
{
	cg_xml_node_addchildnode(con, childCon);
	cg_upnp_media_content_setparentid(childCon, cg_upnp_media_content_getparentid(con));
}

/****************************************
* cg_upnp_media_content_istitlenode
****************************************/

BOOL cg_upnp_media_content_iscontentnode(CgXmlNode *node)
{
	if (cg_upnp_media_content_isitemnode(node))
		return TRUE;
	if (cg_upnp_media_content_iscontainernode(node))
		return TRUE;
	return FALSE;
}

/****************************************
* cg_upnp_media_content_getnchildcontents
****************************************/

int cg_upnp_media_content_getnchildcontents(CgUpnpMediaContent *con)
{
	CgXmlNode *childNode;
	int contentNodeCnt;
	contentNodeCnt = 0;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnp_media_content_iscontentnode(childNode))
			contentNodeCnt++;
	}
	return contentNodeCnt;
}
/****************************************
* cg_upnp_media_content_getchildcontents
****************************************/

CgUpnpMediaContent *cg_upnp_media_content_getchildcontents(CgUpnpMediaContent *con)
{
	CgXmlNode *childNode;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnp_media_content_iscontentnode(childNode))
			return childNode;
	}
	return NULL;
}

/****************************************
* cg_upnp_media_content_istitlenode
****************************************/

BOOL cg_upnp_media_content_haschildcontents(CgUpnpMediaContent *con)
{
	CgXmlNode *childNode;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnp_media_content_iscontentnode(childNode))
			return TRUE;
	}
	return FALSE;
}

/****************************************
* cg_upnp_media_content_next
****************************************/

CgUpnpMediaContent *cg_upnp_media_content_next(CgUpnpMediaContent *con)
{
	CgXmlNode *nextNode;
	nextNode = cg_xml_node_next(con);
	while (nextNode) {
		if (cg_upnp_media_content_iscontentnode(nextNode))
			return nextNode;
		nextNode = cg_xml_node_next(con);
	}
	return NULL;
}
