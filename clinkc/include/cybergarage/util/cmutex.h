/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmutex.h
*
*	Revision:
*
*	01/17/05
*		- first revision
*
*	10/31/05
*		- Added comments to all functions and structs
*
******************************************************************/

#ifndef _CG_UTIL_CMUTEX_H_
#define _CG_UTIL_CMUTEX_H_

#include <cybergarage/typedef.h>

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

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
 * Data Types
 ****************************************/

/**
 * \brief The generic wrapper struct for CyberLinkC's mutexes.
 *
 * This wrapper has been created to enable 100% code
 * compatibility for different platforms (Linux, Win32 etc..)
 */
typedef struct _CgMutex {
#if defined(WIN32) && !defined(ITRON)
	HANDLE	mutexID;
#elif defined(BTRON)
	WERR	mutexID;
#elif defined(ITRON)
	ER_ID	mutexID;
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	ID mutexID;
#elif defined(TENGINE) && defined(PROCESS_BASE)
	WERR	mutexID;
#else
	/** The mutex entity */
	pthread_mutex_t mutexID;
#endif
} CgMutex;

/****************************************
 * Functions
 ****************************************/

/** 
 * Create a new mutex
 */
CgMutex *cg_mutex_new();

/** 
 * Destroy a mutex
 *
 * \param mutex The mutex to destroy
 */
BOOL cg_mutex_delete(CgMutex *mutex);

/** 
 * Acquire a mutex lock
 *
 * \param mutex Mutex to lock
 */
BOOL cg_mutex_lock(CgMutex *mutex);

/** 
 * Release a locked mutex
 *
 * \param mutex Mutex to unlock
 */
BOOL cg_mutex_unlock(CgMutex *mutex);

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif
