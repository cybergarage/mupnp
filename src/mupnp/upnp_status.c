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
*	File: cupnp_status.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <mupnp/upnp_status.h>
#include <mupnp/util/list.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_status_new
****************************************/

CgUpnpStatus *mupnp_upnp_status_new()
{
	CgUpnpStatus *upnpStat;

	mupnp_log_debug_l4("Entering...\n");

	upnpStat = (CgUpnpStatus *)malloc(sizeof(CgUpnpStatus));

	if ( NULL != upnpStat )
	{
		upnpStat->code = 0;
		upnpStat->description = mupnp_string_new();
	}

	return upnpStat;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_status_delete
****************************************/

void mupnp_upnp_status_delete(CgUpnpStatus *upnpStat)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_delete(upnpStat->description);
	free(upnpStat);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_status_code2string
****************************************/

char *mupnp_upnp_status_code2string(int code)
{
	mupnp_log_debug_l4("Entering...\n");

	switch (code) {
	case CG_UPNP_STATUS_INVALID_ACTION: return "Invalid Action";
	case CG_UPNP_STATUS_INVALID_ARGS: return "Invalid Args";
	case CG_UPNP_STATUS_OUT_OF_SYNC: return "Out of Sync";
	case CG_UPNP_STATUS_INVALID_VAR: return "Invalid Var";
	case CG_UPNP_STATUS_ACTION_FAILED: return "Action Failed";
	}
	 return "";

	mupnp_log_debug_l4("Leaving...\n");
}
