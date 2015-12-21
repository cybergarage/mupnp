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

#ifndef _MUPNP_UTIL_TIME_H_
#define _MUPNP_UTIL_TIME_H_

#include <mupnp/typedef.h>

#if defined(BTRON)
#include <btron/proctask.h>
#include <btron/clk.h>
#elif defined(ITRON)
#include <kernel.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#if defined(BTRON) || defined(ITRON)
typedef size_t mUpnpTime;
#else
typedef long mUpnpTime;
#endif

/****************************************
* Function
****************************************/

float mupnp_random();

void mupnp_wait(mUpnpTime mtime);
void mupnp_waitrandom(mUpnpTime mtime);

#define mupnp_sleep(val) mupnp_wait(val)
#define mupnp_sleeprandom(val) mupnp_waitrandom(val)

mUpnpTime mupnp_getcurrentsystemtime();

#ifdef __cplusplus
}
#endif

#endif
