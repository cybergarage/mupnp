/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cupnp_function.h
*
*	Revision:
*
*	02/21/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNP_FUNCTION_H_
#define _CG_UPNP_FUNCTION_H_

#include <cybergarage/typedef.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_UPNP_UUID_NAME "uuid"
#define CG_UPNP_UUID_MAX_LEN (5 + ((4+1)*4) + 1)
#define CG_UPNP_SEVERNAME_MAXLEN 64

/****************************************
* Data Type
****************************************/

/****************************************
* Function
****************************************/

char *cg_upnp_createuuid(char *uuidBuf);
char *cg_upnp_getservername(char *buf, int bufSize);

void cg_upnp_setnmprmode(BOOL onFlag);
BOOL cg_upnp_isnmprmode();

#ifdef  __cplusplus
}
#endif

#endif
