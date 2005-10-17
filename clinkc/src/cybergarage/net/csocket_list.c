/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: csocket_list.c
*
*	Revision:
*
*	07/20/05
*		- first revision
*
******************************************************************/

#include <cybergarage/net/csocket.h>

#if defined(CG_NET_USE_SOCKET_LIST)

/****************************************
* cg_socketlist_new
****************************************/

CgSocketList *cg_socketlist_new()
{
	CgSocketList *socketList = (CgSocketList *)malloc(sizeof(CgSocketList));
	cg_list_header_init((CgList *)socketList);
	return socketList;
}

/****************************************
* cg_socketlist_delete
****************************************/

void cg_socketlist_delete(CgSocketList *socketList)
{
	cg_socketlist_clear(socketList);
	free(socketList);
}

#endif
