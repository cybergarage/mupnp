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
*	File: chttp_header.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <mupnp/util/list.h>
#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_header_new
****************************************/

CgHttpHeader *mupnp_http_header_new()
{
	CgHttpHeader *header;
	
	mupnp_log_debug_l4("Entering...\n");

	header = (CgHttpHeader *)malloc(sizeof(CgHttpHeader));

	if ( NULL != header )
	{
		mupnp_list_node_init((CgList *)header);
		header->name = mupnp_string_new();
		header->value = mupnp_string_new();
	}

	return header;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_delete
****************************************/

void mupnp_http_header_delete(CgHttpHeader *header)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)header);
	mupnp_string_delete(header->name);
	mupnp_string_delete(header->value);
	free(header);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_setname
****************************************/

void mupnp_http_header_setname(CgHttpHeader *header, const char *name)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(header->name, name);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_getname
****************************************/

const char *mupnp_http_header_getname(CgHttpHeader *header)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(header->name);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_setvalue
****************************************/

void mupnp_http_header_setvalue(CgHttpHeader *header, const char *value)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(header->value, value);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_header_getvalue
****************************************/

const char *mupnp_http_header_getvalue(CgHttpHeader *header)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(header->value);

	mupnp_log_debug_l4("Leaving...\n");
}
