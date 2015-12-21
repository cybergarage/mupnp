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

#if defined(WIN32)
#include <winbase.h>
#else
#include <sys/time.h>
#endif

/****************************************
* mupnp_cond_new
****************************************/

mUpnpCond* mupnp_cond_new()
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
#else
  struct timeval now;
  struct timespec timeout_s;

  mupnp_log_debug_l4("Entering...\n");

  gettimeofday(&now, NULL);

  if (timeout < 1) {
    pthread_cond_wait(&cond->condID, &mutex->mutexID);
  }
  else {
    timeout_s.tv_sec = now.tv_sec + timeout;
    timeout_s.tv_nsec = now.tv_usec * 1000;
    pthread_cond_timedwait(&cond->condID, &mutex->mutexID, &timeout_s);
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
