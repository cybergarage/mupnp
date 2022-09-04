/******************************************************************
*
*	mUPnP for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cwebservice.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNPAV_WEBSERVICE_H_
#define _CG_UPNPAV_WEBSERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <mupnp/util/string.h>

BOOL mupnp_http_getrestresponse(char* url, mUpnpString* str);
BOOL mupnp_http_getredirecturl(char* orgURL, char* redirectURL, int redirectURLSize);

#ifdef __cplusplus
}
#endif

#endif
