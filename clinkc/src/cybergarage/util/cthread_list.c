/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*	File: cthread_list.c
*
*	Revision:
*
*	10/21/05
*		- first revision
*
******************************************************************/

#include <cybergarage/util/cthread.h>

/****************************************
* cg_threadlist_new
****************************************/

CgThreadList *cg_threadlist_new()
{
	CgThreadList *threadList = (CgThreadList *)malloc(sizeof(CgThreadList));
	cg_list_header_init((CgList *)threadList);
	threadList->runnableFlag = FALSE;
	threadList->action = NULL;
	threadList->userData = NULL;
	return threadList;
}

/****************************************
* cg_threadlist_delete
****************************************/

void cg_threadlist_delete(CgThreadList *threadList)
{
	cg_threadlist_clear(threadList);
	free(threadList);
}

/****************************************
* cg_threadlist_start
****************************************/

BOOL cg_threadlist_start(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	for (thread = cg_threadlist_gets(threadList); thread != NULL; thread = cg_thread_next(thread))
		cg_thread_start(thread);

	return TRUE;
}

/****************************************
* cg_threadlist_stop
****************************************/

BOOL cg_threadlist_stop(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	for (thread = cg_threadlist_gets(threadList); thread != NULL; thread = cg_thread_next(thread))
		cg_thread_stop(thread);

	return TRUE;
}
