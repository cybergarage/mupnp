/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cinterface.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>

/****************************************
* cg_net_interface_new
****************************************/

CgNetworkInterface *cg_net_interface_new()
{
	CgNetworkInterface *netIf = (CgNetworkInterface *)malloc(sizeof(CgNetworkInterface));
	cg_list_node_init((CgList *)netIf);
	netIf->name = cg_string_new();
	netIf->ipaddr = cg_string_new();
	netIf->index = 0;
	return netIf;
}

/****************************************
* cg_net_interface_delete
****************************************/

void cg_net_interface_delete(CgNetworkInterface *netIf)
{
	cg_list_remove((CgList *)netIf);
	cg_string_delete(netIf->name);
	cg_string_delete(netIf->ipaddr);
	free(netIf);
}

/****************************************
* cg_net_interface_getany
****************************************/

CgNetworkInterface *cg_net_interface_getany()
{
	CgNetworkInterface *netIf = cg_net_interface_new();
	/* It would be more approriate to use INADDR_ANY, but this will do */
	cg_net_interface_setname(netIf, "INADDR_ANY");
	cg_net_interface_setaddress(netIf, "0.0.0.0");
	
	return netIf;
}

/****************************************
* cg_net_interface_setname
****************************************/

void cg_net_interface_setname(CgNetworkInterface *netIf, char *name)
{
	cg_string_setvalue(netIf->name, name);
}

/****************************************
* cg_net_interface_getname
****************************************/

char *cg_net_interface_getname(CgNetworkInterface *netIf)
{
	return cg_string_getvalue(netIf->name);
}

/****************************************
* cg_net_interface_setaddress
****************************************/

void cg_net_interface_setaddress(CgNetworkInterface *netIf, char *value)
{
	cg_string_setvalue(netIf->ipaddr, value);
}

/****************************************
* cg_net_interface_getaddress
****************************************/

char *cg_net_interface_getaddress(CgNetworkInterface *netIf)
{
	return cg_string_getvalue(netIf->ipaddr);
}

/****************************************
* cg_net_interface_setindex
****************************************/

void cg_net_interface_setindex(CgNetworkInterface *netIf, int value)
{
	netIf->index = value;
}

/****************************************
* cg_net_interface_getaddress
****************************************/

int cg_net_interface_getindex(CgNetworkInterface *netIf)
{
	return netIf->index;
}
