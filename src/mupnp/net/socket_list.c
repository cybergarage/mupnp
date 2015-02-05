/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: csocket_list.c
*
*	Revision:
*
*	07/20/05
*		- first revision
*
******************************************************************/

#include <mupnp/net/socket.h>
#include <mupnp/util/log.h>

#if defined(CG_NET_USE_SOCKET_LIST)

/****************************************
* mupnp_socketlist_new
****************************************/

CgSocketList *mupnp_socketlist_new()
{
	mupnp_log_debug_l4("Entering...\n");

	CgSocketList *socketList = (CgSocketList *)malloc(sizeof(CgSocketList));

	if ( NULL != socketList )
		mupnp_list_header_init((CgList *)socketList);

	return socketList;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socketlist_delete
****************************************/

void mupnp_socketlist_delete(CgSocketList *socketList)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_socketlist_clear(socketList);
	free(socketList);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif
