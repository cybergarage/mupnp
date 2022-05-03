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

#if !defined(BTRON) && !defined(ITRON)
#include <time.h>
#endif

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
typedef time_t mUpnpTime;
#else
typedef long mUpnpTime;
#endif

/****************************************
* FIXME: XCode 13 might not include <time.h> normally
****************************************/

#ifndef _TIME_H_
struct tm {
  int  tm_sec;    /* seconds after the minute [0-60] */
  int  tm_min;    /* minutes after the hour [0-59] */
  int  tm_hour;  /* hours since midnight [0-23] */
  int  tm_mday;  /* day of the month [1-31] */
  int  tm_mon;    /* months since January [0-11] */
  int  tm_year;  /* years since 1900 */
  int  tm_wday;  /* days since Sunday [0-6] */
  int  tm_yday;  /* days since January 1 [0-365] */
  int  tm_isdst;  /* Daylight Savings Time flag */
  long  tm_gmtoff;  /* offset from UTC in seconds */
  char  *tm_zone;  /* timezone abbreviation */
};
#endif

/****************************************
* Function
****************************************/

float mupnp_random(void);

void mupnp_wait(mUpnpTime mtime);
void mupnp_waitrandom(mUpnpTime mtime);

#define mupnp_sleep(val) mupnp_wait(val)
#define mupnp_sleeprandom(val) mupnp_waitrandom(val)

mUpnpTime mupnp_getcurrentsystemtime(void);

#ifdef __cplusplus
}
#endif

#endif
