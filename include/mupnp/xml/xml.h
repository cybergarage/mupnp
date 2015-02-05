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

typedef struct _CgXmlAttribute {
	BOOL headFlag;
	struct _CgXmlAttribute *prev;
	struct _CgXmlAttribute *next;
	CgString *name;
	CgString *value;
} CgXmlAttribute, CgXmlAttributeList;

typedef struct _CgXmlNode {
	BOOL headFlag;
	struct _CgXmlNode *prev;
	struct _CgXmlNode *next;
	CgString *name;
	CgString *value;
	struct _CgXmlAttribute *attrList;
	struct _CgXmlNode *nodeList;
	struct _CgXmlNode *parentNode;
	void *userData;
	CG_XML_NODE_USERDATA_DESTRUCTORFUNC userDataDestructorFunc;
} CgXmlNode, CgXmlNodeList;

typedef struct _CgXmlParser {
	BOOL parseResult;
} CgXmlParser;

/****************************************
* Function (AttribureList)
****************************************/

CgXmlAttributeList *mupnp_xml_attributelist_new();
void mupnp_xml_attributelist_delete(CgXmlAttributeList *attrList);

#define mupnp_xml_attributelist_clear(attrList) mupnp_list_clear((CgList *)attrList, (CG_LIST_DESTRUCTORFUNC)mupnp_xml_attribute_delete)
#define mupnp_xml_attributelist_size(attrList) mupnp_list_size((CgList *)attrList)
#define mupnp_xml_attributelist_gets(attrList) (CgXmlAttribute *)mupnp_list_next((CgList *)attrList)
#define mupnp_xml_attributelist_add(attrList, attr) mupnp_list_add((CgList *)attrList, (CgList *)attr)

void mupnp_xml_attributelist_set(CgXmlAttributeList *attrList, const char *name, const char *value);
CgXmlAttribute *mupnp_xml_attributelist_get(CgXmlAttributeList *attrList, const char *name);
const char *mupnp_xml_attributelist_getvalue(CgXmlAttributeList *attrList, const char *name);

/****************************************
* Function (Attributes)
****************************************/

CgXmlAttribute *mupnp_xml_attribute_new();
void mupnp_xml_attribute_delete(CgXmlAttribute *attr);

#define mupnp_xml_attribute_next(attr) (CgXmlAttribute *)mupnp_list_next((CgList *)attr)
#define mupnp_xml_attribute_remove(attr) mupnp_list_remove((CgList *)attr)

void mupnp_xml_attribute_setname(CgXmlAttribute *attr, const char *name);
const char *mupnp_xml_attribute_getname(CgXmlAttribute *attr);
BOOL mupnp_xml_attribute_isname(CgXmlAttribute *attr, const char *name);

void mupnp_xml_attribute_setvalue(CgXmlAttribute *attr, const char *value);
const char *mupnp_xml_attribute_getvalue(CgXmlAttribute *attr);

/****************************************
* Function (NodeList)
****************************************/

CgXmlNodeList *mupnp_xml_nodelist_new();
void mupnp_xml_nodelist_delete(CgXmlNodeList *nodeList);

#define mupnp_xml_nodelist_clear(nodeList) mupnp_list_clear((CgList *)(nodeList), (CG_LIST_DESTRUCTORFUNC)mupnp_xml_node_delete)
#define mupnp_xml_nodelist_size(nodeList) mupnp_list_size((CgList *)(nodeList))
#define mupnp_xml_nodelist_gets(nodeList) (CgXmlNode *)mupnp_list_next((CgList *)(nodeList))
#define mupnp_xml_nodelist_add(nodeList, node) mupnp_list_add((CgList *)(nodeList), (CgList *)(node))

/* Deprecated : Use mupnp_xml_nodelist_getbyname */
#define mupnp_xml_nodelist_get(nodeList, name) mupnp_xml_nodelist_getbyname(nodeList, name)
CgXmlNode *mupnp_xml_nodelist_getbyname(CgXmlNodeList *nodeList, const char *name);
CgXmlNode *mupnp_xml_nodelist_getbyxpath(CgXmlNodeList *nodeList, const char *xpath);

/****************************************
* Function (Node)
****************************************/

CgXmlNode *mupnp_xml_node_new();
void mupnp_xml_node_delete(CgXmlNode *node);

#define mupnp_xml_node_next(node) (CgXmlNode *)mupnp_list_next((CgList *)node)
#define mupnp_xml_node_remove(node) mupnp_list_remove((CgList *)node)

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
CgXmlNode *mupnp_xml_node_getrootnode(CgXmlNode *node);

#define mupnp_xml_node_getchildnodelist(node) (node->nodeList)
void mupnp_xml_node_addchildnode(CgXmlNode *node, CgXmlNode *cnode);
void mupnp_xml_node_setchildnode(CgXmlNode *node, const char *name, const char *value);
BOOL mupnp_xml_node_removechildnode(CgXmlNode *node, const char *name);
const char *mupnp_xml_node_getchildnodevalue(CgXmlNode *node, const char *name);
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
CgXmlNode *mupnp_xml_node_getchildnodewithnamespace(CgXmlNode *node, const char *name, const char *ns, BOOL ignoreNs);

#define mupnp_xml_node_getattributes(node) mupnp_xml_attributelist_gets(node->attrList)
#define mupnp_xml_node_getattribute(node, name) mupnp_xml_attributelist_get(node->attrList,name)
#define mupnp_xml_node_addattribute(node, attr) mupnp_xml_attributelist_add(node->attrList,attr)
#define mupnp_xml_node_setattribute(node, name, value) mupnp_xml_attributelist_set(node->attrList,name,value)
#define mupnp_xml_node_getattributevalue(node, name) mupnp_xml_attributelist_getvalue(node->attrList, name)
BOOL mupnp_xml_node_removeattribute(CgXmlNode *node, const char *name);

#define mupnp_xml_node_setnamespace(node, ns, value) mupnp_xml_node_setattribute(node, "xmlns:" ns , value)

char *mupnp_xml_node_tostring(CgXmlNode *node, BOOL withChildNode, CgString *str);
void  mupnp_xml_node_print(CgXmlNode *node);

void mupnp_xml_node_copy(CgXmlNode *dstNode, CgXmlNode *srcNode);

/****************************************
* Function (Parser)
****************************************/

CgXmlParser *mupnp_xml_parser_new();
void mupnp_xml_parser_delete(CgXmlParser *parser);
BOOL mupnp_xml_parse(CgXmlParser *parser, CgXmlNodeList *nodeList, const char *data, size_t len);

/****************************************
* Function (Other)
****************************************/

char *mupnp_xml_escapechars(CgString *str);
char *mupnp_xml_unescapechars(CgString *str);


#ifdef  __cplusplus
}
#endif

#endif
