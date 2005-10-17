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

#include <cybergarage/typedef.h>

#if defined(BTRON)
#include <btron/proctask.h>
#include <btron/clk.h>
#elif defined(ITRON)
#include <kernel.h>
#else
#include <time.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#if defined(BTRON) || defined(ITRON) 
typedef long CgSysTime;
#else
typedef time_t CgSysTime;
#endif

/****************************************
* Function
****************************************/
void cg_wait(CgSysTime mtime);
void cg_waitrandom(CgSysTime mtime);
CgSysTime cg_getcurrentsystemtime();
float cg_random();

#ifdef  __cplusplus
}
#endif

#endif
