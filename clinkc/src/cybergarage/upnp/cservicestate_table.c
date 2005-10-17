/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cstataevariable_table.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cstatevariable.h>

/****************************************
* cg_upnp_servicestatetable_new
****************************************/

CgUpnpServiceStateTable *cg_upnp_servicestatetable_new()
{
	CgUpnpServiceStateTable *statevariableTable = (CgUpnpServiceStateTable *)malloc(sizeof(CgUpnpServiceStateTable));
	cg_list_header_init((CgList *)statevariableTable);
	return statevariableTable;
}

/****************************************
* cg_upnp_servicestatetable_delete
****************************************/

void cg_upnp_servicestatetable_delete(CgUpnpServiceStateTable *statevariableTable)
{
	cg_list_remove((CgList *)statevariableTable);
	free(statevariableTable);
}
