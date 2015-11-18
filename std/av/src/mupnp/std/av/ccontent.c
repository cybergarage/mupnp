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

#include <mupnp/std/av/ccontent.h>
#include <mupnp/util/string.h>

#define CG_UPNPAV_CONTENT_CONTAINER_STRING "container"
#define CG_UPNPAV_CONTENT_ITEM_STRING "item"

/****************************************
* mupnp_upnpav_content_settype
****************************************/

void mupnp_upnpav_content_settype(mUpnpAvContent *con, int type)
{
	switch (type) {
	case CG_UPNPAV_CONTENT_CONTAINER:
		{
			mupnp_xml_node_setname(con, CG_UPNPAV_CONTENT_CONTAINER_STRING);
			mupnp_upnpav_content_setupnpclass(con, CG_UPNPAV_UPNPCLASS_CONTAINER);
		}
		break;
	case CG_UPNPAV_CONTENT_ITEM:
		{
			mupnp_xml_node_setname(con, CG_UPNPAV_CONTENT_ITEM_STRING);
		}
		break;
	}
}

/****************************************
* mupnp_upnpav_content_gettype
****************************************/

int mupnp_upnpav_content_gettype(mUpnpAvContent *con)
{
	if (mupnp_streq(mupnp_xml_node_getname(con), CG_UPNPAV_CONTENT_ITEM_STRING))
		return CG_UPNPAV_CONTENT_ITEM;
	if (mupnp_streq(mupnp_xml_node_getname(con), CG_UPNPAV_CONTENT_CONTAINER_STRING))
		return CG_UPNPAV_CONTENT_CONTAINER;
	return CG_UPNPAV_CONTENT_NONE;
}

/****************************************
* mupnp_upnpav_content_copy
****************************************/

void mupnp_upnpav_content_copy(mUpnpAvContent *destContent, mUpnpAvContent *srcContent)
{
	mUpnpXmlAttribute *attr;
	mUpnpAvContent *destNode;
	mUpnpAvContent *srcNode;

	mupnp_xml_node_setname(destContent, mupnp_xml_node_getname(srcContent));
	mupnp_xml_node_setvalue(destContent, mupnp_xml_node_getvalue(srcContent));
	for (attr=mupnp_xml_node_getattributes(srcContent); attr; attr=mupnp_xml_attribute_next(attr))
		mupnp_xml_node_setattribute(destContent, mupnp_xml_attribute_getname(attr), mupnp_xml_attribute_getvalue(attr));

	for (srcNode=mupnp_xml_node_getchildnodes(srcContent); srcNode; srcNode=mupnp_xml_node_next(srcNode)) {
		if (mupnp_upnpav_content_gettype(srcNode) != CG_UPNPAV_CONTENT_NONE)
			continue;
		if (mupnp_streq(mupnp_xml_node_getname(srcNode), CG_UPNPAV_RESOURCE_NAME)) {
			destNode = mupnp_upnpav_resource_new();
			mupnp_upnpav_resource_copy(destNode, srcNode);
		}
		else {
			destNode = mupnp_upnpav_content_new();
			mupnp_xml_node_setname(destNode, mupnp_xml_node_getname(srcNode));
			mupnp_xml_node_setvalue(destNode, mupnp_xml_node_getvalue(srcNode));
			for (attr=mupnp_xml_node_getattributes(srcNode); attr; attr=mupnp_xml_attribute_next(attr))
				mupnp_xml_node_setattribute(destNode, mupnp_xml_attribute_getname(attr), mupnp_xml_attribute_getvalue(attr));
		}
		mupnp_xml_node_addchildnode(destContent, destNode);
	}
}

/****************************************
* mupnp_upnpav_content_findbyuserfunc
****************************************/

mUpnpAvContent *mupnp_upnpav_content_findbyuserfunc(mUpnpAvContent *con, CG_UPNPAV_CONTENT_COMPARE_FUNC userFunc, void *userData)
{
	mUpnpAvContent *childCon, *lower;

	if (userFunc(con, userData))
		return con;

	for (childCon=mupnp_upnpav_content_getchildcontents(con); childCon; childCon=mupnp_upnpav_content_next(childCon)) {
		if (userFunc(childCon, userData))
			return childCon;
        if (mupnp_upnpav_content_haschildcontents(childCon))
            if ((lower = mupnp_upnpav_content_findbyuserfunc(childCon,
                                                 userFunc,
                                                          userData)))
                return lower;
	}

	return NULL;
}

/****************************************
* mupnp_upnpav_content_getbyobjectid
****************************************/

static bool mupnp_upnpav_content_comparebyid(mUpnpAvContent *con, void *userData)
{
	if (!userData)
		return false;
	return mupnp_streq(mupnp_upnpav_content_getid(con), (char *)userData);
}

mUpnpAvContent *mupnp_upnpav_content_getbyid(mUpnpAvContent *con, char *id)
{
	return mupnp_upnpav_content_findbyuserfunc(con, mupnp_upnpav_content_comparebyid, id);
}

/****************************************
* mupnp_upnpav_content_getbytitle
****************************************/

static bool mupnp_upnpav_content_comparebytitle(mUpnpAvContent *con, void *userData)
{
	if (!userData)
		return false;
	return mupnp_streq(mupnp_upnpav_content_gettitle(con), (char *)userData);
}

mUpnpAvContent *mupnp_upnpav_content_getbytitle(mUpnpAvContent *con, char *title)
{
	return mupnp_upnpav_content_findbyuserfunc(con, mupnp_upnpav_content_comparebytitle, title);
}

/****************************************
* mupnp_upnpav_content_addchildcontent
****************************************/

void mupnp_upnpav_content_addchildcontent(mUpnpAvContent *con, mUpnpAvContent *childCon)
{
	mupnp_xml_node_addchildnode(con, childCon);
	mupnp_upnpav_content_setparentid(childCon, mupnp_upnpav_content_getid(con));
}

/****************************************
* mupnp_upnpav_content_istitlenode
****************************************/

bool mupnp_upnpav_content_iscontentnode(mUpnpXmlNode *node)
{
	if (mupnp_upnpav_content_isitemnode(node))
		return true;
	if (mupnp_upnpav_content_iscontainernode(node))
		return true;
	return false;
}

/****************************************
* mupnp_upnpav_content_getnchildcontents
****************************************/

int mupnp_upnpav_content_getnchildcontents(mUpnpAvContent *con)
{
	mUpnpXmlNode *childNode;
	int contentNodeCnt;
	contentNodeCnt = 0;
	for (childNode = mupnp_xml_node_getchildnodes(con); childNode; childNode=mupnp_xml_node_next(childNode)) {
		if (mupnp_upnpav_content_iscontentnode(childNode))
			contentNodeCnt++;
	}
	return contentNodeCnt;
}
/****************************************
* mupnp_upnpav_content_getchildcontents
****************************************/

mUpnpAvContent *mupnp_upnpav_content_getchildcontents(mUpnpAvContent *con)
{
	mUpnpXmlNode *childNode;
	for (childNode = mupnp_xml_node_getchildnodes(con); childNode; childNode=mupnp_xml_node_next(childNode)) {
		if (mupnp_upnpav_content_iscontentnode(childNode))
			return childNode;
	}
	return NULL;
}

/****************************************
* mupnp_upnpav_content_istitlenode
****************************************/

bool mupnp_upnpav_content_haschildcontents(mUpnpAvContent *con)
{
	mUpnpXmlNode *childNode;
	for (childNode = mupnp_xml_node_getchildnodes(con); childNode; childNode=mupnp_xml_node_next(childNode)) {
		if (mupnp_upnpav_content_iscontentnode(childNode))
			return true;
	}
	return false;
}

/****************************************
* mupnp_upnpav_content_next
****************************************/

mUpnpAvContent *mupnp_upnpav_content_next(mUpnpAvContent *con)
{
	mUpnpXmlNode *nextNode;
	nextNode = mupnp_xml_node_next(con);
	while (nextNode) {
		if (mupnp_upnpav_content_iscontentnode(nextNode))
			return nextNode;
		nextNode = mupnp_xml_node_next(con);
	}
	return NULL;
}
