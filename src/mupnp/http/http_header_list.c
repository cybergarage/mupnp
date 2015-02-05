/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_header_list.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_headerlist_new
****************************************/

CgHttpHeaderList *mupnp_http_headerlist_new()
{
	CgHttpHeaderList *headerList;

	mupnp_log_debug_l4("Entering...\n");

	headerList = (CgHttpHeaderList *)malloc(sizeof(CgHttpHeaderList));

	if ( NULL != headerList )
	{
		mupnp_list_header_init((CgList *)headerList);
		headerList->name = NULL;
		headerList->value = NULL;
	}

	return headerList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_delete
****************************************/

void mupnp_http_headerlist_delete(CgHttpHeaderList *headerList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_headerlist_clear(headerList);
	free(headerList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_getheader
****************************************/

CgHttpHeader *mupnp_http_headerlist_get(CgHttpHeaderList *headerList, const char *name)
{
	CgHttpHeader *header;
	const char *headerName;
		
	mupnp_log_debug_l4("Entering...\n");

	if (name == NULL)
		return NULL;
		
	for (header = mupnp_http_headerlist_gets(headerList); header != NULL; header = mupnp_http_header_next(header)) {
		headerName = mupnp_http_header_getname(header);
		if (headerName == NULL)
			continue;
		if (mupnp_strcasecmp(headerName, name) == 0)
			return header;
	}
	
	return NULL;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_setheader
****************************************/

void mupnp_http_headerlist_set(CgHttpHeaderList *headerList, const char *name, const char *value)
{
	CgHttpHeader *header;
	
	mupnp_log_debug_l4("Entering...\n");

	header = mupnp_http_headerlist_get(headerList, name);
	if (header == NULL) {
		header = mupnp_http_header_new();
		mupnp_http_headerlist_add(headerList, header);
		mupnp_http_header_setname(header, name);
	}
	
	mupnp_http_header_setvalue(header, value);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_headerlist_setheader
****************************************/

const char *mupnp_http_headerlist_getvalue(CgHttpHeaderList *headerList, const char *name)
{
	CgHttpHeader *header;
	
	mupnp_log_debug_l4("Entering...\n");

	header = mupnp_http_headerlist_get(headerList, name);
	if (header != NULL)
		return mupnp_http_header_getvalue(header);
		
	return NULL;

	mupnp_log_debug_l4("Leaving...\n");
}
