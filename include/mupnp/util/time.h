/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ctime.h
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UTIL_CTIME_H_
#define _CG_UTIL_CTIME_H_

#include <mupnp/typedef.h>

#if defined(BTRON)
#include <btron/proctask.h>
#include <btron/clk.h>
#elif defined(ITRON)
#include <kernel.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#if defined(BTRON) || defined(ITRON) 
typedef size_t CgTime;
#else
typedef long CgTime;
#endif

/****************************************
* Function
****************************************/

float mupnp_random();

void mupnp_wait(CgTime mtime);
void mupnp_waitrandom(CgTime mtime);

#define mupnp_sleep(val) mupnp_wait(val)
#define mupnp_sleeprandom(val) mupnp_waitrandom(val)

CgTime mupnp_getcurrentsystemtime();

#ifdef  __cplusplus
}
#endif

#endif
