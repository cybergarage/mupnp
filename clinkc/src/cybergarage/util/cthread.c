/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cthread.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*	09/11/05
*		- Thanks for Visa Smolander <visa.smolander@nokia.com>
*		- Changed cg_thread_start() to set the flag at first.
*
******************************************************************/

#include <cybergarage/util/cthread.h>

/****************************************
* Thread Function
****************************************/

#if defined(WIN32) && !defined(ITRON)
static DWORD WINAPI Win32ThreadProc(LPVOID lpParam)
{
	CgThread *thread = (CgThread *)lpParam;
	if (thread->action != NULL)
		thread->action(thread);
	return 0;
}
#elif defined(BTRON)
static VOID BTronTaskProc(W param)
{
	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	ext_tsk();
}
#elif defined(ITRON)
static TASK ITronTaskProc(int param)
{
	T_RTSK rtsk;
	CgThread *thread;
	if (ref_tsk(TSK_SELF, &rtsk) != E_OK)
		return;
	thread = (CgThread *)rtsk.exinf;
	if (thread->action != NULL)
		thread->action(thread);
	exd_tsk();
}
#elif defined(TENGINE) && !defined(PROCESS_BASE)
static VOID TEngineTaskProc(INT stacd, VP param)
{
	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	tk_exd_tsk();
}
#elif defined(TENGINE) && defined(PROCESS_BASE)
static VOID TEngineProcessBasedTaskProc(W param)
{
	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	b_ext_tsk();
}
#else
static void *PosixThreadProc(void *param)
{
	CgThread *thread = (CgThread *)param;
	if (thread->action != NULL)
		thread->action(thread);
	return 0;
}
#endif

/****************************************
* cg_thread_new
****************************************/

CgThread *cg_thread_new()
{
	CgThread *thread = (CgThread *)malloc(sizeof(CgThread));
	thread->runnableFlag = FALSE;
	thread->action = NULL;
	thread->userData = NULL;
	return thread;
}

/****************************************
* cg_thread_delete
****************************************/

BOOL cg_thread_delete(CgThread *thread)
{
	cg_thread_stop(thread);
	free(thread);
	return TRUE;
}

/****************************************
* cg_thread_start
****************************************/

BOOL cg_thread_start(CgThread *thread)
{
	/**** Thanks for Visa Smolander (09/11/2005) ****/
	thread->runnableFlag = TRUE;
	
#if defined(WIN32) && !defined(ITRON)
	thread->hThread = CreateThread(NULL, 0, Win32ThreadProc, (LPVOID)thread, 0, &thread->threadID);
#elif defined(BTRON)
	P_STATE pstate;
	prc_sts(0, &pstate, NULL);
	thread->taskID = cre_tsk(BTronTaskProc, pstate.priority, (W)thread);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
#elif defined(ITRON)
	T_CTSK ctsk = {TA_HLNG,  (VP_INT)thread, ITronTaskProc, 6, 512, NULL, NULL};
	thread->taskID = acre_tsk(&ctsk);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
	if (sta_tsk(thread->taskID, 0) != E_OK) {
		thread->runnableFlag = FALSE;
		del_tsk(thread->taskID);
		return FALSE;
	}
#elif defined(TENGINE) && !defined(PROCESS_BASE)
	T_CTSK ctsk = {(VP)thread, TA_HLNG, TEngineTaskProc,10, 2048};
	thread->taskID = tk_cre_tsk(&ctsk);
	if (thread->taskID < E_OK) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
	if (tk_sta_tsk(thread->taskID, 0) < E_OK) {
		thread->runnableFlag = FALSE;
		tk_del_tsk(thread->taskID);
		return FALSE;
	}
#elif defined(TENGINE) && defined(PROCESS_BASE)
	P_STATE pstate;
	b_prc_sts(0, &pstate, NULL);
	thread->taskID = b_cre_tsk(TEngineProcessBasedTaskProc, pstate.priority, (W)thread);
	if (thread->taskID < 0) {
		thread->runnableFlag = FALSE;
		return FALSE;
	}
#else
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&thread->pThread, &thread_attr, PosixThreadProc, thread) != 0) {
		thread->runnableFlag = FALSE;
		pthread_attr_destroy(&thread_attr);
		return FALSE;
	}
	pthread_attr_destroy(&thread_attr);
#endif

	return TRUE;
}

/****************************************
* cg_thread_stop
****************************************/

BOOL cg_thread_stop(CgThread *thread)
{
	if (thread->runnableFlag == TRUE) {
		thread->runnableFlag = FALSE;
#if defined(WIN32) && !defined(ITRON)
		TerminateThread(thread->hThread, 0);
		WaitForSingleObject(thread->hThread, INFINITE);
#elif defined(BTRON)
		ter_tsk(thread->taskID);
#elif defined(ITRON)
		ter_tsk(thread->taskID);
		del_tsk(thread->taskID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
		tk_ter_tsk(thread->taskID);
		tk_del_tsk(thread->taskID);
#elif defined(TENGINE) && defined(PROCESS_BASE)
		b_ter_tsk(thread->taskID);
#else
		pthread_cancel(thread->pThread);
		pthread_detach(thread->pThread);
#endif
	}
	return TRUE;
}

/****************************************
* cg_thread_restart
****************************************/

BOOL cg_thread_restart(CgThread *thread)
{
	cg_thread_stop(thread);
	cg_thread_start(thread);
	return TRUE;
}

/****************************************
* cg_thread_isrunnable
****************************************/

BOOL cg_thread_isrunnable(CgThread *thread)
{
	return thread->runnableFlag;
}

/****************************************
* cg_thread_setaction
****************************************/

void cg_thread_setaction(CgThread *thread, CG_THREAD_FUNC func)
{
	thread->action = func;
}

/****************************************
* cg_thread_setuserdata
****************************************/

void cg_thread_setuserdata(CgThread *thread, void *value)
{
	thread->userData = value;
}

/****************************************
* cg_thread_getuserdata
****************************************/

void *cg_thread_getuserdata(CgThread *thread)
{
	return thread->userData;
}
