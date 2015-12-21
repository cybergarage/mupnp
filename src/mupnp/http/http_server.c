/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/http/http.h>
#include <mupnp/util/thread.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>

#if defined(HAVE_UNAME) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
#include <sys/utsname.h>
#endif

/****************************************
* mupnp_http_server_new
****************************************/

mUpnpHttpServer* mupnp_http_server_new()
{
  mUpnpHttpServer* httpServer;

  mupnp_log_debug_l4("Entering...\n");

  httpServer = (mUpnpHttpServer*)malloc(sizeof(mUpnpHttpServer));

  if (NULL != httpServer) {
    mupnp_list_node_init((mUpnpList*)httpServer);

    httpServer->sock = NULL;
    httpServer->acceptThread = NULL;
    httpServer->listener = NULL;

    /**** Thanks for Makela Aapo (10/31/05) ****/
    httpServer->clientThreads = NULL;

    mupnp_http_server_setuserdata(httpServer, NULL);

    mupnp_http_server_settimeout(httpServer, MUPNP_HTTP_SERVER_READ_TIMEOUT);

    /* Mutex */
    httpServer->mutex = mupnp_mutex_new();
  }

  mupnp_log_debug_l4("Leaving...\n");

  return httpServer;
}

/****************************************
* mupnp_http_server_delete
****************************************/

void mupnp_http_server_delete(mUpnpHttpServer* httpServer)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_server_stop(httpServer);
  mupnp_http_server_close(httpServer);

  if (httpServer->mutex)
    mupnp_mutex_delete(httpServer->mutex);

  mupnp_list_remove((mUpnpList*)httpServer);

  free(httpServer);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_server_delete
****************************************/

bool mupnp_http_server_open(mUpnpHttpServer* httpServer, int bindPort, const char* bindAddr)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_http_server_isopened(httpServer) == true)
    return false;

  httpServer->sock = mupnp_socket_stream_new();
  if (mupnp_socket_bind(httpServer->sock, bindPort, bindAddr, true, false) == false) {
    mupnp_socket_delete(httpServer->sock);
    httpServer->sock = NULL;
    return false;
  }
  if (mupnp_socket_listen(httpServer->sock) == false) {
    mupnp_socket_delete(httpServer->sock);
    httpServer->sock = NULL;
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_server_delete
****************************************/

bool mupnp_http_server_close(mUpnpHttpServer* httpServer)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_server_stop(httpServer);

  if (httpServer->sock != NULL) {
    mupnp_socket_close(httpServer->sock);
    mupnp_socket_delete(httpServer->sock);
    httpServer->sock = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_server_thread
****************************************/

typedef struct _mUpnpHttpServerClientData {
  mUpnpSocket* clientSock;
  mUpnpHttpServer* httpServer;
} mUpnpHttpServerClientData;

static mUpnpHttpServerClientData* mupnp_http_server_clientdata_new(mUpnpHttpServer* httpServer, mUpnpSocket* clientSock)
{
  mUpnpHttpServerClientData* clientData;

  mupnp_log_debug_l4("Entering...\n");

  clientData = (mUpnpHttpServerClientData*)malloc(sizeof(mUpnpHttpServerClientData));

  if (NULL != clientData) {
    clientData->httpServer = httpServer;
    clientData->clientSock = clientSock;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return clientData;
}

static void mupnp_http_server_clientdata_delete(mUpnpHttpServerClientData* clientData)
{
  mupnp_log_debug_l4("Entering...\n");

  free(clientData);

  mupnp_log_debug_l4("Leaving...\n");
}

static void mupnp_http_server_clientthread(mUpnpThread* thread)
{
  mUpnpHttpServerClientData* clientData;
  mUpnpHttpServer* httpServer;
  mUpnpSocket* clientSock;
  void* httpServerUserData;
  mUpnpHttpRequest* httpReq;
  char* version = NULL;

  mupnp_log_debug_l4("Entering...\n");

  clientData = (mUpnpHttpServerClientData*)mupnp_thread_getuserdata(thread);
  httpServer = clientData->httpServer;
  clientSock = clientData->clientSock;
  httpServerUserData = mupnp_http_server_getuserdata(httpServer);

  httpReq = mupnp_http_request_new();
  mupnp_http_request_setsocket(httpReq, clientSock);

  /**** Thanks for Makela Aapo (10/31/05) ****/
  while (mupnp_http_request_read(httpReq, clientSock) == true && mupnp_thread_isrunnable(thread) == true) {
    /* Check some validity of the request */
    version = mupnp_http_request_getversion(httpReq);
    if (mupnp_strcmp(version, MUPNP_HTTP_VER11) == 0) {
      /* According to HTTP/1.1 spec, we must not tolerate
			   HTTP/1.1 request without HOST-header */
      if (mupnp_http_request_gethost(httpReq) == NULL) {
        mupnp_http_request_postbadrequest(httpReq);
        continue;
      }
    }

    if (httpServer->listener != NULL) {
      mupnp_http_request_setuserdata(httpReq, httpServerUserData);
      httpServer->listener(httpReq);
    }

    /* Close connection according to HTTP version and headers */
    if (mupnp_strcmp(version, MUPNP_HTTP_VER10) == 0) {
      /* Terminate connection after HTTP/1.0 request */
      break;
    }

    /* We are having HTTP/1.1 or better => terminate, if requested */
    if (mupnp_http_request_iskeepaliveconnection(httpReq) == false) {
      break;
    }
  }

  mupnp_log_debug_s("Dropping HTTP client\n");
  mupnp_http_request_delete(httpReq);

  mupnp_socket_close(clientSock);
  mupnp_socket_delete(clientSock);

  mupnp_http_server_clientdata_delete(clientData);
  mupnp_thread_setuserdata(thread, NULL);

  // This code frequently crashes. mutex lock referencing free'd memory.
  mupnp_http_server_lock(httpServer);
  mupnp_thread_remove(thread);
  mupnp_http_server_unlock(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

  mupnp_thread_delete(thread);
}

/****************************************
* mupnp_http_server_thread
****************************************/

static void mupnp_http_server_thread(mUpnpThread* thread)
{
  mUpnpHttpServer* httpServer;
  mUpnpThread* httpClientThread;
  mUpnpHttpServerClientData* clientData;
  mUpnpSocket* serverSock;
  mUpnpSocket* clientSock;

  mupnp_log_debug_l4("Entering...\n");

  httpServer = (mUpnpHttpServer*)mupnp_thread_getuserdata(thread);

  if (mupnp_http_server_isopened(httpServer) == false)
    return;

  serverSock = httpServer->sock;
  while (mupnp_thread_isrunnable(thread) == true) {
    clientSock = mupnp_socket_stream_new();
    if (mupnp_socket_accept(serverSock, clientSock) == false) {
      mupnp_socket_delete(clientSock);
      break;
    }

    mupnp_socket_settimeout(clientSock, mupnp_http_server_gettimeout(httpServer));
    clientData = mupnp_http_server_clientdata_new(httpServer, clientSock);
    httpClientThread = mupnp_thread_new();
    mupnp_thread_setaction(httpClientThread, mupnp_http_server_clientthread);
    mupnp_thread_setuserdata(httpClientThread, clientData);

    /**** Thanks for Makela Aapo (10/31/05) ****/
    mupnp_http_server_lock(httpServer);
    mupnp_threadlist_add(httpServer->clientThreads, httpClientThread);
    mupnp_http_server_unlock(httpServer);

    mupnp_thread_start(httpClientThread);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_server_start
****************************************/

bool mupnp_http_server_start(mUpnpHttpServer* httpServer)
{
  mupnp_log_debug_l4("Entering...\n");

  if (httpServer->acceptThread != NULL)
    return false;

  httpServer->acceptThread = mupnp_thread_new();
  mupnp_thread_setaction(httpServer->acceptThread, mupnp_http_server_thread);
  mupnp_thread_setuserdata(httpServer->acceptThread, httpServer);

  /**** Thanks for Makela Aapo (10/31/05) ****/
  httpServer->clientThreads = mupnp_threadlist_new();

  if (mupnp_thread_start(httpServer->acceptThread) == false) {
    mupnp_thread_delete(httpServer->acceptThread);
    httpServer->acceptThread = NULL;

    /**** Thanks for Makela Aapo (10/31/05) ****/
    mupnp_threadlist_delete(httpServer->clientThreads);
    httpServer->clientThreads = NULL;

    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_server_stop
****************************************/

bool mupnp_http_server_stop(mUpnpHttpServer* httpServer)
{
  mupnp_log_debug_l4("Entering...\n");

  if (httpServer->acceptThread != NULL) {
    mupnp_thread_stop(httpServer->acceptThread);
    mupnp_thread_delete(httpServer->acceptThread);
    httpServer->acceptThread = NULL;
  }
  /**** Thanks for Makela Aapo (10/31/05) ****/
  if (httpServer->clientThreads != NULL) {
    mupnp_threadlist_stop(httpServer->clientThreads);
    mupnp_threadlist_delete(httpServer->clientThreads);
    httpServer->clientThreads = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_server_setlistener
****************************************/

void mupnp_http_server_setlistener(mUpnpHttpServer* httpServer, MUPNP_HTTP_LISTENER listener)
{
  mupnp_log_debug_l4("Entering...\n");

  httpServer->listener = listener;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_getservername()
****************************************/

const char* mupnp_http_getservername(char* buf, size_t bufSize)
{
#if defined(WIN32) && !defined(ITRON)
  OSVERSIONINFO verInfo;
  mupnp_strcpy(buf, "Platform 1.0");
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
  mupnp_strcpy(buf, "Platform 1.0");
  if (get_ver(&verInfo) == 0) {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "BTRON %hd", verInfo.spver);
#else
    sprintf(buf, "BTRON %hd", verInfo.spver);
#endif
  }
#elif defined(ITRON)
  mupnp_strcpy(buf, "uITRON 4.0");
#elif defined(TENGINE)
  mupnp_strcpy(buf, "T-Engine 1.0");
#elif defined(HAVE_UNAME) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
  struct utsname unameBuf;
  mupnp_strcpy(buf, "Platform 1.0");
  if (uname(&unameBuf) == 0) {
#if defined(HAVE_SNPRINTF)
    snprintf(buf, bufSize, "%s %s", unameBuf.sysname, unameBuf.release);
#else
    sprintf(buf, "%s %s", unameBuf.sysname, unameBuf.release);
#endif
  }
#else
  mupnp_strcpy(buf, "Platform 1.0");
#endif

  mupnp_log_debug_l4("Entering...\n");

  return buf;

  mupnp_log_debug_l4("Leaving...\n");
}
