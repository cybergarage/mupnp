/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cupnp_status.c
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/cupnp_status.h>
#include <cybergarage/util/clist.h>

/****************************************
* cg_upnp_status_new
****************************************/

CgUpnpStatus *cg_upnp_status_new()
{
	CgUpnpStatus *upnpStat = (CgUpnpStatus *)malloc(sizeof(CgUpnpStatus));
	upnpStat->code = 0;
	upnpStat->description = cg_string_new();
	return upnpStat;
}

/****************************************
* cg_upnp_status_delete
****************************************/

void cg_upnp_status_delete(CgUpnpStatus *upnpStat)
{
	cg_string_delete(upnpStat->description);
	free(upnpStat);
}

/****************************************
* cg_upnp_status_code2string
****************************************/

char *cg_upnp_status_code2string(int code)
{
	switch (code) {
	case CG_UPNP_STATUS_INVALID_ACTION: return "Invalid Action";
	case CG_UPNP_STATUS_INVALID_ARGS: return "Invalid Args";
	case CG_UPNP_STATUS_OUT_OF_SYNC: return "Out of Sync";
	case CG_UPNP_STATUS_INVALID_VAR: return "Invalid Var";
	case CG_UPNP_STATUS_ACTION_FAILED: return "Action Failed";
	}
	 return "";
}
