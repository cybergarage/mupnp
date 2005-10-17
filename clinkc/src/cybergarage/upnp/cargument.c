/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cargument.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cargument.h>

/****************************************
* cg_upnp_argument_new
****************************************/

CgUpnpArgument *cg_upnp_argument_new()
{
	CgUpnpArgument *arg = (CgUpnpArgument *)malloc(sizeof(CgUpnpArgument));
	cg_list_node_init((CgList *)arg);
	
	arg->serviceNode = NULL;
	arg->argumentNode = NULL;
	
	arg->value = cg_string_new();
	
	return arg;
}

/****************************************
* cg_upnp_argument_delete
****************************************/

void cg_upnp_argument_delete(CgUpnpArgument *arg)
{
	cg_list_remove((CgList *)arg);
	
	cg_string_delete(arg->value);
	
	free(arg);
}
