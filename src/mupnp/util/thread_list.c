/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cthread_list.c
*
*	Revision:
*
*	10/21/05
*		- first revision
*
******************************************************************/

#include <mupnp/util/thread.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_threadlist_new
****************************************/

CgThreadList *mupnp_threadlist_new()
{
	CgThreadList *threadList;

	mupnp_log_debug_l4("Entering...\n");

	threadList = (CgThreadList *)malloc(sizeof(CgThreadList));

	if ( NULL != threadList )
	{
		mupnp_list_header_init((CgList *)threadList);
		threadList->runnableFlag = FALSE;
		threadList->action = NULL;
		threadList->userData = NULL;
	}

	return threadList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_delete
****************************************/

void mupnp_threadlist_delete(CgThreadList *threadList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_threadlist_clear(threadList);
	free(threadList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_start
****************************************/

BOOL mupnp_threadlist_start(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	mupnp_log_debug_l4("Entering...\n");

	for (thread = mupnp_threadlist_gets(threadList); thread != NULL; thread = mupnp_thread_next(thread))
		mupnp_thread_start(thread);

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_stop
****************************************/

BOOL mupnp_threadlist_stop(CgThreadList *threadList)
{
	CgThreadList *thread;
	
	mupnp_log_debug_l4("Entering...\n");

	for (thread = mupnp_threadlist_gets(threadList); thread != NULL; thread = mupnp_thread_next(thread))
		mupnp_thread_stop(thread);

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}
