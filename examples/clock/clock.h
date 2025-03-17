/************************************************************
 *
 *	mUPnP for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 *	File: clock.h
 *
 *	Revision:
 *       06/07/05
 *               - first release.
 *
 ************************************************************/

#ifndef _CG_CLOCK_H_
#define _CG_CLOCK_H_

#include <mupnp/util/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
 * Define
 ****************************************/

#define SYSTEM_TIME_BUF_LEN 128

/****************************************
 * Function
 ****************************************/

char* get_system_time_string(mUpnpTime currTime, char* buf);
char* get_date_string(mUpnpTime currTime, char* buf);
char* get_time_string(mUpnpTime currTime, char* buf);
int get_second(mUpnpTime currTime);

#ifdef __cplusplus
}
#endif

#endif
