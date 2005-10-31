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
*	10/31/05
*		- Added comments to all functions and structs
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
#include <signal.h>
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
	/** Indicates whether the thread is running or not */
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

  /** The POSIX thread handle */
  pthread_t pThread;

#endif

  /** Thread's worker function */
  void (*action)(struct _CgThread *);

  /** Arbitrary user data pointer */
  void *userData;

} CgThread, CgThreadList;

/**
 * Prototype for the threads' worker functions 
 */
typedef void (*CG_THREAD_FUNC)(CgThread *);

/****************************************
* Function
****************************************/

/**
 * Create a new thread
 */
CgThread *cg_thread_new();

/**
 * Stop and destroy a thread.
 *
 * \param thread Thread to destroy
 */
BOOL cg_thread_delete(CgThread *thread);

/**
 * Start a thread (must be created first with ch_thread_new())
 *
 * \param thread Thread to start
 */
BOOL cg_thread_start(CgThread *thread);

/**
 * Stop a running thread.
 *
 * \param thread Thread to stop
 */
BOOL cg_thread_stop(CgThread *thread);

/**
 * Restart a thread. Essentially calls cg_thread_stop() and cg_thread_start()
 *
 * \param thread Thread to restart
 */
BOOL cg_thread_restart(CgThread *thread);

/**
 * Check if a thread has been started
 *
 * \param thread Thread to check
 */
BOOL cg_thread_isrunnable(CgThread *thread);

/**
 * Set the thread's worker function.
 *
 * \param thread Thread struct
 * \param actionFunc Function pointer to set as the worker function
 */
void cg_thread_setaction(CgThread *thread, CG_THREAD_FUNC actionFunc);

/**
 * Set the user data pointer
 *
 * \param thread Thread struct
 * \param data Pointer to user data
 */
void cg_thread_setuserdata(CgThread *thread, void *data);

/**
 * Get the user data pointer
 *
 * \param thread Thread from which to get the pointer
 */
void *cg_thread_getuserdata(CgThread *thread);

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif
