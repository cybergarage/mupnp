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

#ifndef _MUPNP_UTIL_MUTEX_H_
#define _MUPNP_UTIL_MUTEX_H_

#include <mupnp/typedef.h>

#if defined(WIN32) && !defined(ITRON)
#include <winsock2.h>
#elif defined(BTRON)
#include <btron/taskcomm.h>
#elif defined(ITRON)
#include <kernel.h>
#elif defined(TENGINE) && !defined(PROCESS_BASE)
#include <tk/tkernel.h>
#elif defined(TENGINE) && defined(PROCESS_BASE)
#include <btron/taskcomm.h>
#elif defined(ESP32) || defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#else
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
 * Data Types
 ****************************************/

/**
 * \brief The generic wrapper struct for mUPnP's mutexes.
 *
 * This wrapper has been created to enable 100% code
 * compatibility for different platforms (Linux, Win32 etc..)
 */
typedef struct _mUpnpMutex {
#if defined(WIN32) && !defined(ITRON)
  HANDLE mutexID;
#elif defined(BTRON)
  WERR mutexID;
#elif defined(ITRON)
  ER_ID mutexID;
#elif defined(TENGINE) && !defined(PROCESS_BASE)
  ID mutexID;
#elif defined(TENGINE) && defined(PROCESS_BASE)
  WERR mutexID;
#elif defined(ESP32) || defined(ESP_PLATFORM)
  /** FreeRTOS semaphore for ESP32 */
  SemaphoreHandle_t mutexID;
#else
  /** The mutex entity */
  pthread_mutex_t mutexID;
#endif
} mUpnpMutex;

#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#include <stdlib.h>

typedef struct _mUpnpLockInfo {
  struct _mUpnpLockInfo* next;
  pthread_t thread_id;
  char *file, *function;
  int line, mutex_id;
} mUpnpLockInfo;
#endif

/****************************************
 * Functions
 ****************************************/

/**
 * @brief Create a new mutex for synchronization
 *
 * @details
 * Allocates and initializes a platform-independent mutex object.
 * Mutexes are used to protect shared resources and ensure thread-safe
 * access to data structures across the mUPnP library.
 *
 * The mutex is initially unlocked and ready to use.
 *
 * Platform support:
 * - Linux/Unix: pthread_mutex_t
 * - Windows: HANDLE (CreateMutex)
 * - ITRON, T-Engine, BTRON: platform-specific mutex primitives
 *
 * @return A newly-created mUpnpMutex on success, or NULL if mutex
 *         creation fails (e.g., insufficient system resources).
 *
 * @note The returned mutex must be freed with mupnp_mutex_delete() when
 *       no longer needed to avoid resource leaks.
 * @note Thread-safe: Can be called concurrently from multiple threads.
 * @note On some platforms, there may be system limits on the number of
 *       mutexes that can be created.
 *
 * @see mupnp_mutex_delete()
 * @see mupnp_mutex_lock()
 * @see mupnp_mutex_unlock()
 *
 * @code
 * // Example: Protect a shared counter
 * static int shared_counter = 0;
 * static mUpnpMutex* counter_mutex = NULL;
 * 
 * void init() {
 *     counter_mutex = mupnp_mutex_new();
 * }
 * 
 * void increment_counter() {
 *     mupnp_mutex_lock(counter_mutex);
 *     shared_counter++;
 *     mupnp_mutex_unlock(counter_mutex);
 * }
 * 
 * void cleanup() {
 *     mupnp_mutex_delete(counter_mutex);
 * }
 * @endcode
 */
mUpnpMutex* mupnp_mutex_new(void);

/**
 * @brief Destroy a mutex and free resources
 *
 * @details
 * Releases all resources associated with the mutex. The mutex must be
 * unlocked before calling this function.
 *
 * @param mutex The mutex to destroy. May be NULL (no-op if NULL).
 *
 * @retval true  Successfully destroyed the mutex
 * @retval false Failed to destroy (e.g., mutex is NULL or still locked)
 *
 * @note After calling this function, the mutex pointer is invalid.
 * @note Thread-safe: Must not be called concurrently on the same mutex.
 * @note Must not be called while the mutex is locked; behavior is undefined
 *       and may cause deadlocks or resource leaks.
 *
 * @warning Destroying a locked mutex may cause undefined behavior on some
 *          platforms.
 *
 * @see mupnp_mutex_new()
 */
bool mupnp_mutex_delete(mUpnpMutex* mutex);

/**
 * @brief Acquire a mutex lock (blocking)
 *
 * @details
 * Attempts to lock the mutex. If the mutex is already locked by another
 * thread, this function blocks (waits) until the mutex becomes available.
 *
 * After successfully locking, the calling thread has exclusive access to
 * the protected resource until mupnp_mutex_unlock() is called.
 *
 * Mutexes are not recursive on all platforms. Attempting to lock the same
 * mutex twice from the same thread may cause deadlock.
 *
 * When WITH_THREAD_LOCK_TRACE is defined, this is replaced with a macro
 * that enables lock tracing for debugging deadlocks and lock contention.
 *
 * @param mutex The mutex to lock. Must not be NULL.
 *
 * @retval true  Successfully acquired the lock
 * @retval false Failed to acquire lock (mutex is NULL or system error)
 *
 * @note Blocking behavior: This function will not return until the lock
 *       is acquired or an error occurs.
 * @note Thread-safe: Can be called from any thread.
 * @note Always pair with mupnp_mutex_unlock() and unlock as soon as possible
 *       to minimize lock contention.
 * @note On most platforms, mutexes are not recursive. Do not attempt to
 *       lock the same mutex twice from the same thread.
 *
 * @warning Failing to unlock a locked mutex will cause other threads to
 *          block indefinitely (deadlock).
 * @warning Do not call this function from signal handlers or with
 *          interrupts disabled.
 *
 * @see mupnp_mutex_unlock()
 * @see mupnp_mutex_new()
 */

#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#define mupnp_mutex_lock(mutex) mupnp_mutex_lock_trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, mutex)
#else
bool mupnp_mutex_lock(mUpnpMutex* mutex);
#endif

/**
 * @brief Release a previously acquired mutex lock
 *
 * @details
 * Unlocks a mutex that was previously locked with mupnp_mutex_lock().
 * This allows other threads waiting on the mutex to acquire the lock.
 *
 * The mutex must be locked by the calling thread before calling this
 * function. Unlocking a mutex that is not locked or is locked by a
 * different thread results in undefined behavior.
 *
 * When WITH_THREAD_LOCK_TRACE is defined, this is replaced with a macro
 * that enables lock tracing for debugging.
 *
 * @param mutex The mutex to unlock. Must not be NULL and must be locked
 *              by the calling thread.
 *
 * @retval true  Successfully released the lock
 * @retval false Failed to unlock (mutex is NULL, not locked, or system error)
 *
 * @note Thread-safe: Can be called from any thread, but must be the thread
 *       that acquired the lock.
 * @note Always call this function after mupnp_mutex_lock() as soon as the
 *       protected operation is complete.
 * @note Unlock must be called from the same thread that acquired the lock.
 *
 * @warning Unlocking a mutex from a different thread than the one that
 *          locked it results in undefined behavior.
 * @warning Unlocking an already unlocked mutex results in undefined behavior.
 *
 * @see mupnp_mutex_lock()
 * @see mupnp_mutex_new()
 */
#if defined(WITH_THREAD_LOCK_TRACE) && defined(__USE_ISOC99)
#define mupnp_mutex_unlock(mutex) mupnp_mutex_unlock_trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, mutex)
#else
bool mupnp_mutex_unlock(mUpnpMutex* mutex);
#endif

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
