/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cupnpav.h
*
*	Revision:
*       2008/06/16
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_CUPNPAV_H_
#define _CG_CLINKCAV_CUPNPAV_H_

#include <mupnp/http/http.h>
#include <mupnp/upnp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CG_UPNPAV_OBJECT_ID "id"
#define CG_UPNPAV_OBJECT_PARENTID "parentID"
#define CG_UPNPAV_OBJECT_CHILDCOUNT "childCount"
#define CG_UPNPAV_OBJECT_RESTRICTED "restricted"
#define CG_UPNPAV_OBJECT_TITLE "dc:title"
#define CG_UPNPAV_OBJECT_DATE "dc:date"
#define CG_UPNPAV_OBJECT_UPNPCLASS "upnp:class"
#define CG_UPNPAV_OBJECT_ALBUMARTURI "upnp:albumArtURI"
#define CG_UPNPAV_OBJECT_ROOT_ID "0"

typedef BOOL (*CG_UPNPAV_HTTP_LISTENER)(mUpnpHttpRequest*);
typedef BOOL (*CG_UPNPAV_ACTION_LISTNER)(mUpnpAction*);
typedef BOOL (*CG_UPNPAV_STATEVARIABLE_LISTNER)(mUpnpStateVariable*);

#ifdef __cplusplus
}
#endif

#include <mupnp/std/av/cdidl.h>
#include <mupnp/std/av/ccontent.h>
#include <mupnp/std/av/cmd5.h>
#include <mupnp/std/av/cresource.h>
#include <mupnp/std/av/cprotocolinfo.h>
#include <mupnp/std/av/cmediaserver.h>
#include <mupnp/std/av/cmediarenderer.h>

#endif
