/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ctime.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/ctime.h>

#if defined(WIN32) && !defined(ITRON)
#include <windows.h>
#include <time.h>
#elif defined(BTRON) 
#include <btron/proctask.h>
#include <btron/clk.h>
#elif defined(ITRON)
#include <kernel.h>
#elif defined(TENGINE) && !defined(PROCESS_BASE)
#include <tk/tkernel.h>
#include <time.h>
#elif defined(TENGINE) && defined(PROCESS_BASE)
#include <tk/tkernel.h>
#include <btron/proctask.h>
#include <time.h>
#else
#include <unistd.h>
#include <time.h>
#endif

/****************************************
* cg_time_wait
****************************************/

void cg_wait(CgSysTime mtime)
{
#if defined(WIN32) && !defined(ITRON)
	Sleep(mtime);
#elif defined(BTRON)
	slp_tsk(mtime);
#elif defined(ITRON)
	tslp_tsk(mtime);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_slp_tsk(mtime);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_slp_tsk(mtime);
#else
	usleep(mtime*1000);
#endif
}

/****************************************
* cg_time_wait
****************************************/

void cg_waitrandom(CgSysTime mtime)
{
	double factor = (double)rand() / (double)RAND_MAX;
	long waitTime = (long)((double)mtime * factor);
	cg_wait(waitTime);
}

/****************************************
* cg_time_wait
****************************************/

CgSysTime cg_getcurrentsystemtime()
{
#if defined(BTRON)
	STIME CgSysTime;
	TIMEZONE tz;
	STIME localtime;
	if (get_tim(&CgSysTime, &tz) != 0)
		return 0;
	localtime = CgSysTime - tz.adjust + (tz.dst_flg ? (tz.dst_adj*60): 0);
	return localtime;
#elif defined(ITRON)
	static BOOL initialized = FALSE;
	SYSTIM sysTim;
	if (initialized == FALSE) {
		sysTim.utime = 0;
		sysTim.ltime = 0;
		set_tim(&sysTim);
	}
	get_tim(&sysTim);
	return ((sysTim.utime / 1000) << 32) + (sysTim.ltime / 1000);
#else
	return time(NULL);
#endif
}

/****************************************
* cg_random
****************************************/

float cg_random()
{
	static BOOL seedDone = FALSE;
	if (seedDone == FALSE) {
		srand(cg_getcurrentsystemtime());
		seedDone = TRUE;
	}
	return (float)rand() / (float)RAND_MAX;
}
