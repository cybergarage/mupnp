/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdictionary_elem.c
*
*	Revision:
*
*	05/26/09
*		- first revision
*
******************************************************************/

#include <mupnp/util/dictionary.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_dictionary_element_new
****************************************/

CgDictionaryElement *mupnp_dictionary_element_new()
{
	CgDictionaryElement *dirElem;

	mupnp_log_debug_l4("Entering...\n");

	dirElem = (CgDictionaryElement *)malloc(sizeof(CgDictionaryElement));

	mupnp_log_debug_s("Creating dirElem data into %p\n", dirElem);

	if ( NULL != dirElem )
	{
		mupnp_list_node_init((CgList *)dirElem);
		dirElem->key = mupnp_string_new();
		dirElem->value = mupnp_string_new();
	}

	mupnp_log_debug_l4("Leaving...\n");

	return dirElem;
}

/****************************************
* mupnp_dictionary_element_delete
****************************************/

BOOL mupnp_dictionary_element_delete(CgDictionaryElement *dirElem)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)dirElem);

	if (dirElem->key)
		mupnp_string_delete(dirElem->key);
	if (dirElem->value)
		mupnp_string_delete(dirElem->value);
	free(dirElem);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}
