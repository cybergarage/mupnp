/******************************************************************
*
* CyberGarage for C
*
* Copyright (C) Satoshi Konno 2005
*
* File: typedef.h
*
* Revision:
*
* 01/17/05
*  - first revision
* 08/16/05
*  - Thanks for Theo Beisch <theo.beisch@gmx.de>
*  - Added support for WindowsCE platform.
* 03/18/07
*  - Added CgInt64 to <typedef.h>
* 06/24/07
*  - Added CgByte to <typedef.h>
*
******************************************************************/

#ifndef _CG_TYPEDEF_H_
#define _CG_TYPEDEF_H_

#include <stdio.h>
#include <stdlib.h>

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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Data Type
****************************************/

#if !defined(BOOL) && !defined(BTRON) && !defined(TENGINE)
typedef int BOOL;
#endif

#if !defined(TRUE)
#define TRUE (1)
#endif

#if !defined(FALSE)
#define FALSE (0)
#endif

typedef unsigned char CgByte;

#if defined(__USE_ISOC99)
typedef long long int CgInt64;
#define CG_USE_INT64 1
#elif defined(HAVE_LONGLONG)
typedef long long CgInt64;
#define CG_USE_INT64 1
#elif (defined(WIN32) || defined(WINCE))
typedef __int64 CgInt64;
#define CG_USE_INT64 1
#else
typedef long CgInt64;
#undef CG_USE_INT64
#endif

#ifdef  __cplusplus
}
#endif

#endif
