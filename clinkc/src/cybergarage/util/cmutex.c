/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmutex.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cmutex.h>

/****************************************
* cg_mutex_new
****************************************/

CgMutex *cg_mutex_new()
{
	CgMutex *mutex = (CgMutex *)malloc(sizeof(CgMutex));
#if defined(WIN32) && !defined(ITRON)
	mutex->mutexID = CreateMutex(NULL, FALSE, NULL);
#elif defined(BTRON)
	mutex->mutexID = cre_sem(1, SEM_EXCL);
#elif defined(ITRON) 
	T_CSEM	csem;
	csem.sematr = TA_TFIFO;
	csem.isemcnt = 1;
	csem.maxsem = 1;
	csem.name = NULL;
	mutex->mutexID = acre_sem(&csem);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	T_CSEM	csem;
	csem.exinf = 0;
	csem.sematr = TA_TFIFO | TA_FIRST;
	csem.isemcnt = 0;
	csem.maxsem = 1;
	mutex->mutexID = tk_cre_sem(&csem);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	mutex->mutexID = b_cre_sem(1, SEM_EXCL);
#else
	pthread_mutex_init(&mutex->mutexID, NULL);
#endif
	return mutex;
}

/****************************************
* cg_mutex_delete
****************************************/

BOOL cg_mutex_delete(CgMutex *mutex)
{
#if defined(WIN32) && !defined(ITRON)
	CloseHandle(mutex->mutexID);
#elif defined(BTRON)
	del_sem(mutex->mutexID);
#elif defined(ITRON)
	del_sem(mutex->mutexID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_del_sem(mutex->mutexID);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_del_sem(mutex->mutexID);
#else
	pthread_mutex_destroy(&mutex->mutexID);
#endif
	free(mutex);
	return TRUE;
}

/****************************************
* cg_mutex_lock
****************************************/

BOOL cg_mutex_lock(CgMutex *mutex)
{
#if defined(WIN32) && !defined(ITRON)
	WaitForSingleObject(mutex->mutexID, INFINITE);
#elif defined(BTRON)
	wai_sem(mutex->mutexID, T_FOREVER);
#elif defined(ITRON)
	twai_sem(mutex->mutexID, TMO_FEVR);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_wai_sem(mutex->mutexID, 1, TMO_FEVR);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_wai_sem(mutex->mutexID, T_FOREVER);
#else
	pthread_mutex_lock(&mutex->mutexID);
#endif
	return TRUE;
}

/****************************************
* cg_mutex_unlock
****************************************/

BOOL cg_mutex_unlock(CgMutex *mutex)
{
#if defined(WIN32) && !defined(ITRON)
	ReleaseMutex(mutex->mutexID);
#elif defined(BTRON)
	sig_sem(mutex->mutexID);
#elif defined(ITRON)
	sig_sem(mutex->mutexID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	tk_sig_sem(mutex->mutexID, 1);
#elif defined(TENGINE) && defined(PROCESS_BASE)
	b_sig_sem(mutex->mutexID);
#else
	pthread_mutex_unlock(&mutex->mutexID);
#endif
	return TRUE;
}

