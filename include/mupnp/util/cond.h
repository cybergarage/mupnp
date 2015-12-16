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

#ifndef _MUPNP_UTIL_COND_H_
#define _MUPNP_UTIL_COND_H_

#include <mupnp/typedef.h>
#include <mupnp/util/mutex.h>

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
 * \brief The generic wrapper struct for mUPnP's conds.
 *
 * This wrapper has been created to enable 100% code
 * compatibility for different platforms (Linux, Win32 etc..)
 */
typedef struct _mUpnpCond {
#if defined(WIN32) && !defined(ITRON)
  HANDLE condID;
#elif defined(BTRON)
  WERR condID;
#elif defined(ITRON)
  ER_ID condID;
#elif defined(TENGINE) && !defined(PROCESS_BASE)
  ID condID;
#elif defined(TENGINE) && defined(PROCESS_BASE)
  WERR condID;
#else
  /** The cond entity */
  pthread_cond_t condID;
#endif
} mUpnpCond;

/****************************************
 * Functions
 ****************************************/

/** 
 * Create a new condition variable
 */
mUpnpCond* mupnp_cond_new();

/** 
 * Destroy a condition variable
 *
 * \param cond The cond to destroy
 */
bool mupnp_cond_delete(mUpnpCond* cond);

/** 
 * Wait for condition variable to be signalled.
 *
 * \param cond Cond to be waited
 * \param mutex Mutex used for synchronization
 * \param timeout Maximum time in seconds to wait, 0 to wait forever
 */
bool mupnp_cond_wait(mUpnpCond* cond, mUpnpMutex* mutex, unsigned long timeout);

/** 
 * Signal a condition variable
 *
 * \param cond Cond to be signalled
 */
bool mupnp_cond_signal(mUpnpCond* cond);

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
