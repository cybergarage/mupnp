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

#include <mupnp/util/list.h>
#include <mupnp/util/time.h>
#include <mupnp/util/mutex.h>
#include <mupnp/http/http.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/log.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef MUPNP_HTTP_CURL
#include <curl/curl.h>
#endif

#define MUPNP_HTTP_PERSISTENT_CACHE_SIZE 5
#define MUPNP_HTTP_PERSISTENT_TIMEOUT_PERIOD 60

typedef struct _mUpnpHttpPersistentConnection {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* host;
  int port;
  void* cacheData;

  mUpnpTime timestamp;

} mUpnpHttpPersistentConnection, mUpnpHttpPersistentConnectionList;

static mUpnpHttpPersistentConnectionList* cache = NULL;
static mUpnpMutex* persistent_connection_mutex = NULL;

/****************************************
* mupnp_http_persistentconnection_init
****************************************/

bool mupnp_http_persistentconnection_init(void)
{
  mupnp_log_debug_l4("Entering...\n");

  if (cache == NULL) {
    persistent_connection_mutex = mupnp_mutex_new();

    if (persistent_connection_mutex == NULL)
      return false;

    mupnp_http_persistentconnection_lock();
    cache = (mUpnpHttpPersistentConnectionList*)malloc(sizeof(mUpnpHttpPersistentConnectionList));

    if (cache == NULL) {
      if (persistent_connection_mutex != NULL) {
        mupnp_http_persistentconnection_unlock();
        mupnp_mutex_delete(persistent_connection_mutex);
        persistent_connection_mutex = NULL;
      }
      return false;
    }

    mupnp_list_header_init((mUpnpList*)cache);

    cache->host = NULL;
    cache->port = 0;
    cache->cacheData = NULL;

    cache->timestamp = 0;

    mupnp_http_persistentconnection_unlock();
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_persistentconnection_new
****************************************/

mUpnpHttpPersistentConnection* mupnp_http_persistentconnection_new(void)
{
  mUpnpHttpPersistentConnection* new_node;

  mupnp_log_debug_l4("Entering...\n");

  new_node = (mUpnpHttpPersistentConnection*)malloc(sizeof(mUpnpHttpPersistentConnection));

  if (NULL != new_node) {
    new_node->headFlag = false;
    new_node->next = NULL;
    new_node->prev = NULL;

    new_node->host = mupnp_string_new();
    new_node->port = 0;
    new_node->cacheData = NULL;

    new_node->timestamp = 0;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return new_node;
}

/****************************************
* mupnp_http_persistentconnection_delete
****************************************/

void mupnp_http_persistentconnection_delete(mUpnpHttpPersistentConnection* node)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_delete(node->host);

/* Terminate and delete connection according libcurl usage */
#if !defined(MUPNP_HTTP_CURL)
  mupnp_socket_close((mUpnpSocket*)node->cacheData);
  mupnp_socket_delete((mUpnpSocket*)node->cacheData);
#else
  curl_easy_cleanup((CURL*)node->cacheData);
#endif
  free(node);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_persistentconnection_get
****************************************/

void* mupnp_http_persistentconnection_get(char* host, int port)
{
  mUpnpHttpPersistentConnection* node;
  mUpnpTime sys_time = mupnp_getcurrentsystemtime();
  bool iterate;

  mupnp_log_debug_l4("Entering...\n");

  /* If we dont have cache, then just exit */
  if (cache == NULL) {
    mupnp_log_debug("(get) No cache! Persistent connections not initialized?\n");
    return NULL;
  }

  /* Clear all expired nodes */
  do {
    iterate = false;
    for (node = (mUpnpHttpPersistentConnection*)mupnp_list_gets((mUpnpList*)cache);
         node != NULL;
         node = (mUpnpHttpPersistentConnection*)mupnp_list_next((mUpnpList*)node)) {
      if (sys_time > node->timestamp + MUPNP_HTTP_PERSISTENT_TIMEOUT_PERIOD) {
        mupnp_log_debug_s("Timeout for persistent HTTP Connection to %s:%d "
                          "(timestamp: %d)\n",
            mupnp_string_getvalue(node->host),
            node->port,
            node->timestamp);
        mupnp_list_remove((mUpnpList*)node);
        mupnp_http_persistentconnection_delete(node);
        iterate = true;
        break;
      }
    }
  } while (iterate);

  /* Get persistent node */
  for (node = (mUpnpHttpPersistentConnection*)mupnp_list_gets((mUpnpList*)cache);
       node != NULL;
       node = (mUpnpHttpPersistentConnection*)mupnp_list_next((mUpnpList*)node)) {
    if (mupnp_strcmp(mupnp_string_getvalue(node->host), host) == 0 && node->port == port) {
      /* Node was required, remove and add again to refresh cache */
      mupnp_list_remove((mUpnpList*)node);
      mupnp_list_add((mUpnpList*)cache, (mUpnpList*)node);

      node->timestamp = mupnp_getcurrentsystemtime();

      mupnp_log_debug_s("Persistent HTTP Connection cache HIT for %s:%d\n", host, port);

      return node->cacheData;
    }
  }

  mupnp_log_debug_s("Persistent HTTP Connection cache MISS for %s:%d\n", host, port);

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_http_persistentconnection_put
****************************************/

bool mupnp_http_persistentconnection_put(char* host, int port, void* data)
{
  mUpnpHttpPersistentConnection *new_node = NULL, *node = NULL;

  mupnp_log_debug_l4("Entering...\n");

  /* If we dont have cache, then just exit */
  if (cache == NULL) {
    mupnp_log_debug("(put) No cache! Persistent connections not initialized?\n");
    return false;
  }

  /* Check if we already have this one cached */
  for (node = (mUpnpHttpPersistentConnection*)mupnp_list_gets((mUpnpList*)cache);
       node != NULL;
       node = (mUpnpHttpPersistentConnection*)mupnp_list_next((mUpnpList*)node)) {
    if (mupnp_strcmp(mupnp_string_getvalue(node->host), host) == 0 && node->port == port) {
      /* If also data is the same, then update just
       timestamp */
      if (node->cacheData == data) {
        node->timestamp = mupnp_getcurrentsystemtime();
        return true;
      }

      mupnp_log_debug_s("Found cached persistent connection for %s:%d\n", mupnp_string_getvalue(node->host), node->port);
      new_node = node;
      mupnp_list_remove((mUpnpList*)new_node);
      break;
    }
  }

  /* We didn't find it */
  if (new_node == NULL) {
    /* Check if we have already too many cached things */
    if (mupnp_list_size((mUpnpList*)cache) >= MUPNP_HTTP_PERSISTENT_CACHE_SIZE) {
      /* Take last node (not refreshed for a long time) */
      new_node = (mUpnpHttpPersistentConnection*)mupnp_list_next((mUpnpList*)cache);
      mupnp_list_remove((mUpnpList*)new_node);
      mupnp_http_persistentconnection_delete(new_node);
      new_node = NULL;

      mupnp_log_debug_s("Max persistent HTTP connection cache reached.\n");
    }

    if (new_node == NULL) {
      if (data == NULL)
        return true;

      new_node = mupnp_http_persistentconnection_new();
      if (new_node == NULL)
        return false;

      mupnp_log_debug_s("Adding persistent HTTP Connection %s:%d to cache\n", host, port);
      mupnp_log_debug_s("Persistent connections: %d\n", mupnp_list_size((mUpnpList*)cache));
    }
  }

  if (data != NULL) {
    /* Set appropriate values for the node */
    mupnp_string_setvalue(new_node->host, host);
    new_node->port = port;
    new_node->cacheData = data;
    new_node->timestamp = mupnp_getcurrentsystemtime();

    mupnp_list_add((mUpnpList*)cache, (mUpnpList*)new_node);
  }
  else {
    /* remove and delete node */
    mupnp_http_persistentconnection_delete(new_node);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_http_persistentconnection_clear
****************************************/

void mupnp_http_persistentconnection_clear(void)
{
  mupnp_log_debug_l4("Entering...\n");

  /* If we dont have cache, then just exit */
  if (cache == NULL)
    return;

  mupnp_http_persistentconnection_lock();
  mupnp_list_clear((mUpnpList*)cache, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_http_persistentconnection_delete);
  free(cache);
  cache = NULL;
  mupnp_http_persistentconnection_unlock();

  mupnp_mutex_delete(persistent_connection_mutex);
  persistent_connection_mutex = NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_persistentconnection_lock
****************************************/

void mupnp_http_persistentconnection_lock(void)
{
  mupnp_log_debug_l4("Entering...\n");

  if (persistent_connection_mutex == NULL)
    return;

  mupnp_mutex_lock(persistent_connection_mutex);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_persistentconnection_unlock
****************************************/

void mupnp_http_persistentconnection_unlock(void)
{
  mupnp_log_debug_l4("Entering...\n");

  if (persistent_connection_mutex == NULL)
    return;

  mupnp_mutex_unlock(persistent_connection_mutex);

  mupnp_log_debug_l4("Leaving...\n");
}
