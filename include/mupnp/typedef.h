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

#ifndef _MUPNP_TYPEDEF_H_
#define _MUPNP_TYPEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(WINCE) && !defined(WIN32)
#define WIN32 1
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(BTRON) || defined(TENGINE)
#include <typedef.h>
#elif defined(ITRON)
#include <kernel.h>
#endif

#if defined(__APPLE_CPP__) || defined(__APPLE_CC__)
#include <TargetConditionals.h>
#endif
	
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Data Type
****************************************/

#if !defined(BOOL) && !defined(BTRON) && !defined(TENGINE) && !defined(__OBJC__)
typedef int BOOL;
#endif

#if !defined(TRUE)
#if defined(__OBJC__)
#define TRUE YES
#else
#define TRUE (1)
#endif
#endif

#if !defined(FALSE)
#if defined(__OBJC__)
#define FALSE NO
#else
#define FALSE (0)
#endif
#endif
	
typedef unsigned char mUpnpByte;

#ifdef  __cplusplus
}
#endif

#endif
