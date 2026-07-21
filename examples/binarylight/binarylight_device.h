/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2026
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifndef _CG_BINARYLIGHT_DEVICE_H_
#define _CG_BINARYLIGHT_DEVICE_H_

#include <mupnp/upnp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UPNP_BINARYLIGHT_DEVICE_TYPE "urn:schemas-upnp-org:device:BinaryLight:1"
#define UPNP_BINARYLIGHT_SERVICE_TYPE "urn:schemas-upnp-org:service:SwitchPower:1"

mUpnpDevice* upnp_binarylight_device_new(void);
bool upnp_binarylight_device_settarget(mUpnpDevice* dev, bool target);
bool upnp_binarylight_device_gettarget(mUpnpDevice* dev, bool* target);
bool upnp_binarylight_device_getstatus(mUpnpDevice* dev, bool* status);

#ifdef __cplusplus
}
#endif

#endif
