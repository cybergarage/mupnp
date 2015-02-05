/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cxml.h
*
*	Revision:
*
*	02/07/05
*		- first revision
*	10/22/07 Aapo Makela
*		- Added NULL check to mupnp_xml_node_getchildnode() macro
*
******************************************************************/

#ifndef _CG_XML_CXML_H_
#define _CG_XML_CXML_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_XML_INDENT_STRING " "
#define CG_XML_VERSION_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define CG_XML_CONTENT_TYPE "text/xml; charset=\"utf-8\""
#define CG_XML_CONTENT_LF "\n"

typedef void (*CG_XML_NODE_USERDATA_DESTRUCTORFUNC)(void *);

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpXmlAttribute {
	BOOL headFlag;
	struct _mUpnpXmlAttribute *prev;
	struct _mUpnpXmlAttribute *next;
	mUpnpString *name;
	mUpnpString *value;
} mUpnpXmlAttribute, mUpnpXmlAttributeList;

typedef struct _mUpnpXmlNode {
	BOOL headFlag;
	struct _mUpnpXmlNode *prev;
	struct _mUpnpXmlNode *next;
	mUpnpString *name;
	mUpnpString *value;
	struct _mUpnpXmlAttribute *attrList;
	struct _mUpnpXmlNode *nodeList;
	struct _mUpnpXmlNode *parentNode;
	void *userData;
	CG_XML_NODE_USERDATA_DESTRUCTORFUNC userDataDestructorFunc;
} mUpnpXmlNode, mUpnpXmlNodeList;

typedef struct _mUpnpXmlParser {
	BOOL parseResult;
} mUpnpXmlParser;

/****************************************
* Function (AttribureList)
****************************************/

mUpnpXmlAttributeList *mupnp_xml_attributelist_new();
void mupnp_xml_attributelist_delete(mUpnpXmlAttributeList *attrList);

#define mupnp_xml_attributelist_clear(attrList) mupnp_list_clear((mUpnpList *)attrList, (CG_LIST_DESTRUCTORFUNC)mupnp_xml_attribute_delete)
#define mupnp_xml_attributelist_size(attrList) mupnp_list_size((mUpnpList *)attrList)
#define mupnp_xml_attributelist_gets(attrList) (mUpnpXmlAttribute *)mupnp_list_next((mUpnpList *)attrList)
#define mupnp_xml_attributelist_add(attrList, attr) mupnp_list_add((mUpnpList *)attrList, (mUpnpList *)attr)

void mupnp_xml_attributelist_set(mUpnpXmlAttributeList *attrList, const char *name, const char *value);
mUpnpXmlAttribute *mupnp_xml_attributelist_get(mUpnpXmlAttributeList *attrList, const char *name);
const char *mupnp_xml_attributelist_getvalue(mUpnpXmlAttributeList *attrList, const char *name);

/****************************************
* Function (Attributes)
****************************************/

mUpnpXmlAttribute *mupnp_xml_attribute_new();
void mupnp_xml_attribute_delete(mUpnpXmlAttribute *attr);

#define mupnp_xml_attribute_next(attr) (mUpnpXmlAttribute *)mupnp_list_next((mUpnpList *)attr)
#define mupnp_xml_attribute_remove(attr) mupnp_list_remove((mUpnpList *)attr)

void mupnp_xml_attribute_setname(mUpnpXmlAttribute *attr, const char *name);
const char *mupnp_xml_attribute_getname(mUpnpXmlAttribute *attr);
BOOL mupnp_xml_attribute_isname(mUpnpXmlAttribute *attr, const char *name);

void mupnp_xml_attribute_setvalue(mUpnpXmlAttribute *attr, const char *value);
const char *mupnp_xml_attribute_getvalue(mUpnpXmlAttribute *attr);

/****************************************
* Function (NodeList)
****************************************/

mUpnpXmlNodeList *mupnp_xml_nodelist_new();
void mupnp_xml_nodelist_delete(mUpnpXmlNodeList *nodeList);

#define mupnp_xml_nodelist_clear(nodeList) mupnp_list_clear((mUpnpList *)(nodeList), (CG_LIST_DESTRUCTORFUNC)mupnp_xml_node_delete)
#define mupnp_xml_nodelist_size(nodeList) mupnp_list_size((mUpnpList *)(nodeList))
#define mupnp_xml_nodelist_gets(nodeList) (mUpnpXmlNode *)mupnp_list_next((mUpnpList *)(nodeList))
#define mupnp_xml_nodelist_add(nodeList, node) mupnp_list_add((mUpnpList *)(nodeList), (mUpnpList *)(node))

/* Deprecated : Use mupnp_xml_nodelist_getbyname */
#define mupnp_xml_nodelist_get(nodeList, name) mupnp_xml_nodelist_getbyname(nodeList, name)
mUpnpXmlNode *mupnp_xml_nodelist_getbyname(mUpnpXmlNodeList *nodeList, const char *name);
mUpnpXmlNode *mupnp_xml_nodelist_getbyxpath(mUpnpXmlNodeList *nodeList, const char *xpath);

/****************************************
* Function (Node)
****************************************/

mUpnpXmlNode *mupnp_xml_node_new();
void mupnp_xml_node_delete(mUpnpXmlNode *node);

#define mupnp_xml_node_next(node) (mUpnpXmlNode *)mupnp_list_next((mUpnpList *)node)
#define mupnp_xml_node_remove(node) mupnp_list_remove((mUpnpList *)node)

#define mupnp_xml_node_setname(node, nodename) mupnp_string_setvalue(node->name, nodename)
#define mupnp_xml_node_getname(node) mupnp_string_getvalue(node->name)
#define mupnp_xml_node_isname(node, nodename) (mupnp_strcmp(mupnp_xml_node_getname(node), nodename) == 0 ? TRUE : FALSE)

#define mupnp_xml_node_setvalue(node, nodevalue) mupnp_string_setvalue(node->value, nodevalue)
#define mupnp_xml_node_setintvalue(node, nodevalue) mupnp_string_setintvalue(node->value, nodevalue)
#define mupnp_xml_node_setlongvalue(node, nodevalue) mupnp_string_setlongvalue(node->value, nodevalue)
#define mupnp_xml_node_setnvalue(node, nodevalue, nodevaluelen) mupnp_string_setnvalue(node->value, nodevalue, nodevaluelen)
#define mupnp_xml_node_addvalue(node, nodevalue) mupnp_string_addvalue(node->value, nodevalue)
#define mupnp_xml_node_naddvalue(node, nodevalue, len) mupnp_string_naddvalue(node->value, nodevalue, len)
#define mupnp_xml_node_getvalue(node) mupnp_string_getvalue(node->value)
#define mupnp_xml_node_getintvalue(node) mupnp_string_getintvalue(node->value)
#define mupnp_xml_node_getlongvalue(node) mupnp_string_getlongvalue(node->value)

#define mupnp_xml_node_setparentnode(node, pnode) (node->parentNode = pnode)
#define mupnp_xml_node_getparentnode(node) (node->parentNode)
mUpnpXmlNode *mupnp_xml_node_getrootnode(mUpnpXmlNode *node);

#define mupnp_xml_node_getchildnodelist(node) (node->nodeList)
void mupnp_xml_node_addchildnode(mUpnpXmlNode *node, mUpnpXmlNode *cnode);
void mupnp_xml_node_setchildnode(mUpnpXmlNode *node, const char *name, const char *value);
BOOL mupnp_xml_node_removechildnode(mUpnpXmlNode *node, const char *name);
const char *mupnp_xml_node_getchildnodevalue(mUpnpXmlNode *node, const char *name);
#define mupnp_xml_node_getchildnodeintvalue(node, name) mupnp_str2int(mupnp_xml_node_getchildnodevalue(node, name))
#define mupnp_xml_node_getchildnodelongvalue(node, name) mupnp_str2long(mupnp_xml_node_getchildnodevalue(node, name))
	
#define mupnp_xml_node_setuserdata(node, data) (node->userData = data)
#define mupnp_xml_node_getuserdata(node) (node->userData)
#define mupnp_xml_node_setuserdatadestructor(node, func) (node->userDataDestructorFunc = func)

#define mupnp_xml_node_getchildnodes(node) mupnp_xml_nodelist_gets(node->nodeList)
#define mupnp_xml_node_haschildnodes(node) ((mupnp_xml_node_getchildnodes(node) != NULL) ? TRUE : FALSE) 
/* Deprecated : Use mupnp_xml_node_getchildnodebyname */
#define mupnp_xml_node_getchildnode(node,name) ((node != NULL) ? mupnp_xml_nodelist_getbyname(node->nodeList,name) : NULL)
#define mupnp_xml_node_getchildnodebyname(node,name) ((node != NULL) ? mupnp_xml_nodelist_getbyname(node->nodeList,name) : NULL)
#define mupnp_xml_node_getchildnodebyxpath(node,xpath) ((node != NULL) ? mupnp_xml_nodelist_getbyxpath(node->nodeList,xpath) : NULL)

/* Get childnode with some specific namespace prefix, or ignore namespace prefix.
   If ns is NULL, name string must match completely */
mUpnpXmlNode *mupnp_xml_node_getchildnodewithnamespace(mUpnpXmlNode *node, const char *name, const char *ns, BOOL ignoreNs);

#define mupnp_xml_node_getattributes(node) mupnp_xml_attributelist_gets(node->attrList)
#define mupnp_xml_node_getattribute(node, name) mupnp_xml_attributelist_get(node->attrList,name)
#define mupnp_xml_node_addattribute(node, attr) mupnp_xml_attributelist_add(node->attrList,attr)
#define mupnp_xml_node_setattribute(node, name, value) mupnp_xml_attributelist_set(node->attrList,name,value)
#define mupnp_xml_node_getattributevalue(node, name) mupnp_xml_attributelist_getvalue(node->attrList, name)
BOOL mupnp_xml_node_removeattribute(mUpnpXmlNode *node, const char *name);

#define mupnp_xml_node_setnamespace(node, ns, value) mupnp_xml_node_setattribute(node, "xmlns:" ns , value)

char *mupnp_xml_node_tostring(mUpnpXmlNode *node, BOOL withChildNode, mUpnpString *str);
void  mupnp_xml_node_print(mUpnpXmlNode *node);

void mupnp_xml_node_copy(mUpnpXmlNode *dstNode, mUpnpXmlNode *srcNode);

/****************************************
* Function (Parser)
****************************************/

mUpnpXmlParser *mupnp_xml_parser_new();
void mupnp_xml_parser_delete(mUpnpXmlParser *parser);
BOOL mupnp_xml_parse(mUpnpXmlParser *parser, mUpnpXmlNodeList *nodeList, const char *data, size_t len);

/****************************************
* Function (Other)
****************************************/

char *mupnp_xml_escapechars(mUpnpString *str);
char *mupnp_xml_unescapechars(mUpnpString *str);


#ifdef  __cplusplus
}
#endif

#endif
