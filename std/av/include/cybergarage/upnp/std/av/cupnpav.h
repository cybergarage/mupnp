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

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (*CG_UPNPAV_HTTP_LISTENER)(CgHttpRequest *);
typedef BOOL (*CG_UPNPAV_ACTION_LISTNER)(CgUpnpAction *);
typedef BOOL (*CG_UPNPAV_STATEVARIABLE_LISTNER)(CgUpnpStateVariable *);

#ifdef __cplusplus
}
#endif

#include <cybergarage/upnp/std/av/cdidl.h>
#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/upnp/std/av/cmd5.h>
#include <cybergarage/upnp/std/av/cresource.h>
#include <cybergarage/upnp/std/av/cprotocolinfo.h>
#include <cybergarage/upnp/std/av/cmediaserver.h>
#include <cybergarage/upnp/std/av/cmediarenderer.h>

#endif
