/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifndef _MUPNP_TEST_DEVICE_H_
#define _MUPNP_TEST_DEVICE_H_

#include <mupnp/upnp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_DEVICE_DEVICE_TYPE "urn:schemas-upnp-org:device:BinaryLight:1"

#define TEST_DEVICE_EMBEDDEDDEVICE_TYPE1 "urn:schemas-upnp-org:device:EmbeddedsBinaryLight:1"
#define TEST_DEVICE_EMBEDDEDDEVICE_TYPE2 "urn:schemas-upnp-org:device:EmbeddedsBinaryLight:2"

#define TEST_DEVICE_SERVICE_TYPE "urn:schemas-upnp-org:service:SwitchPower:1"

#define TEST_DEVICE_FREINDLYNAME "CyberGarage Power Device"
#define TEST_DEVICE_STATEVARIABLE_STATUS "Status"

/****************************************
 * Function
 ****************************************/

mUpnpDevice* upnp_test_device_new(void);

#ifdef __cplusplus
}
#endif

#endif
