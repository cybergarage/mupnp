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

#include <cybergarage/util/cdictionary.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_dictionary_element_new
****************************************/

CgDictionaryElement *cg_dictionary_element_new()
{
	CgDictionaryElement *dictionaryElem;

	cg_log_debug_l4("Entering...\n");

	dictionaryElem = (CgDictionaryElement *)malloc(sizeof(CgDictionaryElement));

	cg_log_debug_s("Creating dictionaryElem data into %p\n", dictionaryElem);

	if ( NULL != dictionaryElem )
	{
		cg_list_node_init((CgList *)dictionaryElem);
		dictionaryElem->key = cg_string_new();
		dictionaryElem->value = cg_string_new();
	}

	cg_log_debug_l4("Leaving...\n");

	return dictionaryElem;
}

/****************************************
* cg_dictionary_element_delete
****************************************/

BOOL cg_dictionary_element_delete(CgDictionaryElement *dictionaryElem)
{
	cg_log_debug_l4("Entering...\n");

	cg_list_remove((CgList *)dictionaryElem);

	if (dictionaryElem->key)
		cg_string_delete(dictionaryElem->key);
	if (dictionaryElem->value)
		cg_string_delete(dictionaryElem->value);
	free(dictionaryElem);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}
