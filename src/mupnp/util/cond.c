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

#include <mupnp/util/cond.h>
#include <mupnp/util/log.h>
#if defined(ESP_PLATFORM)
#include <errno.h>
#include <esp_timer.h>
#include <mupnp/util/thread.h>
#include <stdint.h>
#endif

#if defined(WIN32)
#include <winbase.h>
#else
#include <sys/time.h>
#endif

/****************************************
 * mupnp_cond_new
 ****************************************/

mUpnpCond* mupnp_cond_new(void)
{
  mUpnpCond* cond;

  mupnp_log_debug_l4("Entering...\n");

  cond = (mUpnpCond*)malloc(sizeof(mUpnpCond));

  if (NULL != cond) {
#if defined(WIN32) && !defined(ITRON)
    cond->condID = CreateEvent(NULL, false, false, NULL);
#elif defined(BTRON)
/* TODO: Add implementation */
#elif defined(ITRON)
/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
/* TODO: Add implementation */
#else
    pthread_cond_init(&cond->condID, NULL);
#endif
  }

  return cond;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_cond_delete
 ****************************************/

bool mupnp_cond_delete(mUpnpCond* cond)
{
  mupnp_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
  CloseHandle(cond->condID);
#elif defined(BTRON)
/* TODO: Add implementation */
#elif defined(ITRON)
/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
/* TODO: Add implementation */
#else
  pthread_cond_destroy(&cond->condID);
#endif
  free(cond);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_cond_lock
 ****************************************/

bool mupnp_cond_wait(mUpnpCond* cond, mUpnpMutex* mutex, unsigned long timeout)
{
#if defined(WIN32) && !defined(ITRON)
  DWORD timeout_s = (timeout == 0 ? INFINITE : timeout);
  mupnp_mutex_unlock(mutex);
  WaitForSingleObject(cond->condID, timeout_s);
  mupnp_mutex_lock(mutex);
#elif defined(BTRON)
/* TODO: Add implementation */
#elif defined(ITRON)
/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
/* TODO: Add implementation */
#elif defined(ESP_PLATFORM)
  mUpnpThread* thread = mupnp_thread_self();
  int64_t started = esp_timer_get_time();
  int64_t duration = (int64_t)timeout * 1000000;

  /* stop_with_cond cannot lock the caller's mutex. Bounded waits close the
   * check-to-wait lost-wakeup window without changing the requested deadline. */
  while (thread == NULL || mupnp_thread_isrunnable(thread)) {
    int64_t remaining = duration - (esp_timer_get_time() - started);
    if (timeout != 0 && remaining <= 0)
      break;
    long slice = (timeout != 0 && remaining < 100000) ? (long)remaining : 100000;
    struct timeval now;
    struct timespec until;
    gettimeofday(&now, NULL);
    until.tv_sec = now.tv_sec;
    until.tv_nsec = now.tv_usec * 1000 + slice * 1000;
    if (until.tv_nsec >= 1000000000) {
      until.tv_sec++;
      until.tv_nsec -= 1000000000;
    }
    int result = pthread_cond_timedwait(&cond->condID, &mutex->mutexID, &until);
    if (result == 0)
      break;
    if (result != ETIMEDOUT)
      return false;
  }
#else
  struct timeval now;
  struct timespec timeoutS;

  mupnp_log_debug_l4("Entering...\n");

  gettimeofday(&now, NULL);

  if (timeout < 1) {
    pthread_cond_wait(&cond->condID, &mutex->mutexID);
  }
  else {
    timeoutS.tv_sec = now.tv_sec + timeout;
    timeoutS.tv_nsec = now.tv_usec * 1000;
    pthread_cond_timedwait(&cond->condID, &mutex->mutexID, &timeoutS);
  }
#endif
  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_cond_unlock
 ****************************************/

bool mupnp_cond_signal(mUpnpCond* cond)
{
  bool success = false;

  mupnp_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
  /* TODO: Add implementation */
  /* success = (SignalObjectAndWait(cond->condID, NULL, INFINITE, false) != WAIT_FAILED); */
  /* success = (WaitForSingleObject(cond->condID, INFINITE) != WAIT_FAILED); */
  success = SetEvent(cond->condID);
#elif defined(BTRON)
/* TODO: Add implementation */
#elif defined(ITRON)
/* TODO: Add implementation */
#elif defined(TENGINE) && !defined(PROCESS_BASE)
/* TODO: Add implementation */
#elif defined(TENGINE) && defined(PROCESS_BASE)
/* TODO: Add implementation */
#else
  success = (pthread_cond_signal(&cond->condID) == 0);
#endif
  mupnp_log_debug_l4("Leaving...\n");

  return success;
}
