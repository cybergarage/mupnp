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
#include <mupnp/util/cond.h>
#include <mupnp/util/list.h>

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
#endif // WINCE

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
 * @brief Create a new thread instance
 *
 * @details
 * Allocates and initializes a platform-independent thread object.
 * The thread is initially in a stopped state (not running).
 *
 * Before starting the thread, you must:
 * 1. Set an action function with mupnp_thread_setaction()
 * 2. Optionally set user data with mupnp_thread_setuserdata()
 *
 * Platform support:
 * - Linux/Unix: pthread (POSIX threads)
 * - Windows: CreateThread API
 * - ITRON, T-Engine, BTRON: platform-specific task APIs
 *
 * @return A newly-created mUpnpThread on success, or NULL if thread
 *         creation fails (e.g., insufficient system resources).
 *
 * @note The returned thread must be freed with mupnp_thread_delete() when
 *       no longer needed.
 * @note Thread-safe: Can be called concurrently from multiple threads.
 * @note The thread does not start automatically; call mupnp_thread_start()
 *       to begin execution.
 *
 * @see mupnp_thread_delete()
 * @see mupnp_thread_start()
 * @see mupnp_thread_setaction()
 *
 * @code
 * // Example: Create and start a worker thread
 * void worker_function(mUpnpThread* thread) {
 *     while (mupnp_thread_isrunnable(thread)) {
 *         // Perform work...
 *         mupnp_sleep(100);
 *     }
 * }
 * 
 * mUpnpThread* thread = mupnp_thread_new();
 * mupnp_thread_setaction(thread, worker_function);
 * mupnp_thread_start(thread);
 * @endcode
 */
mUpnpThread* mupnp_thread_new(void);

/**
 * @brief Get the mUpnpThread object for the currently executing thread
 *
 * @details
 * Returns a reference to the thread structure for the calling thread.
 * This is useful when a thread needs to access its own thread data or
 * check if it should continue running.
 *
 * @return Pointer to the current thread's mUpnpThread structure, or NULL
 *         if the calling thread was not created with mupnp_thread_new().
 *
 * @note Thread-safe: Can be called from any thread.
 * @note This function only works for threads created by the mUPnP library;
 *       external threads (e.g., main thread, OS threads) will return NULL.
 *
 * @see mupnp_thread_isrunnable()
 */

mUpnpThread* mupnp_thread_self(void);

/**
 * @brief Stop and destroy a thread, freeing all resources
 *
 * @details
 * Stops the thread if it is running and releases all associated resources.
 * This function blocks until the thread has fully terminated.
 *
 * The thread's action function should periodically check
 * mupnp_thread_isrunnable() and exit when it returns false to ensure
 * graceful shutdown.
 *
 * @param thread The thread to destroy. May be NULL (no-op if NULL).
 *
 * @retval true  Successfully stopped and destroyed the thread
 * @retval false Failed to destroy (e.g., thread is NULL)
 *
 * @note After calling this function, the thread pointer is invalid.
 * @note Blocking behavior: This function blocks until the thread terminates.
 *       If the thread does not check mupnp_thread_isrunnable(), it may
 *       block indefinitely.
 * @note Thread-safe: Must not be called concurrently on the same thread.
 *
 * @warning Do not call this function from the thread's own action function;
 *          this will cause deadlock.
 *
 * @see mupnp_thread_new()
 * @see mupnp_thread_stop()
 */
bool mupnp_thread_delete(mUpnpThread* thread);

/**
 * @brief Start a thread's execution
 *
 * @details
 * Starts the thread, causing its action function to begin execution in
 * a new thread of control. The thread must have been created with
 * mupnp_thread_new() and must have an action function set with
 * mupnp_thread_setaction().
 *
 * After starting, the action function runs concurrently with the calling
 * thread until the thread is stopped or the action function returns.
 *
 * @param thread The thread to start. Must not be NULL and must have
 *               an action function set.
 *
 * @retval true  Successfully started the thread
 * @retval false Failed to start due to:
 *               - thread is NULL
 *               - No action function set
 *               - Thread already running
 *               - System error (e.g., cannot create OS thread)
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Calling this function on an already-running thread has no effect
 *       and returns false.
 * @note Side effect: Sets the thread's runnable flag to true and creates
 *       a new OS-level thread.
 *
 * @see mupnp_thread_stop()
 * @see mupnp_thread_isrunnable()
 * @see mupnp_thread_setaction()
 */
bool mupnp_thread_start(mUpnpThread* thread);

/**
 * @brief Request a thread to stop execution
 *
 * @details
 * Signals the thread to stop by setting its runnable flag to false.
 * This function does NOT forcibly terminate the thread; the thread's
 * action function must cooperatively check mupnp_thread_isrunnable()
 * and exit when it returns false.
 *
 * This function returns immediately without waiting for the thread to
 * terminate. Use mupnp_thread_delete() if you need to wait for termination.
 *
 * @param thread The thread to stop. Must not be NULL.
 *
 * @retval true  Successfully signaled the thread to stop
 * @retval false Failed to signal (e.g., thread is NULL)
 *
 * @note Non-blocking: Returns immediately without waiting for thread exit.
 * @note Thread-safe: Can be called from any thread, including the thread
 *       being stopped.
 * @note Side effect: Sets the thread's runnable flag to false.
 * @note The thread continues running until its action function checks
 *       mupnp_thread_isrunnable() and exits.
 *
 * @see mupnp_thread_start()
 * @see mupnp_thread_isrunnable()
 * @see mupnp_thread_delete()
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
#endif // DEBUG_MEM
#endif // WIN32_WCE

#define mupnp_thread_next(thread) (mUpnpThread*)mupnp_list_next((mUpnpList*)thread)
#define mupnp_thread_remove(thread) mupnp_list_remove((mUpnpList*)thread)

/****************************************
 * Function (Thread List)
 ****************************************/

/**
 * Create a new thread list
 *
 * \return Thread list
 */
mUpnpThreadList* mupnp_threadlist_new(void);

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
#define mupnp_threadlist_gets(threadList) (mUpnpThread*)mupnp_list_next((mUpnpList*)threadList)

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
