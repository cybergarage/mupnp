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

#ifndef _MUPNP_UPNPLIMIT_H_
#define _MUPNP_UPNPLIMIT_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#else
#include <limits.h>
#endif

#define MUPNP_ACTOINNAME_LEN_MAX 128

/* DLNA specifies (7.2.26.6) max UDN length to 68 bytes,
 * we just play safe here and use 128 byte buffer. */
#define MUPNP_UDN_LEN_MAX 128
#define MUPNP_LOCATION_URL_MAX 512
#define MUPNP_CONTROLPOINT_EVENTSUBURL_MAX 512

#if defined(HAVE_STDINT_H)
#define MUPNP_NOTIFY_COUNT_MAX UINT32_MAX
#else
#define MUPNP_NOTIFY_COUNT_MAX UINT_MAX
#endif

#endif
