/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: chttp_header.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/http/chttp.h>

/****************************************
* cg_http_header_new
****************************************/

CgHttpHeader *cg_http_header_new()
{
	CgHttpHeader *header = (CgHttpHeader *)malloc(sizeof(CgHttpHeader));
	cg_list_node_init((CgList *)header);
	header->name = cg_string_new();
	header->value = cg_string_new();
	return header;
}

/****************************************
* cg_http_header_delete
****************************************/

void cg_http_header_delete(CgHttpHeader *header)
{
	cg_list_remove((CgList *)header);
	cg_string_delete(header->name);
	cg_string_delete(header->value);
	free(header);
}

/****************************************
* cg_http_header_setname
****************************************/

void cg_http_header_setname(CgHttpHeader *header, char *name)
{
	cg_string_setvalue(header->name, name);
}

/****************************************
* cg_http_header_getname
****************************************/

char *cg_http_header_getname(CgHttpHeader *header)
{
	return cg_string_getvalue(header->name);
}

/****************************************
* cg_http_header_setvalue
****************************************/

void cg_http_header_setvalue(CgHttpHeader *header, char *value)
{
	cg_string_setvalue(header->value, value);
}

/****************************************
* cg_http_header_getvalue
****************************************/

char *cg_http_header_getvalue(CgHttpHeader *header)
{
	return cg_string_getvalue(header->value);
}
