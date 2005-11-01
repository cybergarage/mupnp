/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: chttp_server.cpp
*
*	Revision:
*
*	02/07/05
*		- first revision
*	10/30/05
*		- Thanks for Makela Aapo <aapo.makela@nokia.com>
*		- Fixed a memory leak problem in the http client thread.
*
******************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/cthread.h>
#include <cybergarage/util/cdebug.h>

#if defined(HAVE_UNAME)
#include <sys/utsname.h>
#endif

/****************************************
* cg_http_server_new
****************************************/

CgHttpServer *cg_http_server_new()
{
	CgHttpServer *httpServer = (CgHttpServer *)malloc(sizeof(CgHttpServer));
	
	cg_list_node_init((CgList *)httpServer);
	
	httpServer->sock = NULL;
	httpServer->acceptThread = NULL;
	httpServer->listener = NULL;

	/**** Thanks for Makela Aapo (10/31/05) ****/
	httpServer->clientThreads = NULL;

	cg_http_server_setuserdata(httpServer, NULL);
		
	return httpServer;
}

/****************************************
* cg_http_server_delete
****************************************/

void cg_http_server_delete(CgHttpServer *httpServer)
{
	cg_http_server_stop(httpServer);
	cg_http_server_close(httpServer);
	
	cg_list_remove((CgList *)httpServer);
	
	free(httpServer);
}

/****************************************
* cg_http_server_delete
****************************************/

BOOL cg_http_server_open(CgHttpServer *httpServer, int bindPort, char *bindAddr)
{
	if (cg_http_server_isopened(httpServer) == TRUE)
		return FALSE;
	
	httpServer->sock = cg_socket_stream_new();
	if (cg_socket_bind(httpServer->sock, bindPort, bindAddr, TRUE, FALSE) == FALSE) {
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
		return FALSE;
	}
	if (cg_socket_listen(httpServer->sock) == FALSE) {
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
		return FALSE;
	}
	return TRUE;
}

/****************************************
* cg_http_server_delete
****************************************/

BOOL cg_http_server_close(CgHttpServer *httpServer)
{
	cg_http_server_stop(httpServer);
	
	if (httpServer->sock != NULL) {
		cg_socket_close(httpServer->sock);
		cg_socket_delete(httpServer->sock);
		httpServer->sock = NULL;
	}
	
	return TRUE;
}

/****************************************
* cg_http_server_thread
****************************************/

typedef struct _CgHttpServerClientData {
	CgSocket *clientSock;
	CgHttpServer *httpServer;
} CgHttpServerClientData;

static CgHttpServerClientData *cg_http_server_clientdata_new(CgHttpServer *httpServer, CgSocket *clientSock)
{
	CgHttpServerClientData *clientData = (CgHttpServerClientData *)malloc(sizeof(CgHttpServerClientData));
	clientData->httpServer = httpServer;
	clientData->clientSock = clientSock;
	return clientData;
}

static void cg_http_server_clientdata_delete(CgHttpServerClientData *clientData)
{
	free(clientData);
}

static void cg_http_server_clientthread(CgThread *thread)
{
	CgHttpServerClientData *clientData;
	CgHttpServer *httpServer;
	void *httpServerUserData;
	CgSocket *clientSock;
	CgHttpRequest *httpReq;
	
	clientData = (CgHttpServerClientData *)cg_thread_getuserdata(thread);
	httpServer = clientData->httpServer;
	httpServerUserData = cg_http_server_getuserdata(httpServer);
	clientSock = clientData->clientSock;
	
	httpReq = cg_http_request_new();
	/**** Thanks for Makela Aapo (10/31/05) ****/
	while (cg_http_request_read(httpReq, clientSock) == TRUE && cg_thread_isrunnable(thread) == TRUE) {
		if (cg_debug_ison() == TRUE)
			cg_http_request_print(httpReq);
		if (httpServer->listener != NULL) {
			cg_http_request_setsocket(httpReq, clientSock);
			cg_http_request_setuserdata(httpReq, httpServerUserData);
			httpServer->listener(httpReq);
		}

		/* TODO: HTTP 1.1 persistent connections */
		/*if (cg_http_request_iskeepaliveconnection(httpReq) == FALSE) */
		break;
	}
	cg_http_server_clientdata_delete(clientData);
	cg_http_request_delete(httpReq);
	cg_socket_close(clientSock);
	cg_socket_delete(clientSock);

	cg_thread_delete(thread);
}

/****************************************
* cg_http_server_thread
****************************************/

static void cg_http_server_thread(CgThread *thread)
{
	CgHttpServer *httpServer;
	CgThread *httpClientThread;
	CgHttpServerClientData *clientData;
	CgSocket *serverSock;
	CgSocket *clientSock;
			 
	httpServer = (CgHttpServer *)cg_thread_getuserdata(thread);
	
	if (cg_http_server_isopened(httpServer) == FALSE)
		return;
		
	serverSock = httpServer->sock;
	while (cg_thread_isrunnable(thread) == TRUE) {
		clientSock = cg_socket_stream_new();
		if (cg_socket_accept(serverSock, clientSock) == FALSE) {
			cg_socket_delete(clientSock);
			break;
		}
		clientData = cg_http_server_clientdata_new(httpServer, clientSock);
		httpClientThread = cg_thread_new();
		cg_thread_setaction(httpClientThread, cg_http_server_clientthread);
		cg_thread_setuserdata(httpClientThread, clientData);
		
		/**** Thanks for Makela Aapo (10/31/05) ****/
		cg_threadlist_add(httpServer->clientThreads, httpClientThread);
		
		cg_thread_start(httpClientThread);		
	}
}

/****************************************
* cg_http_server_start
****************************************/

BOOL cg_http_server_start(CgHttpServer *httpServer)
{
	if (httpServer->acceptThread != NULL)
		return FALSE;
	
	httpServer->acceptThread = cg_thread_new();
	cg_thread_setaction(httpServer->acceptThread, cg_http_server_thread);
	cg_thread_setuserdata(httpServer->acceptThread, httpServer);

	/**** Thanks for Makela Aapo (10/31/05) ****/
	httpServer->clientThreads = cg_threadlist_new();

	if (cg_thread_start(httpServer->acceptThread) == FALSE) {
		cg_thread_delete(httpServer->acceptThread);
		httpServer->acceptThread = NULL;

		/**** Thanks for Makela Aapo (10/31/05) ****/
		cg_threadlist_delete(httpServer->clientThreads);
		httpServer->clientThreads = NULL;

		return FALSE;
	}

	return TRUE;
}

/****************************************
* cg_http_server_stop
****************************************/

BOOL cg_http_server_stop(CgHttpServer *httpServer)
{
	if (httpServer->acceptThread != NULL) {
		cg_thread_stop(httpServer->acceptThread);
		cg_thread_delete(httpServer->acceptThread);
		httpServer->acceptThread = NULL;
	}
	/**** Thanks for Makela Aapo (10/31/05) ****/
	if (httpServer->clientThreads != NULL) {
		cg_threadlist_stop(httpServer->clientThreads);
		cg_threadlist_delete(httpServer->clientThreads);
		httpServer->clientThreads = NULL;
	}
	return TRUE;
}

/****************************************
* cg_http_server_setlistener
****************************************/

void cg_http_server_setlistener(CgHttpServer *httpServer, CG_HTTP_LISTENER listener)
{
	httpServer->listener = listener;
}

/****************************************
* cg_http_getservername()
****************************************/

char *cg_http_getservername(char *buf, int bufSize)
{
#if defined(WIN32) && !defined(ITRON)
	OSVERSIONINFO verInfo;
	cg_strcpy(buf, "Platform 1.0");
	ZeroMemory(&verInfo, sizeof(OSVERSIONINFO));
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&verInfo)) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "Windows %d.%d", verInfo.dwMajorVersion, verInfo.dwMinorVersion);
		#else
		sprintf(buf, "Windows %d.%d", verInfo.dwMajorVersion, verInfo.dwMinorVersion);
		#endif
	}
#elif defined(BTRON)
	T_VER verInfo;
	cg_strcpy(buf, "Platform 1.0");
	if (get_ver(&verInfo) == 0) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "BTRON %hd", verInfo.spver);
		#else
		sprintf(buf, "BTRON %hd", verInfo.spver);
		#endif
	}
#elif defined(ITRON)
	cg_strcpy(buf, "uITRON 4.0");
#elif defined(TENGINE) 
	cg_strcpy(buf, "T-Engine 1.0");
#elif defined(HAVE_UNAME)
	struct utsname unameBuf;
	cg_strcpy(buf, "Platform 1.0");
	if (uname(&unameBuf) == 0) {
		#if defined(HAVE_SNPRINTF)
		snprintf(buf, bufSize, "%s %s", unameBuf.sysname, unameBuf.release);
		#else
		sprintf(buf, "%s %s", unameBuf.sysname, unameBuf.release);
		#endif
	}
#else
	cg_strcpy(buf, "Platform 1.0");
#endif

	return buf;
}


