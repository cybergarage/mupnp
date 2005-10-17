/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cthread.h
*
*	Revision:
*
*	02/07/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UTIL_CTHREAD_H_
#define _CG_UTIL_CTHREAD_H_

#include <cybergarage/typedef.h>

#if defined(WIN32) && !defined(ITRON)
#include <windows.h>
#elif defined(BTRON)
#include <btron/proctask.h>
#elif defined(ITRON)
#include <kernel.h>
#elif defined(TENGINE) && !defined(PROCESS_BASE)
#include <tk/tkernel.h>
#elif defined(TENGINE) && defined(PROCESS_BASE)
#include <btron/proctask.h>
#else
#include <pthread.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
* Data Type
****************************************/

typedef struct _CgThread {
	BOOL runnableFlag;
#if defined(WIN32) && !defined(ITRON)
	HANDLE	hThread;
	DWORD	threadID;
#elif defined(BTRON)
	W taskID;
#elif defined(ITRON)
	ER_ID	taskID;
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	ID taskID;
#elif defined(TENGINE) && defined(PROCESS_BASE)
	WERR taskID;
#else
	pthread_t pThread;
#endif
	void (*action)(struct _CgThread *);
	void *userData;
} CgThread;

typedef void (*CG_THREAD_FUNC)(CgThread *);

/****************************************
* Function
****************************************/

CgThread *cg_thread_new();
BOOL cg_thread_delete(CgThread *thread);
BOOL cg_thread_start(CgThread *thread);
BOOL cg_thread_stop(CgThread *thread);
BOOL cg_thread_restart(CgThread *thread);
BOOL cg_thread_isrunnable(CgThread *thread);
void cg_thread_setaction(CgThread *thread, CG_THREAD_FUNC actionFunc);
void cg_thread_setuserdata(CgThread *thread, void *data);
void *cg_thread_getuserdata(CgThread *thread);

#ifdef  __cplusplus
}
#endif

#endif
