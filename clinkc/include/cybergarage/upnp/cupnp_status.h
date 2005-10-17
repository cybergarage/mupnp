/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: upnp_status.h
*
*	Revision:
*
*	02/13/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_CUPNPSTATUS_H_
#define _CG_UPNP_CUPNPSTATUS_H_

#include <cybergarage/typedef.h>
#include <cybergarage/util/cstring.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_STATUS_INVALID_ACTION 401
#define CG_UPNP_STATUS_INVALID_ARGS 402
#define CG_UPNP_STATUS_OUT_OF_SYNC 403
#define CG_UPNP_STATUS_INVALID_VAR 404
#define CG_UPNP_STATUS_ACTION_FAILED 501

/****************************************
* Data Type
****************************************/

typedef struct _CgUpnpStatus {
	int code;
	CgString *description;
} CgUpnpStatus;

/****************************************
* Function
****************************************/

CgUpnpStatus *cg_upnp_status_new();
void cg_upnp_status_delete(CgUpnpStatus *upnpStat);

#define cg_upnp_status_setcode(upnpStat, value) (upnpStat->code = value)
#define cg_upnp_status_getcode(upnpStat) (upnpStat->code)

#define cg_upnp_status_setdescription(upnpStat, value) cg_string_setvalue(upnpStat->description, value)
#define cg_upnp_status_getdescription(upnpStat) cg_string_getvalue(upnpStat->description)

char *cg_upnp_status_code2string(int code);

#ifdef  __cplusplus
}
#endif

#endif
