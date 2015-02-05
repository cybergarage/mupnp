/******************************************************************
*
*	CyberXML for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_attribute.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <mupnp/xml/xml.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_xml_attribute_new
****************************************/

CgXmlAttribute *mupnp_xml_attribute_new()
{
	CgXmlAttribute *attr;

	mupnp_log_debug_l4("Entering...\n");

	attr = (CgXmlAttribute *)malloc(sizeof(CgXmlAttribute));

	if ( NULL != attr )
	{
		mupnp_list_node_init((CgList *)attr);
		attr->name = mupnp_string_new();
		attr->value = mupnp_string_new();
	}
	return attr;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_delete
****************************************/

void mupnp_xml_attribute_delete(CgXmlAttribute *attr)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)attr);
	mupnp_string_delete(attr->name);
	mupnp_string_delete(attr->value);
	free(attr);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setname
****************************************/

void mupnp_xml_attribute_setname(CgXmlAttribute *attr, const char *name)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(attr->name, name);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_getname
****************************************/

const char *mupnp_xml_attribute_getname(CgXmlAttribute *attr)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(attr->name);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_isname
****************************************/

BOOL mupnp_xml_attribute_isname(CgXmlAttribute *attr, const char *name)
{
	mupnp_log_debug_l4("Entering...\n");

	return ((mupnp_strcmp(mupnp_xml_attribute_getname(attr), name) == 0) ? TRUE : FALSE);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setvalue
****************************************/

void mupnp_xml_attribute_setvalue(CgXmlAttribute *attr, const char *value)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(attr->value, value);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_xml_attribute_setvalue
****************************************/

const char *mupnp_xml_attribute_getvalue(CgXmlAttribute *attr)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(attr->value);

	mupnp_log_debug_l4("Leaving...\n");
}
