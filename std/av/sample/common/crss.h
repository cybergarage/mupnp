/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: crss.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNPAV_RSS_H_
#define _CG_UPNPAV_RSS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <mupnp/std/av/ccontent.h>

mUpnpMediaContent* mupnp_http_getrsscontents(char* rssURL);

#ifdef __cplusplus
}
#endif

#endif
