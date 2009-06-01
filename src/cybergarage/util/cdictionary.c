/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdictionary.c
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
	CgDictionary *dictionaryList;

	cg_log_debug_l4("Entering...\n");

	dictionaryList = (CgDictionary *)malloc(sizeof(CgDictionary));

	if ( NULL != dictionaryList )
	{
		cg_list_header_init((CgList *)dictionaryList);
		dictionaryList->key = NULL;
		dictionaryList->value = NULL;
	}

	cg_log_debug_l4("Leaving...\n");

	return dictionaryList;
}

/****************************************
* cg_dictionary_delete
****************************************/

void cg_dictionary_delete(CgDictionary *dictionaryList)
{
	cg_log_debug_l4("Entering...\n");

	cg_dictionary_clear(dictionaryList);
	free(dictionaryList);

	cg_log_debug_l4("Leaving...\n");
}

