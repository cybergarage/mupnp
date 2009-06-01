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
*	File: cdictionary.cpp
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
* cg_dictionary_new
****************************************/

CgDictionary *cg_dictionary_new()
{
	CgDictionary *dictionary;

	cg_log_debug_l4("Entering...\n");

	dictionary = (CgDictionary *)malloc(sizeof(CgDictionary));

	cg_log_debug_s("Creating dictionary data into %p\n", dictionary);

	if ( NULL != dictionary )
	{
		cg_list_node_init((CgList *)dictionary);
		dictionary->key = cg_string_new();
		dictionary->value = cg_string_new();
	}

	return dictionary;
}

/****************************************
* cg_dictionary_delete
****************************************/

BOOL cg_dictionary_delete(CgDictionary *dictionary)
{
	if (dictionary->key)
		cg_string_delete(dictionary->key);
	if (dictionary->value)
		cg_string_delete(dictionary->value);
	free(dictionary);

	return TRUE;
}
