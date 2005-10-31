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
