/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csocket.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*	10/26/05
*		- Added changes when the parametes are null.
*
*	03/19/06 Theo Beisch
*		- Added NULL check for CG_LIST_DESTRUCTORFUNC
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/util/clog.h>

/****************************************
* cg_list_header_init
****************************************/

void cg_list_header_init(CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return;

	list->headFlag = TRUE;			
	list->prev = list->next = list;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_node_init
****************************************/

void cg_list_node_init(CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return;

	list->headFlag = FALSE;			
	list->prev = list->next = list;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_size
****************************************/

int cg_list_size(CgList *headList)
{
	CgList *list;
	int listCnt;
	
	cg_log_debug_l5("Entering...\n");

	if (headList == NULL)
		return 0;

	listCnt = 0;
	for (list = cg_list_next(headList); list != NULL; list = cg_list_next(list))
		listCnt++;
	
	return listCnt;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_get
****************************************/

CgList *cg_list_get(CgList *headList, int index)
{
	CgList *list;
	int n;
	
	cg_log_debug_l5("Entering...\n");

	if (headList == NULL)
		return NULL;

	list = cg_list_next(headList);
	for (n=0; n<index; n++) {
		if (list == NULL)
			break;
		list = cg_list_next(list);
	}
		
	return list;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_insert
****************************************/

void cg_list_insert(
CgList *prevList,
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (prevList == NULL || list == NULL)
		return;

	list->prev = prevList;
	list->next = prevList->next;
	prevList->next->prev = list;
	prevList->next = list;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_add
****************************************/

void cg_list_add(
CgList *headList,
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (headList == NULL || list == NULL)
		return;

	cg_list_insert(headList->prev, list);

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_remove
****************************************/

void cg_list_remove(CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return;

	list->prev->next = list->next;
	list->next->prev = list->prev;
	list->prev = list->next = list;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_prev_circular
****************************************/

CgList *cg_list_prev_circular (
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return NULL;

	if (list->prev->headFlag)
		list = list->prev;
	return list->prev;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_prev
****************************************/

CgList *cg_list_prev(
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return NULL;

	if (list->prev->headFlag == TRUE)
		return NULL;
	return list->prev;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_next_circular
****************************************/

CgList *cg_list_next_circular(
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return NULL;

	if (list->next->headFlag == TRUE)
		list = list->next;
	return list->next;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_next
****************************************/

CgList *cg_list_next(
CgList *list)
{
	cg_log_debug_l5("Entering...\n");

	if (list == NULL)
		return NULL;

	if (list->next->headFlag == TRUE)
		return NULL;
	return list->next;

	cg_log_debug_l5("Leaving...\n");
}

/****************************************
* cg_list_clear
****************************************/

void cg_list_clear(CgList *headList, CG_LIST_DESTRUCTORFUNC destructorFunc)
{
	CgList *list;

	cg_log_debug_l5("Entering...\n");

	if (headList == NULL)
		return;

	list = cg_list_next(headList);
	while(list != NULL) {
		cg_list_remove(list);
		//Theo Beisch: use destructorFunc or just free(listElement)
		if (destructorFunc != NULL){
			destructorFunc(list);
		} else {
			free(list);
		}
		list = cg_list_next(headList);
	}

	/*** list header must be deleted by user ***/
	/* free(headList); */

	cg_log_debug_l5("Leaving...\n");
}
