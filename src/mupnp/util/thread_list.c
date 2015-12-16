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

#include <mupnp/util/thread.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_threadlist_new
****************************************/

mUpnpThreadList* mupnp_threadlist_new()
{
  mUpnpThreadList* threadList;

  mupnp_log_debug_l4("Entering...\n");

  threadList = (mUpnpThreadList*)malloc(sizeof(mUpnpThreadList));

  if (NULL != threadList) {
    mupnp_list_header_init((mUpnpList*)threadList);
    threadList->runnableFlag = false;
    threadList->action = NULL;
    threadList->userData = NULL;
  }

  return threadList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_delete
****************************************/

void mupnp_threadlist_delete(mUpnpThreadList* threadList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_threadlist_clear(threadList);
  free(threadList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_start
****************************************/

bool mupnp_threadlist_start(mUpnpThreadList* threadList)
{
  mUpnpThreadList* thread;

  mupnp_log_debug_l4("Entering...\n");

  for (thread = mupnp_threadlist_gets(threadList); thread != NULL; thread = mupnp_thread_next(thread))
    mupnp_thread_start(thread);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_threadlist_stop
****************************************/

bool mupnp_threadlist_stop(mUpnpThreadList* threadList)
{
  mUpnpThreadList* thread;

  mupnp_log_debug_l4("Entering...\n");

  for (thread = mupnp_threadlist_gets(threadList); thread != NULL; thread = mupnp_thread_next(thread))
    mupnp_thread_stop(thread);

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}
