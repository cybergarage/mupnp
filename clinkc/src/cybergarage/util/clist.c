/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
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
******************************************************************/

#include <cybergarage/util/clist.h>

/****************************************
* cg_list_header_init
****************************************/

void cg_list_header_init(CgList *list)
{
	if (list == NULL)
		return;

	list->headFlag = TRUE;			
	list->prev = list->next = list;
}

/****************************************
* cg_list_node_init
****************************************/

void cg_list_node_init(CgList *list)
{
	if (list == NULL)
		return;

	list->headFlag = FALSE;			
	list->prev = list->next = list;
}

/****************************************
* cg_list_size
****************************************/

int cg_list_size(CgList *headList)
{
	CgList *list;
	int listCnt;
	
	if (headList == NULL)
		return 0;

	listCnt = 0;
	for (list = cg_list_next(headList); list != NULL; list = cg_list_next(list))
		listCnt++;
	
	return listCnt;
}

/****************************************
* cg_list_get
****************************************/

CgList *cg_list_get(CgList *headList, int index)
{
	CgList *list;
	int n;
	
	if (headList == NULL)
		return NULL;

	list = cg_list_next(headList);
	for (n=0; n<index; n++) {
		if (list == NULL)
			break;
		list = cg_list_next(list);
	}
		
	return list;
}

/****************************************
* cg_list_insert
****************************************/

void cg_list_insert(
CgList *prevList,
CgList *list)
{
	if (prevList == NULL || list == NULL)
		return;

	list->prev = prevList;
	list->next = prevList->next;
	prevList->next->prev = list;
	prevList->next = list;
}

/****************************************
* cg_list_add
****************************************/

void cg_list_add(
CgList *headList,
CgList *list)
{
	if (headList == NULL || list == NULL)
		return;

	cg_list_insert(headList->prev, list);
}

/****************************************
* cg_list_remove
****************************************/

void cg_list_remove(CgList *list)
{
	if (list == NULL)
		return;

	list->prev->next = list->next;
	list->next->prev = list->prev;
	list->prev = list->next = list;
}

/****************************************
* cg_list_prev_circular
****************************************/

CgList *cg_list_prev_circular (
CgList *list)
{
	if (list == NULL)
		return NULL;

	if (list->prev->headFlag)
		list = list->prev;
	return list->prev;
}

/****************************************
* cg_list_prev
****************************************/

CgList *cg_list_prev(
CgList *list)
{
	if (list == NULL)
		return NULL;

	if (list->prev->headFlag == TRUE)
		return NULL;
	return list->prev;
}

/****************************************
* cg_list_next_circular
****************************************/

CgList *cg_list_next_circular(
CgList *list)
{
	if (list == NULL)
		return NULL;

	if (list->next->headFlag == TRUE)
		list = list->next;
	return list->next;
}

/****************************************
* cg_list_next
****************************************/

CgList *cg_list_next(
CgList *list)
{
	if (list == NULL)
		return NULL;

	if (list->next->headFlag == TRUE)
		return NULL;
	return list->next;
}

/****************************************
* cg_list_clear
****************************************/

void cg_list_clear(CgList *headList, CG_LIST_DESTRUCTORFUNC destructorFunc)
{
	CgList *list;

	if (headList == NULL)
		return;

	list = cg_list_next(headList);
	while(list != NULL) {
		cg_list_remove(list);
		destructorFunc(list);
		list = cg_list_next(headList);
	}

	/*** list header must be deleted by user ***/
	/* free(headList); */
}
