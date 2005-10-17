/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cupnp_function.c
*
*	Revision:
*
*	02/21/05
*		- first revision
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/cupnp.h>

#include <time.h>
#include <stdlib.h>

/****************************************
* Static
****************************************/

static BOOL isUpnpNMPRMode = FALSE;

/****************************************
* cg_upnp_createuuid
****************************************/

char *cg_upnp_createuuid(char *uuidBuf)
{
	time_t time1 = time(NULL);
	time_t time2 = (time_t)((double)time(NULL) * ((double)rand() / (double)RAND_MAX));
	sprintf(uuidBuf, "%04x-%04x-%04x-%04x",
		(int)(time1 & 0xFFFF),
		(int)(((time1 >> 31) | 0xA000) & 0xFFFF),
		(int)(time2 & 0xFFFF),
		(int)(((time2 >> 31) | 0xE000) & 0xFFFF));
	return uuidBuf;
}

/****************************************
* cg_upnp_getservername
****************************************/

char *cg_upnp_getservername(char *buf, int bufSize)
{
	int nameLen;
	
	cg_http_getservername(buf, bufSize);
	nameLen = cg_strlen(buf);
	bufSize -= nameLen;
	if (bufSize <= 0)
		return buf;
#if defined(HAVE_SNPRINTF)
	snprintf((buf+nameLen), bufSize, " UPnP/%s %s/%s", CG_UPNP_VER, CG_CLINK_NAME, CG_CLINK_VER);
#else
	sprintf((buf+nameLen), " UPnP/%s %s/%s", CG_UPNP_VER, CG_CLINK_NAME, CG_CLINK_VER);
#endif
	
	return buf;
}

/****************************************
* cg_upnp_setnmprmode
****************************************/

void cg_upnp_setnmprmode(BOOL onFlag)
{
	isUpnpNMPRMode = onFlag;
}

/****************************************
* cg_upnp_isnmprmode
****************************************/

BOOL cg_upnp_isnmprmode()
{
	return isUpnpNMPRMode;
}

