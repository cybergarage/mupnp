/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: clist.h
*
*	Revision:
*
*	01/18/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UTIL_CLIST_H_
#define _CG_UTIL_CLIST_H_

#include <cybergarage/typedef.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

typedef void (*CG_LIST_DESTRUCTORFUNC)(void *);

/****************************************
* Data Type
****************************************/

typedef struct _CgList {
	BOOL headFlag;
	struct _CgList *prev;
	struct _CgList *next;
} CgList;

/****************************************
* Function
****************************************/

void cg_list_header_init(CgList *list);
void cg_list_node_init(CgList *list);
void cg_list_insert(CgList *prevList, CgList *list);
void cg_list_add(CgList *headList, CgList *list);
void cg_list_remove(CgList *list);
int cg_list_size(CgList *headList);
CgList *cg_list_get(CgList *headList, int index);

CgList *cg_list_prev_circular(CgList *list);
CgList *cg_list_prev(CgList *list);
CgList *cg_list_next_circular(CgList *list);
CgList *cg_list_next(CgList *list);

void cg_list_clear(CgList *headList, CG_LIST_DESTRUCTORFUNC destructorFunc);

#define cg_list_gets(headList) cg_list_next(headList)

#ifdef  __cplusplus
}
#endif

#endif
