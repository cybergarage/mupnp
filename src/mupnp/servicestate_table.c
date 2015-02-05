/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cstataevariable_table.c
*
*	Revision:
*
*	02/23/05
*		- first revision
*
******************************************************************/

#include <mupnp/statevariable.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_servicestatetable_new
****************************************/

CgUpnpServiceStateTable *mupnp_upnp_servicestatetable_new()
{
	CgUpnpServiceStateTable *statevariableTable;

	mupnp_log_debug_l4("Entering...\n");

	statevariableTable = (CgUpnpServiceStateTable *)malloc(sizeof(CgUpnpServiceStateTable));

	if ( NULL != statevariableTable )
		mupnp_list_header_init((CgList *)statevariableTable);

	mupnp_log_debug_l4("Leaving...\n");
	
	return statevariableTable;
}

/****************************************
* mupnp_upnp_servicestatetable_delete
****************************************/

void mupnp_upnp_servicestatetable_delete(CgUpnpServiceStateTable *statevariableTable)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_list_remove((CgList *)statevariableTable);
	free(statevariableTable);

	mupnp_log_debug_l4("Leaving...\n");
}
