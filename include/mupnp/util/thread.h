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

#ifndef _MUPNP_UTIL_THREAD_H_
#define _MUPNP_UTIL_THREAD_H_

#include <mupnp/typedef.h>
#include <mupnp/util/list.h>
#include <mupnp/util/cond.h>

#include <mupnp/util/time.h>
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

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
 * Define
 ****************************************/

#if defined(WINCE)
#define MUPNP_THREAD_SHUTDOWN_ATTEMPTS 10
#define MUPNP_THREAD_MIN_SLEEP 1000
/* ADD Fabrice Fontaine Orange 24/04/2007 */
/* Bug correction : Variable used to wait for thread termination by sleeping */
/* instead of joining */
#else
#define MUPNP_THREAD_MIN_SLEEP 1000
/* ADD END Fabrice Fontaine Orange 24/04/2007 */
#endif

/****************************************
 * Data Type
 ****************************************/

/**
 * \brief The generic wrapper struct for mUPnP's threads.
 *
 * This wrapper has been created to enable 100% code
 * compatibility between different platforms (Linux, Win32 etc..)
 */
typedef struct _mUpnpThread {
  MUPNP_LIST_STRUCT_MEMBERS

  /** Indicates whether this thread is ready to run */
  bool runnableFlag;
#if defined WINCE
  /** serves as look ahead to have the thread manage its own delete(thread) on exit */
  bool isRunning;
  bool deletePending;
  mUpnpTime sleep;
#endif //WINCE

#if defined DEBUG
  char friendlyName[32];
#endif

#if defined(WIN32) && !defined(ITRON)
  HANDLE hThread;
  DWORD threadID;
#elif defined(BTRON)
  W taskID;
#elif defined(ITRON)
  ER_ID taskID;
#elif defined(TENGINE) && !defined(PROCESS_BASE)
  ID taskID;
#elif defined(TENGINE) && defined(PROCESS_BASE)
  WERR taskID;
#else

  /** The POSIX thread handle */
  pthread_t pThread;

#endif

  /** Thread's worker function */
  void (*action)(struct _mUpnpThread*);

  /** Arbitrary data pointer */
  void* userData;
} mUpnpThread, mUpnpThreadList;

/**
 * Prototype for the threads' worker functions 
 */
typedef void (*MUPNP_THREAD_FUNC)(mUpnpThread*);

/****************************************
* Function
****************************************/

/**
 * Create a new thread
 */
mUpnpThread* mupnp_thread_new();

/**
 * Get a self reference to thread.
 */

mUpnpThread* mupnp_thread_self();

/**
 * Stop and destroy a thread.
 *
 * \param thread Thread to destroy
 */
bool mupnp_thread_delete(mUpnpThread* thread);

/**
 * Start a thread (must be created first with ch_thread_new())
 *
 * \param thread Thread to start
 */
bool mupnp_thread_start(mUpnpThread* thread);

/**
 * Stop a running thread.
 *
 * \param thread Thread to stop
 */
bool mupnp_thread_stop(mUpnpThread* thread);

/**
 * Stop the running thread and signal the given CGCond.
 */
bool mupnp_thread_stop_with_cond(mUpnpThread* thread, mUpnpCond* cond);

/**
 * Restart a thread. Essentially calls mupnp_thread_stop() and mupnp_thread_start()
 *
 * \param thread Thread to restart
 */
bool mupnp_thread_restart(mUpnpThread* thread);

/**
 * Check if a thread has been started
 *
 * \param thread Thread to check
 */
bool mupnp_thread_isrunnable(mUpnpThread* thread);

/**
 * Set the thread's worker function.
 *
 * \param thread Thread struct
 * \param actionFunc Function pointer to set as the worker function
 */
void mupnp_thread_setaction(mUpnpThread* thread, MUPNP_THREAD_FUNC actionFunc);

/**
 * Set the user data pointer
 *
 * \param thread Thread struct
 * \param data Pointer to user data
 */
void mupnp_thread_setuserdata(mUpnpThread* thread, void* data);

/**
 * Get the user data pointer
 *
 * \param thread Thread from which to get the pointer
 */
void* mupnp_thread_getuserdata(mUpnpThread* thread);

#if defined(WINCE)
void mupnp_thread_sleep(mUpnpThread* thread);
void mupnp_thread_exit(DWORD exitCode);
#if defined DEBUG_MEM
void mupnp_thread_monitor(mUpnpThread* thread);
#endif //DEBUG_MEM
#endif //WIN32_WCE

#define mupnp_thread_next(thread) (mUpnpThread*) mupnp_list_next((mUpnpList*)thread)
#define mupnp_thread_remove(thread) mupnp_list_remove((mUpnpList*)thread)

/****************************************
* Function (Thread List)
****************************************/

/**
 * Create a new thread list
 *
 * \return Thread list
 */
mUpnpThreadList* mupnp_threadlist_new();

/**
 * Destroy a thread list
 *
 * \param threadList The thread list in question
 */
void mupnp_threadlist_delete(mUpnpThreadList* threadList);

/**
 * Clear the contents of a thread list
 *
 * \param threadList Thread list in question
 */
#define mupnp_threadlist_clear(threadList) mupnp_list_clear((mUpnpList*)threadList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_thread_delete)

/**
 * Get the size of a thread list
 *
 * \param threadList The thread list in question
 */
#define mupnp_threadlist_size(threadList) mupnp_list_size((mUpnpList*)threadList)

/**
 * Get the first actual item from a thread list to use as an iterator
 *
 * \param threadList The thread list in question
 */
#define mupnp_threadlist_gets(threadList) (mUpnpThread*) mupnp_list_next((mUpnpList*)threadList)

/**
 * Add a thread into a thread list
 *
 * \param threadList The thread list in question
 * \param thread The thread to add to the list
 */
#define mupnp_threadlist_add(threadList, thread) mupnp_list_add((mUpnpList*)threadList, (mUpnpList*)thread)

/**
 * Remove a thread from thread list
 *
 * \param threadList The thread list in question
 * \param thread The thread to be removed 
 */
#define mupnp_threadlist_remove(thread) mupnp_list_remove((mUpnpList*)thread)

/**

 * Start all threads in the thread list
 *
 * \param threadList The thread list in question
 */
bool mupnp_threadlist_start(mUpnpThreadList* threadList);

/**
 * Stop all threads in the thread list
 *
 * \param threadList The thread list in question
 */
bool mupnp_threadlist_stop(mUpnpThreadList* threadList);

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
